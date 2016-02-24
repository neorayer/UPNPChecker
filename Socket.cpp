#include "StdAfx.h"
#include "Socket.h"

Socket::Socket() {
	this->sock = socket(AF_INET, SOCK_STREAM, 0);
	if (this->sock < 0)
		throw 1;

};

void Socket::foo() {
	InetSocketAddress isa = InetSocketAddress("10.1.1.221", 9333);
	this->connect(&isa, 3000);
}

void Socket::connect(InetSocketAddress * inetAddr,int timeout) {
	//inetAddr->println("Connecting to ");
	int ret = ::connect(sock, inetAddr->get_sockaddr(), sizeof(sockaddr ));
	if (-1 == ret) 
		throw 1;
	int one = 1;
	if (setsockopt(this->sock, IPPROTO_TCP, TCP_NODELAY, (char *)&one, sizeof(int)) != 0)
		throw 1;
};

void Socket::connect(char *host, int port,int timeout) {
	InetSocketAddress isa = InetSocketAddress(host, port);
	this->connect(&isa, timeout);
};

Socket::~Socket(void)
{
}

int Socket::write(const char * buf, int len) {
	return ::send(sock, buf, len, 0);
}

void Socket::blockRead(char * buf, int len){
	int c = 0;
	do {
		int ret = recv(sock, buf + c, len - c, 0);
		if (ret < 0)
			throw 1;
		if (ret == 0) { 
			Sleep(1);
			continue;
		}
		c += ret;
	}while(c < len);
}