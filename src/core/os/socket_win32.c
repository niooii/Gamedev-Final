#include "socket.h"

#ifdef OS_WINDOWS

#pragma comment(lib, "wsock32.lib")
#include <winsock.h>

typedef struct InternalState {
    SOCKET sock;
} InternalState;

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

GDF_Socket* GDF_MakeSocket()
{
    if (!INITIALIZED)
    {
        LOG_ERR("Net subsystem not INITIALIZED.");
        return NULL;
    }

    GDF_Socket* gdf_socket = (GDF_Socket*)GDF_Malloc(sizeof(GDF_Socket), GDF_MEMTAG_IO);
    if (!gdf_socket)
    {
        return NULL;
    }

    InternalState* state = (InternalState*)GDF_Malloc(sizeof(InternalState), GDF_MEMTAG_IO);

    state->sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (state->sock == INVALID_SOCKET)
    {
        GDF_Free(state);
        GDF_Free(gdf_socket);
        return NULL;
    }

    gdf_socket->internals = state;
    return gdf_socket;
}

void GDF_DestroySocket(GDF_Socket* socket)
{
    if (socket)
    {
        InternalState* state = (InternalState*)socket->internals;
        if (state)
        {
            closesocket(state->sock);
            GDF_Free(state);
        }
        free(socket);
    }
}

bool GDF_SocketListen(GDF_Socket* socket, u16 port)
{
    if (!socket || !socket->internals)
    {
        return false;
    }

    InternalState* state = (InternalState*)socket->internals;
    struct sockaddr_in service;

    service.sin_family = AF_INET;
    service.sin_addr.s_addr = INADDR_ANY;
    service.sin_port = htons(port);

    if (bind(state->sock, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
    {
        return false;
    }

    if (listen(state->sock, SOMAXCONN) == SOCKET_ERROR)
    {
        return false;
    }

    return true;
}

GDF_Socket* GDF_SocketAccept(GDF_Socket* socket)
{
    if (!socket || !socket->internals)
    {
        return NULL;
    }

    InternalState* state = (InternalState*)socket->internals;
    SOCKET client_socket = accept(state->sock, NULL, NULL);
    if (client_socket == INVALID_SOCKET)
    {
        return NULL;
    }

    GDF_Socket* client_gdf_socket = (GDF_Socket*)malloc(sizeof(GDF_Socket));
    if (!client_gdf_socket)
    {
        closesocket(client_socket);
        return NULL;
    }

    InternalState* client_state = (InternalState*)malloc(sizeof(InternalState));
    if (!client_state)
    {
        closesocket(client_socket);
        free(client_gdf_socket);
        return NULL;
    }

    client_state->sock = client_socket;
    client_gdf_socket->internals = client_state;

    return client_gdf_socket;
}

bool GDF_SocketConnect(GDF_Socket* socket, const char* address, u16 port)
{
    InternalState* state = (InternalState*)socket->internals;
    struct sockaddr_in service;

    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr(address);
    service.sin_port = htons(port);

    if (connect(state->sock, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
    {
        return false;
    }

    return true;
}

bool GDF_SocketSend(GDF_Socket* socket, const char* buffer, u32 buf_size)
{
    InternalState* state = (InternalState*)socket->internals;
    int r = send(state->sock, buffer, buf_size, 0);
    return r != SOCKET_ERROR;
}

bool GDF_SocketRecv(GDF_Socket* socket, char* buffer, u32 buf_size)
{
    InternalState* state = (InternalState*)socket->internals;
    int r = recv(state->sock, buffer, buf_size, 0);
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
