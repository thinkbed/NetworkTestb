#include <unistd.h>
#include <sys/types.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "../include/KcpConnection.h"

KcpConnection::KcpConnection()
{
    m_socket = -1;
}

void KcpConnection::tick()
{

}

void KcpConnection::connect(const char* ip, unsigned short port)
{
    if( ( m_socket = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
        ERR_EXIT("socket error.");

    memset(&peer_addr, 0, sizeof(peer_addr));
    peer_addr.sin_family = AF_INET;
    peer_addr.sin_port = htons(port);
    peer_addr.sin_addr.s_addr = inet_addr(ip);
}

void KcpConnection::send(const char* data, const unsigned int data_size)
{
	sendto(m_socket, data, data_size, 0, (struct sockaddr *)&peer_addr, sizeof(peer_addr));
}
