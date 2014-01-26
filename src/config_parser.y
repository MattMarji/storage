%{

/*
   Parser for configuration file specification of Milestone 1 - ECE297.
 */

#include <string.h>
#include <stdio.h>
#include "utils.h"


//struct table *tl, *t;

//struct configuration *c;

int first_table = 0; /* The first table should have a special condition! */

%}
%union{
  char *sval;
  int  pval;
}
%token <sval> STRING ALPHANUM_STRING
%token <pval> NUMBER 
%token HOST_PROPERTY PORT_PROPERTY DDIR_PROPERTY TABLE USER_NAME PASS_WORD COLON COMMA LEFT_BRACE RIGHT_BRACE CHAR INT CONCURRENCY

%%

commands:
	{

    c = (struct configuration *) malloc(sizeof(struct configuration));
    //t = (struct table *) malloc(sizeof(struct table));
   // tl = (struct table *) malloc(sizeof(struct table));	

     if (c == NULL){
    printf("Error on malloc `configuration'.\n");
    exit(-1);
    		 }
     else{
       c->host = "$$$";
       c->num_tables = 0;
       c->port = 0;
	   c->concurrency = -1;
       c->username = "$$$";
       c->password = "$$$";
       c->tlist = NULL;
     	  };	
	}
	|
	commands command 
	;

command:
	host_name | host_port | username | password | password2 | concurr | full_table | column_char | column_int | property
	;
	
host_name:
	HOST_PROPERTY STRING
	{
		c->host = strdup($2);
		printf("HOSTNAME %s\n", c->host);
		
	}
	|
	HOST_PROPERTY ALPHANUM_STRING
	{
		c->host = strdup($2);
		printf("HOSTNAME %s\n", c->host);
		
	}
	;

host_port:
	PORT_PROPERTY NUMBER
	{
		c->port = $2;
		printf("PORT NUM %d \n", c->port);
	}
	;

concurr:
	CONCURRENCY NUMBER
	{
		c->concurrency = $2;
		if (c->concurrency > 1){
		c->concurrency = 1;
		}
		printf("CONCURRENCY VALUE: %d \n", c->concurrency);
	}
		
full_table:
	TABLE ALPHANUM_STRING ALPHANUM_STRING COLON CHAR LEFT_BRACE NUMBER RIGHT_BRACE 
	{
//=============================================================
// Initialize and store the tokens so they can be stored in our 2D array
	char*col_name = $3;
	char* empty = "$\0";
	char type [100];
	sprintf(type, "char[%d]",$7);
	char* key = "KEY"; 
//=============================================================

//Assuming this is the first time a table has ever found in the config file.
		if (first_table == 0){
		tl = (struct table *) malloc(sizeof(struct table));
		
		       tl->numkeys =0;
			   tl->array_config_index = 0;
		       tl->table_name = strdup($2);
		          int i;
   
   				for (i=0; i<4096; i++){
   				strcpy(tl->array_empty[i],"#");
   				}
		       //Begin to populate the number of tables from the config file.
		        strcpy(c->all_table_names[c->num_tables],tl->table_name);
		       //Add one once the number of tables is added!
		       c->num_tables++;
		       
		       //As soon as we add a table, we know we have one column!
			   tl->numCols=0;
			   tl->numCols = tl->numCols + 1;
		       
		       tl->next = NULL;
		
		//Set-up the 2D Array
		int counter_row = 4;
		for (tl->row_index = 0; tl->row_index < counter_row; (tl->row_index)++){
				if (tl->row_index==0){
				strcpy(tl->array_config[tl->row_index], key);
 				}
				if (tl->row_index==1){
				strcpy(tl->array_config[tl->row_index], col_name);
				}
				if (tl->row_index == 2){
				strcpy(tl->array_config[tl->row_index], empty);
				}
				if (tl->row_index == 3){
				strcpy(tl->array_config[tl->row_index], type);				
				}
		}		
		printf("TABLE NAME - FIRST TABLE %s \n", $2);
		first_table = 1;
		     }
//================================================================

//================================================================
//Assuming this is a new table, but NOT the first one ever found in the config file.
		else {

			   t = (struct table *) malloc(sizeof(struct table));
		       t->table_name = strdup($2);
			t->numkeys=0;
		    t->array_config_index = 0;   
		       int i;
   
   				for (i=0; i<4096; i++){
   				strcpy(t->array_empty[i],"#");
   				}
		       
		       //Check if the table already exists		
//==================================================================
			//If there are tables, check to see if there are duplicates
		    int j;
			for (j=0; j< c->num_tables; j++){
				if (strcmp(c->all_table_names[j],t->table_name)==0)
				{
					return -1;	
				}
			}
//==================================================================	
		       
		       //Begin to populate the number of tables from the config file.
		        strcpy(c->all_table_names[c->num_tables],t->table_name);
		       //Add one once the number of tables is added!
		       c->num_tables++;
		       
		       //As soon as we add a table, we know we have one column!
		       t->numCols = 0;
			   t->numCols = t->numCols + 1;
			
		//Set-up the 2D Array -> assuming that the row_index starts at 0 because this is the first time the table was found in the config file. 
		int counter_row = 4;
		for (t->row_index = 0; t->row_index < counter_row; t->row_index++){
				if (t->row_index==0){
				strcpy(t->array_config[t->row_index], key);
 				}

				if (t->row_index ==1){
				strcpy(t->array_config[t->row_index], col_name);
				}
				
				if (t->row_index == 2){
				strcpy(t->array_config[t->row_index], empty);
				}

				if (t->row_index == 3){
				strcpy(t->array_config[t->row_index], type);				
				}
		}
		       t->next = tl;
		       tl = t;
		       printf("TABLE NAME - NEXT TABLE %s \n", $2);	
		 }
			
	} 
//================================================================
	|TABLE ALPHANUM_STRING ALPHANUM_STRING COLON INT
	{
//=============================================================
// Initialize and store the tokens so they can be stored in our 2D array
	char*col_name = $3;
	char* empty = "$\0";
	char* type = "int\0";
	char* key = "KEY";
//=============================================================
		
		if (first_table == 0){
			   tl = (struct table *) malloc(sizeof(struct table));
		       tl->table_name = strdup($2);
			tl->numkeys = 0;
		    tl->array_config_index = 0;   
		       		       
		       int i;
   
   				for (i=0; i<4096; i++){
   				strcpy(tl->array_empty[i],"#");
   				}
		       
		       
		       //Begin to populate the number of tables from the config file.
		        strcpy(c->all_table_names[c->num_tables],tl->table_name);
		       //Add one once the number of tables is added!
		       c->num_tables++;
		       
		       //As soon as we add a table, we know we have one column!
		       tl->numCols =0;
			   tl->numCols = tl->numCols + 1;
		
		//Set-up the 2D Array
		int counter_row = 4;
		for (tl->row_index = 0; tl->row_index < counter_row; (tl->row_index)++){
				if (tl->row_index==0){
				strcpy(tl->array_config[tl->row_index], key);
 				}

				if (tl->row_index==1){
				strcpy(tl->array_config[tl->row_index], col_name);
				}
				
				if (tl->row_index == 2){
				strcpy(tl->array_config[tl->row_index], empty);
				}
				if (tl->row_index == 3){
				strcpy(tl->array_config[tl->row_index], type);				
				}
		}

		tl->next = NULL;
		printf("TABLE NAME - FIRST TABLE %s \n", $2);	
		first_table = 1;	
		}

		else{
		       t = (struct table *) malloc(sizeof(struct table));
		       t->table_name = strdup($2);
		       t->numkeys = 0;
		       t->array_config_index = 0;
		       		       
		       int i;
   
   				for (i=0; i<4096; i++){
   				strcpy(t->array_empty[i],"#");
   				}
		       
		       
		    //Check if the table already exists		
			//==================================================================
			//If there are tables, check to see if there are duplicates
			int j;
			for (j=0; j< c->num_tables; j++){
				if (strcmp(c->all_table_names[j],t->table_name)==0)
				{
					return -1;	
				}
			}
			//==================================================================	
		       
		       //Begin to populate the number of tables from the config file.
		       strcpy(c->all_table_names[c->num_tables],t->table_name);
		       //Add one once the number of tables is added!
		       c->num_tables++;
		       
		       //As soon as we add a table, we know we have one column!
		       t->numCols=0;
			   t->numCols = t->numCols + 1;
			
		//Set up the 2D-Array
		int counter_row = 4;		
		for (t->row_index = 0; t->row_index < counter_row; t->row_index++){
				if (t->row_index==0){
				strcpy(t->array_config[t->row_index], key);
 				}

				if (t->row_index ==1){
				strcpy(t->array_config[t->row_index], col_name);
				}
				
				if (t->row_index == 2){
				strcpy(t->array_config[t->row_index], empty);
				}
				if (t->row_index == 3){
				strcpy(t->array_config[t->row_index], type);				
				}
		}
		t->next = tl;
		tl = t;
		printf("TABLE NAME - NEXT TABLE %s \n", $2);	
		     
		}	
	} 
	;

column_char:
	COMMA ALPHANUM_STRING COLON CHAR LEFT_BRACE NUMBER RIGHT_BRACE
	{
	
	//As soon as we get here, we know we have one column!
			tl->numCols = tl->numCols + 1;
			
//===============================================
//First, we store the necessary values to later store them in our 2D->ARRAY
	char*col_name = $2;
	char* empty = "$\0";
	char type [100];
	sprintf(type, "char[%d]",$6); 
	char* key = "KEY";
//============================================================= 		
		
		//Set-up the 2D Array
		int counter_row = tl->row_index + 4;
		for (; tl->row_index < counter_row; (tl->row_index)++){
				
				if (tl->row_index== (counter_row-4)){
				strcpy(tl->array_config[tl->row_index], key);
 				}
				if (tl->row_index== (counter_row-3)){
				strcpy(tl->array_config[tl->row_index], col_name);
 				}

				if (tl->row_index== (counter_row-2)){
				strcpy(tl->array_config[tl->row_index], empty);
				}
				
				if (tl->row_index == (counter_row-1)){
				strcpy(tl->array_config[tl->row_index], type);
				}
		}
		printf("FOUND A NEW COLUMN CHAR from table: %s! \n", tl->table_name);
	}
	;

column_int:
	COMMA ALPHANUM_STRING COLON INT
	{
	
	//As soon as we add a table, we know we have one column!
	tl->numCols = tl->numCols + 1;
//===============================================
//First, we store the necessary values to later store them in our 2D->ARRAY
	char*col_name = $2;
	char* empty = "$\0";
	char* type = "int\0"; 
	char* key = "KEY";
//=============================================================
		//Set-up the 2D Array
		int counter_row = tl->row_index + 4;
		for (; tl->row_index < counter_row; (tl->row_index)++){
				
				if (tl->row_index== (counter_row-4)){
				strcpy(tl->array_config[tl->row_index], key);
 				}
				if (tl->row_index== (counter_row-3)){
				strcpy(tl->array_config[tl->row_index], col_name);
 				}

				if (tl->row_index== (counter_row-2)){
				strcpy(tl->array_config[tl->row_index], empty);
				}
				
				if (tl->row_index == (counter_row-1)){
				strcpy(tl->array_config[tl->row_index], type);
				}
		}

		printf("FOUND A NEW COLUMN INT from table: %s! \n", tl->table_name);	
	}
	;

username:
	USER_NAME STRING
	{
		c->username = strdup($2);
		printf("USERNAME %s\n", c->username);
		
	}
	|
	USER_NAME ALPHANUM_STRING
	{
		c->username = strdup($2);
		printf("USERNAME %s\n", c->username);
		
	}
	;

password:
	PASS_WORD STRING
	{
		c->password = strdup($2);
		printf("PASSWORD %s\n", c->password);
		
	}
	;
	
password2:
	PASS_WORD ALPHANUM_STRING
	{
		c->password = strdup($2);
		printf("PASSWORD %s\n", c->password);
		
	}
	;
property: DDIR_PROPERTY STRING
	{
	 print_configuration(c);
	}

%%

yyerror(char *e){
  //fprintf(stderr,"%s.\n",e);
}

int print_configuration(struct configuration *c){

  struct table *tl;

  printf("Configuration:\n Host: %s, port: %d, data directory: %s\n",
	 c->host, c->port);
  
    tl = c->tlist;
    printf("  Table(s):\n");
    while(tl != NULL){
	
      printf("	%s \n",tl->table_name);
	
	//PRINT THE 2D Array for EACH TABLE
	int i;
	int j;	
	for (i = 0; i < tl->row_index; i++)
    	{
            printf("	%s \n", tl->array_config[i]);
        }
        printf("\n");
    	

	tl=tl->next;
    }
    printf("\n");
}

