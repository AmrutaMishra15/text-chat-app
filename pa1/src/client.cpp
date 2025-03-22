/**
 * @client
 * @author  Ritik Ranjan <ritikran@buffalo.edu>, Amruta Mishra<amrutami@buffalo.edu>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * This file contains the client.
 */
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <sys/socket.h>
#include <strings.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netdb.h>
#include "../include/global.h"
#include "../include/logger.h"
#include "../include/cmdinput.h"

using namespace std;

#define TRUE 1
#define MSG_SIZE 280
#define STDIN 0

int server = -1;
/**
 * main client function
 *
 */
int client(int argc, char **argv)
{
	char input[MSG_SIZE];
	char msg[MSG_SIZE];
	// Use select to monitor stdin and the client socket
    	fd_set read_fds;
    	int max_fd = (server > STDIN) ? server : STDIN;
		int selret, bytes_received;

	while (TRUE)
	{
		printf("\n[PA1-Client@CSE489/589]$ ");
		fflush(stdout);

        FD_ZERO(&read_fds);
        FD_SET(STDIN, &read_fds); 
        FD_SET(server, &read_fds); 
		max_fd = (server > STDIN) ? server : STDIN;

        // Wait for activity on any of the monitored file descriptors
		selret = select(max_fd + 1, &read_fds, NULL, NULL, NULL);
        if (selret < 0)
            continue;

		if (selret > 0)
        {
            // Check if there's data to read from stdin
        	if (FD_ISSET(STDIN, &read_fds)) {
				memset(input, '\0', MSG_SIZE);
				if (fgets(input, MSG_SIZE - 1, stdin) != NULL) // Mind the newline character that will be written to msg
					input[strcspn(input, "\n")] = '\0';
				cmdinputclient(input, argv);
        	}
	
        	// Check if there's data to read from the client socket
        	else if (FD_ISSET(server, &read_fds)) {
    			memset(msg, '\0', MSG_SIZE);

				bytes_received = recv(server, msg, MSG_SIZE, 0);
        	    if (bytes_received >= 0) {
					// std::cout << msg;
					// Send passed
					char client_ip[16]; 
    				char msg_content[256]; 
					if (sscanf(msg, "%15s %[^\n]", client_ip, msg_content) == 2) {
        		    char command_str[] = "RECEIVED";
					
        		    cse4589_print_and_log("[%s:SUCCESS]\n", command_str);
        		    cse4589_print_and_log("msg from:%s\n[msg]:%s\n", client_ip, msg_content);
        		    cse4589_print_and_log("[%s:END]\n", command_str);
        			}
				}else if (bytes_received == 0) {
        			// Connection closed
        			printf("Client closed connection\n");
    			} else {
					perror("recv");
				}
        	}	

		}
    
	}
}
