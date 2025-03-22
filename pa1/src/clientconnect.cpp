#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <sys/socket.h>
#include <strings.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netdb.h>
#include "../include/cmdinput.h"
#include "../include/clientconnect.h"

using namespace std;

int connect_to_host(const char *server_ip, const char *server_port, const char *client_port)
{
    int fdsocket;
    struct addrinfo hints, *res, *client_addr;

    // cout << "Server IP: " << server_ip << endl;
    // cout << "Server Port: " << server_port << endl;
    // cout << "Client Port: " << client_port << endl;

    /* Set up hints structure */
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    /* Fill up address structures for client*/
    if (getaddrinfo(NULL, client_port, &hints, &client_addr) != 0)
        perror("getaddrinfo failed for client");

    /* Fill up address structures for server*/
    if (getaddrinfo(server_ip, server_port, &hints, &res) != 0)
        perror("getaddrinfo failed for server");

    /* Socket */
    fdsocket = socket(client_addr->ai_family, client_addr->ai_socktype, client_addr->ai_protocol);
    if (fdsocket < 0)
        perror("Failed to create socket");

    /* Bind Socket to client port*/
    // if (bind(fdsocket, client_addr->ai_addr, client_addr->ai_addrlen) < 0)
    //     perror("Bind failed for client");

    /* Connect */
    if (connect(fdsocket, res->ai_addr, res->ai_addrlen) < 0)
        perror("Connect failed");

    int client_port_num = atoi(client_port);
    if (send(fdsocket, &client_port_num, sizeof(client_port_num), 0) < 0)
        perror("Send failed");

    freeaddrinfo(res);
    freeaddrinfo(client_addr);

    return fdsocket;
}