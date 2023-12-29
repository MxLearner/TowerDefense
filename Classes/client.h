#include <cstdio>
#include <cstdlib>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib")

#define BUFFSIZE 2048
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 16555

class UDPClient {
public:
	UDPClient();
	~UDPClient();

	bool Connect();
	void Send(const char* message);
	void Receive();

private:
	SOCKET sockfd;
	sockaddr_in servaddr;
};