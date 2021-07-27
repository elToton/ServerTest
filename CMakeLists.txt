cmake_minimum_required(VERSION 3.10)
set(CXX_FLAGS "-std=c++14")

project(mainS)
set(SERVERSOURCES mainServer.cpp Handler.cpp Server.cpp)
add_executable(mainS ${SERVERSOURCES})

project(mainC)
set(CLIENTSOURCES mainClient.cpp Handler.cpp TCP_Client.cpp UDP_Client.cpp)
add_executable(mainC ${CLIENTSOURCES})