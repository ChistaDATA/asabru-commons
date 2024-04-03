#ifndef PROTOCOL_HELPER_DOT_H
#define PROTOCOL_HELPER_DOT_H
// UNIX/Linux-specific definitions
#define SOCKET int

#include "util/endian_utils.h"
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iostream>
#include <sys/time.h>

#ifdef WINDOWS_OS
#include <windows.h>
#else
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/socket.h>

#define DWORD unsigned long
#endif

#define SOCKET_ERROR (-1)

static uint32_t readInt32(std::string buffer, int start, int length = 4) {
	const std::string packet_length_nw_bytes = buffer.substr(start, length);
	uint32_t packet_length_nw = 0;
	memcpy(&packet_length_nw, &packet_length_nw_bytes, length);
	const uint32_t packet_length = le32toh(packet_length_nw);
	return packet_length;
}

static std::string readCString(std::string buffer, int start) {
	std::string name;
	int i = start;
	while (buffer[i] != '\0') {
		name += buffer[i];
		i++;
	}
	return name;
}

class ProtocolHelper {
  public:
	static std::string GetIPAddressAsString(struct sockaddr_in *client_addr);
	static std::string GetIPPortAsString(struct sockaddr_in *client_addr);
	static bool SetReadTimeOut(SOCKET s, long second);
	static bool SetKeepAlive(SOCKET s, int flags);
	static bool ReadSocketBuffer(SOCKET s, char *bfr, int size, int *num_read);
	static bool ReadSocketBuffer(SOCKET s, char *bfr, int size);
	static bool WriteSocketBuffer(SOCKET s, char *bfr, int size);
};

#endif
