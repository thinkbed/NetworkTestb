project (NetworkTestb)

cmake_minimum_required (VERSION 2.8)

INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}/include
)

LINK_DIRECTORIES(
)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

add_subdirectory(src)
add_subdirectory(client)
add_subdirectory(server)
add_subdirectory(client_test)
add_subdirectory(server_test)
