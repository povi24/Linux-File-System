// Copyright 2021-2022 Povarna Andreea & Nicolae Radu 314CAa
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"
#define TREE_CMD_INDENT_SIZE 4
#define NO_ARG ""
#define PARENT_DIR ".."

// cream un nod generic
TreeNode* create_generic_node(char *name)
{
	TreeNode* node = malloc(sizeof(TreeNode));
	node->name = name;
	node->parent = NULL;
	node->content = NULL;
	return node;
}

// cream un nod de tip folder
TreeNode* create_folder_node(char *folder_name)
{
	TreeNode *folder_node = create_generic_node(folder_name);
	folder_node->type = FOLDER_NODE;
	folder_node->content = calloc(1, sizeof(FolderContent));
	((FolderContent *)(folder_node->content))->children = calloc(1, sizeof(List));
	((FolderContent *)(folder_node->content))->children->head = NULL;

	return folder_node;
}

// cream un nod de tip fisier
TreeNode* create_file_node(char *file_name)
{
	TreeNode *file_node = create_generic_node(file_name);
	file_node->type = FILE_NODE;
	return file_node;
}

// intializam root-ul
FileTree* createFileTree(char* rootFolderName) {
	FileTree *home = malloc(sizeof(*home));
	if(home == NULL) {
		return NULL;
	}
	home->root = create_folder_node(rootFolderName);

	return home;
}


void touch(TreeNode* currentNode, char* fileName, char* fileContent)
{
	// cream fisierul
	TreeNode *file = create_file_node(fileName);
	FileContent *file_content_pointer = malloc(sizeof(FileContent));
	file_content_pointer->text = fileContent;
	file->content = file_content_pointer;

	ListNode *curr_file = malloc(sizeof(ListNode));
	curr_file->next = NULL;
	curr_file->info = file;
	ListNode *first =((FolderContent*)currentNode->content)->children->head;

	if(first == NULL){
		((FolderContent*)currentNode->content)->children->head = curr_file;
	} else{
		while(first != NULL){
			if(strcmp(first->info->name, fileName) == 0)
				return;
			first = first->next;
		}
		curr_file->next = ((FolderContent*)currentNode->content)->children->head;
		((FolderContent*)currentNode->content)->children->head = curr_file;
	}
}

void ls(TreeNode* currentNode, char* arg) {
	int ok = 0;
	char *content;

	if(strlen(arg) == 0){
		ListNode *first =((FolderContent*)currentNode->content)->children->head;
		while(first != NULL){
			printf("%s\n", first->info->name);
			first = first->next;
		}
	} else{
		ListNode *first =((FolderContent*)currentNode->content)->children->head;

		while(first != NULL){
			content = ((FileContent *)first->info->content)->text;

			if(strcmp(arg, first->info->name) == 0){
				if(first->info->type == FILE_NODE) {
					printf("%s: %s\n", first->info->name, content);
				} else{
					ListNode *continut = ((FolderContent*)first->info->content)->children->head;
					while(continut != NULL){
						printf("%s\n", continut->info->name);
						continut = continut->next;
					}
				}
				ok = 0;
				return;
			} else{
				ok = 1;
			}
			first = first->next;
		}
	}
	if(ok == 1){
		printf("ls: cannot access '%s': No such file or directory", arg);
	}
}


void freeTreeNode(TreeNode* node) {
	free(node->name);

	if (node->type == FILE_NODE) {
		free(((FileContent*)node->content)->text);
		free(((FileContent*)node->content));
	} else  {
		ListNode *curr = NULL;
		curr = ((FolderContent*)node->content)->children->head;

		while (curr) {
			ListNode* next = curr->next;
			freeTreeNode(curr->info);
			free(curr);
			curr = next;
		}
		free(((FolderContent*)node->content)->children);
		free(((FolderContent*)node->content));
	}
	free(node);
}

void freeTree(FileTree fileTree) {
	freeTreeNode(fileTree.root);
}

void pwd(TreeNode* treeNode) {
	if(treeNode == NULL)
		return;
	pwd(treeNode->parent);
	if(strcmp(treeNode->name, "root") == 0)
		printf("%s", treeNode->name);
	else
		printf("/%s", treeNode->name);
}


TreeNode* cd(TreeNode* currentNode, char* path) {
	char *partialpath;
	TreeNode *curr_folder = currentNode;
	partialpath = strtok(path, "/");

	while(partialpath){
		ListNode *first =((FolderContent*)curr_folder->content)->children->head;
		// verificam daca ce se afla pana la / este ..
		if(strcmp(partialpath, "..") != 0){
			while(first != NULL){
				// daca gasim un folder care are acelasi nume cu argumentul,
				// folderul curent va deveni directorul cu numele argumentului
				if(strcmp(first->info->name, partialpath) == 0){
					curr_folder = first->info;
				}
			first = first->next;
			}
		} else{
			// daca sunt folosite .. folderul curent devine parintele sau
			curr_folder = curr_folder->parent;
		}
		partialpath = strtok(NULL, "/");
	}
	// daca nu se poate ajunge la destinatie, comanda va afisa eroare
	if(curr_folder == currentNode){
		printf("cd: no such file or directory: %s", path);
	}
	return curr_folder;
}


void tree(TreeNode* currentNode, char* arg, int n, int *k, int *j) {
	ListNode *First = ((FolderContent *)currentNode->content)->children->head;

	if(strlen(arg) == 0){
		while(First != NULL){
			for(int i = 0 ; i < n; i++)
				printf("\t");

			printf("%s\n", First->info->name);
			if(First->info->type == FILE_NODE)
			{
				*j = *j+1;
			}
			else
			{
				*k = *k + 1;
				tree(First->info, arg, n+1, k, j);
			}
			First = First->next;
		}
	} else{
		int ok = 0;
		TreeNode *curr_folder = currentNode;
		char *argpath = strtok(arg, "/");

		while(argpath){
		ok = 0;
		ListNode *first =((FolderContent*)curr_folder->content)->children->head;
		if(strcmp(argpath, "..") != 0){
			while(first != NULL){
				if(strcmp(first->info->name, argpath) == 0){
					curr_folder = first->info;
					ok = 1;
				}
			first = first->next;
			}
		} else{
			curr_folder = curr_folder->parent;
		}
			argpath = strtok(NULL, "/");
		}
		if(curr_folder->type == FILE_NODE)
		{
			printf("%s [error opening dir]\n\n", arg);
		} else if (ok == 0) {
			printf("%s [error opening dir]\n\n", arg);
		} else{
		ListNode *Arg = ((FolderContent *)curr_folder->content)->children->head;
		while(Arg != NULL){
			for(int i = 0 ; i < n; i++)
				printf("\t");
			printf("%s\n", Arg->info->name);
			if(Arg->info->type == FILE_NODE)
			{
				*j = *j+1;
			}
			else
			{
				*k = *k + 1;
				tree(Arg->info, arg, n+1, k, j);
			}
			Arg = Arg->next;
		}
	}
	}
}


void mkdir(TreeNode* currentNode, char* folderName) {
	int ok = 0;
	TreeNode *folder = create_folder_node(folderName);
	ListNode *curr_folder = malloc(sizeof(ListNode));
	curr_folder->next = NULL;
	curr_folder->info = folder;
	ListNode *first =((FolderContent*)currentNode->content)->children->head;
	curr_folder->info->parent = currentNode;

	if(first == NULL){
		((FolderContent*)currentNode->content)->children->head = curr_folder;
	} else{
		while(first != NULL){
			if(strcmp(first->info->name, folderName) == 0){
				printf("mkdir: cannot create directory '%s': File exists", folderName);
				free(((FolderContent*)folder->content)->children);
				free(((FolderContent*)folder->content));
				free(folder->name);
				free(folder);
				free(curr_folder);
				return;
			}
			first = first->next;
		}
		curr_folder->next = ((FolderContent*)currentNode->content)->children->head;
		((FolderContent*)currentNode->content)->children->head = curr_folder;
	}
}


void rmrec(TreeNode* currentNode, char* resourceName) {
	ListNode *prev = NULL;
	ListNode *first =((FolderContent*)currentNode->content)->children->head;

	while(first && strcmp(first->info->name, resourceName) != 0){
		prev = first;
		first = first->next;
	}
	if(!first) {
		printf("rmrec: failed to remove '%s': No such file or directory",resourceName);
		return;
	} else if (first->info->type == FILE_NODE){
		if(prev == NULL){
			((FolderContent*)currentNode->content)->children->head = first->next;
		} else{
			prev->next = first->next;
		}
		FileContent* file_text = first->info->content;
		free(file_text->text);
		free(first->info->content);
		free(first->info->name);
		free(first->info);
		free(first);
	} else{
		if(prev == NULL){
			((FolderContent*)currentNode->content)->children->head = first->next;
		} else{
			prev->next = first->next;
		}
		freeTreeNode(first->info);
		free(first);
	}
}


void rm(TreeNode* currentNode, char* fileName) {
	ListNode *prev = NULL;
	ListNode *first =((FolderContent*)currentNode->content)->children->head;

	while(first && strcmp(first->info->name, fileName) != 0){
		prev = first;
		first = first->next;
	}
	if(!first) {
		printf("rm: failed to remove '%s': No such file or directory", fileName);
		return;
	} else if (first->info->type == FOLDER_NODE){
		printf("rm: cannot remove '%s': Is a directory", fileName);
		return;
	} else {
		if(prev == NULL){
			((FolderContent*)currentNode->content)->children->head = first->next;
		} else{
			prev->next = first->next;
		}
	}
	FileContent* file_text = first->info->content;
	free(file_text->text);
	free(first->info->content);
	free(first->info->name);
	free(first->info);
	free(first);
}

void rmdir(TreeNode* currentNode, char* folderName) {
	ListNode *prev = NULL;
	ListNode *first =((FolderContent*)currentNode->content)->children->head;

	while(first && strcmp(first->info->name, folderName) != 0){
		prev = first;
		first = first->next;
	}
	if(!first) {
		printf("rmdir: failed to remove '%s': No such file or directory", folderName);
		return;
	} else if (first->info->type == FILE_NODE){
		printf("rmdir: failed to remove '%s': Not a directory", folderName);
		return;
	} else if (((FolderContent*)first->info->content)->children->head != NULL){
		printf("rmdir: failed to remove '%s': Directory not empty", folderName);
		return;
	} else{
		if(prev == NULL){
			((FolderContent*)currentNode->content)->children->head = first->next;
		} else{
			prev->next = first->next;
		}
	}
	freeTreeNode(first->info);
	free(first);
}

void cp(TreeNode* currentNode, char* source, char* destination) {
	char *partialpath;
	int i = 0, j = 0;
	char *create = malloc(sizeof(char) * 10);
	char *destinatie;
	destinatie = malloc(sizeof(char) * 10);
	strcpy(destinatie, destination);
	TreeNode *curr_folder = currentNode;
	TreeNode *File = currentNode;
	partialpath = strtok(destination, "/");

	while(partialpath){
		ListNode *first =((FolderContent*)curr_folder->content)->children->head;
		if(strcmp(partialpath, "..") != 0){
			while(first != NULL){
				if(strcmp(first->info->name, partialpath) == 0){
					curr_folder = first->info;
					i++;
				}
			first = first->next;
			}
		} else{
			curr_folder = curr_folder->parent;
			i++;
		}
		j++;
		if(j != i)
			strcpy(create, partialpath);
		partialpath = strtok(NULL, "/");
	}
	char *sourcepath = strtok(source, "/");
	while(sourcepath){
		ListNode *Source =((FolderContent*)File->content)->children->head;
		if(strcmp(sourcepath, "..") != 0){
			while(Source != NULL){
				if(strcmp(Source->info->name, sourcepath) == 0){
					File = Source->info;
				}
			Source = Source->next;
			}
		} else{
			File = File->parent;
		}
		sourcepath = strtok(NULL, "/");
	}
	if(j - i == 1){
		char *content = malloc(sizeof(char) * 10);
		strcpy(content, ((FileContent *)File->content)->text);
		char *name = malloc(sizeof(char) * 10);
		strcpy(name, create);
		touch(curr_folder, name, content);
	} else if (i == j){
		if(curr_folder->type == FILE_NODE){
				strcpy(((FileContent *)curr_folder->content)->text,
							((FileContent *)File->content)->text);
		} else{
			if(File->type == FILE_NODE){
				char *name = malloc(sizeof(char) * 10);
				strcpy(name, File->name);
				char *content = malloc(sizeof(char) * 10);
				strcpy(content, ((FileContent *)File->content)->text);
				touch(curr_folder, name, content);
			} else{
				printf("cp: -r not specified; omitting directory '%s'", source);
			}
		}
	} else{
		printf("cp: failed to access '%s': Not a directory", destinatie);
	}
	free(destinatie);
	free(create);
}

void mv(TreeNode* currentNode, char* source, char* destination) {
    char *partialpath;
	int i = 0, j = 0;
	char *create = malloc(sizeof(char) * 10);
	char *destinatie;
	destinatie = malloc(sizeof(char) * 10);
	strcpy(destinatie, destination);
	TreeNode *DeleteFile = currentNode;
	TreeNode *curr_folder = currentNode;
	TreeNode *File = currentNode;
	partialpath = strtok(destination, "/");

	while(partialpath){
		ListNode *first =((FolderContent*)curr_folder->content)->children->head;
		if(strcmp(partialpath, "..") != 0){
			while(first != NULL){
				if(strcmp(first->info->name, partialpath) == 0){
					curr_folder = first->info;
					i++;
				}
			first = first->next;
			}
		} else{
			curr_folder = curr_folder->parent;
			i++;
		}
		j++;
		if(j != i)
			strcpy(create, partialpath);
		partialpath = strtok(NULL, "/");
	}
	char *sourcepath = strtok(source, "/");
	while(sourcepath){
		DeleteFile = File;
		ListNode *Source =((FolderContent*)File->content)->children->head;
		if(strcmp(sourcepath, "..") != 0){
			while(Source != NULL){
				if(strcmp(Source->info->name, sourcepath) == 0){
					File = Source->info;
				}
			Source = Source->next;
			}
		} else{
			File = File->parent;
		}
		sourcepath = strtok(NULL, "/");
	}
	if(j - i == 1){
		char *content = malloc(sizeof(char) * 10);
		strcpy(content, ((FileContent *)File->content)->text);
		char *name = malloc(sizeof(char) * 10);
		strcpy(name, create);
		char *delname = malloc(sizeof(char) * 10);
		strcpy(delname, File->name);
		touch(curr_folder, name, content);
		rm(DeleteFile, delname);
	} else if (i == j){
		if(curr_folder->type == FILE_NODE){
				strcpy(((FileContent *)curr_folder->content)->text,
							((FileContent *)File->content)->text);
				rm(DeleteFile, File->name);
		} else{
			if(File->type == FILE_NODE){
				char *name = malloc(sizeof(char) * 10);
				strcpy(name, File->name);
				char *content = malloc(sizeof(char) * 10);
				strcpy(content, ((FileContent *)File->content)->text);
				touch(curr_folder, name, content);
				rm(DeleteFile, name);
			} else{
				List *list;
				list = ((FolderContent*)File->parent->content)->children;
				ListNode *rupere = list->head;
				ListNode *prev = NULL;
				while (rupere != NULL && strcmp(rupere->info->name, File->name) != 0)
				{
					prev = rupere;
					rupere = rupere->next;
				}
				if(prev == NULL){
					list->head = list->head->next;
					rupere->next = NULL;
				} else{
					prev->next = rupere->next;
					rupere->next = NULL;
				}
				rupere->info->parent = curr_folder;
				List *toadd;
				toadd = ((FolderContent*)curr_folder->content)->children;
				rupere->next = toadd->head;
				toadd->head = rupere;
				}
			}
	} else{
		printf("mv: failed to access '%s': Not a directory", destinatie);
	}
	free(destinatie);
	free(create);
}
