#include "GoBackNRdtReceiver.h"
#include "stdafx.h"
#include "Global.h"

GoBackNRdtReceiver::GoBackNRdtReceiver() : expectSequenceNumberRcvd(1)
{

    lastAckPkt.acknum = -1; //初始状态下，上次发送的确认包的确认序号为-1，使得当第一个接受的数据包出错时该确认报文的确认号为-1
    lastAckPkt.checksum = 0;
    lastAckPkt.seqnum = -1; //忽略该字段
    for (int i = 0; i < Configuration::PAYLOAD_SIZE; i++)
    {
        lastAckPkt.payload[i] = '.';
    }
    lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
}

GoBackNRdtReceiver::~GoBackNRdtReceiver()
{
}

void GoBackNRdtReceiver::receive(const Packet &packet)
{
    //calculate checksum
    int checkSum = pUtils->calculateCheckSum(packet);
    //if checksum is right and this packet is expect to recv
    if (checkSum == packet.checksum && packet.seqnum == this->expectSequenceNumberRcvd)
    {
        //load message and send it to applayer
        Message msg;
        memcpy(msg.data, packet.payload, sizeof(packet.payload));
        pns->delivertoAppLayer(RECEIVER, msg);
        lastAckPkt.acknum = packet.seqnum; //确认号为当前受到的报文号（按顺序）
        lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
        pUtils->printPacket("接收方发送确认报文", lastAckPkt);
        pns->sendToNetworkLayer(SENDER, lastAckPkt); //调用模拟网络环境的sendToNetworkLayer，通过网络层发送确认报文到对方
        this->expectSequenceNumberRcvd++;
    }
    else
    {
        if (checkSum != packet.checksum)
        {
            pUtils->printPacket("接收方没有正确收到发送方的报文,数据校验错误", packet);
        }
        else
        {
            pUtils->printPacket("接收方没有正确收到发送方的报文,报文序号不对", packet);
        }
        pUtils->printPacket("接收方重新发送上次的确认报文", lastAckPkt);
        pns->sendToNetworkLayer(SENDER, lastAckPkt); //调用模拟网络环境的sendToNetworkLayer，通过网络层发送上次的确认报文
    }
}