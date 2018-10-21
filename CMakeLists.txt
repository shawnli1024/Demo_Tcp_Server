cmake_minimum_required (VERSION 2.8)
PROJECT (SHTcpServer)

# 添加 binParser 子目录
add_subdirectory(binParser)
add_subdirectory(mo_log)
add_subdirectory(dataSwitchByte)

SET(CMAKE_C_COMPILER g++)
if(CMAKE_COMPILER_IS_GNUCXX)
   set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
endif(CMAKE_COMPILER_IS_GNUCXX)


SET(SRC_LIST main.cpp MoMysql.cpp MoBase64.cpp)
ADD_EXECUTABLE(SHTcpServer ${SRC_LIST})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(SHTcpServer Threads::Threads)

# 添加链接库
target_link_libraries(SHTcpServer binParser)
target_link_libraries(SHTcpServer mo_log)
target_link_libraries(SHTcpServer dataSwitchByte)
target_link_libraries(SHTcpServer mysqlclient)

