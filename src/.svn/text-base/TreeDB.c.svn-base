
/**
 * @brief The struct TreeDB will simply be a pointer to a binary search tree that holds nodes of
 * type "TreeNode". Each TreeNode will store a key, value, and a pointer of type "struct TreeDB".
 * This will allows us to create a BST inside of the tables, that store the entries!
 *
 * Qualifies for Doxygen
 * LEVELS:
 * Level 1:
 * KEY = table_name
 * VALUE = NULL (not necessary)
 * TREEDB pointer -> head pointer to a BST that will hold the entries
 * 
 * Level 2:
 * KEY = key of entry
 * VALUE = value of entry
 * TREEDB pointer -> head pointer that will be NULL (no need to have another BST)
 */

#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<stdbool.h>

#include "TreeDB.h"

/**
 *@brief Construct a TREEDB head pointer
 *
 */

struct TreeDB* createTreeDB()
{
    struct TreeDB* tree = (struct TreeDB*)malloc(sizeof(struct TreeDB));
    tree->root = NULL;
    return tree;
}

/* 
 * @brief Destruct the TREEDB
 */
void deleteTreeDB(struct TreeDB* tree){
    
    if(tree != NULL && tree->root != NULL)
    	deleteTreeNode(tree->root);
        free(tree->root); 
}

/** 
 * @brief Insert NODES into the struct TreeDB. We must check to see if the 'key' already exists or not.
 * @return 'false' if the entry's key already exists and 'true' if not.
 */

bool insert (struct TreeDB* tree, struct TreeEntry* newEntry){
   
    //Before we start, we should check if the database is empty, if so, there is no need to continue.
    	if(tree == NULL)
	return false;
    
    //BASE CASE -> When we find where to insert the node, we now insert the node.
    if(tree->root == NULL)
    {
        //Create the TreeNode since it has not been created before.
        tree->root = createTreeNode(newEntry);
        return true;
    }
        
    //We must retrieve the name of the key!     
    char* nodeName = getTableName((struct TreeEntry*)getEntry((struct TreeNode*)tree->root));
    
    //Once we have the key (char* name) - We now compare to see if it exists by traversing through the BST
    
    //IF THE KEY ALREADY EXISTS - RETURN FALSE
    if(strcmp(getTableName(newEntry), nodeName) == 0)
    {
        return false;
    }
    
    //IF THE KEY value is smaller go to the left of the node
    else if(strcmp(getTableName(newEntry), nodeName) < 0)
    {
        if(getLeft(tree->root) == NULL)
        {
            setLeft(tree->root, createTreeNode(newEntry));
            return true;
        }
        
        //recurse to the left
        return insertTable(getLeft(tree->root), newEntry);
    }
    
    //IF THE KEY value is larger, go to the right of the node
    else if(strcmp(getTableName(newEntry), nodeName) > 0)
    {
        if(getRight(tree->root) == NULL)
        {
            setRight(tree->root, createTreeNode(newEntry));
            return true;
        }

        //recurse to the right
        return insertTable(getRight(tree->root), newEntry);
    }
    
    return false;
}

/**
 * @brief Find if the entry exists with the given key 'name'
 *
 */

struct TreeEntry* find(struct TreeDB* tree, char* name)
{
        if(tree == NULL)
	return NULL;

    if(tree->root == NULL)
    {
    return NULL;
    }
        
    //Retrieve the entry to compare.    
    char* nodeName = getTableName(getEntry(tree->root));

    //SAME ALGORITHM as INSERT
    if(strcmp(nodeName, name) == 0)
    {
        return getEntry(tree->root);
    }
    else if(strcmp(name, nodeName) < 0)
    {
        if(getLeft(tree->root) == NULL)
        {
            return NULL;
        }

        return findTable(getLeft(tree->root), name);
    }
    else if(strcmp(name, nodeName) > 0)
    {
        if(getRight(tree->root) == NULL)
        {
            return NULL;
        }

        return findTable(getRight(tree->root), name);
    }

    return NULL;
}

/**
 * @brief Remove a specific entry!
 *
 */
bool removeDBEntry(struct TreeDB* tree, char* name)
{
    if(tree == NULL || tree->root == NULL)
        return false;

    struct TreeNode* temp = NULL;
    
   tree->root = removeTable(tree->root, name, &temp);

    if(temp == NULL)
        return false;

    //set left and right to null 
    setLeft(temp, NULL);
    setRight(temp, NULL);
    deleteTreeNode(temp);
    free(temp);
    return true;
}

/** 
 * @brief deletes all the entries in the database.
 *
 */
void clear(struct TreeDB* tree)
{
    if(tree != NULL && tree->root != NULL)
    {
        deleteTreeNode(tree->root);
        free(tree->root);
    }

    tree->root = NULL;
}

// Prints the entire tree
void printTree(struct TreeDB* tree)
{
    if(tree == NULL || tree->root == NULL)
    {
        return;
    }

    printNodes(tree->root);
    printf("\n");

    return;
}
