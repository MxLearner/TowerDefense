#include "client.h"
#include"cocos2d.h"
USING_NS_CC;
UDPClient::UDPClient() : sockfd(INVALID_SOCKET)
{
	// 初始化winsock
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		printf("Failed to initialize Winsock\n");
		exit(EXIT_FAILURE);
	}
	// 使用ipv4协议，数据报套接字，使用默认协议udp
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd == INVALID_SOCKET) {
		printf("Create socket error\n");
		WSACleanup();
		exit(EXIT_FAILURE);
	}
	// 设置服务器地址
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;// ipv4
	servaddr.sin_addr.s_addr = inet_addr(SERVER_IP);// 服务器ip
	servaddr.sin_port = htons(SERVER_PORT);// 端口
}

UDPClient::~UDPClient()
{
	// 关闭套接字
	if (sockfd != INVALID_SOCKET) {
		closesocket(sockfd);
		printf("Client closed\n");
	}
	// 清理winsock
	WSACleanup();
}

bool UDPClient::Connect()
{
	return true;  // UDP是无连接的，因此Connect操作在UDP中并不需要。
}

void UDPClient::Send(const char* message)
{
	if (sendto(sockfd, message, strlen(message), 0, (sockaddr*)&servaddr, sizeof(servaddr)) == -1) {
		CCLOG("client send error");
	}

}

void UDPClient::Receive()
{
	char buff[BUFFSIZE];

	while (isReceiving) {
		int bytesRead = recvfrom(sockfd, buff, BUFFSIZE - 1, 0, NULL, NULL);

		if (bytesRead == SOCKET_ERROR) {
			printf("Receive error\n");
			continue;
		}

		buff[bytesRead] = '\0';
		message = buff;
	}
}

void UDPClient::stop()
{
	isReceiving = false;
	// 关闭套接字
	if (sockfd != INVALID_SOCKET) {
		closesocket(sockfd);
		printf("Server closed\n");
	}
	// 清理winsock
	WSACleanup();
}
