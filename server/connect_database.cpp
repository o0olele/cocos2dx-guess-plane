#include "connect_database.h"
#include <string.h>
#include <stdlib.h>
using namespace std;

DATABASE_INFO database;

int Get_Update(char* chaxun, char *id, char *changed,int model)
{
	char condition[100] = " where id = '";
	if (id == NULL)
		return -1;
	chaxun[0] = '\0';
	strcpy(chaxun, "update info set ");
	if(model == 0)
		strcat(chaxun, "passwd = '");
	else if(model == 1)
		strcat(chaxun, "nwin = ");
	else if(model == 2)
		strcat(chaxun, "nfail = ");
	strcat(chaxun, changed);
	if (model == 0)
		strcat(chaxun, "'");
	strcat(condition, id);
	strcat(condition, "'");
	strcat(chaxun, condition);
	cout << "检索式" << chaxun << endl;
	return 1;
}

int Mysql_Init(MYSQL **mysql)
{
	if ((*mysql = mysql_init(NULL)) == NULL)
	{
		cout << "mysql_init failed" << endl;
		return -1;
	}

	if (mysql_real_connect(*mysql, "localhost", "root", "123456789", "mydb", 0, NULL, 0) == NULL)
	{
		cout << "mysql_real_connect failed(" << mysql_error(*mysql) << ")" << endl;
		mysql_close(*mysql);
		return -1;
	}

	mysql_set_character_set(*mysql, "utf8");
	return 1;
}

int Info_Insert(char* id, char* passwd, char *nwin, char* nfail)
{
	MYSQL *mysql;
	MYSQL_RES *result;
	MYSQL_ROW row;
	char buf[2000];
	time_t timer;//time_t就是long int 类型
	struct tm *tblock;
	timer = time(NULL);
	tblock = localtime(&timer);
	strcpy(buf, "insert into info ");
	strcat(buf, " values('");
	strcat(buf, id);
	strcat(buf, "','");
	strcat(buf, passwd);
	strcat(buf, "','");
	strcat(buf, nwin);
	strcat(buf, "','");
	strcat(buf, nfail);
	strcat(buf, "');");
	cout << "新的注册用户" << buf << endl;
	int status;
	status = Mysql_Init(&mysql);
	if (status == -1)
		return -1;
	if (mysql_query(mysql, buf)) //只要有表，就一定返回0
	{
		cout << "mysql_query>> failed(" << mysql_error(mysql) << ")" << endl;
		return -1;
	}

	if ((result = mysql_store_result(mysql)) == NULL)
	{
		cout << "mysql_store_result failed" << endl;
		return -1;
	}

	int num = (int)mysql_num_rows(result);
	cout << "select return " << num << " records" << endl;

	while ((row = mysql_fetch_row(result)) != NULL)
		;

	mysql_free_result(result);

	mysql_close(mysql);

	return num;
}

int Database_Load()
{
	MYSQL *mysql;
	MYSQL_RES *result;
	MYSQL_ROW row;
	char search[100] = "select * from info";
	int status;
	status = Mysql_Init(&mysql);
	if (status == -1)
		return -1;

	if (mysql_query(mysql, search)) //只要有表，就一定返回0
	{
		cout << "mysql_query>> failed(" << mysql_error(mysql) << ")" << endl;
		return -1;
	}

	if ((result = mysql_store_result(mysql)) == NULL)
	{
		cout << "mysql_store_result failed" << endl;
		return -1;
	}

	int num = (int)mysql_num_rows(result);
	int i = 0;
	cout << "select return " << num << " records" << endl;

	database.row_num = 0;
	while ((row = mysql_fetch_row(result)) != NULL)
	{
		strcpy(database.rowdata[database.row_num].id, row[0]);
		strcpy(database.rowdata[database.row_num].passwd, row[1]);
		strcpy(database.rowdata[database.row_num].nwin, row[2]);
		strcpy(database.rowdata[database.row_num].nfail, row[3]);
		database.row_num++;
	}

	mysql_free_result(result);

	mysql_close(mysql);

	return num;
}

int Info_Update(char *id, char *changed,int model)
{
	MYSQL *mysql;
	MYSQL_RES *result;
	MYSQL_ROW row;
	int num;

	if (id == NULL)
		return -1;
	int status;
	status = Mysql_Init(&mysql);
	if (status == -1)
		return -1;

	char *chaxun = new char[100];
	
	if (Get_Update(chaxun, id, changed,model) == -1)
	{
		cout << "更新目标不明确" << endl;
		return -1;
	}

	if (mysql_query(mysql, chaxun)) //没查到密码则不为0
	{
		cout << "mysql_query>> failed(" << mysql_error(mysql) << ")" << endl;
		return -1;
	}

	if ((result = mysql_store_result(mysql)) == NULL)
	{
		cout << "mysql_store_result failed" << endl;
		return -1;
	}


	num = (int)mysql_num_rows(result);
	int i = 0;
	cout << "select return " << num << " records" << endl;

	database.row_num = 0;
	while ((row = mysql_fetch_row(result)) != NULL)
		;

	mysql_free_result(result);

	mysql_close(mysql);

	delete chaxun;

	return num;
}
