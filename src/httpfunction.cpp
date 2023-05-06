#include "httpfunction.h"
#include <cstddef>
#include <cstdio>
#include <iostream>
#include<mutex>
#include <string>
#include<sys/stat.h>
#include <sys/types.h>
#include <type_traits>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#define SERVER_STRING "Cache-Control: max-age=31536000, immutable\r\n"

std::mutex m;
int get_line(int sock, char* buf, int size) {
	int i = 0;
	char c = '\0';
	int n;

	while ((i < size - 1) && (c != '\n')) {
		n = recv(sock, &c, 1, 0);
		if (n > 0) {
			if (c == '\r') {
				n = recv(sock, &c, 1, MSG_PEEK);
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

	//struct stat stat_buf {};

	//fstat(fd, &stat_buf);

	//sendfile(client, fd, NULL, stat_buf.st_size);
	std::cout << "enter sendfile" << std::endl;
	struct stat stat = {};
	int ret = fstat(fd, &stat);
	if (ret == -1 || stat.st_size < 0)
	{
		// print error message
		return 0;
	}

	ret = sendfile(client, fd, NULL, stat.st_size);
	if (ret == -1)
	{
		// print error message
		return 0;
	}

	std::cout << "enter sendfile over" << std::endl;
	return 0;


}

void cat(int client, FILE* resource) {
	char buf[2048];

	fgets(buf, sizeof(buf), resource);
	while (!feof(resource)) {
		send(client, buf, strlen(buf), MSG_NOSIGNAL);
		fgets(buf, sizeof(buf), resource);
	}
}

void headers(int client, const char* filename) {
	char buf[2048];
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
	std::vector<std::string> s1 = { ".js",".avi",".css",".html",".wasm",".ico",".jpg",".png",".mp3",".txt" };
	std::vector<std::string> s2 = { "Content-Type: text/javascript; charset=utf-8\r\n","Content-Type: video/x-msvideo\r\n","Content-Type: text/css\r\n","Content-Type: text/html; charset=utf-8\r\n","Content-Type: application/wasm\r\n","Content-Type: image/x-icon\r\n","Content-Type: application/x-jpg\r\n" ,"Content-Type: image/png\r\n","Content-Type: audio/mpeg\r\n","Content-Type: text/plain\r\n" };
	url<std::string, std::string> str(s1, s2);

	auto res = str.findtype(sg, s1);
	const char* ress = nullptr;

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

	buf[0] = 'A';
	buf[1] = '\0';
	while ((numchars > 0) && strcmp("\n", buf))  /* read & discard headers */
		numchars = get_line(client, buf, sizeof(buf));

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


	if (strcmp(sg, ".mp3") == 0 || strcmp(sg, ".wasm") == 0 || strcmp(sg, ".jpg") == 0 || strcmp(sg, ".png") == 0 || strcmp(sg, ".ico") == 0) {

		int fd = open(filename, O_RDONLY);
		if (fd == -1)
			//not_found(client);

			printf("not found file\n");
		else {
			headers(client, filename);

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
			headers(client, filename);
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

void execute_cgi(int client, char* path, char* method, char* query_string) {
	printf("ciging\n");
	std::cout << path << std::endl;
	std::cout << method << std::endl;
	std::string strpath = path;
	std::string strmethod = method;


	int fds[2];

	if (pipe(fds) == -1) {
		perror("make pipe");
		exit(1);
	}

	pid_t pid = fork();
	if (pid < 0) {
		perror("fork error");
		exit(1);
	}
	if (pid == 0) {
		char buf[1024] = {};
		read(fds[0], buf, 1024);
		if (strmethod == "GET") {
			std::string strquery_string = query_string;
			execl(buf, strquery_string.c_str(), NULL);
		}
		else if (strmethod == "POST") {
			std::cout << "cgi pars" << std::endl;
		}

	}
	else {
		strpath.erase(strpath.size() - 4, 4);
		write(fds[1], strpath.c_str(), 1024);

	}

}

bool accept_request(int client) {
	printf("com in accept\n");

	char buffer[1024];
	char method[255];
	size_t i, j;
	char url[255];
	int cgi = 0;
	int numchars;
	char* query_string = {};


	char path[512];
	struct stat st;

	numchars = get_line(client, buffer, sizeof(buffer));
	i = 0;
	j = 0;

	printf("%s", buffer);
	while (!ISspace(buffer[j]) && (i < sizeof(method) - 1)) {
		method[i] = buffer[j];
		//printf("%s",buffer[j]);
		i++;
		j++;

	}


	method[i] = '\0';
	std::cout << method << " method" << std::endl;
	if (strcasecmp(method, "GET") && strcasecmp(method, "POST")) {
		printf("erro method\n");

		return false;

	}
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
	if (strcasecmp(method, "GET") == 0) {
		query_string = url;

		while ((*query_string != '?') && (*query_string != '\0'))
			query_string++;

		if (*query_string == '?') {
			cgi = 1;
			*query_string = '\0';
			query_string++;
		}
	}



	sprintf(path, "htdocs%s", url);

	if (path[strlen(path) - 1] == '/')
		strcat(path, "index.html");

	if (stat(path, &st) == -1) {
		while ((numchars > 0) && strcmp("\n", buffer)) { /* read & discard headers */
			//numchars = get_line(client, buffer, sizeof(buf));
			printf("not find file\n");
			std::cout << path << std::endl;
			close(client);

			return false;
			//not_found(client);
		}
	}
	else {
		if ((st.st_mode & S_IFMT) == S_IFDIR)
			strcat(path, "/index.html");

		if ((st.st_mode & S_IXUSR) ||
			(st.st_mode & S_IXGRP) ||
			(st.st_mode & S_IXOTH))
			cgi = 1;



		if (!cgi) {
			printf("serve_file %s\n", path);
			serve_file(client, path);
		}
		else {
			std::cout << "befor cgi" << std::endl;
			//execute_cgi(client, path, method, query_string);
		}


	}



	return false;
}



