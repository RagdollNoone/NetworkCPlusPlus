add_executable(FirstClient
        Client.cpp
        )

find_package(Threads REQUIRED)
target_link_libraries(FirstClient Threads::Threads)