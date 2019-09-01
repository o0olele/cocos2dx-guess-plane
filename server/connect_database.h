#include <iostream> // cin,cout等
#include <iomanip>  // setw等
#include <mysql.h>  // mysql特有
#include <string.h>
#include <cstring>
#define MAXFILENAME 28
#define MAXSIZE 1024
struct ROW_INFO{
    char id[20];
    char passwd[20];
	char  nwin[3];
    char  nfail[3];
};

struct text_format
{
	char source_type;
	char response_type;
	unsigned short len;
	char *content;
};

struct DATABASE_INFO{
    ROW_INFO* rowdata;
    int row_num;
};

int Get_Update(char *chaxun, char *id, char *passwd,int model);
int Mysql_Init(MYSQL **mysql);
int Database_Load();
int Info_Update(char *id, char *changed,int model);
int Info_Insert(char* id, char* changed, char *nwin, char* nfail);
void padding(char *str,int lens);
