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
#include "../include/client.h"
#include "../include/clientconnect.h"
#include "../include/list_data.h"
#include "../include/utilities.h"
using namespace std;

#define YOUR_TEAM_NAME "ritikran-amrutami"
#define INPUT_SIZE 1024

void author();
void server_port(const char *argv);
void receiveClientDetails(int server, bool isExistCheck);
void refresh();
void list();
void block(const char* input_char, const char* ip);
void unblock(const char* input_char, const char* ip);

std::vector<ListData> client_lists;
std::vector<Block> client_block;
string serverIP;
int serverPort;
int port_int;
ListData clientDetails;

void cmdinput(const char *input, char *port)
{
    map<string, char> map_in;
    map_in["AUTHOR"] = 'a';
    map_in["IP"] = 'i';
    map_in["PORT"] = 'p';
    map_in["LIST"] = 'l';
    map_in["LOGIN"] = '@';
    map_in["REFRESH"] = 'r';
    map_in["EXIT"] = 'e';
    map_in["BLOCKED"] = 'b';

    char command[100];

    sscanf(input, "%s", command);
    istringstream iss(input);
    string cpp_command(command);
    string input_char,ip;
    iss >> input_char;
    switch (map_in[cpp_command])
    {
    case 'a':
        cse4589_print_and_log("[%s:SUCCESS]\n", command);
        author();
        cse4589_print_and_log("[%s:END]\n", command);
        break;

    case 'p':
        cse4589_print_and_log("[%s:SUCCESS]\n", command);
        cse4589_print_and_log("PORT:%d\n", atoi(port));
        cse4589_print_and_log("[%s:END]\n", command);
        break;

    case 'i':
        // Get IP address
        // cout<< "Before IP fn";
        ip = getIP(input_char.c_str());
        cse4589_print_and_log("[%s:SUCCESS]\n", input_char.c_str());
	    cse4589_print_and_log("IP:%s\n", ip.c_str());
	    cse4589_print_and_log("[%s:END]\n", input_char.c_str());
        break;


    case 'e':
        // cse4589_print_and_log("[%s:SUCCESS]\n", input_char.c_str());
        exit(-1);
        // cse4589_print_and_log("[%s:END]\n", input_char.c_str());
        break;

    // case 'b':
    //     iss >> ip;
    //     if (!isValidIPv4(ip) || !isExist(ip.c_str())) {
    //         cse4589_print_and_log("[%s:ERROR]\n", command);
    //         cse4589_print_and_log("[%s:END]\n", command);
    //     } else {
    //         blockList(input_char.c_str(),ip.c_str());
    //     }
    //     break;

    default:
        break;
    }
}
void cmdinputclient(const char *input, char **argv)
{
    /*Start Here*/
    map<string, char> map_in;
    map_in["AUTHOR"] = 'a';
    map_in["IP"] = 'i';
    map_in["PORT"] = 'p';
    map_in["LIST"] = 'l';
    map_in["LOGIN"] = '@';
    map_in["REFRESH"] = 'r';
    map_in["EXIT"] = 'e';
    map_in["SEND"] = 's';
    map_in["BROADCAST"] = 'c';
    map_in["BLOCK"] = 'b';
    map_in["UNBLOCK"] = 'u';
    map_in["LOGOUT"] = 'x';

    std::string command, ip, port, src_ip, dest_ip, msg_sent, msg;
    bool isDestIpInList = false;
    const char* msg_cstr;
    istringstream iss(input);
    string input_char;
    iss >> input_char;
    // Invalid command
    if (map_in.find(input_char.c_str()) == map_in.end())
    {
        cse4589_print_and_log("[ERROR] Invalid Command\n");
    }
    // Switch input to execute command
    switch (map_in[input_char])
    {
    case 'a':
        cse4589_print_and_log("[%s:SUCCESS]\n", input_char.c_str());
        author();
        cse4589_print_and_log("[%s:END]\n", input_char.c_str());
        break;
    case 'p':
        cse4589_print_and_log("[%s:SUCCESS]\n", input_char.c_str());
        server_port(argv[2]);
        cse4589_print_and_log("[%s:END]\n", input_char.c_str());
        break;
    case 'i':
        // Get IP address
        // cout<< "Before IP fn";
        ip = getIP(input_char.c_str());
        cse4589_print_and_log("[%s:SUCCESS]\n", input_char.c_str());
	    cse4589_print_and_log("IP:%s\n", ip.c_str());
	    cse4589_print_and_log("[%s:END]\n", input_char.c_str());
        break;
    case 'e':
        // cse4589_print_and_log("[%s:SUCCESS]\n", input_char.c_str());
        exit(-1);
        // cse4589_print_and_log("[%s:END]\n", input_char.c_str());
        break;
    case 'r':
        if(!isLoggedInServer(clientDetails)){
            break;
        }
        cse4589_print_and_log("[%s:SUCCESS]\n", input_char.c_str());
        refresh();
        cse4589_print_and_log("[%s:END]\n", input_char.c_str());
        break;
    case 'l':
        if(!isLoggedInServer(clientDetails)){
            break;
        }
        cse4589_print_and_log("[%s:SUCCESS]\n", input_char.c_str());
        list();
        cse4589_print_and_log("[%s:END]\n", input_char.c_str());
        break;
    case '@':
        iss >> ip >> port;
        port_int = atoi(port.c_str());
        if (iss.fail() || ip.empty() || port_int <= 0 || port_int > 65535 || isValidIPv4(ip) == false)
        {
            cse4589_print_and_log("[%s:ERROR]\n", input_char.c_str());
            cse4589_print_and_log("[%s:END]\n", input_char.c_str());
        }
        else
        {
            bool isExistCheck = isExist(clientDetails.ip, client_lists);
            cse4589_print_and_log("[%s:SUCCESS]\n", input_char.c_str());
            if(!isExistCheck){
                server = connect_to_host(ip.c_str(), port.c_str(), argv[2]);
            } else {
                server = clientDetails.server;
            }
            std::cout<<server;
            const std::string request_str = "LOGIN " + ip + " " + port.c_str() + "\n";
            if (send(server, request_str.c_str(), request_str.size(), 0) == request_str.size())
                fflush(stdout);
            receiveClientDetails(server, isExistCheck);
    
            cse4589_print_and_log("[%s:END]\n", input_char.c_str());
        }
        break;
    case 's': 

        src_ip = getIP(input_char.c_str());
        iss >> dest_ip;
        for (const auto& client : client_lists) {
            if (strcmp(client.ip, dest_ip.c_str()) == 0) {
                isDestIpInList = true; // Found dest_ip in the list
            }
        }
        // Exceptions
        if(src_ip == dest_ip || isValidIPv4(ip) || isDestIpInList == false){
            cse4589_print_and_log("[%s:ERROR]\n", input_char.c_str());
            cse4589_print_and_log("[%s:END]\n", input_char.c_str());
        }
        else{
            // Extract message (Max 256 bytes)
            std::getline(iss, msg);
            msg_sent = "SEND ";
            msg_sent += dest_ip;
            msg_sent += msg;
            msg_cstr = msg_sent.c_str();
            // cout<< msg_cstr;
            // Send the message using send()
            if (send(server, msg_cstr, strlen(msg_cstr), 0) < 0)
            {
                perror("Send failed");
            }
            // else {
            //     cout<< "Success";
            // }
        }
        break;
    case 'c':
            std::getline(iss, msg);
            // Send the message using send()
            msg_sent = "BROADCAST ";
            msg_sent += msg;
            msg_cstr = msg_sent.c_str();
            // cout << msg_cstr;
            if (send(server, msg_cstr, strlen(msg_cstr), 0) < 0)
            {
                perror("Send failed");
            }
        break;
    case 'b':
        iss >> ip;
        if(!isLoggedInServer(clientDetails)){
            break;
        }
        if (!isValidIPv4(ip)) {
            cse4589_print_and_log("[%s:ERROR]\n", input_char.c_str());
            cse4589_print_and_log("[%s:END]\n", input_char.c_str());
        } else {
            block(input_char.c_str(),ip.c_str());
        }
        break;
    case 'u':
        iss >> ip;
        if(!isLoggedInServer(clientDetails)){
            break;
        }
        if (!isValidIPv4(ip)) {
            cse4589_print_and_log("[%s:ERROR]\n", input_char.c_str());
            cse4589_print_and_log("[%s:END]\n", input_char.c_str());
        } else {
            unblock(input_char.c_str(),ip.c_str());
        }
        break;
    case 'x':
        if(!isLoggedInServer(clientDetails)){
            break;
        }
        clientDetails.isLoggedIn = false;
        send(server, input_char.c_str(), strlen(input_char.c_str()), 0);
        server = 0;
        cse4589_print_and_log("[%s:SUCCESS]\n",  input_char.c_str());
        cse4589_print_and_log("[%s:END]\n",  input_char.c_str());
        break;
    default:
        break;
    }
}

void block(const char* input_char ,const char* ip){
    
    if (!isExist(ip, client_lists) || isBlock(ip,client_block)) {
        cse4589_print_and_log("[%s:ERROR]\n", input_char);
        cse4589_print_and_log("[%s:END]\n", input_char);
    } else {
        struct Block tempBlock;
        strncpy(tempBlock.IP, ip, 49);
        tempBlock.IP[49] = '\0';
        client_block.push_back(tempBlock);

        cse4589_print_and_log("[%s:SUCCESS]\n",  input_char);
        const std::string request_str = std::string("BLOCK ") + ip +"\n";
        send(server, request_str.c_str(), request_str.size(), 0);
        cse4589_print_and_log("[%s:END]\n",  input_char);
    }
}

void unblock(const char* input_char ,const char* ip){
    if (!isExist(ip, client_lists) || !isBlock(ip,client_block)) {
        cse4589_print_and_log("[%s:ERROR]\n", input_char);
        cse4589_print_and_log("[%s:END]\n", input_char);
    } else {
        for(int i=0;i<client_block.size();i++){
			if(strcmp(client_block[i].IP,ip)==0)
			{
				client_block.erase(client_block.begin()+i);
            }
        }

        cse4589_print_and_log("[%s:SUCCESS]\n",  input_char);
        const std::string request_str = std::string("UNBLOCK ") + ip +"\n";
        send(server, request_str.c_str(), request_str.size(), 0);
        cse4589_print_and_log("[%s:END]\n",  input_char);
    }
}
void receiveClientDetails(int server, bool isExistCheck)
{
    char input[INPUT_SIZE];
    memset(input, '\0', INPUT_SIZE);
    std::cout<<"inside receiveClientDetails" << std::endl;
    if (recv(server, input, INPUT_SIZE, 0) >= 0)
    {
        std::cout<<"inside receiveClientDetails if" << std::endl;
        char *token = strtok(input, "\n");
        std::cout<<"after token" << token <<std::endl;
        // while (token != NULL)
        // {
        //     ListData client;
        //     sscanf(token, "%s %s %d", client.hostname, client.ip, &client.port);
        //     if(isExistCheck){
        //         for (auto& clientArr : client_lists) {
        //             if (strcmp(clientArr.ip, client.ip) == 0) {
        //                 clientArr.isLoggedIn = true;
        //             }
        //         }
        //     } else {
        //         client.isLoggedIn = true;
        //         client.server = server;
        //         clientDetails = client;
        //         client_lists.push_back(client);
        //     }
        //     token = strtok(NULL, "\n");
        // }

        while (token != NULL)
        {
            ListData client;
            sscanf(token, "%s %s %d", client.hostname, client.ip, &client.port);
            client.isLoggedIn = true;
            client.server = server;
            clientDetails = client;
            client_lists.push_back(client);
            token = strtok(NULL, "\n");
        }
    }
}
void author()
{
    cse4589_print_and_log("I, %s, have read and understood the course academic integrity policy.\n", YOUR_TEAM_NAME);
}
void server_port(const char *argv)
{
    int port = atoi(argv);
    cse4589_print_and_log("PORT:%d\n", port);
}
void refresh()
{
    std::cout<< server <<std::endl;
    client_lists.clear();
    if (send(server, "refresh", strlen("refresh"), 0) < 0)
    {
        perror("Send failed");
        return;
    }
    receiveClientDetails(server,false);
}

void list()
{
    if (!client_lists.empty())
    {
        int i = 1;
        // Sort the list data based on port number
        sort(client_lists.begin(), client_lists.end(), isSort);
        for (const ListData &client : client_lists)
        {
            cse4589_print_and_log("%-5d%-35s%-20s%-8d\n", i++, client.hostname, client.ip, client.port);
        }
    }
}

