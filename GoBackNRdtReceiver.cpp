#include "GoBackNRdtReceiver.h"
#include "stdafx.h"
#include "Global.h"

# pragma warning (disable:4819)

GoBackNRdtReceiver::GoBackNRdtReceiver() : expectSequenceNumberRcvd(0)
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
        pUtils->printPacket("Recevier receive packet successfully", packet);
        //load message and send it to applayer
        Message msg;
        memcpy(msg.data, packet.payload, sizeof(packet.payload));
        pns->delivertoAppLayer(RECEIVER, msg);
        lastAckPkt.acknum = packet.seqnum; //确认号为当前受到的报文号（按顺序）
        lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
        pUtils->printPacket("Recevier send ack packet", lastAckPkt);
        pns->sendToNetworkLayer(SENDER, lastAckPkt); //调用模拟网络环境的sendToNetworkLayer，通过网络层发送确认报文到对方
        this->expectSequenceNumberRcvd = Circulate::add(this->size, this->expectSequenceNumberRcvd, 1);
    }
    else
    {
        if (checkSum != packet.checksum)
        {
            pUtils->printPacket("Receive wrong, checkSum wrong", packet);
        }
        else
        {
            pUtils->printPacket("Receive wrong, seqnum wrong", packet);
        }
        pUtils->printPacket("Receiver resend ack", lastAckPkt);
        pns->sendToNetworkLayer(SENDER, lastAckPkt); //调用模拟网络环境的sendToNetworkLayer，通过网络层发送上次的确认报文
    }
}