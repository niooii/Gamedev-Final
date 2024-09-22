#include "socket.h"

#ifdef OS_WINDOWS

#pragma comment(lib, "wsock32.lib")
#include <winsock.h>

typedef struct GDF_Socket_T {
    SOCKET sock;
} GDF_Socket_T;

static bool INITIALIZED = false;

bool GDF_InitSockets()
{
    WSADATA data;
    if (WSAStartup(MAKEWORD(2, 2), &data) != 0)
    {
        return false;
    }

    INITIALIZED = true;
    return true;
}

void GDF_ShutdownSockets()
{
    if (INITIALIZED)
    {
        WSACleanup();
        INITIALIZED = false;
    }
}

GDF_Socket GDF_MakeSocket()
{
    if (!INITIALIZED)
    {
        LOG_ERR("Net subsystem not INITIALIZED.");
        return NULL;
    }

    GDF_Socket gdf_socket = (GDF_Socket)GDF_Malloc(sizeof(GDF_Socket_T), GDF_MEMTAG_IO);
    if (!gdf_socket)
    {
        return NULL;
    }

    gdf_socket->sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (gdf_socket->sock == INVALID_SOCKET)
    {
        GDF_Free(gdf_socket);
        return NULL;
    }

    return gdf_socket;
}

void GDF_DestroySocket(GDF_Socket socket)
{
    if (socket)
    {
        GDF_Free(socket);
    }
}

bool GDF_SocketListen(GDF_Socket socket, u16 port)
{
    struct sockaddr_in service;

    service.sin_family = AF_INET;
    service.sin_addr.s_addr = INADDR_ANY;
    service.sin_port = htons(port);

    if (bind(socket->sock, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
    {
        return false;
    }

    if (listen(socket->sock, SOMAXCONN) == SOCKET_ERROR)
    {
        return false;
    }

    return true;
}

GDF_Socket GDF_SocketAccept(GDF_Socket socket)
{
    if (!socket)
    {
        return NULL;
    }

    SOCKET client_socket = accept(socket->sock, NULL, NULL);
    if (client_socket == INVALID_SOCKET)
    {
        return NULL;
    }

    GDF_Socket client_sock = GDF_Malloc(sizeof(GDF_Socket_T), GDF_MEMTAG_APPLICATION);
    if (!client_sock)
    {
        closesocket(client_socket);
        GDF_Free(client_sock);
        return NULL;
    }

    client_sock->sock = client_socket;

    return client_sock;
}

bool GDF_SocketConnect(GDF_Socket socket, const char* address, u16 port)
{
    struct sockaddr_in service;

    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr(address);
    service.sin_port = htons(port);

    if (connect(socket->sock, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
    {
        return false;
    }

    return true;
}

bool GDF_SocketSend(GDF_Socket socket, const char* buffer, u32 buf_size)
{
    int r = send(socket->sock, buffer, buf_size, 0);
    return r != SOCKET_ERROR;
}

bool GDF_SocketRecv(GDF_Socket socket, char* buffer, u32 buf_size)
{
    int r = recv(socket->sock, buffer, buf_size, 0);
    return r != SOCKET_ERROR;
}

GDF_SOCKERR GDF_SocketGetLastError()
{
    int error = WSAGetLastError();
    switch (error)
    {
        case 0: return GDF_SOCKERR_NONE;
        case WSANOTINITIALISED: return GDF_SOCKERR_NOT_INITIALIZED;
        case WSAENETDOWN: return GDF_SOCKERR_NET_DOWN;
        case WSAEADDRINUSE: return GDF_SOCKERR_ADDR_IN_USE;
        case WSAEINTR: return GDF_SOCKERR_INTR;
        case WSAEINPROGRESS: return GDF_SOCKERR_IN_PROGRESS;
        case WSAEFAULT: return GDF_SOCKERR_FAULT;
        case WSAENOBUFS: return GDF_SOCKERR_NO_BUFS;
        case WSAENOTCONN: return GDF_SOCKERR_NOT_CONN;
        case WSAENOTSOCK: return GDF_SOCKERR_NOT_SOCK;
        case WSAEOPNOTSUPP: return GDF_SOCKERR_OP_NOT_SUPP;
        case WSAESHUTDOWN: return GDF_SOCKERR_SHUTDOWN;
        case WSAEMSGSIZE: return GDF_SOCKERR_MSG_SIZE;
        case WSAEINVAL: return GDF_SOCKERR_INVAL;
        case WSAECONNABORTED: return GDF_SOCKERR_CONN_ABORTED;
        case WSAETIMEDOUT: return GDF_SOCKERR_TIMED_OUT;
        case WSAECONNREFUSED: return GDF_SOCKERR_CONN_REFUSED;
        case WSAEHOSTDOWN: return GDF_SOCKERR_HOST_DOWN;
        case WSAEHOSTUNREACH: return GDF_SOCKERR_HOST_UNREACH;
        case WSAENETUNREACH: return GDF_SOCKERR_NET_UNREACH;
        case WSAEWOULDBLOCK: return GDF_SOCKERR_WOULD_BLOCK;
        case WSAEACCES: return GDF_SOCKERR_ACCESS;
        default: return GDF_SOCKERR_UNKNOWN;
    }
}

#endif
