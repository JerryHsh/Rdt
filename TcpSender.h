#pragma once
#include "RdtSender.h"
class TcpSender : public RdtSender
{
private:
    int base;                  //当前窗口第一个元素序号
    int nextSeqNum;            //当前窗口下一个将要发送的元素序号
    bool waitingState;         // 是否处于window full的状态
    const int window_size = 4; //window's size
    const int size = 8;        //quantity of seqnum
    int ackCount = 0;
    Packet current_packet; //当前发送的分组
    list<Packet> store_packet;

public:
    TcpSender() : base(0), nextSeqNum(0), waitingState(false) {}
    virtual ~TcpSender();

public:
    bool send(const Message &message);  //发送应用层下来的Message，由NetworkService调用,如果发送方成功地将Message发送到网络层，返回true;如果因为发送方处于等待确认状态或发送窗口已满而拒绝发送Message，则返回false
    void receive(const Packet &ackPkt); //接受确认Ack，将被NetworkService调用
    void timeoutHandler(int seqNum);    //Timeout handler，将被NetworkService调用
    bool getWaitingState();
    bool checkExistence(int seq);   //return true if No.seq packet in store_packet
};
