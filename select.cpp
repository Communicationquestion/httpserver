#include"select.h"
#include"httpfunction.h"
#include"ThreadPool.h"


int initserver(int port) {
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		printf("socket() failed.\n");
		return -1;
	}
	int opt = 1;
	unsigned int len = sizeof(opt);
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, len);
	struct sockaddr_in servaddr;
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(port);
	if (bind(sock, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
		printf("bind() failed.\n");
		close(sock);
		return -1;
	}
	if (listen(sock, 5) != 0) {
		printf("listen() failed.\n");
		close(sock);
		return -1;
	}
	return sock;
}

int listenfun(int sockserver) {
	task t[100];
	
	
	threadpool pool(1024);
	pool.threadstart();
	std::mutex m;
	int listensock = sockserver;
	printf("listensock=%d\n", listensock);
	if (listensock < 0) {

		printf("initserver() failed.\n");
		return -1;
	}
	fd_set readfdset;
	int maxfd;
	FD_ZERO(&readfdset);
	FD_SET(listensock, &readfdset);
	maxfd = listensock;
	
		while (1) {
			fd_set tmpfdset = readfdset;
			int infds = select(maxfd + 1, &tmpfdset, NULL, NULL, NULL);
			printf("select infds=%d\n", infds);
			if (infds < 0) {
				printf("select() failed.\n");
				perror("select()");
				break;
			}
			if (infds == 0) {
				printf("select() timeout.\n");
				continue;
			}
			for (int eventfd = 0; eventfd <= maxfd; eventfd++) {
				if (FD_ISSET(eventfd, &tmpfdset) <= 0)
					continue;

				if (eventfd == listensock) {
					struct sockaddr_in client;
					socklen_t len = sizeof(client);
					int clientsock = accept(listensock, (struct sockaddr*)&client, &len);
					if (clientsock < 0) {
						printf("accept() failed.\n");
						continue;
					}
					printf("client(socket=%d) connected ok.\n", clientsock);

					FD_SET(clientsock, &readfdset);
					if (maxfd < clientsock)
						maxfd = clientsock;
					continue;
				}
				else {
					//std::cout << "esle" << std::endl;
					char buffer[1024];
					memset(buffer, 0, sizeof(buffer));	
					//printf("%s", buffer);

					t[eventfd].geta(eventfd);
					//int* ma = &maxfd;
					//t[i].getnum(ma);
					//t[i].getsd(readfdset);
					t[eventfd].getaccpet(accept_request);
					//t[i].getb(buffer);

					pool.push_rewu_tasks(&t[eventfd]);

					if (t[eventfd].cnt==false) {
						//close(t[eventfd].a);
						printf("client(eventfd=%d) disconnected.\n", t[eventfd].a);
						
						t[eventfd].cnt = true;
						FD_CLR(t[eventfd].a, &readfdset);
						if (t[eventfd].a == maxfd) {
							for (int ii = maxfd; ii > 0; ii--) {
								if (FD_ISSET(ii, &readfdset)) {
									maxfd = ii;
									break;
								}
							}
							printf("maxfd=%d\n", maxfd);
						}
						continue;
					}

					

					printf("maxfd=%d\n", maxfd);
					
				}
				
				
			}
			
			
				
			
			
		}
	
	return 0;

}