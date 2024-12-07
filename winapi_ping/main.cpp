#include <chrono>
#include <iostream>
#include <ctime>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "stopwatch.h"
#include "icmpdef.h"

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
#define SOCKET_READ_TIMEOUT_MSEC 10 * 1000

WSADATA wsaData;

int init_winsock();
addrinfo* resolve_addr(const char* addr);
SOCKET create_socket(addrinfo* addr_info);
int connect(SOCKET sock, addrinfo* addr_info);

uint16_t calc_checksum(ICMP_Echo packet);
void print_bytes(void* data, size_t size);

int main()
{
	Stopwatch stopwatch;

	ICMP_Echo echo;
	echo.id = htons(1);
	echo.data = htons(0xf15a);

	uint16_t checksum = calc_checksum(echo);

	echo.checksum = checksum;

	print_bytes(&echo, sizeof(echo));

	int res = 0;

	res = init_winsock();

	if (res != 0) {
		std::cerr << "Failure: init socket. WSAStartup code: " << res << std::endl;

		return 1;
	}

	addrinfo* addr_info = resolve_addr("google.com");

	if (!addr_info) {
		std::cerr << "Failure: resolve addr. WSAGetLastError: " << WSAGetLastError() << std::endl;
		WSACleanup();

		return 2;
	}

	SOCKET sock = create_socket(addr_info);

	if (sock == INVALID_SOCKET) {
		std::cerr << "Failure: create socket. WSAGetLastError: " << WSAGetLastError() << std::endl;
		freeaddrinfo(addr_info);
		WSACleanup();

		return 3;
	}

	res = connect(sock, addr_info);

	if (res == SOCKET_ERROR) {
		std::cerr << "Failure: connect. WSAGetLastError: " << WSAGetLastError() << std::endl;
		closesocket(sock);
		freeaddrinfo(addr_info);
		WSACleanup();

		return 4;
	}

	res = send(sock, (const char*)&echo, sizeof(echo), 0);

	stopwatch.start();

	if (res == SOCKET_ERROR) {
		std::cerr << "Failure: send. WSAGetLastError: " << WSAGetLastError() << std::endl;
		closesocket(sock);
		freeaddrinfo(addr_info);
		WSACleanup();

		return 5;
	}

	printf("Bytes Sent: %ld\n", res);

	// shutdown the connection for sending since no more data will be sent
	// the client can still use the dest_sock for receiving data
	res = shutdown(sock, SD_SEND);

	if (res == SOCKET_ERROR) {
		std::cerr << "Failure: shutdown. WSAGetLastError: " << WSAGetLastError() << std::endl;
		closesocket(sock);
		freeaddrinfo(addr_info);
		WSACleanup();

		return 6;
	}

	// ip header
	// 0x45, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x6c, 0x01, 0x7e, 0x18, 0x08, 0x08, 0x08, 0x08, 0xc0, 0xa8, 0x00, 0x0f,
	// icmp header
	// 0x00, 0x00, 0x9e, 0x9c, 0x00, 0x01, 0x00, 0x00, 0x61, 0x62, 

	int recvbuflen = DEFAULT_BUFLEN;
	char recvbuf[DEFAULT_BUFLEN];
	ZeroMemory(recvbuf, DEFAULT_BUFLEN);

	// Receive data until the server closes the connection
	res = recv(sock, recvbuf, recvbuflen, 0);

	if (res > 0) {
		printf("Bytes received: %d\n", res);
		printf("Time: %f ms\n", stopwatch.duration_ns() / 1000.f);
	}
	else if (res == 0) {
		printf("Connection closed\n");
	}
	else {
		printf("recv failed: %d\n", WSAGetLastError());
	}

	closesocket(sock);
	freeaddrinfo(addr_info);
	WSACleanup();

	return 0;
}

int init_winsock()
{
	return WSAStartup(MAKEWORD(2, 2), &wsaData);
}

addrinfo* resolve_addr(const char* addr)
{
	addrinfo* result = nullptr, hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_RAW;
	hints.ai_protocol = IPPROTO_ICMP;

	int res = getaddrinfo(addr, "", &hints, &result);

	if (res) {
		return nullptr;
	}

	return result;
}

SOCKET create_socket(addrinfo* addr_info)
{
	SOCKET sock = socket(addr_info->ai_family, addr_info->ai_socktype, addr_info->ai_protocol);

	uint32_t timeout = SOCKET_READ_TIMEOUT_MSEC;
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));

	return sock;
}

int connect(SOCKET sock, addrinfo* addr_info)
{
	return connect(sock, addr_info->ai_addr, (int)(addr_info->ai_addrlen));
}

uint16_t calc_checksum(ICMP_Echo packet)
{
	uint16_t* start = (uint16_t*)&packet;
	uint8_t size = sizeof(packet) / 2;

	uint32_t acc = 0;

	for (int i = 0; i < size; i++)
	{
		acc += (uint16_t)~(*start);
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
