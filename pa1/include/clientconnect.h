// clientconnect.h
#include <string>

using namespace std;

#ifndef CLIENTCONNECT_H_
#define CLIENTCONNECT_H_

extern int server;

int connect_to_host(const char *server_ip, const char *server_port, const char *client_port);

#endif