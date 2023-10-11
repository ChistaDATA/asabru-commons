#pragma once

#include <iostream>
#include <vector>
#include <string>

using namespace std;
typedef struct
{
    std::string name;       // name of the proxy server
    std::string ipaddress;  // ip address of the Remote Endpoint
    int proxyPort;          // port at which the proxy listens
    int port;               // target port to which proxy connects
    int r_w;                //  Read Endpoint or Write EndPoint
    std::string alias;      //  unused
    float reserved;         //  unused
    char Buffer[255];       // unused
} TARGET_ENDPOINT_CONFIG;

//====================================
// The following DTO represents an EndPoint
//             - Needs tidying up
typedef struct
{
    std::string ipaddress; // ip address of the Remote Endpoint
    int port;         //  port at which Remote Listens
    int r_w;          //  Read Endpoint or Write EndPoint
    std::string alias;     //  unused
    float reserved;   //  unused
    char Buffer[255]; // unused
} END_POINT;



