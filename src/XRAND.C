/****************************************************************************
 *   ___  ___  ___  __    __                                                *
 *  |   ||   ||   ||  |\/|  | 2-x    NiMUD is a software currently under    *
 *   |  \ | |  | |  | |\/| |         development.  It is based primarily on *
 *   | |\\| |  | |  | |  | |         the discontinued package, Merc 2.2.    *
 *   | | \  |  | |  | |  | |         NiMUD is being written and developed   *
 *  |___||___||___||__|  |__|        By Locke and Surreality as a new,      *
 *   NAMELESS INCARNATE *MUD*        frequently updated package similar to  *
 *        S O F T W A R E            the original Merc 2.x.                 *
 *                                                                          *
 *  Just look for the Iron Maiden skull wherever NiMUD products are found.  *
 *                                                                          *
 *  Much time and thought has gone into this software and you are           *
 *  benefitting.  We hope that you share your changes too.  What goes       *
 *  around, comes around.                                                   *
 ****************************************************************************/

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"


/*
 * Stick a little fuzz on a number.
 */
int number_fuzzy( int number )
{
    switch ( number_bits( 2 ) )
    {
    case 0:  number -= 1; break;
    case 3:  number += 1; break;
    }

    return UMAX( 1, number );
}



/*
 * Generate a random number.
 */
int number_range( int from, int to )
{
    int power;
    int number;

    if ( ( to = to - from + 1 ) <= 1 )
	return from;

    for ( power = 2; power < to; power <<= 1 )
	;

    while ( ( number = number_mm( ) & (power - 1) ) >= to )
	;

    return from + number;
}



/*
 * Generate a percentile roll.
 */
int number_percent( void )
{
    int percent;

    while ( ( percent = number_mm( ) & (128-1) ) > 99 )
	;

    return 1 + percent;
}



/*
 * Generate a random door.
 */
int number_door( void )
{

    return number_range( 0, 32768 ) % MAX_DIR;
}



int number_bits( int width )
{
    return number_mm( ) & ( ( 1 << width ) - 1 );
}



/*
 * I've gotten too many bad reports on OS-supplied random number generators.
 * This is the Mitchell-Moore algorithm from Knuth Volume II.
 * Best to leave the constants alone unless you've read Knuth.
 * -- Furey
 */
static	int	rgiState[2+55];

void init_mm( )
{
    int *piState;
    int iState;

    piState	= &rgiState[2];

    piState[-2]	= 55 - 55;
    piState[-1]	= 55 - 24;

    piState[0]	= ((int) current_time) & ((1 << 30) - 1);
    piState[1]	= 1;
    for ( iState = 2; iState < 55; iState++ )
    {
	piState[iState] = (piState[iState-1] + piState[iState-2])
			& ((1 << 30) - 1);
    }
    return;
}



int number_mm( void )
{
    int *piState;
    int iState1;
    int iState2;
    int iRand;

    piState		= &rgiState[2];
    iState1	 	= piState[-2];
    iState2	 	= piState[-1];
    iRand	 	= (piState[iState1] + piState[iState2])
			& ((1 << 30) - 1);
    piState[iState1]	= iRand;
    if ( ++iState1 == 55 )
	iState1 = 0;
    if ( ++iState2 == 55 )
	iState2 = 0;
    piState[-2]		= iState1;
    piState[-1]		= iState2;
    return iRand >> 6;
}



/*
 * Roll some dice.
 */
int dice( int number, int size )
{
    int idice;
    int sum;

    switch ( size )
    {
    case 0: return 0;
    case 1: return number;
    }

    for ( idice = 0, sum = 0; idice < number; idice++ )
	sum += number_range( 1, size );

    return sum;
}
