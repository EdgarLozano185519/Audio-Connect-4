#ifndef _NETWORK_H
#define _NETWORK_H
#include <SDL_Net.h>
#include <string.h>

class Network{
private:
    TCPsocket socket;
    IPaddress myip;
    char *hoststring;
    Uint16 portnum;
public:
    Network(){hoststring=0; portnum=0; socket=0;};
    void setPort(Uint16 p){portnum=p;};
    void setIp(const char *c){
        hoststring = new char[strlen(c)+1];
        strcpy(hoststring,c);
    };
    Network(const char *c, Uint16 p){
        portnum=p;
        hoststring = new char[strlen(c)+1];
        strcpy(hoststring, c);
    };
    bool connect();
    bool isConnected();
    void sendNum(int);
    void close(){
        if(socket){
            SDLNet_TCP_Close(socket);
            socket=0;
        }
    };
    ~Network(){
        close();
        if(hoststring) delete [] hoststring;
    };
    int receiveHelper();
    int receiveWait();
};

/*
Opens a connection and establishes tcp socket for use. Everything is handled internally regarding the socket.
*/
bool Network::connect(){
    if(portnum==0 || hoststring==0)
        return false;
    SDLNet_ResolveHost(&myip, hoststring, portnum);
    socket = SDLNet_TCP_Open(&myip);
    if(socket) return true;
    else return false;
}

bool Network::isConnected(){
    if(socket) return true;
    return false;
}

void Network::sendNum(int n){
    char data[10];
    SDLNet_Write32(n, data);
    SDLNet_TCP_Send(socket,data,10);
}

int Network::receiveHelper(){
    char data[10];
    for(int i = 0; i < 10; i++) data[i]='\0';
    int length = SDLNet_TCP_Recv(socket,data,10);
    int n = SDLNet_Read32(data);
    if(length>0) return n;
    else return -1;
}

int Network::receiveWait(){
    int receive = receiveHelper();
    while(receive==-1) receive=receiveHelper();
    return receive;
}

#endif
