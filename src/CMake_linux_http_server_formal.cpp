
#include "CMake_linux_http_server_formal.h"
#include <stdio.h>
#include "ThreadPool.h"
#include <signal.h>

int main()
{

	listenfun(initserver(8888));

	return 0;
}
