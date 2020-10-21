#pragma once
#include "RdtReceiver.h"
class SelectiveRepeatReceiver : public RdtReceiver
{
private:
    list<int> recvseq_list;
    list<Packet> store_packet;
    int recvbase;
    Packet lastAckPkt;
    const int window_size = 4; //window's size
    const int size = 8;        //quantity of seqnum

public:
    SelectiveRepeatReceiver();
    virtual ~SelectiveRepeatReceiver();

public:
    void receive(const Packet &packet); //接收报文，将被NetworkService调用
    bool checkPktReceive(int seqnum);//检查包裹是否被收到
};  
