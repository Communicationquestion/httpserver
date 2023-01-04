
#include "httpfunction.h"
#include<mutex>
#include<sys/stat.h>
#define SERVER_STRING "Cache-Control: max-age=31536000, immutable\r\n"

std::mutex m;
int get_line(int sock, char* buf, int size) {
	int i = 0;
	char c = '\0';
	int n;

	while ((i < size - 1) && (c != '\n')) {
		//recv()������<sys/socket.h>,�ζ���TLPI��P1259,
		//��һ���ֽڵ����ݴ���� c ��
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


int catbinary(int client, int fd) {
	
	struct stat stat_buf;
	fstat(fd, &stat_buf);
	sendfile(client, fd, NULL, stat_buf.st_size);

}

void cat(int client, FILE* resource) {
	char buf[1024];

	//���ļ��ļ��������ж�ȡָ������
	fgets(buf, sizeof(buf), resource);
	while (!feof(resource)) {
		send(client, buf, strlen(buf), 0);
		fgets(buf, sizeof(buf), resource);
	}
}

void headers(int client, const char* filename) {
	char buf[1024];
	char sg[1024] = {};
	char* sc = new char[strlen(filename) + 1];
	strcpy(sc, filename);
	auto i = 0;
	for (int j = 0; j < strlen(filename) + 1; ++j)
	{
		if (*sc == '.') {
			sg[1024] = {};
			i = 0;
		}
		sg[i] = *sc;
		++i;
		++sc;
	}
	std::cout << sg << std::endl;
	std::vector<std::string> s1 = { ".js",".avi",".css",".html",".wasm",".ico",".jpg",".png",".mp3"};
	std::vector<std::string> s2 = { "Content-Type: text/javascript; charset=utf-8\r\n","Content-Type: video/x-msvideo\r\n","Content-Type: text/css\r\n","Content-Type: text/html; charset=utf-8\r\n","Content-Type: application/wasm\r\n","Content-Type: image/x-icon\r\n","Content-Type: application/x-jpg\r\n" ,"Content-Type: image/png\r\n","Content-Type: audio/mpeg\r\n"};
	url<std::string, std::string> str(s1, s2);

	auto res = str.findtype(sg, s1);
	const char* ress;

	//(void) filename;  /* could use filename to determine file type */
	std::cout << "res " << res << std::endl;
	strcpy(buf, "HTTP/1.0 200 OK\r\n");
	send(client, buf, strlen(buf), 0);
	strcpy(buf, SERVER_STRING);
	send(client, buf, strlen(buf), 0);
	sprintf(buf, res.c_str());
	send(client, buf, strlen(buf), 0);
	strcpy(buf, "X-Content-Type-Options: nosniff\r\n");
	send(client, buf, strlen(buf), 0);

	strcpy(buf, "\r\n");
	send(client, buf, strlen(buf), 0);
}
void serve_file(int client, const char* filename) {

	FILE* resource = NULL;
	int numchars = 1;
	char buf[1024];

	//ȷ�� buf �����ж������ܽ�������� while ѭ��
	buf[0] = 'A';
	buf[1] = '\0';
	//ѭ�������Ƕ�ȡ�����Ե���� http ����������������
	while ((numchars > 0) && strcmp("\n", buf))  /* read & discard headers */
		numchars = get_line(client, buf, sizeof(buf));

	//����������������·����ָ���ļ�
	char sg[1024] = {};
	char* sc = new char[strlen(filename) + 1];
	strcpy(sc, filename);
	auto i = 0;
	for (int j = 0; j < strlen(filename) + 1; ++j)
	{
		if (*sc == '.') {
			sg[1024] = {};
			i = 0;
		}
		sg[i] = *sc;
		++i;
		++sc;
	}
	

	if (strcmp(sg, ".mp3") == 0 || strcmp(sg, ".wasm") == 0 || strcmp(sg, ".jpg") == 0|| strcmp(sg, ".png") == 0 || strcmp(sg, ".ico") == 0) {

		int fd = open(filename, O_RDONLY);
		if (fd== -1)
			//not_found(client);
			printf("not found\n");
		else {
			//�򿪳ɹ��󣬽�����ļ��Ļ�����Ϣ��װ�� response ��ͷ��(header)������
			headers(client, filename);
			//���Ű�����ļ������ݶ�������Ϊ response �� body ���͵��ͻ���

			catbinary(client, fd);
		}
		close(fd);
	}
	else
	{
		resource = fopen(filename, "r");
		if (resource == NULL)
			//not_found(client);
			printf("not found\n");
		else {
			//�򿪳ɹ��󣬽�����ļ��Ļ�����Ϣ��װ�� response ��ͷ��(header)������
			headers(client, filename);
			//���Ű�����ļ������ݶ�������Ϊ response �� body ���͵��ͻ���
			cat(client, resource);
		}
		fclose(resource);
	}




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
	//�õ�buffer

	numchars = get_line(client, buffer, sizeof(buffer));
	i = 0;
	j = 0;
	// �õ����󷽷� get �� post �ȡ�

	printf("%s", buffer);
	while (!ISspace(buffer[j]) && (i < sizeof(method) - 1)) {
		method[i] = buffer[j];
		//printf("%s",buffer[j]);
		i++;
		j++;

	}


	method[i] = '\0';
	std::cout << method << " method" << std::endl;
	//�ж����󷽷� �Ƿ�Ϊget��post 
	if (strcasecmp(method, "GET") && strcasecmp(method, "POST")) {
		printf("erro method\n");
		return false;
	}
	//�����post����
	if (strcasecmp(method, "POST") == 0) {
		cgi = 1;
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
	//�����get����
	if (strcasecmp(method, "GET") == 0) {
		query_string = url;

		//ȥ������� url�������ַ� ��ǰ��������ַ�������������Ҳû�ҵ��ַ� �����˳�ѭ��
		while ((*query_string != '?') && (*query_string != '\0'))
			query_string++;

		//�˳�ѭ�����鵱ǰ���ַ��� �������ַ���(url)�Ľ�β
		if (*query_string == '?') {
			//����� �� �Ļ���֤�����������Ҫ���� cgi���� cgi ��־������һ(true)
			cgi = 1;
			//���ַ� �� �����ַ��� url ���ָ�������
			*query_string = '\0';
			//ʹָ��ָ���ַ� ��������Ǹ��ַ�
			query_string++;
		}
	}



	sprintf(path, "htdocs%s", url);

	//��� path �����е�����ַ��������һ���ַ������ַ� / ��β�Ļ�����ƴ����һ��"index.html"���ַ�������ҳ����˼
	if (path[strlen(path) - 1] == '/')
		strcat(path, "index.html");

	//��ϵͳ��ȥ��ѯ���ļ��Ƿ����
	if (stat(path, &st) == -1) {
		//��������ڣ��ǰ���� http ���������������(head �� body)ȫ�����겢����
		while ((numchars > 0) && strcmp("\n", buffer)) { /* read & discard headers */
			//numchars = get_line(client, buffer, sizeof(buf));
		//Ȼ�󷵻�һ���Ҳ����ļ��� response ���ͻ���
			printf("not find file\n");
			std::cout<<path<<std::endl;
			return false;
			//not_found(client);
		}
	}
	else {
		//�ļ����ڣ���ȥ������S_IFMT���룬����֮���ֵ���������жϸ��ļ���ʲô���͵�
		//S_IFMT�ζ���TLPI��P281�����������������һ���ǰ�����<sys/stat.h>
		if ((st.st_mode & S_IFMT) == S_IFDIR)
			//�������ļ��Ǹ�Ŀ¼���Ǿ���Ҫ���� path ����ƴ��һ��"/index.html"���ַ���
			strcat(path, "/index.html");

		//S_IXUSR, S_IXGRP, S_IXOTH���߿��Բζ���TLPI��P295
		if ((st.st_mode & S_IXUSR) ||
			(st.st_mode & S_IXGRP) ||
			(st.st_mode & S_IXOTH))
			//�������ļ���һ����ִ���ļ��������������û�/��/�������������͵ģ��ͽ� cgi ��־������һ
			cgi = 1;

		if (!cgi) {
			//�������Ҫ cgi ���ƵĻ���
			std::unique_lock<std::mutex> ulock(m);
			printf("serve_file %s\n", path);
			serve_file(client, path);

			ulock.unlock();
		}
		else {
			//�����Ҫ�����
			//execute_cgi(client, path, method, query_string);
			printf("need cgi\n");

		}
	}

	close(client);

	return false;
}


