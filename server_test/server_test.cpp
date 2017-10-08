#include "../include/KcpConnectionManager.h"

int main(void)
{
	KcpConnectionManager connection_manager;

	connection_manager.listen(40960);

	while(1)
	{
		connection_manager.tick();
	}

	return 0;
}