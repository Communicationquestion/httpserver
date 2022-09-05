#include <string.h>
#include "httpfunction.h"
#include<mutex>
#include <sys/socket.h>
#include <sys/stat.h>
#include <iostream>
#define SERVER_STRING "Server: jdbhttpd/0.1.0\r\n"
std::mutex m;
int get_line(int sock, char* buf, int size) {
	int i = 0;
	char c = '\0';
	int n;

	while ((i < size - 1) && (c != '\n')) {
		//recv()包含于<sys/socket.h>,参读《TLPI》P1259,
		//读一个字节的数据存放在 c 中
		n = recv(sock, &c, 1, 0);
		/* DEBUG printf("%02X\n", c); */
		if (n > 0) {
			if (c == '\r') {
				//
				n = recv(sock, &c, 1, MSG_PEEK);
				/* DEBUG printf("%02X\n", c); */
				if ((n > 0) && (c == '\n'))
					recv(sock, &c, 1, 0);
				else
					c = '\n';
			}
			buf[i] = c;
			i++;
		}
		else
			c = '\n';
	}
	buf[i] = '\0';

	return (i);
}


void cat(int client, FILE* resource) {
	char buf[1024];

	//从文件文件描述符中读取指定内容
	fgets(buf, sizeof(buf), resource);
	while (!feof(resource)) {
		send(client, buf, strlen(buf), 0);
		fgets(buf, sizeof(buf), resource);
	}
}

void headers(int client, const char* filename) {
	char buf[1024];
	(void)filename;  /* could use filename to determine file type */
	
	strcpy(buf, "HTTP/1.0 200 OK\r\n");
	send(client, buf, strlen(buf), 0);
	strcpy(buf, SERVER_STRING);
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "Content-Type: text/html\r\n");
	send(client, buf, strlen(buf), 0);
	strcpy(buf, "\r\n");
	send(client, buf, strlen(buf), 0);
	}
void serve_file(int client, const char* filename) {
	
	FILE* resource = NULL;
	int numchars = 1;
	char buf[1024];

	//确保 buf 里面有东西，能进入下面的 while 循环
	buf[0] = 'A';
	buf[1] = '\0';
	//循环作用是读取并忽略掉这个 http 请求后面的所有内容
	while ((numchars > 0) && strcmp("\n", buf))  /* read & discard headers */
		numchars = get_line(client, buf, sizeof(buf));

	//打开这个传进来的这个路径所指的文件
	resource = fopen(filename, "r");
	if (resource == NULL)
		//not_found(client);
		printf("not found");
	else {
		//打开成功后，将这个文件的基本信息封装成 response 的头部(header)并返回
		headers(client, filename);
		//接着把这个文件的内容读出来作为 response 的 body 发送到客户端
		cat(client, resource);
	}
	printf("over read file\n");
	fclose(resource);
	

}

int sendbuffer(int fd, char* buffer) {
	
	
	std::cout << "write qian" << std::endl;
	write(fd, buffer, strlen(buffer));
	
	return 1;
}


bool accept_request(int client) {
	printf("com in accept\n");
	
	char buffer[1024];
	char method[255];
	size_t i, j;
	char url[255];
	int cgi = 0;
	int numchars;
	char* query_string = NULL;
	char path[512];
	struct stat st;
	//拿到buffer

	numchars = get_line(client, buffer, sizeof(buffer));
	i = 0;
	j = 0;
	// 拿到请求方法 get 或 post 等。
	while (!ISspace(buffer[j]) && (i < sizeof(method) - 1)) {
		method[i] = buffer[j];
		//printf("%s",buffer[j]);
		i++;
		j++;

	}


	method[i] = '\0';
	std::cout << method<<" method"<< std::endl;
	//判断请求方法 是否为get或post 
	if (strcasecmp(method, "GET") && strcasecmp(method, "POST")) {
		printf("erro method\n");
		return false;
	}
	//如果是post方法
	if (strcasecmp(method, "POST") == 0) {
		printf("post\n");
	}


	i = 0;
	while (ISspace(buffer[j]) && (j < sizeof(buffer)))
		j++;
	while (!ISspace(buffer[j]) && (i < sizeof(url) - 1) && (j < sizeof(buffer))) {
		url[i] = buffer[j];
		i++;
		j++;
	}
	url[i] = '\0';
	//如果是get请求
	if (strcasecmp(method, "GET") == 0) {
		printf("get\n");
		query_string = url;

		//去遍历这个 url，跳过字符 ？前面的所有字符，如果遍历完毕也没找到字符 ？则退出循环
		while ((*query_string != '?') && (*query_string != '\0'))
			query_string++;

		//退出循环后检查当前的字符是 ？还是字符串(url)的结尾
		if (*query_string == '?') {
			//如果是 ？ 的话，证明这个请求需要调用 cgi，将 cgi 标志变量置一(true)
			cgi = 1;
			//从字符 ？ 处把字符串 url 给分隔会两份
			*query_string = '\0';
			//使指针指向字符 ？后面的那个字符
			query_string++;
		}
	}



	sprintf(path, "htdocs%s", url);

	//如果 path 数组中的这个字符串的最后一个字符是以字符 / 结尾的话，就拼接上一个"index.html"的字符串。首页的意思
	if (path[strlen(path) - 1] == '/')
		strcat(path, "index.html");

	//在系统上去查询该文件是否存在
	if (stat(path, &st) == -1) {
		//如果不存在，那把这次 http 的请求后续的内容(head 和 body)全部读完并忽略
		while ((numchars > 0) && strcmp("\n", buffer)) { /* read & discard headers */
			//numchars = get_line(client, buffer, sizeof(buf));
		//然后返回一个找不到文件的 response 给客户端
			printf("not find file");
			return false;
			//not_found(client);
		}
	}
	else {
		//文件存在，那去跟常量S_IFMT相与，相与之后的值可以用来判断该文件是什么类型的
		//S_IFMT参读《TLPI》P281，与下面的三个常量一样是包含在<sys/stat.h>
		if ((st.st_mode & S_IFMT) == S_IFDIR)
			//如果这个文件是个目录，那就需要再在 path 后面拼接一个"/index.html"的字符串
			strcat(path, "/index.html");

		//S_IXUSR, S_IXGRP, S_IXOTH三者可以参读《TLPI》P295
		if ((st.st_mode & S_IXUSR) ||
			(st.st_mode & S_IXGRP) ||
			(st.st_mode & S_IXOTH))
			//如果这个文件是一个可执行文件，不论是属于用户/组/其他这三者类型的，就将 cgi 标志变量置一
			cgi = 1;

		if (!cgi) {
			//如果不需要 cgi 机制的话，
			std::unique_lock<std::mutex> ulock(m);
			serve_file(client, path);
			ulock.unlock();
		
		}
		else{
			//如果需要则调用
			//execute_cgi(client, path, method, query_string);
			printf("cgi");
		}
	}

	printf("client(eventfd=%d) disconnected.\n", client);
	close(client);
	
	return false;
}



