#include "../include/KcpConnectionManager.h"

int main(void)
{
	KcpConnectionManager connection_manager;

	connection_manager.listenWithKqueue(40960);

	while(1)
	{
		connection_manager.tickWithKqueue();
	}

	return 0;
}