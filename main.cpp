#include <stdio.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char** argv) {
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
