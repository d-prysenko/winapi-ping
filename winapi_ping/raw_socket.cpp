#include "raw_socket.h"

RawSocket::~RawSocket()
{
	_cleanup();
}

void RawSocket::init(const char* addr)
{
	int res = _init_winsock();

	if (res != 0) {
		throw socket_exception("init socket. WSAStartup code: " + res, res);
	}

	_resolve_addr(addr);

	if (!_addr_info) {
		int last_err = WSAGetLastError();
		_cleanup();

		throw socket_exception("resolve addr. WSAGetLastError code: " + std::to_string(last_err), last_err);
	}

	_create_socket();

	if (_sock == INVALID_SOCKET) {
		int last_err = WSAGetLastError();
		_cleanup();

		throw socket_exception("create socket. WSAGetLastError code: " + std::to_string(last_err), last_err);
	}
}

int RawSocket::send(const char* buf, size_t len)
{
	int res = sendto(_sock, buf, len, 0, _addr_info->ai_addr, _addr_info->ai_addrlen);

	if (res == SOCKET_ERROR) {
		int last_err = WSAGetLastError();
		_cleanup();

		throw socket_exception("send. WSAGetLastError code: " + std::to_string(last_err), last_err);
	}

	return res;
}

size_t RawSocket::recv(char* buf, size_t len)
{
	int res = ::recv(_sock, buf, len, 0);

	int last_err = WSAGetLastError();

	if (res < 0) {
		throw socket_exception("recv failed. WSAGetLastError code: " + std::to_string(last_err), last_err);
	}
	else if (res == 0) {
		throw socket_exception("recv connection closed by server. WSAGetLastError code: " + std::to_string(last_err), last_err);
	}

	return res;
}



int RawSocket::_init_winsock()
{
	return WSAStartup(MAKEWORD(2, 2), &_wsaData);
}

addrinfo* RawSocket::_resolve_addr(const char* addr)
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

	_addr_info = result;

	return result;
}

SOCKET RawSocket::_create_socket()
{
	_sock = socket(_addr_info->ai_family, _addr_info->ai_socktype, _addr_info->ai_protocol);

	uint32_t timeout = SOCKET_READ_TIMEOUT_MSEC;
	setsockopt(_sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));

	return _sock;
}

void RawSocket::_cleanup()
{
	if (_sock != INVALID_SOCKET) {
		closesocket(_sock);
		_sock = INVALID_SOCKET;
	}

	if (_addr_info != nullptr) {
		freeaddrinfo(_addr_info);
		_addr_info = nullptr;
	}

	WSACleanup();
}
