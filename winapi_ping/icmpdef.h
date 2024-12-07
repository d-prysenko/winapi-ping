#pragma once
#include <cstdint>

struct ICMP_Echo {
	uint8_t type = 8;
	uint8_t code = 0;
	uint16_t checksum = 0;
	uint16_t id = 0;
	uint16_t seq_num = 0;
	uint16_t data = 0;
};

__declspec(align(8)) struct ICMP_Header {
	uint8_t type = 0;
	uint8_t code = 0;
	uint16_t checksum = 0;
};

__declspec(align(8)) struct ICMP_Echo_Request : public ICMP_Header {
	//ICMP_Header header;
	uint16_t id = 0;
	uint16_t seq_num = 0;
};