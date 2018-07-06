#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include <windows.h>

#include "merc.h"
#include "merc-win.rh"

/*
 * Global variables.
 */
DESCRIPTOR_DATA *   descriptor_free;	/* Free list for descriptors	*/
DESCRIPTOR_DATA *   descriptor_list;	/* All open descriptors		*/
DESCRIPTOR_DATA *   d_next;		/* Next descriptor in loop	*/
FILE *		    fpReserve;		/* Reserved file handle		*/
bool		    god;		/* All new chars are gods!	*/
bool		    merc_down;		/* Shutdown			*/
bool		    wizlock;		/* Game is wizlocked		*/
char		    str_boot_time[MAX_INPUT_LENGTH];
time_t		    current_time;	/* Time of this pulse		*/

/*
 * Other local functions (OS-independent).
 */
bool	check_parse_name	args( ( char *name ) );
bool	check_reconnect		args( ( DESCRIPTOR_DATA *d, char *name,
				    bool fConn ) );
bool	check_playing		args( ( DESCRIPTOR_DATA *d, char *name ) );
int	main			args( ( int argc, char **argv ) );
void	nanny			args( ( DESCRIPTOR_DATA *d, char *argument ) );
bool	process_output		args( ( DESCRIPTOR_DATA *d, bool fPrompt ) );
void	read_from_buffer	args( ( DESCRIPTOR_DATA *d ) );
void	stop_idling		args( ( CHAR_DATA *ch ) );
void    bust_a_prompt           args( ( CHAR_DATA *ch ) );

// @@@ My junk
char      UserMessage[512];
BOOL      fUserReady;
//#define crypt(d,s) strcpy(d,s)
const	char	echo_off_str	[] = { '\0' };
const	char	echo_on_str	[] = { '\0' };
const	char 	go_ahead_str	[] = { '\0' };
bool	write_to_descriptor	args( ( int desc, char *txt, int length ) );
void	send_to_char	args( ( const char *txt, CHAR_DATA *ch ) )
{
    if ( txt == NULL || ch->desc == NULL )
        return;
    ch->desc->showstr_head = alloc_mem( strlen( txt ) + 1 );
    strcpy( ch->desc->showstr_head, txt );
    ch->desc->showstr_point = ch->desc->showstr_head;
    show_string( ch->desc, "" );
}

void gettimeofday( struct timeval *tp, void *tzp )
{
    tp->tv_sec  = time( NULL );
    tp->tv_usec = 0;
}

bool read_from_descriptor( DESCRIPTOR_DATA *d )
{
    int iStart;

    /* Hold horses if pending command already. */
    if ( d->incomm[0] != '\0' )
	return TRUE;

    /* Check for overflow. */
    iStart = strlen(d->inbuf);
    if ( iStart >= sizeof(d->inbuf) - 10 )
    {
	sprintf( log_buf, "%s input overflow!", d->host );
	log_string( log_buf );
	write_to_descriptor( d->descriptor,
	    "\n\r*** PUT A LID ON IT!!! ***\n\r", 0 );
	return FALSE;
    }

#if 0
    /* Snarf input. */
    for ( ; ; )
    {
	int c;
	c = getc( stdin );
	if ( c == '\0' || c == EOF )
	    break;
	putc( c, stdout );
	if ( c == '\r' )
	    putc( '\n', stdout );
	d->inbuf[iStart++] = c;
	if ( iStart > sizeof(d->inbuf) - 10 )
	    break;
    }
#else
    if (fUserReady)
       {
       int nLen = strlen(UserMessage);
       fUserReady = FALSE;
       if (iStart + nLen <= sizeof d->inbuf - 10)
          {
          memcpy(d->inbuf + iStart, UserMessage, nLen);
          iStart += nLen;
          d->inbuf[iStart++] = '\r';
          }
       }
#endif

    d->inbuf[iStart] = '\0';
    return TRUE;
}



/*
 * Transfer one line from input buffer to input line.
 */
void read_from_buffer( DESCRIPTOR_DATA *d )
{
    int i, j, k;

    /*
     * Hold horses if pending command already.
     */
    if ( d->incomm[0] != '\0' )
	return;

    /*
     * Look for at least one new line.
     */
    for ( i = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++ )
    {
	if ( d->inbuf[i] == '\0' )
	    return;
    }

    /*
     * Canonical input processing.
     */
    for ( i = 0, k = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++ )
    {
	if ( k >= MAX_INPUT_LENGTH - 2 )
	{
	    write_to_descriptor( d->descriptor, "Line too long.\n\r", 0 );

	    /* skip the rest of the line */
	    for ( ; d->inbuf[i] != '\0'; i++ )
	    {
		if ( d->inbuf[i] == '\n' || d->inbuf[i] == '\r' )
		    break;
	    }
	    d->inbuf[i]   = '\n';
	    d->inbuf[i+1] = '\0';
	    break;
	}

	if ( d->inbuf[i] == '\b' && k > 0 )
	    --k;
	else if ( isascii(d->inbuf[i]) && isprint(d->inbuf[i]) )
	    d->incomm[k++] = d->inbuf[i];
    }

    /*
     * Finish off the line.
     */
    if ( k == 0 )
	d->incomm[k++] = ' ';
    d->incomm[k] = '\0';

    /*
     * Deal with bozos with #repeat 1000 ...
     */
    if ( k > 1 || d->incomm[0] == '!' )
    {
    	if ( d->incomm[0] != '!' && strcmp( d->incomm, d->inlast ) )
	{
	    d->repeat = 0;
	}
	else
	{
	    if ( ++d->repeat >= 20 )
	    {
		sprintf( log_buf, "%s input spamming!", d->host );
		log_string( log_buf );
		write_to_descriptor( d->descriptor,
		    "\n\r*** PUT A LID ON IT!!! ***\n\r", 0 );
		strcpy( d->incomm, "quit" );
	    }
	}
    }

    /*
     * Do '!' substitution.
     */
    if ( d->incomm[0] == '!' )
	strcpy( d->incomm, d->inlast );
    else
	strcpy( d->inlast, d->incomm );

    /*
     * Shift the input buffer.
     */
    while ( d->inbuf[i] == '\n' || d->inbuf[i] == '\r' )
	i++;
    for ( j = 0; ( d->inbuf[j] = d->inbuf[i+j] ) != '\0'; j++ )
	;
    return;
}

void CloseMerc(void)
{
}

HINSTANCE hInst;
HWND      hQryDlgBox;                         // handle of modeless dialog box
HWND      hWndMain;
HWND      hWndOutput;
char      UserMessage[512];
BOOL      fUserReady;

LRESULT CALLBACK _export MercWndProc(HWND hWnd, UINT message,
                             WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_MOVE:
            // Move the dialog box on top of our main window every
            // time the main window moves.
            if (IsWindow(hQryDlgBox))
                SendMessage(hQryDlgBox, message, wParam, lParam);
            break;

        case WM_SETFOCUS:
            // Always set the input focus to the dialog box.
            if (IsWindow(hQryDlgBox))
                SendMessage(hQryDlgBox, message, wParam, lParam);
            break;

        case WM_CLOSE:
            // Tell windows to destroy our window.
            DestroyWindow(hWnd);
            break;

        case WM_QUERYENDSESSION:
            // If we return TRUE we are saying it's ok with us to end the
				// windows session.
            return((long) TRUE);  // we agree to end session.

        case WM_ENDSESSION:
            // If wParam is not zero, it meany every application said ok
            // to WM_QUERYENDSESSION messages, so we are really ending.
            if (wParam)           // if all apps aggreed to end session.
                CloseMerc();     // This is the end. We will not get a
                                   // WM_DESTROY message on end session.
            break;

        case WM_DESTROY:
            // This is the end if we were closed by a DestroyWindow call.

            CloseMerc();
				PostQuitMessage(0);
            break;

        default:
            return(DefWindowProc(hWnd, message, wParam, lParam));
    }

    return(0L);
}

BOOL CALLBACK _export MercDlgProc(HWND hDlg, UINT message,
                           WPARAM wParam, LPARAM lParam)
{
    static RECT   wrect;

    int           x, y, w, h, i;
    long          rc;
//    char         *cp, *cpd, tmp[30], sdrives[30];
//    HANDLE        hCursor;
    HWND hWndInput;

    switch (message)
    {
        case WM_INITDIALOG:
            // Save the handle of this proc for use by main window proc.
            hQryDlgBox = hDlg;

            // Save the handle to the output window
            hWndOutput = GetDlgItem(hDlg, 102);
            SendMessage(hWndOutput, WM_SETFONT,
               (WPARAM) GetStockObject(SYSTEM_FIXED_FONT), 0);
            SetFocus(GetDlgItem(hDlg, 101));

            // Get position of dialog box window.
            GetWindowRect(hDlg, (LPRECT) &wrect);
            w = wrect.right - wrect.left;
            h = wrect.bottom - wrect.top;

            // Move main application window to same position.
            SetWindowPos(hWndMain, hDlg,
                         wrect.left, wrect.top, w, h,
                         0);
            break;

        case WM_MOVE:
            // Always keep this dialog box on top of main window.
            GetWindowRect(hWndMain, (LPRECT) &wrect);
            x = wrect.left;
            y = wrect.top;
            w = wrect.right - wrect.left;
            h = wrect.bottom - wrect.top;
            MoveWindow(hDlg, x, y, w, h, 1);
            break;

        case WM_SYSCOMMAND:
            // Pass WM_SYSCOMMAND messages on to main window so both
            // main window and dialog box get iconized, minimized etc.
            // in parallel.
            SendMessage(hWndMain, message, wParam, lParam);
            break;

        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                case IDOK:
                    hWndInput = GetDlgItem(hDlg, 101);
                    SendMessage(hWndInput, WM_GETTEXT, sizeof UserMessage, (LPARAM) UserMessage);
                    SendMessage(hWndInput, WM_SETTEXT, 0, (LPARAM) "");
                    SetFocus(hWndInput);
                    fUserReady = TRUE;
                    break;

                case IDCANCEL:                  // Cancel button
                    // Tell main application window we want to quit.
                    SendMessage(hWndMain, WM_CLOSE, 0, 0L);
                    break;

                default:
                    break;
            }
            break;

        case WM_CLOSE:
            // Unlock dialog resource we locked above.

            // Zero handle to this dialog window.
            hQryDlgBox = 0;

            // Tell main window to close.
            PostMessage(hWndMain, WM_CLOSE, 0, 0L);

            // Destroy ourseleves.
            DestroyWindow(hDlg);
            break;

        default:
            return FALSE;
    }

    return(TRUE);
}

void InitMerc22(HINSTANCE hInstance, int cmdShow)
{
    WNDCLASS wcMercClass;

    // Define the window class for this application.
    wcMercClass.lpszClassName = "Merc22";
    wcMercClass.hInstance     = hInstance;
    wcMercClass.lpfnWndProc   = MercWndProc;
    wcMercClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
//    wcMercClass.hIcon         = LoadIcon(hInstance, SetUpData.szAppName);
    wcMercClass.hIcon         = 0;
    wcMercClass.lpszMenuName  = (LPSTR) NULL;
    wcMercClass.hbrBackground = GetStockObject(WHITE_BRUSH);
    wcMercClass.style         = CS_HREDRAW | CS_VREDRAW;
    wcMercClass.cbClsExtra    = 0;
    wcMercClass.cbWndExtra    = 0;

    // Register the class
    if (RegisterClass(&wcMercClass) == 0)
       {
       MessageBox(0, "Could not create Window", "@@@", MB_ICONHAND|MB_OK);
       exit(1);
       }

    hInst = hInstance;       // save for use by window procs

    // Create applications main window.
    hWndMain = CreateWindow(
                  "Merc22",
                  "Welcome to Merc22/Win32",
                    WS_BORDER |
                    WS_CAPTION |
                    WS_SYSMENU |
                    WS_MINIMIZEBOX,
                  10,
                  19,
                  256,
                  123,
                  NULL,
                  NULL,
                  hInstance,
                  NULL
                  );

    CreateDialog(hInst, MAKEINTRESOURCE(1), hWndMain, (DLGPROC) MercDlgProc);
    ShowWindow(hWndMain, cmdShow);
    UpdateWindow(hWndMain);
}

int SpinTheMessageLoop()
{
   MSG msg;
   if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
      {
      if (msg.message == WM_QUIT)
         return FALSE;

      if (!IsDialogMessage(hQryDlgBox, &msg))
         {
         TranslateMessage(&msg);
         DispatchMessage(&msg);
         }
      }

   return TRUE;
}

WPARAM game_loop_win32(HINSTANCE hInstance, HINSTANCE hPrevInstance, int nCmdShow)
{
    static DESCRIPTOR_DATA dcon;
    DESCRIPTOR_DATA *d;
    MSG   msg;
    DWORD dwTick = 0;
    int   fBackground;
	extern char * help_greeting;

    /*
     * New_descriptor analogue.
     */
    dcon.descriptor	= 0;
    dcon.connected	= CON_GET_NAME;
    dcon.host		= str_dup( "localhost" );
    dcon.outsize	= 2000;
    dcon.outbuf		= alloc_mem( dcon.outsize );
    dcon.next		= descriptor_list;
    descriptor_list	= &dcon;

    /*
     * Send the greeting.
     */
	if ( help_greeting[0] == '.' )
	    write_to_buffer( &dcon, help_greeting+1, 0 );
	else
	    write_to_buffer( &dcon, help_greeting  , 0 );

    /* Main loop */

    // Go init this application.
    InitMerc22(hInstance, nCmdShow);

    // Get and dispatch messages for this applicaton.
    fBackground = FALSE;
    for ( ;; )
        {
        // Always give Windows messages priority over Merc execution
        if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
            {
            if (msg.message == WM_QUIT)
               break;

            if (!IsDialogMessage(hQryDlgBox, &msg))
               {
               TranslateMessage(&msg);
               DispatchMessage(&msg);
               }
            }
       else if (!fBackground)
          {
          fBackground++;

    time( &current_time );

	/*
	 * Autonomous game motion.
	 * (Synchronize to a clock.
	 * Busy wait (blargh).)
	 */
    if (GetTickCount() - dwTick >= 1000 / PULSE_PER_SECOND)
       {
       dwTick = GetTickCount();
	/*
	 * Process input.
	 */
	for ( d = descriptor_list; d != NULL; d = d_next )
	   {
       // Give Windows a change to run other programs
//       if (!SpinTheMessageLoop())
//          goto merc_done;

	   d_next	= d->next;
	   d->fcommand	= FALSE;

	   if ( d->character != NULL )
	        d->character->timer = 0;

       // Did the user break connection?
	   if ( !read_from_descriptor( d ) )
	     	{
		    if ( d->character != NULL )
			save_char_obj( d->character );
		    d->outtop	= 0;
		    close_socket( d );
		    continue;
		    }

	    if ( d->character != NULL && d->character->wait > 0 )
	        {
		    --d->character->wait;
		    continue;
	        }

	    read_from_buffer( d );
	    if ( d->incomm[0] != '\0' )
	    {
		d->fcommand	= TRUE;
		stop_idling( d->character );

		if ( d->connected == CON_PLAYING )
		    if ( d->showstr_point )
		        show_string( d, d->incomm );
		    else
		        interpret( d->character, d->incomm );
		else
		    nanny( d, d->incomm );

		d->incomm[0]	= '\0';
	    }
	   }


       update_handler( );

	/*
	 * Output.
	 */
	for ( d = descriptor_list; d != NULL; d = d_next )
	{
       // Give Windows a change to run other programs
//       if (!SpinTheMessageLoop())
//          goto merc_done;

	    d_next = d->next;

	    if ( ( d->fcommand || d->outtop > 0 ) )
	    {
		if ( !process_output( d, TRUE ) )
		{
		    if ( d->character != NULL )
			save_char_obj( d->character );
		    d->outtop	= 0;
		    close_socket( d );
		}
	    }
	}


#if 0
	now_time = last_time;
	for ( ; ; )
	{
	    int delta;

	    {
		if ( dcon.character != NULL )
		    dcon.character->timer = 0;
		if ( !read_from_descriptor( &dcon ) )
		{
		    if ( dcon.character != NULL )
			save_char_obj( d->character );
		    dcon.outtop	= 0;
		    close_socket( &dcon );
		}
#if defined(MSDOS)
		break;
#endif
	    }

	    gettimeofday( &now_time, NULL );
	    delta = ( now_time.tv_sec  - last_time.tv_sec  ) * 1000 * 1000
		  + ( now_time.tv_usec - last_time.tv_usec );
	    if ( delta >= 1000000 / PULSE_PER_SECOND )
		break;
	}
	last_time    = now_time;
	current_time = (time_t) last_time.tv_sec;
#endif
       }
   fBackground--;
          }
       }
merc_done:

    return(msg.wParam);
}

//int main( int argc, char **argv )
int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR lpszCmdLine, int nCmdShow)
{
    struct timeval now_time;
//    int port;

#if defined(unix)
    int control;
#endif

    /*
     * Memory debugging if needed.
     */
#if defined(MALLOC_DEBUG)
    malloc_debug( 2 );
#endif

    /*
     * Init time.
     */
    gettimeofday( &now_time, NULL );
    current_time = (time_t) now_time.tv_sec;
    strcpy( str_boot_time, ctime( &current_time ) );

    /*
     * Macintosh console initialization.
     */
#if defined(macintosh)
    console_options.nrows = 31;
    cshow( stdout );
    csetmode( C_RAW, stdin );
    cecho2file( "log file", 1, stderr );
#endif

#if 0
    /*
     * Reserve one channel for our use.
     */
    if ( ( fpReserve = fopen( NULL_FILE, "r" ) ) == NULL )
    {
	perror( NULL_FILE );
	return 1;
    }
#endif

#if 0
    /*
     * Get the port number.
     */
    port = 1234;
    if ( lpszCmdLine[0] )
    {
	if ( !is_number( lpszCmdLine ) )
	{
	    fprintf( stderr, "Usage: WinMerc [port #]\n" );
	    return 1;
	}
	else if ( ( port = atoi( lpszCmdLine ) ) <= 1024 )
	{
	    fprintf( stderr, "Port number must be above 1024.\n" );
	    return 1;
	}
    }
#endif

#if 0
    /*
     * Run the game.
     */
#if defined(macintosh) || defined(MSDOS)
    boot_db( );
    log_string( "Merc is ready to rock." );
    game_loop_mac_msdos( );
#endif

#if defined(unix)
    control = init_socket( port );
    boot_db( );
    sprintf( log_buf, "Merc is ready to rock on port %d.", port );
    log_string( log_buf );
    game_loop_unix( control );
    close( control );
#endif
#else
    boot_db( );
//    MessageBox(0, "Merc is ready to rock", "@@@", MB_ICONINFORMATION|MB_OK);
    log_string( "Merc is ready to rock." );
    game_loop_win32(hInstance, hPrev, nCmdShow);
#endif

    /*
     * That's all, folks.
     */
    log_string( "Normal termination of game." );
    return 0;
}

// @@@ Orig junk

/*
 * The primary output interface for formatted output.
 */
void act( const char *format, CHAR_DATA *ch, const void *arg1,
	 const void *arg2, int type )
{
    static char * const he_she	[] = { "it",  "he",  "she" };
    static char * const him_her	[] = { "it",  "him", "her" };
    static char * const his_her	[] = { "its", "his", "her" };

    char buf[MAX_STRING_LENGTH];
    char fname[MAX_INPUT_LENGTH];
    CHAR_DATA *to;
    CHAR_DATA *vch = (CHAR_DATA *) arg2;
    OBJ_DATA *obj1 = (OBJ_DATA  *) arg1;
    OBJ_DATA *obj2 = (OBJ_DATA  *) arg2;
    const char *str;
    const char *i;
    char *point;

    /*
     * Discard null and zero-length messages.
     */
    if ( format == NULL || format[0] == '\0' )
	return;

    to = ch->in_room->people;
    if ( type == TO_VICT )
    {
	if ( vch == NULL )
	{
	    bug( "Act: null vch with TO_VICT.", 0 );
	    return;
	}
	to = vch->in_room->people;
    }
    
    for ( ; to != NULL; to = to->next_in_room )
    {
	if ( ( to->desc == NULL 
	    && ( IS_NPC( to ) && !(to->pIndexData->progtypes & ACT_PROG ) ) )
	    || !IS_AWAKE(to) )
	    continue;

	if ( type == TO_CHAR && to != ch )
	    continue;
	if ( type == TO_VICT && ( to != vch || to == ch ) )
	    continue;
	if ( type == TO_ROOM && to == ch )
	    continue;
	if ( type == TO_NOTVICT && (to == ch || to == vch) )
	    continue;

	point	= buf;
	str	= format;
	while ( *str != '\0' )
	{
	    if ( *str != '$' )
	    {
		*point++ = *str++;
		continue;
	    }
	    ++str;

	    if ( arg2 == NULL && *str >= 'A' && *str <= 'Z' )
	    {
		bug( "Act: missing arg2 for code %d.", *str );
		i = " <@@@> ";
	    }
	    else
	    {
		switch ( *str )
		{
		default:  bug( "Act: bad code %d.", *str );
			  i = " <@@@> ";				break;
		/* Thx alex for 't' idea */
		case 't': i = (char *) arg1;				break;
		case 'T': i = (char *) arg2;          			break;
		case 'n': i = PERS( ch,  to  );				break;
		case 'N': i = PERS( vch, to  );				break;
		case 'e': i = he_she  [URANGE(0, ch  ->sex, 2)];	break;
		case 'E': i = he_she  [URANGE(0, vch ->sex, 2)];	break;
		case 'm': i = him_her [URANGE(0, ch  ->sex, 2)];	break;
		case 'M': i = him_her [URANGE(0, vch ->sex, 2)];	break;
		case 's': i = his_her [URANGE(0, ch  ->sex, 2)];	break;
		case 'S': i = his_her [URANGE(0, vch ->sex, 2)];	break;

		case 'p':
		    i = can_see_obj( to, obj1 )
			    ? obj1->short_descr
			    : "something";
		    break;

		case 'P':
		    i = can_see_obj( to, obj2 )
			    ? obj2->short_descr
			    : "something";
		    break;

		case 'd':
		    if ( arg2 == NULL || ((char *) arg2)[0] == '\0' )
		    {
			i = "door";
		    }
		    else
		    {
			one_argument( (char *) arg2, fname );
			i = fname;
		    }
		    break;
		}
	    }
		
	    ++str;
	    while ( ( *point = *i ) != '\0' )
		++point, ++i;
	}

	*point++ = '\n';
	*point++ = '\r';
	buf[0]   = UPPER(buf[0]);
	if (to->desc)
	  write_to_buffer( to->desc, buf, point - buf );
	if (MOBtrigger)
	  mprog_act_trigger( buf, to, ch, obj1, vch );
	                                            /* Added by Kahn */
    }

    MOBtrigger = TRUE;
    return;
}

/*
 * Append onto an output buffer.
 */
void write_to_buffer( DESCRIPTOR_DATA *d, const char *txt, int length )
{
    /* @@@ ECS */
    if (d == 0)
       return;
       
    /*
     * Find length in case caller didn't.
     */
    if ( length <= 0 )
	length = strlen(txt);

    /*
     * Initial \n\r if needed.
     */
    if ( d->outtop == 0 && !d->fcommand )
    {
	d->outbuf[0]	= '\n';
	d->outbuf[1]	= '\r';
	d->outtop	= 2;
    }

    /*
     * Expand the buffer as needed.
     */
    while ( d->outtop + length >= d->outsize )
    {
	char *outbuf;

	outbuf      = alloc_mem( 2 * d->outsize );
	strncpy( outbuf, d->outbuf, d->outtop );
	free_mem( d->outbuf, d->outsize );
	d->outbuf   = outbuf;
	d->outsize *= 2;
    }

    /*
     * Copy.
     */
    strcpy( d->outbuf + d->outtop, txt );
    d->outtop += length;
    return;
}

void close_socket( DESCRIPTOR_DATA *dclose )
{
    CHAR_DATA *ch;

    if ( dclose->outtop > 0 )
	process_output( dclose, FALSE );

    if ( dclose->snoop_by != NULL )
    {
	write_to_buffer( dclose->snoop_by,
	    "Your victim has left the game.\n\r", 0 );
    }

    {
	DESCRIPTOR_DATA *d;

	for ( d = descriptor_list; d != NULL; d = d->next )
	{
	    if ( d->snoop_by == dclose )
		d->snoop_by = NULL;
	}
    }

    if ( ( ch = dclose->character ) != NULL )
    {
	sprintf( log_buf, "Closing link to %s.", ch->name );
	log_string( log_buf );
	if ( dclose->connected == CON_PLAYING )
	{
	    act( "$n has lost $s link.", ch, NULL, NULL, TO_ROOM );
	    ch->desc = NULL;
	}
	else
	{
	    free_char( dclose->character );
	}
    }

    if ( d_next == dclose )
	d_next = d_next->next;

    if ( dclose == descriptor_list )
    {
	descriptor_list = descriptor_list->next;
    }
    else
    {
	DESCRIPTOR_DATA *d;

	for ( d = descriptor_list; d && d->next != dclose; d = d->next )
	    ;
	if ( d != NULL )
	    d->next = dclose->next;
	else
	    bug( "Close_socket: dclose not found.", 0 );
    }

//    close( dclose->descriptor );
    free_string( dclose->host );
    dclose->next	= descriptor_free;
    descriptor_free	= dclose;
//@@@#if defined(MSDOS) || defined(macintosh)
    exit(1);
//@@@#endif
    return;
}

/*
 * Lowest level output function.
 * Write a block of text to the file descriptor.
 * If this gives errors on very long blocks (like 'ofind all'),
 *   try lowering the max block size.
 */
bool write_to_descriptor( int desc, char *txt, int length )
{
    int iStart;
    int nWrite;
    int nBlock;

#if defined(macintosh) || defined(MSDOS)
    if ( desc == 0 )
	desc = 1;
#endif
   char c;
   int n;
   char *p;
      char szBuffer[32767];

    if ( length <= 0 )
	length = strlen(txt);

#if 0
    for ( iStart = 0; iStart < length; iStart += nWrite )
    {
	nBlock = UMIN( length - iStart, 4096 );
	if ( ( nWrite = write( desc, txt + iStart, nBlock ) ) < 0 )
	    { perror( "Write_to_descriptor" ); return FALSE; }
    }
#else
   c = txt[length];
   txt[length] = '\0';
//   MessageBox(0, txt, "@@@", MB_ICONINFORMATION|MB_OK);
//   SendMessage(hWndOutput, WM_SETTEXT, 0, (LPARAM) txt);
   SendMessage(hWndOutput, WM_GETTEXT, 32767, (LPARAM) szBuffer);
   /* Count the newlines */
   for (p = szBuffer, n = 0; *p; p++)
      if (*p == '\n')
         n++;
   for (p = txt; *p; p++)
      if (*p == '\n')
         n++;
   if (n < 22) // @@@ Totally font dependent...
      {
//      strcpy(szBuffer+n, txt);
      strcat(szBuffer, "\n");
      strcat(szBuffer, txt);
      SendMessage(hWndOutput, WM_SETTEXT, 0, (LPARAM) szBuffer);
      }
   else
      SendMessage(hWndOutput, WM_SETTEXT, 0, (LPARAM) txt);

   txt[length] = c;
#endif

    return TRUE;
}



/*
 * Deal with sockets that haven't logged in yet.
 */
void nanny( DESCRIPTOR_DATA *d, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *ch;
    NOTE_DATA *pnote;
    char *pwdnew;
    char *p;
    int iClass;
    int lines;
    int notes;
    bool fOld;

    while ( isspace(*argument) )
	argument++;

    ch = d->character;

    switch ( d->connected )
    {

    default:
	bug( "Nanny: bad d->connected %d.", d->connected );
	close_socket( d );
	return;

    case CON_GET_NAME:
	if ( argument[0] == '\0' )
	{
	    close_socket( d );
	    return;
	}

	argument[0] = UPPER(argument[0]);
	if ( !check_parse_name( argument ) )
	{
	    write_to_buffer( d, "Illegal name, try another.\n\rName: ", 0 );
	    return;
	}

	fOld = load_char_obj( d, argument );
	ch   = d->character;

	if ( IS_SET(ch->act, PLR_DENY) )
	{
	    sprintf( log_buf, "Denying access to %s@%s.", argument, d->host );
	    log_string( log_buf );
	    write_to_buffer( d, "You are denied access.\n\r", 0 );
	    close_socket( d );
	    return;
	}

	if ( check_reconnect( d, argument, FALSE ) )
	{
	    fOld = TRUE;
	}
	else
	{
	    if ( wizlock && !IS_HERO( ch ) && !ch->wizbit )
	    {
		write_to_buffer( d, "The game is wizlocked.\n\r", 0 );
		close_socket( d );
		return;
	    }
	}

	if ( fOld )
	{
	    /* Old player */
	    write_to_buffer( d, "Password: ", 0 );
	    write_to_buffer( d, echo_off_str, 0 );
	    d->connected = CON_GET_OLD_PASSWORD;
	    return;
	}
	else
	{
	    /* New player */
	    /* New characters with same name fix by Salem's Lot */
	    if ( check_playing( d, ch->name ) )
	        return;
	    sprintf( buf, "Did I get that right, %s (Y/N)? ", argument );
	    write_to_buffer( d, buf, 0 );
	    d->connected = CON_CONFIRM_NEW_NAME;
	    return;
	}
//@@@	break;

    case CON_GET_OLD_PASSWORD:
#if defined(unix)
	write_to_buffer( d, "\n\r", 2 );
#endif

	if ( strcmp( crypt( argument, ch->pcdata->pwd ), ch->pcdata->pwd ) )
	{
	    write_to_buffer( d, "Wrong password.\n\r", 0 );
	    close_socket( d );
	    return;
	}

	write_to_buffer( d, echo_on_str, 0 );

	if ( check_reconnect( d, ch->name, TRUE ) )
	    return;

	if ( check_playing( d, ch->name ) )
	    return;
		    
	sprintf( log_buf, "%s@%s has connected.", ch->name, d->host );
	log_string( log_buf );
	lines = ch->pcdata->pagelen;
	ch->pcdata->pagelen = 20;
	if ( IS_HERO(ch) )
	    do_help( ch, "imotd" );
	do_help( ch, "motd" );
	ch->pcdata->pagelen = lines;
	d->connected = CON_READ_MOTD;
	break;

    case CON_CONFIRM_NEW_NAME:
	switch ( *argument )
	{
	case 'y': case 'Y':
	    sprintf( buf, "New character.\n\rGive me a password for %s: %s",
		ch->name, echo_off_str );
	    write_to_buffer( d, buf, 0 );
	    d->connected = CON_GET_NEW_PASSWORD;
	    break;

	case 'n': case 'N':
	    write_to_buffer( d, "Ok, what IS it, then? ", 0 );
	    free_char( d->character );
	    d->character = NULL;
	    d->connected = CON_GET_NAME;
	    break;

	default:
	    write_to_buffer( d, "Please type Yes or No? ", 0 );
	    break;
	}
	break;

    case CON_GET_NEW_PASSWORD:
#if defined(unix)
	write_to_buffer( d, "\n\r", 2 );
#endif

	if ( strlen(argument) < 3 )
	{
	    write_to_buffer( d,
		"Password must be at least three characters long.\n\rPassword: ",
		0 );
	    return;
	}

	pwdnew = crypt( argument, ch->name );
	for ( p = pwdnew; *p != '\0'; p++ )
	{
	    if ( *p == '~' )
	    {
		write_to_buffer( d,
		    "New password not acceptable, try again.\n\rPassword: ",
		    0 );
		return;
	    }
	}

	free_string( ch->pcdata->pwd );
	ch->pcdata->pwd	= str_dup( pwdnew );
	write_to_buffer( d, "Please retype password: ", 0 );
	d->connected = CON_CONFIRM_NEW_PASSWORD;
	break;

    case CON_CONFIRM_NEW_PASSWORD:
#if defined(unix)
	write_to_buffer( d, "\n\r", 2 );
#endif

	if ( strcmp( crypt( argument, ch->pcdata->pwd ), ch->pcdata->pwd ) )
	{
	    write_to_buffer( d, "Passwords don't match.\n\rRetype password: ",
		0 );
	    d->connected = CON_GET_NEW_PASSWORD;
	    return;
	}

	write_to_buffer( d, echo_on_str, 0 );
	write_to_buffer( d, "What is your sex (M/F/N)? ", 0 );
	d->connected = CON_GET_NEW_SEX;
	break;

    case CON_GET_NEW_SEX:
	switch ( argument[0] )
	{
	case 'm': case 'M': ch->sex = SEX_MALE;    break;
	case 'f': case 'F': ch->sex = SEX_FEMALE;  break;
	case 'n': case 'N': ch->sex = SEX_NEUTRAL; break;
	default:
	    write_to_buffer( d, "That's not a sex.\n\rWhat IS your sex? ", 0 );
	    return;
	}

	strcpy( buf, "Select a class [" );
	for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
	{
	    if ( iClass > 0 )
		strcat( buf, " " );
	    strcat( buf, class_table[iClass].who_name );
	}
	strcat( buf, "]: " );
	write_to_buffer( d, buf, 0 );
	d->connected = CON_GET_NEW_CLASS;
	break;

    case CON_GET_NEW_CLASS:
	for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
	{
	    if ( !str_cmp( argument, class_table[iClass].who_name ) )
	    {
		ch->class = iClass;
		break;
	    }
	}

	if ( iClass == MAX_CLASS )
	{
	    write_to_buffer( d,
		"That's not a class.\n\rWhat IS your class? ", 0 );
	    return;
	}

	sprintf( log_buf, "%s@%s new player.", ch->name, d->host );
	log_string( log_buf );
	write_to_buffer( d, "\n\r", 2 );
	ch->pcdata->pagelen = 20;
	ch->prompt = "<%hhp %mm %vmv> ";
	do_help( ch, "motd" );
	d->connected = CON_READ_MOTD;
	break;

    case CON_READ_MOTD:
	ch->next	= char_list;
	char_list	= ch;
	d->connected	= CON_PLAYING;

	send_to_char(
    "\n\rWelcome to Merc Diku Mud.  May your visit here be ... Mercenary.\n\r",
	    ch );

	if ( ch->level == 0 )
	{
	    OBJ_DATA *obj;

	    switch ( class_table[ch->class].attr_prime )
	    {
	    case APPLY_STR: ch->pcdata->perm_str = 16; break;
	    case APPLY_INT: ch->pcdata->perm_int = 16; break;
	    case APPLY_WIS: ch->pcdata->perm_wis = 16; break;
	    case APPLY_DEX: ch->pcdata->perm_dex = 16; break;
	    case APPLY_CON: ch->pcdata->perm_con = 16; break;
	    }

	    ch->level	= 1;
	    ch->exp	= 1000;
	    ch->hit	= ch->max_hit;
	    ch->mana	= ch->max_mana;
	    ch->move	= ch->max_move;
	    sprintf( buf, "the %s",
		title_table [ch->class] [ch->level]
		[ch->sex == SEX_FEMALE ? 1 : 0] );
	    set_title( ch, buf );

	    obj = create_object( get_obj_index(OBJ_VNUM_SCHOOL_BANNER), 0 );
	    obj_to_char( obj, ch );
	    equip_char( ch, obj, WEAR_LIGHT );

	    obj = create_object( get_obj_index(OBJ_VNUM_SCHOOL_VEST), 0 );
	    obj_to_char( obj, ch );
	    equip_char( ch, obj, WEAR_BODY );

	    obj = create_object( get_obj_index(OBJ_VNUM_SCHOOL_SHIELD), 0 );
	    obj_to_char( obj, ch );
	    equip_char( ch, obj, WEAR_SHIELD );

	    obj = create_object( get_obj_index(class_table[ch->class].weapon),
		0 );
	    obj_to_char( obj, ch );
	    equip_char( ch, obj, WEAR_WIELD );

	    char_to_room( ch, get_room_index( ROOM_VNUM_SCHOOL ) );
	}
	else if ( ch->in_room != NULL )
	{
	    char_to_room( ch, ch->in_room );
	}
	else if ( IS_IMMORTAL(ch) )
	{
	    char_to_room( ch, get_room_index( ROOM_VNUM_CHAT ) );
	}
	else
	{
	    char_to_room( ch, get_room_index( ROOM_VNUM_TEMPLE ) );
	}

	act( "$n has entered the game.", ch, NULL, NULL, TO_ROOM );
	do_look( ch, "auto" );
	/* check for new notes */
	notes = 0;

	for ( pnote = note_list; pnote != NULL; pnote = pnote->next )
	    if ( is_note_to( ch, pnote ) && str_cmp( ch->name, pnote->sender )
		&& pnote->date_stamp > ch->last_note )
	        notes++;

	if ( notes == 1 )
	    send_to_char( "\n\rYou have one new note waiting.\n\r", ch );
	else
	    if ( notes > 1 )
	    {
		sprintf( buf, "\n\rYou have %d new notes waiting.\n\r",
			notes );
		send_to_char( buf, ch );
	    }

	break;
    }

    return;
}



/*
 * Parse a name for acceptability.
 */
bool check_parse_name( char *name )
{
    /*
     * Reserved words.
     */
    if ( is_name( name, "all auto immortal self someone" ) )
	return FALSE;

    /*
     * Length restrictions.
     */
    if ( strlen(name) <  3 )
	return FALSE;

#if defined(MSDOS)
    if ( strlen(name) >  8 )
	return FALSE;
#endif

#if defined(macintosh) || defined(unix)
    if ( strlen(name) > 12 )
	return FALSE;
#endif

    /*
     * Alphanumerics only.
     * Lock out IllIll twits.
     */
    {
	char *pc;
	bool fIll;

	fIll = TRUE;
	for ( pc = name; *pc != '\0'; pc++ )
	{
	    if ( !isalpha(*pc) )
		return FALSE;
	    if ( LOWER(*pc) != 'i' && LOWER(*pc) != 'l' )
		fIll = FALSE;
	}

	if ( fIll )
	    return FALSE;
    }

    /*
     * Prevent players from naming themselves after mobs.
     */
    {
	extern MOB_INDEX_DATA *mob_index_hash[MAX_KEY_HASH];
	MOB_INDEX_DATA *pMobIndex;
	int iHash;

	for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
	{
	    for ( pMobIndex  = mob_index_hash[iHash];
		  pMobIndex != NULL;
		  pMobIndex  = pMobIndex->next )
	    {
		if ( is_name( name, pMobIndex->player_name ) )
		    return FALSE;
	    }
	}
    }

    return TRUE;
}



/*
 * Look for link-dead player to reconnect.
 */
bool check_reconnect( DESCRIPTOR_DATA *d, char *name, bool fConn )
{
    CHAR_DATA *ch;
    OBJ_DATA *obj;

    for ( ch = char_list; ch != NULL; ch = ch->next )
    {
	if ( !IS_NPC(ch)
	&& ( !fConn || ch->desc == NULL )
	&&   !str_cmp( d->character->name, ch->name ) )
	{
	    if ( fConn == FALSE )
	    {
		free_string( d->character->pcdata->pwd );
		d->character->pcdata->pwd = str_dup( ch->pcdata->pwd );
	    }
	    else
	    {
		free_char( d->character );
		d->character = ch;
		ch->desc	 = d;
		ch->timer	 = 0;
		send_to_char( "Reconnecting.\n\r", ch );
		act( "$n has reconnected.", ch, NULL, NULL, TO_ROOM );
		sprintf( log_buf, "%s@%s reconnected.", ch->name, d->host );
		log_string( log_buf );
		d->connected = CON_PLAYING;

		/*
		 * Contributed by Gene Choi
		 */
		if ( ( obj = get_eq_char( ch, WEAR_LIGHT ) ) != NULL
		    && obj->item_type == ITEM_LIGHT
		    && obj->value[2] != 0
		    && ch->in_room != NULL )
		    ++ch->in_room->light;
	    }
	    return TRUE;
	}
    }

    return FALSE;
}



/*
 * Check if already playing.
 */
bool check_playing( DESCRIPTOR_DATA *d, char *name )
{
    DESCRIPTOR_DATA *dold;

    for ( dold = descriptor_list; dold; dold = dold->next )
    {
	if ( dold != d
	&&   dold->character != NULL
	&&   dold->connected != CON_GET_NAME
	&&   dold->connected != CON_GET_OLD_PASSWORD
	&&   !str_cmp( name, dold->original
	         ? dold->original->name : dold->character->name ) )
	{
	    write_to_buffer( d, "Already playing.\n\rName: ", 0 );
	    d->connected = CON_GET_NAME;
	    if ( d->character != NULL )
	    {
		free_char( d->character );
		d->character = NULL;
	    }
	    return TRUE;
	}
    }

    return FALSE;
}



void stop_idling( CHAR_DATA *ch )
{
    if ( ch == NULL
    ||   ch->desc == NULL
    ||   ch->desc->connected != CON_PLAYING
    ||   ch->was_in_room == NULL
    ||   ch->in_room != get_room_index( ROOM_VNUM_LIMBO ) )
	return;

    ch->timer = 0;
    char_from_room( ch );
    char_to_room( ch, ch->was_in_room );
    ch->was_in_room	= NULL;
    act( "$n has returned from the void.", ch, NULL, NULL, TO_ROOM );
    return;
}

/*
 * Low level output function.
 */
bool process_output( DESCRIPTOR_DATA *d, bool fPrompt )
{
    extern bool merc_down;

    /*
     * Bust a prompt.
     */
    if ( fPrompt && !merc_down && d->connected == CON_PLAYING )
        if ( d->showstr_point )
	    write_to_buffer( d,
  "[Please type (c)ontinue, (r)efresh, (b)ack, (h)elp, (q)uit, or RETURN]:  ",
			    0 );
	else
	{
	    CHAR_DATA *ch;

	    ch = d->original ? d->original : d->character;
	    if ( IS_SET(ch->act, PLR_BLANK) )
	        write_to_buffer( d, "\n\r", 2 );

	    if ( IS_SET(ch->act, PLR_PROMPT) )
	        bust_a_prompt( ch );

	    if ( IS_SET(ch->act, PLR_TELNET_GA) )
	        write_to_buffer( d, go_ahead_str, 0 );
	}

    /*
     * Short-circuit if nothing to write.
     */
    if ( d->outtop == 0 )
	return TRUE;

    /*
     * Snoop-o-rama.
     */
    if ( d->snoop_by != NULL )
    {
	write_to_buffer( d->snoop_by, "% ", 2 );
	write_to_buffer( d->snoop_by, d->outbuf, d->outtop );
    }

    /*
     * OS-dependent output.
     */
    if ( !write_to_descriptor( d->descriptor, d->outbuf, d->outtop ) )
    {
	d->outtop = 0;
	return FALSE;
    }
    else
    {
	d->outtop = 0;
	return TRUE;
    }
}

/*
 * Bust a prompt (player settable prompt)
 * coded by Morgenes for Aldara Mud
 */
void bust_a_prompt( CHAR_DATA *ch )
{
   char buf[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];
   const char *str;
   const char *i;
   char *point;

   if( ch->prompt == NULL || ch->prompt[0] == '\0' )
   {
      send_to_char( "\n\r\n\r", ch );
      return;
   }

   point = buf;
   str = ch->prompt;
   while( *str != '\0' )
   {
      if( *str != '%' )
      {
         *point++ = *str++;
         continue;
      }
      ++str;
      switch( *str )
      {
         default :
            i = " "; break;
         case 'h' :
            sprintf( buf2, "%d", ch->hit );
            i = buf2; break;
         case 'H' :
            sprintf( buf2, "%d", ch->max_hit );
            i = buf2; break;
         case 'm' :
            sprintf( buf2, "%d", ch->mana );
            i = buf2; break;
         case 'M' :
            sprintf( buf2, "%d", ch->max_mana );
            i = buf2; break;
         case 'v' :
            sprintf( buf2, "%d", ch->move );
            i = buf2; break;
         case 'V' :
            sprintf( buf2, "%d", ch->max_move );
            i = buf2; break;
         case 'x' :
            sprintf( buf2, "%d", ch->exp );
            i = buf2; break;
         case 'g' :
            sprintf( buf2, "%d", ch->gold);
            i = buf2; break;
         case 'a' :
            if( ch->level < 5 )
               sprintf( buf2, "%d", ch->alignment );
            else
               sprintf( buf2, "%s", IS_GOOD(ch) ? "good" : IS_EVIL(ch) ? 
                "evil" : "neutral" );
            i = buf2; break;
         case 'r' :
            if( ch->in_room != NULL )
               sprintf( buf2, "%s", ch->in_room->name );
            else
               sprintf( buf2, " " );
            i = buf2; break;
         case 'R' :
            if( IS_IMMORTAL( ch ) && ch->in_room != NULL )
               sprintf( buf2, "%d", ch->in_room->vnum );
            else
               sprintf( buf2, " " );
            i = buf2; break;
         case 'z' :
            if( IS_IMMORTAL( ch ) && ch->in_room != NULL )
               sprintf( buf2, "%s", ch->in_room->area->name );
            else
               sprintf( buf2, " " );
            i = buf2; break;
         case '%' :
            sprintf( buf2, "%%" );
            i = buf2; break;
      } 
      ++str;
      while( (*point = *i) != '\0' )
         ++point, ++i;
   }
   write_to_buffer( ch->desc, buf, point - buf );
   return;
}

/* The heart of the pager.  Thanks to N'Atas-Ha, ThePrincedom
   for porting this SillyMud code for MERC 2.0 and laying down the groundwork.
   Thanks to Blackstar, hopper.cs.uiowa.edu 4000 for which
   the improvements to the pager was modeled from.  - Kahn */

void show_string(struct descriptor_data *d, char *input)
{
  char buffer[ MAX_STRING_LENGTH ];
  char buf[ MAX_INPUT_LENGTH ];
  register char *scan, *chk;
  int lines = 0, toggle=1;

  one_argument(input, buf);

  switch( UPPER( buf[0] ) )
  {
  case '\0':
  case 'C': /* show next page of text */
    lines = 0;
    break;

  case 'R': /* refresh current page of text */
    lines = - 1 - (d->character->pcdata->pagelen);
    break;

  case 'B': /* scroll back a page of text */
    lines = -(2*d->character->pcdata->pagelen);
    break;

  case 'H': /* Show some help */
    write_to_buffer( d,
        "C, or Return = continue, R = redraw this page,\n\r", 0 );
    write_to_buffer( d,
        "B = back one page, H = this help, Q or other keys = exit.\n\r\n\r",
		    0 );
    lines = - 1 - (d->character->pcdata->pagelen);
    break;

  default: /*otherwise, stop the text viewing */
    if ( d->showstr_head )
    {
      free_string( d->showstr_head );
      d->showstr_head  = 0;
    }
    d->showstr_point = 0;
    return;

  }

  /* do any backing up necessary */
  if (lines < 0)
  {
    for ( scan = d->showstr_point; scan > d->showstr_head; scan-- )
         if ( ( *scan == '\n' ) || ( *scan == '\r' ) )
	 {
	     toggle = -toggle;
	     if ( toggle < 0 )
	         if ( !( ++lines ) )
		     break;
	 }
    d->showstr_point = scan;
  }

  /* show a chunk */
  lines  = 0;
  toggle = 1;
  for ( scan = buffer; ; scan++, d->showstr_point++ )
       if ( ( ( *scan = *d->showstr_point ) == '\n' || *scan == '\r' )
	   && ( toggle = -toggle ) < 0 )
	   lines++;
       else
	   if ( !*scan || ( d->character && !IS_NPC( d->character )
			  && lines >= d->character->pcdata->pagelen) )
	   {

	       *scan = '\0';
	       write_to_buffer( d, buffer, strlen( buffer ) );

	     /* See if this is the end (or near the end) of the string */
	       for ( chk = d->showstr_point; isspace( *chk ); chk++ );
	       if ( !*chk )
	       {
		   if ( d->showstr_head )
		   {
		      free_string( d->showstr_head );
		      d->showstr_head  = 0;
		   }
		   d->showstr_point = 0;
	       }
	       return;
	   }

//@@@  return;
}

