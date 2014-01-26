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


// HEINDRIK: ALGORITHM TO RUN THE SET COMMANDS FOR THE SERVER! ***********
	
	int error_occurred = 0;
	int auto_on = 0; // if 1 auto is on
	//Open file for reading.
	FILE *file = fopen("commandcensus.txt", "r");
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

//Hold new key here
char newKey[MAX_KEY_LEN]; //CONSTANT
int SERVERPORT;

//To keep track in the shell on whether the user is connected/disconnected from server
//We state: when connected == 0 > DISCONNECTED & when connected == 1 > CONNECTED
int connected = 0;

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
	printf("> -------------------- \n");

//Create a while loop with options for the user to select from in the shell

while (userInput != 6){
	
	// HEINDRIK: COMMAND ALGORITHM *************************************
	
	if (auto_on){
		// Read a line from the file.
		char line[MAX_CONFIG_LINE_LEN];
		char *l = fgets(line, sizeof line, file);
		// Process the line.
		if (l==line){
		  char command[100];
                char l2[100];
                char l3[100];
                char l4[100];
		  char l5[100];
                		int items = sscanf(line,"%s %s %s %s",command,l2,l3,l4,l5);
				printf("SSCANF: %s %s %s %s\n", command,l2,l3,l4,l5);
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
				
		}
		if (userInput==8)
			auto_on =0;
	}		
//	}
	
	//******************************************************************
	
	printf("> Please enter your selection: ");

	//Retrieve user option selection
	if (auto_on==0)
	scanf("%d", &userInput);

	//OPTION 1: Connect - Necessary Components: ServerName and PORT - found in default.config file
	if (userInput == 1){

	//ENTER HOSTNAME
	printf("> Please enter the hostname: ");
	if (auto_on==0)
	scanf("%s\n", SERVERHOST);

	//ENTER PORT NUMBER
	printf("> Please enter the port: ");
	if (auto_on==0)
	scanf("%d", &SERVERPORT);

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
	  connected = 1;
  }


	}
////////////////////////////////////////////////////////////////////////////

	//OPTION 2: Authenticate - The user must input the same username and password as found in the default.config file
	else if (userInput == 2 && connected == 1){
	//ENTER USERNAME
	if (auto_on==0){
	printf("> Please enter your username: ");
	scanf("%s", SERVERUSERNAME);
	}

	//ENTER PASSWORD
	if (auto_on==0){
	printf("> Please enter your password: ");
	scanf("%s", SERVERPASSWORD);
	}

	//BEGIN AUTHENTICATION
	printf("> Authentication in progress... \n");

// Authenticate the client.
  status = storage_auth(SERVERUSERNAME, SERVERPASSWORD, conn);
  if(status != 0) {
    printf("storage_auth failed with username '%s' and password '%s'. "
           "Error code: %d.\n", SERVERUSERNAME, SERVERPASSWORD, status);
    storage_disconnect(conn);
  }
  else
	  printf("storage_auth: successful.\n");

	  }

	//OPTION 2: ERROR catching from the shell; must connect before authenticating...
	else if (userInput == 2 && connected == 0){
		printf("> ERROR! You must connect to server before you can authenticate! \n");
	}

////////////////////////////////////////////////////////////////////////////
	//OPTION 3: GET RECORD ONLY IF YOU ARE CONNECTED TO SERVER
	else if (userInput == 3 && connected == 1){
	
	// Heindrik: BEGIN TIMING STORAGE_GET
	//struct timeval tvgetBegin, tvgetEnd, tvgetDiff;
	//gettimeofday(&tvgetBegin, NULL);
	// ********************************************	

	if (auto_on==0){
	printf("> Enter a table: ");
	scanf("%s", TABLE);
	}
	
	if (auto_on==0){
	printf("> Enter a key: ");
	if (auto_on==0)
	scanf("%s", KEY);
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
    storage_disconnect(conn);
  }
  else{

printf("storage_get: the value returned for key '%s' is '%s'.\n",
         KEY, r.value);
	  }

  }

	//OPTION 3: //ERROR catching from the shell; must connect to server before using storage_get()
	else if (userInput == 3 && connected == 0){
		printf("> ERROR! Cannot GET from server, you must connect first! \n");
	}
////////////////////////////////////////////////////////////////////////////

	//OPTION 4: SET RECORD ONLY IF YOU ARE CONNECTED TO SERVER
	else if (userInput == 4 && connected == 1){

	  if (auto_on==0){
	  printf("> Enter a table to look-up: ");
	  scanf("%s", TABLE);
	  }

	  if (auto_on==0){
	  printf("> Enter a key: ");
	  scanf("%s", KEY);
	  }

	  if (auto_on==0){
	  printf("> Enter a change of value for the key: ");
	  if (auto_on==0)
	  scanf("%s", newKey);
	  }

  strcpy(r.value, newKey);
  status = storage_set(TABLE, KEY, &r, conn);
  printf("%d",status);

  if(status != 0) {
    printf("storage_set failed. Error code: %d.\n", errno);
    storage_disconnect(conn);
  }
  else{
	   printf("storage_set: successful.\n");
  }

  }
	//OPTION 4: ERROR catching from shell; must connect to server before using storage_set()
	else if (userInput == 4 && connected == 0){
		printf("> ERROR! Cannot SET key in database... you must connect first! \n");
	}

////////////////////////////////////////////////////////////////////////////

	//OPTION 5: DISCONNECT ONLY IF YOU ARE CONNECTED TO SERVER INITIALLY

	else if(userInput == 5 && connected == 1){
  //Disconnect from server
  status = storage_disconnect(conn);
  if(status != 0) {
    printf("storage_disconnect failed. Error code: %d.\n", errno);
    return status;
  }
  else if (status == 0){
	printf("> Successfully Disconnected from Server! \n");
	connected = 0;
	}

 }
	//OPTION 5: ERROR catching from shell; can only disconnect if initally connected!
	else if (userInput == 5 && connected == 0){
			    printf("> ERROR! You may not be connected to the server! \n");
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
		if (error_occurred ==1)
			printf ("> Error: File Not Found\n");
		else{		
			printf ("> AutoShelling ON \n");
			auto_on = 1;
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
		printf("> -------------------- \n");
	}
	
  }

//END OF PARSER

}

