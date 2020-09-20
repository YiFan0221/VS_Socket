#pragma once

#include <winsock2.h> //<winsock.h>
#pragma  comment(lib,"WS2_32.lib")
//int WSAStartup(WORD wVersionRequested, LPWSADATA lpwsadata);
class tSocket
{
	
	public:
		WSAData wsaData;

	tSocket();
	virtual ~tSocket();
};

