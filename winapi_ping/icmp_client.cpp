#include "icmp_client.h"


std::chrono::steady_clock::duration ICMP_Client::ping(const char* addr)
{
	Stopwatch stopwatch;

	ICMP_PACKET_BUF packet = create_icmp_echo_packet();
	
	print_bytes(packet.buf, packet.buflen);

	RawSocket raw_socket;

	raw_socket.init(addr);
	int sent_len = raw_socket.send(packet.buf, packet.buflen);
	
	stopwatch.start();

	debug("Bytes Sent: %ld\n", sent_len);

	// ip header
	// 0x45, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x6c, 0x01, 0x7e, 0x18, 0x08, 0x08, 0x08, 0x08, 0xc0, 0xa8, 0x00, 0x0f,
	// icmp header
	// 0x00, 0x00, 0x9e, 0x9c, 0x00, 0x01, 0x00, 0x00, 0x61, 0x62, 

	int recvbuflen = DEFAULT_BUFLEN;
	char recvbuf[DEFAULT_BUFLEN];
	ZeroMemory(recvbuf, DEFAULT_BUFLEN);

	int recv_len = raw_socket.recv(recvbuf, recvbuflen);

	auto duration = stopwatch.duration();

	print_bytes(recvbuf, recv_len);

	debug("Bytes Received: %ld\n", recv_len);

	return duration;
}




