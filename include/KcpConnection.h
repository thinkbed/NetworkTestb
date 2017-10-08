#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define KCP_CONNECTION_STATUS_UNKNOWN 0
#define KCP_CONNECTION_STATUS_CONNECTING 1
#define KCP_CONNECTION_STATUS_CONNECTING_ACK 2
#define KCP_CONNECTION_STATUS_CONNECTED 3
#define KCP_CONNECTION_STATUS_DISCONNECTED 4
#define KCP_CONNECTION_STATUS_ACTIVE_SHUTDOWN 5
#define KCP_CONNECTION_STATUS_PASSIVE_SHUTDOWN 6

#define ERR_EXIT(m) \
    do { \
    perror(m); \
    exit(EXIT_FAILURE); \
    } while (0)

typedef unsigned int KcpConnectionStatus;

class KcpConnection
{
  protected:
    friend class KcpConnectionManager;

    struct sockaddr_in peer_addr;
    int m_socket;

    KcpConnectionStatus m_status;

  public:
    KcpConnection();
    void tick();
    void connect(const char* ip, unsigned short port);
    void send(const char* data, const unsigned int data_size);
};
