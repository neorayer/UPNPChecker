#pragma once

class InetSocketAddress
{
public:
	InetSocketAddress(void);
	InetSocketAddress(char * host, int port);
	struct sockaddr_in * get_sockaddr_in();
	sockaddr * get_sockaddr();
	void println(char * head);
public:
	~InetSocketAddress(void);
private:
	struct sockaddr_in sin;
};


