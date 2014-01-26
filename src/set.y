%{

/*
   Parser for configuration file specification of Milestone 1 - ECE297.
 */

#include <string.h>
#include <stdio.h>
#include "utils.h"

extern int sslex();

int sserror(char *str)
{
        fprintf(stderr,"error: %s\n",str);
				return -1;
}
 
int sswrap()
{
        return 1;
} 

int str_concat2;
int first_col2;
int last_string2; 

%}

%union{
  char *sval;
  int ival;
}
%token <sval> STRING INT NEG_INT POS_INT
%token <ival> 
%token COMMA
%%

commands:
	{
	c->numValues = 0;

	str_concat2=0;
	first_col2 = 0;
	last_string2=0; 

	}
	command
	{
	if (last_string2 == 1)
	{
	printf("PROBLEM Full string is: %s \n", c->set_values[c->numValues]);
	c->numValues++;
	}

	int i;
	for (i=0;i<c->numValues;i++){
	printf(" %s \n", c->set_values[i]); 	
	}
	printf("SIZE OF ARRAY IS: %d \n" , c->numValues);
	}
	;

command: 
	command COMMA column
	|
	column
	;

column:
	colname colvalue
	;

colname:
	STRING
	{
	last_string2=0;
	//First column in the string	
	if (first_col2 == 0){
	printf("Store the column name %s \n", $1);
	strcpy(c->set_values[c->numValues],$1);
	c->numValues++;
	first_col2 = 1;
	}
	
	else if (first_col2 == 1 && str_concat2==1){
	//We are done storing the strings for the predicate value


	printf("Full string is: %s \n", c->set_values[c->numValues]);	
	c->numValues++;

	printf("Store the column name %s \n", $1);
	strcpy(c->set_values[c->numValues],$1);
	c->numValues++;
	}
	
	else{
	printf("Store the column name %s \n", $1);
	strcpy(c->set_values[c->numValues],$1);
	c->numValues++;
	}
	
	//Reset the counter here!
	str_concat2 = 0;

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
	last_string2=1;
	if (str_concat2 == 0){
	printf("Store the column value p1 %s \n", $1);
	strcpy(c->set_values[c->numValues], $1);
	}

	else {
		printf("Store the column value contd %s \n", $1);
		strcat(c->set_values[c->numValues], " "); 
		strcat(c->set_values[c->numValues], $1);
	}

	str_concat2 = 1; 
	}
	;
int_value:
	NEG_INT
	{
	printf("Store value -ve integer %s \n", $1);
	strcpy(c->set_values[c->numValues], $1);
	c->numValues++;

	}
	| POS_INT
	{
	printf("Store value +ve integer %s \n", $1);
	strcpy(c->set_values[c->numValues], $1);
	c->numValues++;
	}
	| INT
	{
	printf("Store value-integer %s \n", $1);
	strcpy(c->set_values[c->numValues], $1);
	c->numValues++;
	}
	;
	
%%


