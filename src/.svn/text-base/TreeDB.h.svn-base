/** 
 * @brief 
 * File:   struct TreeDB.h
 * Author: MatthewMarji
 *
 * Created on February 16, 2013, 12:35 AM
 */

#ifndef TREEDB_H
#define	TREEDB_H



#include "TreeNode.h"
#include "TreeDB.h"
#include <stdbool.h>
    
struct TreeDB{
    struct TableNode* root;
};

struct TreeDB* createTreeDB();
void deleteTreeDB(struct TreeDB* tree);
bool insert (struct TreeDB* tree, struct TreeEntry* newEntry);
struct TreeEntry* find(struct TreeDB* tree, char* name);
bool removeDBtable(struct TreeDB* tree, char* name);
void clear(struct TreeDB* tree);
void printTree(struct TreeDB* tree);

#endif	/* TREEDB_H */

