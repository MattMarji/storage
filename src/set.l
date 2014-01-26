%{
/*
   Lexer for configuration file specification of Milestone 1 - ECE297.
 */

#include "set.tab.h"
#include <string.h>
extern YYSTYPE sslval;
%}

%option prefix = "ss"

%%

[\-][0-9]+	{
		   //sslval.ival = (int) atoi(sstext);
		   sslval.sval = (char *)strdup(sstext);
		   //printf("neg \n");
		   return NEG_INT;	
		}

[\+][0-9]+	{
		  //sslval.ival = (int) atoi(sstext);
		  sslval.sval = (char *)strdup(sstext);
			// printf("pos \n");
		   return POS_INT;	
		}

[0-9]+	{
		  // sslval.ival = (int) atoi(sstext);
		  sslval.sval = (char *)strdup(sstext);	
			// printf("int \n");
		   return INT;	
	}

[a-zA-Z0-9][a-zA-Z0-9]*	  { /* probably the COL_NAME/VALUE */
                   sslval.sval = (char *)strdup(sstext);
		   // printf("str \n");
		   return STRING;
                 }

[ \t]+		{
		}

,		{
			// printf("comma \n");
		   return COMMA;
		}

\n		{
		}
                 
%%

