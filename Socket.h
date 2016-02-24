#pragma once
#include "net_io.h"

class Socket {
public:
	Socket();
	~Socket();
	void connect(InetSocketAddress * soAddr,int timeout);
	void connect(char * host, int port,int timeout);
	int write(const char * buf, int len);
	void blockRead(char * buf, int len); 
	void foo();
private:
	SOCKET sock;
};


