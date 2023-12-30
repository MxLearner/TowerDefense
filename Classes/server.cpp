#include "server.h"



UDPServer::UDPServer() : sockfd(INVALID_SOCKET)
{
	// 初始化winsock
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {// 初始化失败
		printf("Failed to initialize Winsock\n");
		exit(EXIT_FAILURE);
	}
	// 使用ipv4协议，数据报套接字，使用默认协议udp
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd == INVALID_SOCKET) {// 创建失败
		printf("Create socket error\n");
		WSACleanup();// 清理winsock
		exit(EXIT_FAILURE);// 退出程序
	}
	// 设置服务器地址
	memset(&servaddr, 0, sizeof(servaddr));// 清零
	servaddr.sin_family = AF_INET;// ipv4
	servaddr.sin_addr.s_addr = INADDR_ANY;// 任意地址
	servaddr.sin_port = htons(DEFAULT_PORT);// 端口
}

UDPServer::~UDPServer()
{
	Stop();
}

bool UDPServer::Start()
{   // 绑定套接字
	if (bind(sockfd, (sockaddr*)&servaddr, sizeof(servaddr)) == SOCKET_ERROR) {
		printf("Bind error\n");
		Stop();
		return false;
	}

	printf("Server listening on port %d...\n", DEFAULT_PORT);
	return true;
}

void UDPServer::Stop()
{	// 关闭套接字
	if (sockfd != INVALID_SOCKET) {
		closesocket(sockfd);
		printf("Server closed\n");
	}
	// 清理winsock
	WSACleanup();
}

void UDPServer::Receive()
{
	char buff[BUFFSIZE];
	// 客户端地址
	sockaddr_in clientAddr;
	// 客户端地址长度
	int clientAddrLen = sizeof(clientAddr);

	while (true) {
		// 接收数据
		int bytesRead = recvfrom(sockfd, buff, BUFFSIZE - 1, 0, (sockaddr*)&clientAddr, &clientAddrLen);
		// 接收失败
		if (bytesRead == SOCKET_ERROR) {
			printf("Receive error\n");
			break;
		}
		// 接收到0字节数据
		buff[bytesRead] = '\0';
		// 打印客户端地址和数据
		printf("Received from %s:%d - %s\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port), buff);

		// Respond to the client
		const char* sendBuffer = gameMassageBuffer.c_str();
		sendto(sockfd, sendBuffer, strlen(sendBuffer), 0, (sockaddr*)&clientAddr, sizeof(clientAddr));
	}
}

