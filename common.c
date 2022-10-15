#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>

void logexit(const char *mensagem){
    perror(mensagem);
    exit(EXIT_FAILURE);
}

int addrparse(const char *addrstr, const char *portstr, struct sockaddr_storage *storage){
    // Se receber null
    if (addrstr == NULL || portstr == NULL){
        return -1;
    }

    // Parse da porta
    uint16_t port; //unsigned short
    port = (uint16_t)atoi(portstr);

    // Verifica se o portstr passado corresponde a um valor válido
    if (port == 0){
        return -1;
    }

    // Converte Little Endian para Big Endian, caso necessário
    port = htons(port); // host to network short

    struct in_addr inaddr4; // 32-bit IP address
    // Tenta fazer parser de ipv4. Se der certo, resultado
    // na variável inaddr4
    if (inet_pton(AF_INET, addrstr, &inaddr4)){
        struct sockaddr_in *addr4 = (struct sockaddr_in *)storage;
        addr4->sin_family = AF_INET;
        addr4->sin_port = port;
        addr4->sin_addr = inaddr4;
        return 0; // Sucesso
    }

    struct in6_addr inaddr6; // 128-bit IPv6 address
    if (inet_pton(AF_INET6, addrstr, &inaddr6)){
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)storage;
        addr6->sin6_family = AF_INET6;
        addr6->sin6_port = port;
        memcpy(&(addr6->sin6_addr), &inaddr6, sizeof(inaddr6));
        return 0; // Sucesso
    }
    
    // Se não conseguir fazer o parser com nenhum dois dois 
    return -1;
}

void addrtostr(const struct sockaddr *addr, char *str, size_t strsize){
    int version;
    char addrstr[INET6_ADDRSTRLEN + 1] = "";
    uint16_t port;
    
    if (addr->sa_family == AF_INET){
        version = 4;
        struct sockaddr_in *addr4 = (struct sockaddr_in *)addr;
        
        if (!inet_ntop(AF_INET, &(addr4->sin_addr), addrstr, INET6_ADDRSTRLEN + 1)){
            logexit("ntop ipv4");
        }
        port = ntohs(addr4->sin_port); // network to host short

    } else if (addr->sa_family == AF_INET6){
        version = 6;
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)addr;
        
        if (!inet_ntop(AF_INET6, &(addr6->sin6_addr), addrstr, INET6_ADDRSTRLEN + 1)){
            logexit("ntop ipv6");
        }
        port = ntohs(addr6->sin6_port); // network to host short
    } else {
        logexit("unknown protocol family.");
    }
    if (str){
        snprintf(str, strsize, "IPv%d %s %hu", version, addrstr, port);
    }
}

int server_sockaddr_init(const char *proto, const char *portstr, struct sockaddr_storage *storage){
    // Parse da porta
    uint16_t port; //unsigned short
    port = (uint16_t)atoi(portstr);

    // Verifica se o portstr passado corresponde a um valor válido
    if (port == 0){
        return -1;
    }

    // Converte Little Endian para Big Endian, caso necessário
    port = htons(port); // host to network short

    memset(storage, 0, sizeof(*storage));
    if(0 == strcmp(proto, "v4")){
        struct sockaddr_in *addr4 = (struct sockaddr_in *)storage;
        addr4->sin_family = AF_INET;
        addr4->sin_port = port;
        // "INADDR_ANY": qualquer endereço que o computador tenha
        addr4->sin_addr.s_addr = INADDR_ANY;
        return 0; // Sucesso
    } else if(0 == strcmp(proto, "v6")){
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)storage;
        addr6->sin6_family = AF_INET6;
        addr6->sin6_port = port;
        // "in6addr_any": qualquer endereço que o computador tenha
        addr6->sin6_addr = in6addr_any;
        return 0; // Sucesso
    } else {
        return -1;
    }
}