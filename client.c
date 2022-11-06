#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFSZ 1024

void usage(int argc, char **argv){
    printf("usage: %s <server IP> <server port>\n", argv[0]);
    printf("example: %s 127.0.0.1 51511\n", argv[0]);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv){
    if (argc < 3){
        usage(argc, argv);
    }

    struct sockaddr_storage storage;
    // Faz parse do enderço ipv4 ou ipv6, e se der certo retorna zero
    if (0 != addrparse(argv[1], argv[2], &storage)){
        // Se der erro, imprime a mensagem de usage do programa
        usage(argc, argv);
    }

    // Cria socket
    int s = socket(storage.ss_family, SOCK_STREAM, 0);
    // Verifica se houve erro ao abrir o socket. Se houver erro
    // a função socket retorna -1
    if(s == -1){
        // Se der algum erro, logexit e exibe a mensagem informando
        // que saiu no socket
        logexit("socket");
    }

    //  Inicializa addr | Cast de sockaddr_storage para sckaddr | ponteiro para storage 
    struct sockaddr *addr = (struct sockaddr *)(&storage);
    // Verifica se houve erro ao estabelecer a conexão. Se a conexão
    // for estabelecida com sucesso, retorna 0
    if (0 != connect(s, addr, sizeof(storage))){
        // Se der algum erro, logexit e exibe a mensagem informando
        // que saiu no connect
        logexit("connect");
    }

    // Cria variável para representação do endereço em string
    char addrstr[BUFSZ];
    // Chama função para converter endereço em string
    addrtostr(addr, addrstr, BUFSZ);
    // Imprime a string do endereço
    printf("connected to %s\n", addrstr);

    // Cria novo buffer para armazenar o dado
    char buf[BUFSZ];
    while (1){
        // Inicializa o buffer com zero
        memset(buf, 0, BUFSZ);

        // Imprime no prompt
        // printf("mensagem> ");
        // Recebe a mensagem via input do teclado
        fgets(buf, BUFSZ-1, stdin);
        // Envia para o socket "s" o buffer "buf" de tamanho 
        // strlen(buf) acrescido do caracter /0 
        size_t count = send(s, buf, strlen(buf)+1, 0);
        // Se o número de bits transmitidos na rede (count) for
        // diferente de strlen(buf)+1, houve erro no send
        if (count != strlen(buf)+1){
            // Se der algum erro, logexit e exibe a mensagem informando
            // que saiu no send
            logexit("send");
        }

        // Não mover esse if
        if(strcmp(buf, "exit\n") == 0){
            close(s);
            return 0;
            exit(EXIT_SUCCESS);
    }

    // Inicializa o buffer com zero, agora para receber do servidor
    memset(buf, 0, BUFSZ);

    // Total de bits recebidos até o momento
    unsigned total = 0;
        //count = recv(s, buf + total, BUFSZ - total, 0);
        count = read(s, buf + total, BUFSIZ - total);
        if(strcmp(buf, "exit\n") == 0){
            close(s);
            return 0;
            exit(EXIT_SUCCESS);
        }
        // Se não receber nada (isso significa que a conexão
        // está fechada)
        if (count == 0){
            break;
        }
        total += count;

    //printf("received %u bytes\n", total);
    puts(buf);    
    }
    close(s);

    exit(EXIT_SUCCESS);
}