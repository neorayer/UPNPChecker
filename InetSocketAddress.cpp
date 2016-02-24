#include "stdafx.h"
#include "stdio.h"
#include "stdlib.h"
#include "net_io.h"


void hostToAddr(char * hostname, struct in_addr * addr) {
    struct hostent *h;

    h = gethostbyname(hostname);
    if (h == 0)
		throw 1;
    if (h->h_addrtype != AF_INET) {
            fprintf(stderr, "%s: unexpected address type %d\n",
                            hostname, h->h_addrtype);
    }
    *addr = *(struct in_addr*)(h->h_addr);

}

InetSocketAddress::InetSocketAddress(void) {
}

InetSocketAddress::~InetSocketAddress(void) {
}


InetSocketAddress::InetSocketAddress(char * host, int port) {
	sin.sin_family = AF_INET;
	hostToAddr(host, &(sin.sin_addr));
	sin.sin_port = htons(port);
}

struct sockaddr_in * InetSocketAddress::get_sockaddr_in() {
	return &sin;
}

sockaddr * InetSocketAddress::get_sockaddr(){
	return (sockaddr *) & sin;
}

void InetSocketAddress::println(char * head){
	printf("%s%d.%d.%d.%d:%d\r\n", 
				head,
				sin.sin_addr.S_un.S_un_b.s_b1,
				sin.sin_addr.S_un.S_un_b.s_b2,
				sin.sin_addr.S_un.S_un_b.s_b3,
				sin.sin_addr.S_un.S_un_b.s_b4,
				ntohs(sin.sin_port)
	);
}
