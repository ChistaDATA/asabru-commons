#pragma once

#include <iostream>
#include <vector>
#include <string>

using namespace std;

const string CRLF = "\r\n";

typedef struct
{
    int port;
    string protocol;
    string name;
    string handler;
} SERVICE;

typedef struct
{
    string node;
    string host;
    vector<SERVICE> services;
} LOCAL_END_POINT;

typedef struct
{
    string clusterName;
    vector<LOCAL_END_POINT> endpoints;
} LOCAL_END_POINTS;

typedef struct
{
    string name;
    LOCAL_END_POINTS localEndpoints;
} WORK_SPACE;

typedef struct
{
    string endPointName;
    bool readWrite;
    string host;
    vector<SERVICE> services;
    string ipaddress;
} REMOTE_END_POINT;

typedef struct
{
    vector<REMOTE_END_POINT> endPoints;
    string clusterName;
} CLUSTER;

typedef struct
{
    vector<CLUSTER> clusters;
    vector<string> handlers;
} PROXY_CONFIG;

typedef struct
{
    string clusterName;
    string endPointName;
    string serviceName;
    string handlerName;
    int port;
    string protocol;
} RESOLVE_CONFIG;

typedef struct
{
    string ipaddress; // ip address of the Remote Endpoint
    int port;         //  port at which Remote Listens
    int r_w;          //  Read Endpoint or Write EndPoint
    string alias;     //  unused
    float reserved;   //  unused
    char Buffer[255]; // unused
    void * handler;
} RESOLVE_ENDPOINT_RESULT;

//====================================
// The following DTO represents an EndPoint
//             - Needs tidying up
typedef struct
{
    string ipaddress; // ip address of the Remote Endpoint
    int port;         //  port at which Remote Listens
    int r_w;          //  Read Endpoint or Write EndPoint
    string alias;     //  unused
    float reserved;   //  unused
    char Buffer[255]; // unused
} END_POINT;
