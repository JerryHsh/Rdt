#pragma once
#include "RdtReceiver.h"
class TcpReceiver : public RdtReceiver
{
private:
    int recvbase;
    const int size = 8;        //quantity of seqnum
    const int window_size = 4; //window's size
    bool waitingState;         // 是否处于window full的状态
    list<Packet> storePacket;
    list<int> recvPktSeqList;

public:
    TcpReceiver();
    virtual ~TcpReceiver();

public:
    void receive(const Packet &packet); //接收报文，将被NetworkService调用
};
