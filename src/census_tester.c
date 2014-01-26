/*
 * census_tester.c
 *
 *  Created on: 2013-02-18
 *      Author: marjimat
 */

//IMPORT EXISTING CLIENT SHELL BUT AUTOMATE IT TO READ FROM THE CENSUS FILE AND RUN ACCORDINGLY.
//NOTE: The census file that this reads from has been specifically formatted to meet the specifications of the current server, which is:
/*
 * CONNECT, AUTHENTICATE, then use get/set commands.
 * ....
 * Also note, this program will simply populate the database with entries from the file!
 * If successful and there are no errors, we can safely assume that all values are stored properly.
 * To test if this is true, we could automate get commands as well to ensure the entries exist.
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "storage.h"
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include "utils.h"
#include "client.h"

#define SERVERHOST "localhost"
#define SERVERPORT 2111
#define SERVERUSERNAME "admin"
#define SERVERPASSWORD "dog4sale"
#define TABLE "marks"
#define KEY "ece297"

FILE *output;
void options(bool connected, bool authenticated);
void omit_newLine(char *l);
/**
 * @brief Start a client to interact with the storage server.
 *
 * If connect is successful, the client performs a storage_set/get() on
 * TABLE and KEY and outputs the results on stdout. Finally, it exists
 * after disconnecting from the server.
 */
int main(int argc, char *argv[]) {

	//Log for client side
	int operationCount = 0;
	char *fileLogName_client = newClientFileName();
	if(LOGGING == 0 || LOGGING == 1)
			;
	else
		fileptr = fopen(fileLogName_client, "w");

	char testfile[100] = "ParsedCensusData";
	char outputchar[100] = "output";
	char line0[1000], line1[1000], line2[1000], line3[1000], line4[1000];
	FILE *test = fopen(testfile, "r");
	output = fopen(outputchar, "w");
	//status variables
	bool connected = false;
	bool authenticated = false;
	char *message = (char*)malloc(sizeof(char)*1000);
	char * hostname= (char*)malloc(sizeof(char)*1000);
	char *s_port= (char*)malloc(sizeof(char)*1000);
	char *user_Name= (char*)malloc(sizeof(char)*1000);
	char *password= (char*)malloc(sizeof(char)*1000);
	char *table_get = (char*)malloc(sizeof(char)*1000);
	char *key_get = (char*)malloc(sizeof(char)*1000);
	char *table_set = (char*)malloc(sizeof(char)*1000);
	char *key_set= (char*)malloc(sizeof(char)*1000);
	char *value_set= (char*)malloc(sizeof(char)*1000);
	int port;

	void *conn = NULL;
	struct storage_record r;
	options(connected, authenticated);

	char *l = fgets(line1, sizeof line1, test);
	omit_newLine(l);
	char* instruction = (char*)malloc(sizeof(char)*1000);
	sstream(&l, instruction, MAX_STR_LEN);
	while(true)
	{
		/*int i;
		for(i = 0; i < 10000000; i++)
			;*/
		if(connected)
		{
			if(authenticated)
			{
				if(strcmp(instruction, "3") == 0)
						{
									// Issue storage_get
									//fprintf(output, "> Please input a table to access: \n");
									sstream(&l, table_get, MAX_STR_LEN);
									sstream(&l, key_get, MAX_STR_LEN);

									int status = storage_get(table_get, key_get, &r, conn);
									if(status != 0) {

										sprintf(message, "\n\n\n%d> storage_get failed. Error code: %d.\n", operationCount, errno);
										logger(fileptr, message, LOGGING);
										if(status == ERR_CONNECTION_FAIL)
										{
											storage_disconnect(conn);
											connected = false;
										}
										operationCount++;
									}
									else
									{
										sprintf(message, "\n\n\n%d> storage_get: the value returned for key '%s' is '%s'.\n",operationCount,
											key_get, r.value);
										logger(fileptr, message, LOGGING);
										operationCount++;
									}
						}
						else if(strcmp(instruction, "4") == 0)
						{
							// Issue storage_set
							//fprintf(output, "> Please input a table to set: \n");
							sstream(&l, table_set, MAX_STR_LEN);
							sstream(&l, key_set, MAX_STR_LEN);
							sstream(&l, value_set, MAX_STR_LEN);

							strncpy(r.value, value_set, sizeof r.value);
							int status = storage_set(table_set, key_set, &r, conn);
							if(status != 0) {
								sprintf(message, "\n\n\n\n%d> storage_set failed. Error code: %d.\n",operationCount, errno);
								logger(fileptr, message, LOGGING);
								if(status == ERR_CONNECTION_FAIL)
								{
									storage_disconnect(conn);
									connected = false;
								}
								operationCount++;
							}
							else
							{
								sprintf(message, "\n\n\n\n%d> storage_set: successful.\n",operationCount);
								logger(fileptr, message, LOGGING);
								operationCount++;
							}

						}
						else if(strcmp(instruction, "5") == 0)
						{
							  // Disconnect from server
							int status = storage_disconnect(conn);
							if(status != 0) {
								sprintf(message, "\n%d>storage_disconnect failed. Error code: %d.\n",operationCount, errno);
								logger(fileptr, message, LOGGING);
								operationCount++;
							}
							else
							{
								connected = false;
								authenticated = false;
								sprintf(message, "\n%d> Disconnected successfully.\n",operationCount);
								logger(fileptr, message, LOGGING);
								operationCount++;
							}
						}
						else if(strcmp(instruction, "6") == 0)
						{
							// Exit
							sprintf(message, "\n%d> Program terminated.", operationCount);
							logger(fileptr, message, LOGGING);
							operationCount++;
							break;
						}
						else if(strcmp(instruction, "\0") == 0)
						{
							l = fgets(line1, sizeof line1, test);
							l = fgets(line0, sizeof line0, test);
						}
						else
						{
							sprintf(message, "\n%d> Input a command listed above. Try again.\n",operationCount);
							logger(fileptr, message, LOGGING);
							operationCount++;
						}
			}
			else
			{
				if(strcmp(instruction, "2") == 0)
						{
							authenticated = true;
							sstream(&l, user_Name, MAX_STR_LEN);
							sstream(&l, password, MAX_STR_LEN);

							int status = storage_auth(user_Name, password, conn);
							if(status != 0) {
								sprintf(message, "\n\n\n%d> storage_auth failed with username '%s' and password '%s'. " \
										"Error code: %d.\n", operationCount,user_Name, password, errno);
								logger(fileptr, message, LOGGING);
								if(status == ERR_CONNECTION_FAIL)
								{
									storage_disconnect(conn);
									connected = false;
								}
								operationCount++;
				  			}
							else
							{
								authenticated = true;
								sprintf(message, "\n\n\n%d> storage_auth: successful.\n",operationCount);// Authenticate the  client.
								logger(fileptr, message, LOGGING);
								operationCount++;
							}
						}
				else if(strcmp(instruction, "5") == 0)
						{
							  // Disconnect from server

							int status = storage_auth(user_Name, password, conn);
							status = storage_disconnect(conn);
							if(status != 0) {
								sprintf(message, "\n%dstorage_disconnect failed. Error code: %d.\n",operationCount, errno);
								logger(fileptr, message, LOGGING);
								operationCount++;
							}
							else
							{
								connected = false;
								authenticated = false;
								fprintf(output, "\n%d> Disconnected successfully.\n",operationCount);
								operationCount++;
							}
						}
				else if(strcmp(instruction, "6") == 0)
						{
							// Exit
							connected = false;
							authenticated = false;
							sprintf(message, "\n%d> Program terminated.",operationCount);
							logger(fileptr, message, LOGGING);
							operationCount++;
							break;
						}
				else if(strcmp(instruction, "\0") == 0)
				{
					l = fgets(line1, sizeof line1, test);
					l = fgets(line0, sizeof line0, test);
				}
				else
					{
						sprintf(message, "\n%d> Input a command listed above. Try again.\n",operationCount);
						logger(fileptr, message, LOGGING);
						operationCount++;
					}

			}
		}
		else
		{
			if(strcmp(instruction, "1") == 0)
					{
						//fprintf(output, "> Please input the hostname: \n");
						sstream(&l, hostname, MAX_STR_LEN);
						sstream(&l, s_port, MAX_STR_LEN);
						port = atoi(s_port);
						conn = storage_connect(hostname, port);
						if(!conn) {
							sprintf(message, "\n\n\n%d> Cannot connect to server @ %s:%d. Error code: %d.\n",operationCount,
									hostname, port, errno);
							logger(fileptr, message, LOGGING);
							operationCount++;
						}
						else
						{
							connected = true;
							sprintf(message, "\n\n\n%d> Connected to server.\n",operationCount);
							logger(fileptr, message, LOGGING);
							operationCount++;
						}
					}
			else if(strcmp(instruction, "6") == 0)
				{
					// Exit
					connected = false;
					authenticated = false;
					sprintf(message, "\n%d> Program terminated.",operationCount);
					logger(fileptr, message, LOGGING);
					operationCount++;
					break;
				}
			else if(strcmp(instruction, "\0") == 0)
			{
				l = fgets(line1, sizeof line1, test);
				l = fgets(line0, sizeof line0, test);
			}
			else
					{
						sprintf(message, "\n%d> Input a command listed above. Try again.\n",operationCount);
						logger(fileptr, message, LOGGING);
						operationCount++;
					}
		}
		//checks for null character
		if(strcmp(instruction, "\0") == 0)
		{
			;
		}
		else
		{
			options(connected, authenticated);
			l = fgets(line1, sizeof line1, test);
			sstream(&l, instruction, MAX_STR_LEN);
		}

	}
	free(fileLogName_client);
	close(fileptr);
	close(test);
	close(output);
	    //for connect to server

	return 0;
}
void options(bool connected, bool authenticated)
{
	/*if(connected && authenticated)
	{
		fprintf(output, "*********************************************************\n");
		fprintf(output, "****Connection Established,Authentication Established****\n");
		fprintf(output, "*********************************************************\n");
		fprintf(output, "> Press 3 to Retrieve Data\n");
		fprintf(output, "> Press 4 to Set Data\n");
		fprintf(output, "> Press 5 to Disconnect\n");
		fprintf(output, "> Press 6 to Terminate Program\n");
		fprintf(output, "*********************************************************\n");
		fprintf(output, "> Please enter your selection: \n");
	}
	else if(connected)
	{
		fprintf(output, "*********************************************\n");
		fprintf(output, "********* Connection Established ************\n");
		fprintf(output, "****** Authentication Not Established *******\n");
		fprintf(output, "*********************************************\n");
		fprintf(output, "> Enter 2 to Authenticate\n");
		fprintf(output, "> Enter 5 to Disconnect\n");
		fprintf(output, "> Enter 6 to Terminate Program\n");
		fprintf(output, "*********************************************\n");
		fprintf(output, "> Please enter your selection: \n");
	}
	else
	{
		fprintf(output, "*********************************************\n");
		fprintf(output, "********* No Connection Established *********\n");
		fprintf(output, "****** Authentication Not Established *******\n");
		fprintf(output, "*********************************************\n");
		fprintf(output, "> Enter 1 to Connect\n");
		fprintf(output, "> Enter 6 to Terminate Program\n");
		fprintf(output, "*********************************************\n");
		fprintf(output, "> Please enter your selection: \n");

	}*/

}
void omit_newLine(char *l)
{
	int count = 0;
	while(*(l+count) != '\n')
	{
		count++;
	}
	*(l + count) = '\0';
}
