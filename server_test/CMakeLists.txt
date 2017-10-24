add_executable(server_test server_test.cpp)


target_link_libraries(server_test libkcp_connection.a;libevent.a)
