cmake_minimum_required(VERSION 3.13)
project(Network)

set(CMAKE_CXX_STANDARD 14)

add_executable(
        Network main.cpp
        NetWorkFirstTry/NetWorkFirstTry.cpp
        NetWorkFirstTry/NetWorkFirstTry.h
        ShowIp/ShowIp.cpp
        ShowIp/ShowIp.h
#        ClientServer/Server.cpp
#        ClientServer/Client.cpp
)