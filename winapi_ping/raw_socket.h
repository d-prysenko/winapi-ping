#pragma once
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <cstdint>
#include <exception>
#include <string>

#define DEFAULT_BUFLEN 512
#define SOCKET_READ_TIMEOUT_MSEC 10 * 1000

class RawSocket
{
public:
	~RawSocket();
	
	void init(const char* addr);
	int send(const char* buf, size_t len);
	size_t recv(char* buf, size_t len);

private:
	WSADATA _wsaData;
	addrinfo* _addr_info = nullptr;
	SOCKET _sock = INVALID_SOCKET;

	int _init_winsock();
	addrinfo* _resolve_addr(const char* addr);
	SOCKET _create_socket();
	void _cleanup();
};

class socket_exception : public std::exception
{
public:
	socket_exception()
		: std::exception()
	{ }

	socket_exception(std::string msg, size_t err_code = 0)
		: std::exception(msg.c_str())
	{
		_err_code = err_code;
	}

	size_t get_err_code()
	{
		return _err_code;
	}

private:

	size_t _err_code = 0;
};