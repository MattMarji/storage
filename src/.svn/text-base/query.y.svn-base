%{

/*
   Parser for configuration file specification of Milestone 1 - ECE297.
 */

#include <string.h>
#include <stdio.h>
#include "utils.h"
#define YYERROR_VERBOSE

extern int qqlex();

int qqerror(char *str)
{
        fprintf(stderr,"error: %s\n",str);
	return -1;
}
 
int qqwrap()
{
        return 1;
} 

int str_concat;
int first_col;
int last_string; 
%}
%union{
  char *sval;
  int ival;
}
%token <sval> STRING OPERATOR INT NEG_INT POS_INT
%token <ival> 
%token COMMA
%%

commands:
	{
	str_concat;
	first_col = 0;
	last_string=0; 
	c->totPredicates = 0;
	}
	command
	{
	if (last_string == 1)
	{
	printf("Full string is: %s \n", c->predicates[c->totPredicates]);
	c->totPredicates++;
	}
	int i;
	for (i=0; i<c->totPredicates; i++){
		printf("%s \n", c->predicates[i]);	
		}
	printf("total Num of Predicates: %d \n", c->totPredicates);
	}
	
	;

command: 
	command COMMA predicate
	|
	predicate
	;

predicate:
	colname op colvalue
	;

colname:
	STRING
	{
	last_string=0;
	//First column in the string	
	if (first_col == 0){
	printf("Store the COL_NAME %s \n", $1);
	strcpy(c->predicates[c->totPredicates], $1);
	c->totPredicates++;
	first_col = 1;
	}
	
	else if (first_col == 1 && str_concat==1){
	//We are done storing the strings for the predicate value
	printf("Full string is: %s \n", c->predicates[c->totPredicates]);	
	c->totPredicates++;

	printf("Store the COL_NAME %s \n", $1);
	strcpy(c->predicates[c->totPredicates], $1);
	c->totPredicates++;
	}
	
	else{
	printf("Store the COL_NAME %s \n", $1);
	strcpy(c->predicates[c->totPredicates], $1);
	c->totPredicates++;
	}
	
	//Reset the counter here!
	str_concat = 0;
	}
	;

op:
	OPERATOR
	{
	printf("Store the OPERATOR %s \n", $1);
	strcpy(c->predicates[c->totPredicates], $1);
	c->totPredicates++;
	}
	;
colvalue: 
	string_values
	|
	int_value
	;

string_values:
	|
	string_values string_value
	;
string_value:
	STRING
	{
	last_string=1;
	//If we get here, we know we need to store a string.
	if (str_concat == 0) //first part of the string
	{
	printf("Store the char-part1 %s \n", $1);
	strcpy(c->predicates[c->totPredicates], $1);
	}
	
	//else, We must concatenate the parts!
	else{
		printf("Store the char-nextpart %s \n", $1);
		strcat(c->predicates[c->totPredicates], " ");
		strcat(c->predicates[c->totPredicates], $1);
	}
	
	str_concat = 1; // 
	}
	;
int_value:
	NEG_INT
	{
	printf("Store the -ve integer %s \n", $1);
	strcpy(c->predicates[c->totPredicates], $1);
	c->totPredicates++;
	}
	| POS_INT
	{
	printf("Store the +ve integer %s \n", $1);
	strcpy(c->predicates[c->totPredicates], $1);
	c->totPredicates++;
	}
	| INT
	{
	printf("Store the integer %s \n", $1);
	strcpy(c->predicates[c->totPredicates], $1);
	c->totPredicates++;
	}
	;
	
%%


