#pragma once
#include "RdtReceiver.h"
class TcpReceiver : public RdtReceiver
{
private:
    list<Packet> storePacket;
    int recvBase;
    Packet ackPkt;
    const int window_size = 4; //window's size
    const int size = 8;        //quantity of seqnum

public:
    TcpReceiver();
    virtual ~TcpReceiver();

public:
    void receive(const Packet &packet); //接收报文，将被NetworkService调用
    bool checkPktReceive(int seqNum);
};
