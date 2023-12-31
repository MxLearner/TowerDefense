#include "server.h"



UDPServer::UDPServer() : sockfd(INVALID_SOCKET)
{
	// ��ʼ��winsock
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {// ��ʼ��ʧ��
		printf("Failed to initialize Winsock\n");
		exit(EXIT_FAILURE);
	}
	// ʹ��ipv4Э�飬���ݱ��׽��֣�ʹ��Ĭ��Э��udp
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd == INVALID_SOCKET) {// ����ʧ��
		printf("Create socket error\n");
		WSACleanup();// ����winsock
		exit(EXIT_FAILURE);// �˳�����
	}
	// ���÷�������ַ
	memset(&servaddr, 0, sizeof(servaddr));// ����
	servaddr.sin_family = AF_INET;// ipv4
	servaddr.sin_addr.s_addr = INADDR_ANY;// �����ַ
	servaddr.sin_port = htons(DEFAULT_PORT);// �˿�
}

UDPServer::~UDPServer()
{
	Stop();
}

bool UDPServer::Start()
{   // ���׽���
	if (bind(sockfd, (sockaddr*)&servaddr, sizeof(servaddr)) == SOCKET_ERROR) {
		printf("Bind error\n");
		Stop();
		return false;
	}

	printf("Server listening on port %d...\n", DEFAULT_PORT);
	return true;
}

void UDPServer::Stop()
{	// �ر��׽���
	if (sockfd != INVALID_SOCKET) {
		closesocket(sockfd);
		printf("Server closed\n");
	}
	// ����winsock
	WSACleanup();
}

void UDPServer::Receive()
{
	char buff[BUFFSIZE];
	// �ͻ��˵�ַ
	sockaddr_in clientAddr;
	// �ͻ��˵�ַ����
	int clientAddrLen = sizeof(clientAddr);

	while (true) {
		// ��������
		int bytesRead = recvfrom(sockfd, buff, BUFFSIZE - 1, 0, (sockaddr*)&clientAddr, &clientAddrLen);
		// ����ʧ��
		if (bytesRead == SOCKET_ERROR) {
			printf("Receive error\n");
			break;
		}
		// ���յ�0�ֽ�����
		buff[bytesRead] = '\0';
		// ��ӡ�ͻ��˵�ַ������
		printf("Received from %s:%d - %s\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port), buff);

		// Respond to the client
		const char* sendBuffer = gameMassageBuffer.c_str();
		sendto(sockfd, sendBuffer, strlen(sendBuffer), 0, (sockaddr*)&clientAddr, sizeof(clientAddr));
	}
}

