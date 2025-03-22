#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <strings.h>
#include <string.h>
#include <string>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netdb.h>
#include <sstream>
#include <algorithm>
#include <vector>
#include <netinet/in.h>
#include <unistd.h>
#include <map>
#include "../include/global.h"
#include "../include/logger.h"
#include "../include/ip.h"
#include "../include/list_data.h"

using namespace std;

#define INPUT_SIZE 1024

bool isExist(const char* ip, vector<ListData> clients);
bool isBlock(const char* ip, vector<Block> clientBlock);
bool isSort(ListData a, ListData b);
bool isValidIPv4(const string &ip);
bool isLoggedInServer(ListData client);
ListData findListData(const char* ip, vector<ListData> clients);
void blockList(const char* input_char ,const char* ip, ListData client, vector<ListData> connectedClients);


bool isLoggedInServer(ListData client){
    if (client.ip[0] == '\0') {
        return false;
    }
    if(client.isLoggedIn){
        return true;
    }
    return false;
}
bool isExist(const char* ip, vector<ListData> clients){
    for(ListData &client: clients){
        // std::cout << "client list : " <<client.ip <<"\n"<< std::endl;
        if (strcmp(client.ip, ip) == 0) {
            // std::cout << client.ip <<"\n"<< std::endl;
            return true;
        }
    }
    return false;
}
bool isBlock(const char* ip, vector<Block> clientBlock){
    for(Block blockClient : clientBlock){
        // std::cout << "client block list : "<<blockClient.IP <<"\n"<< std::endl;
        if (strcmp(blockClient.IP, ip) == 0) {
            // std::cout << blockClient.IP <<"/n"<< std::endl;
            return true;
        }
    }
    return false;
}
ListData findListData(const char* ip, vector<ListData> clients) {
    ListData temp;
    for (size_t i = 0; i < clients.size(); ++i) {
        if (strcmp(clients[i].ip, ip) == 0) {
            temp = clients[i];
            return temp;
        }
    }
    return temp;
}
void blockList(const char* input_char ,const char* ip,  ListData client, vector<ListData> connectedClients){
     
        std::vector<ListData> blockedClients;
        for (const auto& block : client.BlockList) {
            ListData blockedClient = findListData(block.IP,connectedClients);
            if (blockedClient.ip[0] != '\0') {
                blockedClients.push_back(blockedClient);
            }
        }
        sort(blockedClients.begin(), blockedClients.end(), isSort);
        cse4589_print_and_log("[%s:SUCCESS]\n",  input_char);
        int i=1;
        for (const ListData &blockClient : blockedClients) {
            cse4589_print_and_log("%-5d%-35s%-20s%-8d\n", i++, blockClient.hostname, blockClient.ip, htons(blockClient.port));
        }
        cse4589_print_and_log("[%s:END]\n",  input_char);   
}
bool isSort(ListData a, ListData b)
{
    return a.port < b.port;
}
bool isValidIPv4(const string &ip) {
    struct sockaddr_in sa;
    return inet_pton(AF_INET, ip.c_str(), &(sa.sin_addr)) != 0;
}