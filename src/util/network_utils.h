#pragma once

#ifdef WINDOWS_OS
#include <windows.h>
#else
#define DWORD unsigned long
#define u_long unsigned long

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdexcept>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#endif

#include "ThreadUtils.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

class NetworkUtils {
  public:
	// Function to create and initialize a hostent structure
	static struct hostent *create_hostent(const char *ip_address) {
		struct hostent *he = (struct hostent *)malloc(sizeof(struct hostent));
		if (he == NULL) {
			perror("malloc");
			exit(EXIT_FAILURE);
		}

		// Set the official name of the host (can be NULL)
		he->h_name = NULL;

		// Set the list of aliases (can be NULL)
		he->h_aliases = NULL;

		// Set the address type (AF_INET for IPv4)
		he->h_addrtype = AF_INET;

		// Set the length of the address
		he->h_length = sizeof(struct in_addr);

		// Allocate memory for the list of addresses (null-terminated array of pointers)
		he->h_addr_list = (char **)malloc(2 * sizeof(char *));
		if (he->h_addr_list == NULL) {
			perror("malloc");
			free(he);
			exit(EXIT_FAILURE);
		}

		// Allocate memory for the address
		he->h_addr_list[0] = (char *)malloc(he->h_length);
		if (he->h_addr_list[0] == NULL) {
			perror("malloc");
			free(he->h_addr_list);
			free(he);
			exit(EXIT_FAILURE);
		}

		// Convert the IP address from text to binary form
		if (inet_pton(AF_INET, ip_address, he->h_addr_list[0]) <= 0) {
			perror("inet_pton");
			free(he->h_addr_list[0]);
			free(he->h_addr_list);
			free(he);
			exit(EXIT_FAILURE);
		}

		// Null-terminate the list of addresses
		he->h_addr_list[1] = NULL;

		return he;
	}
};
