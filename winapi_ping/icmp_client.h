#pragma once


#include <exception>
#include <string>
#include "stopwatch.h"
#include "icmpdef.h"
#include "helpers.h"
#include "raw_socket.h"

class ICMP_Client
{
public:

	std::chrono::steady_clock::duration ping(const char* addr);
	
};

