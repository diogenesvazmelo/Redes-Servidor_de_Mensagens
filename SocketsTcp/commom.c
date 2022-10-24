#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <string.h>

void logExit(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

int addrParse(const char *addrStr, const char *portStr, struct sockaddr_storage *storage)
{
    if (addrStr == NULL || portStr == NULL)
    {
        return -1;
    }
    uint16_t port = (uint16_t)atoi(portStr);
    if (port == 0)
        return -1;
    port = htons(port);

    struct in_addr inaddr4;
    if (inet_pton(AF_INET, addrStr, &inaddr4.s_addr))
    {
        struct sockaddr_in *addr4 = (struct sockaddr_in *)storage;
        addr4->sin_family = AF_INET;
        addr4->sin_port = port;
        addr4->sin_addr = inaddr4;
        return 0;
    }

    struct in6_addr inaddr6;
    if (inet_pton(AF_INET6, addrStr, &inaddr6))
    {
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)storage;
        addr6->sin6_family = AF_INET6;
        addr6->sin6_port = port;
        memcpy(&(addr6->sin6_addr), &inaddr6, sizeof(inaddr6));
        return 0;
    }

    return -1;
}

void addrToStr(const struct sockaddr *addr, char *str, size_t strSize)
{
    int version;
    char addrStr[INET6_ADDRSTRLEN + 1] = "";
    uint16_t port;
    if (addr->sa_family == AF_INET)
    {
        struct sockaddr_in *addr4 = (struct sockaddr_in *)addr;
        version = 4;
        if (!inet_ntop(AF_INET, &(addr4->sin_addr), addrStr, INET_ADDRSTRLEN))
        {
            logExit("ntop");
        }
        port = ntohs(addr4->sin_port);
    }
    else if (addr->sa_family == AF_INET6)
    {
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)addr;
        version = 6;
        if (!inet_ntop(AF_INET6, &(addr6->sin6_addr), addrStr, INET6_ADDRSTRLEN))
        {
            logExit("ntop");
        }
        port = ntohs(addr6->sin6_port);
    }
    else
    {
        logExit("unknown protocol family.");
    }
    if (str)
    {
        snprintf(str, strSize, "IPv%d %s %hu", version, addrStr, port);
    }
}

int ServerSockInit(const char *proto, const char *portStr,
                   struct sockaddr_storage *storage)
{
    if (proto == NULL || portStr == NULL)
    {
        return -1;
    }
    uint16_t port = (uint16_t)atoi(portStr);
    if (port == 0){return -1;}
    port = htons(port);

    memset(storage, 0, sizeof(*storage));
    if (strcmp(proto, "v4") == 0)
    {
        struct sockaddr_in *addr4 = (struct sockaddr_in *)storage;
        addr4->sin_family = AF_INET;
        addr4->sin_port = port;
        addr4->sin_addr.s_addr = INADDR_ANY;
        return 0;
    }
    else if (strcmp(proto, "v6") == 0)
    {
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)storage;
        addr6->sin6_family = AF_INET6;
        addr6->sin6_port = port;
        addr6->sin6_addr = in6addr_any;
        return 0;
    }
    else
    {
        return -1;
    }
}


/*Funcao de tratamento de mensagens pelo servidor. Recebe como parâmetro uma string enviada pelo cliente
e a partir dos dados nela, realiza a acao necessaria. Retorna uma mensagem de resposta que deve ser enviada
ao cliente.*/
char* handleMessage(char* data, int* rack1, int* rack2){
    char* sub;
    char* res = (char*)malloc(100 * sizeof(char));
    memset(res, 0, 100);
    char buf[100];
    memset(buf, 0, 100);
    //cont_param e um contador para contar quantos parametros foram passados (1 ou 2)
    //rack e um inteiro para identificar o rack passado (1 ou 2)
    int cont_param = 0, rack;
    int to_add[2] = {-1, -1};
    if((sub = strtok(data, " "))!= NULL){
        //Caso o comando seja add:
        if(strcmp(sub, "add")== 0){
            sub = strtok(NULL, " ");
            if(sub == NULL){return NULL;}
            if(strcmp(sub, "sw") != 0){return NULL;}
            sub = strtok(NULL, " ");
            if(sub == NULL){return NULL;}
            while((strcmp(sub, "in"))!= 0 && strlen(sub) == 2 && cont_param < 2){
                int i = atoi(sub);
                if(i>4 || i<1){return "error switch type unknown\n";}
                to_add[cont_param] = i;
                cont_param++; sub = strtok(NULL, " ");
                if(sub == NULL){return NULL;}
            }
            sub = strtok(NULL, " ");
            if(sub == NULL){return NULL;}
            rack = atoi(sub);
            if(rack > 2 || rack < 1){return NULL;}
            for(int i=0 ; i<cont_param; i++){
                int flag = 0;
                if(rack == 1){
                    for(int j = 0; j<4; j++){
                        if(rack1[j] == 0){
                            flag = 1;
                            break;
                        }
                    }
                    if(!flag){return "error rack limit exceeded";}
                    if(rack1[to_add[i] - 1] == 0){
                        rack1[to_add[i] - 1] = 1;
                    }else{
                        return "error switch already installed";
                    }
                }
                else{
                    for(int j = 0; j<4; j++){
                        if(rack2[j] == 0){
                            flag = 1;
                            break;
                        }
                    }
                    if(!flag){return "error rack limit exceeded";}
                    if(rack2[to_add[i] - 1] == 0){
                        rack2[to_add[i] - 1] = 1;
                    }else{
                        return "error switch already installed";
                    }
                }
            }
            if(cont_param == 2){
                snprintf(buf, sizeof(buf), "switch 0%d 0%d installed in 0%d", to_add[0], to_add[1], rack);
            }
            else{
            	memset(res, 0, 100);
                snprintf(buf, sizeof(buf), "switch 0%d installed in 0%d", to_add[0], rack);
            }
            strcpy(res, buf);
            return res;
        }
        //Caso o comando seja rm:
        else if(strcmp(sub, "rm") == 0){
            sub = strtok(NULL, " ");
            if(sub == NULL){return NULL;}
            if(strcmp(sub, "sw") != 0){return NULL;}
            sub = strtok(NULL, " ");
            if(sub == NULL){return NULL;}
            while((strcmp(sub, "in"))!= 0 && strlen(sub) == 2 && cont_param < 2){
                int i = atoi(sub);
                if(i>4 || i<1){return "error switch type unknown\n";}
                to_add[cont_param] = i;
                cont_param++; sub = strtok(NULL, " ");
                if(sub == NULL){return NULL;}
            }
            sub = strtok(NULL, " ");
            if(sub == NULL){return NULL;}
            rack = atoi(sub);
            if(rack > 2 || rack < 1){return NULL;}
            for(int i=0; i<cont_param; i++){
                if(to_add[i] == -1){break;}
                if(rack == 1){
                    if(rack1[to_add[i] - 1] == 0){
                        return "error switch doesnt exist";
                    }else{
                        rack1[to_add[i] - 1] = 0;
                    }
                }
                else{
                    if(rack2[to_add[i]- 1] == 0){
                        return "error switch doesnt exist";
                    }else{
                        rack2[to_add[i] - 1] = 0;
                    }
                }
            }
            if(cont_param == 2){
                snprintf(buf, sizeof(buf), "switch 0%d 0%d removed from 0%d", to_add[0], to_add[1], rack);
            }
            else{
                snprintf(buf, sizeof(buf), "switch 0%d removed from 0%d", to_add[0], rack);
            }
            strcpy(res, buf);
            return res;

        }else if(strcmp(sub, "get") == 0){
            //caso o comando seja get:
            sub = strtok(NULL, " ");
            if(sub == NULL){return NULL;}
            if(strcmp(sub, "sw") != 0){return NULL;}
            sub = strtok(NULL, " ");
            if(sub == NULL){return NULL;}
            while((strcmp(sub, "in"))!= 0 && strlen(sub) == 2 && cont_param < 2){
                int i = atoi(sub);
                if(i>4 || i<1){return "error switch type unknown\n";}
                to_add[cont_param] = i;
                cont_param++; sub = strtok(NULL, " ");
                if(sub == NULL){return NULL;}
            }
            sub = strtok(NULL, " ");
            if(sub == NULL){return NULL;}
            rack = atoi(sub);
            if(rack > 2 || rack < 1){return NULL;}
            for(int i=0; i<cont_param; i++){
                if(to_add[i] == -1){break;}
                if(rack == 1){
                    if(rack1[to_add[i]- 1] == 0){
                        return "error switch doesnt exist";
                    }
                }
                else{
                    if(rack2[to_add[i]- 1] == 0){
                        return "error switch doesnt exist";
                    }
                }
            }
            if(cont_param == 2){
                snprintf(buf, sizeof(buf), "4300 Kbps 134Kbps");
            }
            else{
                snprintf(buf, sizeof(buf), "4300 Kbps");
            }
            strcpy(res, buf);
            return res;

        }else if(strcmp(sub, "ls") == 0){
            //caso o comando seja list:
            sub = strtok(NULL, " ");
            if(sub == NULL){return NULL;}
            rack = atoi(sub);
            int cont = 0;
            if(rack == 1){
                for(int i =0; i<4; i++){
                    if(rack1[i]){cont++;}
                }
            }else if(rack ==2){
                for(int i =0; i<4; i++){
                    if(rack2[i]){cont++;}
                }
            }
            else{return "invalid rack";}
            if(cont == 0){return "empty rack";}
            int* list = (int*)malloc(cont*sizeof(int));
            cont = 0;
            if(rack == 1){
                for(int i =0; i<4; i++){
                    if(rack1[i]){
                        list[cont] = i + 1;
                        cont++;
                    }
                }
            }else{
                for(int i =0; i<4; i++){
                    if(rack2[i]){
                        list[cont] = i + 1;
                        cont++;
                    }
                }
            }
            for(int i = 0; i < cont; i++){
                snprintf(buf, sizeof(buf), "0%d ", list[i]);
                strcat(res, buf);
            }
            return res;
        }else{
            return NULL;
        }
    }
    return NULL;
}

