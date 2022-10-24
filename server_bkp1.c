#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

#define BUFSZ 1024


// VAR Globais para switches e racks
char *ID_RACKS[] = {
    "01",
    "02",
    "03",
    "04"
};
char *ID_SWITCHES[] = {
    "01",
    "02",
    "03",
    "04"
};
char *INSTRUCOES[] = {
    "add sw in ",   // Instrução 1
    "rm sw in ",    // Instrução 2
    "get in ",      // Instrução 3
    "ls "           // Instrução 4
};

typedef struct {
    char *switches[3];
} Rack;

char DATACENTER[4][3];

int identificaInst(const char *msgnonumbers){
    for (int i = 0; i < (sizeof(INSTRUCOES) / sizeof(char *)); i++){
        if (0 == strcmp(INSTRUCOES[i], msgnonumbers)){
            return i+1;
        }
    }
    return 0;
}

void rmvnumeros(const char *mensagem, char *msgnonumbers){
    int i = 0;
    while(mensagem[i] != '\n'){
        if (
                mensagem[i] != '0' &&
                mensagem[i] != '1' &&
                mensagem[i] != '2' &&
                mensagem[i] != '3' &&
                mensagem[i] != '4' &&
                mensagem[i] != '5' &&
                mensagem[i] != '6' &&
                mensagem[i] != '7' &&
                mensagem[i] != '8' &&
                mensagem[i] != '9'
        ){
            if(!(msgnonumbers[strlen(msgnonumbers)-1] == ' ' && mensagem[i] == ' ')){
                //printf("Caracter: --%c--\n", mensagem[i]);
                msgnonumbers[strlen(msgnonumbers)] = mensagem[i];
            }
        }
        i++;
    }
}

void rmvletras(const char *mensagem, char *msgnumbers){    
    int i = 0;
    while(mensagem[i] != '\n'){
        if (
                mensagem[i] == '0' ||
                mensagem[i] == '1' ||
                mensagem[i] == '2' ||
                mensagem[i] == '3' ||
                mensagem[i] == '4' ||
                mensagem[i] == '5' ||
                mensagem[i] == '6' ||
                mensagem[i] == '7' ||
                mensagem[i] == '8' ||
                mensagem[i] == '9' ||
                mensagem[i] == ' '
        ){
            if((!(msgnumbers[strlen(msgnumbers)-1] == ' ' && mensagem[i] == ' ')) && !(strlen(msgnumbers) == 0 && mensagem[i] == ' ')){
                //printf("Caracter: --%c--\n", mensagem[i]);
                msgnumbers[strlen(msgnumbers)] = mensagem[i];
            }
        }
        i++;
    }
    printf("%s\n", msgnumbers);
}

int analisamsg(const char *mensagem){
    // Checa se a mensagem é se saída
    if (0 == strcmp(mensagem, "exit")){
        exit(EXIT_SUCCESS);
    }

    // Nova string para apenas a instrução, sem números de racks ou switches
    char msgnonumbers[BUFSZ];
    memset(msgnonumbers, 0, BUFSZ);
    // A função rmvnumeros() remove os caracteres numéricos da mensagem original
    // e salva a string resultante no segundo parâmetro
    rmvnumeros(mensagem, msgnonumbers);
    printf("%s\n", msgnonumbers);
    // A função identificaInst() encontra uma instrução correspondente para a mensagem.
    // Se não encontrar nenhuma instrução correspondente, retorna zero
    int instrucao = identificaInst(msgnonumbers);
    if (instrucao == 0){
        exit(EXIT_FAILURE);
    }
    printf("Instrucao: %d\n", instrucao);

    char msgnumbers[BUFSZ];
    memset(msgnumbers, 0, BUFSZ);
    rmvletras(mensagem, msgnumbers);
    return 0;
}

void usage(int argc, char **argv){
    printf("usage: %s <v4|v6> <server port>\n", argv[0]);
    printf("example: %s v4 51511\n", argv[0]);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv){
    if (argc < 3){
        usage(argc, argv);
    }

    struct sockaddr_storage storage;
    // Faz parse do enderço ipv4 ou ipv6, e se der certo retorna zero
    if (0 != server_sockaddr_init(argv[1], argv[2], &storage)){
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

    // Para poder reutilizar a mesma porta após encerrar o servidor
    int enable = 1;
    if (0 != setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int))){
        logexit("setsockopt");
    }

    //  Inicializa addr | Cast de sockaddr_storage para sckaddr | ponteiro para storage 
    struct sockaddr *addr = (struct sockaddr *)(&storage);
    // Verifica se houve erro ao estabelecer ao ligar. Se a conexão
    // for estabelecida com sucesso, retorna 0
    if (0 != bind(s, addr, sizeof(storage))){
        // Se der algum erro, logexit e exibe a mensagem informando
        // que saiu no bind
        logexit("bind");
    }

    // 10 é o máximo de conexões que podem estar pendentes para tratamento
    if (0 != listen(s, 10)){
        // Se der algum erro, logexit e exibe a mensagem informando
        // que saiu no listen
        logexit("listen");
    }

    // Endereço como string
    char addrstr[BUFSZ];
    // Converte o endereço para string
    addrtostr(addr, addrstr, BUFSZ);
    // Imprime mensagem informativa no terminal
    printf("bound to %s, waiting connections\n", addrstr);

    // Tratavita de conexões dentro do loop. 
    // O servidor, após tratar o 1º cliente, vai tratar o 2º, 3º, etc...
    while (1){
        // Armazena o endereço do cliente
        struct sockaddr_storage cstorage;
        struct sockaddr *caddr = (struct sockaddr *)(&cstorage);
        socklen_t caddrlen = sizeof(cstorage);

        // "accept" retorna um novo cliente.
        // "s" é o socket que recebe a conexão, enquanto
        // "csock" é o socket que conversa com o cliente
        int csock = accept(s, caddr, &caddrlen);
        if (csock == -1){
            logexit("accept");
        }

        // Toda vez que o cliente conectar, printa a mensagem na tela
        char caddrstr[BUFSZ];
        addrtostr(caddr, caddrstr, BUFSZ);
        printf("[log] connection from %s\n", caddrstr);

    /* Ler a mensagem do cliente */ 
        char buf[BUFSZ];
        // Zerar a memória
        memset(buf, 0, BUFSZ);
        // Recebe a mensagem do cliente
        size_t count = recv(csock, buf, BUFSZ, 0);
        // Imprime mensagem do cliente
        printf("[msg] %s, %d bytes: %s\n", caddrstr, (int)count, buf);
    /**/ 

        analisamsg(buf);

    /* Mandar resposta para o cliente. Nesse caso, manda o ip que conectou */ 
        sprintf(buf, "remote endpoint: %.1000s\n", caddrstr);
        // count é opcional, apenas para conferência do número 
        // de dados enviados 
        count = send(csock, buf, strlen(buf)+1, 0);
        if (count != strlen(buf)+1){
            // Se não mandou o número certo de dados, fecha o servidor
            logexit("send");
        }
    /**/ 

        // Fecha a conexão com o cliente atual
        close(csock);
    }
    
    exit(EXIT_SUCCESS);
}