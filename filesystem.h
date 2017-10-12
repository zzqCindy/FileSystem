// filesystem.h

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_BLOCK_NUMBER 256
#define MAX_BLOCK_SIZE 1024
#define MAX_INODE_NUMBER 512
#define MAX_DATANODE_NUM 2
using namespace std;

// 数据结构

typedef struct user { // 八位账号密码
	char account[10];
	char password[10];
}user;

typedef struct block {
	int number; // 块号
	int offset; // 块内偏移量
	char content[MAX_BLOCK_SIZE]; // 块内内容
}block;

typedef struct data {
	int num; // 块号
	int begin;
	int end;
}data;

typedef struct iNode {
	int node_num; // 数据块的数量
	int size; // 文件长度
	int num;  // inode编号
	char filename[30]; // 所指文件名
	struct data fat[MAX_DATANODE_NUM]; // 存储数据的块所在位置
	char code[30]; // 保护码
}iNode;

typedef struct inodeList {
	iNode ind;
	struct inodeList *next;
}inodeList;


char userPath[30] = ".\\fileSystem\\user";
char blockPath[30] = ".\\fileSystem\\block";
char bitmapPath[30] = ".\\fileSystem\\bitmap";
static char *dirpath;
static int bitmap[MAX_BLOCK_NUMBER];
static user currUser;
static int inodeNumber = 0;
int max_node = 0;
static bool login = false;
static inodeList *inodeHead;
static inodeList *curInode;
static inodeList *selectedInode;

void getUser();
void initiate();
void initBit();
void initDir(char *);
char *getPath(char *);
void saveDir();
void saveBit();
inodeList* isInDir(char *);


int lgrg(int);
void dir();
void create();
void del();
void open();
void close();


