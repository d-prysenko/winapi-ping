#pragma once

#include <cstdint>
#include <string>
#include <WinSock2.h>

#define ICMP_HEADER uint8_t type = 0; uint8_t code = 0; uint16_t checksum = 0

enum ICMP_Type : uint8_t {
	ECHO_REPLY = 0,
	DEST_UNREACHABLE = 3,
	SRC_QUENCH = 4,
	REDIRECT = 5,
	ECHO = 8,
	TIME_EXCEEDED = 11,
	PARAM_PROBLEM = 12,
	TIMESTAMP = 13,
	TIMESTAMP_REPLY = 14,
	INFORMATION_REQUEST = 15,
	INFORMATION_REPLY = 16,
};

__declspec(align(1)) struct ICMP_Echo_Request {
	ICMP_HEADER;
	uint16_t id = 0;
	uint16_t seq_num = 0;
	char data[];
};

struct ICMP_PACKET_BUF
{
	char* buf = nullptr;
	size_t buflen = 0;

	ICMP_PACKET_BUF()
	{ }

	ICMP_PACKET_BUF(char* buf, size_t buflen)
		: buf(buf), buflen(buflen)
	{ }

	~ICMP_PACKET_BUF()
	{
		if (buf) {
			delete[] buf;
			buf = nullptr;
			buflen = 0;
		}
	}
};

uint16_t calc_checksum(const char* buf, size_t size);

ICMP_PACKET_BUF create_icmp_echo_packet(uint8_t code = 0, uint16_t id = 0, uint16_t seq_num = 0, std::string data = "test");