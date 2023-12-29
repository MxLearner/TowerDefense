#include <cstdio>
#include <cstdlib>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib")
#define BUFFSIZE 2048
#define DEFAULT_PORT 16555


class UDPServer {
public:
	UDPServer();
	~UDPServer();

	bool Start();
	void Stop();
	void Receive();
private:
	// 套接字
	SOCKET sockfd;
	// 服务器地址
	sockaddr_in servaddr;

};