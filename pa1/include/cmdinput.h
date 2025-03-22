// cmdinput.h
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include "../include/list_data.h"

using namespace std;

#ifndef CMDINPUT_H_
#define CMDINPUT_H_

void author();
void cmdinput(const char *input, char *port);
void cmdinputclient(const char *input, char **argv);

#endif