/* get checks */
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <check.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <math.h>
#include "storage.h"

#define TESTTIMEOUT	10		// How long to wait for each test to run.
#define SERVEREXEC	"./server"	// Server executable file.
#define SERVEROUT	"default.serverout"	// File where the server's output is stored.
#define SERVEROUT_MODE	0666		// Permissions of the server ouptut file.
#define ONETABLE_CONF			"conf-onetable.conf"	// Server configuration file with one table.
#define SIMPLETABLES_CONF		"conf-simpletables.conf"	// Server configuration file with simple tables.
#define COMPLEXTABLES_CONF		"conf-complextables.conf"	// Server configuration file with complex tables.
#define DUPLICATE_COLUMN_TYPES_CONF     "conf-duplicatetablecoltype.conf"        // Server configuration file with duplicate column types.
#define BADTABLE	"bad table"	// A bad table name.
#define BADKEY		"bad key"	// A bad key name.
#define KEY		"somekey"	// A key used in the test cases.
#define KEY1		"somekey1"	// A key used in the test cases.
#define KEY2		"somekey2"	// A key used in the test cases.
#define KEY3		"somekey3"	// A key used in the test cases.
#define KEY4		"somekey4"	// A key used in the test cases.
#define VALUESPC	"someval 4"	// A value used in the test cases.
#define INTCOL		"col"		// An integer column
#define INTVALUE	"22"		// An integer value
#define INTCOLVAL	"col 22"	// An integer column name and value

// These settings should correspond to what's in the config file.
#define SERVERHOST	"localhost"	// The hostname where the server is running.
#define SERVERPORT	5520		// The port where the server is running.
#define SERVERUSERNAME	"admin"		// The server username
#define SERVERPASSWORD	"dog4sale"	// The server password
//#define SERVERPUBLICKEY	"keys/public.pem"	// The server public key
// #define DATADIR		"./mydata/"	// The data directory.
#define TABLE		"inttbl"	// The table to use.
#define INTTABLE	"inttbl"	// The first simple table.
//#define FLOATTABLE	"floattbl"	// The second simple table.
#define STRTABLE	"strtbl"	// The third simple table.
#define THREECOLSTABLE	"threecols"	// The first complex table.
#define FOURCOLSTABLE	"fourcols"	// The second complex table.
#define SIXCOLSTABLE	"sixcols"	// The third complex table.
#define MISSINGTABLE	"missingtable"	// A non-existing table.
#define MISSINGKEY	"missingkey"	// A non-existing key.
/*
 * main.c
 *
 *  Created on: 2013-03-22
 *      Author: changxin
 */


#define FLOATTOLERANCE  0.0001		// How much a float value can be off by (due to type conversions).

/* Server port used by test */
int server_port;

/**
 * @brief Compare whether two floating point numbers are almost the same.
 * @return 0 if the numbers are almost the same, -1 otherwise.
 */
int floatcmp(float a, float b)
{
	if (fabs(a - b) < FLOATTOLERANCE)
		return 0;
	else
		return -1;
}

/**
 * @brief Remove trailing spaces from a string.
 * @param str The string to trim.
 * @return The modified string.
 */
char* trimtrailingspc(char *str)
{
	// Make sure string isn't null.
	if (str == NULL)
		return NULL;

	// Trim spaces from the end.
	int i = 0;
	for (i = strlen(str) - 1; i >= 0; i--) {
		if (str[i] == ' ')
			str[i] = '\0';
		else
			break; // stop if it's not a space.
	}
	return str;
}

/**
 * @brief Start the storage server.
 *
 * @param config_file The configuration file the server should use.
 * @param status Status info about the server (from waitpid).
 * @param serverout_file File where server output is stored.
 * @return Return server process id on success, or -1 otherwise.
 */
int start_server(char *config_file, int *status, const char *serverout_file)
{
	sleep(1);       // Give the OS enough time to kill previous process

	pid_t childpid = fork();
	if (childpid < 0) {
		// Failed to create child.
		return -1;
	} else if (childpid == 0) {
		// The child.

		// Redirect stdout and stderr to a file.
		const char *outfile = serverout_file == NULL ? SERVEROUT : serverout_file;
		//int outfd = creat(outfile, SERVEROUT_MODE);
		int outfd = open(outfile, O_CREAT|O_WRONLY, SERVEROUT_MODE);
		close(STDOUT_FILENO);
		close(STDERR_FILENO);
		if (dup2(outfd, STDOUT_FILENO) < 0 || dup2(outfd, STDERR_FILENO) < 0) {
			perror("dup2 error");
			return -1;
		}

		// Start the server
		execl(SERVEREXEC, SERVEREXEC, config_file, NULL);

		// Should never get here.
		perror("Couldn't start server");
		exit(EXIT_FAILURE);
	} else {
		// The parent.

		// If the child terminates quickly, then there was probably a
		// problem running the server (e.g., config file not found).
		sleep(1);
		int pid = waitpid(childpid, status, WNOHANG);
		//printf("Parent returned %d with child status %d\n", pid, WEXITSTATUS(*status));
		if (pid == childpid)
			return -1; // Probably a problem starting the server.
		else
			return childpid; // Probably ok.
	}
}

/**
 * @brief Start the server, and connect to it.
 * @return A connection to the server if successful.
 */
void* start_connect(char *config_file, char *serverout_file, int *serverpid)
{
	// Start the server.
	int pid = start_server(config_file, NULL, serverout_file);
	fail_unless(pid > 0, "Server didn't run properly.");
	if (serverpid != NULL)
		*serverpid = pid;

	// Connect to the server.
	void *conn = storage_connect(SERVERHOST, server_port);
	fail_unless(conn != NULL, "Couldn't connect to server.");

	// Authenticate with the server.
	int status = storage_auth(SERVERUSERNAME,
				  SERVERPASSWORD,
				  //SERVERPUBLICKEY,
				      conn);
	fail_unless(status == 0, "Authentication failed.");

	return conn;
}

/**
 * @brief Delete the data directory, start the server, and connect to it.
 * @return A connection to the server if successful.
 */
void* clean_start_connect(char *config_file, char *serverout_file, int *serverpid)
{
	// Delete the data directory.
//	system("rm -rf " DATADIR);

	return start_connect(config_file, serverout_file, serverpid);
}

/**
 * @brief Create an empty data directory, start the server, and connect to it.
 * @return A connection to the server if successful.
 */
void* init_start_connect(char *config_file, char *serverout_file, int *serverpid)
{
	// Delete the data directory.
//	system("rm -rf " DATADIR);

	// Create the data directory.
//	mkdir(DATADIR, 0777);

	return start_connect(config_file, serverout_file, serverpid);
}

/**
 * @brief Kill the server with given pid.
 * @return 0 on success, -1 on error.
 */
int kill_server(int pid)
{
	int status = kill(pid, SIGKILL);
	fail_unless(status == 0, "Couldn't kill server.");
	return status;
}


/// Connection used by test fixture.
void *test_conn = NULL;


// Keys array used by test fixture.
char* test_keys[MAX_RECORDS_PER_TABLE];

/**
 * @brief Text fixture setup.  Start the server.
 */
void test_setup_simple()
{
	test_conn = init_start_connect(SIMPLETABLES_CONF, "simpleempty.serverout", NULL);
	fail_unless(test_conn != NULL, "Couldn't start or connect to server.");
}

/**
 * @brief Text fixture setup.  Start the server and populate the tables.
 */
void test_setup_simple_populate()
{
	test_conn = init_start_connect(SIMPLETABLES_CONF, "simpledata.serverout", NULL);
	fail_unless(test_conn != NULL, "Couldn't start or connect to server.");

	struct storage_record record;
	int status = 0;
	int i = 0;

	// Create an empty keys array.
	// No need to free this memory since Check will clean it up anyway.
	for (i = 0; i < MAX_RECORDS_PER_TABLE; i++) {
		test_keys[i] = (char*)malloc(MAX_KEY_LEN);
		strncpy(test_keys[i], "", sizeof(test_keys[i]));
	}

	// Do a bunch of sets (don't bother checking for error).

	strncpy(record.value, "col -2", sizeof record.value);
	status = storage_set(INTTABLE, KEY1, &record, test_conn);
	strncpy(record.value, "col 2", sizeof record.value);
	status = storage_set(INTTABLE, KEY2, &record, test_conn);
	strncpy(record.value, "col 4", sizeof record.value);
	status = storage_set(INTTABLE, KEY3, &record, test_conn);

//	strncpy(record.value, "col -2.2", sizeof record.value);
//	status = storage_set(FLOATTABLE, KEY1, &record, test_conn);
//	strncpy(record.value, "col 2.2", sizeof record.value);
//	status = storage_set(FLOATTABLE, KEY2, &record, test_conn);
//	strncpy(record.value, "col 4.0", sizeof record.value);
//	status = storage_set(FLOATTABLE, KEY3, &record, test_conn);

	strncpy(record.value, "col abc", sizeof record.value);
	status = storage_set(STRTABLE, KEY1, &record, test_conn);
	strncpy(record.value, "col def", sizeof record.value);
	status = storage_set(STRTABLE, KEY2, &record, test_conn);
	strncpy(record.value, "col abc def", sizeof record.value);
	status = storage_set(STRTABLE, KEY3, &record, test_conn);
}

/**
 * @brief Text fixture setup.  Start the server with complex tables.
 */
void test_setup_complex()
{
	test_conn = init_start_connect(COMPLEXTABLES_CONF, "complexempty.serverout", NULL);
	fail_unless(test_conn != NULL, "Couldn't start or connect to server.");
}

/**
 * @brief Text fixture setup.  Start the server with complex tables and populate the tables.
 */
void test_setup_complex_populate()
{
	test_conn = init_start_connect(COMPLEXTABLES_CONF, "complexdata.serverout", NULL);
	fail_unless(test_conn != NULL, "Couldn't start or connect to server.");

	struct storage_record record;
	int status = 0;
	int i = 0;

	// Create an empty keys array.
	// No need to free this memory since Check will clean it up anyway.
	for (i = 0; i < MAX_RECORDS_PER_TABLE; i++) {
		test_keys[i] = (char*)malloc(MAX_KEY_LEN);
		strncpy(test_keys[i], "", sizeof(test_keys[i]));
	}

	// Do a bunch of sets (don't bother checking for error).

	strncpy(record.value, "col1 -2,col2 -2,col3 abc", sizeof record.value);
	status = storage_set(THREECOLSTABLE, KEY1, &record, test_conn);
	strncpy(record.value, "col1 2,col2 2,col3 def", sizeof record.value);
	status = storage_set(THREECOLSTABLE, KEY2, &record, test_conn);
	strncpy(record.value, "col1 4,col2 4,col3 abc def", sizeof record.value);
	status = storage_set(THREECOLSTABLE, KEY3, &record, test_conn);

	strncpy(record.value, "col1 abc,col2 -2,col3 -2,col4 ABC", sizeof record.value);
	status = storage_set(FOURCOLSTABLE, KEY1, &record, test_conn);
	strncpy(record.value, "col1 def,col2 2,col3 2,col4 DEF", sizeof record.value);
	status = storage_set(FOURCOLSTABLE, KEY2, &record, test_conn);
	strncpy(record.value, "col1 abc def,col2 4,col3 4,col4 ABC DEF", sizeof record.value);
	status = storage_set(FOURCOLSTABLE, KEY3, &record, test_conn);

	strncpy(record.value, "col1 abc,col2 ABC,col3 -2,col4 2,col5 -2,col6 2", sizeof record.value);
	status = storage_set(SIXCOLSTABLE, KEY1, &record, test_conn);
	strncpy(record.value, "col1 abc,col2 ABC,col3 2,col4 -2,col5 2,col6 -2", sizeof record.value);
	status = storage_set(SIXCOLSTABLE, KEY2, &record, test_conn);
	strncpy(record.value, "col1 def,col2 DEF,col3 4,col4 -4,col5 4,col6 -4", sizeof record.value);
	status = storage_set(SIXCOLSTABLE, KEY3, &record, test_conn);

}

/**
 * @brief Text fixture teardown.  Disconnect from the server.
 */
void test_teardown()
{
	// Disconnect from the server.
	storage_disconnect(test_conn);
	//fail_unless(status == 0, "Error disconnecting from the server.");
}




/**
 * Four Added tests for storage_get():
 *  get with non-existing key
 *  pass a NULL pointer to the parameter "record"
 *  get test with simple tables
 *  get test with complex tables
 */


START_TEST (test_getnonexisting_nonexistingkey)
{
	// retrieve with a non-existing key
	struct storage_record record;
	int status = storage_get (INTTABLE, MISSINGKEY, &record, test_conn);
	// check if status is set to -1
	fail_unless(status == -1, "storage_get with missing key should fail.");
	// check if error number is correctly set
	fail_unless(errno == ERR_KEY_NOT_FOUND, "storage_get with missing key not setting errno properly.");
}
END_TEST


START_TEST (test_getrecord_nullptr)
{
	// trying to retrieve with a NULL storage destination
    int status = storage_get(INTTABLE, KEY1, NULL, test_conn);
    // check if status is set to -1
    fail_unless(status == -1, "storage_get with invalid storage_record pointer should fail.");
    // check if error number is correctly set
    fail_unless(errno == ERR_INVALID_PARAM, "storage_get with invalid storage_record pointer not setting errno properly.");
}
END_TEST


START_TEST (test_getsingle_valid)
{
	struct storage_record record;
	int fields = 0;
	int intval = 0;

	// set the value of col to -2
	strncpy(record.value, "col -2", sizeof record.value);
    int	status = storage_set(INTTABLE, KEY1, &record, test_conn);

	// reset the value field of the record
	strncpy(record.value, "", sizeof record.value);
	status = storage_get(INTTABLE, KEY1, &record, test_conn);
	// if the function did not successfully execute, indicate fail
	fail_unless(status == 0, "Error getting a value.");

	// pull out the value retrieved and store into intval
	// and indicate the number of variables pulled out in fields
	fields = sscanf(record.value, "col %d", &intval);
	// check if retrieved 1 value and check if that value is -2
	fail_unless(fields == 1 && intval == -2, "Got wrong value.");

}
END_TEST


START_TEST (test_getsetthreecols_valid)
{
	struct storage_record record;
	int fields = 0;
	int intval1 = 0;
	int intval2 = 0;
	char strval[MAX_STRTYPE_SIZE];
	strncpy(strval, "", sizeof strval);

	// set the value of threecols table's col1 to 4, col2 to 11, col3 to "happy birthday"
	strncpy(record.value, "col1 4, col2 11, col3 happy birthday", sizeof record.value);
	int status = storage_set(THREECOLSTABLE, KEY1, &record, test_conn);
	// check if can set successfully
	fail_unless(status == 0, "Error setting a value.");

	// reset the value field of the record
	strncpy(record.value, "", sizeof record.value);
	status = storage_get(THREECOLSTABLE, KEY1, &record, test_conn);
	// check if can perform get successfully
	fail_unless(status == 0, "Error getting a value. ");
	//pull out the value retrieved and store into
	fields = sscanf(record.value, "col1 %d, col2 %d, col3 %[a-zA-Z0-9 ]", &intval1, &intval2, strval);
	fail_unless(fields == 3, "Got wrong number of fields.");
	fail_unless(intval1 = 4, "Got wrong value (first column). ");
	fail_unless(intval2 = 11, "Got wrong value (second column).");
	fail_unless(strcmp(trimtrailingspc(strval),"happy birthday")==0, "Got wrong value (third column).");

}
END_TEST


START_TEST (test_getsetfourcols_valid)
{
	struct storage_record record;
	int fields = 0;
	char strval1[MAX_STRTYPE_SIZE];
	strncpy(strval1, "", sizeof strval1);
	int intval1 = 0;
	int intval2 = 0;
	char strval2[MAX_STRTYPE_SIZE];
	strncpy(strval2, "", sizeof strval2);

	// set the value of fourcols table's col1 to "ever", col2 to 4, col3 to 11, col4 to "k pop"
	strncpy(record.value, "col1 ever, col2 4, col3 11, col4 k pop", sizeof record.value);
	int status = storage_set(FOURCOLSTABLE, KEY1, &record, test_conn);
	fail_unless(status == 0, "Error setting a value.");

	// reset the value field of the record
	strncpy(record.value, "", sizeof record.value);
	status = storage_get(FOURCOLSTABLE, KEY1, &record, test_conn);
	fail_unless(status == 0, "Error getting a value.");
	fields = sscanf(record.value, "col1 %[a-zA-Z0-9 ], col2 %d, col3 %d, col4 %[a-zA-Z0-9 ]", strval1, &intval1, &intval2, strval2);
	fail_unless(fields == 4, "Got wrong number of fields.");
	fail_unless(strcmp(trimtrailingspc(strval1),"ever")==0, "Got wrong value (first column).");
    fail_unless(intval1 = 4, "Got wrong value (second column).");
    fail_unless(intval2 = 11, "Got wrong value (third column).");
    fail_unless(strcmp(trimtrailingspc(strval2),"k pop")==0, "Got wrong value (fourth column).");
}

END_TEST

/**
 * @brief This runs the test for storage_get()
 */
int main (int argc, char *argv[])
{
	if(argc == 2)
		server_port = atoi(argv[1]);
	else
		server_port = SERVERPORT;
	printf("Using server port: %d.\n", server_port);
	Suite *s = suite_create("a3-get");
	TCase *tc;

	// Get test with non-existing key in a table
	tc = tcase_create("getnonexistingkey");
	tcase_set_timeout(tc,TESTTIMEOUT);
    tcase_add_checked_fixture(tc, test_setup_simple_populate, test_teardown);
    tcase_add_test(tc, test_getnonexisting_nonexistingkey);
    suite_add_tcase(s,tc);

    // Get test with NULL pointer to parameter "record"
    tc = tcase_create("getnullptrrecord");
    tcase_set_timeout(tc,TESTTIMEOUT);
    tcase_add_checked_fixture(tc, test_setup_simple_populate,test_teardown);
    tcase_add_test(tc,test_getrecord_nullptr);
    suite_add_tcase(s,tc);

    // Get test with setting and retrieving from a single-column table
    tc = tcase_create("getsinglerecord");
    tcase_set_timeout(tc,TESTTIMEOUT);
    tcase_add_checked_fixture(tc, test_setup_simple, test_teardown);
    tcase_add_test(tc,test_getsingle_valid);
    suite_add_tcase(s,tc);

    // Get/Set test with setting and retrieving from a three-column table
    tc = tcase_create("getthreerecord");
    tcase_set_timeout(tc,TESTTIMEOUT);
    tcase_add_checked_fixture(tc, test_setup_complex, test_teardown);
    tcase_add_test(tc,test_getsetthreecols_valid);
    suite_add_tcase(s,tc);

    // Get/Set test with setting and retrieving from a four-column table
    tc = tcase_create("getfourrecord");
    tcase_set_timeout(tc,TESTTIMEOUT);
    tcase_add_checked_fixture(tc,test_setup_complex, test_teardown);
    tcase_add_test(tc, test_getsetfourcols_valid);
    suite_add_tcase(s,tc);


	SRunner *sr = srunner_create(s);
	srunner_set_log(sr, "results.log");
	srunner_run_all(sr, CK_ENV);
	srunner_ntests_failed(sr);
	srunner_free(sr);
	return EXIT_SUCCESS;

}






