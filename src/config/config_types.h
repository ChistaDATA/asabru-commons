#pragma once

#include <iostream>
#include <vector>
#include <string>

typedef struct
{
    std::string ipaddress; // ip address of the Remote Endpoint
    int port;              // target port to which proxy connects
    int r_w;               //  Read Endpoint or Write EndPoint
    std::string alias;     //  unused
    float reserved;        //  unused
    char Buffer[255];      // unused
    int weight;            // This property will be used in the case of weighted round robin load balancing
} RESOLVED_SERVICE;

typedef struct
{
    std::string name; // name of the proxy server
    int proxyPort;    // port at which the proxy listens
    std::vector<RESOLVED_SERVICE> services;
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



