#pragma once
#include "RdtSender.h"
class SelectiveRepeatSender : public RdtSender
{
private:
    int base;              //当前窗口第一个元素序号
    int next_seqnum;       //当前窗口下一个将要发送的元素序号
    bool waitingState;     // 是否处于window full的状态
    const int N = 16;      //window's size
    Packet current_packet; //当前发送的分组
    //Packet *send_packet = new Packet[N];//存储已经发出但没有确认的数据包
    list<Packet> store_packet;
    list<int> ack_pkt_num; //store the seqnum of which the packet receive ack

public:
    SelectiveRepeatSender() : base(1), next_seqnum(1), waitingState(false) {}
    virtual ~SelectiveRepeatSender();
    bool send(const Message &message) = 0;  //发送应用层下来的Message，由NetworkService调用,如果发送方成功地将Message发送到网络层，返回true;如果因为发送方处于等待确认状态或发送窗口已满而拒绝发送Message，则返回false
    void receive(const Packet &ackPkt) = 0; //接受确认Ack，将被NetworkService调用
    void timeoutHandler(int seqNum) = 0;    //Timeout handler，将被NetworkService调用
    bool getWaitingState() = 0;             //返回RdtSender是否处于等待状态，如果发送方正等待确认或者发送窗口已满，返回true
    bool checkPktAck(int seqnum);           //若该编号分组已收到ack返回true 否则返回false
};
