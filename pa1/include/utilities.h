#ifndef UTILITIES_H_
#define UTILITIES_H_

#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include "../include/list_data.h"

bool isExist(const char* ip, vector<ListData> clients);
bool isBlock(const char* ip, vector<Block> clientBlock);
bool isSort(ListData a, ListData b);
bool isValidIPv4(const string &ip);
bool isLoggedInServer(ListData client);
ListData findListData(const char* ip, vector<ListData> clients);
void blockList(const char* input_char ,const char* ip, ListData client, vector<ListData> connectedClients);

#endif
