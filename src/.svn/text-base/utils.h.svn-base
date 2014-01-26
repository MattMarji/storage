/**
 * @file
 * @brief This file declares various utility functions that are
 * can be used b
 * y the storage server and client library.
 */

#ifndef	UTILS_H
#define UTILS_H

#include <errno.h>
#include <stdio.h>
#include "storage.h"
#include <sys/time.h>
#include "TreeDB.h"
#include "TreeNode.h"
#include "TreeEntry.h"

/**
 * @brief Any lines in the config file that start with this character 
 * are treated as comments.
 */
static const char CONFIG_COMMENT_CHAR = '#';

/**
 * @brief The max length in bytes of a command from the client to the server.
 */
#define MAX_CMD_LEN (1024 * 8)

/**
 * @brief A macro to log some information.
 *
 * Use it like this:  LOG(("Hello %s", "world\n"))
 *
 * Don't forget the double parentheses, or you'll get weird errors!
 */
#define LOG(x)  {printf x; fflush(stdout);}

/**
 * @brief A macro to output debug information.
 * 
 * It is only enabled in debug builds.
 */


#ifdef NDEBUG
#define DBG(x)  {}
#else
#define DBG(x)  {printf x; fflush(stdout);}
#endif

/**
 * @brief A struct to store config parameters.
 */
typedef struct {
	/// The hostname of the server.
	char server_host[MAX_HOST_LEN];

	/// The listening port of the server.
	int server_port;

	/// The storage server's username
	char username[MAX_USERNAME_LEN];

	/// The storage server's encrypted password
	char password[MAX_ENC_PASSWORD_LEN];
	
	/// Heindrik: The directory where table names are stored
	char table_names[MAX_TABLES][MAX_TABLE_LEN];

	int num_tables;

	/// The directory where tables are stored.
//	char data_directory[MAX_PATH_LEN];
}config_params;

struct table{
  int numkeys; //For HEINDRIK-SET to populate
  int numCols; //Stores the number of columns expected for EACH table
  int row_index; //This is the size of array_config
  char *table_name;
  int array_config_index; //Stores the current size of array_config! 
  char array_config[1024][MAX_COLNAME_LEN];
  char array_empty[4096][MAX_COLNAME_LEN]; //
  char array_keys[MAX_RECORDS_PER_TABLE][MAX_KEY_LEN];
  struct table *next;
};

struct configuration {
  char *host;
  char *username;
  char *password;
  int concurrency;
  int port;
  int num_tables;
  char all_table_names[MAX_TABLES][MAX_TABLE_LEN];
  char set_values[20][20]; // store the column names and values
  int numValues; // store the size of set_values!
  char predicates [100][100]; // store the predicates for query here
  int totPredicates; //store the length of that string here
  struct table *tlist;
};

/**
 * @brief A struct to store parameters needed to pass into thread function.
 */
struct thread_params {
	struct TreeDB* treeptr;
	config_params configptr;
	int clientsocket;
	int conn_counter;
};

extern struct configuration *c;
extern struct table *tl;
extern struct table *t;
/**
 * @brief Exit the program because a fatal error occured.
 *
 * @param msg The error message to print.
 * @param code The program exit return value.
 */
static inline void die(char *msg, int code)
{
	printf("%s\n", msg);
	exit(code);
}

/**
 * @brief Keep sending the contents of the buffer until complete.
 * @return Return 0 on success, -1 otherwise.
 *
 * The parameters mimic the send() function.
 */
int sendall(const int sock, const char *buf, const size_t len);

/**
 * @brief Receive an entire line from a socket.
 * @return Return 0 on success, -1 otherwise.
 */
int recvline(const int sock, char *buf, const size_t buflen);

/**
 * @brief Read and load configuration parameters.
 *
 * @param config_file The name of the configuration file.
 * @param params The structure where config parameters are loaded.
 * @return Return 0 on success, -1 otherwise.
 */
int read_config(const char *config_file, config_params *params);

/**
 * @brief Generates a log message.
 * 
 * @param file The output stream
 * @param message Message to log.
 */
void logger(FILE *file, char *message, int LOGGER);

/**
 * @brief Default two character salt used for password encryption.
 */

//This function will print the current date and time.
char *currentDateTime();


#define DEFAULT_CRYPT_SALT "xx"

/**
 * @brief Generates an encrypted password string using salt CRYPT_SALT.
 * 
 * @param passwd Password before encryption.
 * @param salt Salt used to encrypt the password. If NULL default value
 * DEFAULT_CRYPT_SALT is used.
 * @return Returns encrypted password.
 */
char *generate_encrypted_password(const char *passwd, const char *salt);

/**
 * @brief This "valid_string_check" function will take in a string and check whether this string contains
 * the proper ASCII values from A-Z and 0-9. If it is then the function returns a zero
 * if not then the function returns another value
 */
int valid_string_check(char* input,int value);

/**
 * @brief this function will replace all the spaces within a string into a '#' for storing purposes in the value.
 */
char* add_delimiter(char *input);

/**
 * @brief this function will replace all the '#' within a string into a ' ' for retrieving purposes in the value.
 */
char* undo_delimiter(char *input);

/**
 * @brief The following two functions below are used to time the execution time of the server, 
 * and the storage. "timeval_subtract" subtracts final and the initial value to get the elapsed  time.
 * "timeval_print" will print out the value of "timeval_subtract"
 *
*/
int timeval_subtract(struct timeval *result, struct timeval *t2, struct timeval *t1);

void timeval_print(struct timeval *tv);

//char * checkSpaces(char *array);

/**
 * @brief The type_identifier function will receive a string and check whether it is all numbers or not. 
 * if it is all integers we return 0 identifying that the string sent is an integer else we send 0
 * which identifies that the string sent is a char value 
*/
int type_identifier (char string[800]);

int size_get (char string[800]);
#endif
