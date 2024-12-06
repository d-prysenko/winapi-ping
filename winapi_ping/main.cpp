#include <chrono>
#include <iostream>
#include <ctime>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment (lib, "Ws2_32.lib")

/*

https://learn.microsoft.com/en-us/windows/win32/winsock/about-clients-and-servers

Client
1. Initialize Winsock.
2. Create a socket.
3. Connect to the server.
4. Send and receive data.
5. Disconnect.

*/

#define DEFAULT_BUFLEN 512

WSADATA wsaData;
SOCKET dest_sock = INVALID_SOCKET;
addrinfo *p_addrinfo = NULL;

int initWinsock()
{
	return WSAStartup(MAKEWORD(2, 2), &wsaData);
}

int createSocket(PCSTR ip)
{
	addrinfo* result = NULL, * ptr = NULL, hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_RAW;
	hints.ai_protocol = IPPROTO_ICMP;

	int res = getaddrinfo(ip, "", &hints, &result);

	if (res) {
		WSACleanup();

		return res;
	}	

	ptr = result;

	dest_sock = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

	if (dest_sock == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();

		return 1;
	}

	p_addrinfo = ptr;

	return 0;
}

int connect()
{
	int res = connect(dest_sock, p_addrinfo->ai_addr, (int)(p_addrinfo->ai_addrlen));

	if (res == SOCKET_ERROR) {
		closesocket(dest_sock);
		freeaddrinfo(p_addrinfo);
		WSACleanup();
		return 1;
	}

	return 0;
}

struct ICMP_Echo {
	uint8_t type = 8;
	uint8_t code = 0;
	uint16_t checksum = 0;
	uint16_t id = htons(1);
	uint16_t seq_num = 0;
	uint16_t data = htons(0xf15a);
};


uint16_t calc_checksum(ICMP_Echo packet)
{
	uint16_t* start = (uint16_t*)&packet;
	uint8_t size = sizeof(packet) / 2;

	uint32_t acc = 0;

	for (int i = 0; i < size; i++)
	{
		acc += ~(*start) & 0xffff;
		start++;
	}

	acc = (acc & 0xffff) + (acc >> 16);

	return acc;
}

void print_bytes(void* data, size_t size)
{
	uint8_t* start = (uint8_t*)data;

	for (int i = 0; i < size; i++)
	{
		printf("%X ", *start);
		start++;
	}

	printf("\n");
}

int main()
{
	ICMP_Echo echo;

	uint16_t checksum = calc_checksum(echo);

	echo.checksum = checksum;

	print_bytes(&echo, sizeof(echo));

	int res = 0;

	res = initWinsock();

	if (res != 0) {
		std::cerr << "Failed init socket. WSAStartup code: " << res << std::endl;

		return -1;
	}

	res = createSocket("8.8.8.8");

	if (res != 0) {
		std::cerr << "Failed create socket. ret code: " << res << std::endl;
		
		return -1;
	}

	res = connect();

	if (res != 0) {
		std::cerr << "Failed to connect. ret code: " << res << std::endl;

		return -1;
	}

	res = send(dest_sock, (const char*)&echo, sizeof(echo), 0);
	
	auto start_time = std::chrono::high_resolution_clock::now();

	if (res == SOCKET_ERROR) {
		printf("send failed: %d\n", WSAGetLastError());
		closesocket(dest_sock);
		WSACleanup();
		return 1;
	}

	printf("Bytes Sent: %ld\n", res);

	// shutdown the connection for sending since no more data will be sent
	// the client can still use the dest_sock for receiving data
	res = shutdown(dest_sock, SD_SEND);
	if (res == SOCKET_ERROR) {
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(dest_sock);
		WSACleanup();
		return 1;
	}

	// ip header
	// 0x45, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x6c, 0x01, 0x7e, 0x18, 0x08, 0x08, 0x08, 0x08, 0xc0, 0xa8, 0x00, 0x0f,
	// icmp header
	// 0x00, 0x00, 0x9e, 0x9c, 0x00, 0x01, 0x00, 0x00, 0x61, 0x62, 

	int recvbuflen = DEFAULT_BUFLEN;
	char recvbuf[DEFAULT_BUFLEN];
	ZeroMemory(recvbuf, DEFAULT_BUFLEN);

	// Receive data until the server closes the connection
	res = recv(dest_sock, recvbuf, recvbuflen, 0);
	if (res > 0) {
		auto end_time = std::chrono::high_resolution_clock::now();
		auto result = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
		printf("Bytes received: %d\n", res);
		printf("Time: %f ms\n", result / 1000.f);
	}
	else if (res == 0)
		printf("Connection closed\n");
	else
		printf("recv failed: %d\n", WSAGetLastError());

	return 0;
}
