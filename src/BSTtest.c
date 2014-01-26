#include "TreeDB.h"
#include "TreeNode.h"
#include "TreeEntry.h"
#include "storage.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

int main(int argc, char *argv[])
{
	struct TreeDB* tree = createTreeDB();
	char line[MAX_CONFIG_LINE_LEN];
	//BINARY SEARCH TREE TEST CODE......COMMAND PROMPT
	bool loopit = true;
	char filename[100] = "Test_Table_tree_code.txt";
	FILE *file = fopen(filename, "r");
					//error_occurred = 1;
	while(loopit)
	{



		//char *l = fgets(line, sizeof line, file);
		int BSTchoice;
		printf(">>Options:\n>>1: insert\n>>2: remove\n>>3: find\n>>4: count\n>>5: clear\n>>6: print\n>>7: printProbes\n>>8: quit\n>>");
		scanf("%d", &BSTchoice);

		if(BSTchoice == 1)
		//if(strcmp(l, "1\n") == 0)
		{
			char* name = (char*)malloc(sizeof(char)*MAX_TABLE_LEN);
			printf("Insert Table Name: ");
			scanf("%s", name);
			//char *l = fgets(line, sizeof line, file);
			if(!insert(tree, createTable(name)))
			//if(!insert(tree, createTable(l)))
				printf("The entry %s already exists\n", name);
				//printf("The entry %s already exists\n", l);
			else
				printf("Success\n");
		}
		else if(BSTchoice == 2)
		//else if(strcmp(l, "2\n") == 0)
		{
			char* name = (char*)malloc(sizeof(char)*MAX_TABLE_LEN);
			printf("Remove Table Name: ");
			scanf("%s", name);
			//char *l = fgets(line, sizeof line, file);
			//removeDBtable(tree, name);
			//removeDBtable(tree, l);
		}
		else if(BSTchoice == 3)
		//else if(strcmp(l, "3\n") == 0)
		{
			char* name = (char*)malloc(sizeof(char)*MAX_TABLE_LEN);
			printf("Find Table Name: ");
			scanf("%s", name);
			//char *l = fgets(line, sizeof line, file);
			struct TreeEntry* querry = find(tree, name);
			//struct TreeEntry* querry = find(tree, l);
			if(querry != NULL)
				printf("%s\n", getTableName(querry));
			else
				printf("The entry %s could not be found\n", name);
				//printf("The entry %s could not be found\n", l);
		}
		else if(BSTchoice == 4)
		//else if(strcmp(l, "4\n") == 0)
		{
			//printf("Table count: %d\n", count(tree));
		}
		else if(BSTchoice == 5)
		//else if(strcmp(l, "5\n") == 0)
		{
			clear(tree);
		}
		else if(BSTchoice == 6)
		//else if(strcmp(l, "6\n") == 0)
		{
			printTree(tree);
		}
		else if(BSTchoice == 7)
		//else if(strcmp(l, "7\n") == 0)
		{
		//	printf("Nodes traversed: %d\n", getProbes(tree));
		}
		else if(BSTchoice == 8)
		//else if(strcmp(l, "8\n") == 0)
		{
			loopit = false;
		}
	}

	return 0;
}
