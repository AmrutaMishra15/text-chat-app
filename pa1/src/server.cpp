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
#include <netinet/in.h>
#include <unistd.h>
#include <vector>
#include <sstream>
#include <algorithm>
#include <map>
#include "../include/global.h"
#include "../include/logger.h"
#include "../include/ip.h"
#include "../include/cmdinput.h"
#include "../include/list_data.h"
#include "../include/utilities.h"
using namespace std;

#define BACKLOG 5
#define STDIN 0
#define TRUE 1
#define CMD_SIZE 100
#define BUFFER_SIZE 1024

// struct ListData
// {
//     char ip[100];
//     char hostname[BUFFER_SIZE];
//     int socket;
//     int port;
// };
void send_msg(string src_ip, string dest_ip, string msg, bool isSend);
std::vector<ListData> connectedClients;
string clientList()
{
    std::ostringstream response;
    for (const auto &client : connectedClients)
    {
        response << client.hostname
                 << " " << client.ip
                 << " " << htons(client.port)
                 << "\n"; 
    }
    return response.str();
}

void server(char *port)
{

    int server_socket, head_socket, selret, sock_index, fdaccept = 0, caddr_len;
    struct sockaddr_in client_addr;
    struct addrinfo hints, *res;
    fd_set master_list, watch_list;
    // For STDIN
    //  char input[256];

    // Set up hints structure
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    // Fill up address structures NULL is to bind the socket to the address of the machine.
    if (getaddrinfo(NULL, port, &hints, &res) != 0)
        perror("getaddrinfo failed");

    /* Socket */
    server_socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (server_socket < 0)
        perror("Cannot create socket");

    /* Bind */
    if (bind(server_socket, res->ai_addr, res->ai_addrlen) < 0)
        perror("Bind failed");

    freeaddrinfo(res);

    /* Listen */
    if (listen(server_socket, BACKLOG) < 0)
        perror("Unable to listen on port");

    /* Zero select FD sets */
    FD_ZERO(&master_list);
    FD_ZERO(&watch_list);

    /* Register the listening socket */
    FD_SET(server_socket, &master_list);
    /* Register STDIN */
    FD_SET(STDIN, &master_list);

    head_socket = server_socket;

    while (TRUE)
    {
        printf("\n[PA1-Server@CSE489/589]$ ");
        fflush(stdout);
        memcpy(&watch_list, &master_list, sizeof(master_list));

        /* select() system call. This will BLOCK */
        selret = select(head_socket + 1, &watch_list, NULL, NULL, NULL);
        if (selret < 0)
            perror("select failed.");

        /* Check if we have sockets/STDIN to process */
        if (selret > 0)
        {
            /* Loop through socket descriptors to check which ones are ready */
            for (sock_index = 0; sock_index <= head_socket; sock_index += 1)
            {

                if (FD_ISSET(sock_index, &watch_list))
                {

                    /* Check if new command on STDIN */
                    if (sock_index == STDIN)
                    {
                        // std::cout << "\nI got: " << cmd << std::endl;

                        // // Process PA1 commands here ...

                        // printf("\n[PA1-Server@CSE489/589]$ ");
                        // fflush(stdout);
                        char input[256];
                        memset(input, '\0', 256);
                        
                        if (fgets(input, 256 - 1, stdin) != NULL) // Mind the newline character that will be written to msg
                            input[strcspn(input, "\n")] = '\0';

                        char command[100];
                        char msg[1024];
                        // sscanf(input, "%s", command);
                        sscanf(input, "%s %[^\n]", command, msg);
                        istringstream iss(input);
                        string cpp_command(command);
                        string input_char,ip;
                        iss >> input_char;
                        if (strcmp(command, "LIST") == 0)
                        {
                            // sort and print list
                            sort(connectedClients.begin(), connectedClients.end(), [](const ListData &a, const ListData &b)
                                 { return a.port < b.port; });

                            cse4589_print_and_log("[%s:SUCCESS]\n", input);
                            int i = 1;
                            for (const auto &client : connectedClients)
                            {
                                cse4589_print_and_log("%-5d%-35s%-20s%-8d\n", i++, client.hostname, client.ip, htons(client.port));
                            }
                            cse4589_print_and_log("[%s:END]\n", input);
                        } else if  (strcmp(command, "BLOCKED") == 0){
                            iss >> ip;
                            ListData client = findListData(ip.c_str(), connectedClients);
                            if (!isValidIPv4(ip)) {
                                cse4589_print_and_log("[%s:ERROR]\n", command);
                                cse4589_print_and_log("[%s:END]\n", command);
                            } else {
                                if (client.ip[0] == '\0') {
                                    cse4589_print_and_log("[%s:ERROR]\n", command);
                                    cse4589_print_and_log("[%s:END]\n", command);
                                }  else {
                                    blockList(command,ip.c_str(), client,connectedClients);
                                }
                            }
                        }else if  (strcmp(command, "STATISTICS") == 0){
                            
                            sort(connectedClients.begin(), connectedClients.end(), isSort);
                            cse4589_print_and_log("[%s:SUCCESS]\n", command);
                            int i =1;
                            for (auto& client : connectedClients)
                            {
                                char logStatus[50] = {0};
                                if(client.isLoggedIn){
                                    strcpy(logStatus, "logged-in");
                                } else {
                                    strcpy(logStatus, "logged-out");
                                }
                                cse4589_print_and_log("%-5d%-35s%-8d%-8d%-8s\n", i++, client.hostname, client.num_msg_sent, client.num_msg_rcv, logStatus);

                            }
                            cse4589_print_and_log("[%s:END]\n", command);
                        }  
                        else
                        {
                            cmdinput(input, port);
                        }
                    }

                    /* Check if new client is requesting connection */
                    else if (sock_index == server_socket)
                    {
                        caddr_len = sizeof(client_addr);
                        fdaccept = accept(server_socket, (struct sockaddr *)&client_addr, reinterpret_cast<socklen_t *>(&caddr_len));
                        if (fdaccept < 0)
                            perror("Accept failed.");

                        std::cout << "\nRemote Host connected!\n";

                        int client_port;
                        if (recv(fdaccept, &client_port, sizeof(client_port), 0) > 0)
                        {
                             ListData newClient;
                             strcpy(newClient.ip, inet_ntoa(client_addr.sin_addr));
                            // cout << newClient.port << htons(newClient.port);
                            if(!isExist(newClient.ip, connectedClients)){
                                client_port = ntohs(client_port);
                               
                                newClient.socket = fdaccept;
                                char host[1024];
                                getnameinfo((struct sockaddr *)&client_addr, sizeof(client_addr), host, sizeof(host), NULL, 0, 0);
                                strcpy(newClient.hostname, host);
                                newClient.port = client_port;
                                newClient.num_msg_sent = 0;
                                newClient.num_msg_rcv = 0;
                                newClient.isLoggedIn = true;
                                connectedClients.push_back(newClient);
                            } else {
                                 for (auto& client : connectedClients)
                                    {
                                        if(strcmp(client.ip,newClient.ip)==0)
                                        {
                                            client.isLoggedIn = true;
                                            newClient = client;
                                            break;
                                        }
                                    }
                            }
                            std::cout<< "LOGIN: " << newClient.socket << " " << newClient.ip;
                            // Send client info for all clients connected
                            string clientListStr = clientList();
                            send(newClient.socket, clientListStr.c_str(), clientListStr.size(), 0);

                            // Flush the respective buffer
                        }

                        /* Add to watched socket list */
                        FD_SET(fdaccept, &master_list);
                        if (fdaccept > head_socket)
                            head_socket = fdaccept;
                    }
                    /* Read from existing clients */
                    else
                    {
                        /* Initialize buffer to receive response */
                        char *buffer = new char[BUFFER_SIZE];
                        memset(buffer, '\0', BUFFER_SIZE);

                        if (recv(sock_index, buffer, BUFFER_SIZE, 0) <= 0)
                        {
                            close(sock_index);
                            cout << "Remote Host terminated connection!\n";

                            /* Remove from watched list */
                            for (auto itr = connectedClients.begin(); itr != connectedClients.end(); ++itr)
                            {
                                if (itr->socket == sock_index)
                                {
                                    itr = connectedClients.erase(itr);
                                    break;
                                }
                            }
                            FD_CLR(sock_index, &master_list);
                        }
                        else
                        {
                            // Process incoming data from existing clients here ...

                            // std::cout << "\nClient sent me: " << buffer << std::endl;
                            // std::cout << "ECHOing it back to the remote host ... ";

                            
                            char command[10];     
                            char msg[272]; 

                            // Extract command and message from buffer
                            if (sscanf(buffer, "%s %[^\n]", command, msg) >= 1){
                                if (strcmp(command, "refresh") == 0)
                                {
                                    // Handle REFRESH command
                                    std::string clientListStr = clientList(); // Generate updated client list
                                    if (send(sock_index, clientListStr.c_str(), clientListStr.size(), 0) < 0)
                                    {
                                        perror("Send failed");
                                    }
                                } else if (strcmp(command, "BLOCK") == 0)
                                {
                                    // Handle BLOCK command
                                    char blockedIP[50];
                                    char msgBuffer[256];
                                    Block temp;
                                    sscanf(msg, "%s %[^\n]", blockedIP, msgBuffer);
                                     for (auto& client : connectedClients)
                                    {
                                        if (client.socket == sock_index)
                                        {
                                            strcpy(temp.IP, blockedIP);
                                            client.BlockList.push_back(temp);
                                            break;
                                        }
                                    }
                                    
                                } else if (strcmp(command, "UNBLOCK") == 0)
                                {
                                    // Handle UNBLOCK command
                                    char unblockedIP[50];
                                    char msgBuffer[256];
                                    sscanf(msg, "%s %[^\n]", unblockedIP, msgBuffer);
                                    for (auto& client : connectedClients)
                                    {
                                        if (client.socket == sock_index)
                                        {
                                            for(int i=0;i<client.BlockList.size();i++){
                                                if(strcmp(client.BlockList[i].IP,unblockedIP)==0)
                                                {
                                                    client.BlockList.erase(client.BlockList.begin()+i);
                                                }
                                            }
                                            break;
                                        }
                                    }
                                    
                                } else if (strcmp(command, "LOGOUT") == 0)
                                {
                                    // Handle LOGOUT command
                
                                    for (auto& client : connectedClients)
                                    {
                                        if (client.socket == sock_index)
                                        {
                                            client.isLoggedIn = false;
                                            break;
                                        }
                                    }
                                    
                                } else if (strcmp(command, "LOGIN") == 0)
                                {
                                    // Handle LOGOUT command
                                    ListData newClient;
                                    for (auto& client : connectedClients)
                                    {
                                        if (client.socket == sock_index)
                                        {
                                            client.isLoggedIn = true;
                                            newClient = client;
                                            break;
                                        }
                                    }
                                    string clientListStr = clientList();
                                    send(newClient.socket, clientListStr.c_str(), clientListStr.size(), 0);
                                    
                                }
                                else if(strcmp(command, "SEND") == 0){

                                    // Dest_ip and msgContent
                                    char dest_ip[16]; 
                                    char msgBuffer[256]; 
                                    bool sendFlag = true;
                                    if (sscanf(msg, "%s %[^\n]", dest_ip, msgBuffer) != 2) {
                                        std::cerr << "Failed to parse message" << std::endl;
                                    }

                                    std::string destIP = dest_ip;
                                    std::string msgContent = msgBuffer;
                                    // Src_IP
                                    std::string src_ip;
                                    for (auto& client : connectedClients)
                                    {
                                        if (client.socket == sock_index)
                                        {
                                            src_ip = client.ip;
                                            client.num_msg_sent++;
                                            break;
                                        }
                                    }
                                    ListData destClient = findListData(destIP.c_str(),connectedClients);
                                    if(isBlock(src_ip.c_str(),destClient.BlockList)){
                                        sendFlag = false;
                                    }
                                    
                                    if(sendFlag && destClient.isLoggedIn){
                                        send_msg(src_ip, dest_ip, msgContent, true);
                                    }
                                    // Send to buffer if logged out
                                    else if(!destClient.isLoggedIn){
                                        msg_block temp;
                                        temp.src_IP = src_ip;
                                        temp.dest_IP = dest_ip;
                                        temp.msg_content = msgContent;
                                        temp.isSend = true;
                                        destClient.buffer.push_back(temp);
                                    }
                                    
                                }
                                else if(strcmp(command, "BROADCAST") == 0){

                                    // Src_IP
                                    std::string src_ip;
                                    std::string msgContent = msg;
                                    bool sendFlag = true;
                                    for (const auto& client : connectedClients)
                                    {
                                        if (client.socket == sock_index)
                                        {
                                            src_ip = client.ip;
                                            break;
                                        }
                                    }
                                    // cout<< src_ip << " " << msgContent << endl;

                                    for (auto &client : connectedClients)
                                    {
                                        if (std::string(client.ip) != src_ip)
                                        {
                                            if(isBlock(src_ip.c_str(),client.BlockList)){
                                                sendFlag = false;
                                            }
                                            // cout<< client.ip << " " << msgContent << endl;
                                            if(sendFlag && client.isLoggedIn){
                                                send_msg(src_ip, client.ip, msgContent, false);
                                            }
                                            // Send to buffer if logged out
                                            else if(!client.isLoggedIn){
                                                msg_block temp;
                                                temp.src_IP = src_ip;
                                                temp.dest_IP = client.ip;
                                                temp.msg_content = msgContent;
                                                temp.isSend = false;
                                                client.buffer.push_back(temp);
                                            }
                                        }
                                    }
                                }
                            
                            }
                            std::cout << std::flush;
                        }

                        delete[] buffer;
                    }
                }
            }
        }
    }
}

void send_msg(string src_ip, string dest_ip, string msg, bool isSend){

    // Find the destination client in the list of connected clients
    ListData dest_client;
    dest_client.buffer = std::vector<msg_block>();

    for (auto &client : connectedClients)
    {
        if (std::string(client.ip) == dest_ip)
        {
            dest_client = client;
            client.num_msg_rcv++;
            break;
        }
    }
    std::string msg_sent = src_ip + " " + msg;
    // Send if dest_client is in the list
    if (send(dest_client.socket, msg_sent.c_str(), msg_sent.length(), 0) < 0)
        {
            perror("Send failed");
        }
    else {
        cse4589_print_and_log("[%s:SUCCESS]\n", "RELAYED");
        if(isSend) {
            cse4589_print_and_log("msg from:%s, to:%s\n[msg]:%s\n", src_ip.c_str(), dest_ip.c_str(), msg.c_str());
        }
        else {
            cse4589_print_and_log("msg from:%s, to:%s\n[msg]:%s\n", src_ip.c_str(), "255.255.255.255", msg.c_str());
        }
        
        cse4589_print_and_log("[%s:END]\n", "RELAYED");
    }
}