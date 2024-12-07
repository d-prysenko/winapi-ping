#include "icmpdef.h"

uint16_t calc_checksum(const char* buf, size_t size)
{
	uint16_t* p16_buf = (uint16_t*)buf;

	uint32_t sum = 0;

	while (size > 1)
	{
		sum += *p16_buf;
		p16_buf++;
		size -= 2;
	}

	if (size)
	{
		sum += *(uint8_t*)p16_buf;
	}

	sum = (sum & 0xffff) + (sum >> 16);

	return (uint16_t)(~sum);
}

ICMP_PACKET_BUF create_icmp_echo_packet(uint8_t code, uint16_t id, uint16_t seq_num, std::string data)
{
	size_t buflen = sizeof(ICMP_Echo_Request) + data.length();

	char* buf = new char[buflen];
	memset(buf, 0, buflen);

	ICMP_Echo_Request* icmp_echo = (ICMP_Echo_Request*)buf;

	icmp_echo->type = ICMP_Type::ECHO;
	icmp_echo->id = htons(1);

	memcpy(icmp_echo->data, data.c_str(), data.length());

	icmp_echo->checksum = calc_checksum(buf, buflen);

	return ICMP_PACKET_BUF(buf, buflen);
}