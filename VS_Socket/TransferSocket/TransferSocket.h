#pragma once

#include "resource.h"
#include <winsock2.h>
#include <string>
#include <cstring>
#include "SockThd.h"
#include<iostream>
#include<fstream>

bool ReadConnInfo(string* IP, string* Port);

string OpenFileDialog();

//using namespace std;
string Local_IP;
string Local_Port;

