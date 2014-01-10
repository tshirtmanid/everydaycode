#include <stdio.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


static void dump_addrinfo(struct addrinfo* ai, const char* description) {
    /*
     *   struct addrinfo
     *       {
     *           int ai_flags;
     *               int ai_family; //AF_INET,AF_INET6,UNIX etc
     *                   int ai_socktype; //STREAM,DATAGRAM,RAW
     *                       int ai_protocol; //IPPROTO_IP, IPPROTO_IPV4, IPPROTO_IPV6 etc
     *                           size_t ai_addrlen;//length of ai_addr
     *                               char* ai_canonname; //full hostname
     *                                   struct sockaddr* ai_addr; //addr of host
     *                                       struct addrinfo* ai_next;
     *                                           }
     *                                               */

    printf("-----------------dump_addr_info:%s-----------------\n", description);
    while (ai) {
        printf("ai_flags:%d, ai_family:%d, ai_socktype:%d, ai_protocol:%d, "\
                "ai_addrlen:%d, ai_canonname:%s, ai_addr:0x%lX\n", ai->ai_flags, ai->ai_family, ai->ai_socktype, ai->ai_protocol, ai->ai_addrlen,
                ai->ai_canonname ? ai->ai_canonname : "", (unsigned long) ai->ai_addr);

        if (ai->ai_addr) {
            if (ai->ai_family == AF_INET) {
                struct sockaddr_in *addr_in = (struct sockaddr_in*) ai->ai_addr;
                printf("AF_INET address, ai_addr:%s\n", inet_ntoa(addr_in->sin_addr));
            } else if (ai->ai_family == AF_INET6) {
                struct sockaddr_in6 *addr_in = (struct sockaddr_in6*) ai->ai_addr;

                char ip_buf[1024] = { 0 };
                inet_ntop(AF_INET6, &addr_in->sin6_addr, ip_buf, sizeof(ip_buf) / sizeof(ip_buf[0]));
                printf("AF_INET6 address, ai_addr:%s\n", ip_buf);
            }
            printf("----------------------------\n");
        }

        ai = ai->ai_next;
    }
}

int test_common_host_empty_hints() {
    struct addrinfo ai;
    memset(&ai, 0, sizeof(struct addrinfo)); 

    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    //hints.ai_flags = AI_NUMERICSERV;
    //hints.ai_protocol = IPPROTO_TCP;
    //hints.ai_socktype = SOCK_STREAM;
    //hints.ai_family = AF_INET;

    struct addrinfo* ret_ai =  NULL;
    int ret = getaddrinfo("www.qq.com", NULL, &hints, &ret_ai);
    if (ret != 0) {
        printf("getaddrinfo failed.err:%d\n", ret);
        return ret;
    }

    dump_addrinfo(ret_ai, __FUNCTION__);


    return 0;
} 

int test_common_host_with_family_host() {
    struct addrinfo ai;
    memset(&ai, 0, sizeof(struct addrinfo)); 

    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    //hints.ai_flags = AI_NUMERICSERV;
    //hints.ai_protocol = IPPROTO_TCP;
    //hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_INET;

    struct addrinfo* ret_ai =  NULL;
    int ret = getaddrinfo("www.qq.com", NULL, &hints, &ret_ai);
    if (ret != 0) {
        printf("getaddrinfo failed.err:%d\n", ret);
        return ret;
    }

    dump_addrinfo(ret_ai, __FUNCTION__);


    return 0;
} 

int test_cannonname() {
    struct addrinfo ai;
    memset(&ai, 0, sizeof(struct addrinfo)); 

    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_flags = AI_CANONNAME;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_INET;

    struct addrinfo* ret_ai =  NULL;
    int ret = getaddrinfo("qq.com", NULL, &hints, &ret_ai);
    if (ret != 0) {
        printf("getaddrinfo failed.err:%d\n", ret);
        return ret;
    }

    dump_addrinfo(ret_ai, __FUNCTION__);


    return 0;
} 

int test_numericserv() {
    struct addrinfo ai;
    memset(&ai, 0, sizeof(struct addrinfo)); 

    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_flags = AI_NUMERICSERV;
    //hints.ai_protocol = IPPROTO_TCP;
    //hints.ai_socktype = SOCK_STREAM;
    //hints.ai_family = AF_INET;

    struct addrinfo* ret_ai =  NULL;
    int ret = getaddrinfo(NULL, "80", &hints, &ret_ai);
    if (ret != 0) {
        printf("getaddrinfo failed.err:%d\n", ret);
        return ret;
    }

    dump_addrinfo(ret_ai, "test getservbyport");


    return 0;
}


int test_getserrvbyport(int argc, char** argv) {
    if (argc < 2) {
        printf("usage:'this_binary port'\n");
        return -1;
    }

    unsigned short port = atoi(argv[1]);

    struct servent* ent = getservbyport(htons(port), "tcp");
    if (!ent) {
        //if ent is null, errno is still 0 from the test
        printf("getservbyport failed.errno:%s\n", strerror(errno));
        return -1;
    }

    /*from strace result:
     *first /var/run/nscd/socket
     * second nss_db 
     * last reads /etc/service
     */
    printf("port:%d, s_name:%s, s_port:%d, s_proto:%s\n", port, ent->s_name, ntohs(ent->s_port), ent->s_proto);

    char** p = ent->s_aliases;
    while (p && *p) {
        printf("alias:%s\n", *p);
        ++p;
    }

    return 0;
}

int main(int argc, char** argv) {
    //test_getserrvbyport(argc, argv);
    //test_numericserv();
    //test_common_host_with_family_host();
    //test_common_host_empty_hints();
    test_cannonname();
}
