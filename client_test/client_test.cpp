#include "../include/KcpConnection.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
	KcpConnection c;

	c.connect("127.0.0.1", 40960);

	char sendbuf[1024] = {0};

	while(fgets(sendbuf, sizeof(sendbuf), stdin) != NULL)
	{
		c.send(sendbuf, strlen(sendbuf));
	}

	return 0;
}