#include <cstdio>
#include <cstdlib>
#include <cstring>
#include<iostream>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#define BUFFSIZE 102400
#define DEFAULT_PORT 16555


class UDPServer {

private:
	// �׽���
	SOCKET sockfd;
	// ��������ַ
	sockaddr_in servaddr;
	std::string  gameMassageBuffer; // ���ڴ浵��json

public:
	UDPServer();
	~UDPServer();

	bool Start();
	void Stop();
	void Receive();
	void setGameMassageBuffer(std::string Buffer) {
		gameMassageBuffer = Buffer;
	}

	std::string  getGameMassageBuffer() {
		return gameMassageBuffer;
	}

};
