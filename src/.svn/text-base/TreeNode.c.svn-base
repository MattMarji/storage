/** 
 * @file 
 * @brief 
 * In this file we will create the necessary functions to insert, delete, get and set the entries for the tables.
 * Every TableNode has the following:
 * left pointer
 * right pointer
 * struct entry -> has a key and value.
 * place the Node in the BST based on the entry->key value.
 */

#include<stdlib.h>
#include<stdio.h>
#include<stdbool.h>
#include "TreeNode.h"
#include "TreeDB.h"


/**
 * @brief
 * Create an entry node for storage in the appropriate table
 */
struct TreeNode* createTreeNode (struct TreeEntry* entryPtr){
    struct TreeNode *tempNode = (struct TreeNode*)malloc(sizeof(struct TreeNode));
    tempNode->left=NULL;
    tempNode->right=NULL;
    tempNode->entryPtr = entryPtr;
    return tempNode;
 }

void deleteTreeNode(struct TreeNode* node)
{
	if(node == NULL)
		return;
    
    if(node->left != NULL)
    {
        deleteTreeNode(node->left);
        free(node->left);
        node->left = NULL;
    }
    
    if(node->right != NULL)
    {
        deleteTreeNode(node->right);
        free(node->right);
        node->right = NULL;
    }
        
    if(node->entryPtr != NULL)
    {
        deleteTable(node->entryPtr);
        free(node->entryPtr);
        node->entryPtr = NULL;
    }
    
}


/** 
 * @brief   sets the left child of the struct TreeNode.
 */

void setLeft(struct TreeNode* current, struct TreeNode* newLeft)
{
	if(current != NULL)
		current->left = newLeft;
}



/** 
 * @brief  sets the right child of the struct TreeNode
 */

void setRight(struct TreeNode* current, struct TreeNode* newRight)
{
	if(current != NULL)
		current->right = newRight;
}


/** 
 * @brief  gets the left child of the struct TreeNode
 */
struct TreeNode* getLeft(struct TreeNode* current)
{
	if(current != NULL)
		return current->left;

	return NULL;
}

/** 
 * @brief gets the right child of the struct TreeNode
 */
struct TreeNode* getRight(struct TreeNode* current)
{
	if(current != NULL)
		return current->right;

	return NULL;
}

/** 
 * @brief returns a pointer to the DBentry the struct TreeNode contains.
 */
struct TreeEntry* getEntry(struct TreeNode* node)
{
	if(node != NULL)
		return node->entryPtr;

	return NULL;
}

/**
 * @brief Search for an entry by comparing a string to the current keys in the BST.
 * If the entry is found, we will return the structure so that the elements can be accessed as necessary.
 */

struct TreeEntry *findTable(struct TreeNode *node, char* name){
    	if(node == NULL)
        return NULL;

   // (*probes)++; //updates probes count

    if(node->entryPtr == NULL)
        return NULL; //this should never happen, protects against runtime errors

    char* nodeName = getTableName(node->entryPtr);

    if(strcmp(nodeName, name) == 0)
    {   //found entry
        return node->entryPtr;
    }
    else if(strcmp(name, nodeName) < 0)
    {   //search left
        if(node->left == NULL)
            return NULL;

        return findTable(node->left, name);
    }
    else if(strcmp(name, nodeName) > 0)
    {   //search right
        if(node->right == NULL)
            return NULL;

        return findTable(node->right, name);
    }

    return NULL;

}

/**
 * @brief Inserts, if it already exists returns false, otherwise true
 *
 */
bool insertTable(struct TreeNode* node, struct TreeEntry* newEntry)
{
	if(node == NULL)
		return false;

    if(node->entryPtr == NULL)
        return false;  //should never get here, this is protection against runtime errors

    char* nodeName = getTableName(node->entryPtr);

    if(strcmp(getTableName(newEntry), nodeName) == 0)
    {   //found existing entry
        return false;
    }
    else if(strcmp(getTableName(newEntry), nodeName) < 0)
    {   //search left
        if(node->left == NULL)
        {
            node->left = createTreeNode(newEntry);
            return true;
        }

        return insertTable(node->left, newEntry);
    }
    else if(strcmp(getTableName(newEntry), nodeName) > 0)
    {   //search right
        if(node->right == NULL)
        {
            node->right = createTreeNode(newEntry);
            return true;
        }

        return insertTable(node->right, newEntry);
    }

    return false;
}


struct TreeNode* removeTable(struct TreeNode* node, char* name, struct TreeNode** check)
{
	if(node == NULL)
		return NULL;

    if(node->entryPtr == NULL)
        return NULL; //should never get here, protects from runtime errors

    char* nodeName = getTableName(node->entryPtr);
    struct TreeNode* greatestNode = NULL;

    if(strcmp(name, nodeName) == 0)
    {   //found entry
        *check = node; //sets check to delete the node by the caller
        if(node->left == NULL) //left is empty, replace this node with its right child
            return node->right;

        greatestNode = getLargest(node->left); //finds the greatest key in left subtree

        if(node->left != greatestNode) //if the first to the left is the greatest, don't set its left child
            setLeft(greatestNode, node->left);

        setRight(greatestNode, node->right); //sets the right child

        return greatestNode; //sets the greatest node to replace the node to delete
    }
    else if(strcmp(name, nodeName) < 0)
    {   //search left
        if(node->left != NULL)
            node->left = removeTable(node->left, name, check);

        //this isn't the node to delete, therefore set the previous node to continue pointing here
        return node;
    }
    else if(strcmp(name, nodeName) > 0)
    {   //search right
        if(node->right != NULL)
            node->right = removeTable(node->right, name, check);

        //this isn't the node to delete, therefore set the previous node to continue pointing here
        return node;
    }

    return NULL;
}

/**
 * @brief In the case where we have to delete an entry that has two entries (to the left, and to the right)
 * We must find the MAX entry from the left subtree and replace it with this.
 * @return Returning the largest element
 */
struct TreeNode* getLargest(struct TreeNode* node)
{
	if(node == NULL)
		return NULL;

    //base case
    if(node->right == NULL)
        return node;

    //continues searching
    struct TreeNode* temp = NULL;
    temp = getLargest(node->right); //propagates the greatest up the stack

    //only if right child is the greatest, set right to point to the greatest's left so its not lost
    if(node->right == temp)
        node->right = temp->left;

    //returns the greatest
    return temp;
}

void printNodes(struct TreeNode* node)
{
	if(node == NULL)
		return;

	if(node->left != NULL)
		printNodes(node->left);
	if(node->entryPtr != NULL)
		printTable(node->entryPtr);
	if(node->right != NULL)
		printNodes(node->right);
}
