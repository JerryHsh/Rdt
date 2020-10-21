#include "RdtReceiver.h"
#pragma once
class GoBackNRdtReceiver : public RdtReceiver
{
private:
    int expectSequenceNumberRcvd; // 期待收到的下一个报文序号
    Packet lastAckPkt;            //上次发送的确认报文
    const int size = 8;              //the quantity of seq num(3bit)
public:
    GoBackNRdtReceiver();
    virtual ~GoBackNRdtReceiver();
    void receive(const Packet &packet);
};
