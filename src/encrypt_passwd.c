/**
 * @file
 * @brief This program implements a password encryptor.
 */

#include <stdlib.h>
#include <stdio.h>
#include "utils.h"

/**
 * @brief Print the usage to stdout.
 */
void print_usage()
{
	printf("Usage: encrypt_passwd PASSWORD [SALT]\n");
}

/**
 *
 */
int main(int argc, char *argv[])
{
	char *passwd;
	char *salt;

	if(argc == 2) {
		passwd = argv[1];
		salt = NULL;
	} else if(argc == 3) {
		passwd = argv[1];
		salt = argv[2];
	} else {
		print_usage();
		return -1;
	}

	char *encrypted_passwd = generate_encrypted_password(passwd, salt);
	if(encrypted_passwd == NULL) {
		printf("An error occured.\n");
		return -1;
	}
	printf("%s\n", encrypted_passwd);
	return 0;
}
