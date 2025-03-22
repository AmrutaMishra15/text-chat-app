#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <strings.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include "../include/global.h"
#include "../include/logger.h"
#include "../include/ip.h"

using namespace std;

std::string getIP(const char *command)
{

	int dummysockfd;
	struct sockaddr_in serverAddr;
	struct sockaddr_in localAddr;
	socklen_t addrLen;

	// Dummy UDP socket
	dummysockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (dummysockfd < 0)
	{
		cerr << "Error creating socket" << endl;
		return "";
	}

	// Server Address
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(53);
	serverAddr.sin_addr.s_addr = inet_addr("8.8.8.8");

	// Connect the socket to the server
	if (connect(dummysockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
	{
		cerr << "Error connecting to server" << endl;
		close(dummysockfd);
		return "";
	}

	// Get the local IP address
	addrLen = sizeof(localAddr); // Move addrLen declaration here
	if (getsockname(dummysockfd, (struct sockaddr *)&localAddr, &addrLen) < 0)
	{
		cerr << "Error getting local socket address" << endl;
		close(dummysockfd);
		cse4589_print_and_log("[%s:ERROR]\n", command);
		cse4589_print_and_log("[%s:END]\n", command);
		return "";
	}

	// cse4589_print_and_log("[%s:SUCCESS]\n", command);
	// cse4589_print_and_log("IP:%s\n", inet_ntoa(localAddr.sin_addr));
	// cse4589_print_and_log("[%s:END]\n", command);

	close(dummysockfd);
	return inet_ntoa(localAddr.sin_addr);
}