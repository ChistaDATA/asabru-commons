#include "ProtocolHelper.h"
#include "Utils.h"
#include <netinet/tcp.h>

#define CHISTA_TCP_KEEPINTVL 75 // TCP keep alive interval in seconds

std::string ProtocolHelper::GetIPAddressAsString(struct sockaddr_in *client_addr)
{
    struct sockaddr_in *pV4Addr = client_addr;
    struct in_addr ipAddr = pV4Addr->sin_addr;
    char str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &ipAddr, str, INET_ADDRSTRLEN);
    return std::string(str);
}

std::string ProtocolHelper::GetIPPortAsString(struct sockaddr_in *client_addr)
{
    struct sockaddr_in *pV4Addr = client_addr;
    in_port_t ipPort = pV4Addr->sin_port;
    return std::to_string(ipPort);
}

bool ProtocolHelper::SetReadTimeOut(SOCKET s, long second)
{
    struct timeval tv;
    tv.tv_sec = second;
    tv.tv_usec = 0;
    int timeoutValSizeInTimeVal = sizeof(timeval);
    if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO,
                   (const char *)&tv, timeoutValSizeInTimeVal) != SOCKET_ERROR)
    {
        return true;
    }
    return false;
}

bool ProtocolHelper::SetKeepAlive(SOCKET s, int flags)
{
    if (setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, (void *)&flags, sizeof(flags)) == SOCKET_ERROR) {
        perror("ERROR: setsocketopt(), SO_KEEPALIVE");
        return false;
    }
    int tcp_keep_time = CHISTA_TCP_KEEPINTVL;
#if defined(TCP_KEEPIDLE)
    // using the same value for TCP_KEEPIDLE and TCP_KEEPINTVL
    if (setsockopt(s, IPPROTO_TCP, TCP_KEEPIDLE, (void *)&tcp_keep_time, sizeof(int)) == SOCKET_ERROR) {
        perror("ERROR: setsocketopt(), TCP_KEEPIDLE");
        return false;
    }
    if (setsockopt(s, IPPROTO_TCP, TCP_KEEPINTVL, (void *)&tcp_keep_time,
             sizeof(int)) == SOCKET_ERROR) {
        perror("ERROR: setsocketopt(), TCP_KEEPINTVL");
        return false;
    }
#elif defined(TCP_KEEPALIVE)
    if (setsockopt(s, IPPROTO_TCP, TCP_KEEPALIVE, &tcp_keep_time, sizeof(tcp_keep_time))) {
        perror("ERROR: setsocketopt(), TCP_KEEPALIVE");
        return false;
    }
#endif
    return true;
}

bool ProtocolHelper::ReadSocketBuffer(SOCKET s, char *bfr, int size, int *num_read)
{
    int RetVal = recv(s, bfr, size, 0);
    if (RetVal == 0 || RetVal == -1)
    {
        *num_read = RetVal;
        return false;
    }
    *num_read = RetVal;
    return true;
}
bool ProtocolHelper::ReadSocketBuffer(SOCKET s, char *bfr, int size)
{
    int RetVal = recv(s, bfr, size, 0);
    if (RetVal == 0 || RetVal == -1)
    {
        return false;
    }

    return true;
}
bool ProtocolHelper::WriteSocketBuffer(SOCKET s, char *bfr, int size)
{
    int RetVal = send(s, bfr, size, 0);
    if (RetVal == 0 || RetVal == -1)
        return false;
    return true;
}
