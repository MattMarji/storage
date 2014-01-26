/* query main */

/* set checks */
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
char* test_keys1[1];



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

	// Create an empty keys array.40
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
 * Added tests for query
 *  Check if error number is properly set when predicate contains an non-existing col name
 *  Check if error number is properly set when operator, operand is inappropriate for datatype
 *  Check if error number is properly set when query in same column more than once
 *  Check if error number is properly set when query with no predicate
 *  Check if error number is properly set when char** keys passed in points to NULL
 *  Query with error values
 *  Query using valid predicates on complex tables
 */

START_TEST (test_querycol_nonexisting) {
	int foundkeys = storage_query (THREECOLSTABLE, "col10 = 10", test_keys, MAX_RECORDS_PER_TABLE, test_conn);
	// query with non-existing column should not be successful
	fail_unless (foundkeys == -1, "storage_query with non-existing column should fail.");
	fail_unless (errno == ERR_INVALID_PARAM, "storage_query with non-existing column not setting error number properly.");

}
END_TEST


START_TEST (test_querycol_invalidop) {
	// col3 is actually a column of chars, therefore it is invalid to query for entries col3 > 10
    int foundkeys = storage_query (THREECOLSTABLE, "col3 > 10", test_keys, MAX_RECORDS_PER_TABLE, test_conn);
    fail_unless (foundkeys == -1, "storage_query trying to compare strings by integer values should fail.");
    fail_unless (errno == ERR_INVALID_PARAM, "storage_query with invalid operator/operand not setting error number properly.");

    // use a string to query col1 (which contains integer values)
    foundkeys = storage_query(THREECOLSTABLE, "col1 = cannot compare", test_keys, MAX_RECORDS_PER_TABLE, test_conn);
    fail_unless (foundkeys == -1, "storage_query trying to compare integers to a string should fail.");
    fail_unless (errno == ERR_INVALID_PARAM, "storage_query with invalid operator/operand not setting error number properly.");

    // use a float as parameter for querying in a column of int type
    foundkeys = storage_query (THREECOLSTABLE, "col1 = 1.0", test_keys, MAX_RECORDS_PER_TABLE, test_conn);
    fail_unless(foundkeys == -1, "storage_query with parameter in wrong datatype from column data type should fail.");
    fail_unless(errno == ERR_INVALID_PARAM, "storage_query with invalid operator/operand not setting error number properly.");

    // not using comma to separate parameters
    foundkeys = storage_query(THREECOLSTABLE, "col1 = 13 col2 = 24", test_keys, MAX_RECORDS_PER_TABLE, test_conn);
    fail_unless(foundkeys == -1, "storage_query with parameters not using comma for separation should fail.");
    fail_unless(errno == ERR_INVALID_PARAM, "storage_query with no separation between parameters not setting error number properly.");

}
END_TEST

START_TEST (test_querycol_multiple) {
	int foundkeys = storage_query (THREECOLSTABLE, "col1 > 10, col1> 10", test_keys, MAX_RECORDS_PER_TABLE, test_conn);
	fail_unless(foundkeys == -1, "storage_query with multiple predicates per column should fail.");
	fail_unless(errno == ERR_INVALID_PARAM, "storage_query with multiple predicates per column not setting error number properly. ");

}
END_TEST

START_TEST (test_query_nopredicate) {
	int foundkeys = storage_query(THREECOLSTABLE, "", test_keys, MAX_RECORDS_PER_TABLE, test_conn);
	fail_unless(foundkeys == -1, "storage_query with no predicate should fail.");
	fail_unless(errno == ERR_INVALID_PARAM, "storage_query with no predicate not setting error number properly.");

}
END_TEST


START_TEST(test_query_invaliddest) {
	int foundkeys = storage_query(THREECOLSTABLE, "col1 = 10", NULL, MAX_RECORDS_PER_TABLE, test_conn);
	fail_unless(foundkeys == -1, "storage_query with invalid destination array of strings should fail.");
	fail_unless(errno == ERR_INVALID_PARAM, "storage_query with invalid destination array of strings not setting error number properly.");

}
END_TEST


START_TEST(test_query_invalid) {
	int foundkeys = storage_query (THREECOLSTABLE, "col1 = 10,, col2 =-10", test_keys, MAX_RECORDS_PER_TABLE, test_conn);
	fail_unless(foundkeys == -1, "storage_query with extra comma separating predicates should fail.");
	fail_unless(errno == ERR_INVALID_PARAM, "storage_query with extra comma not setting error number properly.");

	foundkeys = storage_query (THREECOLSTABLE, ",col1 = 10, col2 = -10", test_keys, MAX_RECORDS_PER_TABLE, test_conn);
	fail_unless(foundkeys == -1, "storage_query with leading comma in the predicates should fail.");
	fail_unless(errno == ERR_INVALID_PARAM, "storage_query with leading comma not setting error number properly.");

}
END_TEST

START_TEST(test_query_valid) {
	// first do a few sets
	struct storage_record record;

	//int i;
	//for (i = 0; i < 1; i++) {
			//test_keys1[i] = (char*)malloc(MAX_KEY_LEN);
			//strncpy(test_keys1[i], "", sizeof(test_keys1[i]));
		//}

	int status = 0;
	strncpy (record.value, "col1 10, col2 -10, col3 abc", sizeof record.value);
	status = storage_set(THREECOLSTABLE, KEY1, &record, test_conn);
	strncpy (record.value, "col1 10, col2 -6, col3 abc", sizeof record.value);
	status = storage_set(THREECOLSTABLE, KEY2, &record, test_conn);
	strncpy (record.value, "col1 10, col2 -6, col3 abc", sizeof record.value);
	status = storage_set(THREECOLSTABLE, KEY3, &record, test_conn);

	// first query, expect one match (KEY1)
	int foundkeys = storage_query (THREECOLSTABLE, "col1 <11, col3 = abc, col2 = -10", test_keys, MAX_RECORDS_PER_TABLE, test_conn);
	fail_unless(foundkeys == 1, "Query didn't find the correct number of keys 1.");
	fail_unless((strcmp (test_keys[0], KEY1)==0), "The returned keys don't match the query.");
	fail_unless(strcmp(test_keys[1], "")==0, "No extra keys should be modified.");

	// second query, expect two matches (KEY2, KEY3)
	foundkeys = storage_query (THREECOLSTABLE, "col2 > -7, col1 > 0", test_keys, MAX_RECORDS_PER_TABLE, test_conn);
	fail_unless(foundkeys == 2, "Query didn' fail_unless((strcmp(test)))t find the correct number of keys.");
	fail_unless((strcmp(test_keys[0], KEY3)==0)||(strcmp(test_keys[0], KEY2)==0), "The returned keys don't match the query.");
	fail_unless((strcmp(test_keys[1], KEY3)==0)||(strcmp(test_keys[1], KEY2)==0), "The returned keys don't match the query.");
    fail_unless(strcmp(test_keys[2], "")==0, "No extra keys should be modified.");

    // third query, expect three matches (KEY1, KEY2, KEY3)
    foundkeys = storage_query (THREECOLSTABLE, "col3 = abc, col1 = 10", test_keys, MAX_RECORDS_PER_TABLE, test_conn);
    fail_unless(foundkeys == 3, "Query didn't find the correct number of keys 2.");
    fail_unless((strcmp(test_keys[0], KEY3)==0)||(strcmp(test_keys[0], KEY2)==0)||(strcmp(test_keys[0], KEY1)==0), "The returned keys don't match the query.");
    fail_unless((strcmp(test_keys[1], KEY3)==0)||(strcmp(test_keys[1], KEY2)==0)||(strcmp(test_keys[1], KEY1)==0), "The returned keys don't match the query.");
    fail_unless((strcmp(test_keys[2], KEY3)==0)||(strcmp(test_keys[2], KEY2)==0)||(strcmp(test_keys[2], KEY1)==0), "The returned keys don't match the query.");
    fail_unless(strcmp(test_keys[3], "")==0, "No extra keys should be modified.");

    strcpy(test_keys[1], "");
    // fourth query, expect two matches (KEY2, KEY3), but use an array with 1 element
    foundkeys = storage_query (THREECOLSTABLE, "col2 > -7, col1 > 0", test_keys, 1, test_conn);
    fail_unless(foundkeys == 2, "Query didn't find the correct number of keys 3.");
    fail_unless((strcmp(test_keys[0], KEY3)==0)||(strcmp(test_keys[0], KEY2)==0), "The returned keys don't match the query.");
    fail_unless(strcmp(test_keys[1], "")==0, "No extra keys should be modified based on the restrained size. The two keys are: %s, %s", test_keys[0], test_keys[1]);

    // fifth query, expect two matches (KEY2, KEY3), but use an array with 0 element
    // as per M3 spec, "if max_keys is set to zero, then keys could be NULL"
    foundkeys = storage_query (THREECOLSTABLE, "col2 > -7,  col1 >0", NULL, 0, test_conn);
    fail_unless(foundkeys == 2, "Query didn't find the correct number of keys 4.");
    fail_if(errno == ERR_INVALID_PARAM, "If max_keys is set to zero, then keys could be NULL. Should not set the error number for this case.");

}
END_TEST

int main (int argc, char *argv[])
{
	if(argc == 2)
		server_port = atoi(argv[1]);
	else
		server_port = SERVERPORT;
	printf("Using server port: %d.\n", server_port);
	Suite *s = suite_create("a3-query");
	TCase *tc;

	// Query test: query for non-existing col
	tc = tcase_create("querynonexistingcolumn");
	tcase_set_timeout(tc, TESTTIMEOUT);
	tcase_add_checked_fixture(tc, test_setup_complex_populate, test_teardown);
	tcase_add_test(tc, test_querycol_nonexisting);
	suite_add_tcase(s, tc);

	

    // Query test: query for multiple predicates per column
	tc = tcase_create("querymultiplepercol");
	tcase_set_timeout(tc, TESTTIMEOUT);
	tcase_add_checked_fixture(tc, test_setup_complex_populate, test_teardown);
	tcase_add_test(tc, test_querycol_multiple);
	suite_add_tcase(s, tc);

	// Query test: query with no predicate
	tc = tcase_create("querynopredicate");
	tcase_set_timeout(tc, TESTTIMEOUT);
	tcase_add_checked_fixture(tc, test_setup_complex_populate, test_teardown);
	tcase_add_test(tc, test_query_nopredicate);
	suite_add_tcase(s, tc);

	// Query test: query with invalid destination string array
	tc = tcase_create("queryinvaliddest");
	tcase_set_timeout(tc, TESTTIMEOUT);
	tcase_add_checked_fixture(tc, test_setup_complex_populate, test_teardown);
	tcase_add_test(tc, test_query_invaliddest);
	suite_add_tcase(s, tc);

	// Query test: query with invalid comma usage in predicates
	tc = tcase_create("queryinvalidcommause");
	tcase_set_timeout (tc, TESTTIMEOUT);
	tcase_add_checked_fixture(tc, test_setup_complex_populate, test_teardown);
	tcase_add_test(tc, test_query_invalid);
	suite_add_tcase(s,tc);
	
	// Query test: query for invalid type of operator with data type of value in col
	tc = tcase_create("queryinvalidop");
	tcase_set_timeout(tc, TESTTIMEOUT);
	tcase_add_checked_fixture(tc, test_setup_complex_populate, test_teardown);
	tcase_add_test(tc, test_querycol_invalidop);
	suite_add_tcase(s, tc);

	// Query test: query with valid predicates
	tc = tcase_create("queryvalid");
	tcase_set_timeout(tc, TESTTIMEOUT);
	tcase_add_checked_fixture(tc, test_setup_complex_populate, test_teardown);
	tcase_add_test(tc, test_query_valid);
	suite_add_tcase(s, tc);


	SRunner *sr = srunner_create(s);
	srunner_set_log(sr, "results.log");
	srunner_run_all(sr, CK_ENV);
	srunner_ntests_failed(sr);
	srunner_free(sr);

	return EXIT_SUCCESS;

}


