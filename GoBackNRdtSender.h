#pragma once
#include "RdtSender.h"

class GoBackNRdtSender : public RdtSender
{
private:
    int base;       //current window start point
    int nextseqnum; //current sending point
    int timer_seqnum;
	bool waitingState;				// 是否处于window full的状态
    const int N = 4; //window's size
    Packet current_packet;
    //Packet *send_packet = new Packet[N];//存储已经发出但没有确认的数据包
    list<Packet> store_packet;

public:
    bool send(const Message &message);  //发送应用层下来的Message，由NetworkService调用,如果发送方成功地将Message发送到网络层，返回true;如果因为发送方处于等待确认状态或发送窗口已满而拒绝发送Message，则返回false
    void receive(const Packet &ackPkt); //接受确认Ack，将被NetworkService调用
    void timeoutHandler(int seqNum);    //Timeout handler，将被NetworkService调用
    bool getWaitingState();             //返回RdtSender是否处于等待状态，如果发送方正等待确认或者发送窗口已满，返回true
public:
    GoBackNRdtSender() : base(1), nextseqnum(1), timer_seqnum(1),waitingState(false){}
    virtual ~GoBackNRdtSender();
};
