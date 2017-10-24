#include <unordered_map>
#include <netinet/in.h>
//#include <sys/epoll.h>
#include <KcpConnection.h>

// typedef std::unordered_map UnorderedMap;
typedef unsigned long ULong;

class KcpConnectionManager
{
  protected:
    std::unordered_map<ULong, KcpConnection> m_connection_map;
    int m_listening_socket, m_epoll_fd;
    

    struct event_base* m_event_base = nullptr;

  protected:
	static void setNonBlocking(int sock);

  public:
  	char m_receive_buff[1024];

  public:
    KcpConnectionManager();
    void tick();
    void listen(unsigned short listening_port);

    // epoll implementation
    // void listenWithEpoll(unsigned short listening_port);
    // void tickWithEpoll();

    // kqueue implementation
    void listenWithKqueue(unsigned short listening_port);
    void tickWithKqueue();

    // libevent implementation
    void listenWithLibevent(unsigned short listening_port);
    void eventMainLoop();
};
