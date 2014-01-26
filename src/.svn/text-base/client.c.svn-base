/**
 * @file
 * @brief This file implements a "very" simple sample client.
 * 
 * The client connects to the server, running at SERVERHOST:SERVERPORT
 * and performs a number of storage_* operations. If there are errors,
 * the client exists.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include "storage.h"
#include "utils.h"

//TO LOG ALL CLIENT CALLS, WE WILL SHARE a CLIENT FILE POINTER
extern FILE *fpCLIENT;
struct timeval tvautoBegin, tvautoEnd, tvautoDiff;


char* newLineRemover (char* input){
	int x;
	for (x=0;x< strlen(input);x++){
		if (input[x]=='\n'){
			input[x]='\0';
		}
	}
	return input;
}

/**
 * @brief Start a client to interact with the storage server.
 *
 * If connect is successful, the client performs a storage_set/get() on
 * TABLE and KEY and outputs the results on stdout. Finally, it exists
 * after disconnecting from the server.
 */
int main(int argc, char *argv[]) {

//Client option stored in this variable
int userInput;
char myline[800];


// HEINDRIK: ALGORITHM TO RUN THE SET COMMANDS FOR THE SERVER! ***********
	char tempStr[30];
	strcpy(tempStr, argv[1]);
	int error_occurred = 0;
	int auto_on = 0; // if 1 auto is on
	//Open file for reading.
	FILE *file = fopen(tempStr, "r");
	if (file == NULL){
		printf ("FILE does not EXIST\n");
		error_occurred = 1;
	}
	else
		printf ("FILE found!\n");


// ***********************************************************************

//Used to hold values from server.c and storage.c
int status;

//allocate memory for user strings
char *SERVERHOST = (char*)malloc(MAX_HOST_LEN*sizeof(char));
char *SERVERUSERNAME = (char*)malloc(MAX_USERNAME_LEN*sizeof(char));
char *SERVERPASSWORD = (char*)malloc(1000*sizeof(char));
char *TABLE = (char*)malloc(MAX_TABLE_LEN*sizeof(char));
char *KEY = (char*)malloc(MAX_KEY_LEN*sizeof(char));
char *PREDICATES = (char*)malloc(MAX_VALUE_LEN*sizeof(char));
char transaction; //Check if the user wants / does not want to complete a get-set transaction...

//FOR QUERY:
	// Create an empty keys array.
	// No need to free this memory ...who cares anyways.
	int i;
	char* test_keys[MAX_RECORDS_PER_TABLE];
	for (i = 0; i < MAX_RECORDS_PER_TABLE; i++) {
		test_keys[i] = (char*)malloc(MAX_KEY_LEN);
		strncpy(test_keys[i], "", sizeof(test_keys[i]));
	}


//Hold new key here
char newKey[MAX_KEY_LEN]; //CONSTANT
int SERVERPORT;

//Stores values from storage.c and server.c of connection status.
void *conn = NULL;

struct storage_record r;

//User options which are displayed
	printf("> -------------------- \n");
	printf("> 1) Connect \n");
	printf("> 2) Authenticate \n");
	printf("> 3) Get \n");
	printf("> 4) Set \n");
	printf("> 5) Disconnect \n");
	printf("> 6) Exit \n");
	printf("> 7) Auto_ON \n");
	printf("> 8) Auto_OFF \n");
	printf("> 9) QUERY \n");
	printf("> -------------------- \n");

//Create a while loop with options for the user to select from in the shell

while (userInput != 6){
	
	// HEINDRIK: COMMAND ALGORITHM *************************************
	
	if (auto_on){
		// Read a line from the file.
		char line[256];
		char *l = fgets(line, sizeof line, file);
		// Process the line.
		if (l==line){
		  char command[100];
                char l2[100];
                char l3[100];
                char l4[100];
				char l5[100];
      	sscanf(line,"%s %s %s %[^\n]",command,l2,l3,l4);
		//printf("SSCANF: %s %s %s %s\n", command,l2,l3,l4,l5);
		userInput = atoi(command);
		// assign proper variables from command
		if (userInput == 1) {
			strcpy(SERVERHOST,l2);
			SERVERPORT = atoi(l3);
		}
		else if (userInput == 2){
			strcpy(SERVERUSERNAME,l2);
			strcpy(SERVERPASSWORD,l3);
		}
		else if (userInput == 3){
			strcpy(TABLE,l2);
			strcpy(KEY,l3);
		}
		else if (userInput == 4){
			strcpy(TABLE,l2);
			strcpy(KEY,l3);
			strcpy(newKey,l4);
		}
		else if (userInput == 9){
		}
		}
	}
	if (userInput==8){
		//output of time difference	
		printf("END-END TIME: ");
		gettimeofday(&tvautoEnd, NULL);
		timeval_subtract(&tvautoDiff, &tvautoEnd, &tvautoBegin);
		printf("%ld.%06ld seconds\n", tvautoDiff.tv_sec, tvautoDiff.tv_usec);

		auto_on =0;
	}

	
	//******************************************************************
	
	printf("> Please enter your selection: ");

	//Retrieve user option selection
	if (auto_on==0){
	fgets(myline, sizeof myline, stdin);
	userInput = atoi(myline);
	}

	//OPTION 1: Connect - Necessary Components: ServerName and PORT - found in default.config file
	if (userInput == 1){

		//ENTER HOSTNAME
		printf("> Please enter the hostname: ");
		if (auto_on==0){
			fgets(myline, sizeof myline, stdin);
			char* newline = newLineRemover(myline);
			strcpy (SERVERHOST,newline);
		}

		//ENTER PORT NUMBER
		printf("> Please enter the port: ");
		if (auto_on==0){
			fgets(myline, sizeof myline, stdin);
			SERVERPORT = atoi(myline);
		}


		//TELL USER WHAT IS HAPPENING AT THE CURRENT POINT
		printf("> Connecting to %s:%d ... \n", SERVERHOST, SERVERPORT);

		// Connect to server
		conn = storage_connect(SERVERHOST, SERVERPORT);

	  if(!conn) {
	    printf("> Cannot connect to server @ %s:%d. Error code: %d.\n",
           SERVERHOST, SERVERPORT, errno);
	    return -1;
	  }
	  else{
	  printf("> Connection to server @  %s:%d successful! \n", SERVERHOST, SERVERPORT);
	  }
	}
////////////////////////////////////////////////////////////////////////////

	//OPTION 2: Authenticate - The user must input the same username and password as found in the default.config file
	else if (userInput == 2){
	//ENTER USERNAME
		if (auto_on==0){
			printf("> Please enter your username: ");
			fgets(myline, sizeof myline, stdin);
			char* newline = newLineRemover(myline);
			strcpy (SERVERUSERNAME,newline);
		}

		//ENTER PASSWORD
		if (auto_on==0){
			printf("> Please enter your password: ");
			fgets(myline, sizeof myline, stdin);
			char* newline = newLineRemover(myline);
			strcpy (SERVERPASSWORD,newline);
		}

		//BEGIN AUTHENTICATION
		printf("> Authentication in progress... \n");

		// Authenticate the client.
		status = storage_auth(SERVERUSERNAME, SERVERPASSWORD, conn);
		if(status != 0) {
		printf("storage_auth failed with username '%s' and password '%s'. "
           "Error code: %d.\n", SERVERUSERNAME, SERVERPASSWORD, errno);
    	int status1 = storage_disconnect(conn);
	if(status1 != 0) {
		//printf("storage_disconnect failed\n");
	}
	else if (status1 == 0){
		//printf("> Successfully Disconnected from Server! \n");
		conn = NULL;
	}
  	}
  	else
	  printf("storage_auth: successful.\n");
	}

////////////////////////////////////////////////////////////////////////////
	//OPTION 3: GET RECORD ONLY IF YOU ARE CONNECTED TO SERVER
	else if (userInput == 3){
	
	// Heindrik: BEGIN TIMING STORAGE_GET
	//struct timeval tvgetBegin, tvgetEnd, tvgetDiff;
	//gettimeofday(&tvgetBegin, NULL);
	// ===========================================

	if (auto_on==0){
	printf("> Enter a table: ");
	fgets(myline, sizeof myline, stdin);
	char* newline = newLineRemover(myline);
	strcpy (TABLE,newline);
	}

	if (auto_on==0){
	printf("> Enter a key: ");
	if (auto_on==0)
	fgets(myline, sizeof myline, stdin);
	char* newline = newLineRemover(myline);
	strcpy (KEY,newline);
	}

	 // Issue storage_get
  	status = storage_get(TABLE, KEY, &r, conn);

	//ouput of time difference	
	//printf("time for get: ");
	//gettimeofday(&tvgetEnd, NULL);
	//timeval_subtract(&tvgetDiff, &tvgetEnd, &tvgetBegin);
	//printf("%ld.%06ld seconds\n", tvgetDiff.tv_sec, tvgetDiff.tv_usec);

  if(status != 0) {

    printf("storage_get failed. Error code: %d.\n", errno);
    status = storage_disconnect(conn);
		if(status != 0) {
			//printf("storage_disconnect failed\n");
		}
		else if (status == 0){
			//printf("> Successfully Disconnected from Server! \n");
			conn = NULL;

  	}
  }
  else{

printf("storage_get: the value returned for key '%s' is '%s'.\n",
         KEY, r.value);
	  }

  }

////////////////////////////////////////////////////////////////////////////

	//OPTION 4: SET RECORD ONLY IF YOU ARE CONNECTED TO SERVER
	else if (userInput == 4){

	  if (auto_on==0){
	  printf("> Enter a table to look-up: ");
		fgets(myline, sizeof myline, stdin);
		char* newline = newLineRemover(myline);
		strcpy (TABLE,newline);
	  }

	  if (auto_on==0){
	  printf("> Enter a key: ");
		fgets(myline, sizeof myline, stdin);
		char* newline = newLineRemover(myline);
		strcpy (KEY,newline);
	  }
	  
	  if (auto_on==0){
	  printf("> Set part of Transaction? y/n: ");
		fgets(myline, sizeof myline, stdin);
		transaction = 0;//myline[0];
	//========= M4 TRANSACTIONS ======================================================
		if (transaction == 'n'){
		memset(r.metadata, 0, sizeof(r.metadata));
		printf("METADATA SET TO 0 ! \n");
	  }
	  //We assume that if the user says the set IS INDEED part of a transaction, then they already called GET, and retrieved a version number.
	//==================================================================================
	}
	  if (auto_on==0){
	  printf("> Enter a change of value for the key: ");
	  if (auto_on==0)
		fgets(myline, sizeof myline, stdin);
		char* newline = newLineRemover(myline);
		strcpy (newKey,newline);
	  }

  	strcpy(r.value, newKey);
  	printf ("::: %s ",r.value);
  	//CHANGE BACK &r
  	status = storage_set(TABLE, KEY, &r, conn);

  	if(status != 0) {
    	   printf("storage_set failed. Error code: %d.\n", errno);
  	}
  	else{
	   printf("\n storage_set: successful.\n");
  	}

  }

////////////////////////////////////////////////////////////////////////////

	//OPTION 5: DISCONNECT ONLY IF YOU ARE CONNECTED TO SERVER INITIALLY

	else if(userInput == 5){
  	//Disconnect from server
  	status = storage_disconnect(conn);
  		if(status != 0) {
   		 printf("storage_disconnect failed. Error code: %d.\n", errno);
    		//return status;
  		}
  		else if (status == 0){
		printf("> Successfully Disconnected from Server! \n");
		conn = NULL;
		//connected = 0;
	}

 }	

////////////////////////////////////////////////////////////////////////////

	else if (userInput == 6){

	  // Exit

	  printf("> Exiting the program...\n");
/*
	  //Free allocated memory!
	  free (SERVERHOST);
	  free (SERVERUSERNAME);
	  free (SERVERPASSWORD);
	  free (TABLE);
	  free (KEY);

	  //flush and close the document properly.
	  fflush(fpCLIENT);
	  fclose(fpCLIENT);
*/
	  return 0;
  }


	//turns the auto shell on and reads command from the file
	else if (userInput == 7){
		// Heindrik: BEGIN AUTOTIME
		
		gettimeofday(&tvautoBegin, NULL);
		// ===========================================
		if (error_occurred ==1)
			printf ("> Error: File Not Found\n");
		else{
			printf ("> AutoShelling ON \n");
			auto_on = 1;
		}
	}
	
	
	else if (userInput == 9){
	if (auto_on==0){
	printf("> Enter a table: ");
	fgets(myline, sizeof myline, stdin);
	char* newline = newLineRemover(myline);
	strcpy (TABLE,newline);
	}

	if (auto_on==0){
	printf("> Enter predicates: ");
	fgets(myline, sizeof myline, stdin);
	char* newline = newLineRemover(myline);
	strcpy (PREDICATES,newline);
	}
	
	status = storage_query(TABLE, PREDICATES, test_keys, MAX_RECORDS_PER_TABLE, conn);

  	if(status == -1) {
    	   printf("storage_query failed. Error code: %d.\n", errno);
  	}
  	else{
	   printf("\n storage_query: successful, found %d matching entries.\n", status);
  	}
	
	}

	

	else{
	  printf("> Please enter a valid option! \n");
	  }

	if (!auto_on){
		printf("> -------------------- \n");
		printf("> 1) Connect \n");
		printf("> 2) Authenticate \n");
		printf("> 3) Get \n");
		printf("> 4) Set \n");
		printf("> 5) Disconnect \n");
		printf("> 6) Exit \n");
		printf("> 7) Auto_ON \n");
		printf("> 8) Auto_OFF \n");
		printf("> 9) QUERY \n");
		printf("> -------------------- \n");
	}

	//}
	}

//END OF PARSER
}

