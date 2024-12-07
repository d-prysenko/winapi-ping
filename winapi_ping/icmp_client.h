#pragma once

#include <WinSock2.h>
#include <ws2tcpip.h>
#include <exception>
#include <string>
#include "stopwatch.h"
#include "icmpdef.h"
#include "helpers.h"

#define DEFAULT_BUFLEN 512
#define SOCKET_READ_TIMEOUT_MSEC 10 * 1000


class ICMP_Client
{
public:

	std::chrono::steady_clock::duration ping(const char* addr);

private:

	int init_winsock(WSADATA* wsaData);
	addrinfo* resolve_addr(const char* addr);
	SOCKET create_socket(addrinfo* addr_info);
};

class icmp_exception : public std::exception
{
public:
	icmp_exception()
		: std::exception()
	{ }

	icmp_exception(std::string msg, size_t err_code = 0)
		: std::exception(msg.c_str())
	{
		_err_code = err_code;
	}

	size_t get_err_code()
	{
		return _err_code;
	}
	
private:

	size_t _err_code;
};

