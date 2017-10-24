#include "../include/KcpConnectionManager.h"

int main(void)
{
	KcpConnectionManager connection_manager;

	connection_manager.listenWithLibevent(40960);

	while(1)
	{
		connection_manager.eventMainLoop();
	}

	return 0;
}