#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <event2/event.h>
#include "../include/KcpConnectionManager.h"

KcpConnectionManager::KcpConnectionManager()
{
     m_listening_socket = 0;
}

void KcpConnectionManager::tick()
{
    struct sockaddr_in peer_addr;
    char szAddr[256];
    char addr[16];
    const char* str = NULL;
    int n;
    socklen_t peer_len;

    if(m_listening_socket <= 0)
        ERR_EXIT("socket invalid in tick.");

    peer_len = sizeof(peer_addr);
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
        // parse received data.
        str = inet_ntop(AF_INET, (void*)&peer_addr.sin_addr, addr, 16);

        printf("receive %s from ip %s\n",m_receive_buff, str);
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

    printf("Start listening at port %d\r\n", listening_port);
}

void KcpConnectionManager::setNonBlocking(int sock)
{
    int opt;

    opt = fcntl(sock, F_GETFL);
    if(opt < 0)
    {
        perror("fcntl(sock, F_GETFL)");
        exit(1);
    }

    opt = opt | O_NONBLOCK;
    if( fcntl(sock, F_SETFL, opt) < 0)
    {
        perror("fcntl(sock, F_SETFL, opt)");
        exit(1);
    }
}

/*
void KcpConnectionManager::listenWithEpoll(unsigned short listening_port)
{
    int ret;
    struct epoll_event ev;
    struct sockaddr_in servaddr;

    if(m_listening_socket > 0)
        close(m_listening_socket);

    if((m_listening_socket = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
        ERR_EXIT(“socket error.”);

    m_epoll_fd = epoll_create(256);
    printf("epoll_create(256) return %d\n", m_epoll_fd);

    setnonblocking(m_listening_socket);

    ev.data.fd = m_listening_socket;
    ev.events = EPOLLIN | EPOLLET;

    ret = epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, m_listening_socket, &ev);
    printf("epoll_ctl return %d\n", ret);

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(listening_port);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(m_listening_socket, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
        ERR_EXIT("bind error");
}

void KcpConnectionManager::tickWithEpoll()
{
    int nfds, i, sockfd, n;
    struct epoll_event* events, ev;
    struct sockaddr_in peer_addr;
    socklen_t peer_len;
    char szAddr[256];

    nfds = epoll_wait(m_epoll_fd, events, 10000, 0);

    for(i = 0; i < nfds; i++)
    {
        if(events[i].events & EPOLLIN)
        {
            if ( (sockfd = events[i].data.fd) < 0)
                continue;

            if((n = recvfrom(sockfd, m_receive_buff, sizeof(m_receive_buff), 0, (struct sockaddr *)&peer_addr, peer_len)) < 0)
            {
                if(error == ECONNRESET)
                {
                    close(sockfd);
                    events[i].data.fd = -1;
                }
                else
                    printf("readline error\n");
            }
            else if(n == 0)
            {
                perror("connfd=0\n");
                close(sockfd);
                events[i].data.fd = -1;
            }

            char* p = (char*)&peer_addr.sin_addr;
            sprintf(szAddr, "%d.%d.%d.%d", *p, *(p+1), *(p+2), *(p+3));

            printf("receive %s from ip %s\n", m_receive_buff, szAddr);

            ev.data.fd = sockfd;
            ev.events = EPOLLOUT | EPOLLET;

            epoll_ctl(m_epoll_fd, EPOLL_CTL_MOD, sockfd, &ev);
        }
        else if(events[i].events & EPOLLOUT)
        {
            if(events[i].data.fd == -1)
                continue;

            write(sockfd, m_receive_buff, n);
            printf("send %s\n", m_receive_buff);

            ev.data.fd = sockfd;
            ev.events = EPOLLIN | EPOLLET;
            char send_data[256] = "test";
            sendto(sockfd, send_data, strlen(send_data), 0, (struct sockaddr *) &peer_addr, &peer_len);

            epoll_ctl(m_epoll_fd, EPOLL_CTL_MOD, sockfd, &ev);
        }
    }
}
*/

void KcpConnectionManager::listenWithKqueue(unsigned short listening_port)
{
    int ret;
    struct sockaddr_in servaddr;

    if(m_listening_socket > 0)
        close(m_listening_socket);

    if((m_listening_socket = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
        ERR_EXIT("socket error.");

    m_epoll_fd = kqueue();
    printf("kqueue return %d\n", m_epoll_fd);
    printf("listening fd %d\n", m_listening_socket);

    setNonBlocking(m_listening_socket);

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(listening_port);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(m_listening_socket, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
        ERR_EXIT("bind error");

    struct kevent ev;
    EV_SET(&ev, m_listening_socket, EVFILT_READ, EV_ADD|EV_ENABLE, 0, 0, (void*)(intptr_t)m_listening_socket);

    kevent(m_epoll_fd, &ev, 1, NULL, 0, NULL);
}

void KcpConnectionManager::tickWithKqueue()
{
    struct kevent activeEvs[20];
    struct timespec timeout;
    struct sockaddr_in peer_addr;
    socklen_t peer_len;
    char addr[16];
    const char* str = NULL;
    char szAddr[256];
    int read_data_size = 0;

    peer_len = sizeof(peer_addr);

    int n = kevent(m_epoll_fd, NULL, 0, activeEvs, 20, NULL);

    printf("event num %d\n", n);

    for(int i = 0; i < n; i++)
    {
        int fd = (int)(intptr_t)activeEvs[i].udata;
        int events = activeEvs[i].filter;
        if(events == EVFILT_READ)
        {
            if(fd == m_listening_socket)
            {
                // handleAccept
            }
            else
            {
                // handleRead
            }

            memset(m_receive_buff, 0, sizeof(m_receive_buff));
            if((read_data_size = recvfrom(fd, m_receive_buff, sizeof(m_receive_buff), 0, (struct sockaddr *)&peer_addr, &peer_len)) < 0)
            {
                printf("readline error\n");
            }
            else if(read_data_size == 0)
            {
                perror("connfd=0\n");
                close(fd);
            }

            str = inet_ntop(AF_INET, (void*)&peer_addr.sin_addr, addr, 16);

            char* p = (char*)&peer_addr.sin_addr;

            sprintf(szAddr, "%d.%d.%d.%d", *p, *(p+1), *(p+2), *(p+3));

            printf("fd %d, receive %s from ip %s, port %d\n",fd, m_receive_buff, str, peer_addr.sin_port);

        }
        else if (events == EVFILT_WRITE)
        {
            // handleWrite
        }
        else
        {
            printf("unknown event.");
        }
    }
}

void cb_function(evutil_socket_t fd, short what, void *arg)
{
    struct sockaddr_in peer_addr;
    socklen_t peer_len;
    int receive_data_size;
    char addr[16];
    const char* str = NULL;
    char szAddr[256];

    KcpConnectionManager* kcp_connection_manager = (KcpConnectionManager*) arg;

    peer_len = sizeof(peer_addr);

    if( (what&EV_READ))
    {
        memset(kcp_connection_manager->m_receive_buff, 0, sizeof(kcp_connection_manager->m_receive_buff));

        if ( (receive_data_size = recvfrom(fd, kcp_connection_manager->m_receive_buff, sizeof(kcp_connection_manager->m_receive_buff), 0, (struct sockaddr*)&peer_addr, &peer_len)) < 0)
        {
            /* code */
        }
        else if (receive_data_size == 0)
        {
            /* code */
        }

        str = inet_ntop(AF_INET, (void*)&peer_addr.sin_addr, addr, 16);

        char* p = (char*)&peer_addr.sin_addr;

        sprintf(szAddr, "%d.%d.%d.%d", *p, *(p+1), *(p+2), *(p+3));

        printf("fd %d, receive %s from ip %s, port %d\n",fd, kcp_connection_manager->m_receive_buff, str, peer_addr.sin_port);
    }
}

void KcpConnectionManager::listenWithLibevent(unsigned short listening_port)
{
    struct sockaddr_in servaddr;
    struct event* listening_event;

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

    printf("Start listening at port %d\r\n", listening_port);

    m_event_base = event_base_new();
    listening_event = event_new(m_event_base, m_listening_socket, EV_READ | EV_ET | EV_PERSIST, cb_function, this);
    event_add(listening_event, NULL);
}

void KcpConnectionManager::eventMainLoop()
{
    event_base_dispatch(m_event_base);
}









