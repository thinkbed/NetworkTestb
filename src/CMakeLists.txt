
LINK_DIRECTORIES(
    /usr/local/lib
)

SET (KCPCONNECTION_SRC
  KcpConnection.cpp
  KcpConnectionManager.cpp
)

ADD_LIBRARY(kcp_connection_static STATIC ${KCPCONNECTION_SRC})
ADD_LIBRARY(kcp_connection SHARED ${KCPCONNECTION_SRC})

SET_TARGET_PROPERTIES(kcp_connection_static PROPERTIES OUTPUT_NAME
  "kcp_connection")
#  SET_TARGET_PROPERTIES(kcp_connection_static PROPERTIES CLEAN_DIRECT_OUTPUT 1)

target_link_libraries(kcp_connection event)
