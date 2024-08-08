#pragma once
#include "core.h"

typedef struct GDF_Socket_T* GDF_Socket;

typedef enum GDF_SOCKERR {
    GDF_SOCKERR_NONE,
    GDF_SOCKERR_NOT_INITIALIZED,
    GDF_SOCKERR_NET_DOWN,
    GDF_SOCKERR_ADDR_IN_USE,
    GDF_SOCKERR_INTR,
    GDF_SOCKERR_IN_PROGRESS,
    GDF_SOCKERR_FAULT,
    GDF_SOCKERR_NO_BUFS,
    GDF_SOCKERR_NOT_CONN,
    GDF_SOCKERR_NOT_SOCK,
    GDF_SOCKERR_OP_NOT_SUPP,
    GDF_SOCKERR_SHUTDOWN,
    GDF_SOCKERR_MSG_SIZE,
    GDF_SOCKERR_INVAL,
    GDF_SOCKERR_CONN_ABORTED,
    GDF_SOCKERR_TIMED_OUT,
    GDF_SOCKERR_CONN_REFUSED,
    GDF_SOCKERR_HOST_DOWN,
    GDF_SOCKERR_HOST_UNREACH,
    GDF_SOCKERR_NET_UNREACH,
    GDF_SOCKERR_WOULD_BLOCK,
    GDF_SOCKERR_ACCESS,
    GDF_SOCKERR_UNKNOWN
} GDF_SOCKERR;

bool GDF_InitSockets();
void GDF_ShutdownSockets();

// tcp only for now
GDF_Socket* GDF_MakeSocket();
void GDF_DestroySocket(GDF_Socket* socket);

bool GDF_SocketListen(GDF_Socket* socket, u16 port);
GDF_Socket* GDF_SocketAccept(GDF_Socket* socket);
bool GDF_SocketConnect(GDF_Socket* socket, const char* address, u16 port);
bool GDF_SocketSend(GDF_Socket* socket, const char* buffer, u32 buf_size);
bool GDF_SocketRecv(GDF_Socket* socket, char* buffer, u32 buf_size);

GDF_SOCKERR GDF_SocketGetLastError();