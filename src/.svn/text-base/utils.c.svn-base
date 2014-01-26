/**
 * @file
 * @brief This file implements various utility functions that are
 * can be used by the storage server and client library. 
 */

#define _XOPEN_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>
#include "utils.h"
#include <math.h>

int sendall(const int sock, const char *buf, const size_t len)
{
	size_t tosend = len;
	while (tosend > 0) {
		ssize_t bytes = send(sock, buf, tosend, 0);
		if (bytes <= 0) 
			break; // send() was not successful, so stop.
		tosend -= (size_t) bytes;
		buf += bytes;
	};

	return tosend == 0 ? 0 : -1;
}

/**
 * In order to avoid reading more than a line from the stream,
 * this function only reads one byte at a time.  This is very
 * inefficient, and you are free to optimize it or implement your
 * own function.
 */
int recvline(const int sock, char *buf, const size_t buflen)
{
	int status = 0; // Return status.
	size_t bufleft = buflen;

	while (bufleft > 1) {
		// Read one byte from scoket.
		ssize_t bytes = recv(sock, buf, 1, 0);
		if (bytes <= 0) {
			// recv() was not successful, so stop.
			status = -1;
			break;
		} else if (*buf == '\n') {
			// Found end of line, so stop.
			*buf = 0; // Replace end of line with a null terminator.
			status = 0;
			break;
		} else {
			// Keep going.
			bufleft -= 1;
			buf += 1;
		}
	}
	*buf = 0; // add null terminator in case it's not already there.

	return status;
}


/**
 * @brief Parse and process a line in the config file.
 */
int process_config_line(char *line, config_params *params, int s_table)
{
	int x;
	
	// Ignore comments.
	if (line[0] == CONFIG_COMMENT_CHAR)
		return 0;

	// Extract config parameter name and value.
	char name[MAX_CONFIG_LINE_LEN];
	char value[MAX_CONFIG_LINE_LEN];
	//We assume we will absorb only the first two strings we see.
	int items = sscanf(line, "%s %s \n", name, value);

	// Line wasn't as expected.
	if (items != 2)
		return -1;

	// Process this line.
	if (strcmp(name, "server_host") == 0) {
		//strncpy(params->server_host, value, sizeof params->server_host);
		return 1;
	} else if (strcmp(name, "server_port") == 0) {
		//params->server_port = atoi(value);
		return 2;
	} else if (strcmp(name, "username") == 0) {
		//strncpy(params->username, value, sizeof params->username);
		return 3;
	} else if (strcmp(name, "password") == 0) {
		//strncpy(params->password, value, sizeof params->password);
		return 4;
	}
	else if (strcmp(name, "table")==0){
		// Heindrik: check for any duplicates
		for (x = 0; x <= s_table; x++){
			if (strcmp(value, params->table_names[x])==0)
				return 6; 
		}
		//if no duplicates, program will proceed
		strncpy(params->table_names[s_table], value, sizeof params->table_names[s_table]);
		return 5;
	} else if (strcmp(name, "#") == 0) {
		// Ignore any parameters that begin with "#"
	}
	// else if (strcmp(name, "data_directory") == 0) {
	//	strncpy(params->data_directory, value, sizeof params->data_directory);
	//} 
	else {
		// Ignore unknown config parameters.
	}
    return 0;
}


int read_config(const char *config_file, config_params *params)
{
	int error_occurred = 0;
	int params_ID = 0;
	int s_host = 0;
	int s_port = 0;
	int s_user = 0;
	int s_pass = 0;
	int s_table = 0; //counts number of TABLE
	
	// Open file for reading.
	FILE *file = fopen(config_file, "r");
	if (file == NULL)
		error_occurred = 1;

	// Process the config file.
	while (!error_occurred && !feof(file)) {
		// Read a line from the file.
		char line[MAX_CONFIG_LINE_LEN];
		char *l = fgets(line, sizeof line, file);
		
		// Heindrik: Track the number of paramaters
		if (params_ID == 1)
			s_host++;
		else if (params_ID == 2)
			s_port++;
		else if (params_ID == 3)
			s_user++;
		else if (params_ID == 4)
			s_pass++;
		else if (params_ID == 5)
			s_table++;
		
		params->num_tables =s_table; //WE HAVE THE NUMBER OF TABLES THAT EXIST!

		// Heindrik: Check if the there are duplicates
		if (s_host > 1 ||s_port > 1 || s_user > 1 || s_pass > 1 || params_ID == 6)
			return ERR_INVALID_PARAM;
		
		// Process the line.
		if (l == line)
			params_ID = process_config_line(line, params,s_table);
		else if (!feof(file))
			error_occurred = 1;
	}
	
	// Heindrik: check if missing arguments
	if (s_host==0||s_port==0||s_user==0||s_pass==0||s_table==0){
		return ERR_INVALID_PARAM;
	}

	return error_occurred ? -1 : 0;
}

void logger(FILE *file, char *message, int LOGGING)
{

	if (LOGGING == 0){
		//do nothing
	}
	else if (LOGGING == 1){
		printf("%s",message);
	}
	else{
		fprintf(file,"%s",message);
		fflush(file);
	}

}

//Access the time.h file to print the current date and time based on the machine.
char *currentDateTime(){
	time_t currentTime= time(0);
	struct tm tstruct;
	char *buffer= (char*)malloc(100*sizeof(char));
	tstruct = *localtime(&currentTime);

	strftime(buffer,100,"%Y-%m-%d-%H-%M-%S", &tstruct);
	return buffer;
}

char *generate_encrypted_password(const char *passwd, const char *salt)
{
	if(salt != NULL)
		return crypt(passwd, salt);
	else
		return crypt(passwd, DEFAULT_CRYPT_SALT);
}

int valid_string_check( char* input, int value){
	//if value is == 1 then we are checking for spaces if not then spaces are not allowed
	
	int i;
	// check for Alpha or Numeric characters
	for ( i=0; i < strlen(input); i++){
		input [i];
		if (value == 1){
	               if (((input[i] >= 'A') && (input[i] <= 'Z')) || ((input[i]>='a')&&(input[i]<='z')) || 
		                		((input[i] >= '0') && (input[i] <= '9'))|| input[i]==' '){
			//if it is a space change the value into a delimeter
			//if (input[i]==' '){ 
			//input[i]= '#';
	     		//}
		        //do nothing as it is a valid character
		       }
		       else{
		             	return -1; // error not a valid character
		       }
		       }
			else{
		                if (((input[i] >= 65) && (input[i] <= 90)) || ((input[i]>=97)&&(input[i]<=122)) ||
		                		((input[i] >= 48) && (input[i] <= 57)) ){

		                	//do nothing as it is a valid character
		                }
		                else{
		                	return -1; // error not a valid character
		                }
		}
	}

	return 0;
}

char* add_delimiter(char* input){
	int i;
	for (i=0;i<strlen(input);i++){
		if (input[i]==' '){
			input[i]= '#';
		}
	}
	return input;
}

char* undo_delimiter(char* input){
	int i;
	for (i=0;i<strlen(input);i++){
		if (input[i]=='#'){
			input[i]= ' ';
		}
	}
	return input;
}


int timeval_subtract(struct timeval *result, struct timeval *t2, struct timeval *t1)
{
    long int diff = (t2->tv_usec + 1000000 * t2->tv_sec) - (t1->tv_usec + 1000000 * t1->tv_sec);
    result->tv_sec = diff / 1000000;
    result->tv_usec = diff % 1000000;

    return (diff<0);
}

int type_identifier (char string[800]){
    int x;
    int identify = 0;
    for (x=0;x<800;x++){
        if ( (string[x]>= '0' && string[x]<= '9') || string[x]==' '|| string[0] =='-'){
            identify = 0;
        }
        else if (string[x] == '\0')
            break;
        else if (string[x]=='.'){
        	identify = 2;
        	break;
        }
        else {
            identify = 1;
            break;
        }
    }
    return identify;
}

int size_get (char string[800]){
    char num[100];
    int num1;
    int x=5;
    int y=0;
    while (string[x]!=']'){
        num[y] = string [x];
        x++;
        y++;
    }
    num1 = atoi(num);
    return num1;
}

/*
void timeval_print(struct timeval *tv)
{
    char buffer[30];
    time_t curtime;

    printf("%ld.%06ld", tv->tv_sec, tv->tv_usec);
    curtime = tv->tv_sec;
    strftime(buffer, 30, "%m-%d-%Y  %T", localtime(&curtime));
    printf(" = %s.%06ld\n", buffer, tv->tv_usec);
}*/

/*
char *checkSpaces(char *array)
{
  int i;
 char arrayCheck[20];
 strcpy(arrayCheck, array);
 for(i=0; i=strlen(array); i++)
 if(arrayCheck[i] == ' ')
 {
  arrayCheck[i] == '.';
 }
return arrayCheck;
}

*/
