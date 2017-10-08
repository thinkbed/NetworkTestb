#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <netinet/in.h>
#include "../include/KcpConnectionManager.h"


KcpConnectionManager::KcpConnectionManager()
{
     m_listening_socket = 0;
}

void KcpConnectionManager::tick()
{
    struct sockaddr_in peer_addr;
    int n;
    socklen_t peer_len;

    if(m_listening_socket <= 0)
        ERR_EXIT("socket invalid in tick.");

    memset(m_receive_buff, 0, sizeof(m_receive_buff));
    n = recvfrom(m_listening_socket, m_receive_buff, sizeof(m_receive_buff), 0, (struct sockaddr*)&peer_addr, &peer_len);
    if(n <= 0)
    {
        if(errno == EINTR)
            return;
        ERR_EXIT("receive error.");
    }
    else
    {
        printf("Received data: %s\r\n",  m_receive_buff);
        // parse received data.
    }
}

void KcpConnectionManager::listen(unsigned short listening_port)
{
    struct sockaddr_in servaddr;

    if(m_listening_socket >  0)
         close(m_listening_socket);

    if((m_listening_socket = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
         ERR_EXIT("socket error");
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(listening_port);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(m_listening_socket, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        ERR_EXIT("bind error");

    printf("Start listening at port %d ", listening_port);
}
