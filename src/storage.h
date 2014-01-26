/**
 * @file
 * @brief This file defines the interface between the storage client and
 * server.
 *
 * The functions here should be implemented in storage.c.
 * 
 * <b>You should not modify this file, or else the code used to mark your 
 * implementation will break.</b>
 */

#ifndef STORAGE_H
#define STORAGE_H

#include <stdint.h>

// Configuration constants.
#define MAX_CONFIG_LINE_LEN 1024 ///< Max characters in each config file line.
#define MAX_USERNAME_LEN 64	///< Max characters of server username.
#define MAX_ENC_PASSWORD_LEN 64	///< Max characters of server's encrypted password.
#define MAX_HOST_LEN 64		///< Max characters of server hostname.
#define MAX_PORT_LEN 8		///< Max characters of server port.
#define MAX_PATH_LEN 256	///< Max characters of data directory path.

// Storage server constants.
#define MAX_TABLES 100		///< Max tables supported by the server.
#define MAX_RECORDS_PER_TABLE 1000 ///< Max records per table.
#define MAX_TABLE_LEN 20	///< Max characters of a table name.
#define MAX_KEY_LEN 20		///< Max characters of a key name.
#define MAX_CONNECTIONS 10	///< Max simultaneous client connections.

// Extended storage server constants.
#define MAX_COLUMNS_PER_TABLE 10 ///< Max columns per table.
#define MAX_COLNAME_LEN 20	///< Max characters of a column name.
#define MAX_STRTYPE_SIZE 40	///< Max SIZE of string types.
#define MAX_VALUE_LEN 800	///< Max characters of a value.

// Error codes.
#define ERR_INVALID_PARAM 1		///< A parameter is not valid.
#define ERR_CONNECTION_FAIL 2		///< Error connecting to server.
#define ERR_NOT_AUTHENTICATED 3		///< Client not authenticated.
#define ERR_AUTHENTICATION_FAILED 4	///< Client authentication failed.
#define ERR_TABLE_NOT_FOUND 5		///< The table does not exist.
#define ERR_KEY_NOT_FOUND 6		///< The key does not exist.
#define ERR_UNKNOWN 7			///< Any other error.
#define ERR_TRANSACTION_ABORT 8		///< Transaction abort error.


/**
 * @brief Encapsulate the value associated with a key in a table.
 *
 * The metadata will be used later.
 */
struct storage_record {
	/// This is where the actual value is stored.
	char value[MAX_VALUE_LEN];

	/// A place to put any extra data.
	uintptr_t metadata[8];
};

/**
 * @brief Establish a connection to the server.
 *
 * @param hostname The IP address or hostname of the server.
 * @param port The TCP port of the server.
 * @return If successful, return a pointer to a data structure that represents 
 * a connection to the server. Otherwise return NULL.
 *
 * On error, errno will be set to one of the following, as appropriate: 
 * ERR_INVALID_PARAM, ERR_CONNECTION_FAIL, or ERR_UNKNOWN.
 */
void* storage_connect(const char *hostname, const int port);

/**
 * @brief Authenticate the client's connection to the server.
 *
 * @param username Username to access the storage server.
 * @param passwd Password in its plain text form.
 * @param conn A connection to the server.
 * @return Return 0 if successful, and -1 otherwise.
 *
 * On error, errno will be set to ERR_AUTHENTICATION_FAILED.
 */
int storage_auth(const char *username, const char *passwd, void *conn);

/**
 * @brief Retrieve the value associated with a key in a table.
 *
 * @param table A table in the database.
 * @param key A key in the table.
 * @param record A pointer to a record struture.
 * @param conn A connection to the server.
 * @return Return 0 if successful, and -1 otherwise.
 * 
 * On error, errno will be set to one of the following, as appropriate: 
 * ERR_INVALID_PARAM, ERR_CONNECTION_FAIL, ERR_TABLE_NOT_FOUND, 
 * ERR_KEY_NOT_FOUND, ERR_NOT_AUTHENTICATED, or ERR_UNKNOWN.
 *
 * The record with the specified key in the specified table is retrieved from
 * the server using the specified connection. If the key is found, the
 * record structure is populated with the details of the corresponding record.
 * Otherwise, the record structure is not modified.
 */
int storage_get(const char *table, const char *key, struct storage_record 
		*record, void *conn);

/**
 * @brief Store a key/value pair in a table.
 *
 * @param table A table in the database.
 * @param key A key in the table.
 * @param record A pointer to a record struture.
 * @param conn A connection to the server.
 * @return Return 0 if successful, and -1 otherwise.
 *
 * On error, errno will be set to one of the following, as appropriate: 
 * ERR_INVALID_PARAM, ERR_CONNECTION_FAIL, ERR_TABLE_NOT_FOUND, 
 * ERR_KEY_NOT_FOUND, ERR_NOT_AUTHENTICATED, or ERR_UNKNOWN.
 *
 * The key and record are stored in the table of the database using the
 * connection. If the key already exists in the table, the corresponding
 * record is updated with the one specified here.  If the key exists in the
 * table and the record is NULL, the key/value pair are deleted from the
 * table.
 */
int storage_set(const char *table, const char *key, struct storage_record 
		*record, void *conn);

/**
 * @brief Query the table for records, and retrieve the matching keys.
 *
 * @param table A table in the database.
 * @param predicates A comma separated list of predicates.
 * @param keys An array of strings where the keys whose records match the 
 * specified predicates will be copied.  The array must have room for at 
 * least max_keys elements.  The caller must allocate memory for this array.
 * @param max_keys The size of the keys array.
 * @param conn A connection to the server.
 * @return Return the number of matching keys (which may be more than
 * max_keys) if successful, and -1 otherwise.
 *
 * On error, errno will be set to one of the following, as appropriate: 
 * ERR_INVALID_PARAM, ERR_CONNECTION_FAIL, ERR_TABLE_NOT_FOUND, 
 * ERR_KEY_NOT_FOUND, ERR_NOT_AUTHENTICATED, or ERR_UNKNOWN.
 *
 * Each predicate consists of a column name, an operator, and a value, each
 * separated by optional whitespace. The operator may be a "=" for string
 * types, or one of "<, >, =" for int and float types. An example of query
 * predicates is "name = bob, mark > 90".
 */
int storage_query(const char *table, const char *predicates, char **keys, 
		const int max_keys, void *conn);

/**
 * @brief Close the connection to the server.
 *
 * @param conn A pointer to the connection structure returned in an
 * earlier call to storage_connect().
 * @return Return 0 if successful, and -1 otherwise.
 *
 * On error, errno will be set to one of the following, as appropriate: 
 * ERR_INVALID_PARAM, ERR_CONNECTION_FAIL, or ERR_UNKNOWN.
 */
int storage_disconnect(void *conn);

#endif
