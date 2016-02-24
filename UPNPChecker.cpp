// UPNPChecker.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "winsock2.h"
#include "string.h"
#include "net_io.h"
class FakeWar {
public:
	FakeWar(char * pipeHost, int pipePort, char * virtualIp, int virtualPort);

	char pipeHost[64];
	int pipePort;

	char virtualIp[64];
	int virtualPort;

	void startCtrPipe();
	void connect(char * dst_vip, u_short dst_port);
	void startDataPipe(char * clientId);

private:
	char vip_b[4];
};

FakeWar::FakeWar(char * _pipeHost, int _pipePort, char * _virtualIp, int _virtualPort) {
	strncpy_s(this->pipeHost, _pipeHost, 64);
	this->pipePort = pipePort;

	strncpy_s(this->virtualIp, _virtualIp, 64);
	this->pipePort = _pipePort;

	InetSocketAddress isa = InetSocketAddress(virtualIp, virtualPort);
	memcpy(vip_b, &(isa.get_sockaddr_in()->sin_addr), 4);
}


void startDataPipeProc(char * clientId) {
	//printf("---------%X, %d\n", clientId, *(DWORD *)clientId); 
	FakeWar fakeWar = FakeWar("10.1.1.198", 8000, "10.0.2.1", 6112);
	fakeWar.startDataPipe(clientId);
}

void printError() {
		DWORD nErrorNo = GetLastError ( ); // 得到错误代码
		LPSTR lpBuffer= NULL;    
		FormatMessageA ( FORMAT_MESSAGE_ALLOCATE_BUFFER  |
				FORMAT_MESSAGE_IGNORE_INSERTS  |
				FORMAT_MESSAGE_FROM_SYSTEM,
				NULL,
				nErrorNo, // 此乃错误代码，通常在程序中可由 GetLastError()得之
				0,
				(LPSTR) & lpBuffer,
				0 ,
				NULL );
		printf("ErrorNO=%d, %s",nErrorNo,  lpBuffer  ?  lpBuffer : " Sorry, cannot find this error info. ") ;
		LocalFree (lpBuffer);
}

void FakeWar::startDataPipe(char * clientId) {
	try {
		Socket socket = Socket();
		socket.connect(pipeHost, pipePort, 3000);
		char roleTag = 0x10;
		socket.write(&roleTag, 1);
		socket.write(vip_b, 4);
		socket.write(clientId, 4);
		delete clientId;

		char buf[38];
		socket.blockRead(buf, 38);
	
		DWORD tc = 0;
		DWORD c = 0;
		char nextBuf[4];
		while(1) {
			socket.blockRead((char *)(&tc),4);
			socket.blockRead(nextBuf,4);

			DWORD  tc1 = GetTickCount();
			DWORD tcc = tc1 - tc;
			if (tcc > 0) {
				//printf("%d\t%d\t%d\n",c, tc, tcc);
				c ++;
			}
		}
	}catch(int) {
		printError();
	}
}

void FakeWar::startCtrPipe() {
	try {
		Socket socket = Socket();
		socket.connect(pipeHost, pipePort, 3000);
	
		char roleTag = 0x14;
		socket.write(&roleTag, 1);
		socket.write(vip_b, 4);

		while(1) {
			char ctrTag;
			socket.blockRead(&ctrTag, 1);

			if (0x00 == ctrTag) {
				printf("noop ctl code: %d \n" , ctrTag);
				continue;
			}else if(0x01 == ctrTag) {
				char * clientId = new char[4];
				socket.blockRead(clientId, 4);

				DWORD far threadID;
				CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)(startDataPipeProc), clientId, 0, &threadID);
			}else {
				printf("illigal ctl code: %d\n" , ctrTag);
				continue;
			}
		}

	}catch(int) {
		printError();
	}
}

void FakeWar::connect(char * dst_vip, u_short dst_port) {
	try {
		Socket socket = Socket();
		socket.connect(pipeHost, pipePort, 3000);
		char roleTag = 0x12;
		socket.write(&roleTag, 1);

		char uname[32];
		strcpy_s(uname, "zhourui");
		socket.write(uname, 32);

		char skey[4];
		socket.write(skey, 4);

		socket.write(vip_b, 4);

		char dst_vip_b[4];
		InetSocketAddress isa = InetSocketAddress(dst_vip, dst_port);
		memcpy(dst_vip_b, &(isa.get_sockaddr_in()->sin_addr), 4);
		socket.write(dst_vip_b, 4);

		socket.write((char *)&dst_port, 2);

		int i=0;
		DWORD  tc;
		char zeros[4];
		memset(zeros, 0, 4);
		for(;;) {
			Sleep(27);
			tc = GetTickCount();
			socket.write((char *)&tc, 4);
			socket.write(zeros, 4);
		}
	}catch(int) {
		printError();
	}
	
}

void ServerThreadProc() {
	FakeWar fw = FakeWar("10.1.1.198",8000, "10.0.2.1", 6112);

	fw.startCtrPipe();
}

void ClientThreadProc(int * lastIp) {
	char ip[54];
	sprintf_s(ip, "10.0.2.%d", * lastIp);
	FakeWar fw = FakeWar("10.1.1.198",8000, ip, 6112);

	fw.connect("10.0.2.1", 6112);
}

int _tmain(int argc, _TCHAR* argv[])
{
    WORD wVersionRequested;
    WSADATA wsaData;
    wVersionRequested = MAKEWORD(2, 2);
    int err = WSAStartup(wVersionRequested, &wsaData);

	{
		DWORD far threadID;
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)(ServerThreadProc), NULL, 0, &threadID);
	}
	Sleep(1000);
	printf("start client\n");
	
	int i=0;
	for(i=2; i<60; i++)
	{
		//Sleep(1000);
		int * ip = new int;
		* ip = i;
		DWORD far threadID;
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)(ClientThreadProc), ip, 0, &threadID);
	}
	

	Sleep(600000);
	return 0;
}

