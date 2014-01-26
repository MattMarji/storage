/* 
 * File:  TreeEntry.h
 * Author: MatthewMarji
 *
 * Created on February 16, 2013, 3:39 PM
 */

#ifndef TREEENTRY_H
#define	TREEENTRY_H

#ifdef	__cplusplus
extern "C" {
#endif
struct TreeEntry{
        char* name;
        char* value;
		int record_counter;
        struct TreeDB* tree;
    };

struct TreeEntry* createTable(char* name);
void deleteTable(struct TreeEntry* table);

void setTableName(struct TreeEntry *table, char* name);
char* getTableName(struct TreeEntry* table);
void setEntryValue(struct TreeEntry* table, char* name);
char* getEntryValue(struct TreeEntry* table);

//Get and Set Helper functions to retrieve the record_counter value to match with r.metadata!
void setCounterValue(struct TreeEntry *table, int counter_value);
int getCounterValue(struct TreeEntry* table);
void printTable(struct TreeEntry* table);

#ifdef	__cplusplus
}
#endif

#endif	/* TREEENTRY_H */

