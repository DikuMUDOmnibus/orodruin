#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"



char *first_arg( char *argument, char *arg_first, bool fCase );
char *format_string( char *oldstring /*, bool fSpace */);



void string_edit( CHAR_DATA *ch, char **pString )
{
ansi_color( BOLD, ch );
ansi_color( GREEN, ch );
         send_to_char( "-=====- SEditor (New String Mode) -=====-\n\r", ch );
	 send_to_char( "    Type .help on a new line for help\n\r",     ch );
	 send_to_char( "  Terminate with ~ on a blank line\n\r",   ch );
	 send_to_char( "-=======================================-\n\r", ch );
ansi_color( NTEXT, ch );
ansi_color( GREY, ch );

	 if ( *pString == NULL )
	 {
		  *pString = str_dup( "" );
	 }
	 else
	 {
		  **pString = '\0';
	 }

	 ch->desc->pString = pString;

	 return;
}



void string_append( CHAR_DATA *ch, char **pString )
{
ansi_color( BOLD, ch );
ansi_color( GREEN, ch );
         send_to_char( "-=======- SEditor (Append Mode) -=======-\n\r", ch );
	 send_to_char( "    Type .help on a new line for help\n\r",     ch );
	 send_to_char( "  Terminate with ~ on a blank line\n\r",   ch );
	 send_to_char( "-=======================================-\n\r", ch );
ansi_color( NTEXT, ch );
ansi_color( GREY, ch );

	 if ( *pString == NULL )
	 {
		  *pString = str_dup( "" );
	 }
	 send_to_char( *pString, ch );

	 if ( **pString && *(*pString + strlen( *pString ) - 1) != '\r' )
	send_to_char( "\n\r", ch );

    ch->desc->pString = pString;

	 return;
}



char * string_replace( char * orig, char * old, char * new )
{
    char xbuf[MAX_STRING_LENGTH];
    int i;

    xbuf[0] = '\0';
	 strcpy( xbuf, orig );
	 if ( strstr( orig, old ) != NULL )
    {
        i = strlen( orig ) - strlen( strstr( orig, old ) );
        xbuf[i] = '\0';
        strcat( xbuf, new );
        strcat( xbuf, &orig[i+strlen( old )] );
        free_string( orig );
    }

	 return str_dup( xbuf );
}



void string_add( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];

	 if ( *argument == '.' )
	 {
		  char arg1 [MAX_INPUT_LENGTH];
		  char arg2 [MAX_INPUT_LENGTH];
		  char arg3 [MAX_INPUT_LENGTH];

		  argument = one_argument( argument, arg1 );
		  argument = first_arg( argument, arg2, FALSE );
		  argument = first_arg( argument, arg3, FALSE );

		  if ( LOWER(arg1[1]) == 'c' && !str_prefix( arg1, ".clear" ) )
		  {
ansi_color( BOLD, ch );
ansi_color( GREEN, ch );
                                send_to_char( "String cleared.\n\r", ch );
ansi_color( NTEXT, ch );
ansi_color( GREY, ch );
                                **ch->desc->pString = '\0';
				return;
		  }

		  if ( LOWER(arg1[1]) == 's' && !str_prefix( arg1, ".show" ) )
		  {
ansi_color( BOLD, ch );
ansi_color( GREEN, ch );
                                send_to_char( "String so far:\n\r", ch );
ansi_color( NTEXT, ch );
ansi_color( GREY, ch );
ansi_color( BOLD, ch );
ansi_color( PURPLE, ch );
                                send_to_char( *ch->desc->pString, ch );
ansi_color( NTEXT, ch );
ansi_color( GREY, ch );
                                return;
		  }

		  if ( LOWER(arg1[1]) == 'r' && !str_prefix( arg1, ".replace" ) )
		  {
				if ( arg2[0] == '\0' )
				{
					 send_to_char( "Usage: .replace 'old string' 'new string'\n\r",
				  ch );
					 return;
				}

				*ch->desc->pString = string_replace( *ch->desc->pString, arg2,
						arg3 );
				sprintf( buf, "'%s' replaced with '%s'.\n\r", arg2, arg3 );
ansi_color( BOLD, ch );
ansi_color( GREEN, ch );
                                send_to_char( buf, ch );
ansi_color( NTEXT, ch );
ansi_color( GREY, ch );
                                return;
		  }

		  if ( LOWER(arg1[1]) == 'f' && !str_prefix( arg1, ".format" ) )
		  {
				*ch->desc->pString = format_string( *ch->desc->pString );
				send_to_char( "String formatted.\n\r", ch );
				return;
		  }

		  if ( LOWER(arg1[1]) == 'h' && !str_prefix( arg1, ".help" ) )
		  {
ansi_color( GREEN, ch );
                                send_to_char( "SEdit help (commands available from the beginning of a new line only):\n\r", ch );
				send_to_char( ".replace 'old string' 'new string' - replace a substring with a new string\n\r", ch );
				send_to_char( ".help                              - get help (this info)\n\r", ch );
				send_to_char( ".look                              - show string so far\n\r", ch );
				send_to_char( ".format                            - format (word wrap) string\n\r", ch );
				send_to_char( ".clear                             - clear string so far\n\r", ch );
				send_to_char( "~                                 - end string editor (end string)\n\r", ch );
ansi_color( GREY, ch );
                                return;
		  }

		  send_to_char( "Invalid dot command.\n\r", ch );
		  return;
	 }

	 strcpy( buf, *ch->desc->pString );

	 if ( *argument == '~' )
	 {
		  ch->desc->pString = NULL;
		  return;
	 }

	 if ( strlen( buf ) + strlen( argument ) >= ( MAX_STRING_LENGTH - 4 ) )
	 {
		  send_to_char( "String too long, truncating.\n\r", ch );
		  strncat( buf, argument, MAX_STRING_LENGTH );
		  free_string( *ch->desc->pString );
		  *ch->desc->pString = str_dup( buf );
		  ch->desc->pString = NULL;
		  return;
	 }

	 strcat( buf, argument );
	 strcat( buf, "\n\r" );
	 free_string( *ch->desc->pString );
	 *ch->desc->pString = str_dup( buf );
	 return;
}



/*
 * Thanks to Kalgen for the new procedure (no more bug!)
 * Original wordwrap() written by Surreality.
 */
char *format_string( char *oldstring /*, bool fSpace */)
{
  char xbuf[MAX_STRING_LENGTH];
  char xbuf2[MAX_STRING_LENGTH];
  char *rdesc;
  int i=0;
  bool cap=TRUE;

  xbuf[0]=xbuf2[0]=0;

  i=0;

  for (rdesc = oldstring; *rdesc; rdesc++)
  {
	 if (*rdesc=='\n')
	 {
		if (xbuf[i-1] != ' ')
		{
		  xbuf[i]=' ';
		  i++;
		}
	 }
	 else if (*rdesc=='\r') ;
	 else if (*rdesc==' ')
	 {
		if (xbuf[i-1] != ' ')
		{
		  xbuf[i]=' ';
		  i++;
		}
	 }
	 else if (*rdesc==')')
	 {
		if (xbuf[i-1]==' ' && xbuf[i-2]==' ' &&
			 (xbuf[i-3]=='.' || xbuf[i-3]=='?' || xbuf[i-3]=='!'))
		{
		  xbuf[i-2]=*rdesc;
		  xbuf[i-1]=' ';
		  xbuf[i]=' ';
		  i++;
		}
		else
		{
		  xbuf[i]=*rdesc;
		  i++;
		}
	 }
	 else if (*rdesc=='.' || *rdesc=='?' || *rdesc=='!') {
		if (xbuf[i-1]==' ' && xbuf[i-2]==' ' &&
			 (xbuf[i-3]=='.' || xbuf[i-3]=='?' || xbuf[i-3]=='!')) {
		  xbuf[i-2]=*rdesc;
		  if (*(rdesc+1) != '\"')
		  {
			 xbuf[i-1]=' ';
			 xbuf[i]=' ';
			 i++;
		  }
        else
        {
          xbuf[i-1]='\"';
          xbuf[i]=' ';
          xbuf[i+1]=' ';
          i+=2;
          rdesc++;
        }
      }
      else
      {
		  xbuf[i]=*rdesc;
        if (*(rdesc+1) != '\"')
        {
          xbuf[i+1]=' ';
          xbuf[i+2]=' ';
          i += 3;
        }
        else
        {
          xbuf[i+1]='\"';
          xbuf[i+2]=' ';
          xbuf[i+3]=' ';
			 i += 4;
          rdesc++;
        }
      }
      cap = TRUE;
    }
    else
    {
      xbuf[i]=*rdesc;
      if ( cap )
        {
          cap = FALSE;
			 xbuf[i] = UPPER( xbuf[i] );
        }
      i++;
    }
  }
  xbuf[i]=0;
  strcpy(xbuf2,xbuf);
  
  rdesc=xbuf2;
  
  xbuf[0]=0;
  
  for ( ; ; )
  {
    for (i=0; i<77; i++)
    {
      if (!*(rdesc+i)) break;
    }
    if (i<77)
    {
      break;
    }
    for (i=(xbuf[0]?76:73) ; i ; i--)
    {
		if (*(rdesc+i)==' ') break;
    }
    if (i)
    {
      *(rdesc+i)=0;
      strcat(xbuf,rdesc);
      strcat(xbuf,"\n\r");
      rdesc += i+1;
      while (*rdesc == ' ') rdesc++;
    }
    else
    {
		bug ("No spaces", 0);
      *(rdesc+75)=0;
      strcat(xbuf,rdesc);
      strcat(xbuf,"-\n\r");
      rdesc += 76;
    }
  }
  while (*(rdesc+i) && (*(rdesc+i)==' '||
                        *(rdesc+i)=='\n'||
                        *(rdesc+i)=='\r'))
    i--;
  *(rdesc+i+1)=0;
  strcat(xbuf,rdesc);
  if (xbuf[strlen(xbuf)-2] != '\n')
    strcat(xbuf,"\n\r");

  free_string(oldstring);
  return(str_dup(xbuf));
}



/*
 * Pick off one argument from a string and return the rest.
 * Understands quotes, parenthesis (barring ) ('s) and percentages.
 */
char *first_arg( char *argument, char *arg_first, bool fCase )
{
    char cEnd;

    while ( *argument == ' ' )
	argument++;

    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"'
      || *argument == '%'  || *argument == '(' )
	 {
        if ( *argument == '(' )
        {
            cEnd = ')';
            argument++;
        }
        else cEnd = *argument++;
    }

    while ( *argument != '\0' )
    {
	if ( *argument == cEnd )
	{
	    argument++;
	    break;
	}
    if ( fCase ) *arg_first = LOWER(*argument);
            else *arg_first = *argument;
	arg_first++;
	argument++;
    }
    *arg_first = '\0';

    while ( *argument == ' ' )
	argument++;

    return argument;
}



char * string_unpad( char * argument )
{
    char buf[MAX_STRING_LENGTH];
    char *s;

    s = argument;

    while ( *s == ' ' )
        s++;

    strcpy( buf, s );
    s = buf;

    if ( *s != '\0' )
    {
        while ( *s != '\0' )
            s++;
        s--;

        while( *s == ' ' )
            s--;
        s++;
        *s = '\0';
    }

    free_string( argument );
    return str_dup( buf );
}



char * string_proper( char * argument )
{
    char *s;

    s = argument;

    while ( *s != '\0' )
    {
        if ( *s != ' ' )
        {
            *s = UPPER(*s);
            while ( *s != ' ' && *s != '\0' )
                s++;
        }
        else
        {
            s++;
        }
    }

    return argument;
}



int arg_count( char *argument )
{
    int total;
    char *s;

    total = 0;
    s = argument;

    while ( *s != '\0' )
    {
        if ( *s != ' ' )
        {
            total++;
            while ( *s != ' ' && *s != '\0' )
                s++;
        }
        else
        {
            s++;
        }
    }

    return total;
}

