/* 
 * File:   struct TreeNode.h
 * Author: MatthewMarji
 *
 * Created on February 16, 2013, 3:50 AM
 */

#ifndef TREENODE_H
#define	TREENODE_H

#ifdef	__cplusplus
extern "C" {
#endif
#include "TreeEntry.h"
#include <stdbool.h>

struct TreeNode{
    struct TreeEntry* entryPtr;
    struct TreeNode* left;
    struct TreeNode* right;
};

struct TreeNode* createTreeNode(struct TreeEntry* entryPtr);
void deleteTreeNode(struct TreeNode* node);
void setLeft(struct TreeNode* node, struct TreeNode* left);
void setRight(struct TreeNode* node, struct TreeNode* right);
struct TreeNode* getLeft(struct TreeNode* node);
struct TreeNode* getRight(struct TreeNode* node);
struct TreeEntry* getTable(struct TreeNode* node);
struct TreeEntry* findTable(struct TreeNode* node, char* name);
bool insertTable(struct TreeNode* node, struct TreeEntry* newEntry);
//int countTables(struct TreeNode* node);
struct TreeNode* removeTable(struct TreeNode* node, char* name, struct TreeNode** check);
struct TreeNode* getLargest(struct TreeNode* node);
void printNodes(struct TreeNode* node);
#ifdef	__cplusplus
}
#endif

#endif	/* TREENODE_H */

