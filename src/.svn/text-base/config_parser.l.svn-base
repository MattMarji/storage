%{

#include "config_parser.tab.h"
#include <string.h>

%}

%option noyywrap

alphaNum [a-zA-Z0-9]
allElse	 [\41-\42\44-\53\55-\71\73-\132\134\136-\176]
checkChar [c][h][a][r]
checkInt  [i][n][t]

host server_host
port server_port 
dir data_directory

%%

{host}		 { 
		   return HOST_PROPERTY;
		 }

{port}		 { 
		   return PORT_PROPERTY;
		 }
		 
concurrency {
			return CONCURRENCY;
			}

table		 { 
		   return TABLE;
		 }

username	{
		   return USER_NAME;
		}

password	{
		   return PASS_WORD;
		}

[\t\n ]+         { /* ignore white space */
                 }

:		{
			printf("COLON \n");
		   return COLON;
		}

,		{
			printf("COMMA \n");
		   return COMMA;
		}

\[		{
			printf("LEFT BRACE \n");
	           return LEFT_BRACE;
		}

\]		{
			printf("RIGHT BRACE \n");
		   return RIGHT_BRACE;
		}
		

{dir}		 { 
		   return DDIR_PROPERTY;
		 }

{checkChar}	{
			printf("CHAR \n");
		  return CHAR;
		}

{checkInt}	{
		       printf("INT \n");
		  return INT;
		}

 
#.*\n	         { /* ignore comments; text starting with '#' to 
                      the end of the line */
                  }

[0-9]*           { /* probably the port number */
                   yylval.pval = (int) atoi(yytext);
		   printf("0-9 ONLY ! \n");
		   return NUMBER;
                  }

{alphaNum}+	  { /* TABLE ONLY */
                   yylval.sval = (char *)strdup(yytext);
		   printf("ALPHA NUM ONLY ! \n");
		   return ALPHANUM_STRING;
                 }


{allElse}+         {
		    yylval.sval = (char *)strdup(yytext);
		    printf("ALL OTHER! \n");
		   return STRING;	
		  }
                 
.                 { 
                   return yytext[0]; 
                  }

<<EOF>>           { 
                    return 0;
                  }
%%

