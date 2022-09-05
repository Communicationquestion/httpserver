// CMake_linux_http_server_formal.cpp: 定义应用程序的入口点。
//

#include "CMake_linux_http_server_formal.h"
#include <stdio.h>
#include "ThreadPool.h"

int main()
{
	

	listenfun(initserver(8888));

	getchar();

	
	return 0;
}
