#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <net/if.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include "connect_database.h"
#include "md5.h"

#define MAXCONN 4096
#define MAX 1024
#define TIME_BEAT 6

extern DATABASE_INFO database;



struct PlaneConf
{
	unsigned x1;
	unsigned y1;
	unsigned x2;
	unsigned y2;
};

struct clientsfd_id
{
	int clientsfd;
	char id[8];
	char passwd[32];
	char plane_head[3];
	char plane_tail[3];
	char nwin[3];
	char nfail[3];
	int rival;
	int online;
	int islogin;
	int isgaming;
	int isready;
	int isalive;
	int order;
	int score;
	int base;
	int final;
	int pan[10][10];
	int plane_out[3];
	clientsfd_id * next;
};
typedef clientsfd_id*  clientsfd_ptr;
clientsfd_ptr client_head = new clientsfd_id;

int num_client = 0;
int closed_client = 0;
int jishu = 0;
char workpath[100];
char file_buffer[MAXSIZE * 10];
char cscore[4];
char strpot[100];
int buf_size;
int shun = 0;

void rizhi(char *buf)
{
	char *rfile = new char[1000];
	char *content = new char[MAXSIZE];
	strcpy(rfile, workpath);
	strcat(rfile, "server-record.txt");
	printf("写入文件名:%s\n", rfile);
	int fp = open(rfile, O_CREAT | O_APPEND | O_WRONLY, 0777);
	if (fp == -1)
		printf("errorsssss\n");
	time_t timer; //time_t就是long int 类型
	struct tm *tblock;
	timer = time(NULL);
	tblock = localtime(&timer);

	strcpy(content, asctime(tblock));
	strcat(content, "	");
	strcat(content, buf);
	int len = strlen(content);
	int lenss = write(fp, content, len);
	printf("写入字节数：%d\n", lenss);
	close(fp);
	delete rfile;
	delete content;
}

int Is_write(int sockfd)
{
	struct tcp_info info;
	int len = sizeof(info);
	getsockopt(sockfd, IPPROTO_TCP, TCP_INFO, &info, (socklen_t *)&len);
	if ((info.tcpi_state == TCP_ESTABLISHED))
		return 1;
	else
		return 0;

	return 1;
}

int encrypted(char *str)
{
	int i;
	unsigned char decrypt[16];
	MD5_CTX md5;
	MD5Init(&md5);
	MD5Update(&md5, (unsigned char *)str, strlen(str));
	MD5Final(&md5, decrypt);
	for (i = 0; i < 16; i++)
		str[i] = decrypt[i] % 26 + 97;
	str[16] = '\0';
	return 0;
}

void padding(char *str, int lens)
{
	int len = strlen(str);
	int i = 0;
	for (i = len + 1; i < lens; i++)
		str[i] = '\0';
}

void charadd(char* str)
{
	int num = atoi(str);
	num++;
	if (num < 10)
		str[0] = num + '0';
	else
	{
		str[0] = num / 10 + '0';
		str[1] = num % 10 + '0';
		str[2] = '\0';
	}
}

void init_text_format(text_format *head, char *recvbuf)
{
	head->source_type = recvbuf[0];
	head->response_type = recvbuf[1];
	head->len = (unsigned char)recvbuf[3];
	head->len = head->len * 256;
	head->len += (unsigned char)recvbuf[2];
}

//server
int Sendstring(int sockfd, text_format *text)
{
	struct timeval timelimit;
	fd_set fd;
	int res;
	int len = ntohs(text->len);
	char *content = new char[len];
	int i = 0;
	char tcon[10];

	content[0] = text->source_type;
	content[1] = text->response_type;

	sprintf(tcon, "%04x", text->len);
	for (i = 0; i < 4; i++)
		if (tcon[i] <= '9')
			tcon[i] -= '0';
		else
			tcon[i] = tcon[i] - 'a' + 10;
	content[2] = tcon[0] * 16 + tcon[1];
	content[3] = tcon[2] * 16 + tcon[3];
	content[4] = 0;

	if (text->content)
		memcpy(content + 4, text->content, len - 4);

	timelimit.tv_sec = 0;
	timelimit.tv_usec = 0;

	FD_ZERO(&fd);
	FD_SET(sockfd, &fd);

	if (select(sockfd + 1, NULL, &fd, NULL, &timelimit) > 0 && FD_ISSET(sockfd, &fd))
	{
		while ((res = write(sockfd, content, len)) <= 0 && errno == EINTR)
			;
		if (res == len)
		{
			printf("send success!\n");
			printf("len:%d\n", len);
			printf("packet: ");
			for (i = 0; i < len; i++)
				printf("%02x ", (unsigned char)content[i]);
			printf("\n");
		}
		else
		{
			printf("error send\n");
			return -1;
		}
	}
	else
	{
		perror("send error");
		delete content;
		return -1;
	}
	delete content;
	return 1;
}

int Recvstring(int sockfd, char *recvbuf)
{
	struct timeval timelimit;
	fd_set fd;
	int recvsum;
	int res;
	timelimit.tv_sec = 0;
	timelimit.tv_usec = 0;

	while (1)
	{
		recvsum = 0;
		FD_ZERO(&fd);
		FD_SET(sockfd, &fd);
		res = select(sockfd + 1, &fd, NULL, NULL, &timelimit);
		if (res > 0 && FD_ISSET(sockfd, &fd))
		{
			while ((recvsum = read(sockfd, recvbuf, MAXSIZE)) <= 0 && errno == EINTR)
				;

			if (recvsum <= 0)
			{
				//perror("this is my  error");
				return -1;
			}
			if (recvsum > 0)
			{
				return recvsum;
			}
		}
		else
			return -1;
	}
	return 0;
}

int send_format(int sockfd, char source_type, char response_type, short len, char *content)
{
	struct text_format *head = new text_format;
	head->source_type = source_type;
	head->response_type = response_type;
	head->len = htons(len);
	if (content)
	{
		head->content = new char[len];
		memcpy(head->content, content, len - 4);
	}

	int res = Sendstring(sockfd, head);
	if (content)
		delete head->content;
	delete head;
	return res;
}

void change_score(int score)
{
	if (score == 100)
	{
		cscore[0] = '1'; 
		cscore[1] = '0'; 
		cscore[2] = '0'; 
		cscore[3] = '\0';
	}
	else if (score >= 10)
	{
		cscore[0] = score / 10 + '0';
		cscore[1] = score % 10 + '0';
		cscore[2] = '\0';
	}
	else
	{
		cscore[0] = score + '0';
		cscore[1] = '\0';
	}

}

int getfriendslist(int sockfd, char *online_text)
{
	int i, start, end;
	start = 0;
	for (clientsfd_ptr client_ptr = client_head->next; client_ptr != NULL; client_ptr = client_ptr->next)
		if (client_ptr->clientsfd == sockfd || client_ptr->islogin == 1)
		{
			for (i = 0; i < database.row_num; i++)
			{
				if (strcmp(database.rowdata[i].id, client_ptr->id) == 0)
				{
					memcpy(&online_text[start], client_ptr->id, 8);
					char num[2];
					num[0] = atoi(database.rowdata[i].nwin);
					num[1] = '\0';
					memcpy(&online_text[start + 8], num, 2);
					num[0] = atoi(database.rowdata[i].nfail);
					memcpy(&online_text[start + 10], num, 2);
					start += 12;
					break;
				}
			}
		}

	end = start;
	for (int i = 0; i < end; i += 12)
		printf("friend: %s-%s-%s\n", &online_text[i], &online_text[i + 8], &online_text[i + 10]);
	return end;
}

int Winner(clientsfd_ptr win, clientsfd_ptr lose, int cause)
{
	char score[2];
	score[0] = win->score;
	score[1] = lose->score;
	win->isgaming = 0; lose->isgaming = 0; win->isready = 0; lose->isready = 0;
	if (cause != 9999)
	{
		send_format(win->clientsfd, 0x78, 0x10 + cause, 6, score);
		score[1] = win->score;
		score[0] = lose->score;
		if (cause != 2)
			send_format(lose->clientsfd, 0x78, 0x20, 6, score);
		charadd(win->nwin);
		charadd(lose->nfail);
		
		char *recordtext = new char[MAXSIZE];
		strcpy(recordtext, "WINNER: "); strcat(recordtext, win->id); strcat(recordtext, " score:"); change_score(win->score); strcat(recordtext, cscore); strcat(recordtext, " win:"); strcat(recordtext, win->nwin); strcat(recordtext, " lose:"); strcat(recordtext, win->nfail); strcat(recordtext, "\r\n");
		strcat(recordtext, "LOSER: "); strcat(recordtext, lose->id); strcat(recordtext, " score:"); change_score(lose->score); strcat(recordtext, cscore); strcat(recordtext, " win:"); strcat(recordtext, lose->nwin); strcat(recordtext, " lose:"); strcat(recordtext, lose->nfail); strcat(recordtext, "\r\n");
		rizhi(recordtext);
		delete recordtext;
		Info_Update(win->id, win->nwin, 1);
		Info_Update(lose->id, lose->nfail, 2);
		Database_Load();
	}
	else
	{
		send_format(win->clientsfd, 0x78, 0x30, 6, score);
		send_format(lose->clientsfd, 0x78, 0x30, 6, score);
		return 1;
	}

	char *online_text = new char[1024];
	int lens = getfriendslist(win->clientsfd, online_text);

	for (clientsfd_ptr client_ptr = client_head->next; client_ptr != NULL; client_ptr = client_ptr->next)
		if (client_ptr->islogin == 1 && client_ptr->online == 1)
			send_format(client_ptr->clientsfd, 0x73, 0, lens + 4, online_text);

	delete online_text;
}

void disconnect_record(char *id)
{
	char *recordtext = new char[MAXSIZE];
	strcpy(recordtext, "ID:");
	strcat(recordtext, id);
	strcat(recordtext, "断线了\r\n");
	rizhi(recordtext);
	delete recordtext;
}

void client_down()
{
	clientsfd_ptr temp = NULL;
	for (clientsfd_ptr client_ptr = client_head; client_ptr->next != NULL; )
	{
		if (client_ptr->next->online == 0)
		{
			temp = client_ptr->next;
			char* tid = new char[8];
			strcpy(tid, temp->id);
			client_ptr->next = temp->next;
			printf("sockfd:%d 断开连接!\n", temp->clientsfd);
			if(temp->islogin)
				disconnect_record(temp->id);
			for (clientsfd_ptr client_on = client_head->next; client_on != NULL; client_on = client_on->next)
				if (temp->islogin && client_on->online && client_on->islogin)
				{
					send_format(client_on->clientsfd, 0x7e, 0, 12, tid);
				}

			if (temp->isready == 1)
			{
				clientsfd_ptr client_on;
				for (client_on = client_head->next; client_on != NULL; client_on = client_on->next)
					if (client_on->rival == temp->clientsfd)
						break;
				Winner(client_on, temp, 2);
			}

			close(temp->clientsfd);
			delete tid;
			delete temp;
			printf("-------------------client down is over----------------------\n");
			continue;
		}
		client_ptr = client_ptr->next;
	}
}

void signalHandler(int signo)
{
	clientsfd_ptr temp;
	switch (signo) {
	case SIGALRM:
		printf("----------------heart beat----------------\n", shun++);
		for (clientsfd_ptr client_ptr = client_head->next; client_ptr != NULL; client_ptr = client_ptr->next)
			if (client_ptr->online == 1 && client_ptr->isalive == 1)
			{
				send_format(client_ptr->clientsfd, 0x7f, 0, 4, NULL);
				client_ptr->isalive = 0;
			}
			else if (client_ptr->isalive == 0)
			{
				printf("分配的id:%d掉线了\n", client_ptr->clientsfd);
				client_ptr->online = 0;
			}
		client_down();
		alarm(TIME_BEAT);
		break;
	}
}

int check_id_passwd(char *id, char *passwd)
{
	if (id == NULL || passwd == NULL)
		return -1;
	int flag, i, weizhi;
	printf("账户id:%s\n", id);
	clientsfd_ptr client_ptr;
	for (i = 0, flag = -1; i < database.row_num; i++)
	{
		if (strcmp(id, database.rowdata[i].id) == 0)
		{
			char *encrypt_passwd = new char[32];
			flag = 0;
			strcpy(encrypt_passwd, passwd);
			encrypted(encrypt_passwd);
			weizhi = i;
			if (strcmp(encrypt_passwd, database.rowdata[i].passwd) == 0)
			{
				//检测是否有重复登录的的情况
				flag = 1;
				int jishu = 0;
				for (client_ptr = client_head->next; client_ptr != NULL; client_ptr = client_ptr->next)
					if (strcmp(client_ptr->id, id) == 0 && client_ptr->islogin == 1)
					{
						jishu = 1;
						break;
					}
					else if (strcmp(client_ptr->id, id) == 0)
					{
						strcpy(client_ptr->passwd, encrypt_passwd);
						strcpy(client_ptr->nwin, database.rowdata[i].nwin);
						strcpy(client_ptr->nfail, database.rowdata[i].nfail);
					}
				if (jishu)
				{
					send_format(client_ptr->clientsfd, 0x71, 0x02, 4, NULL);
					printf("重复登录\n");
					strcpy(client_ptr->id, "out");
					strcpy(client_ptr->passwd, "unknown");
					client_ptr->islogin = 0;
					flag = 2;
					break;
				}
			}
			break;
			delete encrypt_passwd;
		}
	}
	return flag;
}

void send_friends(int sockfd)
{
	char *online_text = new char[1024];
	int lens = getfriendslist(sockfd, online_text);
	for (clientsfd_ptr client_ptr = client_head->next; client_ptr != NULL; client_ptr = client_ptr->next)
		if (client_ptr->clientsfd == sockfd)
			client_ptr->islogin = 1;
	send_format(sockfd, 0x73, 0, lens + 4, online_text);
	delete online_text;
}

void send_online(int sockfd)
{
	char *online_text = new char[40];
	int i = 0;
	int weizhi;
	clientsfd_ptr client_ptr;
	for (client_ptr = client_head->next; client_ptr != NULL; client_ptr = client_ptr->next)
		if (client_ptr->clientsfd == sockfd)
		{
			for (i = 0; i < database.row_num; i++)
			{
				if (strcmp(database.rowdata[i].id, client_ptr->id) == 0)
				{
					strcpy(client_ptr->nwin, database.rowdata[i].nwin);
					strcpy(client_ptr->nfail, database.rowdata[i].nfail);
					break;
				}
			}
			break;
		}

	char num[2];
	num[0] = atoi(client_ptr->nwin); num[1] = '\0';
	memcpy(online_text, client_ptr->id, 8);
	padding(online_text, 8);
	memcpy(&online_text[8], num, 2);
	num[0] = atoi(client_ptr->nfail); num[1] = '\0';
	memcpy(&online_text[10], num, 2);

	for (clientsfd_ptr client_ptr = client_head->next; client_ptr != NULL; client_ptr = client_ptr->next)
		if (client_ptr->islogin == 1)
			send_format(client_ptr->clientsfd, 0x71, 3, 16, online_text);
	delete online_text;
}

void login_record(char* id, int model)
{
	char *recordtext = new char[MAXSIZE];
	strcpy(recordtext, "ID:");
	strcat(recordtext, id);
	strcat(recordtext, "登录: ");
	switch (model)
	{
	case -1:
		strcat(recordtext, "失败，原因无此账户");
		break;
	case 0:
		strcat(recordtext, "失败，密码错误");
		break;
	case 1:
		strcat(recordtext, "成功，密码正确");
		break;
	case 2:
		strcat(recordtext, "成功，重复登录");
		break;
	default:
		break;
	}
	strcat(recordtext, "\r\n");
	rizhi(recordtext);
	delete recordtext;
}

int process_login(int sockfd, char *id, char *recvbuf)
{
	struct text_format *head = new text_format;
	head->source_type = recvbuf[0];
	int source = head->source_type >> 4;
	int text_type = head->source_type & 0xf;
	head->response_type = recvbuf[1];
	head->len = recvbuf[3];
	head->len <<= 8;
	head->len += recvbuf[2];
	head->content = new char[head->len - 4];

	memcpy(head->content, &recvbuf[4], 40);
	char passwd[32];
	int flag = 0, i = 0;
	if (head->content == NULL)
		return -1;

	strcpy(id, head->content);
	strcpy(passwd, head->content + 8);

	flag = check_id_passwd(id, passwd);

	if (flag > 0)
	{
		printf("password correct!\n");
		printf("login success!\n");
		padding(id, 8);
		if (flag == 1)
			send_format(sockfd, 0x71, 1, 4, NULL);
		else
			send_format(sockfd, 0x71, 4, 4, NULL);
		send_online(sockfd);
		send_friends(sockfd);
	}
	else
	{
		printf("account or passwd failed\n");
		send_format(sockfd, 0x71, 0, 4, NULL);
	}
	login_record(id, flag);
	delete head->content;
	delete head;
}

void info_record(char* id,char* oldpasswd,char* newpasswd, int model)
{
	char *recordtext = new char[MAXSIZE];
	strcpy(recordtext, "ID:");
	strcat(recordtext, id);
	switch (model)
	{
	case 0:
		strcat(recordtext, "修改密码失败");
		break;
	case 1:
		strcat(recordtext, "修改密码成功	原密码：");
		strcat(recordtext, oldpasswd);
		strcat(recordtext, "新密码：");
		strcat(recordtext, newpasswd);
		break;
	case 2:
		strcat(recordtext, "注册成功，密码：");
		strcat(recordtext, oldpasswd);
		break;
	default:
		break;
	}
	strcat(recordtext, "\r\n");
	rizhi(recordtext);
	delete recordtext;

}

int process_info(int sockfd, char *id, char *recvbuf)
{

	struct text_format *head = new text_format;
	head->source_type = recvbuf[0];
	int source = head->source_type >> 4;
	int text_type = head->source_type & 0xf;
	head->response_type = recvbuf[1];
	head->len = recvbuf[3];
	head->len <<= 8;
	head->len += recvbuf[2];
	head->content = new char[64];

	memcpy(head->content, &recvbuf[4], head->len - 4);
	char passwd[32];

	int flag = -1, i = 0;
	if (head->content == NULL)
		return -1;


	char gnum[3]; char *encrypt_passwd = new char[32]; char* old_passwd = new char[32]; char *new_passwd = new char[32]; char* old_origin = new char[32]; char *new_origin = new char[32];
	switch (int(head->response_type))
	{
	case 0:
		strcpy(id, head->content);
		strcpy(passwd, &(head->content)[8]);
		for (i = 0; i < database.row_num; i++)
			if (strcmp(id, database.rowdata[i].id) == 0)
			{
				printf("id已经存在\n");
				send_format(sockfd, 0x72, 0, 4, NULL);
				return -1;
			}
		strcpy(gnum, "0");
		strcpy(encrypt_passwd, passwd);
		encrypted(encrypt_passwd);
		for (clientsfd_ptr client_ptr = client_head->next; client_ptr != NULL; client_ptr = client_ptr->next)
			if (client_ptr->clientsfd == sockfd)
			{
				strcpy(client_ptr->passwd, encrypt_passwd);
				strcpy(client_ptr->nwin, "00");
				strcpy(client_ptr->nfail, "00");
				break;
			}
		Info_Insert(id, encrypt_passwd, gnum, gnum);
		Database_Load();
		printf("注册成功\n");
		flag = 2;
		info_record(id, passwd, passwd, flag);
		send_format(sockfd, 0x72, 1, 4, NULL);
		break;
	case 1:
		flag = 0;
		memcpy(old_origin, &recvbuf[4], 32);
		memcpy(new_origin, &recvbuf[36], 32);

		memcpy(old_passwd, &recvbuf[4], 32);
		printf("加密前：	%s\n", old_passwd);
		encrypted(old_passwd);
		printf("加密后：	%s\n", old_passwd);
		memcpy(new_passwd, &recvbuf[36], 32);
		for (clientsfd_ptr client_ptr = client_head->next; client_ptr != NULL; client_ptr = client_ptr->next)
			if (client_ptr->clientsfd == sockfd)
			{
				if (strcmp(client_ptr->passwd, old_passwd) == 0)
				{
					encrypted(new_passwd);
					strcpy(client_ptr->passwd, new_passwd);
					Info_Update(id, client_ptr->passwd, 0);
					Database_Load();
					printf("密码修改成功");
					flag = 1;
					send_format(sockfd, 0x72, 3, 4, NULL);
				}
				else
					send_format(sockfd, 0x72, 2, 4, NULL);
				info_record(id,old_origin, new_origin,flag);
				break;

			}
		break;
	default:
		break;
	}
	delete encrypt_passwd;
	delete old_passwd;
	delete new_passwd;
	delete old_origin;
	delete new_origin;
	delete head->content;
	delete head;
	return 1;

}

void StrCoordinate(int x1, int y1, int x2, int y2)
{
	strcpy(strpot, "头：(");
	int len = strlen(strpot);
	strpot[len++] = x1 + '0'; strpot[len++] = ','; strpot[len++] = y1 + '0'; strpot[len++] = ')'; strpot[len] = '\0';
	strcat(strpot, "尾：(");
	len = strlen(strpot);
	strpot[len++] = x2 + '0'; strpot[len++] = ','; strpot[len++] = y2 + '0'; strpot[len++] = ')'; strpot[len] = '\0';
	strcat(strpot, "\r\n");
}

void decode_plane(clientsfd_ptr client)
{
	PlaneConf plane[3];
	char *recordtext = new char[MAXSIZE];
	strcpy(recordtext, "----------------ID:");
	strcat(recordtext, client->id); strcat(recordtext, "的飞机初始头尾----------------------\r\n");
	
	for (int i = 0; i < 3; i++)
	{
		plane[i].x1 = client->plane_head[i] >> 4 & 0xf;
		plane[i].y1 = client->plane_head[i] & 0xf;
		plane[i].x2 = client->plane_tail[i] >> 4 & 0xf;
		plane[i].y2 = client->plane_tail[i] & 0xf;
		StrCoordinate(plane[i].x1, plane[i].y1, plane[i].x2, plane[i].y2);
		strcat(recordtext, strpot);
	}

	rizhi(recordtext);
	delete recordtext;

	for (int i = 0; i < 10; i++)
		for (int j = 0; j < 10; j++)
			(client->pan)[i][j] = 0;

	for (int i = 0; i < 3; i++)
	{
		int hx = plane[i].x1, hy = plane[i].y1;
		int tx = plane[i].x2, ty = plane[i].y2;
		if (hx == tx)
		{
			client->pan[hx][hy] = 3;
			for (int y = hy + 1; y <= ty; y++)
			{
				if (y == hy + 1)
				{
					client->pan[hx - 2][y] = 1;
					client->pan[hx - 1][y] = 1;
					client->pan[hx + 1][y] = 1;
					client->pan[hx + 2][y] = 1;
				}
				else if (y == ty)
				{
					client->pan[hx - 1][y] = 1;
					client->pan[hx + 1][y] = 1;
				}
				client->pan[hx][y] = 1;
			}
			for (int y = hy - 1; y >= ty; y--)
			{
				if (y == hy - 1)
				{
					client->pan[hx - 2][y] = 1;
					client->pan[hx - 1][y] = 1;
					client->pan[hx + 1][y] = 1;
					client->pan[hx + 2][y] = 1;
				}
				else if (y == ty)
				{
					client->pan[hx - 1][y] = 1;
					client->pan[hx + 1][y] = 1;
				}
				client->pan[hx][y] = 1;
			}
		}
		else if (hy == ty)
		{
			client->pan[hx][hy] = 3;
			for (int x = hx + 1; x <= tx; x++)
			{
				if (x == hx + 1)
				{
					client->pan[x][hy - 2] = 1;
					client->pan[x][hy - 1] = 1;
					client->pan[x][hy + 1] = 1;
					client->pan[x][hy + 2] = 1;
				}
				else if (x == tx)
				{
					client->pan[x][hy - 1] = 1;
					client->pan[x][hy + 1] = 1;
				}
				client->pan[x][hy] = 1;
			}
			for (int x = hx - 1; x >= tx; x--)
			{
				if (x == hx - 1)
				{
					client->pan[x][hy - 2] = 1;
					client->pan[x][hy - 1] = 1;
					client->pan[x][hy + 1] = 1;
					client->pan[x][hy + 2] = 1;
				}
				else if (x == tx)
				{
					client->pan[x][hy - 1] = 1;
					client->pan[x][hy + 1] = 1;
				}
				client->pan[x][hy] = 1;
			}
		}
	}
}

int JudgeOrder(clientsfd_ptr player1, clientsfd_ptr player2)
{
	unsigned i = rand() % 2 + 1;
	unsigned j = (i < 2) ? 2 : 1;
	printf("解读飞机1\n");
	decode_plane(player1);
	printf("解读飞机2\n");
	decode_plane(player2);
	printf("解读完毕\n");
	player1->order = i; player2->order = j;
	player1->base = 3; player2->base = 3;
	player1->score = 100; player2->score = 100;
	player1->final = 0; player2->final = 0;
	char content[2];
	content[0] = player1->score; content[1] = player2->score;
	send_format(player1->clientsfd, 0x76, i - 1, 6, content);
	content[0] = player2->score; content[1] = player1->score;
	send_format(player2->clientsfd, 0x76, j - 1, 6, content);
	return 1;
}

void InitGame_record(char* id1, char* id2, int model)
{
	char *recordtext = new char[MAXSIZE];
	strcpy(recordtext, "ID1:");
	strcat(recordtext, id1);
	switch (model)
	{
	case -1:
		break;
	case 0:
		strcat(recordtext, "挑战");
		strcat(recordtext, "ID2:");
		strcat(recordtext, id2);
		break;
	case 1:
		strcat(recordtext, "接受");
		strcat(recordtext, "ID2:");
		strcat(recordtext, id2);
		strcat(recordtext, "挑战");
		break;
	case 2:
		strcat(recordtext, "拒绝");
		strcat(recordtext, "ID2:");
		strcat(recordtext, id2);
		strcat(recordtext, "挑战");
		break;
	default:
		break;
	}
	strcat(recordtext, "\r\n");
	rizhi(recordtext);
	delete recordtext;
}

int process_InitGame(clientsfd_ptr client_source, char *recvbuf)
{
	struct text_format *head = new text_format;
	head->source_type = recvbuf[0];
	int source = head->source_type >> 4;
	int text_type = head->source_type & 0xf;
	head->response_type = recvbuf[1];
	head->len = recvbuf[3];
	head->len <<= 8;
	head->len += recvbuf[2];
	head->content = new char[head->len - 4];

	memcpy(head->content, &recvbuf[4],8);
	char target[8];

	memcpy(target, head->content,8);
	clientsfd_ptr client_ptr;
	client_source->plane_out[0] = 0; client_source->plane_out[1] = 0; client_source->plane_out[2] = 0;
	int model = -1;

	switch (int(head->response_type))
	{
	case 0:
		for (client_ptr = client_head->next; client_ptr != NULL; client_ptr = client_ptr->next)
			if (strcmp(client_ptr->id, target) == 0 && client_ptr->islogin == 1 && client_ptr->isgaming == 0)
			{
				send_format(client_ptr->clientsfd, 0x74, 0, 12, client_source->id);
				model = 0;
				break;
			}
		if (client_ptr == NULL)
			send_format(client_source->clientsfd, 0x74, 2, 12, target);
		break;
	case 1:
		for (client_ptr = client_head->next; client_ptr != NULL; client_ptr = client_ptr->next)
			if (strcmp(client_ptr->id, target) == 0 && client_ptr->islogin == 1 && client_ptr->isgaming == 0)
			{
				padding(client_source->id, 8);
				send_format(client_ptr->clientsfd, 0x74, 1, 12, client_source->id);
				client_ptr->rival = client_source->clientsfd;
				client_source->rival = client_ptr->clientsfd;
				client_ptr->isready = 1;
				client_source->isready = 1;
				model = 1;
				break;
			}
		break;
	case 2:
		for (client_ptr = client_head->next; client_ptr != NULL; client_ptr = client_ptr->next)
			if (strcmp(client_ptr->id, target) == 0 && client_ptr->islogin == 1 && client_ptr->isgaming == 0)
			{
				send_format(client_ptr->clientsfd, 0x74, 2, 12, client_source->id);
				model = 2;
				break;
			}
		break;
	default:
		break;

	}
	InitGame_record(client_source->id, target,model);
	delete head->content;
	delete head;
	return 1;
}

int process_PreGame(clientsfd_ptr client_source, char *recvbuf)
{
	struct text_format *head = new text_format;
	head->source_type = recvbuf[0];
	int source = head->source_type >> 4;
	int text_type = head->source_type & 0xf;
	head->response_type = recvbuf[1];
	head->len = recvbuf[3];
	head->len <<= 8;
	head->len += recvbuf[2];
	head->content = new char[head->len - 4];
	memcpy(head->content, &recvbuf[4], head->len - 4);
	client_source->plane_head[0] = head->content[0]; client_source->plane_head[1] = head->content[2]; client_source->plane_head[2] = head->content[4];
	client_source->plane_tail[0] = head->content[1]; client_source->plane_tail[1] = head->content[3]; client_source->plane_tail[2] = head->content[5];
	client_source->isgaming = 1;

	for (clientsfd_ptr client_ptr = client_head->next; client_ptr != NULL; client_ptr = client_ptr->next)
		if (client_ptr->clientsfd == client_source->rival && client_ptr->isgaming == 1)
		{
			char *recordtext = new char[MAXSIZE];
			strcpy(recordtext, "-----------game start: ");
			strcat(recordtext, client_ptr->id);
			strcat(recordtext, "VS");
			strcat(recordtext, client_source->id);
			strcat(recordtext, "-------------\r\n");
			rizhi(recordtext);
			delete recordtext;
			JudgeOrder(client_ptr, client_source);
			break;
		}
	delete head->content;
	delete head;
	return 1;
}

void singlepot_record(clientsfd_ptr client_source, int x1, int y1)
{
	char *recordtext = new char[MAXSIZE];
	strcpy(recordtext, "ID:");
	strcat(recordtext, client_source->id);
	strcat(recordtext, "选中的是单坐标：(");
	int len = strlen(recordtext);
	recordtext[len++] = x1 + '0'; recordtext[len++] = ','; recordtext[len++] = y1 + '0'; recordtext[len++] = ')'; recordtext[len] = '\0';
	strcat(recordtext, "反馈值:");
	len = strlen(recordtext);
	recordtext[len++] = client_source->pan[x1][y1] + '0'; recordtext[len] = '\0';
	strcat(recordtext, "目前得分: "); change_score(client_source->score); strcat(recordtext, cscore);
	strcat(recordtext, "\r\n");
	rizhi(recordtext);
	delete recordtext;
}

void doublepot_record(clientsfd_ptr client_source, int x1, int y1, int x2, int y2,int flag)
{
	char *recordtext = new char[MAXSIZE];
	strcpy(recordtext, "ID:");
	strcat(recordtext, client_source->id);
	strcat(recordtext, "选中的是双坐标：");
	int len = strlen(recordtext);
	recordtext[len++] = '('; recordtext[len++] = x1 + '0'; recordtext[len++] = ','; recordtext[len++] = y1 + '0'; recordtext[len++] = ')'; recordtext[len] = '\0';
	recordtext[len++] = '('; recordtext[len++] = x2 + '0'; recordtext[len++] = ','; recordtext[len++] = y2 + '0'; recordtext[len++] = ')'; recordtext[len] = '\0';
	strcat(recordtext, "命中飞机:");
	len = strlen(recordtext);
	recordtext[len++] = flag + '0'; recordtext[len] = '\0';
	strcat(recordtext, "目前得分: "); change_score(client_source->score); strcat(recordtext, cscore);
	strcat(recordtext, "\r\n");
	rizhi(recordtext);
	delete recordtext;
}

int process_Locate(clientsfd_ptr client_source, char *recvbuf)
{
	struct text_format *head = new text_format;
	head->source_type = recvbuf[0];
	int source = head->source_type >> 4;
	int text_type = head->source_type & 0xf;
	head->response_type = recvbuf[1];
	head->len = recvbuf[3];
	head->len <<= 8;
	head->len += recvbuf[2];
	head->content = new char[head->len - 4];

	memcpy(head->content, &recvbuf[4], head->len - 4);

	clientsfd_ptr client_target;

	for (client_target = client_head->next; client_target != NULL; client_target = client_target->next)
		if (client_target->clientsfd == client_source->rival)
			break;

	unsigned x1, x2, y1, y2, i;
	int flag = 0, jishu = 0;
	printf("单/双坐标:%d\n", head->len - 4);
	x1 = head->content[0] >> 4 & 0xf;
	y1 = head->content[0] & 0xf;
	x2 = head->content[1] >> 4 & 0xf;
	y2 = head->content[1] & 0xf;
	switch (head->len - 4)
	{
	case 1:
		printf("收到的字符:%c----横坐标:%d------纵坐标:%d-------\n", head->content[0], x1, y1);
		if (client_target->pan[x1][y1] == 0 || client_target->pan[x1][y1] == 1)
		{
			send_format(client_source->clientsfd, 0x77, 0x10 + client_target->pan[x1][y1], 5, head->content);
			send_format(client_target->clientsfd, 0x77, 0x10 + client_target->pan[x1][y1], 5, head->content);
		}
		else if (client_target->pan[x1][y1] == 3)
		{
			send_format(client_source->clientsfd, 0x77, 0x12, 5, head->content);
			send_format(client_target->clientsfd, 0x77, 0x12, 5, head->content);
		}
		client_source->score -= 1;
		client_source->final = 0;
		flag = 0;
		singlepot_record(client_source, x1, y1);
		break;
	case 2:
		char h, t;
		h = head->content[0]; t = head->content[1];
		for (i = 0; i < 3; i++)
			if (client_target->plane_head[i] == h && client_target->plane_tail[i] == t)
			{
				if (client_source->plane_out[i] == 0)
				{
					client_source->plane_out[i] = 1;
					client_target->base--;
					flag = 1;
				}
				break;
			}
			else if (client_target->plane_head[i] == t && client_target->plane_tail[i] == h)
			{
				if (client_source->plane_out[i] == 0)
				{
					client_source->plane_out[i] = 1;
					client_target->base--;
					flag = 2;
				}
				break;
			}
		if (flag == 0)
			client_source->score -= 9;
		if (flag == 2)
		{
			char t = head->content[0];
			head->content[0] = head->content[1];
			head->content[1] = t;
			flag = 1;
		}
		doublepot_record(client_source, x1, y1, x2, y2, flag);
		send_format(client_source->clientsfd, 0x77, 0x20 + flag, 6, head->content);
		send_format(client_target->clientsfd, 0x77, 0x20 + flag, 6, head->content);
		break;
	default:
		break;
	}

	if (flag && client_target->base == 0)
	{
		if (client_source->base == 0)
			if (client_source->score > client_target->score)
				Winner(client_source, client_target, 0);
			else if (client_source->score < client_target->score)
				Winner(client_source, client_target, 0);
			else
				Winner(client_source, client_target, 9999);
		else if (client_source->base > 1 || client_source->order == 2)
			Winner(client_source, client_target, 0);
		else
		{
			char score[2];
			score[0] = client_source->score;
			score[1] = client_target->score;
			send_format(client_source->clientsfd, 0x76, 0x01, 6, score);
			score[1] = client_source->score;
			score[0] = client_target->score;
			send_format(client_target->clientsfd, 0x76, 0x00, 6, score);
			printf("决胜局了\n");
			client_target->final = 1;
		}
	}
	else if (client_source->final)
		Winner(client_target, client_source, 0);
	else
	{
		char score[2];
		score[0] = client_source->score;
		score[1] = client_target->score;
		send_format(client_source->clientsfd, 0x76, 0x01, 6, score);
		score[1] = client_source->score;
		score[0] = client_target->score;
		send_format(client_target->clientsfd, 0x76, 0x00, 6, score);
		printf("正常下一轮\n");
	}
	return 1;

}

int process_OverGame(clientsfd_ptr client_source, char *recvbuf)
{
	clientsfd_ptr client_target;

	for (client_target = client_head->next; client_target != NULL; client_target = client_target->next)
		if (client_target->clientsfd == client_source->rival)
			break;

	Winner(client_target, client_source, 1);
	return 1;
}

int process_beat(int sockfd, char *recvbuf)
{
	for (clientsfd_ptr client_ptr = client_head->next; client_ptr != NULL; client_ptr = client_ptr->next)
		if (client_ptr->clientsfd == sockfd)
			client_ptr->isalive = 1;
}

void Deal_Message(int sockfd, clientsfd_ptr client_ptr, char *recvbuf, int recvsum)
{
	struct text_format *head = new text_format;
	init_text_format(head, recvbuf);
	int source = head->source_type >> 4;
	int text_type = head->source_type & 0xf;
	int i;
	printf("content的长度:%d\n", recvsum);
	for (i = 0; i < recvsum; i++)
	{
		printf("%02x ", recvbuf[i]);
		if (i % 16 == 15)
			printf("\n");
	}

	if (source == 1)
	{
		switch (text_type)
		{
		case 1:
			process_login(sockfd, client_ptr->id, recvbuf);
			delete head;
			break;
		case 2:
			process_info(sockfd, client_ptr->id, recvbuf);
			delete head;
			break;
		case 4:
			process_InitGame(client_ptr, recvbuf);
			delete head;
			break;
		case 5:
			process_PreGame(client_ptr, recvbuf);
			delete head;
			break;
		case 7:
			process_Locate(client_ptr, recvbuf);
			delete head;
			break;
		case 8:
			process_OverGame(client_ptr, recvbuf);
			delete head;
			break;
		case 9:
			if (client_ptr->isgaming && client_ptr->islogin)
				send_format(client_ptr->rival, 0x79, 0x00, 4, NULL);
			delete head;
			break;
		case 15:
			process_beat(sockfd, recvbuf);
			delete head;
			break;
		}
	}
	else
	{
		perror("not valid message!\n");
		printf("%s\n", recvbuf);
		if (head)
			delete head;
	}
}

void judge_packet(int sockfd, clientsfd_ptr client_ptr, char *recvbuf, int recvsum)
{
	memcpy(file_buffer + buf_size, recvbuf, recvsum);
	buf_size += recvsum;

	int i = 0;
	printf("刚开始：");
	for (i = 0; i < buf_size; i++)
	{
		printf("%02x ", file_buffer[i]);
		if (i % 16 == 15)
			printf("\n");
	}
	printf("\n");
	struct text_format *head = new text_format;
	for (i = 0; 1; 1)
	{
		init_text_format(head, file_buffer + i);
		printf("head->len:%d\n", head->len);
		if (buf_size - i < head->len)
			break;
		memcpy(recvbuf, file_buffer + i, head->len);
		Deal_Message(sockfd, client_ptr, recvbuf, head->len);
		i += head->len;
		if (buf_size == i)
			break;
	}
	if (buf_size != i)
	{
		char *temp = new char[MAXSIZE * 10];
		memcpy(temp, file_buffer + i, buf_size - i);
		memcpy(file_buffer, temp, buf_size - i);
		delete temp;
	}
	memset(file_buffer + buf_size - i, i, 0);
	buf_size -= i;
	for (i = 0; i < buf_size; i++)
	{
		printf("%02x ", file_buffer[i]);
		if (i % 16 == 15)
			printf("\n");
	}
	printf("\n剩余字节数为%d\n", buf_size);
	delete head;
}

/*********************************************************/
/*
	保证server，client相连后，server端要不断地发送心跳包，如果一段时间内没有回应
	则server视client为断开连接
*/
/*********************************************************/

int Message_transport(clientsfd_ptr client_ptr)
{
	int recvsum;
	int sendsum;
	char *recvbuf = new char[2 * MAXSIZE];
	int flag = 1;
	char passwd[20];
	int sockfd = client_ptr->clientsfd;

	if ((recvsum = Recvstring(sockfd, recvbuf)) > 0)
		judge_packet(sockfd, client_ptr, recvbuf, recvsum);
	else
	{
		delete recvbuf;
		return -1;
	}
	delete recvbuf;
	return 0;
}

int ServerInit(int *server, char *ip, int port)
{
	int server_sfd;
	int client_sfd;

	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	int sin_size;

	setvbuf(stdout, NULL, _IONBF, 0);

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);

	if (strcmp(ip, "0.0.0.0") == 0)
		server_addr.sin_addr.s_addr = INADDR_ANY;
	else if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0)
	{
		printf("Bad IP address!\n");
		exit(-1);
	}

	if ((server_sfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	{
		perror("socket error\n");
		exit(-1);
	}

	int reuse = 1;
	if (setsockopt(server_sfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuse, sizeof(int)) < 0)
	{
		perror("setsocket error\n");
		exit(-1);
	}

	int flag_s = fcntl(server_sfd, F_GETFL, 0);		 //获取文件的flags值
	fcntl(server_sfd, F_SETFL, flag_s | O_NONBLOCK); //设置成非阻塞模式；

	if (bind(server_sfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0)
	{
		perror("bind error\n");
		exit(-1);
	}

	if (listen(server_sfd, MAXCONN) < 0)
	{
		perror("listen error\n");
		exit(-1);
	};

	database.rowdata = new ROW_INFO[10];
	Database_Load();
	*server = server_sfd;

	return 1;
}

void Server_Main(int server_sfd)
{
	struct sockaddr_in client_addr;
	fd_set fds;
	int data_come_num;
	int  i;
	int sin_size = sizeof(struct sockaddr_in);
	file_buffer[0] = '\0';
	buf_size = 0;
	int jishu = 0;
	struct timeval tt;
	tt.tv_sec = 0;
	tt.tv_usec = 0;
	client_head->next = NULL;
	printf("wait for connect!\n");
	srand(unsigned(time(NULL)));
	signal(SIGALRM, signalHandler);
	alarm(3);

	while (1)
	{
		FD_ZERO(&fds);
		FD_SET(server_sfd, &fds);
		int maxdp = server_sfd;
		shun = 0;
		for (clientsfd_ptr client_ptr = client_head->next; client_ptr != NULL; client_ptr = client_ptr->next)
		{
			FD_SET(client_ptr->clientsfd, &fds);
			if (client_ptr->clientsfd > maxdp)
				maxdp = client_ptr->clientsfd;
		}
		switch (data_come_num = select(maxdp + 1, &fds, NULL, NULL, &tt))
		{
		case -1:
			if (errno != EINTR)
				exit(-1);
			break;
		case 0:
			break;
		default:
			if (FD_ISSET(server_sfd, &fds))
			{
				clientsfd_ptr client_ptr = new clientsfd_id;
				if ((client_ptr->clientsfd = accept4(server_sfd, (struct sockaddr *)&client_addr, (socklen_t *)&sin_size, SOCK_NONBLOCK)) < 0)
				{
					if (errno == EINTR)
					{
						printf("EINTR\n");
						break;
					}
					else
					{
						perror("accept error\n");
						exit(-1);
					}
				}
				printf("sockfd:%d 连接!\n", client_ptr->clientsfd);
				num_client++;
				client_ptr->next = client_head->next;
				client_head->next = client_ptr;
				client_ptr->isalive = 1;
				client_ptr->online = 1;
				client_ptr->islogin = 0;
				client_ptr->isgaming = 0;
			}
			for (clientsfd_ptr client_temp = client_head->next; client_temp != NULL; client_temp = client_temp->next)
			{
				if (FD_ISSET(client_temp->clientsfd, &fds))
					if (Message_transport(client_temp) == 0)
						printf("正确处理完成---id:%s\n", client_temp->id);
			}
		}
	}
}

void do_erro_para()
{
	printf("check your parameters!\n");
	printf("       --port x            (no default)\n");
	exit(-1);
}

void CheckParameters(int ac, char *av[], int *port)
{
	int i;
	int p;
	int n;

	int cnt_port = 0;

	for (i = 1; i < ac; i++)
	{
		if (strcmp(av[i], "--port") == 0)
		{
			p = atoi(av[i + 1]);
			if (p > 65535 || p < 1)
				do_erro_para();
			*port = p;
			i++;
			cnt_port++;
			continue;
		}
		else
			do_erro_para();
	}
	if (cnt_port != 1)
		do_erro_para();
}

int main(int argc, char *argv[])
{
	int server_sfd; //server socket file descriptor
	int client_sfd; //client socket file descriptor
	int port = 0;
	int flag, i;
	char allip[10] = "0.0.0.0";
	getcwd(workpath, 80);
	int len = strlen(workpath);
	workpath[len] = '/';
	workpath[len + 1] = '\0';
	printf("当前server路径：%s\n", workpath);

	if (-1 == daemon(0, 1))
	{
		printf("daemon error\n");
		exit(-1);
	}

	setvbuf(stdout, NULL, _IONBF, 0);

	CheckParameters(argc, argv, &port);

	ServerInit(&server_sfd, allip, port);

	Server_Main(server_sfd);

	delete database.rowdata;
	return 0;
}
