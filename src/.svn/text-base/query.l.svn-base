%{
/*
   Lexer for configuration file specification of Milestone 1 - ECE297.
 */

#include <stdio.h>
#include <string.h>
#include "query.tab.h"
extern YYSTYPE qqlval;
%}

%option prefix = "qq"

%%

[\-][0-9]+	{
		   //qqlval.ival = (int) atoi(qqtext);
		   qqlval.sval = (char *)strdup(qqtext);
		   return NEG_INT;	
		}

[\+][0-9]+	{
		  //qqlval.ival = (int) atoi(qqtext);
		  qqlval.sval = (char *)strdup(qqtext);
		   return POS_INT;	
		}

[0-9]+	{
		  // qqlval.ival = (int) atoi(qqtext);
		  qqlval.sval = (char *)strdup(qqtext);	
		   return INT;	
	}

[a-zA-Z0-9][a-zA-Z0-9]*	  { /* probably the COL_NAME/VALUE */
                   qqlval.sval = (char *)strdup(qqtext);
		   return STRING;
                 }

[ \t]+		{
		}

,		{
		   return COMMA;
		}

[\<\>\=]	{ /* probably the operator! */
		   qqlval.sval = (char*)strdup(qqtext);
		   return OPERATOR;		
		}
                 

%%
