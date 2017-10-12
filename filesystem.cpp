
// filesystem.cpp

#include "filesystem.h"
#include <sys/stat.h>
#define MAX_BLOCK_NUMBER 256
#define MAX_BLOCK_SIZE 1024
#define MAX_INODE_NUMBER 512
#define MAX_DATANODE_NUM 2
using namespace std;

int strcmpi(char *p1, char *p2) {
	if (strlen(p1) != strlen(p2))
		return -1;

	for (int i = 0; i<strlen(p1); i++)
		if (p1[i] != p2[i])
			return -1;

	return 0;
}

void initiate() {
	char path[20] = "fileSystem";
	if (mkdir(path,0777)) {
		printf("error in creating file for the system, press ctrl+c to exit");
		while (true);
	}
	printf("sucessfully create the file for system.\n");

	FILE *userFile = fopen(userPath, "w");
	if (userFile == NULL) {
		printf("error in creating file for the user, press ctrl+c to exit");
		while (true);
	}
	else
		fclose(userFile);

	FILE* blockFile = fopen(blockPath, "w");
	if (blockFile == NULL) {
		printf("error in creating file for the block, press ctrl+c to exit");
		while (true);
	}
	else {
		for (int i = 0; i<MAX_BLOCK_NUMBER; i++)
			for (int j = 0; j<MAX_BLOCK_SIZE; j++)
				fputc('$', blockFile);
		fclose(blockFile);
	}

	FILE* bitFile = fopen(bitmapPath, "w");
	if (bitFile == NULL) {
		printf("error in creating file for the bitmap, press ctrl+c to exit");
		while (true);
	}
	else {
		for (int i = 0; i<MAX_BLOCK_NUMBER; i++) {
			bitmap[i] = 0;
			fprintf(bitFile, " %d", 0);
		}
		fclose(bitFile);
	}
}

void getUser() {
	while (1) {
		printf("account(length<8): ");
		scanf("%s", currUser.account);
		if (strlen(currUser.account) <= 8)
			break;
		else
			printf("username cannot exceed 8 bits.\n");
	}
	while (1) {
		printf("password(length<8): ");
		scanf("%s", currUser.password);
		if (strlen(currUser.password) <= 8)
			break;
		else
			printf("password canno exceed 8 bits.\n");
	}
}

void initBit() {
	FILE* fp = fopen(bitmapPath, "r");
	if (fp == NULL) {
		printf("error to open the bit file\n");
		while (1);
	}
	else {
		int i = 0;
		while (!feof(fp)) {
			fscanf(fp, "%d", &bitmap[i++]);
			if (i == MAX_BLOCK_SIZE - 1)
				break;
		}
	}
}


void initDir(char *dirpath) {
	FILE *p = fopen(dirpath, "r");
	if (p == NULL) {
		p = fopen(dirpath, "w");
		fclose(p);
		p = fopen(dirpath, "r");
	}
	inodeList* pp = (inodeList*)malloc(sizeof(inodeList));
	pp->next = NULL;
	inodeHead = NULL;
	int flag = 0;
	while (!feof(p)) {
		if (flag) {
			inodeList* pt = (inodeList*)malloc(sizeof(inodeList));
			pt->next = NULL;
			pp->next = pt;
			pp = pt;
		}

		int r = fscanf(p, "%s", pp->ind.filename);
		if (r == EOF) // end of file
			break;

		fscanf(p, "%d", &(pp->ind.num));
		if (max_node < pp->ind.num) {
			max_node = pp->ind.num;
		}
		fscanf(p, "%s", pp->ind.code);
		fscanf(p, "%d", &pp->ind.size);

		// 初始化数据块
		if (pp->ind.size != 0) {
			fscanf(p, "%d", &pp->ind.node_num);
			int temp = 0;
			for (int j = 0; j<pp->ind.node_num; j++) {
				fscanf(p, "%d", &pp->ind.fat[j].num);
				fscanf(p, "%d", &pp->ind.fat[j].begin);
				fscanf(p, "%d", &pp->ind.fat[j].end);
			}
		}

		// 超出内存
		inodeNumber++;
		if (inodeNumber == MAX_INODE_NUMBER)
			break;

		if (flag == 0) {
			inodeHead = pp;
			flag = 1;
		}
		curInode = pp;
	}
	fclose(p);
}


char *getPath(char *username) {
	char *res;
	res = (char*)malloc(50*sizeof(char));
	char t[20] = "./fileSystem";
	char tail[5] = ".dir";
	strcpy(res, t);
	strcat(res, username);
	strcat(res, tail);
	return res;
}

void saveDir() {
	FILE *fp = fopen(getPath(currUser.account), "w");
	if (fp == NULL) {
		printf("save data error, please enter ctrl+c to exit\n");
		while (1);
	}
	else {
		inodeList *p = inodeHead;
		while (p != NULL)
		{
			fprintf(fp, " %s", p->ind.filename);
			fprintf(fp, " %d", p->ind.num);
			fprintf(fp, " %s", p->ind.code);
			fprintf(fp, " %d", p->ind.size);
			if (p->ind.size != 0)
			{
				fprintf(fp, " %d", p->ind.node_num);
				for (int j = 0; j<p->ind.node_num; j++)
				{
					fprintf(fp, " %d", p->ind.fat[j].num);
					fprintf(fp, " %d", p->ind.fat[j].begin);
					fprintf(fp, " %d", p->ind.fat[j].end);
				}
			}
			p = p->next;
		}
		fclose(fp);
	}
}

void saveBit() {
	FILE* fp = fopen(bitmapPath, "w");
	if (fp == NULL) {
		printf("error while saving the bitmap, press ctrl+c to exit.\n");
		while (1);
	}

	for (int i = 0; i<MAX_BLOCK_NUMBER; i++)
		fprintf(fp, " %d", bitmap[i]);
	fclose(fp);

}

inodeList* isInDir(char *filename) {
	inodeList *ptr = inodeHead;
	while (ptr != NULL) {
		if (!strcmpi(ptr->ind.filename, filename))
			return ptr;
		ptr = ptr->next;
	}
	return NULL;
}

int lgrg(int num) {
	getUser();
	if (num == 0) { // login
		FILE *file = fopen(userPath, "r");
		if (file == NULL) {
			printf("error when opening the user file.");
			return 0;
		}
		user temp;
		while (!feof(file)) {
			fscanf(file, "%s", temp.account);
			fscanf(file, "%s", temp.password);
			if ((!strcmpi(temp.account, currUser.account)) && (!strcmpi(temp.password, currUser.password)))
			{
				fclose(file);
				return 1;
			}
		}
		fclose(file);
		return 0;
	}
	else { // register
		FILE *file = fopen(userPath, "a");
		if (file == NULL) {
			printf("error when opening the user file.");
			return 0;
		}
		fprintf(file, "%s ", currUser.account);
		fprintf(file, " %s\n", currUser.password);
		fclose(file);
		return 1;
	}
}

void dir() {
	printf("filename\tphysical address\tprotect code\t\tfile size\n");
	int i = 0;
	inodeList *pt = inodeHead;
	while (pt != NULL)
	{
		printf("%s\t%d\t\t%s\t\t%d\n", pt->ind.filename, pt->ind.num, pt->ind.code, pt->ind.size);
		pt = pt->next;
	}
}

void create() {

	int i;
	for (i = 0; i < MAX_BLOCK_NUMBER; i++) {
		if (bitmap[i] == 0)
			break;
	}
	if (i == MAX_BLOCK_NUMBER) {
		printf("no enough memory\n");
		return;
	}

	char tmp;
	inodeList *pt = (inodeList*)malloc(sizeof(inodeList));
	pt->next = NULL;
	while (1)
	{
		printf("filename: ");
		scanf("%s", pt->ind.filename);
		if (isInDir(pt->ind.filename) != NULL)
			printf("the filename is existed. please enter another:\n");
		else
			break;
	}

	pt->ind.num = inodeNumber++;
	// 保护码默认为r
	char code[10] = "r";
	strcpy(pt->ind.code, code);

	// 不支持读写操作，默认占用一个块
	pt->ind.size = 5;
	pt->ind.node_num = 1;
	char content[5] = "hhhh";

	pt->ind.fat[0].num = i;
	pt->ind.fat[0].begin = 0;
	pt->ind.fat[0].end = 5;

	if (curInode == NULL)
		inodeHead = pt;
	else
		curInode->next = pt;
	curInode = pt;
	saveDir();
	saveBit();
	printf("Succeed create file %s!\n", pt->ind.filename);
}

void del() {
	char tmp[30];
	printf("please enter the name for the file: ");
	scanf("%s", tmp);
	if (isInDir(tmp) == NULL) {
		printf("It doesn't exist\n");
		return;
	}
	else {
		inodeList *res = inodeHead;
		if (res == NULL) {
			printf("no files\n");
			return ;
		}
		int flag = 0;

		if (res->next == NULL)
			if (!strcmpi(res->ind.filename, tmp)) {
				printf("delete sucessfully\n");
				inodeHead = curInode = NULL;
				flag = 1;
		}

		if(!flag)
			if (!strcmpi(res->ind.filename, tmp)) {
				printf("delete sucessfully\n");
				inodeHead = res->next;
				flag = 1;
			}

		if(!flag)
			while (res->next != NULL) {
				if (!strcmpi(res->next->ind.filename, tmp)) {

					inodeList *tmp = res->next;
					res->next = tmp->next;
					res = tmp;

					printf("delete sucessfully\n");
					flag = 1;
					break;
				}
				res = res->next;
			}

		if (flag)
			for (int i = 0; i<res->ind.node_num; i++)
				bitmap[res->ind.fat[i].num] = 0;
	}
	saveDir();
	saveBit();
}

void open() {
	char file[50];
	printf("please enter the name of file: ");
	scanf("%s", file);
	selectedInode = isInDir(file);

	if (selectedInode == NULL)
		printf("It doesn't exist\n");
	else {
		printf("file%shas opened,enter close to close it.\n", file);
		char cmd[10];
		while (1) {
			scanf("%s", cmd);
			char *temp = "close";
			if (!strcmpi(cmd, temp)) {
				close();
				break;
			}
			else
				printf("first close the file please\n");
		}
	}
}

void close() {
	selectedInode = NULL;
	printf("The file is closed !\n");
}


int main() {
	fflush(stdout);
	setvbuf(stdout,NULL,_IONBF,0);
	printf("file system design\n");
	FILE *userFile = fopen(userPath, "r");
	if (userFile == NULL) // 第一次使用
		initiate();
	else
		fclose(userFile);

	char temp[10];
	int flag = 1;
	while (flag) {
		printf("please enter \'login\' or \'register\': ");
		scanf("%s", temp);
		if (!strcmpi(temp, (char *)"login")) {
			if (lgrg(0))
				flag = 0;
		}
		else if (!strcmpi(temp, (char *)"register")) {
			if (lgrg(1))
				flag = 0;
		}
		else
			printf("please enter again!");

	}
	printf("log in sucessfully\n");

	initDir(getPath(currUser.account));
	initBit();
	printf("done initialize\n");

	printf("welcome to the file system\n");
	printf("a few order for the system:\n");
	printf("exit\n");
	printf("dir\n");
	printf("create\n");
	printf("delete\n");
	printf("open\n");
	printf("close\n");

	char cmd[10];
	while (true) {
		printf("Command: ");
		scanf("%s", cmd);
		if (!strcmpi(cmd, (char *)"exit"))
			exit(0);
		else if (!strcmpi(cmd, (char *)"dir"))
			dir();
		else if (!strcmpi(cmd, (char *)"create"))
			create();
		else if (!strcmpi(cmd, (char *)"delete"))
			del();
		else if (!strcmpi(cmd, (char *)"open"))
			open();
		else if (!strcmpi(cmd, (char *)"close"))
			close();
	}




	return 0;
}
