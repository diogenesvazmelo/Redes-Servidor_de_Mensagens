#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <stdbool.h>
#include <time.h> 

#include <sys/types.h>
#include <sys/socket.h>

#define BUFSZ 1024
#define MAXTOKENSINSTR 8

#define QTDSW 4
#define QTDRACKS 4

typedef struct {
    int id;
    char *rack;
    char *switches[3];
    int qtdswitches;
    bool find;
} Inst;

typedef struct {
    char *id;
    bool switches[4];
    char msg[BUFSIZ];
    int qtdswitches;
    bool erro;
} Rack;

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

char *INSTRUCOES[4][2] = {
    {"add sw", "in"},      // Instrução 0 -- Add até 3 sw em um rack
    {"rm sw", "in"},       // Instrução 1
    {"get", "in"},         // Instrução 2 -- leitura de até 2 sw em um rack
    {"ls"},                // Instrução 3
};


void imprimeTodasAsInstr(){
    printf("Iniciando teste impressao\n");
    // add 1 sw
    for (int a = 0; a < QTDRACKS; a++){
        for (int i = 0; i < QTDSW; i++){
            printf("%s %s %s %s\n", INSTRUCOES[0][0], ID_SWITCHES[i], INSTRUCOES[0][1], ID_RACKS[a]);
        }
    }
    // add 2 sw
    for (int a = 0; a < QTDRACKS; a++){
        for (int i = 0; i < QTDSW; i++){
            for (int j = 0; j < QTDSW; j++){
                if (i != j){
                    printf("%s %s %s %s %s\n", INSTRUCOES[0][0], ID_SWITCHES[i], ID_SWITCHES[j], INSTRUCOES[0][1], ID_RACKS[a]);
                }
            }
        }
    }
    // add 3 sw
    for (int a = 0; a < QTDRACKS; a++){
        for (int i = 0; i < QTDSW; i++){
            for (int j = 0; j < QTDSW; j++){
                for (int k = 0; k < QTDSW; k++){
                    if ((i != j) && (i != k) && (j != k)){
                        printf("%s %s %s %s %s %s\n", INSTRUCOES[0][0], ID_SWITCHES[i], ID_SWITCHES[j], ID_SWITCHES[k], INSTRUCOES[0][1], ID_RACKS[a]);
                    }
                }
            }
        }
    }

    printf("\n");
    // rm 1 sw
    for (int a = 0; a < QTDRACKS; a++){
        for (int i = 0; i < QTDSW; i++){
            printf("%s %s %s %s\n", INSTRUCOES[1][0], ID_SWITCHES[i], INSTRUCOES[1][1], ID_RACKS[a]);
        }
    }

    printf("\n");
    // get 1 sw
    for (int a = 0; a < QTDRACKS; a++){
        for (int i = 0; i < QTDSW; i++){
            printf("%s %s %s %s\n", INSTRUCOES[2][0], ID_SWITCHES[i], INSTRUCOES[2][1], ID_RACKS[a]);
        }
    }
    // get 2 sw
    for (int a = 0; a < QTDRACKS; a++){
        for (int i = 0; i < QTDSW; i++){
            for (int j = 0; j < QTDSW; j++){
                if (i != j){
                    printf("%s %s %s %s %s\n", INSTRUCOES[2][0], ID_SWITCHES[i], ID_SWITCHES[j], INSTRUCOES[2][1], ID_RACKS[a]);
                }
            }
        }
    }

    printf("\n");
    // ls
    for (int a = 0; a < QTDRACKS; a++){
        printf("%s %s\n", INSTRUCOES[3][0], ID_RACKS[a]);
    }
}

void imprimeInstrucao(Inst ins){
    printf("Instrucao reconhecida: %s\n", ins.find?"true":"false");
    if (ins.find == true){
        printf("Id: %d\n", ins.id);
        printf("Rack: %s\n", ins.rack);
        for (int i = 0; i < ins.qtdswitches; i++){
            printf("Sw%d: %s\n", i+1, ins.switches[i]);
        }
    }
}

Inst analisamsg(const char *mensagem){
    Inst instrucao;
    char instrucaotxt[BUFSZ];

    // add 1 sw
    for (int a = 0; a < QTDRACKS; a++){
        for (int i = 0; i < QTDSW; i++){
            memset(instrucaotxt, 0, BUFSZ);
            sprintf(instrucaotxt, "%s %s %s %s\n", INSTRUCOES[0][0], ID_SWITCHES[i], INSTRUCOES[0][1], ID_RACKS[a]);
            if (0 == strcmp(instrucaotxt, mensagem)){
                instrucao.id = 0;
                instrucao.rack = ID_RACKS[a];
                instrucao.switches[0] = ID_SWITCHES[i];
                instrucao.qtdswitches = 1;
                instrucao.find = true;
                return instrucao;
            }
        }
    }
    // add 2 sw
    for (int a = 0; a < QTDRACKS; a++){
        for (int i = 0; i < QTDSW; i++){
            for (int j = 0; j < QTDSW; j++){
                if (i != j){
                    memset(instrucaotxt, 0, BUFSZ);
                    sprintf(instrucaotxt, "%s %s %s %s %s\n", INSTRUCOES[0][0], ID_SWITCHES[i], ID_SWITCHES[j], INSTRUCOES[0][1], ID_RACKS[a]);
                    if (0 == strcmp(instrucaotxt, mensagem)){
                        instrucao.id = 0;
                        instrucao.rack = ID_RACKS[a];
                        instrucao.switches[0] = ID_SWITCHES[i];
                        instrucao.switches[1] = ID_SWITCHES[j];
                        instrucao.qtdswitches = 2;
                        instrucao.find = true;
                        return instrucao;
                    }
                }
            }
        }
    }
    // add 3 sw
    for (int a = 0; a < QTDRACKS; a++){
        for (int i = 0; i < QTDSW; i++){
            for (int j = 0; j < QTDSW; j++){
                for (int k = 0; k < QTDSW; k++){
                    if ((i != j) && (i != k) && (j != k)){
                        memset(instrucaotxt, 0, BUFSZ);
                        sprintf(instrucaotxt, "%s %s %s %s %s %s\n", INSTRUCOES[0][0], ID_SWITCHES[i], ID_SWITCHES[j], ID_SWITCHES[k], INSTRUCOES[0][1], ID_RACKS[a]);
                        if (0 == strcmp(instrucaotxt, mensagem)){
                            instrucao.id = 0;
                            instrucao.rack = ID_RACKS[a];
                            instrucao.switches[0] = ID_SWITCHES[i];
                            instrucao.switches[1] = ID_SWITCHES[j];
                            instrucao.switches[2] = ID_SWITCHES[k];
                            instrucao.qtdswitches = 3;
                            instrucao.find = true;
                            return instrucao;
                        }
                    }
                }
            }
        }
    }

    printf("\n");
    // rm 1 sw
    for (int a = 0; a < QTDRACKS; a++){
        for (int i = 0; i < QTDSW; i++){
            memset(instrucaotxt, 0, BUFSZ);
            sprintf(instrucaotxt, "%s %s %s %s\n", INSTRUCOES[1][0], ID_SWITCHES[i], INSTRUCOES[1][1], ID_RACKS[a]);
            if (0 == strcmp(instrucaotxt, mensagem)){
                instrucao.id = 1;
                instrucao.rack = ID_RACKS[a];
                instrucao.switches[0] = ID_SWITCHES[i];
                instrucao.qtdswitches = 1;
                instrucao.find = true;
                return instrucao;
            }
        }
    }

    printf("\n");
    // get 1 sw
    for (int a = 0; a < QTDRACKS; a++){
        for (int i = 0; i < QTDSW; i++){
            memset(instrucaotxt, 0, BUFSZ);
            sprintf(instrucaotxt, "%s %s %s %s\n", INSTRUCOES[2][0], ID_SWITCHES[i], INSTRUCOES[2][1], ID_RACKS[a]);
            if (0 == strcmp(instrucaotxt, mensagem)){
                instrucao.id = 2;
                instrucao.rack = ID_RACKS[a];
                instrucao.switches[0] = ID_SWITCHES[i];
                instrucao.qtdswitches = 1;
                instrucao.find = true;
                return instrucao;
            }
        }
    }
    // get 2 sw
    for (int a = 0; a < QTDRACKS; a++){
        for (int i = 0; i < QTDSW; i++){
            for (int j = 0; j < QTDSW; j++){
                if (i != j){
                    memset(instrucaotxt, 0, BUFSZ);
                    sprintf(instrucaotxt, "%s %s %s %s %s\n", INSTRUCOES[2][0], ID_SWITCHES[i], ID_SWITCHES[j], INSTRUCOES[2][1], ID_RACKS[a]);
                    if (0 == strcmp(instrucaotxt, mensagem)){
                        instrucao.id = 2;
                        instrucao.rack = ID_RACKS[a];
                        instrucao.switches[0] = ID_SWITCHES[i];
                        instrucao.switches[1] = ID_SWITCHES[j];
                        instrucao.qtdswitches = 2;
                        instrucao.find = true;
                        return instrucao;
                    }
                }
            }
        }
    }

    printf("\n");
    // ls
    for (int a = 0; a < QTDRACKS; a++){
        memset(instrucaotxt, 0, BUFSZ);
        sprintf(instrucaotxt, "%s %s\n", INSTRUCOES[3][0], ID_RACKS[a]);
        if (0 == strcmp(instrucaotxt, mensagem)){
            instrucao.id = 3;
            instrucao.rack = ID_RACKS[a];
            instrucao.qtdswitches = 0;
            instrucao.find = true;
            return instrucao;
        }
    }

    instrucao.find = false;
    return instrucao;
}

char* handleErrorAddExceeded(const char *mensagem){
    char instrucaotxt[BUFSZ];
    // add 4 sw
    for (int a = 0; a < QTDRACKS; a++){
        for (int i = 0; i < QTDSW; i++){
            for (int j = 0; j < QTDSW; j++){
                for (int k = 0; k < QTDSW; k++){
                    for (int z = 0; z < QTDSW; z++){
                        if ((i != j) && (i != k) && (i != z) && (j != k) && (j != z) && (k != z)){
                            memset(instrucaotxt, 0, BUFSZ);
                            sprintf(instrucaotxt, "%s %s %s %s %s %s %s\n", INSTRUCOES[0][0], ID_SWITCHES[i], ID_SWITCHES[j], ID_SWITCHES[k], ID_SWITCHES[z], INSTRUCOES[0][1], ID_RACKS[a]);
                            if (0 == strcmp(instrucaotxt, mensagem)){
                                return "error rack limit exceeded\n";
                            }
                        }
                    }
                }
            }
        }
    }
    return NULL;
}

char* handleErrorMessage(char* data){
    char* sub;
    
    //cont_param e um contador para contar quantos parametros foram passados (1, 2 ou 3)
    //rack e um inteiro para identificar o rack passado (1, 2, 3 ou 4)
    int cont_param = 0, rack;

    if((sub = strtok(data, " "))!= NULL){
        //Caso o comando seja add:
        if(strcmp(sub, "add") == 0){
            sub = strtok(NULL, " ");
            if(sub == NULL){
                return NULL;
            }
            if(strcmp(sub, "sw") != 0){
                return NULL;
            }
            sub = strtok(NULL, " ");
            if(sub == NULL){
                return NULL;
            }
            while((strcmp(sub, "in"))!= 0 && strlen(sub) == 2 && cont_param < 3){
                int i = atoi(sub);
                if(i>4 || i<1){
                    return "error switch type unknown\n";
                }
                cont_param++; 
                sub = strtok(NULL, " ");
                if(sub == NULL){
                    return NULL;
                }
            }
            sub = strtok(NULL, " ");
            if(sub == NULL){
                return NULL;
            }
            rack = atoi(sub);
            if(rack > 4 || rack < 1){
                return "error rack doesn't exist\n";
            }
        }

        //Caso o comando seja rm:
        else if(strcmp(sub, "rm") == 0){
            sub = strtok(NULL, " ");
            if(sub == NULL){
                return NULL;
            }
            if(strcmp(sub, "sw") != 0){
                return NULL;
            }
            sub = strtok(NULL, " ");
            if(sub == NULL){
                return NULL;
            }
            while((strcmp(sub, "in"))!= 0 && strlen(sub) == 2 && cont_param < 1){
                int i = atoi(sub);
                if(i>4 || i<1){
                    return "error switch type unknown\n";
                }
                cont_param++; 
                sub = strtok(NULL, " ");
                if(sub == NULL){
                    return NULL;
                }
            }
            sub = strtok(NULL, " ");
            if(sub == NULL){
                return NULL;
            }
            rack = atoi(sub);
            if(rack > 4 || rack < 1){
                return "error rack doesn't exist\n";
            }
        }
        
        //caso o comando seja get:
        else if(strcmp(sub, "get") == 0){
            sub = strtok(NULL, " ");
            if(sub == NULL){
                return NULL;
            }
            if(strcmp(sub, "sw") != 0){
                return NULL;
            }
            sub = strtok(NULL, " ");
            if(sub == NULL){
                return NULL;
            }
            while((strcmp(sub, "in"))!= 0 && strlen(sub) == 2 && cont_param < 2){
                int i = atoi(sub);
                if(i>4 || i<1){
                    return "error switch type unknown\n";
                }
                cont_param++; 
                sub = strtok(NULL, " ");
                if(sub == NULL){
                    return NULL;
                }
            }
            sub = strtok(NULL, " ");
            if(sub == NULL){
                return NULL;
            }
            rack = atoi(sub);
            if(rack > 4 || rack < 1){
                return "error rack doesn't exist\n";
            }
        }

        //caso o comando seja list:
        else if(strcmp(sub, "ls") == 0){
            sub = strtok(NULL, " ");
            if(sub == NULL){
                return NULL;
            }
            rack = atoi(sub);
            if(rack > 4 || rack < 1){
                return "error rack doesn't exist\n";
            }
        }
        else{
            return NULL;
        }
    }
    return NULL;
}

void execInstAdd(Rack *r, Inst in){
    r->erro = false;

    // Verifica se ha quantidade de espaço disponivel na rack
    if (in.qtdswitches > (3 - r->qtdswitches)){
        sprintf(r->msg, "error rack limit exceeded\n");
        r->erro = true;
        return;
    }
    
    // Verifica se já foi adicioando
    for(int i = 0; i < in.qtdswitches; i++){
        if(r->switches[atoi(in.switches[i]) - 1] == true){
            snprintf(r->msg, sizeof(r->msg), "error switch %s already installed in %s\n", in.switches[i], r->id);
            return;
        } 
    }

    // Se deu tudo certo
    memset(r->msg, 0, BUFSZ);
    sprintf(r->msg, "switch");
    for (int i = 0; i < in.qtdswitches; i++){
        char temp[BUFSZ];
        sprintf(temp, " %s", in.switches[i]);
        strcat(r->msg, temp);
        r->switches[atoi(in.switches[i]) - 1] = true;
        r->qtdswitches++;    
    }
    strcat(r->msg, " installed\n");
    return;
}
void execInstRm(Rack *r, Inst in){
    r->erro = false;
    // Verifica se existe no rack

    if(r->switches[atoi(in.switches[0]) - 1] == true){
        sprintf(r->msg, "switch %s removed from %s\n", in.switches[0], r->id);
        r->switches[atoi(in.switches[0]) - 1] = false;
        r->qtdswitches--;
        return;
    } 

    sprintf(r->msg, "error switch doesn't exist\n");
    r->erro = true;
}
int genRandom(int qtd, int choose){
    while (true){
        for(int i = 0 ; i < qtd ; i++) {
            if (i == choose){
                return rand() % 5000;
            }
        }
    }
}
void execInstGet(Rack *r, Inst in){
    r->erro = false;
    memset(r->msg, 0, BUFSZ);
    // Verifica se existe no rack
    for(int i = 0; i < in.qtdswitches; i++){
        if(r->switches[atoi(in.switches[i]) - 1] == true){
            char temp[BUFSZ];
            sprintf(temp, "%d kpbs ", genRandom(in.qtdswitches, i));
            strcat(r->msg, temp);
        } 
    }
    strcat(r->msg, "\n");

    if (strcmp(r->msg, "\n") == 0){
        sprintf(r->msg, "error switch doesn't exist\n");
        r->erro = true;
    }
}
void execInstLs(Rack *r, Inst in){
    r->erro = false;

    if (r->qtdswitches == 0){
        sprintf(r->msg, "empty rack\n");
        return;
    }

    memset(r->msg, 0, BUFSZ);
    for (int i = 0; i < 4; i++){
        if (r->switches[i] == true){
            char temp[BUFSZ];
            sprintf(temp, "0%d ", i+1);
            strcat(r->msg, temp);
        }
    }
    strcat(r->msg, "\n");
}

void inicializaRack(Rack *r){
    // strcpy(r->id, id);
    r->erro = false;
    // memset(r->msg, 0, BUFSZ);
    r->qtdswitches = 0;
}

void usage(int argc, char **argv){
    printf("usage: %s <v4|v6> <server port>\n", argv[0]);
    printf("example: %s v4 51511\n", argv[0]);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv){
    srand(time(NULL)); 

    Rack rack1;
    Rack rack2;
    Rack rack3;
    Rack rack4;
    
    rack1.id = "01";
    rack2.id = "02";
    rack3.id = "03";
    rack4.id = "04";

    memset(rack1.msg, 0, BUFSZ);
    memset(rack2.msg, 0, BUFSZ);
    memset(rack3.msg, 0, BUFSZ);
    memset(rack4.msg, 0, BUFSZ);
    
    inicializaRack(&rack1);
    inicializaRack(&rack2);
    inicializaRack(&rack3);
    inicializaRack(&rack4);

    Rack DataCenter[4];
    DataCenter[0] = rack1;
    DataCenter[1] = rack2;
    DataCenter[2] = rack3;
    DataCenter[3] = rack4;

    for (int i = 0; i < 4;  i++){
        for (int j = 0; j < 3; j++){
            DataCenter[i].switches[j] = false;
        }
    }

    //** -------------------------------------------------------- **//


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
        //printf("[log] connection from %s\n", caddrstr);
        
        while (1){
        /* Ler a mensagem do cliente */ 
            char buf[BUFSZ];
            // Zerar a memória
            memset(buf, 0, BUFSZ);
            // Recebe a mensagem do cliente
            // size_t count = recv(csock, buf, BUFSZ, 0);
            size_t count = read(csock, buf, BUFSIZ - 1);
            // Imprime mensagem do cliente
            printf("[msg] %s, %d bytes: %s\n", caddrstr, (int)count, buf);
        /**/ 

        /* Analisa a instrução recebida do cliente */ 
            Inst instrucao = analisamsg(buf);
            // imprimeInstrucao(instrucao);
            if (instrucao.find == true){
                switch (instrucao.id){
                    case 0:
                        execInstAdd(&DataCenter[atoi(instrucao.rack) - 1], instrucao);
                        break;
                    case 1:
                        execInstRm(&DataCenter[atoi(instrucao.rack) - 1], instrucao);
                        break;
                    case 2:
                        execInstGet(&DataCenter[atoi(instrucao.rack) - 1], instrucao);
                        break;
                    case 3:
                        execInstLs(&DataCenter[atoi(instrucao.rack) - 1], instrucao);
                        break;
                    default:
                        break;
                }
                strcpy(buf, DataCenter[atoi(instrucao.rack) - 1].msg);
                DataCenter[atoi(instrucao.rack) - 1].erro = false;
                /* Mandar resposta para o cliente. Nesse caso, manda o ip que conectou */ 
                send(csock, buf, strlen(buf) + 1, 0);
            }
            else if(handleErrorAddExceeded(buf) != NULL){
                strcpy(buf, handleErrorAddExceeded(buf));
                /* Mandar resposta para o cliente. Nesse caso, manda a mensagem de sucesso */ 
                send(csock, buf, strlen(buf) + 1, 0);
            }
            else {
                char* res = handleErrorMessage(buf);
                if(res == NULL){
                    strcpy(buf, "exit\n");
                    /* Mandar resposta para o cliente. Nesse caso, manda erro de mensagem não encontrada */ 
                    send(csock, buf, strlen(buf) + 1, 0);
                    close(csock);
                    return 0;
                }
                else{
                    strcpy(buf, res);
                    /* Mandar resposta para o cliente. Nesse caso, manda mensagem de erro diversa */ 
                    send(csock, buf, strlen(buf) + 1, 0);
                }
            }
        }
    }
    exit(EXIT_SUCCESS);
}