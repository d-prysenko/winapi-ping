#include "icmp_client.h"


std::chrono::steady_clock::duration ICMP_Client::ping(const char* addr)
{
	WSADATA wsaData;
	Stopwatch stopwatch;

	ICMP_Echo echo;
	echo.id = htons(1);
	echo.data = htons(0x3131);

	uint16_t checksum = calc_checksum(echo);

	echo.checksum = checksum;

	print_bytes(&echo, sizeof(echo));

	int res = 0;

	res = init_winsock(&wsaData);

	if (res != 0) {
		throw icmp_exception("init socket. WSAStartup code: " + res, res);
	}

	addrinfo* addr_info = resolve_addr(addr);

	if (!addr_info) {
		int last_err = WSAGetLastError();
		WSACleanup();

		throw icmp_exception("resolve addr. WSAStartup code: " + std::to_string(last_err), last_err);
	}

	SOCKET sock = create_socket(addr_info);

	if (sock == INVALID_SOCKET) {
		int last_err = WSAGetLastError();
		freeaddrinfo(addr_info);
		WSACleanup();

		throw icmp_exception("create socket. WSAStartup code: " + std::to_string(last_err), last_err);
	}

	res = sendto(sock, (const char*)&echo, sizeof(echo), 0, addr_info->ai_addr, addr_info->ai_addrlen);

	stopwatch.start();

	if (res == SOCKET_ERROR) {
		int last_err = WSAGetLastError();
		closesocket(sock);
		freeaddrinfo(addr_info);
		WSACleanup();

		throw icmp_exception("send. WSAStartup code: " + std::to_string(last_err), last_err);
	}

	printf("Bytes Sent: %ld\n", res);

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
		return stopwatch.duration();
	}
	else {
		int last_err = WSAGetLastError();

		throw icmp_exception("recv failed. WSAStartup code: " + std::to_string(last_err), last_err);
	}

	closesocket(sock);
	freeaddrinfo(addr_info);
	WSACleanup();

	return std::chrono::steady_clock::duration();
}

int ICMP_Client::init_winsock(WSADATA* wsaData)
{
	return WSAStartup(MAKEWORD(2, 2), wsaData);
}

addrinfo* ICMP_Client::resolve_addr(const char* addr)
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

SOCKET ICMP_Client::create_socket(addrinfo* addr_info)
{
	SOCKET sock = socket(addr_info->ai_family, addr_info->ai_socktype, addr_info->ai_protocol);

	uint32_t timeout = SOCKET_READ_TIMEOUT_MSEC;
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));

	return sock;
}

uint16_t ICMP_Client::calc_checksum(ICMP_Echo packet)
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



void ICMP_Client::print_bytes(const void* data, size_t size)
{
	uint8_t* start = (uint8_t*)data;

	for (int i = 0; i < size; i++)
	{
		printf("%X ", *start);
		start++;
	}

	printf("\n");
}
