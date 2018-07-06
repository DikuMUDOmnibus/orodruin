/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Thanks to abaddon for proof-reading our comm.c and pointing out bugs.  *
 *  Any remaining bugs are, of course, our work, not his.  :)              *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

/*
 * This file contains all of the OS-dependent stuff:
 *   startup, signals, BSD sockets for tcp/ip, i/o, timing.
 *
 * The data flow for input is:
 *    Game_loop ---> Read_from_descriptor ---> Read
 *    Game_loop ---> Read_from_buffer
 *
 * The data flow for output is:
 *    Game_loop ---> Process_Output ---> Write_to_descriptor -> Write
 *
 * The OS-dependent functions are Read_from_descriptor and Write_to_descriptor.
 * -- Furey  26 Jan 1993
 */

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "merc.h"



/*
 * Malloc debugging stuff.
 */
#if defined(sun)
#undef MALLOC_DEBUG
#endif

#if defined(MALLOC_DEBUG)
#include <malloc.h>
extern	int	malloc_debug	args( ( int  ) );
extern	int	malloc_verify	args( ( void ) );
#endif



/*
 * Signal handling.
 * Apollo has a problem with __attribute(atomic) in signal.h,
 *   I dance around it.
 */
#if defined(apollo)
#define __attribute(x)
#endif

#if defined(unix)
#include <signal.h>
#endif

#if defined(apollo)
#undef __attribute
#endif



/*
 * Socket and TCP/IP stuff.
 */
#if	defined(macintosh) || defined(MSDOS)
const	char	echo_off_str	[] = { '\0' };
const	char	echo_on_str	[] = { '\0' };
const	char 	go_ahead_str	[] = { '\0' };
#endif

#if	defined(unix)
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/telnet.h>
const	char	echo_off_str	[] = { IAC, WILL, TELOPT_ECHO, '\0' };
const	char	echo_on_str	[] = { IAC, WONT, TELOPT_ECHO, '\0' };
const   char    go_ahead_str    [] = { IAC, GA, '\0' };
#endif



/*
 * OS-dependent declarations.
 */
#if	defined(_AIX)
#include <sys/select.h>
int	accept		args( ( int s, struct sockaddr *addr, int *addrlen ) );
int	bind		args( ( int s, struct sockaddr *name, int namelen ) );
void	bzero		args( ( char *b, int length ) );
int	getpeername	args( ( int s, struct sockaddr *name, int *namelen ) );
int	getsockname	args( ( int s, struct sockaddr *name, int *namelen ) );
int	gettimeofday	args( ( struct timeval *tp, struct timezone *tzp ) );
int	listen		args( ( int s, int backlog ) );
int	setsockopt	args( ( int s, int level, int optname, void *optval,
			    int optlen ) );
int	socket		args( ( int domain, int type, int protocol ) );
#endif

#if	defined(apollo)
#include <unistd.h>
void	bzero		args( ( char *b, int length ) );
#endif

#if	defined(__hpux)
int	accept		args( ( int s, void *addr, int *addrlen ) );
int	bind		args( ( int s, const void *addr, int addrlen ) );
void	bzero		args( ( char *b, int length ) );
int	getpeername	args( ( int s, void *addr, int *addrlen ) );
int	getsockname	args( ( int s, void *name, int *addrlen ) );
int	gettimeofday	args( ( struct timeval *tp, struct timezone *tzp ) );
int	listen		args( ( int s, int backlog ) );
int	setsockopt	args( ( int s, int level, int optname,
 				const void *optval, int optlen ) );
int	socket		args( ( int domain, int type, int protocol ) );
#endif

#if	defined(interactive)
#include <net/errno.h>
#include <sys/fcntl.h>
#endif

#if	defined(linux)
int	accept		args( ( int s, struct sockaddr *addr, int *addrlen ) );
int	bind		args( ( int s, struct sockaddr *name, int namelen ) );
int	close		args( ( int fd ) );
int	getpeername	args( ( int s, struct sockaddr *name, int *namelen ) );
int	getsockname	args( ( int s, struct sockaddr *name, int *namelen ) );
int	gettimeofday	args( ( struct timeval *tp, struct timezone *tzp ) );
int	listen		args( ( int s, int backlog ) );
int	read		args( ( int fd, char *buf, int nbyte ) );
int	select		args( ( int width, fd_set *readfds, fd_set *writefds,
			    fd_set *exceptfds, struct timeval *timeout ) );
int	socket		args( ( int domain, int type, int protocol ) );
int	write		args( ( int fd, char *buf, int nbyte ) );
#endif

#if	defined(macintosh)
#include <console.h>
#include <fcntl.h>
#include <unix.h>
struct	timeval
{
	time_t	tv_sec;
	time_t	tv_usec;
};
#if	!defined(isascii)
#define	isascii(c)		( (c) < 0200 )
#endif
static	long			theKeys	[4];

int	gettimeofday		args( ( struct timeval *tp, void *tzp ) );
#endif

#if	defined(MIPS_OS)
extern	int		errno;
#endif

#if	defined(MSDOS)
int	gettimeofday	args( ( struct timeval *tp, void *tzp ) );
int	kbhit		args( ( void ) );
#endif

#if	defined(NeXT)
int	close		args( ( int fd ) );
int	fcntl		args( ( int fd, int cmd, int arg ) );
#if	!defined(htons)
u_short	htons		args( ( u_short hostshort ) );
#endif
#if	!defined(ntohl)
u_long	ntohl		args( ( u_long hostlong ) );
#endif
int	read		args( ( int fd, char *buf, int nbyte ) );
int	select		args( ( int width, fd_set *readfds, fd_set *writefds,
			    fd_set *exceptfds, struct timeval *timeout ) );
int	write		args( ( int fd, char *buf, int nbyte ) );
#endif

#if	defined(sequent)
int	accept		args( ( int s, struct sockaddr *addr, int *addrlen ) );
int	bind		args( ( int s, struct sockaddr *name, int namelen ) );
int	close		args( ( int fd ) );
int	fcntl		args( ( int fd, int cmd, int arg ) );
int	getpeername	args( ( int s, struct sockaddr *name, int *namelen ) );
int	getsockname	args( ( int s, struct sockaddr *name, int *namelen ) );
int	gettimeofday	args( ( struct timeval *tp, struct timezone *tzp ) );
#if	!defined(htons)
u_short	htons		args( ( u_short hostshort ) );
#endif
int	listen		args( ( int s, int backlog ) );
#if	!defined(ntohl)
u_long	ntohl		args( ( u_long hostlong ) );
#endif
int	read		args( ( int fd, char *buf, int nbyte ) );
int	select		args( ( int width, fd_set *readfds, fd_set *writefds,
			    fd_set *exceptfds, struct timeval *timeout ) );
int	setsockopt	args( ( int s, int level, int optname, caddr_t optval,
			    int optlen ) );
int	socket		args( ( int domain, int type, int protocol ) );
int	write		args( ( int fd, char *buf, int nbyte ) );
#endif

/*
 * This includes Solaris SYSV as well
 */

#if defined(sun)
int	accept		args( ( int s, struct sockaddr *addr, int *addrlen ) );
int	bind		args( ( int s, struct sockaddr *name, int namelen ) );
void	bzero		args( ( char *b, int length ) );
int	close		args( ( int fd ) );
int	getpeername	args( ( int s, struct sockaddr *name, int *namelen ) );
int	getsockname	args( ( int s, struct sockaddr *name, int *namelen ) );
int	gettimeofday	args( ( struct timeval *tp, struct timezone *tzp ) );
int	listen		args( ( int s, int backlog ) );
int	read		args( ( int fd, char *buf, int nbyte ) );
int	select		args( ( int width, fd_set *readfds, fd_set *writefds,
			    fd_set *exceptfds, struct timeval *timeout ) );
#if defined(SYSV)
int	setsockopt	args( ( int s, int level, int optname, 
                            const char *optval, int optlen ) );
#else 
int	setsockopt	args( ( int s, int level, int optname, void *optval,
			    int optlen ) );
#endif
int	socket		args( ( int domain, int type, int protocol ) );
int	write		args( ( int fd, char *buf, int nbyte ) );
#endif

#if defined(ultrix)
int	accept		args( ( int s, struct sockaddr *addr, int *addrlen ) );
int	bind		args( ( int s, struct sockaddr *name, int namelen ) );
void	bzero		args( ( char *b, int length ) );
int	close		args( ( int fd ) );
int	getpeername	args( ( int s, struct sockaddr *name, int *namelen ) );
int	getsockname	args( ( int s, struct sockaddr *name, int *namelen ) );
int	gettimeofday	args( ( struct timeval *tp, struct timezone *tzp ) );
int	listen		args( ( int s, int backlog ) );
int	read		args( ( int fd, char *buf, int nbyte ) );
int	select		args( ( int width, fd_set *readfds, fd_set *writefds,
			    fd_set *exceptfds, struct timeval *timeout ) );
int	setsockopt	args( ( int s, int level, int optname, void *optval,
			    int optlen ) );
int	socket		args( ( int domain, int type, int protocol ) );
int	write		args( ( int fd, char *buf, int nbyte ) );
#endif



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
int                 packet[60];


/*
 * OS-dependent local functions.
 */
#if defined(macintosh) || defined(MSDOS)
void	game_loop_mac_msdos	args( ( void ) );
bool	read_from_descriptor	args( ( DESCRIPTOR_DATA *d ) );
bool	write_to_descriptor	args( ( int desc, char *txt, int length ) );
#endif

#if defined(unix)
void	game_loop_unix		args( ( int control ) );
int	init_socket		args( ( int port ) );
void	new_descriptor		args( ( int control ) );
bool	read_from_descriptor	args( ( DESCRIPTOR_DATA *d ) );
bool	write_to_descriptor	args( ( int desc, char *txt, int length ) );
bool    write_to_descr_nice     args( ( DESCRIPTOR_DATA *d ) );
#endif




/*
 * Other local functions (OS-independent).
 */
bool	check_parse_name	args( ( char *name ) );
bool	check_reconnect		args( ( DESCRIPTOR_DATA *d, char *name,
				    bool fConn ) );
bool	check_playing		args( ( DESCRIPTOR_DATA *d, char *name ) );
int	main			args( ( int argc, char **argv ) );
void    nanny           args( ( DESCRIPTOR_DATA *d, char *argument ) );
bool	process_output		args( ( DESCRIPTOR_DATA *d, bool fPrompt ) );
void	read_from_buffer	args( ( DESCRIPTOR_DATA *d ) );
void	stop_idling		args( ( CHAR_DATA *ch ) );
void    bust_a_prompt           args( ( CHAR_DATA *ch ) );


int main( int argc, char **argv )
{
    struct timeval now_time;
    int port;

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

    /*
     * Reserve one channel for our use.
     */
    if ( ( fpReserve = fopen( NULL_FILE, "r" ) ) == NULL )
    {
	perror( NULL_FILE );
	exit( 1 );
    }

    /*
     * Get the port number.
     */
    port = 4000;
    if ( argc > 1 )
    {
     if ( !is_number( argv[1] ) )
     {
         fprintf( stderr, "Usage: %s [port #]\n", argv[0] );
         exit( 1 );
     }
     else if ( ( port = atoi( argv[1] ) ) <= 1024 )
     {
     fprintf( stderr, "Invalid (%d) Port number must be above 1024.\n", port );
     exit( 1 );
     }
    }

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

    /*
     * That's all, folks.
     */
    log_string( "Normal termination of game." );
    exit( 0 );
    return 0;
}



#if defined(unix)
int init_socket( int port )
{
    static struct sockaddr_in sa_zero;
    struct sockaddr_in sa;
    int x = 1;
    int fd;

    if ( ( fd = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
    {
	perror( "Init_socket: socket" );
	exit( 1 );
    }

    if ( setsockopt( fd, SOL_SOCKET, SO_REUSEADDR,
    (char *) &x, sizeof(x) ) < 0 )
    {
	perror( "Init_socket: SO_REUSEADDR" );
	close( fd );
	exit( 1 );
    }

#if defined(SO_DONTLINGER) && !defined(SYSV)
    {
	struct	linger	ld;

	ld.l_onoff  = 1;
	ld.l_linger = 1000;

	if ( setsockopt( fd, SOL_SOCKET, SO_DONTLINGER,
	(char *) &ld, sizeof(ld) ) < 0 )
	{
	    perror( "Init_socket: SO_DONTLINGER" );
	    close( fd );
	    exit( 1 );
	}
    }
#endif

    sa		    = sa_zero;
    sa.sin_family   = AF_INET;
    sa.sin_port	    = htons( port );

    if ( bind( fd, (struct sockaddr *) &sa, sizeof(sa) ) < 0 )
    {
	perror( "Init_socket: bind" );
	close( fd );
	exit( 1 );
    }

    if ( listen( fd, 3 ) < 0 )
    {
	perror( "Init_socket: listen" );
	close( fd );
	exit( 1 );
    }

    return fd;
}
#endif



#if defined(macintosh) || defined(MSDOS)
void game_loop_mac_msdos( void )
{
    struct timeval last_time;
    struct timeval now_time;
    static DESCRIPTOR_DATA dcon;

    gettimeofday( &last_time, NULL );
    current_time = (time_t) last_time.tv_sec;

    /*
     * New_descriptor analogue.
     */
    dcon.descriptor	= 0;
    dcon.connected	= CON_GET_NAME;
    dcon.host		= str_dup( "localhost" );
    dcon.outsize	= 2000;
    dcon.outbuf		= alloc_mem( dcon.outsize );
    dcon.next		= descriptor_list;
    dcon.pEdit      = NULL;
    dcon.pString    = NULL;
    dcon.fpromptok  = TRUE;
    descriptor_list	= &dcon;

    /*
     * Send the greeting.
     */
    {
	extern char * help_greeting;
	if ( help_greeting[0] == '.' )
	    write_to_buffer( &dcon, help_greeting+1, 0 );
	else
	    write_to_buffer( &dcon, help_greeting  , 0 );
    }

    /* Main loop */
    while ( !merc_down )
    {
	DESCRIPTOR_DATA *d;

	/*
     * Process input.
	 */
	for ( d = descriptor_list; d != NULL; d = d_next )
	{
	    d_next	= d->next;
	    d->fcommand	= FALSE;

#if defined(MSDOS)
	    if ( kbhit( ) )
#endif
	    {
		if ( d->character != NULL )
		    d->character->timer = 0;
		if ( !read_from_descriptor( d ) )
		{
		    if ( d->character != NULL )
			save_char_obj( d->character );
		    d->outtop	= 0;
		    close_socket( d );
		    continue;
		}
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

        if ( d->pString != NULL ) string_add( d->character,  d->incomm );
          else
        if ( d->showstr_point )
            show_string( d, d->incomm );
        else
            switch ( d->connected )
            {
            case CON_PLAYING:
                interpret( d->character, d->incomm );
                break;
            case CON_AEDITOR:
                aedit( d->character, d->incomm );
                break;
            case CON_REDITOR:
                redit( d->character, d->incomm );
                break;
            case CON_OEDITOR:
                oedit( d->character, d->incomm );
                break;
            case CON_MEDITOR:
                medit( d->character, d->incomm );
                break;
            default:
                nanny( d, d->incomm );
                break;
            }

		d->incomm[0]	= '\0';
	    }
	}



	/*
	 * Autonomous game motion.
	 */
	update_handler( );



	/*
	 * Output.
	 */
	for ( d = descriptor_list; d != NULL; d = d_next )
	{
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



	/*
	 * Synchronize to a clock.
	 * Busy wait (blargh).
	 */
	now_time = last_time;
	for ( ; ; )
	{
	    int delta;

#if defined(MSDOS)
	    if ( kbhit( ) )
#endif
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
    }

    return;
}
#endif



#if defined(unix)
void game_loop_unix( int control )
{
    static struct timeval null_time;
    struct timeval last_time;

    signal( SIGPIPE, SIG_IGN );
    gettimeofday( &last_time, NULL );
    current_time = (time_t) last_time.tv_sec;

    /* Main loop */
    while ( !merc_down )
    {
	fd_set in_set;
	fd_set out_set;
	fd_set exc_set;
	DESCRIPTOR_DATA *d;
	int maxdesc;

#if defined(MALLOC_DEBUG)
	if ( malloc_verify( ) != 1 )
	    abort( );
#endif

	/*
	 * Poll all active descriptors.
	 */
	FD_ZERO( &in_set  );
	FD_ZERO( &out_set );
	FD_ZERO( &exc_set );
	FD_SET( control, &in_set );
	maxdesc	= control;
	for ( d = descriptor_list; d; d = d->next )
	{
	    maxdesc = UMAX( maxdesc, d->descriptor );
	    FD_SET( d->descriptor, &in_set  );
	    FD_SET( d->descriptor, &out_set );
	    FD_SET( d->descriptor, &exc_set );
	}

	if ( select( maxdesc+1, &in_set, &out_set, &exc_set, &null_time ) < 0 )
	{
	    perror( "Game_loop: select: poll" );
	    exit( 1 );
	}

	/*
	 * New connection?
	 */
	if ( FD_ISSET( control, &in_set ) )
	    new_descriptor( control );

	/*
	 * Kick out the freaky folks.
	 */
	for ( d = descriptor_list; d != NULL; d = d_next )
	{
	    d_next = d->next;   
	    if ( FD_ISSET( d->descriptor, &exc_set ) )
	    {
		FD_CLR( d->descriptor, &in_set  );
		FD_CLR( d->descriptor, &out_set );
		if ( d->character )
		    save_char_obj( d->character );
		d->outtop	= 0;
		close_socket( d );
	    }
	}

	/*
	 * Process input.
	 */
	for ( d = descriptor_list; d != NULL; d = d_next )
	{
	    d_next	= d->next;
	    d->fcommand	= FALSE;

	    if ( FD_ISSET( d->descriptor, &in_set ) )
	    {
		if ( d->character != NULL )
		    d->character->timer = 0;
		if ( !read_from_descriptor( d ) )
		{
		    FD_CLR( d->descriptor, &out_set );
		    if ( d->character != NULL )
			save_char_obj( d->character );
		    d->outtop	= 0;
		    close_socket( d );
		    continue;
		}
	    }

	    if ( d->character != NULL && d->character->wait > 0 )
	    {
                if (IS_IMMORTAL(d->character))
                      d->character->wait = 0;
                 else --d->character->wait;
		continue;
	    }

	    read_from_buffer( d );
	    if ( d->incomm[0] != '\0' )
	    {
		d->fcommand	= TRUE;
		stop_idling( d->character );

        if ( d->pString != NULL ) string_add( d->character,  d->incomm );
          else
        if ( d->showstr_point )
            show_string( d, d->incomm );
        else
            switch ( d->connected )
            {
            case CON_PLAYING:
                interpret( d->character, d->incomm );
                break;
            case CON_AEDITOR:
                aedit( d->character, d->incomm );
                break;
            case CON_REDITOR:
                redit( d->character, d->incomm );
                break;
            case CON_OEDITOR:
                oedit( d->character, d->incomm );
                break;
            case CON_MEDITOR:
                medit( d->character, d->incomm );
                break;
            default:
                nanny( d, d->incomm );
                break;
            }

		d->incomm[0]	= '\0';
	    }
	}



	/*
	 * Autonomous game motion.
	 */
	update_handler( );



	/*
	 * Output.
	 */
	for ( d = descriptor_list; d != NULL; d = d_next )
	{
	    d_next = d->next;

	    if ( ( d->fcommand || d->outtop > 0 )
	    &&   FD_ISSET(d->descriptor, &out_set) )
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



	/*
	 * Synchronize to a clock.
	 * Sleep( last_time + 1/PULSE_PER_SECOND - now ).
	 * Careful here of signed versus unsigned arithmetic.
	 */
	{
	    struct timeval now_time;
	    long secDelta;
	    long usecDelta;

	    gettimeofday( &now_time, NULL );
	    usecDelta	= ((int) last_time.tv_usec) - ((int) now_time.tv_usec)
			+ 1000000 / PULSE_PER_SECOND;
	    secDelta	= ((int) last_time.tv_sec ) - ((int) now_time.tv_sec );
	    while ( usecDelta < 0 )
	    {
		usecDelta += 1000000;
		secDelta  -= 1;
	    }

	    while ( usecDelta >= 1000000 )
	    {
		usecDelta -= 1000000;
		secDelta  += 1;
	    }

	    if ( secDelta > 0 || ( secDelta == 0 && usecDelta > 0 ) )
	    {
		struct timeval stall_time;

		stall_time.tv_usec = usecDelta;
		stall_time.tv_sec  = secDelta;
		if ( select( 0, NULL, NULL, NULL, &stall_time ) < 0 )
		{
		    perror( "Game_loop: select: stall" );
		    exit( 1 );
		}
	    }
	}

	gettimeofday( &last_time, NULL );
	current_time = (time_t) last_time.tv_sec;
    }

    return;
}
#endif



#if defined(unix)
void new_descriptor( int control )
{
    static DESCRIPTOR_DATA d_zero;
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *dnew;
    BAN_DATA *pban;
    struct sockaddr_in sock;
    struct hostent *from;
    int desc;
    int size;

    size = sizeof(sock);
    getsockname( control, (struct sockaddr *) &sock, &size );
    if ( ( desc = accept( control, (struct sockaddr *) &sock, &size) ) < 0 )
    {
	perror( "New_descriptor: accept" );
	return;
    }

#if !defined(FNDELAY)
#define FNDELAY O_NDELAY
#endif

    if ( fcntl( desc, F_SETFL, FNDELAY ) == -1 )
    {
	perror( "New_descriptor: fcntl: FNDELAY" );
	return;
    }

    /*
     * Cons a new descriptor.
     */
    if ( descriptor_free == NULL )
    {
	dnew		= alloc_perm( sizeof(*dnew) );
    }
    else
    {
	dnew		= descriptor_free;
	descriptor_free	= descriptor_free->next;
    }

    *dnew		= d_zero;
    dnew->descriptor	= desc;
    dnew->connected	= CON_GET_NAME;
    dnew->showstr_head  = NULL;
    dnew->showstr_point = NULL;
    dnew->outsize	= 2000;
    dnew->pEdit         = NULL;
    dnew->pString       = NULL;
    dnew->fpromptok     = TRUE;
    dnew->outbuf	= alloc_mem( dnew->outsize );

    size = sizeof(sock);
    if ( getpeername( desc, (struct sockaddr *) &sock, &size ) < 0 )
    {
	perror( "New_descriptor: getpeername" );
	dnew->host = str_dup( "(unknown)" );
    }
    else
    {
	/*
	 * Would be nice to use inet_ntoa here but it takes a struct arg,
	 * which ain't very compatible between gcc and system libraries.
	 */
	int addr;

	addr = ntohl( sock.sin_addr.s_addr );
	sprintf( buf, "%d.%d.%d.%d",
	    ( addr >> 24 ) & 0xFF, ( addr >> 16 ) & 0xFF,
	    ( addr >>  8 ) & 0xFF, ( addr       ) & 0xFF
	    );
	sprintf( log_buf, "Sock.sinaddr:  %s", buf );
	log_string( log_buf );

    sprintf( log_buf, "Notify>  New connection: %s", buf );
    NOTIFY( log_buf, LEVEL_IMMORTAL, WIZ_NOTIFY_LOGIN );

	from = gethostbyaddr( (char *) &sock.sin_addr,
	    sizeof(sock.sin_addr), AF_INET );
	dnew->host = str_dup( from ? from->h_name : buf );
    }
	
    /*
     * Swiftest: I added the following to ban sites.  I don't
     * endorse banning of sites, but Copper has few descriptors now
     * and some people from certain sites keep abusing access by
     * using automated 'autodialers' and leaving connections hanging.
     *
     * Furey: added suffix check by request of Nickel of HiddenWorlds.
     */
    for ( pban = ban_list; pban != NULL; pban = pban->next )
    {
    if ( /*!*/str_suffix( pban->name, dnew->host ) )
	{
	    write_to_descriptor( desc,
		"Your site has been banned from this Mud.\n\r", 0 );
	    close( desc );
	    free_string( dnew->host );
	    free_mem( dnew->outbuf, dnew->outsize );
	    dnew->next		= descriptor_free;
	    descriptor_free	= dnew;
	    return;
	}
    }

    /*
     * Init descriptor data.
     */
    dnew->next			= descriptor_list;
    descriptor_list		= dnew;

    /*
     * Send the greeting.
     */
    {
	extern char * help_greeting;
	if ( help_greeting[0] == '.' )
	    write_to_buffer( dnew, help_greeting+1, 0 );
	else
	    write_to_buffer( dnew, help_greeting  , 0 );
    }

    return;
}
#endif



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
        if ( dclose->connected < CON_PLAYING ) 
             dclose->connected = CON_PLAYING;
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

    close( dclose->descriptor );
    free_string( dclose->host );
    free_mem( dclose->outbuf, dclose->outsize );      /* RT socket leak fix */
    dclose->next	= descriptor_free;
    descriptor_free	= dclose;
#if defined(MSDOS) || defined(macintosh)
    exit(1);
#endif
    return;
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

    /* Snarf input. */
#if defined(macintosh)
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
#endif

#if defined(MSDOS) || defined(unix)
    for ( ; ; )
    {
	int nRead;

	nRead = read( d->descriptor, d->inbuf + iStart,
	    sizeof(d->inbuf) - 10 - iStart );
	if ( nRead > 0 )
	{
	    iStart += nRead;
	    if ( d->inbuf[iStart-1] == '\n' || d->inbuf[iStart-1] == '\r' )
		break;
	}
	else if ( nRead == 0 )
	{
        char buf[MAX_STRING_LENGTH];

        log_string( "EOF encountered on read." );
        sprintf( buf, "Notify> %s has lost link.", (d != NULL && d->character != NULL) 
                                         ? d->character->name : "New_char" );
        NOTIFY( buf, LEVEL_IMMORTAL, WIZ_NOTIFY_LOGIN );
        return FALSE;
	}
	else if ( errno == EWOULDBLOCK )
	    break;
	else
	{
        perror( "Read_from_descriptor" );
	    return FALSE;
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
        if ( ++d->repeat >= 50 )
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

/*
 * Bust a prompt (player settable prompt)
 * coded by Morgenes for Aldara Mud
 */
void bust_a_prompt( CHAR_DATA *ch )
{
   static char * const dir_sh1 [] = { "n", "e", "s", "w", "u", "d" };
   static char * const dir_sh2 [] = { "N", "E", "S", "W", "U", "D" };
   char buf[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];
   const char *str;
   const char *i;
   int dir;
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
         default: i = " "; break;
        case '1': i = ""; if ( IS_SET( ch->act2, PLR_ANSI ) ) i = BLUE;   break;
        case '2': i = ""; if ( IS_SET( ch->act2, PLR_ANSI ) ) i = GREEN;  break;
        case '3': i = ""; if ( IS_SET( ch->act2, PLR_ANSI ) ) i = CYAN;   break;
        case '4': i = ""; if ( IS_SET( ch->act2, PLR_ANSI ) ) i = RED;    break;
        case '5': i = ""; if ( IS_SET( ch->act2, PLR_ANSI ) ) i = PURPLE; break;
        case '6': i = ""; if ( IS_SET( ch->act2, PLR_ANSI ) ) i = YELLOW; break;
        case '7': i = ""; if ( IS_SET( ch->act2, PLR_ANSI ) ) i = GREY;   break;
        case 'B': i = ""; if ( IS_SET( ch->act2, PLR_ANSI )
                    || IS_SET( ch->act2, PLR_VT100 ) ) i = BOLD; break;
        case 'I': i = ""; if ( IS_SET( ch->act2, PLR_ANSI )
                    || IS_SET( ch->act2, PLR_VT100 ) ) i = INVERSE; break;
        case 'F': i = ""; if ( IS_SET( ch->act2, PLR_ANSI )
                    || IS_SET( ch->act2, PLR_VT100 ) ) i = FLASH; break;
        case 'N': i = ""; if ( IS_SET( ch->act2, PLR_ANSI )
                    || IS_SET( ch->act2, PLR_VT100 ) ) i = NTEXT; break;
         case 'h' :
           sprintf( buf2, "%s", STRING_HITS(ch) );
            i = buf2; break;
         case 'H' :
            if ( IS_IMMORTAL(ch) )
            sprintf( buf2, "%d", PERCENTAGE(ch->hit, ch->max_hit) );
       else sprintf( buf2, " " );
            i = buf2; break;
         case 'm' :
            sprintf( buf2, "%s", STRING_MOVES(ch)  );
            i = buf2; break;
         case 'M' :
            if ( IS_IMMORTAL(ch) )
            sprintf( buf2, "%d", PERCENTAGE(ch->move, ch->max_move) );
       else sprintf( buf2, " " );
            i = buf2; break;
         case 'x' :
            sprintf( buf2, "%d", ch->exp );
            i = buf2; break;
         case 'g' :
            sprintf( buf2, "%s", name_amount( tally_coins( ch ) ) );
            i = buf2; break;
         case 'a' :
            if( ch->level < LEVEL_HERO )
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
         case 'e' :
            if( IS_IMMORTAL( ch ) && ch->in_room != NULL )
            {
               ROOM_INDEX_DATA *rch;

               buf2[0] = '\0';
               rch = ch->in_room;
                  for (dir = 0; dir <= 5; dir++)
                  {
                    if (rch->exit[dir] != NULL)
                    {
                       if (IS_SET(rch->exit[dir]->exit_info, EX_ISDOOR))
                          strcat(buf2,dir_sh2[dir]);
                          else
                          strcat(buf2,dir_sh1[dir]);
                    }
                    else               strcat(buf2,"_");
                  }
            }
            i = buf2; break;
         case 'z' :
            if( IS_IMMORTAL( ch ) && ch->in_room != NULL )
               sprintf( buf2, "%s", ch->in_room->area->name );
            else
               sprintf( buf2, " " );
            i = buf2; break;
         case 'Z' :
           if ( ch->wizinvis >= LEVEL_IMMORTAL )
             sprintf(buf2, "I");
           else
           if (IS_AFFECTED(ch, AFF_INVISIBLE))
             sprintf(buf2, "i");
           else
             sprintf(buf2, "V");
           i = buf2;
           break;
         case 'c' :
            if ( IS_IMMORTAL( ch ) )
            {
                switch (ch->desc->connected)
                {
                case CON_AEDITOR:
                    sprintf( buf2, "AEdit" );
                    break;
                case CON_REDITOR:
                    sprintf( buf2, "REdit" );
                    break;
                case CON_OEDITOR:
                    sprintf( buf2, "OEdit" );
                    break;
                case CON_MEDITOR:
                    sprintf( buf2, "MEdit" );
                    break;
                default:
                    buf2[0] = '\0';
                    break;
                }
            }
            else
              sprintf( buf2, " " );
            i = buf2; break;
         case 'n' :
            sprintf( buf2, "\n\r" );
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

/*
 * Low level output function.
 */
bool process_output( DESCRIPTOR_DATA *d, bool fPrompt )
{
    extern bool merc_down;

    fPrompt = (d->fpromptok ? fPrompt : 0);
    /*
     * Bust a prompt.
     */
    if ( d->pString != NULL )
        write_to_buffer( d, "> ", 2 );
     else
    if ( fPrompt  && !merc_down && d->connected <= CON_PLAYING )
        if ( d->showstr_point )
               write_to_buffer( d,  "-*Press Return*- ",    0 );
      else
    {
    CHAR_DATA *ch;

	ch = d->original ? d->original : d->character;
    if ( IS_SET(ch->act2, PLR_BLANK) )
	    write_to_buffer( d, "\n\r", 2 );

    if ( IS_SET(ch->act2, PLR_PROMPT) )
            bust_a_prompt( d->character );

    if ( IS_SET(d->character->act2, PLR_TELNET_GA) )
	    write_to_buffer( d, go_ahead_str, 0 );
    }

    /*
     * Short-circuit if nothing to write.
     */
    if ( d->outtop == 0 )
	return TRUE;

    /*
     * OS-dependent output.
     */
#if !defined(unix)
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
#else
    if ( !write_to_descr_nice( d ) )
      {
        d->outtop = 0;
        return FALSE;
      }
    else
      {
        return TRUE;
      }
#endif
}


void write_to_buffer( DESCRIPTOR_DATA *d, const char *txt, int length )
{
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
 
        if ( d->outsize >= 32000 )
          {
            d->outtop = 0;
            write_to_buffer( d, "OVER 32000 BYTES.  TRUNCATED.\n\r", 0 );
            return;
          }
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

    if ( length <= 0 )
	length = strlen(txt);

    for ( iStart = 0; iStart < length; iStart += nWrite )
    {
	nBlock = UMIN( length - iStart, 4096 );
	if ( ( nWrite = write( desc, txt + iStart, nBlock ) ) < 0 )
	    { perror( "Write_to_descriptor" ); return FALSE; }
    } 

    return TRUE;
}

 
/*
 * Spill the blood let it run on to me
 */

void parse_snoop( DESCRIPTOR_DATA *d, const char *txt )
{
    char str[MAX_STRING_LENGTH];
    int i;
    int j;
    CHAR_DATA *ch;
    
    if ( d->character == NULL )
      {
        bug( "parse_snoop: NULL passed", 0 );
        return;
      }
    else if ( d->original )
      {
        ch = d->original;
      }
    else
      {
        ch = d->character;
      }
 
    for ( i = 0,j=0; j < (MAX_STRING_LENGTH-3) ; i++, j++ )
      {
        if ( txt[i] == 0 )
          {
/*
            if ( j > 1 )
	    {
                str[j] = 0;
                write_to_buffer( d->snoop_by, str, 0 );
		}
*/
            return;
          }
 
        if ( txt[i] == '\r' )
          {
            j--;
            continue;
          }
 
        if ( j == 0 )
          {
            for ( j = 0; j < MAX_INPUT_LENGTH; j++ )
              str[j] = 0;
            strcpy( str, STR(d->character, name) );
            str[3]=0;
            strcat( str, "* " );
            j = strlen( str );
          }
 
        if ( txt[i] == '\n' )
          {
            str[j] = '\n';
            j++;
            str[j] = '\r';
            j++;
            str[j] = 0;
            if ( j >= 10 )
              write_to_buffer( d->snoop_by, str, 0 );
            str[0] = 0;
            j = -1;
          }
        else
          str[j] = txt[i];
      }
  }
 


/*
 * This procedure, an alterate to using the above, will NOT crash the
 * link by spamming the output buffer on the remote side.  This was
 * submitted by your approval by Kalgen of his mud, Zebesta.
 */
bool write_to_descr_nice( DESCRIPTOR_DATA *d )
{
    int iStart;
    int nWrite;
    int nBlock;
    int length;
 
 
    length = d->outtop;
 
    iStart = 0;
    nBlock = UMIN( length - iStart, 4096 );
 
    if ( ( nWrite = write( d->descriptor, d->outbuf + iStart, nBlock ) ) < 0 )
      { perror( "Write_to_descriptor" ); return FALSE; }
 
    if ( (length - nWrite) )
      {
        int i;
        for (i=0; (i+nWrite) < length; i++)
            d->outbuf[i] = d->outbuf[i+nWrite];
        d->outtop -= nWrite;
        d->fpromptok = FALSE;
      }
    else
      {
      if ( d->snoop_by )
        {
          d->outbuf[d->outtop] = 0;
          parse_snoop( d, d->outbuf );
        }
 
        d->fpromptok = TRUE;
        d->outtop = 0;
    }
    packet[current_time%60] += nWrite;
    return TRUE;
  }


/*
 * Deal with sockets that haven't logged in yet.
 */
void nanny( DESCRIPTOR_DATA *d, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    CHAR_DATA *ch;
    char *pwdnew;
    char *p;
    int iRace;
    bool fOld;
    OBJ_DATA *obj;

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
        sprintf( log_buf, "%s is being loaded from disk.", argument );
        NOTIFY( log_buf, LEVEL_SUPREME, WIZ_NOTIFY );
        log_string( log_buf );

	fOld = load_char_obj( d, argument );
	ch   = d->character;

	if ( IS_SET(ch->act, PLR_DENY) )
	{
	    sprintf( log_buf, "Denying access to %s@%s.", argument, d->host );
	    log_string( log_buf );

        sprintf( buf2, "Notify>  Denied %s from %s", argument, d->host );
        NOTIFY( buf2, LEVEL_IMMORTAL, WIZ_NOTIFY_LOGIN );

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
        if ( wizlock && (!IS_HERO(ch) || !IS_SET(ch->act, PLR_WIZBIT)) )
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
        sprintf( buf, "Did I get that right, %s (Y/N)? ", argument );
	    write_to_buffer( d, buf, 0 );
	    d->connected = CON_CONFIRM_NEW_NAME;
	    return;
	}
	break;

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
          {
            check_reconnect( d, ch->name, TRUE );
            return;
          }

		    
	sprintf( log_buf, "%s@%s has connected.", ch->name, d->host );
	log_string( log_buf );
        if ( IS_HERO(ch) )
	    do_help( ch, "imotd" );
	do_help( ch, "motd" );
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

	if ( strlen(argument) < 5 )
	{
	    write_to_buffer( d,
        "Password must be at least five characters long.\n\rPassword: ", 0 );
	    return;
	}

	pwdnew = crypt( argument, ch->name );
	for ( p = pwdnew; *p != '\0'; p++ )
	{
	    if ( *p == '~' )
	    {
        write_to_buffer( d,
               "New password not acceptable, try again.\n\rPassword: ", 0 );
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

    sprintf( buf2, "Notify> New player %s@%s.", ch->name, d->host );
    NOTIFY( buf2, LEVEL_IMMORTAL, WIZ_NOTIFY_LOGIN );

    write_to_buffer( d, echo_on_str, 0 );
    write_to_buffer( d, "What sex? (Male/Female/Neuter) ", 0 );
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

    for ( iRace = 0;  iRace < MAX_RACE;  iRace++ )
    {
        sprintf( buf, "%2d - %s\n\r", iRace, race_table[iRace].race_name );
        write_to_buffer( d, buf, 0 );
    }
    write_to_buffer( d, "\n\rWhich race? ", 0 );
    d->connected = CON_GET_NEW_RACE;
  break;


    case CON_GET_NEW_RACE:
    iRace      = atoi( argument );

    if ( iRace >= MAX_RACE || !is_number( argument ) )
    {
        write_to_buffer( d, "** Thats not a race! **\n\r", 0 );
        for ( iRace = 0;  iRace < MAX_RACE;  iRace++ )
        {
            sprintf( buf, "%2d - %s\n\r", iRace, race_table[iRace].race_name );
            write_to_buffer( d, buf, 0 );
        }
        write_to_buffer( d, "\n\rWhich race? ", 0 );
        d->connected = CON_GET_NEW_RACE;
        return;
    }

    ch->race     = iRace;
    do_help( ch, race_table[iRace].race_name );
    write_to_buffer( d, "Are you sure you want this race? ", 0 );
    d->connected = CON_CONFIRM_NEW_RACE;
  break;


  case CON_CONFIRM_NEW_RACE:
    switch ( argument[0] )
    {
    case 'y': case 'Y':  break;
    default:
        write_to_buffer( d, "Which race? ", 0 );
        d->connected = CON_GET_NEW_RACE;
        return;
    }

                /* Newbie equipment */
    obj = create_object( get_obj_index( OBJ_VNUM_DEFAULT_LIGHT ), 1 );
    obj_to_char( obj, ch );
    obj->wear_loc = WEAR_HOLD_2;
    obj = create_object( get_obj_index( OBJ_VNUM_DEFAULT_WEAPON ), 1 );
    obj_to_char( obj, ch );
    obj->wear_loc = WEAR_HOLD_1;
    obj = create_object( get_obj_index( OBJ_VNUM_DEFAULT_VEST ), 1 );
    obj_to_char( obj, ch );
    obj = create_object( get_obj_index( OBJ_VNUM_DEFAULT_BELT ), 1 );
    obj_to_char( obj, ch );
    obj = create_object( get_obj_index( OBJ_VNUM_DEFAULT_PACK ), 1 );
    obj_to_char( obj, ch );
    obj_to_obj( create_object( get_obj_index( OBJ_VNUM_DEFAULT_TINDERBOX ), 1),
                obj );
    obj_to_obj( create_object( get_obj_index( OBJ_VNUM_DEFAULT_LETTER ), 1),
                obj );
    obj_to_obj( create_object( get_obj_index( OBJ_VNUM_DEFAULT_FOOD ), 1),
                obj );
    obj_to_obj( create_object( get_obj_index( OBJ_VNUM_DEFAULT_DRINK ), 1),
                obj );
            
    create_amount( 20, ch, NULL, NULL );
    ch->practice         = 20;
    ch->learn            = 5;

    ch->affected_by      = race_table[ch->race].affect_bits;
    ch->position         = POS_STANDING;

    ch->act              = 0;
    ch->act2             = PLR_BLANK | PLR_COMBINE | PLR_PROMPT;
    ch->pcdata->pagelen  = 20;
    ch->prompt           = str_dup( "< %h %m > " );

    ch->in_room          = get_room_index( race_table[ch->race].start_room );
    ch->exp              = 0;
    ch->hit              = ch->max_hit;
    ch->move             = ch->max_move;

    /* Race Information */
    ch->pcdata->perm_str = number_range( 9, 13 ) + race_table[ch->race].bonus[0];
    ch->pcdata->perm_int = number_range( 9, 13 ) + race_table[ch->race].bonus[1];
    ch->pcdata->perm_wis = number_range( 9, 13 ) + race_table[ch->race].bonus[2];
    ch->pcdata->perm_dex = number_range( 9, 13 ) + race_table[ch->race].bonus[3];
    ch->pcdata->perm_con = number_range( 9, 13 ) + race_table[ch->race].bonus[4];
    ch->size             = race_table[ch->race].size;

    for ( iRace = 0;  iRace < MAX_LANG;  iRace++ )
    {
        ch->pcdata->learned[lang_gsn[iRace]] = race_table[ch->race].startlang[iRace];
    }

    ch->level            = 0;

    sprintf( buf, "the Young %s", race_table[ch->race].race_name );
    set_title( ch, buf );

    sprintf( log_buf, "%s@%s new player.", ch->name, d->host );
    log_string( log_buf );

	write_to_buffer( d, "\n\r", 2 );
	do_help( ch, "motd" );
    d->connected = CON_READ_MOTD;
	break;

    case CON_READ_MOTD:
    sprintf( buf2, "\n\r\n\rWelcome to %s.\n\r\n\r",  VERSION_STR );
    write_to_buffer( d, buf2, 0 );

    sprintf( buf2, "Notify> %s@%s has entered the game.", ch->name, d->host );
    NOTIFY( buf2, LEVEL_IMMORTAL, WIZ_NOTIFY_LOGIN );

    ch->next      = char_list;
    char_list     = ch;
    d->connected  = CON_PLAYING;

    if ( ch->in_room == NULL )
    {
        if ( IS_IMMORTAL(ch) )
        ch->in_room = get_room_index( ROOM_VNUM_LIMBO );
   else ch->in_room = get_room_index( ROOM_VNUM_TEMPLE );
    }

    if ( ch->in_room == NULL )
         ch->in_room = get_room_index( ROOM_VNUM_TEMPLATE );
        
    char_to_room( ch, ch->in_room );

    if ( ch->level == 0 )             /* damn newbies */
    {
    act( "$n has entered the game for the very first time.",
             ch, NULL, NULL, TO_NOTVICT );
    send_to_char( "\n\rWelcome new player!  Type HELP for help!\n\r\n\r", ch );
    ch->level = 1;
    do_wear( ch, "all" );
    }
    else
    {
    act( "$n has entered the game.", ch, NULL, NULL, TO_NOTVICT );
    do_look( ch, "auto" );
    if ( ch->size == 0 ) ch->size = race_table[ch->race].size;
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
     * Reserved words or anti-lamer tactics.
     */
  if ( is_name( name, "all auto immortal self newbie god hero someone you me the avatar here" ) )
  return FALSE;

    if ( !str_infix( name, "fuck" )           /*  to get rid of lamers    */
    ||   !str_infix( name, "shit" )           /*  who like to swear in    */
    ||   !str_infix( name, "cunt" )           /*  they're names, usually  */
    ||   !str_infix( name, "penis" )          /*  to the expense of the   */
    ||   !str_infix( name, "asshole" )        /*  MUD admins....  like    */
    ||   !str_infix( name, "vagina" )         /*  'FuckYouLocke' for ex.  */
    ||   !str_cmp  ( name, "ass" )     )      /*  can have 'ass' however  */
        return FALSE;                         /*  -assassin is ok etc...  */

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
#ifdef NAME_LENGTH
    if ( strlen(name) > NAME_LENGTH )
#else
    if ( strlen(name) > 12 )
#endif
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
        if ( is_name( name, pMobIndex->name ) )
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
    char buf[MAX_STRING_LENGTH];

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
	
        sprintf( buf, "Notify>  %s", log_buf );
        NOTIFY( buf, LEVEL_IMMORTAL, WIZ_NOTIFY_LOGIN );

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
         write_to_buffer( dold, "Replacing with new player...Bye!\n\r", 0 );
         close_socket( dold );
         return TRUE;
    }
    }

    return FALSE;
}



void stop_idling( CHAR_DATA *ch )
{
    if ( ch == NULL
    ||   ch->desc == NULL
    ||   ch->desc->connected > CON_PLAYING
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
 * Write to one char.
 */
void send_to_char( const char *txt, CHAR_DATA *ch )
{
    if ( txt != NULL && ch->desc != NULL )
        write_to_buffer( ch->desc, txt, strlen(txt) );
    return;
}

/*
 * Send a page to one char.
 */
void page_to_char( const char *txt, CHAR_DATA *ch )
{
    if ( txt == NULL || ch->desc == NULL )
        return;
    free_string( ch->desc->showstr_head );
    ch->desc->showstr_head = str_dup( txt );
    ch->desc->showstr_point = ch->desc->showstr_head;
    show_string( ch->desc, "" );

}

/*
 * Simplified pager.
 */
void show_string(struct descriptor_data *d, char *input)
{
  char buffer[ MAX_STRING_LENGTH * 3 ];
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
        d->showstr_head = str_dup( "" );
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
                       d->showstr_head = str_dup( "" );
                   }
                   d->showstr_point = 0;
               }
               return;
           }

  return;
}



void ansi_color( const char *txt, CHAR_DATA *ch )
{
    if ( txt != NULL && ch->desc != NULL )
    {
     if ( !IS_SET(ch->act2,PLR_ANSI) && !IS_SET(ch->act2,PLR_VT100) ) return;
      else
      if ( IS_SET(ch->act2, PLR_VT100) && !IS_SET( ch->act2, PLR_ANSI ) )
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


/*
 * The primary output interface for formatted output.
 */
void act( const char *format, CHAR_DATA *ch, const void *arg1, const void *arg2, int type )
{
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
        
    if ( ch == NULL )
    {
       bug( "Act: null ch", 0 );
       return;
    }
        
    if ( ch->in_room == NULL && type != TO_CHAR && type != TO_VICT )
    {
       bug( "Act: null ch->in_room", 0 );
       return;
    }

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
    if ( to->desc == NULL || !IS_AWAKE(to) )                 continue;
    if ( type == TO_CHAR    && to != ch )                    continue;
    if ( type == TO_VICT    && ( to != vch || to == ch ) )   continue;
    if ( type == TO_ROOM    && to == ch )                    continue;
    if ( type == TO_NOTVICT && (to == ch || to == vch) )     continue;

    point  = buf;
	str	= format;
	while ( *str != '\0' )
	{
	    if ( *str != '$' )
	    {
		*point++ = *str++;
		continue;
	    }
	    ++str;

        if ( arg2 == NULL && *str >= 'A' && *str <= 'Z'
                          && *str >= '0' && *str <= '9' )
	    {
		bug( "Act: missing arg2 for code %d.", *str );
		i = " <@@@> ";
	    }
	    else
	    {
		switch ( *str )
		{
        default:  bug( "Act: bad code %d.", *str ); i = " <@@@> ";      break;
        case '1': i = ""; if ( IS_SET( to->act2, PLR_ANSI ) ) i = BLUE; break;
        case '2': i = ""; if ( IS_SET( to->act2, PLR_ANSI ) ) i = GREEN; break;
        case '3': i = ""; if ( IS_SET( to->act2, PLR_ANSI ) ) i = CYAN; break;
        case '4': i = ""; if ( IS_SET( to->act2, PLR_ANSI ) ) i = RED; break;
        case '5': i = ""; if ( IS_SET( to->act2, PLR_ANSI ) ) i = PURPLE; break;
        case '6': i = ""; if ( IS_SET( to->act2, PLR_ANSI ) ) i = YELLOW; break;
        case '7': i = ""; if ( IS_SET( to->act2, PLR_ANSI ) ) i = GREY; break;
        case '0': i = ""; if ( IS_SET( to->act2, PLR_ANSI ) ) i = BLACK; break;
        case 'B': i = ""; if ( IS_SET( to->act2, PLR_ANSI )
                    || IS_SET( to->act2, PLR_VT100 ) ) i = BOLD;     break;
        case 'I': i = ""; if ( IS_SET( to->act2, PLR_ANSI )
                    || IS_SET( to->act2, PLR_VT100 ) ) i = INVERSE;  break;
        case 'F': i = ""; if ( IS_SET( to->act2, PLR_ANSI )
                    || IS_SET( to->act2, PLR_VT100 ) ) i = FLASH;    break;
        case 'R': i = ""; if ( IS_SET( to->act2, PLR_ANSI )
                    || IS_SET( to->act2, PLR_VT100 ) ) i = NTEXT;    break;

        /* Thx alex for 't' idea */
        case 't': i = "<null t>"; if (arg1 != NULL) i = (char *) arg1; break;
        case 'T': i = "<null T>"; if (arg2 != NULL) i = (char *) arg2; break;
        case 'n': i = PERS( ch,  to  );     break;
        case 'N': i = PERS( vch, to  );     break;
        case 'e': i = (ch == to) ? "you"    : HE_SHE(ch);   break;
        case 'E': i = (vch == to) ? "you"  : HE_SHE(vch);  break;
        case 'm': i = (ch == to) ? "you"    : HIM_HER(ch);  break;
        case 'M': i = (vch == to) ? "you"  : HIM_HER(vch); break;
        case 's': i = (ch == to) ? "your"   : HIS_HER(ch);  break;
        case 'S': i = (vch == to) ? "your" : HIS_HER(vch); break;

		case 'p':
		    i = can_see_obj( to, obj1 )
                ? STR(obj1, short_descr)
			    : "something";
		    break;

		case 'P':
		    i = can_see_obj( to, obj2 )
                ? STR(obj2, short_descr)
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
        *point++ = '\r';                /*    *point++ = '\0'; */

        if (to->desc != NULL)
         write_to_buffer( to->desc, ansi_uppercase( buf ), point - buf );
    }

    return;
}



/*
 * Macintosh support functions.
 */
#if defined(macintosh)
int gettimeofday( struct timeval *tp, void *tzp )
{
    tp->tv_sec  = time( NULL );
    tp->tv_usec = 0;
}
#endif



