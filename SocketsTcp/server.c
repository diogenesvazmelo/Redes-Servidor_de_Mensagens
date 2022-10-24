#include "commom.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFSZ 1024


int main(int argc, char **argv)
{
    //Inicializa os dois racks sem nenhum switch ativado
    int rack1[4] = {0, 0, 0, 0};
    int rack2[4] = {0, 0, 0, 0};

    struct sockaddr_storage storage;
    if (ServerSockInit(argv[1], argv[2], &storage) != 0)
    {
        logExit("Parameter error");
    }

    int s;
    s = socket(storage.ss_family, SOCK_STREAM, 0);
    if (s == -1)
    {
        logExit("Error at socket creation");
    }

    int enable = 1;
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) != 0)
    {
        logExit("Setsockopt");
    }

    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if (bind(s, addr, sizeof(storage)) != 0)
    {
        logExit("Error at bind");
    }
    if (listen(s, 1) != 0)
    {
        logExit("Error at listen");
    }
    char addrStr[BUFSIZ];
    addrToStr(addr, addrStr, BUFSIZ);
    printf("Bound to %s, waiting connection\n", addrStr);

    while (1)
    {
        struct sockaddr_storage cstorage;
        struct sockaddr *caddr = (struct sockaddr *)(&cstorage);
        socklen_t caddrlen = sizeof(cstorage);
        int csock = accept(s, caddr, &caddrlen);
        if (csock == -1)
        {
            logExit("Accept error");
        }

        char caddrStr[BUFSIZ];
        addrToStr(caddr, caddrStr, BUFSIZ);
        printf("Connection from %s\n", caddrStr);
        char buf[BUFSIZ];
        while(1){
            memset(buf, 0, BUFSIZ);
            size_t count = read(csock, buf, BUFSIZ - 1);
            char* res = handleMessage(buf, rack1, rack2);
            if(res == NULL){
                strcpy(buf, "close");
                count = send(csock, buf, strlen(buf) + 1, 0);
                close(csock);
                return 0;
            }
            memset(buf, 0, BUFSIZ);
            strcpy(buf, res);
            count = send(csock, buf, strlen(buf) + 1, 0);
            if (count != strlen(buf) + 1)
            {
                logExit("Error at send message\n");
            }
        }
        
    }
    exit(EXIT_SUCCESS);
}