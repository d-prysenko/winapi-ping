#include <iostream>
#include "icmp_client.h"

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

int main()
{
	ICMP_Client icmp_client;
	
	try {
		auto time = icmp_client.ping("google.com");

		printf("ping time: %f\n", std::chrono::duration_cast<std::chrono::microseconds>(time).count() / 1000.f);
	}
	catch (icmp_exception& e) {
		error("Failure: %s\n", e.what());

		return 1;
	}

	return 0;
}


