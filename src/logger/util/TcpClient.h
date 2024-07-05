#pragma once
#include "../../util/network_utils.h"
#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <stdexcept>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

const int BUFFER_SIZE = 1024;

class TcpClient {
  public:
	TcpClient(std::string server_name, int client_port) : m_ServerPort(client_port) {
		strcpy(m_ServerName, server_name.c_str());
		CreateSocket();
		Connect();
	}

	~TcpClient() {
		// Close the socket
		close(s_);
	}

	void Connect() {
		std::string host = m_ServerName;
		int port = m_ServerPort;
		std::string error;
		Resolve(host);

		// Step 4
		if (m_HostPointer && m_HostPointer->h_name) {
			std::cout << ("IP address of " + std::string(m_HostPointer->h_name) +
						  " is: " + std::string(inet_ntoa(*(struct in_addr *)m_HostPointer->h_addr)))
					  << std::endl;
		}

		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		addr.sin_addr = *((in_addr *)m_HostPointer->h_addr);
		memset(&(addr.sin_zero), 0, 8);

		if (::connect(s_, (sockaddr *)&addr, sizeof(sockaddr))) {
			std::cout << ("Unable to connect to the host endpoint ") << std::endl;
#if WINDOWS_OS
			error = strerror(WSAGetLastError());
#else
			error = strerror(errno);
#endif
			std::cout << (error) << std::endl;
			throw std::runtime_error(error);
		}
	}

	void CreateSocket() {
		// UDP: use SOCK_DGRAM instead of SOCK_STREAM
		s_ = socket(AF_INET, SOCK_STREAM, 0);

		if (s_ == INVALID_SOCKET) {
			throw std::runtime_error("INVALID_SOCKET");
		}
	}

	void Close() { CloseSocket(s_); }

	void Reconnect() {
		Close();
		CreateSocket();
		Connect();
	}

	bool sendTcpMessage(std::string content) {
		// Send a message to the server
		ssize_t bytesSent = send(s_, content.c_str(), content.size(), 0);
		if (bytesSent == -1) {
			std::cerr << "TcpClient:: Error sending data to the server" << std::endl;
			// Close the socket
			Reconnect();
		}

		return true;
	}

	/**
	 * Resolve the host name or IP address
	 */
	bool Resolve(const std::string &host) {
		std::string error;
		if (isalpha(host[0])) {
			try {
				if ((m_HostPointer = gethostbyname(host.c_str())) == 0) {
					std::cout << ("Unable to get host endpoint by name ") << std::endl;
					error = strerror(errno);
					throw std::runtime_error(error);
				}
			} catch (std::exception &e) {
				std::cout << e.what() << std::endl;
				std::cout << ("Unable to get host endpoint by name ") << std::endl;
				throw std::runtime_error(error);
			}

			return true;
		} else {
			try {
				/* Convert nnn.nnn address to a usable one */
				unsigned int m_addr = inet_addr(host.c_str());
				//            m_HostPointer = gethostbyaddr((char *)&m_addr, 4, AF_INET);
				m_HostPointer = NetworkUtils::create_hostent(host.c_str());
			} catch (std::exception &e) {
				std::cout << e.what() << std::endl;
				std::cout << ("Unable to get host endpoint by name ") << std::endl;
				throw std::runtime_error(error);
			}
			return true;
		}
	}

  private:
	int m_ServerPort;
	char m_ServerName[255];
	struct sockaddr_in m_Server;
	struct hostent *m_HostPointer;
	unsigned int m_addr;
	int s_;
};
