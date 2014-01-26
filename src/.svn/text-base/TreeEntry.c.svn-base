
/**
 * @file
 * @brief GET and SET OPERATION are implemented here.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "TreeEntry.h"
#include "TreeDB.h"
#include "storage.h"

/**
 * 
 * @brief EVERY TABLE HAS AN ENTRY, so we create a TABLE with a key, value, and table-name.
 */
struct TreeEntry* createTable(char* name){
    struct TreeEntry *table = malloc(sizeof(struct TreeEntry));
    table->name =(char*)malloc(MAX_KEY_LEN*sizeof(char)); 
    table->value =(char*)malloc(MAX_VALUE_LEN*sizeof(char));
    strcpy(table->name, name);
    table->tree = createTreeDB();
    return table;    
}

/**
 * @brief SAFELY REMOVE THE TABLE 
 */

void deleteTable(struct TreeEntry* table)
{
	if(table == NULL)
		return;

	if(table->name != NULL)
	{
		free(table->name);
		table->name = NULL;
	}

	if(table->value != NULL)
	{
		free(table->value);
		table->value = NULL;
	}

	if(table->tree != NULL)
	{
		deleteTreeDB(table->tree);
		free(table->tree);
		table->tree = NULL;
	}
}

/**
 *@brief TABLES ARE DEFINED BY NAMES. HERE WE WILL ASSIGN THE NAME TO THE TABLE.
 */

void setTableName(struct TreeEntry *table, char* name){
    if (table == NULL){
        return;
    }
    
    strcpy(table->name, name);
}

/**
 * @brief
 * @return RETURN THE TABLE NAME
 */


char* getTableName(struct TreeEntry* table){
    if(table == NULL)
        return NULL;

    return table->name;
}


/**
 * @brief
 * @return returns string to entry value
 */
char* getEntryValue(struct TreeEntry* table)
{
	return table->value;
}

/**
 * @brief sets entry value to _value
 * 
 */

void setEntryValue(struct TreeEntry* table, char* _value)
{
	if(table == NULL)
		return;

	strcpy(table->value, _value);
}

/**
 * @brief
 * @return returns integer value of counter
 */
int getCounterValue(struct TreeEntry* table)
{
	return table->record_counter;
}

/**
 * @brief sets integer value of record_counter
 * 
 */

void setCounterValue(struct TreeEntry* table, int counter_value)
{
	if(table == NULL)
		return;

	table->record_counter = counter_value;
}


/**
 * @brief prints the name of the table
 * 
 */

void printTable(struct TreeEntry* table)
{
	if(table != NULL)
	{
		char* name = getTableName(table);
		printf("%s ", name);
	}
}

