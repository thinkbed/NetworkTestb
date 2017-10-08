#include <unordered_map>
#include <netinet/in.h>
#include <KcpConnection.h>

// typedef std::unordered_map UnorderedMap;
typedef unsigned long ULong;

class KcpConnectionManager
{
  protected:
    std::unordered_map<ULong, KcpConnection> m_connection_map;
    int m_listening_socket;
    char m_receive_buff[1024];

  public:
    KcpConnectionManager();
    void tick();
    void listen(unsigned short listening_port);
};
