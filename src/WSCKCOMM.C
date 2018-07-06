/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  Envy Diku Mud improvements copyright (C) 1994 by Michael Quan, David   *
 *  Love, Guilherme 'Willie' Arnold, and Mitchell Tse.                     *
 *                                                                         *
 *  Windows sockets port by Slash.                                         *
 *                                                                         *
 *  In order to use any part of this Envy Diku Mud, you must comply with   *
 *  the original Diku license in 'license.doc', the Merc license in        *
 *  'license.txt', as well as the Envy license in 'license.nvy'.           *
 *  In particular, you may not remove either of these copyright notices.   *
 *                                                                         *
 *  Thanks to abaddon for proof-reading our comm.c and pointing out bugs.  *
 *  Any remaining bugs are, of course, our work, not his.  :)              *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

//#include "telnet.h"

#include "merc.h"
#include "merc-win.rh"

#include <windows.h>

// Yes, we want mobprogram support
#define MOBPROGS

// Yes, we want support for Merc 2.2 for Windows additions
#define MERCW32

#define STDERR_FILE     "stderr.txt" /* For what Unix writes to standard error */

/*
 * Definitions for the TELNET protocol.
 */
#define IAC     255             /* interpret as command: */
#define WONT    252             /* I won't use option */
#define WILL    251             /* I will use option */
#define GA      249                /* you may reverse the line */

/* telnet options */
#define TELOPT_ECHO     1       /* echo */

struct host_and_name_lookup {
    // These are used by WSAAsyncGetHostByAddr()
    char       hostdata[MAXGETHOSTSTRUCT];
    HANDLE     hRequestHandle;
    struct     in_addr sin_addr;

	 // These are used by the identd stuff
    char       username[64];
    SOCKET     sAuth;
    struct sockaddr_in authsock;    // Address of the authentication
    struct sockaddr_in us;          // Address of our socket to this player
	 struct sockaddr_in them;        // Address of our socket to this player
};

/* Local #defines for Win32 */
//#define MAKEWORD(a, b)      ((WORD)(((BYTE)(a)) | (((WORD)((BYTE)(b))) << 8)))

#define WM_NET_ACCEPT      (WM_USER+500)
#define WM_NET_READWRITE   (WM_USER+501)
#define WM_NET_GETHOST     (WM_USER+502)
#define WM_NET_AUTHCONNECT (WM_USER+503)

/*
 * Global variables.
 */
DESCRIPTOR_DATA *   descriptor_free;    /* Free list for descriptors    */
DESCRIPTOR_DATA *   first_descriptor;	/* First descriptor		*/
DESCRIPTOR_DATA *   last_descriptor;	/* Last descriptor		*/
DESCRIPTOR_DATA *   descriptor_list;    /* All open descriptors         */
DESCRIPTOR_DATA *   d_next;             /* Next descriptor in loop      */
FILE *              fpReserve;          /* Reserved file handle         */
bool                god;                /* All new chars are gods!      */
bool                merc_down;          /* Shutdown                     */
bool                wizlock;            /* Game is wizlocked            */
int                 numlock = 0;        /* Game is numlocked at <level> (ENVY) */
char                str_boot_time[MAX_INPUT_LENGTH];
time_t              current_time;       /* Time of this pulse           */

/* Windows version variables */
SOCKET control;
int nPlayers = 0;
int nOldPlayers = 0;
FILE *fpStderr;
BOOL fWantsMessageBox = FALSE; // if TRUE, bring up message box on bugs, else log

/*
 * Other local functions (OS-independent).
 */
bool    check_parse_name        args( ( char *name ) );
bool    check_reconnect         args( ( DESCRIPTOR_DATA *d, char *name,
				    bool fConn ) );
bool    check_playing           args( ( DESCRIPTOR_DATA *d, char *name ) );
int     main                    args( ( int argc, char **argv ) );
void    nanny                   args( ( DESCRIPTOR_DATA *d, char *argument ) );
bool    process_output          args( ( DESCRIPTOR_DATA *d, bool fPrompt ) );
void    read_from_buffer        args( ( DESCRIPTOR_DATA *d ) );
void    stop_idling             args( ( CHAR_DATA *ch ) );
//void    bust_a_prompt           args( ( CHAR_DATA *ch ) );
void    bust_a_prompt           args( ( DESCRIPTOR_DATA *d ) );

// @@@ My junk
//char      UserMessage[512];
//BOOL      fUserReady;
const   char    echo_off_str    [] = { IAC, WILL, TELOPT_ECHO, '\0' };
const   char    echo_on_str     [] = { IAC, WONT, TELOPT_ECHO, '\0' };
const   char    go_ahead_str    [] = { IAC, GA, '\0' };
bool    write_to_descriptor     args( ( int desc, char *txt, int length ) );
bool BlastedTrumpet(SOCKET sSocket);
LRESULT SocketAccept (WPARAM /*wParam*/, LPARAM lParam);
LRESULT SocketReadWrite (WPARAM wParam, LPARAM lParam);
LRESULT SocketGetHost(WPARAM wParam, LPARAM lParam);
LRESULT SocketAuthConnect(WPARAM wParam, LPARAM lParam);
SOCKET init_socket( int port );
bool read_from_descriptor( DESCRIPTOR_DATA *d );

void    send_to_char    args( ( const char *txt, CHAR_DATA *ch ) )
{
    if ( txt == NULL || ch->desc == NULL )
	return;
    free_string( ch->desc->showstr_head );
#if 0 // @@@ The next line is from sands, it seems wrong
    ch->desc->showstr_head = str_dup( txt );
#else
    ch->desc->showstr_head = alloc_mem( strlen( txt ) + 1 );
    strcpy( ch->desc->showstr_head, txt );
#endif
    ch->desc->showstr_point = ch->desc->showstr_head;
    show_string( ch->desc, "" );
}

void gettimeofday( struct timeval *tp, void *tzp )
{
	 tp->tv_sec  = time( NULL );
	 tp->tv_usec = 0;
}

#if 0 // @@@
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
	sprintf( log_buf, "Log: %s input overflow!", d->host );
	log_string( log_buf );
	NOTIFY( log_buf, LEVEL_IMMORTAL, WIZ_NOTIFY_LOG );
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
#endif


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
		sprintf( log_buf, "Log: %s input spamming!", d->host );
		log_string( log_buf );
		NOTIFY( log_buf, LEVEL_IMMORTAL, WIZ_NOTIFY_LOG );
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
//char      UserMessage[512];
//bool      fUserReady;

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
//            SendMessage(hWndOutput, WM_SETFONT,
//               (WPARAM) GetStockObject(SYSTEM_FIXED_FONT), 0);
	    SetFocus(GetDlgItem(hDlg, 101));

				// Get position of dialog box window.
	    GetWindowRect(hDlg, (LPRECT) &wrect);
	    w = wrect.right - wrect.left;
	    h = wrect.bottom - wrect.top;

	    // Move main application window to same position.
	    SetWindowPos(hWndMain, hDlg,
			 wrect.left, wrect.top, w, h,
			 0);

	    // Tell Winsock we are listening for accepted sockets
	    WSAAsyncSelect(control, hDlg, WM_NET_ACCEPT, FD_ACCEPT);
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
#if 0 // @@@
		case IDOK:
						  hWndInput = GetDlgItem(hDlg, 101);
		    SendMessage(hWndInput, WM_GETTEXT, sizeof UserMessage, (LPARAM) UserMessage);
		    SendMessage(hWndInput, WM_SETTEXT, 0, (LPARAM) "");
		    SetFocus(hWndInput);
		    fUserReady = TRUE;
		    break;
#endif
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

	case WM_NET_ACCEPT:
	   SocketAccept(wParam, lParam);
	   break;

	case WM_NET_READWRITE:
	   SocketReadWrite(wParam, lParam);
			  break;

	case WM_NET_GETHOST:
	   SocketGetHost(wParam, lParam);
	   break;

	case WM_NET_AUTHCONNECT:
	   SocketAuthConnect(wParam, lParam);
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

/*int SpinTheMessageLoop()
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
}*/

WPARAM game_loop_win32(HINSTANCE hInstance, HINSTANCE hPrevInstance, int nCmdShow)
{
//    static DESCRIPTOR_DATA dcon;
    DESCRIPTOR_DATA *d;
    MSG   msg;
    DWORD dwTick = 0;
    int   fBackground;
	extern char * help_greeting;
//   HDC hDC;
    char szBuffer[128];

   extern int                   nAllocString;
	extern int                      sAllocString;
   extern int                   nAllocPerm;
   extern int                   sAllocPerm;
   static int nOldNString = -1;
   static int nOldSString = -1;
   static int nOldNPerm = -1;
   static int nOldSPerm = -1;

    /* Main loop */

//    // Go init this application.
//    InitMerc22(hInstance, nCmdShow);

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

	   d_next       = d->next;
	   d->fcommand  = FALSE;

	   if ( d->character != NULL )
		d->character->timer = 0;

#if 0
       // Did the user break connection?
		if ( !read_from_descriptor( d ) )
		{
		    if ( d->character != NULL )
			save_char_obj( d->character );
		    d->outtop   = 0;
		    close_socket( d );
		    continue;
		    }
#endif
	    if ( d->character != NULL && d->character->wait > 0 )
		{
		    --d->character->wait;
			 continue;
		}

	    read_from_buffer( d );
	    if ( d->incomm[0] != '\0' )
	    {
		d->fcommand     = TRUE;
		stop_idling( d->character );

		if ( d->pString )
			 string_add( d->character, d->incomm );
		else if ( d->connected == CON_PLAYING )
						  if ( d->showstr_point )
								show_string( d, d->incomm );
						  else
								interpret( d->character, d->incomm );
					 else
						  nanny( d, d->incomm );


		d->incomm[0]    = '\0';
	    }
	   }

       update_handler( );

		 // Update the screen
		 if (nAllocString != nOldNString || sAllocString != nOldSString)
	  {
	  nOldNString = nAllocString;
	  nOldSString = sAllocString;
	  wsprintf( szBuffer, "Strings %5d strings of %7d bytes",
	     nAllocString, sAllocString);
	  SetDlgItemText(hQryDlgBox, 103, szBuffer);
	  }

       if (nAllocPerm != nOldNPerm || sAllocPerm != nOldSPerm)
	  {
	  wsprintf( szBuffer, "Perms   %5d blocks  of %7d bytes",
				 nAllocPerm, sAllocPerm );
	  nOldNPerm = nAllocPerm;
	  nOldSPerm = sAllocPerm;
	  SetDlgItemText(hQryDlgBox, 104, szBuffer);
	  }

       if (nPlayers != nOldPlayers)
	  {
	  wsprintf( szBuffer, "%d", nPlayers);
	  nOldPlayers = nPlayers;
	  SetDlgItemText(hQryDlgBox, 101, szBuffer);
	  }
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
		if ( !process_output( d, TRUE ) && WSAGetLastError() != WSAEWOULDBLOCK)
		{
		    if ( d->character != NULL )
			save_char_obj( d->character );
		    d->outtop   = 0;
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
		    dcon.outtop = 0;
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
    extern char * help_greeting;
    struct timeval now_time;
    int port;

//#if defined(unix)
//    int control;
//#endif
#ifdef WIN32
   WORD wVersionRequested = MAKEWORD( 1, 1 );

   WSADATA wsaData;
   int err;
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

#if 0 // @@@
    /*
	  * Get the port number.
	  */
    port = 1234;
    if ( lpszCmdLine[0] )
    {
	if ( !is_number( lpszCmdLine ) )
	{
//          fprintf( stderr, "Usage: Merc22sv [port #]\n" );
       MessageBox(0, "Usage: Merc22sv [port #]", "Merc22", MB_ICONHAND|MB_OK);
	    return 1;
	}
#if 0
	else if ( ( port = atoi( lpszCmdLine ) ) <= 1024 )
	{
	    fprintf( stderr, "Port number must be above 1024.\n" );
	    return 1;
	}
#endif
    }
#else
    port = 4000;
#endif

#ifdef WIN32
	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) {
      MessageBox(0, "No useable WINSOCK.DLL, not loading sockets", "Serv",
	 MB_ICONHAND|MB_OK);

      return INVALID_SOCKET;
      }

   /* Confirm that the Windows Sockets DLL supports 1.1.*/
   /* Note that if the DLL supports versions greater    */
   /* than 1.1 in addition to 1.1, it will still return */
   /* 1.1 in wVersion since that is the version we      */
	/* requested.                                        */

   if ( LOBYTE( wsaData.wVersion ) != 1 ||
	 HIBYTE( wsaData.wVersion ) != 1 ) {
      /* Tell the user that we couldn't find a useable */
      /* winsock.dll.                                  */
      WSACleanup( );
      MessageBox(0, "Windows sockets version not 1.1, not loading sockets", "Serv",
	 MB_ICONHAND|MB_OK);

      return INVALID_SOCKET;
      }

	/* Make sure that the version requested is >= 1.1.   */
   /* The low byte is the major version and the high    */
   /* byte is the minor version.                        */

   if ( LOBYTE( wVersionRequested ) < 1 ||
	( LOBYTE( wVersionRequested ) == 1 &&
	  HIBYTE( wVersionRequested ) < 1 )) {
      MessageBox(0, "Windows sockets version not 1.1, not loading sockets", "Serv",
	 MB_ICONHAND|MB_OK);

       return INVALID_SOCKET;
	}

   /* Since we only support 1.1, set both wVersion and  */
   /* wHighVersion to 1.1.                              */

//   wsaData.wVersion = MAKEWORD( 1, 1 );
//   wsaData.wHighVersion = MAKEWORD( 1, 1 );
#endif

    // Go init this application.
    fpStderr = fopen( STDERR_FILE, "a" );
    InitMerc22(hInstance, nCmdShow);

	 // Load the areas
    log_string( "Loading areas" );
    UpdateWindow(hWndOutput);
    boot_db( );

    // Validate enough of zones loaded to run Merc
    if (help_greeting == 0)
       log_string( "Error: areas.lst did not include help.are.  Game will crash when players log in" );
#ifdef MERCW32
    if (get_obj_index(OBJ_VNUM_ERROR) == 0)
       {
		 log_string( "Error: Object #4 missing (check limbo.are).  MercW32 needs object #4");
		 log_string( "  to prevent crashes when players with invalid equipment log in.");
       log_string( "  check LIMBO.ARE");
       }
#endif
    if (get_obj_index(OBJ_VNUM_MONEY_ONE) == 0 || get_obj_index(OBJ_VNUM_MONEY_SOME) == 0)
       log_string( "Error: Money objects #2 #3 did not load.  Check LIMBO.ARE");
    if (get_obj_index(OBJ_VNUM_CORPSE_NPC) == 0 || get_obj_index(OBJ_VNUM_CORPSE_PC) == 0)
       log_string( "Error: Corpse templates did not load.  Check LIMBO.ARE");
    if (get_obj_index(OBJ_VNUM_SEVERED_HEAD) == 0 || get_obj_index(OBJ_VNUM_TORN_HEART) == 0 ||
	get_obj_index(OBJ_VNUM_SLICED_ARM) == 0 || get_obj_index(OBJ_VNUM_SLICED_LEG) == 0 ||
	get_obj_index(OBJ_VNUM_FINAL_TURD) == 0)
		 log_string( "Error: Severed body parts did not load.  Check LIMBO.ARE");
	 if (get_obj_index(OBJ_VNUM_MUSHROOM) == 0 || get_obj_index(OBJ_VNUM_LIGHT_BALL) == 0 ||
	get_obj_index(OBJ_VNUM_SPRING) == 0)
       log_string( "Error: Spell objects did not load.  Check LIMBO.ARE #20 #21 #22");
    if (get_obj_index(OBJ_VNUM_SCHOOL_MACE) == 0 || get_obj_index(OBJ_VNUM_SCHOOL_DAGGER) == 0 ||
	get_obj_index(OBJ_VNUM_SCHOOL_SWORD) == 0 || get_obj_index(OBJ_VNUM_SCHOOL_VEST) == 0 ||
	get_obj_index(OBJ_VNUM_SCHOOL_SHIELD) == 0 || get_obj_index(OBJ_VNUM_SCHOOL_BANNER) == 0)
       log_string( "Error: New player equipment did not load.  Check SCHOOL.ARE");
    if (get_room_index(ROOM_VNUM_LIMBO) == 0)
       log_string( "Error: The Void did not load.  Check LIMBO.ARE");
	 if (get_room_index(ROOM_VNUM_CHAT) == 0)
       log_string( "Error:  Immortal chat-room did not load.  Check HITOWER.ARE");
	 if (get_room_index(ROOM_VNUM_TEMPLE) == 0 || get_room_index(ROOM_VNUM_ALTAR) == 0)
		 log_string( "Error: Midgaard temple and Alter necessary for game.  Check MIDGAARD.ARE");
    if (get_room_index(ROOM_VNUM_SCHOOL) == 0)
       log_string( "Error: New player initial room did not load.  Check SCHOOL.ARE" );

    // #define MOB_VNUM_CITYGUARD          3060
    // #define MOB_VNUM_VAMPIRE    3404
    // #define MOB_VNUM_WALKING_DEAD 20

    // Initialize sockets
    log_string( "Initializing sockets" );
	 UpdateWindow(hWndOutput);
	 control = init_socket( port );

    // Tell Winsock we are listening for accepted sockets
    WSAAsyncSelect(control, hQryDlgBox, WM_NET_ACCEPT, FD_ACCEPT);

	 sprintf( log_buf, "Merc is ready to rock on port %d.", port );
    log_string( log_buf );
	 game_loop_win32(hInstance, hPrev, nCmdShow);
    closesocket( control );

	 /*
	  * That's all, folks.
	  */
	 log_string( "Normal termination of game." );
	 fclose(fpStderr);

#ifdef WIN32
	 WSACleanup( );
#endif

	 return 0;
}



void ansi_color( const char *txt, CHAR_DATA *ch )
{
	 if ( txt != NULL && ch->desc != NULL )
	 {
	  if ( !IS_SET(ch->act,PLR_ANSI) && !IS_SET(ch->act,PLR_VT100) ) return;
		else
		if ( IS_SET(ch->act, PLR_VT100) && !IS_SET( ch->act, PLR_ANSI ) )
		{
			if ( !str_cmp(txt, GREEN )
			  || !str_cmp(txt, RED )
			  || !str_cmp(txt, BLUE )
			  || !str_cmp(txt, BLACK )
			  || !str_cmp(txt, CYAN )
			  || !str_cmp(txt, GREY )
			  || !str_cmp(txt, YELLOW )
			  || !str_cmp(txt, PURPLE ) ) return;
		}
	  write_to_buffer( ch->desc, txt, strlen(txt) );
	  return;
	 }
}


/*
 * The primary output interface for echoing to characters.
 */
void global( char *buf, int level, int toggler, int toggler2 )
{
	 DESCRIPTOR_DATA *d;

	 for ( d = descriptor_list; d != NULL; d = d->next )
	 {
		if ( d->connected == CON_PLAYING
		  && get_trust( d->character ) >= level
		  && IS_SET(d->character->act, toggler)
		  && IS_SET(d->character->act, toggler2) )
		{
			send_to_char( buf, d->character );
			send_to_char( "\n\r", d->character );
		}
	 }
}

void set_char_color( sh_int AType, CHAR_DATA *ch )
{
	 char buf[32];

	 if ( !IS_NPC(ch) && IS_SET(ch->act,PLR_ANSI) && ch->desc )
	 {
	if (AType>15)
	  sprintf( buf,"\033[0;5;%d;%dm",(((AType-16) & 8)==8),30+((AType-16) & 7) );
	else
	  sprintf( buf,"\033[0;%d;%dm",((AType & 8)==8),30+(AType & 7) );
	write_to_buffer( ch->desc, buf, strlen(buf) );
	 }
	 return;
}

/*
 * The primary output interface for formatted output.
 */
void act(sh_int AType, const char *format, CHAR_DATA *ch, const void *arg1, const void *arg2, int type )
{
	 static char * const he_she     [] = { "it",  "he",  "she" };
	 static char * const him_her    [] = { "it",  "him", "her" };
	 static char * const his_her    [] = { "its", "his", "her" };

	 char buf[MAX_STRING_LENGTH];
	 char buf2[MAX_STRING_LENGTH];
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
	 if ( !format || format[0] == '\0' )
	return;

	 if ( !ch )
	 {
	bug( "Act: null ch.", 0 );
	sprintf( buf, "(%s)", format );
	bug( buf, 0 );
	return;
	 }

	 if ( !ch->in_room )
		to = NULL;
	 else
		to = ch->in_room->people;

	 if ( type == TO_VICT )
	 {
	if ( !vch )
	{
		 bug( "Act: null vch with TO_VICT.", 0 );
		 sprintf( buf, "%s (%s)", ch->name, format );
		 bug( buf, 0 );
		 return;
	}
	if ( !vch->in_room )
	{
		 bug( "Act: vch in NULL room!", 0 );
		 sprintf( buf, "%s -> %s (%s)", ch->name, vch->name, format );
		 bug( buf, 0 );
		 return;
	}
	to = vch->in_room->people;
	 }

	 for ( ; to; to = to->next_in_room )
	 {
	if ( !to->desc || !IS_AWAKE(to) )
		 continue;

	if ( type == TO_CHAR && to != ch )
		 continue;
	if ( type == TO_VICT && ( to != vch || to == ch ) )
		 continue;
	if ( type == TO_ROOM && to == ch )
		 continue;
	if ( type == TO_NOTVICT && (to == ch || to == vch) )
		 continue;

	point   = buf;
	str     = format;
	while ( *str != '\0' )
	{
		 if ( *str != '$' )
		 {
		*point++ = *str++;
		continue;
		 }
		 ++str;

		 if ( !arg2 && *str >= 'A' && *str <= 'Z' )
		 {
		bug( "Act: missing arg2 for code %d:", *str );
		bug( format, 0);
		i = " <@@@> ";
		 }
		 else
		 {
		switch ( *str )
		{
		default:  bug( "Act: bad code %d.", *str );
			  i = " <@@@> ";                                break;
		/* Thx alex for 't' idea */
		case 't': i = (char *) arg1;                            break;
		case 'T': i = (char *) arg2;                            break;
		case 'n': i = PERS( ch,  to  );                         break;
		case 'N': i = PERS( vch, to  );                         break;
		case 'e': i = he_she  [URANGE(0, ch  ->sex, 2)];        break;
		case 'E': i = he_she  [URANGE(0, vch ->sex, 2)];        break;
		case 'm': i = him_her [URANGE(0, ch  ->sex, 2)];        break;
		case 'M': i = him_her [URANGE(0, vch ->sex, 2)];        break;
		case 's': i = his_her [URANGE(0, ch  ->sex, 2)];        break;
		case 'S': i = his_her [URANGE(0, vch ->sex, 2)];        break;
		case 'q': i = (type == TO_CHAR) ? "" : "s";             break;

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
			 if ( !arg2 || ((char *) arg2)[0] == '\0' )
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
	*point++ = '\0';
	buf[0]   = UPPER(buf[0]);
	if (IS_SET(to->act,PLR_ANSI))
	{
	  if (AType>15)
		 sprintf(buf2,"\033[0;5;%d;%dm",(((AType-16) & 8)==8),30+((AType-16) & 7));
	  else
		 sprintf(buf2,"\033[0;%d;%dm",((AType & 8)==8),30+(AType & 7));
	  strcat(buf2, buf);
	}
	else
	  strcpy(buf2, buf);
	if (to->desc)
		write_to_buffer( to->desc, buf2, strlen(buf2) );
	if (MOBtrigger)
		mprog_act_trigger( buf, to, ch, obj1, vch );
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
	// @@@
//   else if (length < strlen(txt))
//      MessageBox(0, "Length Error", "Merc22Sv", MB_ICONHAND|MB_OK);

	 /*
	  * Initial \n\r if needed.
	  */
	 if ( d->outtop == 0 && !d->fcommand )
	 {
	d->outbuf[0]    = '\n';
	d->outbuf[1]    = '\r';
	d->outtop       = 2;
	 }

	 /*
	  * Expand the buffer as needed.
	  */
	 while ( d->outtop + length >= d->outsize )
	 {
	char *outbuf;

	outbuf      = alloc_mem( 2 * d->outsize );
#if 1 // @@@ Deal with too large a memory increment...
	if (!outbuf)
		{
		length = d->outsize - d->outtop - 1;
		break;
		}
#endif
	strncpy( outbuf, d->outbuf, d->outtop );
	free_mem( d->outbuf, d->outsize );
	d->outbuf   = outbuf;
	d->outsize *= 2;
	 }

	 /*
	  * Copy.
	  */
//@@@ Ack!    strcpy( d->outbuf + d->outtop, txt );
#if 1
	 if (length > 0)
		 {
#endif
	 memcpy(d->outbuf + d->outtop, txt, length);
	 d->outtop += length;
#if 1
		 }
#endif

#ifdef WIN32
	 // Tell Winsock to wake us when the socket becomes ready
	 if (!BlastedTrumpet(d->descriptor))
		 {
		 if ( d->character != NULL )
			 save_char_obj( d->character );
		 d->outtop      = 0;
		 close_socket( d );
		 }
#endif

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
	sprintf( log_buf, "Log: Closing link to %s.", ch->name );
	NOTIFY( log_buf, LEVEL_IMMORTAL, WIZ_NOTIFY_LOG );
	log_string( log_buf );
	sprintf( log_buf, "Monitor: Closing link to %s.", ch->name);
	NOTIFY( log_buf, LEVEL_IMMORTAL, WIZ_NOTIFY_MONITOR );
	if ( dclose->connected == CON_PLAYING )
	{
		 act( AT_CYAN, "$n has lost $s link.", ch, NULL, NULL, TO_ROOM);
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

	 closesocket( dclose->descriptor );
	 free_string( dclose->host );
	 dclose->next   = descriptor_free;
	 descriptor_free        = dclose;
	 nPlayers--;

	 return;
}

// Write_to_descriptor bypasses the whole buffered output system.  Blech!
bool write_to_descriptor( int desc, char *txt, int length )
{
	int iStart;
	int nWrite;
	int nBlock;

	if ( length <= 0 )
		length = strlen(txt);

	for ( iStart = 0; iStart < length; iStart += nWrite )
		{
		nBlock = UMIN( length - iStart, 4096 );
		// @@@ Should check WSAEWOULDBLOCK, and we don't want MessageBox...
		if ( ( nWrite = send( desc, txt + iStart, nBlock, 0 ) ) == SOCKET_ERROR ||
			nWrite != nBlock)
			{
			log_string( "Write_to_descriptor" );
			return FALSE;
			}
		}

#ifdef WIN32
	 // Tell Winsock to wake us when the socket becomes ready
	 BlastedTrumpet(desc);
#endif

	 return TRUE;
}



/*
 * Deal with sockets that haven't logged in yet.
 */
void nanny( DESCRIPTOR_DATA *d, char *argument )
{
    DESCRIPTOR_DATA *d_old, *d_next;
	 char buf[MAX_STRING_LENGTH];
	 CHAR_DATA *ch;
	 NOTE_DATA *pnote;
	 char *pwdnew;
	 char *p;
	 int iClass;
	 int iRace;
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
		 sprintf( log_buf, "Log: Denying access to %s@%s.", argument, d->host );
		 NOTIFY( log_buf, LEVEL_IMMORTAL, WIZ_NOTIFY_LOG );
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
//@@@   break;

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

	sprintf( log_buf, "Log: %s@%s has connected.", ch->name, d->host );
	NOTIFY( log_buf, LEVEL_IMMORTAL, WIZ_NOTIFY_LOG );
	sprintf( log_buf, "Monitor: %s@%s has connected.", ch->name, d->host );
	NOTIFY( log_buf, LEVEL_IMMORTAL, WIZ_NOTIFY_MONITOR );
	log_string( log_buf );
	lines = ch->pcdata->pagelen;
	ch->pcdata->pagelen = 20;
	if ( IS_HERO(ch) )
		 do_help( ch, "imotd" );
	do_help( ch, "motd" );
	ch->pcdata->pagelen = lines;
	d->connected = CON_READ_MOTD;
	break;

	/* RT code for breaking link */

	 case CON_BREAK_CONNECT:
	switch( *argument )
	{
	case 'y' : case 'Y':
				for ( d_old = descriptor_list; d_old != NULL; d_old = d_next )
		 {
		d_next = d_old->next;
		if (d_old == d || d_old->character == NULL)
			 continue;

		if (str_cmp(ch->name,d_old->character->name))
			 continue;

		close_socket(d_old);
		 }
		 if (check_reconnect(d,ch->name,TRUE))
			return;
		 write_to_buffer(d,"Reconnect attempt failed.\n\rName: ",0);
				if ( d->character != NULL )
				{
					 free_char( d->character );
					 d->character = NULL;
				}
		 d->connected = CON_GET_NAME;
		 break;
	  }

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
	ch->pcdata->pwd = str_dup( pwdnew );
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
	write_to_buffer( d, "Do you want ANSI color? (Y/N)? ", 0 );
	d->connected = CON_GET_NEW_COLOR;
	break;

	 case CON_GET_NEW_COLOR:
	switch ( argument[0] )
	{
	case 'y': case 'Y': ch->act = PLR_ANSI;    break;
	case 'n': case 'N': ch->act = PLR_ANSI; break;
	default:
		 write_to_buffer( d, "Please choose yes or no\n\rDo you want color? ", 0 );
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

#ifdef RACES
	write_to_buffer( d, "      Race       Code\n\r\n\r", 0);
	write_to_buffer( d, "      Human      Hum\n\r", 0);
	write_to_buffer( d, "      Dwarf      Dwa\n\r", 0);
	write_to_buffer( d, "      Half-Ogre  Hor\n\r", 0);
	write_to_buffer( d, "      Grey Elf   Elf\n\r", 0);
	write_to_buffer( d, "      Orc        Orc\n\r", 0);
	write_to_buffer( d, "      Drow Elf   Dro\n\r", 0);
	write_to_buffer( d, "      Troll      Tro\n\r", 0);
	write_to_buffer( d, "      Half-Elf   Hel\n\r", 0);
	write_to_buffer( d, "      Pixie      Pix\n\r", 0);
	write_to_buffer( d, "      Halfling   Haf\n\r", 0);
	write_to_buffer( d, "      Gnome      Gno\n\r", 0);
	write_to_buffer( d, "      Giant      Gia\n\r\n\r\n\r", 0);
	strcpy( buf, "Select a race [" );
	for ( iRace = 0; iRace < MAX_RACE; iRace++ )
	{
		 if ( iRace > 0 )
		strcat( buf, " " );
		 strcat( buf, race_table[iRace].who_name );
	}
	strcat( buf, "]: " );
	write_to_buffer( d, buf, 0 );
	d->connected = CON_GET_NEW_RACE;
	break;

	 case CON_GET_NEW_RACE:
	for ( iRace = 0; iRace < MAX_RACE; iRace++ )
	{
		 if ( !str_cmp( argument, race_table[iRace].who_name ) )
		 {
		ch->race = iRace;
		break;
		 }
	}

	if ( iRace == MAX_RACE )
	{
		 write_to_buffer( d,
		"That's not a race.\n\rWhat IS your race? ", 0 );
		 return;
	}
#endif
	write_to_buffer( d, "      Class      Code\n\r\n\r", 0);
	write_to_buffer( d, "      Mage        Mag\n\r", 0);
	write_to_buffer( d, "      Cleric      Cle\n\r", 0);
	write_to_buffer( d, "      Thief       Thi\n\r", 0);
	write_to_buffer( d, "      Warrior     War\n\r", 0);
	write_to_buffer( d, "      Psionicist  Psi\n\r", 0);
	write_to_buffer( d, "      Ranger      Ran\n\r\n\r\n\r", 0);
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

	write_to_buffer( d, "\n\r", 2 );
	ch->pcdata->pagelen = 20;
	ch->prompt = "<%hhp %mm %vmv> ";
	SET_BIT    (ch->act, PLR_PROMPT);
	do_help( ch, "motd" );
	d->connected = CON_READ_MOTD;
	break;

	 case CON_READ_MOTD:
	ch->next        = char_list;
	char_list       = ch;
	d->connected    = CON_PLAYING;

	send_to_char(
	 "\n\rA strange voice tells you, 'Welcome to Orodruin'.\n\r",
		 ch );

	if ( ch->level == 0 )
	{
		 OBJ_DATA *obj;

		 switch ( class_table[ch->class].attr_prime )
		 {
		 case APPLY_STR: ch->pcdata->perm_str = 14; break;
		 case APPLY_INT: ch->pcdata->perm_int = 14; break;
		 case APPLY_WIS: ch->pcdata->perm_wis = 14; break;
		 case APPLY_DEX: ch->pcdata->perm_dex = 14; break;
		 case APPLY_CON: ch->pcdata->perm_con = 14; break;
		 }

#ifdef RACES
		 ch->pcdata->perm_str += race_table[ch->race].mod_str;
		 ch->pcdata->perm_int += race_table[ch->race].mod_int;
		 ch->pcdata->perm_wis += race_table[ch->race].mod_wis;
		 ch->pcdata->perm_dex += race_table[ch->race].mod_dex;
		 ch->pcdata->perm_con += race_table[ch->race].mod_con;
#endif

		 ch->level      = 1;
		 ch->exp     = 1000;
		 ch->hit     = ch->max_hit;
		 ch->mana    = ch->max_mana;
		 ch->move    = ch->max_move;
#ifdef RACES
		 ch->affected_by = race_table[ch->race].bitvector;
#else
		 ch->affected_by = 0;
#endif
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
#ifdef RACES
		 char_to_room( ch, get_room_index( race_table[ch->race].city_temple ) );
#else
		 char_to_room( ch, get_room_index( ROOM_VNUM_TEMPLE ) );
#endif
	}

		sprintf( log_buf, "Log: %s@%s new player.", ch->name, d->host );
		log_string( log_buf );
		NOTIFY( log_buf, LEVEL_IMMORTAL, WIZ_NOTIFY_LOG );
		sprintf( log_buf, "Monitor: %s@%s new player.", ch->name, d->host );
	act(AT_CYAN,  "$n has entered the game.", ch, NULL, NULL, TO_ROOM);
	do_look( ch, "auto" );
	/* check for new notes */
	notes = 0;

	for ( pnote = note_free; pnote != NULL; pnote = pnote->next )
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
		ch->desc         = d;
		ch->timer        = 0;
		send_to_char( "Reconnecting.\n\r", ch );
		act(AT_CYAN, "$n has reconnected.", ch, NULL, NULL, TO_ROOM);
		sprintf( log_buf, "Log: %s@%s reconnected.", ch->name, d->host );
		log_string( log_buf );
		NOTIFY( log_buf, LEVEL_IMMORTAL, WIZ_NOTIFY_LOG );
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
	    write_to_buffer( d, "That character is already playing.\n\r",0);
	    write_to_buffer( d, "Do you wish to connect anyway (Y/N)?",0);
	    d->connected = CON_BREAK_CONNECT;
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
	 ch->was_in_room        = NULL;
	 act(AT_CYAN, "$n has returned from the void.", ch, NULL, NULL, TO_ROOM);
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
		if ( d->pString )
		 write_to_buffer( d, "] ", 2 );
	else if ( d->showstr_point )
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
//              bust_a_prompt( ch );
			  bust_a_prompt( d );

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

#if 0
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
#endif
	// Wake up Winsock, if necessary
	 if (!BlastedTrumpet(d->descriptor))
		 {
		 if ( d->character != NULL )
			 save_char_obj( d->character );
		 d->outtop      = 0;
		 close_socket( d );
		 return FALSE;
		 }
	return TRUE;
}

/*
 * Bust a prompt (player settable prompt)
 * coded by Morgenes for Aldara Mud
 */
//void bust_a_prompt( CHAR_DATA *ch )
void bust_a_prompt( DESCRIPTOR_DATA *d )
{
	char buf[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	const char *str;
	const char *i;
	char *point;
	CHAR_DATA *ch;
	CHAR_DATA *victim;

	ch = d->character;

	if( ch->prompt == NULL || ch->prompt[0] == '\0' )
	{
		send_to_char( "\n\r\n\r", ch );
		return;
	}

	point = buf;
 //  str = ch->prompt;
	str = d->original != NULL ? d->original->prompt :  /* VERY VERY NEW */
			d->character->prompt;
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
   
	/* battle prompt */
	if ((victim = ch->fighting) != NULL)

	  {
	    int percent;
	    char wound[100];
	    char buf[MAX_STRING_LENGTH];

	    write_to_buffer( d, "\n\r", 0);
		if (victim->max_hit > 0)
		percent = victim->hit * 100 / victim->max_hit;
	    else
		percent = -1;
 
	    if (percent >= 100)
					 sprintf(wound,"is in excellent condition.");
	    else if (percent >= 90)
		sprintf(wound,"has a few scratches.");
	    else if (percent >= 75)
					 sprintf(wound,"has some small wounds and bruises.");
	    else if (percent >= 50)
		sprintf(wound,"has quite a few wounds.");
	    else if (percent >= 30)
		sprintf(wound,"has some big nasty wounds and scratches.");
	    else if (percent >= 15)
		sprintf(wound,"looks pretty hurt.");
	    else if (percent >= 0)
		sprintf(wound,"is in awful condition.");
				else
		sprintf(wound,"is bleeding to death.");
 
				sprintf(buf,"%s %s \n\r",
		    IS_NPC(victim) ? victim->short_descr : victim->name,wound);
	    buf[0] = UPPER(buf[0]);
	    write_to_buffer( d, buf, 0);
	  }

	ansi_color( NTEXT, ch);
	ansi_color( GREY, ch);
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
  int lines /*= 0*/, toggle=1;

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
	 // @@@ Sands had some changes here that lead to double freeing
    if ( d->showstr_head )
    {
      free_string( d->showstr_head );
//      d->showstr_head = str_dup( "" );
      d->showstr_head = 0;
    }
//    free_string( d->showstr_point );
//    d->showstr_point = str_dup( "" );
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

SOCKET init_socket( int port )
{
   // Assume there aren't any sockets
//@@@   fSockets = FALSE;
    static struct sockaddr_in sa_zero;
	 struct sockaddr_in sa;
	 SOCKET newsock;
	 int x = 1;

#if 0
	WORD wVersionRequested = MAKEWORD( 1, 1 );

	WSADATA wsaData;
	int err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) {
		MessageBox(0, "No useable WINSOCK.DLL, not loading sockets", "Serv",
			MB_ICONHAND|MB_OK);

		return INVALID_SOCKET;
		}

	/* Confirm that the Windows Sockets DLL supports 1.1.*/
	/* Note that if the DLL supports versions greater    */
	/* than 1.1 in addition to 1.1, it will still return */
	/* 1.1 in wVersion since that is the version we      */
	/* requested.                                        */

	if ( LOBYTE( wsaData.wVersion ) != 1 ||
			HIBYTE( wsaData.wVersion ) != 1 ) {
		/* Tell the user that we couldn't find a useable */
		/* winsock.dll.                                  */
		WSACleanup( );
		MessageBox(0, "Windows sockets version not 1.1, not loading sockets", "Serv",
			MB_ICONHAND|MB_OK);

		return INVALID_SOCKET;
		}

	/* Make sure that the version requested is >= 1.1.   */
	/* The low byte is the major version and the high    */
	/* byte is the minor version.                        */

	if ( LOBYTE( wVersionRequested ) < 1 ||
		  ( LOBYTE( wVersionRequested ) == 1 &&
			 HIBYTE( wVersionRequested ) < 1 )) {
		MessageBox(0, "Windows sockets version not 1.1, not loading sockets", "Serv",
			MB_ICONHAND|MB_OK);

		 return INVALID_SOCKET;
	}

	/* Since we only support 1.1, set both wVersion and  */
	/* wHighVersion to 1.1.                              */

	wsaData.wVersion = MAKEWORD( 1, 1 );
	wsaData.wHighVersion = MAKEWORD( 1, 1 );
#endif
//   fSockets = TRUE;


	 if ( ( newsock = socket( PF_INET, SOCK_STREAM, 0 ) ) == INVALID_SOCKET )
		 {
		 MessageBox(0, "Init_socket: socket", __FILE__, MB_OK);
		 return INVALID_SOCKET;
		 }

	 if ( setsockopt( newsock, SOL_SOCKET, SO_REUSEADDR, (char *) &x, sizeof(x) )
				 == SOCKET_ERROR )
		 {
		 MessageBox(0, "Init_socket: SO_REUSEADDR", __FILE__, MB_OK);
		 closesocket(newsock);
		 return INVALID_SOCKET;
		 }

#if 0
	 {
	struct  linger  ld;

	ld.l_onoff  = 1;
	ld.l_linger = 1000;

	if ( setsockopt( fd, SOL_SOCKET, SO_DONTLINGER, (char *) &ld, sizeof(ld) ) < 0 )
	{
		 MessageBox(0, 0, "Init_socket: SO_DONTLINGER", __FILE__, MB_OK );
		 closesocket( fd );
		 return INVALID_SOCKET;
	}
	 }
#endif

	 sa                 = sa_zero;
	 sa.sin_family   = AF_INET;
	 sa.sin_port        = htons( port );

	 if ( bind( newsock, (struct sockaddr *) &sa, sizeof(sa) ) == SOCKET_ERROR )
	 {
	MessageBox(0, "Init_socket: bind", __FILE__, MB_OK );
	closesocket( newsock );
	return INVALID_SOCKET;
	 }

	 if ( listen( newsock, 3 ) == SOCKET_ERROR )
	 {
	MessageBox(0, "Init_socket: listen", __FILE__, MB_OK );
	closesocket( newsock );
	return INVALID_SOCKET;
	 }

	 return newsock;
}

LRESULT SocketAccept (WPARAM wParam, LPARAM lParam)
{
	 static DESCRIPTOR_DATA d_zero;
		SOCKET  sSocket;
		SOCKADDR_IN     saPeer;
		int             iAddrSize;
//         int          iError;
		char     buf[512];
			DESCRIPTOR_DATA *dnew;

	switch(WSAGETSELECTEVENT(lParam))
		{
		default:
			MessageBeep(-1);
			break;

		case FD_ACCEPT:
			{
//                      LPSOCKADDR_IN   lpsaHostAddr;
//         LPHOSTENT       lpheHostEnt;

//              struct sockaddr_in isa;
//              int i = sizeof(isa);
//              getsockname(s, &isa, &i);
			struct sockaddr_in sock;
			int size = sizeof(sock);

//         MessageBeep(-1);

			/* Get a pending accept */
			iAddrSize = sizeof(SOCKADDR_IN);
			sSocket = accept( control, (LPSOCKADDR) &saPeer, (LPINT) &iAddrSize );
			if ( sSocket == INVALID_SOCKET )
				{
//            MessageBox(0, "Couldn't accept() connection.", "Serv",
//               MB_ICONHAND|MB_OK);
				log_string("Couldn't accept() connection");
				return FALSE;
				}

			// Cons a new descriptor.
			if ( descriptor_free )
				{
				dnew = descriptor_free;
				descriptor_free = descriptor_free->next;
				dnew->descriptor = sSocket;
				}
			else
				dnew = alloc_perm( sizeof(*dnew) );

			*dnew           = d_zero;
			dnew->descriptor        = sSocket;
			dnew->connected = CON_GET_NAME;
			dnew->showstr_head  = NULL;
			dnew->showstr_point = NULL;
			dnew->outsize   = 2000;
			dnew->outbuf    = alloc_mem( dnew->outsize );
			dnew->host_and_name = alloc_mem( sizeof *dnew->host_and_name );
			strcpy(dnew->host_and_name->username, "<waiting>");

			if ( getpeername( sSocket, (struct sockaddr *) &sock, &size ) == SOCKET_ERROR )
				{
#if 1
				wsprintf(buf, "Log: new_descriptor(): getpeername() == %d",
					WSAGetLastError());
				log_string( buf );
			#else
				log_string("New_descriptor: getpeername");
#endif
				dnew->host = str_dup( "(unknown)" );
				}
			else
				{
				int ulen = sizeof dnew->host_and_name->us;

				// Get the caller's machine in quad format
				u_long addr = ntohl( sock.sin_addr.s_addr );
				struct hostent *from;
				wsprintf( buf, "%d.%d.%d.%d",
					( addr >> 24 ) & 0xFF, ( addr >> 16 ) & 0xFF,
					( addr >>  8 ) & 0xFF, ( addr       ) & 0xFF);
				wsprintf( log_buf, "Log: Sock.sinaddr:  %s", buf );
				NOTIFY( log_buf, LEVEL_IMMORTAL, WIZ_NOTIFY_LOG );
				log_string( log_buf );

				// Request the name of the caller's machine from DNS
				dnew->host = str_dup( buf );
				dnew->host_and_name->sin_addr = sock.sin_addr;
				dnew->host_and_name->hRequestHandle = WSAAsyncGetHostByAddr( hQryDlgBox,
					WM_NET_GETHOST, (LPSTR) &dnew->host_and_name->sin_addr, sizeof dnew->host_and_name->sin_addr,
					PF_INET, dnew->host_and_name->hostdata, sizeof dnew->host_and_name->hostdata);

				// Look up the local name for this socket
				if (getsockname(sSocket, (struct sockaddr *)&dnew->host_and_name->us, &ulen) ==
																					  SOCKET_ERROR)
					{
					wsprintf(dnew->host_and_name->username, "(getsockname)#%d", WSAGetLastError());
					goto failure;
					}

				// Create a socket to connect to the user's ident port
				if ((dnew->host_and_name->sAuth = socket(PF_INET, SOCK_STREAM, AF_UNSPEC)) ==
																					INVALID_SOCKET)
					{
					wsprintf(dnew->host_and_name->username, "(socket)#%d", WSAGetLastError());
					goto failure;
					}

				// Save the address of their side of the socket
				dnew->host_and_name->them = sock;

				// htons convers a u_short from host to network byte order. 113 is
				//  the auth socket
				dnew->host_and_name->authsock.sin_port = htons(113);
				// Use Internet protocol
				dnew->host_and_name->authsock.sin_family = AF_INET;
				// Look up the other end of our socket
				dnew->host_and_name->authsock.sin_addr = sock.sin_addr;

				WSAAsyncSelect(dnew->host_and_name->sAuth, hQryDlgBox, WM_NET_AUTHCONNECT,
					FD_CONNECT|FD_READ|FD_WRITE);
				if (connect(dnew->host_and_name->sAuth, (struct sockaddr *)&dnew->host_and_name->authsock,
													sizeof(dnew->host_and_name->authsock)) == SOCKET_ERROR)
					{
					int nError = WSAGetLastError();
					if (nError != WSAEWOULDBLOCK)
						{
						wsprintf(dnew->host_and_name->username, "(connect)#%d", nError);
						goto failure;
						}
					}

				}
failure:

			// Init descriptor data.
			dnew->next      = descriptor_list;
			descriptor_list = dnew;
			nPlayers++;

			// OK now get messages from this socket
			BlastedTrumpet(sSocket);

			// Send the greeting.
			{
			extern char * help_greeting;
			if ( help_greeting[0] == '.' )
			 write_to_buffer( dnew, help_greeting+1, 0 );
			else
			 write_to_buffer( dnew, help_greeting  , 0 );
			}
			} // FD_ACCEPT
		}

	return FALSE;
}

LRESULT SocketReadWrite (WPARAM wParam, LPARAM lParam)
{
	SOCKET  sSocket = (SOCKET) wParam;
//      SOCKADDR_IN     saPeer;
//      int             iAddrSize;
//      int             iError;
//   char     szBuffer[64];
//   int      len;

	switch(WSAGETSELECTEVENT(lParam))
		{
		default:
			MessageBeep(-1);
			break;

		case FD_READ:   // WinSock has something for us
			{
			// Find the descriptor for this FD_READ
			DESCRIPTOR_DATA *d;
			for (d = descriptor_list; d; d = d_next )
				{
				d_next = d->next;

				if (d->descriptor == sSocket)
					{
					d->fcommand     = FALSE;

					if ( d->character != NULL )
						d->character->timer = 0;

					if ( !read_from_descriptor( d ) )
						{
//@@@Dunno what this does?                  FD_CLR( d->descriptor, &out_set );
						if ( d->character != NULL )
							save_char_obj( d->character );
						d->outtop       = 0;
						close_socket( d );
//@@@                  continue;
						}

					break;
					}
				}
			} // FD_READ
			break;

		case FD_WRITE:  // WinSock ready to send data on this socket
			{
#if 0
			if (!BlastedTrumpet(sSocket))
				return FALSE;
#endif
#if 0
			send(sSocket, szMessage, strlen(szMessage), 0);
#else
			// Output.
			DESCRIPTOR_DATA *d;
			for (d = descriptor_list; d != NULL; d = d_next )
				{
				d_next = d->next;
				if (d->descriptor == sSocket)
					if ( d->fcommand || d->outtop > 0 )
						{
#if 0
						if ( !process_output( d, TRUE ) && WSAGetLastError() != WSAEWOULDBLOCK )
							{
							if ( d->character != NULL )
								save_char_obj( d->character );
							d->outtop       = 0;
							close_socket( d );
							}
#endif
						int iStart, nWrite;
						for ( iStart = 0; iStart < d->outtop; iStart += nWrite )
							{
							int nBlock = UMIN( d->outtop - iStart, 4096 );
							if ( ( nWrite = send( d->descriptor, d->outbuf + iStart, nBlock, 0 ) ) == SOCKET_ERROR)
								{
								if (WSAGetLastError() != WSAEWOULDBLOCK)
									{
									log_string( "Write_to_descriptor" );
									if ( d->character != NULL )
										save_char_obj( d->character );
									close_socket( d );
									}

								break;
								}
							}

						// @@@ It would be better at this point to resend if we got
						// EWOULDBLOCK, but we throw all data away...
						d->outtop = 0;
						}
				}
#endif
			} // FD_WRITE
			break;

		case FD_CLOSE:  // Hey, it closed on us...
//          MessageBox(0, "Closing", "Serv", MB_ICONINFORMATION|MB_OK);
//          log_string("Closing");
			 break;

		}

	return FALSE;
}

bool BlastedTrumpet(SOCKET sSocket)
{
	if ( WSAAsyncSelect( sSocket, hQryDlgBox, WM_NET_READWRITE,
		FD_READ|FD_WRITE|FD_CLOSE) == SOCKET_ERROR)
		{
//              MessageBox(0, "Couldn't select() on client socket.", "Serv",
//         MB_ICONHAND|MB_OK);
		log_string("Couldn't select() on client socket.");
		return FALSE;
		}

	return TRUE;
}

bool read_from_descriptor( DESCRIPTOR_DATA *d )
{
    int iStart;

	 /* Hold horses if pending command already. */
	 if ( d->incomm[0] != '\0' )
		 {
		 if (!BlastedTrumpet(d->descriptor))
			 {
			 if ( d->character != NULL )
				 save_char_obj( d->character );
			 d->outtop      = 0;
			 close_socket( d );
			 }
//       BlastedTrumpet(d->descriptor);
		 return TRUE;
		 }

	 /* Check for overflow. */
	 iStart = strlen(d->inbuf);
	 if ( iStart >= sizeof(d->inbuf) - 10 )
	 {
	wsprintf( log_buf, "Log: %s input overflow!", d->host );
	NOTIFY( log_buf, LEVEL_IMMORTAL, WIZ_NOTIFY_LOG );
	log_string( log_buf );
	BlastedTrumpet(d->descriptor);
//@@@   MessageBox(0, "input overflow!", "Serv", MB_ICONHAND|MB_OK);
	write_to_descriptor( d->descriptor,
		 "\n\r*** PUT A LID ON IT!!! ***\n\r", 0 );
	return FALSE;
	 }

	 /* Snarf input. */
	 for ( ; ; )
	 {
	int nRead;

	nRead = recv( d->descriptor, d->inbuf + iStart,
		 sizeof(d->inbuf) - 10 - iStart, 0 );
	if ( nRead > 0 )
	{
		 iStart += nRead;
		 if ( d->inbuf[iStart-1] == '\n' || d->inbuf[iStart-1] == '\r' )
		break;
	}
	else if ( nRead == 0 )
	{
//          log_string( "EOF encountered on read." );
		 return FALSE;
	}
	else if ( WSAGetLastError() == WSAEWOULDBLOCK )
		 break;
	else
	{
//          perror( "Read_from_descriptor" );
//               MessageBox(0, "Read_from_descriptor", "Serv", MB_ICONHAND|MB_OK);
		 log_string("read_from_descriptor() got error");
		 return FALSE;
	}
	 }

	 d->inbuf[iStart] = '\0';
	 return TRUE;
}

/*
 * Writes a string to the log.
 */
void log_string( const char *str )
{
	char *strtime;

	if (IsWindow(hWndOutput))
		SendMessage(hWndOutput, LB_ADDSTRING, 0, (LPARAM)(LPCSTR) str);

	strtime                    = ctime( &current_time );
	strtime[strlen(strtime)-1] = '\0';
	fprintf( fpStderr, "%s :: %s\n", strtime, str );
}

LRESULT SocketGetHost(WPARAM wParam, LPARAM lParam)
{
	DESCRIPTOR_DATA *des = descriptor_list;
	WORD wError;

//   MessageBeep(-1);

	while (des && des->host_and_name->hRequestHandle != (HANDLE) wParam)
		des = des->next;

	if (!des)
		{
		log_string("Got Host info for unknown descriptor");
		return FALSE;
		}

	wError = WSAGETASYNCERROR(lParam);
	if (wError)
		{
		char szBuf[512];
		char *p = "(Unknown Error)";

		if (wError == WSAHOST_NOT_FOUND)
			p = "(Authoritative Answer:  Host Not Found)";
		else if (wError == WSATRY_AGAIN)
			p = "(Host not found, or Server Failure)";
		else if (wError == WSANO_RECOVERY)
			p = "(Former, Refused, or Not Implemented)";
		else if (wError == WSANO_DATA)
			p = "(Valid name, no data record)";
		else if (wError == WSANO_DATA)
			p = "(No Address)";

		wsprintf(szBuf, "%s %s", des->host, p);
		free_string( des->host );
		des->host = str_dup( szBuf );
		}
	else
		{
		free_string( des->host );
		des->host = str_dup( ((struct hostent FAR *)(des->host_and_name->hostdata))->h_name );
		}

	return TRUE;
}

LRESULT SocketAuthConnect(WPARAM wParam, LPARAM lParam)
{
	char szIdentRequest[32];
	char szIdentReply[256];
	char szUserID[32];
	u_short port1, port2;
	char *p;

	DESCRIPTOR_DATA *des = descriptor_list;
	while (des && des->host_and_name->sAuth != (SOCKET) wParam)
		des = des->next;

	if (!des)
		{
		log_string("Ident message info for unknown descriptor");
		return FALSE;
		}

	switch(WSAGETSELECTEVENT(lParam))
		{
		default:
			MessageBeep(-1);
			break;

		case FD_CONNECT:
//         log_string("FD_CONNECT to IDENTD port");
//         break;
//
//      case FD_WRITE:

			// Failure?
			if (WSAGETSELECTERROR(lParam))
				{
				wsprintf(des->host_and_name->username, "(FD_CONNECT)#%d", WSAGetLastError());
				closesocket((SOCKET) wParam);
				return FALSE;
				}

			// Create a string of the form <port>, <port>\r\n
			wsprintf(szIdentRequest, "%u , %u\r\n",
					ntohs(des->host_and_name->them.sin_port),
					ntohs(des->host_and_name->us.sin_port));

			send((SOCKET) wParam, szIdentRequest, strlen(szIdentRequest), 0);
			// @@@ Should check return value
			break;

		case FD_READ:
			// Read the response from the IDENT port
			recv((SOCKET) wParam, szIdentReply, sizeof szIdentReply, 0);
			// @@@ Should check return value

			// Extract the user name provided by IDENT
			sscanf(szIdentReply, "%hd , %hd : USERID : %*[^:]: %10s",
				&port1, &port2, szUserID);
			log_string(szIdentReply); // @@@

			// Remove any trailing newline from the user name
			for (p = szUserID; *p && *p != '\n' && *p != '\r'; p++)
				;

			*p = '\0';

			// Save the user name in the descriptor
			strcpy(des->host_and_name->username, szUserID);

			// Close this socket
			closesocket((SOCKET) wParam);

			break;
		}

	return TRUE;
}
/*
 * Write to all characters.
 */
void send_to_all_char( const char *text )
{
	 DESCRIPTOR_DATA *d;

	 if ( !text )
		  return;
	 for ( d = descriptor_list; d; d = d->next )
		  if ( d->connected == CON_PLAYING )
		 send_to_char( text, d->character );

	 return;
}

/*
 * Reports a bug.
 */
void bug( const char *str, int param )
{
	 extern FILE *                  fpArea;
	 extern char                    strArea[ MAX_INPUT_LENGTH   ];

	 FILE *fp;
	 char  buf [ MAX_STRING_LENGTH ];
	 char  buf2[ MAX_STRING_LENGTH ];

	 if ( fpArea )
	 {
	int iLine;
	int iChar;

	if ( fpArea == stdin )
	{
		 iLine = 0;
	}
	else
	{
		 iChar = ftell( fpArea );
		 fseek( fpArea, 0, 0 );
		 for ( iLine = 0; ftell( fpArea ) < iChar; iLine++ )
		 {
		while ( getc( fpArea ) != '\n' )
			 ;
		 }
		 fseek( fpArea, iChar, 0 );
	}

	sprintf( buf2, "Log: [*****] FILE: %s LINE: %d", strArea, iLine );
	log_string( buf2 );
	NOTIFY( buf2, LEVEL_IMMORTAL, WIZ_NOTIFY_LOG );

	if ( ( fp = fopen( "shutdown.txt", "a" ) ) )
	{
		 fprintf( fp, "[*****] %s\n", buf2 );
		 fclose( fp );
	}
	 }
	else
		buf2[0] = 0;

	 strcpy( buf, "Log: [*****] BUG: " );
	 sprintf( buf + strlen( buf ), str, param );
	 NOTIFY( buf, LEVEL_IMMORTAL, WIZ_NOTIFY_LOG );
	 log_string( buf );


	 fclose( fpReserve );
	 if ( ( fp = fopen( BUG_FILE, "a" ) ) )
	 {
	fprintf( fp, "%s\n", buf );
	fclose( fp );
	 }
	 fpReserve = fopen( NULL_FILE, "r" );

	 if (fWantsMessageBox)
		 MessageBox(0, buf, buf2, MB_ICONHAND|MB_OK);

	 return;
}

// This was moved out of act_wiz so that I can use the host_and_name member
void do_users( CHAR_DATA *ch, char *argument )
{
	 char buf[MAX_STRING_LENGTH];
	 char buf2[MAX_STRING_LENGTH];
	 DESCRIPTOR_DATA *d;
	 int count;

	 count  = 0;
	 buf[0] = '\0';
	 buf2[0]     = '\0';
	 for ( d = descriptor_list; d != NULL; d = d->next )
	 {
	if ( d->character != NULL && can_see( ch, d->character ) )
	{
	    count++;
	    sprintf( buf + strlen(buf), "[%3d %2d] %s (%s@%s)\n\r",
		d->descriptor,
		d->connected,
		d->original  ? d->original->name  :
		d->character ? d->character->name : "(none)",
		d->host_and_name->username, d->host
		);
	}
    }

    sprintf( buf2, "%d user%s\n\r", count, count == 1 ? "" : "s" );
	 strcat( buf, buf2 );
    send_to_char( buf, ch );
    return;
}

