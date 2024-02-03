#pragma
#include<string>
#include<map>
#include<vector>
#include <unistd.h>
#include <cstddef>
#include <sys/select.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <iostream>
#include<fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#define ISspace(x) isspace((int)(x))

int sendbuffer(int fd, char* buf);
bool accept_request(int client);
int get_line(int sock, char* buf, int size);


template<class T, class N>
class url {
public:
	url(auto& x, auto& y) {
		for (int i = 0; i < x.size(); i++)
		{
			m.insert(std::pair<T, N>(x[i], y[i]));
		}
	}
	auto findtype(auto& filename, auto& x) {
		char sc[1024] = {};
		auto i = 0;
		for (auto& c : filename)
		{
			if (c == '.') {
				sc[1024] = {};
				i = 0;
			}
			sc[i] = c;
			++i;
		}
		try
		{
			for (auto& s : x)
			{
				if (sc == s) {
					return m.find(sc)->second;
				}

			}

			throw std::out_of_range("erro");

		}

		catch (const std::exception& e)

		{
			std::cout << e.what() << std::endl;
		}

	}

private:
	std::map<T, N> m;
};


