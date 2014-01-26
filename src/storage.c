/**
 * @file
 * @brief This file contains the implementation of the storage server
 * interface as specified in storage.h.
 */



#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include "storage.h"
#include "client.h"
#include "utils.h"
#include <sys/time.h>
#define LOGGING 1

// global variable to determine auth status
int AUTH_STATUS=1; // 0 if connected, any other number is an error

//Create a global variable that will hold all the logger messages.
char holder[100];
//TO LOG ALL CLIENT CALLS, WE WILL SHARE a CLIENT FILE POINTER
FILE *fpCLIENT = NULL;

void* storage_connect(const char *hostname, const int port)
{
	if(hostname == NULL)
	{
		errno = ERR_INVALID_PARAM;
		return NULL;
	}
	// Create a socket.
	int sock = socket(PF_INET, SOCK_STREAM, 0);

	

	if (sock < 0)
		return NULL;

	// Get info about the server.
	struct addrinfo serveraddr, *res;
	memset(&serveraddr, 0, sizeof serveraddr);
	serveraddr.ai_family = AF_UNSPEC;
	serveraddr.ai_socktype = SOCK_STREAM;
	char portstr[MAX_PORT_LEN];
	snprintf(portstr, sizeof portstr, "%d", port);
	int status = getaddrinfo(hostname, portstr, &serveraddr, &res);

	if (status != 0){
		//CONNECTION FAILS, due to INVALID hostname/port
		errno = ERR_INVALID_PARAM;
		return NULL;
	}

	// Connect to the server.
	status = connect(sock, res->ai_addr, res->ai_addrlen);

	if (status != 0)
		{//CONNECTION FAILS SO ERROR.
		errno = ERR_CONNECTION_FAIL;
		return NULL;
}
	/**
	 *
         * @brief Client connects!
	 *REASONING: We will have to ensure that the user correctly enters the required hostname and password.
	 *we will LOG what the user enters for the hostname and password, as well as what time they connect.
	 *NOTE: If this log does not get written, the client has likely not entered a valid hostname and/or password
	 *LOGGER 1!
	 *If connection to server not made, return NULL,
	 *ELSE if connection made, return socket value (integer).
         */
	  ///////////////////////////////////////////////////////////
	if (fpCLIENT == NULL && LOGGING == 2)
	{
			char str[200];
			strcpy(str,"Client-");
			strcat(str, currentDateTime());
			strcat(str,".log");
			fpCLIENT = fopen(str, "a");
	}
	sprintf(holder, "[LOG] Client has successfully connected @ %s, socket number %d! \n", currentDateTime(),sock);
	logger(fpCLIENT,holder,LOGGING);
	////////////////////////////////////////////////////////////
	
	//AUTH_STATUS = ERR_NOT_AUTHENTICATED; //Not sure why this is here...
	errno=0;
	return (void*) sock;
}


/**
 * @brief This is the authentication function, this function will send the client information to the 
 * server and verify that information with the deafult.conf file. Once the information has been verified
 * this function will return a code depending on the status of the verification. if it returned 0 then the 
 * authentication is successful. If another number is returned however, then an error occured. The type of 
 * error is determined by the code that is returned.
 */
int storage_auth(const char *username, const char *passwd, void *conn)
{
	//We have to handle commands that cannot be handled by the server.
	if (conn == NULL || username == NULL || passwd == NULL){
		errno = ERR_INVALID_PARAM;
		return -1;	
	}
	
	//CANNOT AUTH IF NOT CONNECTED!
	if(!conn)
	{
		errno = ERR_CONNECTION_FAIL;
		return -1;
	}

	//int auth_status=0; //WHAT IS THIS?

	// Connection is really just a socket file descriptor.
	int sock = (int)conn;

	// Send some data.
	char buf[MAX_CMD_LEN];
	memset(buf, 0, sizeof buf);

	char *encrypted_passwd = generate_encrypted_password(passwd, NULL);
	
	snprintf(buf, sizeof buf, "AUTH %s %s\n", username, encrypted_passwd);

	// if sendall is zero, the value is sent to server successfully
	// if recvline is zero, the value is received succcessfully

	if (sendall(sock, buf, strlen(buf)) == 0 && recvline(sock, buf, sizeof buf) == 0) {
		//auth_status = atoi(buf);
		//errno=auth_status;
	
		//Check if authentication successful!		
		if (atoi(buf) == 0){
			//NO ERROR, Send Success.
			AUTH_STATUS = 0; 
			errno = 0;			
			return 0; //success
		}
		else if (atoi(buf) == ERR_AUTHENTICATION_FAILED){
			
			//ERROR, Auth FAILED.			
			AUTH_STATUS = ERR_AUTHENTICATION_FAILED;
			errno = ERR_AUTHENTICATION_FAILED;			
			return -1; //failure
		}
		
		else{
			errno = ERR_UNKNOWN;
			return -1; //failure
		}

	}

	
	//Should never get here, but if we do:
	errno = ERR_UNKNOWN;
	return -1;

}


/**
 * @brief This is just a minimal stub implementation.  You should modify it 
 * according to your design.
 */
int storage_get(const char *table, const char *key, struct storage_record *record, void *conn)
{
	sprintf(holder, "[LOG]: Begin of GET-function [err:%d] \n",errno);
	logger(fpCLIENT,holder,LOGGING);
	
	// Checks if the conn has been made before allowing function get to be called
	if (table == NULL || key == NULL || conn == NULL || record == NULL)
	{
		errno = ERR_INVALID_PARAM;
		return -1;
	}

	//INVALID PARAMS TAKES PRIORITY BEFORE AUTH STATUS!
	sprintf(holder, "[LOG]PASSED THE CONNECTION IF \n");
	logger(fpCLIENT,holder,LOGGING);

	int tableCheck, keyCheck;
	tableCheck = valid_string_check(table,0);
	keyCheck = valid_string_check(key,0);

	if(tableCheck!=0 || keyCheck!=0 )
	{
		 errno = ERR_INVALID_PARAM;
		 return -1;
	}
	// *********************************************

	// Check if client has been authenticated
	if (AUTH_STATUS != 0){
		errno = ERR_NOT_AUTHENTICATED;
		return -1;	
	}
	
	// Connection is really just a socket file descriptor.
	int sock = (int)conn;

	// Send some data.
	char buf[MAX_CMD_LEN];
	memset(buf, 0, sizeof buf);
	snprintf(buf, sizeof buf, "GET %s %s \n" , table, key);

	if (sendall(sock, buf, strlen(buf)) == 0 && recvline(sock, buf, sizeof buf) == 0) {
               

		sprintf(holder, "[LOG] Database successfully returns table: %s and key: %s \n",table,key);
		logger(fpCLIENT,holder,LOGGING);

	int GET_STATUS;
	char CHAR_GET_STATUS[20];
	//================= M4 UPDATE =================================
	int METADATA;
	char char_METADATA[20];
	//To avoid the GET REQUEST value to not be cut off, we must 
		
		// The following line will start reading a number (%d) followed by anything until a new line(%[^\t\n]).
		sscanf(buf,"%s %s %[^\n] \n",CHAR_GET_STATUS, char_METADATA, record->value);
		GET_STATUS = atoi(CHAR_GET_STATUS); //Could possibly be an issue!! //M4 UPDATE!
		
		sprintf(holder, "[LOG] GET STATUS: %d \n", GET_STATUS);
		logger(fpCLIENT,holder,LOGGING);

		if(GET_STATUS==0){
			METADATA = atoi(char_METADATA);
			sprintf(holder, "[LOG] METADATA IN STORAGE IS: %d \n", METADATA);
			logger(fpCLIENT,holder,LOGGING);
		
			record->metadata[0] = METADATA;
			
			sprintf(holder, "[LOG] VALUE IN STORAGE IS: %s \n", record->value);
			logger(fpCLIENT,holder,LOGGING);
			errno = 0;
			return 0; //SUCCESS
		}
	//==================== M4 UPDATE ====================================
		else{
			sprintf(holder, "[LOG] VALUE IS: %s \n", record->value);
			logger(fpCLIENT,holder,LOGGING);
			if (GET_STATUS == ERR_TABLE_NOT_FOUND){
			errno = ERR_TABLE_NOT_FOUND;
			return -1;
			}
			else if (GET_STATUS == ERR_KEY_NOT_FOUND){
			errno = ERR_KEY_NOT_FOUND;
			return -1;			
			}
			
		}

	}
	//SHOULD NEVER GET HERE, BUT IF SO:
	//Set special errno
	errno = ERR_UNKNOWN;
	return -1;
}


/**
 * @brief This is just a minimal stub implementation.  You should modify it 
 * according to your design.
 */
int storage_set(const char *table, const char *key, struct storage_record *record, void *conn)
{	
	sprintf(holder, "[LOG] Entering SET\n");
	logger(fpCLIENT,holder,LOGGING);
	//Why is this here?
	char value[MAX_VALUE_LEN] ;

	//Cannot connect if NULL!
	// ===============================================================

	// Checks if the conn has been made before allowing function get to be called
	if (table == NULL || key == NULL  || conn == NULL)
	{
		errno = ERR_INVALID_PARAM;
		return -1;
	}

	// Variable Declarations
	// ===========================================
	int tableCheck, keyCheck;

	// Check if table and key are alphanumeric
	// ===================================================
	tableCheck = valid_string_check(table,0);
	keyCheck =  valid_string_check(key,0);
	
	sprintf(holder, "[LOG] Checking table\n");
	logger(fpCLIENT,holder,LOGGING);
	if(tableCheck!=0 || keyCheck!=0)
	{
		 errno = ERR_INVALID_PARAM;
		 return -1;
	}		
	sprintf(holder, "[LOG] authentication status %d\n",AUTH_STATUS);
	logger(fpCLIENT,holder,LOGGING);

	// Check if client has been authenticated
	if (AUTH_STATUS != 0){
		errno = ERR_NOT_AUTHENTICATED;
		return -1;	
	}
	sprintf(holder, "[LOG] Checking Record\n");
	logger(fpCLIENT,holder,LOGGING);
	int METADATA;
	char string_METADATA[20];
	if (record == NULL){
		strcpy(value,"NULL");
		printf("FOUND NULL! \n");
		METADATA = 0;
	}
	else {
		strcpy(value,record->value);
		METADATA = record->metadata[0];
		
		sprintf(holder, "[LOG] STORAGE_SET INT METADATA %d! \n",METADATA);
		logger(fpCLIENT,holder,LOGGING);
	}
	
	// Send some data(buf) into server.c
	// ================================================================
	// Connection is really just a socket file descriptor.
	int sock = (int)conn;
	int status;
	char buf[MAX_CMD_LEN];
	memset(buf, 0, sizeof buf);


	sprintf(holder, "[LOG] STORAGE_SET socket number %d! \n",sock);
	logger(fpCLIENT,holder,LOGGING);
	sprintf(holder, "[LOG] Setting buffer\n");
	logger(fpCLIENT,holder,LOGGING);
	// insert, table, key into the buffer
	//int METADATA = *((int*)(&(record->metadata)));
	
	sprintf(holder, "[LOG] METADATA from storage_set() going to server as: %d! \n",METADATA);
	logger(fpCLIENT,holder,LOGGING);
	
	sprintf(buf, "SET %s %s %d %s\n",table, key, METADATA, value);
	
	// LOG 
	// ========================================================================================================
	sprintf(holder,"[LOG]Client Is now calling sendall & receiveall: %s to %s @ %s \n",key,record->value ,currentDateTime());
	logger(fpCLIENT,holder,LOGGING);

	int check_sendall = sendall(sock, buf, strlen(buf));
	int check_recvline = recvline(sock, buf, sizeof buf) ;

	sprintf(holder, "[LOG] BUFFER-VALUE %d, SENDALL-VAL %d, RECV-VAL %d\n", atoi(buf), check_sendall, check_recvline);
			logger(fpCLIENT,holder,LOGGING);

	if (check_sendall == 0 && check_recvline == 0) {
		errno = atoi(buf);
		sprintf(holder, "[LOG] ERRNO in STORAGE_SET inside sendall/recv %d\n", errno);
		logger(fpCLIENT,holder,LOGGING);
		if (errno == 0) //SUCCESS
		{
			return 0;
		}
		else{
			return -1;
		}
		
	}

	//Should never get here, but if it does;
	//Send special error code:
	//errno = ERR_UNKNOWN;
	errno = ERR_INVALID_PARAM; //SHOULD NOT BE DOING THIS!!!
	sprintf(holder, "[LOG] ERRNO in STORAGE_SET outside sendall/recv %d\n", errno);
			logger(fpCLIENT,holder,LOGGING);

	return -1;
}


int storage_query(const char *table, const char *predicates, char **keys, const int max_keys, void *conn){

	if (table == NULL || predicates == NULL || conn == NULL) 
	{
		errno = ERR_INVALID_PARAM;
		return -1;
	}

	if (keys == NULL && max_keys != 0){
	errno = ERR_INVALID_PARAM;
		return -1;
	
	}

	// Check if client has been authenticated
	if (AUTH_STATUS != 0){
		errno = ERR_NOT_AUTHENTICATED;
		return -1;	
	}
	int tableCheck;	
	
	// Check if table and key are alphanumeric
	// ===================================================
	tableCheck = valid_string_check(table,0);
	
	if(tableCheck!=0 )
	{
		 errno = ERR_INVALID_PARAM;
		 return -1;
	}	

	// Send some data(buf) into server.c
	// ================================================================
	// Connection is really just a socket file descriptor.
	int sock = (int)conn;
	int status;
	char buf[MAX_CMD_LEN];
	memset(buf, 0, sizeof buf);
	
	// Convert max_keys into string before sending into buff -> WHY?!
	char string1[20];	    
	sprintf(string1,"%d",max_keys);
	
	// insert, table, key into the buffer
	//SEND IN THIS ORDER BECAUSE PREDICATES MAY HAVE SPACES.
	
	int garbage = 999;
	sprintf(buf, "QUERY %s %s %d %s \n", table, string1, garbage , predicates); //Why is there a \n ...
	

	//When we return we need some variables to set up the 2D array 'keys':
	int all_keys_length;
	char all_keys_string[1024]; //random size because we do not know how long the string can be.
	char errno_string[30]; //temporary errno value as a string
	char all_keys_length_string[30]; //temporary length of 1D keys value


	if (sendall(sock, buf, strlen(buf)) == 0 && recvline(sock, buf, sizeof buf) == 0) {
		sprintf(holder, "[LOG] Before the SSCANF breakdown \n");
			logger(fpCLIENT,holder,LOGGING);

		sscanf(buf,"%s %s %[^\n]\n",errno_string, all_keys_length_string, all_keys_string);

		errno = atoi(errno_string);
		all_keys_length= atoi(all_keys_length_string);

		
		sprintf(holder, "[LOG] After the SSCANF breakdown: %d %d %s \n", errno, all_keys_length, all_keys_string);
			logger(fpCLIENT,holder,LOGGING);

		if (errno == ERR_TABLE_NOT_FOUND || errno == ERR_INVALID_PARAM){
		printf(holder, "[LOG] Unsuccessful Query, returning to client. \n");
			logger(fpCLIENT,holder,LOGGING);
		return -1;		
		}
		
		else{ //SUCCESS so errno is already 0 thus;
		printf("SUCCESS \n");

		if (keys == NULL){
			return all_keys_length;
		}
char temp[800][800];

	 int row=0, col=0, i=0;
	//use row to keep track of all the keys.

	//We must find the length of the 1D string called all_keys_string.
	int n = strlen(all_keys_string);
	sprintf(holder, "[LOG] Size of string: %d \n", n);
			logger(fpCLIENT,holder,LOGGING);

			//add counters
			 for(i=0; i<n; i++)
			 {
				
				if(all_keys_string[i] == ' ')
			   {
				temp[row][col]='\0';
				sprintf(holder, "[LOG] Space Found! \n");
				logger(fpCLIENT,holder,LOGGING);
				 row++;
				 col=0;
			   }
			   else{
					temp[row][col]=all_keys_string[i];
			sprintf(holder, "[LOG] Print the char transferred %c \n", temp[row][col]);
			logger(fpCLIENT,holder,LOGGING);
			   col++;
			   }
			   
			 }
			
			sprintf(holder, "[LOG] Before printing the final string...\n");
			logger(fpCLIENT,holder,LOGGING);
			
			if (all_keys_length<=max_keys){
			sprintf(holder, "[LOG] If all_keys_length <= max_keys...\n");
			logger(fpCLIENT,holder,LOGGING);
			for (i=0; i<all_keys_length; i++){

			 strcpy(keys[i], temp[i]);

			 sprintf(holder, "[LOG] Print the string transferred %s \n", keys[i]);
			 logger(fpCLIENT,holder,LOGGING);
			}
			
			
			}
			else{
			sprintf(holder, "[LOG] If all_keys_length > max_keys...\n");
			logger(fpCLIENT,holder,LOGGING);
			for (i=0; i<max_keys; i++){

			strcpy(keys[i], temp[i]);

			 sprintf(holder, "[LOG] Print the string transferred %s \n", keys[i]);
			logger(fpCLIENT,holder,LOGGING);
			}
			
			
			}

		return all_keys_length;
	    } //end of else - success
		
	} // end of if for recv and sendall == 0

	//SHOULD NEVER GET HERE BUT IF SO:
	//SET ERROR;
	//errno = ERR_UNKNOWN;
	errno = ERR_INVALID_PARAM; //shouldn't be doing this!!!
	return -1;
}


/**
 * @brief This is just a minimal stub implementation.  You should modify it 
 * according to your design.
 */
int storage_disconnect(void *conn)
{
	// Cleanup
	// LOGGER 4!
	// ============================================================================
	if (fpCLIENT == NULL && LOGGING == 2)
	{
		char str[200];
		strcpy(str,"Client-");
		strcat(str, currentDateTime());
		strcat(str,".log");
		fpCLIENT = fopen(str, "a");
	}
	// ============================================================================

	if(conn != NULL)
	{
		int sock = (int)conn;
		close(sock);
		errno = 0;
	}
	else
	{
		errno = ERR_INVALID_PARAM;
		return -1;
	}
	// LOGGER 4!
	// REASONING: We must keep track of when the client connects and disconnects from the server. We must ensure that the socket closes properly
	// when they disconnect.
	sprintf(holder, "[LOG] Client has successfully disconnected @ time: %s \n",currentDateTime());
	logger(fpCLIENT,holder,LOGGING);


	return 0;

}

	

