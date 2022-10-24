#pragma once

#include <stdlib.h>
#include <arpa/inet.h>

void logExit(const char *msg);

int addrParse(const char *addrStr, const char *portStr, struct sockaddr_storage *storage);

void addrToStr(const struct sockaddr *addr, char *str, size_t strSize);

int ServerSockInit(const char *proto, const char *portStr,
                   struct sockaddr_storage *storage);

char* handleMessage(char* data, int* rack1, int* rack2);


