//
// Created by dendy on 19-5-20.
//

#include "ShowIp.h"
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

// getaddrinfo使用方式
// 构建一个addrinfo的模板(拥有指定的枚举类型)
// 然后生成出一个带有具体数据符合模板参数的结构实体

int ShowIp() {
    int status;
    addrinfo hints, *servinfo, *p;
    char ipstr[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // 这句话不写ipv4的结果是127.0.0.1

    if ((status = getaddrinfo(NULL, "3490", &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        return 1;
    }

    for (p = servinfo; p != NULL; p = p->ai_next) {
        void *addr;
        char *ipver;

        if (p->ai_family == AF_INET) {
            sockaddr_in *ipv4 = (sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
            ipver = "Ipv4";
        } else {
            sockaddr_in6 *ipv6 = (sockaddr_in6 *)p->ai_addr;
            addr = &(ipv6->sin6_addr);
            ipver = "Ipv6";
        }

        inet_ntop(p->ai_family, addr, ipstr, sizeof(ipstr));
        printf("  %s: %s\n", ipver, ipstr);
    }

    freeaddrinfo(servinfo);
    return 0;
}


int
ShowIp(int argc, char const *argv[]) {
    addrinfo hints, *res, *p;
    int status;
    char ipstr[INET6_ADDRSTRLEN];

    if (argc != 2) {
        fprintf(stderr, "Usage: ShowIp hostname\n");
        return 1;
    }

    memset(&hints, 0, sizeof(hints)); // 分配内存类似c++的new
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((status = getaddrinfo(argv[1], NULL, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 2;
    }

    printf("IP address for %s:\n\n", argv[1]);


    // 一个域名是可以有多个ip地址的
    for (p = res; p != NULL; p = p->ai_next) {
        void *addr;
        char *ipver;

        if (p->ai_family == AF_INET) {
            sockaddr_in *ipv4 = (sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
            ipver = "Ipv4";
        } else {
            sockaddr_in6 *ipv6 = (sockaddr_in6 *)p->ai_addr;
            addr = &(ipv6->sin6_addr);
            ipver = "Ipv6";
        }

        inet_ntop(p->ai_family, addr, ipstr, sizeof(ipstr));
        printf("  %s: %s\n", ipver, ipstr);
    }

    freeaddrinfo(res);
    return 0;
}
