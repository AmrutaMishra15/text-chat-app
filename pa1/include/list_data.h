#ifndef LIST_DATA_H_
#define LIST_DATA_H_

#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

struct Block {
    char IP[50];
};

struct msg_block {
    std::string src_IP;
    std::string dest_IP;
    std::string msg_content;
    bool isSend = false;
};

struct ListData {
    char ip[100];
    char hostname[1024];
    int server;
    int socket;
    int port;
    std::vector<Block> BlockList;
    bool isConnected;
    bool isLoggedIn;
    int num_msg_sent;
    int num_msg_rcv;
    std::vector<msg_block> buffer;
};

#endif