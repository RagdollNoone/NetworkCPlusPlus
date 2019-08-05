add_executable(EasyClient
        EasyClient.cpp
        main.cpp
        )

find_package(Threads REQUIRED)
target_link_libraries(EasyClient Threads::Threads)