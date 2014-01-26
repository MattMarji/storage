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
#include "storage.h"

#define TESTTIMEOUT	10		// How long to wait for each test to run.
#define SERVEREXEC	"./server"	// Server executable file.
#define SERVEROUT	"default.serverout"	// File where the server's output is stored.
#define SERVEROUT_MODE	0666		// Permissions of the server ouptut file.
#define ONETABLE_CONF		"conf-onetable.conf"	// Server configuration file with one table.
#define TWOTABLES_CONF		"conf-twotables.conf"	// Server configuration file with two tables.
#define THREETABLES_CONF	"conf-threetables.conf"	// Server configuration file with three tables.
#define DUPLICATEPORT_CONF        "conf-duplicateport.conf" // Server configuration file with duplicate port numbers.
#define BADTABLE	"bad table"	// A bad table name.
#define BADKEY		"bad key"	// A bad key name.
#define KEY		"somekey"	// A key used in the test cases.
#define KEY1		"somekey1"	// A key used in the test cases.
#define KEY2		"somekey2"	// A key used in the test cases.
#define KEY3		"somekey3"	// A key used in the test cases.
#define KEY4		"somekey4"	// A key used in the test cases.
#define VALUE		"somevalue"	// A vaule used in the test cases.
#define VALUE1		"somevalue1"	// A vaule used in the test cases.
#define VALUE2		"somevalue2"	// A vaule used in the test cases.
#define VALUE3		"somevalue3"	// A vaule used in the test cases.
#define VALUESPC	"somevalue 4"	// A vaule used in the test cases.

// These settings should correspond to what's in the config file.
#define SERVERHOST	"localhost"	// The hostname where the server is running.
#define SERVERPORT	4848		// The port where the server is running.
#define SERVERUSERNAME	"admin"		// The server username
#define SERVERPASSWORD	"dog4sale"	// The server password
#define TABLE		"foo"		// The first table to use.
#define TABLE1		"foo"		// The first table to use.
#define TABLE2		"bar"		// The second table to use.
#define TABLE3		"joe"		// The third table to use.
#define MISSINGTABLE	"missingtable"	// A non-existing table.
#define MISSINGKEY	"missingkey"	// A non-existing key.

/* Server port used by test */
int server_port;

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
	sleep(1);	// Give the OS enough time to kill previous process
	
	pid_t childpid = fork();
	if (childpid < 0) {
		// Failed to create child.
		return -1;
	} else if (childpid == 0) {
		// The child.

		// Redirect stdout and stderr to a file.
		const char *outfile = serverout_file == NULL ? SERVEROUT : serverout_file;
		int outfd = creat(outfile, SERVEROUT_MODE);
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
	int status = storage_auth(SERVERUSERNAME, SERVERPASSWORD, conn);
	fail_unless(status == 0, "Authentication failed.");

	return conn;
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

/**
 * @brief Text fixture setup.  Start the server.
 */
void test_setup()
{
	test_conn = start_connect(ONETABLE_CONF, SERVEROUT, NULL);
	fail_unless(test_conn != NULL, "Couldn't start or connect to server.");
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
 * This test makes sure that the storage.h file has not been modified.
 */
START_TEST (test_sanity_filemod)
{
	int status = system("md5sum --status -c md5sum.check &> /dev/null");
	fail_if(status == 1, "storage.h has been modified.");
	fail_if(status != 0, "Error computing md5sum of storage.h.");
}
END_TEST


/*
 * Test how the server processes the configuration file.
 *
 * Config file (server start) tests:
 * 	start server with no config file (fail)
 * 	start server with non existent config file (fail)
 * 	start server with no port (fail)
 * 	start server with invalid port (fail)
 * 	start server with occupied port (fail)
 * 	start server with no directory specified (fail)
 * 	start server with occupied data directory (fail)
 * 	start server with non-existent directory (pass)
 * 	start server with no tables specified (pass)
 * 	start server with 1/2/3 tables specified (pass)
 * 	start server with invalid table name (fail)
 *	start server with duplicate table names (fail)
 *      start server with duplicate port numbers (fail)
 *      start server with duplicate host addresses (fail)
 */

START_TEST (test_config_onetable)
{
	int serverpid = start_server(ONETABLE_CONF, NULL, "test_config_onetable.serverout");
	fail_unless(serverpid > 0, "Server didn't run properly.");
}
END_TEST

START_TEST (test_config_twotables)
{
	int serverpid = start_server(TWOTABLES_CONF, NULL, "test_config_twotables.serverout");
	fail_unless(serverpid > 0, "Server didn't run properly.");
}
END_TEST

START_TEST (test_config_threetables)
{
	int serverpid = start_server(THREETABLES_CONF, NULL, "test_config_threetables.serverout");
	fail_unless(serverpid > 0, "Server didn't run properly.");
}
END_TEST

/*
* ADDED 
*/

START_TEST (test_config_duplicateport)
{
        int serverpid = start_server(DUPLICATEPORT_CONF, NULL, "test_config_duplicateport.serverout");
        fail_unless(serverpid < 0, "Server should not run with duplicate port numbers in the config file.");
}
END_TEST


/*
 * Connection tests:
 * 	connect to and disconnect from server (pass)
 * 	connect to server without server running (fail)
 * 	connect to server with invalid hostname (fail)
 * 	disconnect from server without server running (fail)
 * 	disconnect from server with invalid params (fail)
 */

START_TEST (test_conn_basic)
{
	int serverpid = start_server(ONETABLE_CONF, NULL, "test_conn_basic.serverout");
	fail_unless(serverpid > 0, "Server didn't run properly.");

	void *conn = storage_connect(SERVERHOST, server_port);
	fail_unless(conn != NULL, "Couldn't connect to server.");

	int status = storage_disconnect(conn);
	fail_unless(status == 0, "Error disconnecting from the server.");
}
END_TEST

START_TEST (test_conninvalid_disconnectinvalidparam)
{
	int status = storage_disconnect(NULL);
	fail_unless(status == -1, "storage_disconnect with invalid param should fail.");
	fail_unless(errno == ERR_INVALID_PARAM, "storage_disconnect with invalid param not setting errno properly.");
}
END_TEST


/*
 * Get failure tests:
 * 	get without server running (fail) [don't test this since they can assume programs won't crash.]
 * 	get with invalid table/key/record/conn parameter (fail)
 * 	get with bad table/key (fail)
 * 	get with non-existent table/key (fail)
 */


START_TEST (test_getmissing_missingtable)
{
	struct storage_record record;
	int status = storage_get(MISSINGTABLE, KEY, &record, test_conn);
	fail_unless(status == -1, "storage_get with missing table should fail.");
	fail_unless(errno == ERR_TABLE_NOT_FOUND, "storage_get with missing table not setting errno properly.");
}
END_TEST


/*
 * Set failure tests:
 * 	set with invalid table/key/conn (fail)
 * 	set with bad table/key (fail)
 * 	set with non-existent table/key (fail)
 */


START_TEST (test_setinvalid_badkey)
{
	struct storage_record record;
	strncpy(record.value, VALUE, sizeof record.value);
	int status = storage_set(TABLE, BADKEY, &record, test_conn);
	fail_unless(status == -1, "storage_set with bad key name should fail.");
	fail_unless(errno == ERR_INVALID_PARAM, "storage_set with bad key name not setting errno properly.");
}
END_TEST


/*
 * One server instance get/set pass tests:
 * 	set/get basic table/key/value names from one table.
 * 	set/get extended table/key/value names from one table.
 * 	set/get extended table/key/value names from two tables.
 */

START_TEST (test_oneserver_onetable)
{
	struct storage_record r;

	// Start and connect to the server.
	void *conn = start_connect(ONETABLE_CONF, "test_oneserver_onetable.serverout", NULL);
	fail_unless(conn != NULL, "Couldn't start or connect to server.");

	// Set a key/value.
	strncpy(r.value, VALUE, sizeof r.value);
	int status = storage_set(TABLE, KEY, &r, conn);
	fail_unless(status == 0, "Error setting a key/value pair.");

	// Get a value.
	strncpy(r.value, "", sizeof r.value);
	status = storage_get(TABLE, KEY, &r, conn);
	fail_unless(status == 0, "Error getting a value.");
	fail_unless(strcmp(r.value, VALUE) == 0, "Got wrong value.");

	// Disconnect from the server.
	status = storage_disconnect(conn);
	//fail_unless(status == 0, "Error disconnecting from the server.");
}
END_TEST



/* Added
 * Two client instance get/set pass tests:
 *      set/get basic table/key/value names from one table.
 *      set/get extended table/key/value names from one table.
 *      set/get extended table/key/value names from two tables.
 */

START_TEST (test_restartclient_onetable)
{
        struct storage_record r;

        // Start and connect to the server.
        int serverpid = 0;
        void *conn = start_connect(ONETABLE_CONF, "test_restartclient_onetable.serverout", &serverpid);
        fail_unless(conn != NULL, "Couldn't start or connect to server.");

        // Set a key/value.
        strncpy(r.value, VALUE, sizeof r.value);
        int status = storage_set(TABLE, KEY, &r, conn);
        fail_unless(status == 0, "Error setting a key/value pair.");

        // Disconnect from the server.
        status = storage_disconnect(conn);
        //fail_unless(status == 0, "Error disconnecting from the server.");

        // Reconnect to the server
	conn = storage_connect(SERVERHOST, server_port);
        fail_unless(conn != NULL, "Couldn't connect to server.");
	
	// Authenticate with the server.
	status = storage_auth(SERVERUSERNAME, SERVERPASSWORD, conn);
	fail_unless(status == 0, "Authentication failed.");

        // Get a value.
        strncpy(r.value, "", sizeof r.value);
        status = storage_get(TABLE, KEY, &r, conn);
        fail_unless(status == 0, "Error getting a value.");
        fail_unless(strcmp(r.value, VALUE) == 0, "Got wrong value.");

        // Disconnect from the server.
        status = storage_disconnect(conn);
        //fail_unless(status == 0, "Error disconnecting from the server.");
}
END_TEST


/**
 * @brief This runs the marking tests for Assignment 1.
 */
int main(int argc, char *argv[])
{
	if(argc == 2)
		server_port = atoi(argv[1]);
	else
		server_port = SERVERPORT;
	printf("Using server port: %d.\n", server_port);
	Suite *s = suite_create("a1-partial");
	TCase *tc;

	// Sanity tests
	tc = tcase_create("sanity");
	tcase_set_timeout(tc, TESTTIMEOUT);
	tcase_add_test(tc, test_sanity_filemod);
	suite_add_tcase(s, tc);

	// Config file tests
	tc = tcase_create("config");
	tcase_set_timeout(tc, TESTTIMEOUT);
	tcase_add_test(tc, test_config_onetable);
	tcase_add_test(tc, test_config_twotables);
	tcase_add_test(tc, test_config_threetables);
	tcase_add_test(tc, test_config_duplicateport);
	suite_add_tcase(s, tc);

	// Connection tests
	tc = tcase_create("conn");
	tcase_set_timeout(tc, TESTTIMEOUT);
	tcase_add_test(tc, test_conn_basic);
	suite_add_tcase(s, tc);

	// Connection tests with invalid parameters
	tc = tcase_create("conninvalid");
	tcase_set_timeout(tc, TESTTIMEOUT);
	tcase_add_test(tc, test_conninvalid_disconnectinvalidparam);
	suite_add_tcase(s, tc);

	// Get tests with missing key/table
	tc = tcase_create("getmissing");
	tcase_set_timeout(tc, TESTTIMEOUT);
	tcase_add_checked_fixture(tc, test_setup, test_teardown);
	tcase_add_test(tc, test_getmissing_missingtable);
	suite_add_tcase(s, tc);

	// Set tests with invalid parameters
	tc = tcase_create("setinvalid");
	tcase_set_timeout(tc, TESTTIMEOUT);
	tcase_add_checked_fixture(tc, test_setup, test_teardown);
	tcase_add_test(tc, test_setinvalid_badkey);
	suite_add_tcase(s, tc);


	// One server instance tests
	tc = tcase_create("oneserver");
	tcase_set_timeout(tc, TESTTIMEOUT);
	tcase_add_test(tc, test_oneserver_onetable);
	suite_add_tcase(s, tc);

	// Two client instances tests
        tc = tcase_create("restartclient");
        tcase_set_timeout(tc, TESTTIMEOUT);
        tcase_add_test(tc, test_restartclient_onetable);
        suite_add_tcase(s, tc);

	SRunner *sr = srunner_create(s);
	srunner_set_log(sr, "results.log");
	srunner_run_all(sr, CK_ENV);
	srunner_ntests_failed(sr);
	srunner_free(sr);

	return EXIT_SUCCESS;
}

