/**
 * @file
 * @brief This file implements the storage server.
 *
 * The storage server should be named "server" and should take a single
 * command line argument that refers to the configuration file.
 *
 * The storage server should be able to communicate with the client
 * library functions declared in storage.h and implemented in storage.c.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>
#include "utils.h"
#include "TreeDB.h"
#include "TreeNode.h"
#include "TreeEntry.h"

//create the necessary structures for our server!
//These are available
struct configuration *c;
struct table *tl;
struct table *t;
struct thread_params tptr;

// Initializing MUTEX
pthread_mutex_t mlock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t setlock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t getlock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t connectlock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t disconnectlock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t querylock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t authenticationlock = PTHREAD_MUTEX_INITIALIZER;

//LOGGING CONSTANT
//IF LOGGING = 0 -- LOGGING DISABLED
//IF LOGGING = 1 -- LOGGING TO STDOUT
//IF LOGGING = 2 -- LOGGING TO FILE
#define LOGGING 0

#define MAX_LISTENQUEUELEN 20	///< The maximum number of queued connections.
#define STRING_LENGTH 20

//Create a file pointer for logging use
//RETRIEVE THE VALUE OF LOGGING
FILE *fpSERVER;

//STORE STATUS VALUES IN THE FOLLOWING VARIABLES...assume not authenticated @ the start.
//AUTHENTICATION_STATUS 1 = NOT AUTH
//AUTHENTICATION_STATUS 0 = AUTH
//int authentication_status=1;
	//Heindrik: BEGIN TIMING handle_command
	struct timeval tvtBegin, tvtEnd, tvtDiff;
	
	// ********************************************
int global_counter=0;
int error_status;
/*
 * @brief Process a command from the client.
 *
 * @param sock The socket connected to the client.
 * @param cmd The command received from the client.
 * @return Returns 0 on success, -1 otherwise.
 */

void* thread (int clientsock){
	char holder[100]; /* used for LOGGING */
	sprintf(holder,"[LOG] Entered tread function\n");
	logger(fpSERVER,holder,LOGGING);

	//detach to avoid memory leak
	// basically frees the current thread for use
	pthread_detach(pthread_self());

	// increment thread counter
	pthread_mutex_lock(&mlock);
	global_counter++;
	tptr.conn_counter= tptr.conn_counter+1;
	pthread_mutex_unlock(&mlock);
	
	
	sprintf(holder, "[LOG] THREAD: Current amount of connections: %d \n",tptr.conn_counter);
	logger(fpSERVER,holder,LOGGING);

	if (tptr.conn_counter<=10){
	// Variable Declaration
	// ===========================================
	struct TreeDB* tree = tptr.treeptr;
	config_params params = tptr.configptr;

	// service the client
	// Get commands from client.
	int wait_for_commands = 1;
		//while (tptr.conn_counter<2){
			//if (global_counter==1)
				//break;
			//do nothing, wait for second connection
		//}
		do {
			// Read a line from the client.
			char cmd[MAX_CMD_LEN];
			int status = recvline(clientsock, cmd, MAX_CMD_LEN);
			if (status != 0) {
			// Either an error occurred or the client closed the connection.
				wait_for_commands = 0;
			}
			else {
			// Handle the command from the client.
				// * * *  Implement YACC AND LEX parser here!
				char *command = (char *)malloc(MAX_KEY_LEN);
				char *table_name = (char *)malloc(MAX_TABLE_LEN);
				char *key_name= (char *)malloc(MAX_KEY_LEN);
				char *predicates = (char *)malloc(100);
				char *value= (char *)malloc(MAX_VALUE_LEN);
				char *username = (char *)malloc(MAX_USERNAME_LEN);
				char *password = (char *)malloc(MAX_ENC_PASSWORD_LEN);
				char *reply = (char *)malloc(MAX_CMD_LEN);
				char temp_METADATA[20];
				sscanf(cmd,"%s %s %s %s %[^\n]",command,table_name,key_name, temp_METADATA,value);

				int lex_status = 0;
				if(strcmp(command, "SET")==0)
				{
			    	sprintf(holder, "[LOG] THREAD: Checking within YACC and LEX \n");
			    	logger(fpSERVER,holder,LOGGING);

					ss_scan_string(value);
					lex_status = ssparse();

					if (lex_status != 0)
					{
						lex_status = -1;
						printf("WRONG! lex_status is %d \n", lex_status);
						//errno = ERR_INVALID_PARAM;
						//return -1;
					}
					else
					{
						printf("success, error status is %d \n", lex_status);
					}
						sslex_destroy();
				}
				else if(strcmp(command, "QUERY")==0)
				{
			    	sprintf(holder, "[LOG] THREAD: Checking within YACC and LEX \n");
			    	logger(fpSERVER,holder,LOGGING);
					qq_scan_string(value);
					lex_status = qqparse();
					if (lex_status != 0)
					{
						lex_status = -1;
						printf("WRONG! lex_status is %d \n", lex_status);
						//errno = ERR_INVALID_PARAM;
						//return -1;
					}
					else
					{
					printf("success, error status is %d \n", lex_status);
					}
					qqlex_destroy();
				}
			    	sprintf(holder, "[LOG] THREAD: Calling handle command \n");
			    	logger(fpSERVER,holder,LOGGING);

					int status = handle_command(clientsock, cmd, tree, params, lex_status);
					pthread_mutex_unlock(&setlock);
					pthread_mutex_unlock(&getlock);
					pthread_mutex_unlock(&authenticationlock);
					pthread_mutex_unlock(&querylock);
					printf("The number of connections: %d\n",tptr.conn_counter);
					if (status != 0)
					wait_for_commands = 0; // Oops.  An error occured.
				}
			} while (wait_for_commands);
			}
			else if(tptr.conn_counter>10){
				sprintf(holder, "[LOG] THREAD: Too many connections \n");
				logger(fpSERVER,holder,LOGGING);
			}
			printf ("disconnecting\n");
			printf("total connections: %d\n",(global_counter-1));
			//endtime:
			printf(" THREAD: handle authentication time: ");
			gettimeofday(&tvtEnd, NULL);
			timeval_subtract(&tvtDiff, &tvtEnd, &tvtBegin);
			printf("%ld.%06ld seconds\n", tvtDiff.tv_sec, tvtDiff.tv_usec);
			// Close the connection with the client.
			pthread_mutex_lock(&disconnectlock);
			close(clientsock);
			pthread_mutex_unlock(&disconnectlock);
			// Decrement the counter
			pthread_mutex_lock(&mlock);
			tptr.conn_counter= tptr.conn_counter-1;
			pthread_mutex_unlock(&mlock);
			// LOG MESSAGE 4
			// =========================================================================================================
			sprintf(holder, "[LOG] THREAD: Closed connection \n");
			logger(fpSERVER,holder,LOGGING);
			return NULL;
} 
 
int handle_command(int sock, char *cmd, struct TreeDB* tree, config_params params, int lex_status)
{
	//Heindrik: BEGIN TIMING handle_command
	struct timeval tvhBegin, tvhEnd, tvhDiff;
	gettimeofday(&tvhBegin, NULL);
	// ********************************************


	///////////////////////////////////////////////////////////////
	char *command = (char *)malloc(MAX_KEY_LEN);
	char *table_name = (char *)malloc(MAX_TABLE_LEN);
	char *key_name= (char *)malloc(MAX_KEY_LEN);
	char *predicates = (char *)malloc(100);
	char *value= (char *)malloc(MAX_VALUE_LEN);
	char *username = (char *)malloc(MAX_USERNAME_LEN);
	char *password = (char *)malloc(MAX_ENC_PASSWORD_LEN);
	char *reply = (char *)malloc(MAX_CMD_LEN);


	// pull in command
	sscanf(cmd, "%s\n",command);
	//need to accept spaces


	 //////////////////////////////////////////////////////////////
	//LOGGER MESSAGE 1 - This is not Logger Message 1 BTW! - Heindrik
	char holder[100];
	if (fpSERVER == NULL && LOGGING == 2)
	{
		char str[200];
		strcpy(str,"Server-");
		strcat(str, currentDateTime());
		strcat(str,".log");
		fpSERVER = fopen(str, "a");
	}
	sprintf(holder, "[LOG] Processing command '%s'\n", cmd);
	logger(fpSERVER,holder,LOGGING);
	//LOG(("Processing command '%s'\n", cmd));

//////////////////////////////////////////////////////////////
//AUTHENTICATION COMPLETED HERE//////////////////////////////

/* TO BE COMPLETED HERE:
 * 1. CHECK FOR VALIDITY OF STRING BEFORE CONTINUING!
 */

	sprintf(holder, "[LOG] command: '%s'\n", command);
	logger(fpSERVER,holder,LOGGING);

	if(strcmp(command,"AUTH")==0)
	{
	if (c->concurrency==1)
		pthread_mutex_lock(&authenticationlock);
    //LOG COMMENT
	gettimeofday(&tvtBegin, NULL);
    sprintf(holder, "[LOG] Processing AUTHENTICATION command \n");
	logger(fpSERVER,holder,LOGGING);

		int error_status;
		//Retrieve the values from CMD and store them into 3 separate strings for the server to use.
		sscanf(cmd, "%s %s %s",command,username,password);

	//checking if the username and password match with the config line.
	if(strcmp(params.username, username)==0 && strcmp(params.password, password)==0 )
	  {
            //authentication_status=0;
            errno = 0;
            //LOG COMMENT
            sprintf(holder, "[LOG] Authentication Successful \n");
            logger(fpSERVER,holder,LOGGING);
	  }
	else
	  {     //LOG COMMENT
	      	sprintf(holder, "[LOG] Authentication Unsuccessful: Incorrect Username/Password \n");
            logger(fpSERVER,holder,LOGGING);

            //authentication_status = 1; //incorrect username/password entry
            errno = ERR_AUTHENTICATION_FAILED;
	  }

		//store the authentication status in the variable reply which will be sent over the network.
     	sprintf(reply, "%d\n", errno);

	    //sending the status to the buffer
	    sendall(sock, reply, strlen(reply));
	//endtime:
	printf("handle authentication time: ");
	gettimeofday(&tvhEnd, NULL);
	timeval_subtract(&tvhDiff, &tvhEnd, &tvhBegin);
	printf("%ld.%06ld seconds\n", tvhDiff.tv_sec, tvhDiff.tv_usec);

	return 0;
	}

///////////////////////////////////////////////////////////////
//////////////////GET COMMAND//////////////////////////////////
/*
 * We check to see if the user is authenticated, then
 * we check to see if the table they entered exists, then
 * we check to see if the entry they entered exists, then
 * if the two pass, we return the value of that entry.
 * Otherwise, we return error-codes.

 NOTE TO SELF: THIS FUNCTION WORKS FULLY, the only thing we must implement is the validity checking if necessary.
 */


	else if(strcmp(command,"GET")==0){
		if (c->concurrency==1)
		pthread_mutex_lock(&getlock);
	    //LOG COMMENT
        sprintf(holder, "[LOG] Processing GET command \n");
        logger(fpSERVER,holder,LOGGING);
		
		int	new_counter; // M4 ADD-ON -> GIVE THE CLIENT THE CURRENT VERSION OF KEY.
		sscanf(cmd,"%s %s %s",command,table_name,key_name);

		//BEFORE continuing, check to see if the user has authenticated. If the user has 			not then we do not allow them to access the database.
		/*if (authentication_status!=0){

			//DO NOT ALLOW TO GET WITHOUT BEING AUTHENTICATED
			error_status = ERR_NOT_AUTHENTICATED;
			errno = error_status;

			//SEND THE ERROR OVER TO THE CLIENT
	     		sprintf(reply, "%d %s\n",errno,value);
	     		sendall(sock, reply, strlen(reply));

	     		//NOT AUTHENTICATED... RETURN WITH A -1;
	     		//LOG COMMENT
                sprintf(holder, "[LOG] Processing GET command: ERROR- Not Authenticated \n");
                logger(fpSERVER,holder,LOGGING);
	     		return -1;
		}*/

	  //Check to see if a TABLE exists with the given name. Return the table.
	  struct TreeEntry* table = find(tree,table_name);

	  //OPTION1: The table does NOT exist.
		if (table == NULL)
		{
		//Table Not Found
		//ERROR MESSAGE CAN GO HERE
			error_status = ERR_TABLE_NOT_FOUND;

			//LOG COMMENT
            sprintf(holder, "[LOG] Processing GET command: Unsuccessful GET - TABLE NOT FOUND.\n");
            logger(fpSERVER,holder,LOGGING);
		}


			//If the table is NOT NULL and the pointer to entries is NOT NULL, check the possibilities.
   			//OPTION2: The table exist and there is a matching key.
   			if(table != NULL)
   			{
   				struct TreeEntry* entry = find(table->tree, key_name);

   				if(entry != NULL)
			   		{
			   		//Success
			   		value = getEntryValue(entry);
			   		//printf("THERE IS A MATCHING ENTRY \n");
			   		error_status = 0;

			   		//LOG COMMENT
                    sprintf(holder, "[LOG] Processing GET command: Successful GET of key '%s'.\n", key_name);
                    logger(fpSERVER,holder,LOGGING);
					
					//============= M4 ADD-ON TRANSACTIONS ==================
				new_counter = getCounterValue(entry);
			
				sprintf(holder, "[LOG] Processing GET command: KEY '%s' is Version: %d\n", key_name, new_counter);
				logger(fpSERVER,holder,LOGGING);	
			   	    }
				//========================================================
					
			   	

				//OPTION3: The table exists BUT there is no matching key.
				else if (entry == NULL)
				{
				//Key Not Found
				//ERROR MESSAGE CAN GO HERE

            //LOG COMMENT
            sprintf(holder, "[LOG] Processing GET command: Unsuccessful GET of key '%s' - KEY DOES NOT EXIST.\n", key_name);
            logger(fpSERVER,holder,LOGGING);

					error_status = ERR_KEY_NOT_FOUND;
				}
			}

			//CATCH UNKNOWN ERROR
   			if (error_status != ERR_KEY_NOT_FOUND && error_status != ERR_NOT_AUTHENTICATED && error_status != ERR_TABLE_NOT_FOUND && error_status != 0){
   				error_status = ERR_UNKNOWN;
			}

			errno = error_status;
	     	sprintf(reply, "%d %d %s\n",errno, new_counter,value);
	     	sendall(sock, reply, strlen(reply));

	    	//endtime:
	    	printf("handle get time: ");
	    	gettimeofday(&tvhEnd, NULL);
	    	timeval_subtract(&tvhDiff, &tvhEnd, &tvhBegin);
	    	printf("%ld.%06ld seconds\n", tvhDiff.tv_sec, tvhDiff.tv_usec);
			return 0;
	}

	// SET COMMAND
	// ==============================================================================================
	// ==============================================================================================

	else if(strcmp(command, "SET")==0)
	{
	//****  Process the string 'value' using YACC AND LEX. We will create a new 2D Array called set_values in c-> ...
	//The length of the array set_values is c->numValues ...
	//Any errors caused from Yacc and Lex must cause the server to immediately stop and send an error back to the client.
	//printf ("before lock\n");
	if (c->concurrency==1)
	pthread_mutex_lock(&setlock);
		//LOG COMMENT
	//printf ("after lock\n");
        sprintf(holder, "[LOG] Processing SET command: \n");
        logger(fpSERVER,holder,LOGGING);
		//Before anything, we check yacc and lex. It makes no sense to continue if this fails.
		

		if (lex_status == -1) //this means that the lex failed.
			{
				errno = ERR_INVALID_PARAM;
				sprintf(reply, "%d\n", errno);
				printf("VALUE OF reply, errno: %s %d", reply, errno);
			    sendall(sock, reply, strlen(reply));
				return -1;
			}
		
	    int error_status;
		char temp_METADATA[20];
		int metadata;
		sscanf(cmd,"%s %s %s %s %[^\n]",command,table_name,key_name,temp_METADATA,value);
		metadata = atoi(temp_METADATA);
		sprintf(holder,"[LOG]  SET: Metadata value from Client is: %d \n", metadata);
			logger(fpSERVER,holder,LOGGING);

	
	//================= M4 - TRANSACTION AND VERSION CHECK =================================================
	//We will traverse the BST to match the metadata value sent by the client with the one held in the BST.
	//Create a temp table...
	
	   struct TreeEntry* table2 = find(tree,table_name);
	   struct TreeEntry* entry2;
	   
	   if(table2 == NULL)
		{
			errno = ERR_TABLE_NOT_FOUND;
			sprintf(reply, "%d\n", errno);
			sendall(sock, reply, strlen(reply));
			//LOG COMMENT
            sprintf(holder, "[LOG] Processing SET command: TABLE '%s' does not exist.\n", table_name);
            logger(fpSERVER,holder,LOGGING);
            return -1;
		}
		
	   //if Entry is not existent - insert an entry...
	   else if(table2 != NULL)
		{
			//Create a temp entry
			sprintf(holder,"[LOG]  SET: STUCK 1 \n");
			logger(fpSERVER,holder,LOGGING);
			
			entry2 = find(table2->tree, key_name);

					if (entry2 == NULL){
					//If the entry does not exist, we must check the value of METADATA. 
					//If the metadata from the client is NOT zero and the key does not exist, TRANSACTION ABORT!
						if (metadata != 0){
						errno = ERR_TRANSACTION_ABORT;
						sprintf(reply, "%d\n", errno);
						sendall(sock, reply, strlen(reply));
						return 0;
						}
					}
					
					else{ //If the entry exists, we check the metadata value vs. the BST record_counter.	
						//Create a temp entry
									
		sprintf(holder,"[LOG]  SET: STUCK 2 \n");
			logger(fpSERVER,holder,LOGGING);
						
						entry2 = find(table2->tree, key_name);
						
						sprintf(holder, "[LOG] Processing SET command: KEY '%s' found, retriving Version No.\n", key_name);
						logger(fpSERVER,holder,LOGGING);
						
					int new_counter;
					new_counter = getCounterValue(entry2);
					
								if (metadata != 0){
									
									if (metadata != new_counter){
										//TRANSACTION ABORT -> the client version does not match the BST version...
										errno = ERR_TRANSACTION_ABORT;
										sprintf(reply, "%d\n", errno);
										printf("VALUE OF reply, errno: %s %d", reply, errno);
										sendall(sock, reply, strlen(reply));
										return 0;
									}
								}
						
					sprintf(holder, "[LOG] Processing SET command: KEY '%s' is the same version as the Client: %d\n", key_name, metadata);
					logger(fpSERVER,holder,LOGGING);
					
					// ============================================================
					
						
					}

		}
	
	//=======================================================================================================
					
		// Variable Declarations
		// ===========================================
		int status = 0;
		int tableCheck, keyCheck;
		int table_exist=0; //0 if it doesn't, 1 if it does
		char col_name[1000][800]; // array config
		char col_value[1000][800];
		char col_name_temp[1000][800]; // user input
		char  col_type_array[1000];
		char  col_type_temp_array[1000];
		int col_status;
		char temp_array_config[1024][MAX_COLNAME_LEN];
		int x;

		sprintf(holder,"[LOG]  SET: Before VALIDITY CHECK\n");
			logger(fpSERVER,holder,LOGGING);
		
		tableCheck = valid_string_check(table_name,0);
		keyCheck =  valid_string_check(key_name,0);

		sprintf(holder,"[LOG]  SET: Table and Key VALIDITY CHECK\n");
			logger(fpSERVER,holder,LOGGING);
		
		//valueCheck = valid_string_check(record->value,1);
		if(tableCheck!=0 || keyCheck!=0)
		{
			 errno = ERR_INVALID_PARAM;
			 return -1;
		}

		sprintf(holder,"[LOG]  LINKED_LIST: Going in to linked list.\n");
			logger(fpSERVER,holder,LOGGING);

		// find table in the LINKED LIST
		// ======================================
		// **** create a pointer to point to the head pointer of the array_table_conf (tl) Should be a global pointer
		struct table *config;
		config = c->tlist;


		// find the table which holds the configuration
		// ================================================================================
		while(config != NULL){

		sprintf(holder,"[LOG]  LINKED_LIST: Checking into the link list\n");
		logger(fpSERVER,holder,LOGGING);
			if (!strcmp(table_name,config->table_name)){
			sprintf(holder,"[LOG]  LINKED_LIST: Table has been found\n");
			logger(fpSERVER,holder,LOGGING);
				table_exist =1;
				break;
			}
			else{
				config = config->next;
			}
		}
		// if the table doesn't exist we will set an error code and return error code
		if (!table_exist){
			sprintf(holder,"[LOG]  LINKED_LIST:Table does not exist\n");
			logger(fpSERVER,holder,LOGGING);

			error_status = ERR_TABLE_NOT_FOUND;
			errno = error_status;
		 	sprintf(reply, "%d\n", errno);
		    sendall(sock, reply, strlen(reply));
			return -1;
		}
		
		//check if key exists
		int keyexist=0; // 0 - NO, 1-YES
		for(x=0;x<=(config->numkeys);x++){
			sprintf(holder,"[LOG]  LINKED_LIST: Checking if the KEY exist in\n");
			logger(fpSERVER,holder,LOGGING);
			if (!strcmp(config->array_keys[x],key_name)){
				sprintf(holder,"[LOG]  LINKED_LIST: The key DOES exist\n");
				logger(fpSERVER,holder,LOGGING);
				keyexist=1;
				break;
			}
		}
		int isNULL=0; // 0 no,1 yes

		// check if the value is NULL
		if (!strcmp(value,"NULL")){
			isNULL =1;
		}

		if (isNULL==0){


		// Store configuration parameters that are needed
		// =============================================================
		int num_cols = config->numCols;
		int nm =0;
		int num_cols_input = (c->numValues)/2;
		// check if number of columns match
				
		if (num_cols!=num_cols_input){
			sprintf(holder,"[LOG]  LINKED_LIST: ERROR Number of columns DO NOT MATCH \n");
			logger(fpSERVER,holder,LOGGING);
			error_status = ERR_INVALID_PARAM ;
			errno = error_status;
			sprintf(reply, "%d\n", errno);
			sendall(sock, reply, strlen(reply));
			return -1;
		}
		// Populate col_name
		for (x=0; x<num_cols;x++){
			strcpy(col_name[x],c->set_values[nm]);
			nm=nm+2;
			printf("col_name_input[%d]: %s\n",x,col_name[x]);
		}

		// function to store array config variables
		// ============================================================

		int cols = 1;
		int types = 3;

		// check if the the col_names are in the right area
	    for (x=0;x<num_cols;x++){
	        strcpy (col_name_temp[x],config->array_config[cols]);
	        // if its not equal then the Column names are invalid
	        if (strcmp(col_name_temp[x],col_name[x])!=0){
			sprintf(holder,"[LOG]  LINKED_LIST: Column names are invalid, does not match configuration\n");
			logger(fpSERVER,holder,LOGGING);
			error_status = ERR_INVALID_PARAM ;
			errno = error_status;
			sprintf(reply, "%d\n", errno);
			sendall(sock, reply, strlen(reply));
			return -1;
	        }
		printf("col_name_config[%d]: %s\n",x,col_name_temp[x]);
	        cols = cols +4;

	    }

	    // Function to check for the types
	    // =================================================

	    int a1=1;
	    // Populate col_value
	    for (x=0;x<num_cols;x++){
	    	strcpy(col_value[x],c->set_values[a1]);
	    	printf("col_value_input: %s \n",col_value[x]);
	    	a1 = a1+2;

	    }

		printf("col_type_user:");
	    // convert and populate the types in the input
	    for (x=0;x<num_cols;x++){
	        int type_status = type_identifier (col_value[x]);
	        col_type_temp_array[x] = type_status;
	        printf(" %d ", col_type_temp_array[x]);
	    }
	    printf("\n");

		printf("col_type_config: ");
	    //populate col_type array with the types of the array config
	    for (x=0;x<num_cols;x++){
	        if (config->array_config[types][0]=='i'){
	            col_type_array[x] = 0;
	        }
	        else if (config->array_config[types][0]=='c'){
	            col_type_array[x] = 1;
	        }
	        printf("%d ",col_type_array[x]);
	        types = types + 4;
	    }

	    // check is there is a type error
	    for (x=0;x<num_cols;x++){
	        if (col_type_array[x] != col_type_temp_array[x]){
			sprintf(holder,"[LOG]  LINKED_LIST: TYPE ERROR\n");
			logger(fpSERVER,holder,LOGGING);
			error_status = ERR_INVALID_PARAM ;
			errno = error_status;
			sprintf(reply, "%d\n", errno);
			sendall(sock, reply, strlen(reply));
			return -1;
	        }
	    }

	    int types1 =3;

	    // check for the string value and truncate if necessary
	    for (x=0;x<num_cols;x++){

		if (config->array_config[types1][0]=='c'){
	            //printf("%s\n",config->array_config[types1-1]);
	            int arraysize = size_get(config->array_config[types1]);
				
				sprintf(holder,"[LOG]  VALUE OF arraysize: %d\n", arraysize);
	            	logger(fpSERVER,holder,LOGGING);
					
				sprintf(holder,"[LOG]  SIZE OF STRING: %d\n", strlen(col_value[x]));
	            	logger(fpSERVER,holder,LOGGING);
				
	            // now truncate the object id necessary
	           if (strlen(col_value[x])>arraysize ){
			 /*
			|| strlen(col_value[x])>MAX_STRTYPE_SIZE
			error_status = ERR_INVALID_PARAM ;
			errno = error_status;
			sprintf(reply, "%d\n", errno);
			sendall(sock, reply, strlen(reply));
			return -1;
			*/		
					sprintf(holder,"[LOG]  LINKED_LIST: Object truncated, arraysize: %d\n", arraysize);
	            	logger(fpSERVER,holder,LOGGING);
	                col_value[x][arraysize]='\0';
	                //printf("%s\n",col_value[x]);
	            }
	            printf("char: %d\n",arraysize);
	        }
		
	        types1 = types1 + 4;

	    }

		}
		int keysearch=0;
		int fk=0;
		//find key in array_empty
		if (keyexist){
			sprintf(holder,"[LOG]  LINKED_LIST: KEY does exist\n");
			logger(fpSERVER,holder,LOGGING);
			while(strcmp(config->array_empty[fk],key_name)!=0){
				fk=fk+4;
				keysearch++;
			}
			int nv=0;
			if (strcmp(value,"NULL")!=0){
			sprintf(holder,"[LOG]  LINKED_LIST: Key exist value is NOT NULL, update the key\n");
			logger(fpSERVER,holder,LOGGING);
			//insert new value
			
			    //populate value
			int numCols_counter=0;
			for(x=(fk+2);numCols_counter<config->numCols;x=x+4){
				strcpy(config->array_empty[x],col_value[nv]);
				nv++;
				numCols_counter++;
			}
			}
			//delete the thing
			else if(!strcmp(value,"NULL")){
			sprintf(holder,"[LOG]  LINKED_LIST: Key Exist value given is NULL, delete the key fk:%d\n",fk);
			logger(fpSERVER,holder,LOGGING);
			config->array_config_index = config->array_config_index -4;
			printf(" VALUE OF c->array_config_index IS %d \n", config->array_config_index);
			int total_val = config->row_index + fk;
			printf("-- config->row_index + fk = total: %d + %d = %d",config->row_index,fk,total_val);
				for(x=(fk);x<total_val;x++){
					strcpy(config->array_empty[x],"#");
					//config->array_empty[x][0]='#';
					//config->array_empty[x][1]='\0';

				}
				for(x=0;x<(config->numkeys);x++){
					if (!strcmp(config->array_keys[x],key_name)){
						config->array_keys[x][0]='#';
						config->array_keys[x][1]='\0';
					}
				}
				config->numkeys = config->numkeys-1;
			}

		}
		else{
			// If the key doesn't exist yet the value is NULL then we return an error
			
			if (strcmp(value,"NULL")==0){
				sprintf(holder,"[LOG]  LINKED_LIST: Key doesn't exist and Value is NULL\n");
				logger(fpSERVER,holder,LOGGING);
				error_status = ERR_KEY_NOT_FOUND;
				errno = error_status;
				sprintf(reply, "%d\n", errno);
				sendall(sock, reply, strlen(reply));
				return -1;
			}
            // We only enter here if the key does not exist
			sprintf(holder,"[LOG]  LINKED_LIST: KEY does not exist prepare for insert\n");
			logger(fpSERVER,holder,LOGGING);
	    // We have completed parsing the value is correct we checked for both type and number
	    // Time to insert into the freaking database

	    //copy the template
	    // char temp_array_config[1024][MAX_COLNAME_LEN];

	    for (x=0;x<(config->row_index);x++){
	    	strcpy(temp_array_config[x],config->array_config[x]);
	    	printf("temp_array_config[%d]: %s\n",x,temp_array_config[x]);
			
	    }
	    printf("--------------------------\n");
	    int popkey=0;
	    int popval=2;
	    //populate key
	    for (x=0;x<(config->numCols);x++){
	    	strcpy(temp_array_config[popkey],key_name);
			strcpy(config->array_config[popkey],key_name);
	    	popkey=popkey+4;
			config->array_config_index = config->array_config_index + popkey;
			printf(" VALUE OF c->array_config_index IS %d \n", config->array_config_index);
	    	//printf("temp_array_config[%d](with key): %s\n",x,temp_array_config[x]);
	    }

	    //populate value
	    for (x=0;x<(config->numCols);x++){
	    	strcpy(temp_array_config[popval],col_value[x]);
			strcpy(config->array_config[popval],col_value[x]);
	    	popval=popval+4;
	    	//printf("temp_array_config[%d](with value): %s\n",x,temp_array_config[x]);
	    }

	    //print temporary array
	    for (x=0;x<(config->row_index);x++){
	    	printf("temp_array_config[%d](with value): %s\n",x,temp_array_config[x]);
	    }
	     printf("--------------------------\n");
	    //update key table and key counter
	    strcpy(config->array_keys[config->numkeys],key_name);
	    config->numkeys = (config->numkeys) +1;


	    //insert into array_empty
	    //1) find a free area and store into the hashtag
	    int freeA=0;
	    int hashtag_search=0;
	    while((config->array_empty[hashtag_search][0])!='#' ){
	    	hashtag_search++;
	    }
	    //insert into the array
	    for(x=0;x<(config->row_index);x++){
	    	strcpy(config->array_empty[hashtag_search+x],temp_array_config[x]);
	    }




		}//end of else

		//number of keys
		 printf("keys: %d\n",config->numkeys);
		  printf("--------------------------\n");
		//print keys array
		 for(x=0;x<config->numkeys+1;x++){
		   	printf("array_keys[%d]: %s\n",x,config->array_keys[x]);
		 }
		  printf("--------------------------\n");
		//printarray empty
	    for(x=0;x<30;x++){
	    	printf("array_empty[%d]: %s\n",x,config->array_empty[x]);
	    }




			 printf("--------------------------\n");

			 sprintf(holder,"[LOG] Entering BST\n");
			logger(fpSERVER,holder,LOGGING);

			printf("%s \n",value);
/*
		//must clear all the input values
			memset(col_name, 0, sizeof col_name);
			memset(col_value, 0, sizeof col_value);
			memset(col_name_temp, 0, sizeof col_name_temp);
			memset(c->set_values,0, sizeof c->set_values);
			col_type_array[0] = '\0';
			col_type_temp_array[0] = '\0';
*/
	// BST
	// ===========================================================================================
	//HERE, WE WILL CHECK TO ENSURE THAT WE ARE INSERTING DATA...
	//Create a temp table...
	   struct TreeEntry* table = find(tree,table_name);
	   struct TreeEntry* entry;
	   if(table == NULL)
		{
			error_status = ERR_TABLE_NOT_FOUND;

			//LOG COMMENT
            sprintf(holder, "[LOG] Processing SET command: TABLE '%s' does not exist.\n", table_name);
            logger(fpSERVER,holder,LOGGING);
            return -1;
		}

	   //if Entry is not existent - insert an entry...

	   else if(table != NULL)
		{
			//Create a temp entry
			entry = find(table->tree, key_name);
			if (entry == NULL){
				//Insert ENTRY!
				entry = createTable(key_name);
				insert(table->tree, entry);
				
			//===================== NEW addition for M4 ==================
			setCounterValue(entry,1); //initialize record_counter to 1 !
			sprintf(holder, "[LOG] Processing SET command: KEY '%s' created, record_counter is now 1.\n", key_name);
                logger(fpSERVER,holder,LOGGING);
			// ============================================================
		
				//LOG COMMENT
                sprintf(holder, "[LOG] Processing SET command: Successful INSERT of key '%s'.\n", key_name);
                logger(fpSERVER,holder,LOGGING);
			}
			else{
			
				entry = find(table->tree, key_name);
				//Create a temp entry
				//Return a pointer to that entry
				sprintf(holder, "[LOG] Processing SET command: KEY '%s' already exists, now updating.\n", key_name);
                logger(fpSERVER,holder,LOGGING);
			//===================== NEW addition for M4 ==================
			
			int new_counter;
			new_counter = getCounterValue(entry) +1;
			setCounterValue(entry,new_counter); //initialize record_counter to 1 !
			
			sprintf(holder, "[LOG] Processing SET command: KEY '%s' updated. record_counter is now: %d\n", key_name, new_counter);
            logger(fpSERVER,holder,LOGGING);
			
			// ============================================================
			
				
			}

		}

		//If the value entered is NULL, this means the user wants to delete the value of the specified key.
				if(strcmp(value, "NULL")==0){
					value = NULL;
				 }

		//If the value entered is NULL, this means we must remove the entry.
		if(value == NULL && table!=NULL)
		{
		    //LOG COMMENT
            sprintf(holder, "[LOG] Processing SET command: Key '%s' will now be removed.\n", key_name);
            logger(fpSERVER,holder,LOGGING);

			//We must change the BST to safely remove a 2D Array called value.
			removeDBEntry(table->tree, key_name);
			error_status = 0;
			errno = error_status;
		 	sprintf(reply, "%d\n", errno);
		    sendall(sock, reply, strlen(reply));
			return 0;

		}
		//If the value entered is NOT NULL, this means we add the new value.
		else if (value != NULL && table!=NULL){
			//We must change the BST to allow setting a 2D Array called value.
			setEntryValue(entry, value);
			error_status = 0;
			errno = error_status;
		 	sprintf(reply, "%d\n", errno);
		    sendall(sock, reply, strlen(reply));
			return 0;

		}
		//errno = error_status;
	 	//sprintf(reply, "%d \n", errno);
	   // sendall(sock, reply, strlen(reply));


    //LOG COMMENT
    sprintf(holder, "[LOG] Processing SET command: Successful - Now Ending. \n");
    logger(fpSERVER,holder,LOGGING);

	//endtime:
	printf("handle set time: ");
	gettimeofday(&tvhEnd, NULL);
	timeval_subtract(&tvhDiff, &tvhEnd, &tvhBegin);
	printf("%ld.%06ld seconds\n", tvhDiff.tv_sec, tvhDiff.tv_usec);
	return 0;
}
	// QUERY COMMAND
	// ==============================================================================================
	// ==============================================================================================

	else if(strcmp(command, "QUERY")==0)
{
		char str_max_key[60];
		char temp_holder[20];
		//LOG COMMENT
	        sprintf(holder, "[LOG] Processing QUERY command: ");
	        logger(fpSERVER,holder,LOGGING);			
///////////////////////////////////////////////////////////////////////////////////////////////////////
		        int error_status;
			sscanf(cmd,"%s %s %s %s %[^\n]",command,table_name,str_max_key,temp_holder,predicates);
			printf("COMMAND: %s, TABLE-NAME: %s, MAX-KEYS-STRING: %s, MAX-KEYS-INT %d, PREDICATES: %s \n", command, table_name, str_max_key, atoi(str_max_key), predicates);
///////////////////////////////////////////////////////////////////////////////////////////////////////////
				//Check yacc and lex first ... no need to continue if it fails...
			if (lex_status == -1) //this means that the lex failed.
			{
				errno = ERR_INVALID_PARAM;
			sprintf(reply, "%d\n", errno);
			sendall(sock, reply, strlen(reply));
			return -1; //when integrated to the server, error number will be returned
			}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////Christilda's Code////////////////////////////////////////////////////////////////////
// find the table which holds the configuration

		// Variable Declarations
		// ===========================================
		int tableCheck;
		int table_exist=0; //0 if it doesn't, 1 if it does

		tableCheck = valid_string_check(table_name,0);

		if(tableCheck!=0)
		{
			 errno = ERR_INVALID_PARAM;
			 return -1;
		}


		// find table in the LINKED LIST
		// ======================================
		// **** create a pointer to point to the head pointer of the array_table_conf (tl) Should be a global pointer
		struct table *config;
		config = c->tlist;


		// find the table which holds the configuration
		// ================================================================================
		while(config != NULL){

		sprintf(holder,"[LOG]  LINKED_LIST: Checking into the link list\n");
		logger(fpSERVER,holder,LOGGING);
			if (!strcmp(table_name,config->table_name)){
			sprintf(holder,"[LOG]  LINKED_LIST: Table has been found\n");
			logger(fpSERVER,holder,LOGGING);
				table_exist =1;
				break;
			}
			else{
				config = config->next;
			}
		}
		// if the table doesn't exist we will set an error code and return error code
		if (!table_exist){
			sprintf(holder,"[LOG]  LINKED_LIST:Table does not exist\n");
			logger(fpSERVER,holder,LOGGING);

			error_status = ERR_TABLE_NOT_FOUND;
			errno = error_status;
		 	sprintf(reply, "%d\n", errno);
		    sendall(sock, reply, strlen(reply));
			return -1;
		}
	
		// ================================================================================

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //Initialize all temp variables
  char keys[50][25];
  char all_matching_keys[50][25];

  int value_of_type_int_BST;
  int value_of_type_int_Query;

  int i,j,no_of_predicates; //loop for query  //i-> BST and j->query
  int k=0; //loop for keys
  int counter2=0, counter3=0,  flag=0;

  int  temp_count_keys_to_match_predicate;
  int counter_to_check_array_match = 0;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
 /*------------------------------------------Invalid Query- if the col names do not match-------------------------*/
  no_of_predicates=c->totPredicates/3;
  int b =0;
  int a = 0;
  char column_holder[80][80];
  sprintf(holder,"[LOG]  Query: No of Predicates: %d , Value of array_config_index: %d \n",no_of_predicates,config->array_config_index);
			logger(fpSERVER,holder,LOGGING);
  
  for(j=0; j< c->totPredicates; j+=3)
  {
    for(i=1; i<=config->array_config_index; i+=4)
    {
			  if(strcmp( c->predicates[j],config->array_empty[i])==0)
			  {
				//ok to continue
				flag=1;
				counter3++; //but if the same name exists in the BST?
			  }
    }
			if(flag==1){
			flag=0;
			}
			else{
			sprintf(holder,"[LOG]  Invalid Query: Column Names do not match 2: %d \n",counter3);
			logger(fpSERVER,holder,LOGGING);
			errno = ERR_INVALID_PARAM;
			sprintf(reply, "%d\n", errno);
			sendall(sock, reply, strlen(reply));
			return -1; //when integrated to the server, error number will be returned
			}
	
		for (a=0; a<b; a++){
				if (strcmp(c->predicates[j], column_holder[a]) == 0){
				//ERROR, predicate shows up more than once.
				sprintf(holder,"[LOG]  Invalid Query: Column Names do not match 1: %d \n",counter3);
				logger(fpSERVER,holder,LOGGING);
				errno = ERR_INVALID_PARAM;
				sprintf(reply, "%d\n", errno);
				sendall(sock, reply, strlen(reply));
				return -1; //when integrated to the server, error number will be returned
		
		}
				else{
				strcpy(column_holder[b], c->predicates[j]);
				}
		}
		
		if (k == 0){
				strcpy(column_holder[b], c->predicates[j]);
				k++;
				}
		b++;
	
  }
  
  /*
  	//column name found, store in a temp array for rechecking.
	for (a = 0; a<b; a++){
		//check if column name already part of predicates.
		if (strcmp(c->predicates[j], column_holder[a]) == 0){
		//ERROR, predicate shows up more than once.
		sprintf(holder,"[LOG]  Invalid Query: Column Names do not match 1: %d \n",counter3);
			logger(fpSERVER,holder,LOGGING);
    errno = ERR_INVALID_PARAM;
    sprintf(reply, "%d\n", errno);
    sendall(sock, reply, strlen(reply));
    return -1; //when integrated to the server, error number will be returned
		
		}
		else{
		strcpy(column_holder[b], c->predicates[j]);
		}
	}
	
	if (k == 0){
	strcpy(column_holder[b], c->predicates[j]);
	k++;
	}
	b++;
  */
  /*
  if(counter3!=no_of_predicates)
  {
	sprintf(holder,"[LOG]  Invalid Query: Column Names do not match 2: %d \n",counter3);
			logger(fpSERVER,holder,LOGGING);
    errno = ERR_INVALID_PARAM;
    sprintf(reply, "%d\n", errno);
    sendall(sock, reply, strlen(reply));
    return -1; //when integrated to the server, error number will be returned
  }
*/
  /*-----------------------------------------------------------------------------------------------------------------*/


  printf("\n 1");
   for(j=0; j< c->totPredicates; j+=3)//query
  {
     int count_keys_to_match_predicate=0; //resetting the counter

  for(i=1; i<=config->array_config_index; i+=4)  //BST
  {  printf("2 \n");
  if(strcmp( c->predicates[j],config->array_empty[i])==0) //if the col name matches in the BST
  {   printf("3 \n");

  if(strcmp( c->predicates[j+1], "<")==0) //if the operator matches
    {
	  printf("4 \n");
/*----------------------------------------------------INVALID QUERY---------------------------------------------------------*/
	//check if the value is assigned to the correct operator depending on the datatype
	char string_possibilities[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	char* predVal;
	char* actVal;
	predVal = strpbrk (string_possibilities, c->predicates[j+2]);
	actVal = strpbrk (string_possibilities, config->array_empty[i+1]); 
	//If either are not null, this is an invalid query check!
	
	if (predVal != NULL || actVal != NULL) //if the pointer is not NULL, then a letter was found, and this is WRONG!
	{
		sprintf(holder,"[LOG]  Invalid Query: Invalid Operator type for CHAR '<'\n");
			logger(fpSERVER,holder,LOGGING);
	    errno = ERR_INVALID_PARAM;
    	    sprintf(reply, "%d\n", errno);
    	    sendall(sock, reply, strlen(reply));
	    return -1;
	}
/*----------------------------------------------------------------------------------------------------------------------------*/
	//do the conversion of the string here...already know that it is int
	value_of_type_int_BST = atoi(config->array_empty[i+1]);
	value_of_type_int_Query = atoi( c->predicates[j+2]);

	if(value_of_type_int_BST < value_of_type_int_Query ) //if the value matches....still in one  key-value pair
	    { //YES
	    //store in array
	    printf("\n 5");
	    strcpy(keys[k], config->array_empty[i-1]);

		sprintf(holder,"[LOG] QUERY-int < ... Key: %s \n", keys[k]);
			logger(fpSERVER,holder,LOGGING);
			
	    sprintf(holder,"[LOG] QUERY-int < ... stringBST: %s \n", config->array_empty[i]);
			logger(fpSERVER,holder,LOGGING);
		k++;
		
		sprintf(holder,"[LOG] QUERY-int < ... Value of 'k': %d\n", k);
			logger(fpSERVER,holder,LOGGING);
	    //traverse through the array to keeping checking for more
	}

      //NO - traverse through the array to keeping inserting more


    }

  else  if(strcmp( c->predicates[j+1], ">")==0)
  {
      printf("6 \n");

 /*----------------------------------------------------INVALID QUERY---------------------------------------------------------*/
	//check if the value is assigned to the correct operator depending on the datatype
	char string_possibilities2[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	char* predVal2;
	char* actVal2;
	
	sprintf(holder,"[LOG]  Query > ... Value of c->predicates: %s \n",c->predicates[j+2]);
			logger(fpSERVER,holder,LOGGING);
	
	predVal2 = strpbrk (string_possibilities2, c->predicates[j+2]);
	actVal2 = strpbrk (string_possibilities2, config->array_empty[i+1]);
	
	if (predVal2 != NULL || actVal2 != NULL) //if the pointer is not NULL, then a letter was found, and this is WRONG!
	{
		sprintf(holder,"[LOG]  Invalid Query: Invalid Operator type for CHAR '>'\n");
			logger(fpSERVER,holder,LOGGING);
	    errno = ERR_INVALID_PARAM;
    	    sprintf(reply, "%d\n", errno);
    	    sendall(sock, reply, strlen(reply));
	    return -1;
	}
/*----------------------------------------------------------------------------------------------------------------------------*/

	value_of_type_int_BST = atoi(config->array_empty[i+1]);
	value_of_type_int_Query = atoi( c->predicates[j+2]);

	if(value_of_type_int_BST > value_of_type_int_Query )

      { //YES
	//store in array
	printf("\n 7");
	strcpy(keys[k],config->array_empty[i-1]);
			sprintf(holder,"[LOG] QUERY-int > ... Key: %s \n", keys[k]);
			logger(fpSERVER,holder,LOGGING);
			
	    sprintf(holder,"[LOG] QUERY-int > ... stringBST: %s \n", config->array_empty[i]);
			logger(fpSERVER,holder,LOGGING);
		k++;
		
		sprintf(holder,"[LOG] QUERY-int > ... Value of 'k': %d\n", k);
			logger(fpSERVER,holder,LOGGING);
	//traverse through the array to keeping checking for more
    }
  //NO - traverse through the array to keeping checking for more


  }
  
  else  if(strcmp( c->predicates[j+1],"=")==0)
  {
  printf("8 \n");
	sprintf(holder,"[LOG] Value of array_empty: %s , Value of c->predicates: %s \n",config->array_empty[i+1], c->predicates[j+2]);
			logger(fpSERVER,holder,LOGGING);  

  //check if the value is assigned to the correct operator depending on the datatype
	char string_possibilities3[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	char *predVal3;
	char *actVal3;
	predVal3 = strpbrk (string_possibilities3, c->predicates[j+2]);
	actVal3 = strpbrk (string_possibilities3, config->array_empty[i+1]);
	//We must check that we are comparing a string with a string!
	if (predVal3 != NULL && actVal3 != NULL) //If both are NOT null, then we are comparing 2 strings...VALID
	{
      if(strcmp(config->array_empty[i+1], c->predicates[j+2])==0)
      {
		 //YES
		//store in array

		strcpy(keys[k], config->array_empty[i-1]);
				sprintf(holder,"[LOG] QUERY-char = ... Key: %s \n", keys[k]);
			logger(fpSERVER,holder,LOGGING);
			
	    sprintf(holder,"[LOG] QUERY-char = ... stringBST: %s \n", config->array_empty[i]);
			logger(fpSERVER,holder,LOGGING);
		k++;
		
		sprintf(holder,"[LOG] QUERY-char = ... Value of 'k': %d\n", k);
			logger(fpSERVER,holder,LOGGING);
		//traverse through the array to keeping checking for more

	  //NO - traverse through the array to keeping checking for more


	}
      }
	  
  else if (predVal3 == NULL && actVal3 == NULL) //that means both are comparing two numbers!
  {
    value_of_type_int_BST = atoi(config->array_empty[i+1]);
    value_of_type_int_Query = atoi( c->predicates[j+2]);

	if(value_of_type_int_BST == value_of_type_int_Query )
      {
		{ //YES
		//store in array

		strcpy(keys[k], config->array_empty[i-1]);
				sprintf(holder,"[LOG] QUERY-int ... Key: %s \n", keys[k]);
			logger(fpSERVER,holder,LOGGING);
			
	    sprintf(holder,"[LOG] QUERY-int ... stringBST: %s \n", config->array_empty[i]);
			logger(fpSERVER,holder,LOGGING);
		k++;
		
		sprintf(holder,"[LOG] QUERY-int ... Value of 'k': %d\n", k);
			logger(fpSERVER,holder,LOGGING);
		//traverse through the array to keeping checking for more
	    }
	  //NO - traverse through the array to keeping checking for more
	}
  }
  
  else{
	sprintf(holder,"Invalid Query - Cannot Query with these values in '=' \n");
			logger(fpSERVER,holder,LOGGING);
    errno = ERR_INVALID_PARAM;
    sprintf(reply, "%d\n", errno);
    sendall(sock, reply, strlen(reply));
	return -1;
  }
  
  }
  else{
  
	sprintf(holder,"Invalid Query - if the operator does not match <,>,= OR Invalid = ... \n");
			logger(fpSERVER,holder,LOGGING);
    printf("9 \n" );
    errno = ERR_INVALID_PARAM;
    sprintf(reply, "%d\n", errno);
    sendall(sock, reply, strlen(reply));
	return -1;
  }
  counter2++;

  }//end of if

  printf("10 - End of BST \n" );

  }//end of BST

  printf("11 - End of Query ");


  } //end of query

  printf("12 - Begin checking for matches \n");

 sprintf(holder,"[LOG] QUERY: printing a key multiple times: %s \n", keys[i]);
			logger(fpSERVER,holder,LOGGING);
sprintf(holder,"[LOG] ---------------------------------------------------- \n");
			logger(fpSERVER,holder,LOGGING);
  int all_matches=0;
  int strKeys_counter, keys_counter;
  /*----------------------------------------------------------------------------------------------*/
  for(strKeys_counter=0; strKeys_counter < config->numkeys; strKeys_counter++)  //stringKeys
  {    int matches_counter=0;
    for(keys_counter=0; keys_counter<k; keys_counter++) //keys
    {
      if(strcmp(config->array_keys[strKeys_counter],keys[keys_counter])==0)
      {

	matches_counter++;
	 sprintf(holder,"[LOG] QUERY-matches_counter: Number of matches: %d \n", matches_counter);
			logger(fpSERVER,holder,LOGGING);
      }
    }

	if(matches_counter == no_of_predicates)
	{

	  strcpy(all_matching_keys[all_matches], config->array_keys[strKeys_counter]);
	sprintf(holder,"[LOG] ---------------------------------------------------- \n");
			logger(fpSERVER,holder,LOGGING);
	sprintf(holder,"[LOG] printing each key once:%s \n", all_matching_keys[all_matches]);
			logger(fpSERVER,holder,LOGGING);
	  all_matches++;

	}
  }
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Declare a 1D array to send back to the client.
char *keys_sent_to_the_client = (char *)malloc(800);
for(i=0; i<all_matches; i++)
    {
    strcat(keys_sent_to_the_client, all_matching_keys[i]);
    strcat(keys_sent_to_the_client, " ");
    }

		sprintf(holder,"[LOG] QUERY: 1D Array-going to storage-Constructed: %s \n", keys_sent_to_the_client);
			logger(fpSERVER,holder,LOGGING);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	        sprintf(holder, "[LOG] Ending QUERY command \n");
	        logger(fpSERVER,holder,LOGGING);

		//%d -> str_keys_found
		//%s ->str_keys_array
		errno = 0;
	     	sprintf(reply, "%d %d %s\n", errno, all_matches, keys_sent_to_the_client);
	        	sendall(sock, reply, strlen(reply));
			return 0;


	}

	free(command);
	free(table_name);
	free(key_name);
	free(value);
	free(username);
	free(password);
    free(reply);

	return 0;
}

/**
 * @brief Start the storage server.
 *
 * This is the main entry point for the storage server.  It reads the
 * configuration file, starts listening on a port, and processes
 * commands from clients.
 */
int main(int argc, char *argv[])
{
	printf("\n>> Server starting\n");
	char holder[100];

	char str[200];
	strcpy(str,"Server-");
	strcat(str, currentDateTime());
	strcat(str,".log");
	if (LOGGING == 2){
		fpSERVER = fopen(str, "w");
	}

	//LOG COMMENT
    	sprintf(holder, "[LOG] Server Starting: ... \n");
    	logger(fpSERVER,holder,LOGGING);
	//CREATE THE DATABASE POINTER - This pointer will be the link between the server and the 		database.
	struct TreeDB* tree = createTreeDB();

    	//LOG COMMENT
    	sprintf(holder, "[LOG] Server Starting: Database Created \n");
    	logger(fpSERVER,holder,LOGGING);
	/////////////////////////////////////

	// Process command line arguments.
	// This program expects exactly one argument: the config file name.
	assert(argc > 0);
	if (argc != 2) {
		printf("Usage %s <config_file>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	//This stores the name of the CONFIG FILE
	extern FILE *yyin;
	FILE *f;
	char tempStr[30];
	strcpy(tempStr, argv[1]);
	f = fopen(tempStr,"r");
	yyin = f;

	//READ THE CONFIG FILE HERE using YACC and LEX
	while(! feof(yyin) )  {
		int status = yyparse();
	      if (status!= 0){
	    	  status = -1;
	    		printf(">> Error processing config file.\n");
	    		exit(EXIT_FAILURE);
	      }
	}

	//If there are no errors parsing from YACC and LEX, we must now check for missing and duplicates.
	if (strcmp(c->host,"$$$")==0 || strcmp(c->password,"$$$")==0 || c->port == 0 || strcmp(c->username,"$$$")==0 || c->num_tables == 0)
	{
		printf(">> Error processing config file.\n");
				exit(EXIT_FAILURE);
	}
	
	if (c->concurrency != 0 && c->concurrency != 1){
		printf(">> Error processing config file.\n");
				exit(EXIT_FAILURE);
	}

	//If we get here, we do not have errors in the config file, and we can continue.
	config_params params;
	int status;
	c->tlist = tl;
    	strcpy(params.password, c->password);
    	strcpy(params.username, c->username);
    	strcpy(params.server_host, c->host);
    	params.server_port = c->port;


    	//Check Number of Columns!
    	struct table *temp;

      printf("Configuration:\n Host: %s, port: %d, data directory: %s\n",
    	 c->host, c->port);

        temp = c->tlist;
        printf("  Table(s):\n");
        while(temp != NULL){

          printf("	%s \n",temp->table_name);

    	//PRINT THE 2D Array for EACH TABLE
    	int a;
    	for (a = 0; a < temp->row_index; a++)
        	{
                printf("	%s %d \n", temp->array_config[a], temp->numCols);
            }
            printf("\n");


    	temp=temp->next;
        }
        printf("\n");


	//IF THERE ARE NO ERRORS PROCESSING THE CONFIG FILE, WE STORE THE TABLES IN THE DATABASE.
	//Insert Configuration Tables

		int counter;
		for(counter = 0; counter < c->num_tables;counter++)
		{
			struct TreeEntry* table = createTable(c->all_table_names[counter]);
			insert(tree, table);
		}

	//////////////////////////////////////////////////////////////
	//LOG MESSAGE 2

	sprintf(holder,"[LOG] Server on %s:%d\n", params.server_host,params.server_port);
	logger(fpSERVER, holder,LOGGING);
	//LOG(("Server on %s:%d\n", params.server_host,params.server_port));

	//////////////////////////////////////////////////////////////

// BEGIN CONNECTION
// =========================================================================
	// initiate pthreads
	pthread_t myid;	
	
	// Create a socket.
	int listensock = socket(PF_INET, SOCK_STREAM, 0);
	if (listensock < 0) {
		printf("Error creating socket.\n");
		exit(EXIT_FAILURE);
	}

	// Allow listening port to be reused if defunct.
	int yes = 1;
	status = setsockopt(listensock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);
	if (status != 0) {
		printf("Error configuring socket.\n");
		exit(EXIT_FAILURE);
	}

	// Bind it to the listening port.
	struct sockaddr_in listenaddr;
	memset(&listenaddr, 0, sizeof listenaddr);
	listenaddr.sin_family = AF_INET;
	listenaddr.sin_port = htons(params.server_port);
	inet_pton(AF_INET, params.server_host, &(listenaddr.sin_addr)); // bind to local IP address
	status = bind(listensock, (struct sockaddr*) &listenaddr, sizeof listenaddr);
	if (status != 0) {
		printf("Error binding socket.\n");
		exit(EXIT_FAILURE);
	}

	// Listen for connections.
	status = listen(listensock, MAX_LISTENQUEUELEN);
	if (status != 0) {
		printf("Error listening on socket.\n");
		exit(EXIT_FAILURE);
	}
	
	tptr.conn_counter=0;
	
	// Listen loop.
	int wait_for_connections = 1;
	while (wait_for_connections) {
		// Wait for a connection.
		struct sockaddr_in clientaddr;
		socklen_t clientaddrlen = sizeof clientaddr;
		int clientsock = accept(listensock, (struct sockaddr*)&clientaddr, &clientaddrlen);
		if (clientsock < 0) {
			printf("Error accepting a connection.\n");
			exit(EXIT_FAILURE);
		}

		//////////////////////////////////////////////////////////////
		//LOG MESSAGE 3
		sprintf(holder,"[LOG] Got a connection from %s:%d.\n", inet_ntoa(clientaddr.sin_addr), clientaddr.sin_port);
		logger(fpSERVER,holder,LOGGING);
		//LOG(("Got a connection from %s:%d.\n", inet_ntoa(clientaddr.sin_addr), clientaddr.sin_port));
		//////////////////////////////////////////////////////////////

		//Heindrik: BEGIN TIMING handle_command
		struct timeval tvsysBegin, tvsysEnd, tvsysDiff;
		gettimeofday(&tvsysBegin, NULL);
		// ********************************************
		if (c->concurrency==1){

			sprintf(holder,"[LOG] Multi-Threading Enabled\n");
			logger(fpSERVER,holder,LOGGING);
			sprintf(holder,"[LOG] TID: %d\n",(int)myid);
			logger(fpSERVER,holder,LOGGING);

			// Update the global struct to have params and tree
			tptr.treeptr = tree;
			tptr.configptr = params;
			sprintf(holder,"[LOG] Stored all variables into the struct\n");
			logger(fpSERVER,holder,LOGGING);

			//creates the ptread and also checks if an error does occur
			if (pthread_create(&myid,NULL,thread,clientsock)){
				sprintf(holder,"[LOG] Error in pthread_create\n");
				logger(fpSERVER,holder,LOGGING);
				errno = 7;
				return -1;
			}
		}
		else if (c->concurrency==0){
		
		// Get commands from client.
		int wait_for_commands = 1;
		do {
			// Read a line from the client.
			char cmd[MAX_CMD_LEN];
			int status = recvline(clientsock, cmd, MAX_CMD_LEN);
			if (status != 0) {
				// Either an error occurred or the client closed the connection.
				wait_for_commands = 0;
			} else {
				// Handle the command from the client.

		    	sprintf(holder, "[LOG] Declarings variables \n");
		    	logger(fpSERVER,holder,LOGGING);

				// * * *  Implement YACC AND LEX parser here!
				char *command = (char *)malloc(MAX_KEY_LEN);
				char *table_name = (char *)malloc(MAX_TABLE_LEN);
				char *key_name= (char *)malloc(MAX_KEY_LEN);
				char *predicates = (char *)malloc(100);
				char *value= (char *)malloc(MAX_VALUE_LEN);
				char *username = (char *)malloc(MAX_USERNAME_LEN);
				char *password = (char *)malloc(MAX_ENC_PASSWORD_LEN);
				char *reply = (char *)malloc(MAX_CMD_LEN);
				char temp_METADATA[20];
				sscanf(cmd,"%s %s %s %s %[^\n]",command,table_name,key_name, temp_METADATA,value);

				int lex_status = 0;
				if(strcmp(command, "SET")==0)
				{
			    	sprintf(holder, "[LOG] Checking within YACC and LEX \n");
			    	logger(fpSERVER,holder,LOGGING);
					ss_scan_string(value);
					lex_status = ssparse();

					if (lex_status != 0)
					{
						lex_status = -1;
						printf("WRONG! lex_status is %d \n", lex_status);
						//errno = ERR_INVALID_PARAM;
						//return -1;
					}
					else
					{
						printf("success, error status is %d \n", lex_status);
					}

					sslex_destroy();
				}
				else if(strcmp(command, "QUERY")==0)
				{
			    	sprintf(holder, "[LOG] Checking within YACC and LEX \n");
			    	logger(fpSERVER,holder,LOGGING);
					qq_scan_string(value);
					lex_status = qqparse();

					if (lex_status != 0)
					{
						lex_status = -1;
						printf("WRONG! lex_status is %d \n", lex_status);
						//errno = ERR_INVALID_PARAM;
						//return -1;
					}
					else
					{
						printf("success, error status is %d \n", lex_status);
					}

					qqlex_destroy();
				}
		    	sprintf(holder, "[LOG] Calling handle command \n");
		    	logger(fpSERVER,holder,LOGGING);
				int status = handle_command(clientsock, cmd, tree, params, lex_status);
				if (status != 0)
					wait_for_commands = 0; // Oops.  An error occured.
			}
		} while (wait_for_commands);
		
		// Close the connection with the client.
		close(clientsock);

		//endtime:
		printf("handle system time: ");
		gettimeofday(&tvsysEnd, NULL);
		timeval_subtract(&tvsysDiff, &tvsysEnd, &tvsysBegin);
		printf("%ld.%06ld seconds\n", tvsysDiff.tv_sec, tvsysDiff.tv_usec);
		//////////////////////////////////////////////////////////////
		//LOG MESSAGE 4
		sprintf(holder, "[LOG] Closed connection from %s:%d.\n", inet_ntoa(clientaddr.sin_addr), clientaddr.sin_port);
		logger(fpSERVER,holder,LOGGING);
		//LOG(("Closed connection from %s:%d.\n", inet_ntoa(clientaddr.sin_addr), clientaddr.sin_port));
		//////////////////////////////////////////////////////////////
	}
}
	// Stop listening for connections.
	close(listensock);

	//CLOSE FILE!
	fclose(fpSERVER);

	deleteTreeDB(tree);
	free(tree);

	return EXIT_SUCCESS;
}


