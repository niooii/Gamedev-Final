#include "server.h"

WORLDSERVER_EXIT_CODE world_server_run()
{
    GDF_Socket* server_socket = GDF_MakeSocket();
    if (!GDF_SocketListen(server_socket, SERVER_PORT))
    {
        return SOCKET_ERR;
    }

    GDF_Socket client_sock;
    while(client_sock = GDF_SocketAccept(server_socket))
    {
        LOG_INFO("CONNECTION ACCEPTED");
    }
}