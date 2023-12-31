#include <cstdio>
#include <cstdlib>
#include<iostream>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib")

#define BUFFSIZE 102400
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 16555

class UDPClient {

private:
	SOCKET sockfd;
	sockaddr_in servaddr;
	std::string message;
	bool isReceiving = true;
public:
	UDPClient();
	~UDPClient();

	bool Connect();
	void Send(const char* message);
	void Receive();
	void stop();

	void setMessage(std::string message) {
		this->message = message;
	}
	std::string getMessage() {
		return message;
	}

};