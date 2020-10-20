#include "SelectiveRepeatReceiver.h"
#include "stdafx.h"
#include "Global.h"

SelectiveRepeatReceiver::SelectiveRepeatReceiver() : recvbase(1)
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

SelectiveRepeatReceiver::~SelectiveRepeatReceiver()
{
}

void SelectiveRepeatReceiver::receive(const Packet &packet)
{
    //check if checkSum is right
    int checkSum = pUtils->calculateCheckSum(packet);
    if (checkSum == packet.checksum)
    {
        pUtils->printPacket("接收方正确收到发送方的报文", packet);
        //send ack no matter if this packet is in the window
        lastAckPkt.acknum = packet.acknum;
        lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
        pUtils->printPacket("接收方发送确认报文", lastAckPkt);
        pns->sendToNetworkLayer(SENDER, lastAckPkt);
        if (packet.seqnum >= this->recvbase)
        {
            Message msg;

            this->recvPktSeq.push_back(packet.seqnum);

            if (store_packet.empty())
            {
                store_packet.push_back(packet);
            }
            else
            {
                for (auto iter = store_packet.begin();;)
                {
                    if (iter->seqnum > packet.seqnum)
                    {
                        store_packet.insert(iter, packet);
                        break;
                    }
                    //not completed
                    iter++;
                    if (iter == store_packet.end())
                    {
                        store_packet.insert(iter, packet);
                        break;
                    }
                }
            }

            while (checkPktReceive(this->recvbase))
            {
                memcpy(msg.data, packet.payload, sizeof(packet.payload));
                pns->delivertoAppLayer(RECEIVER, msg);
                this->store_packet.pop_front();
                this->recvPktSeq.remove(this->recvbase);
                this->recvbase++;
            }
        }
    }
}

bool SelectiveRepeatReceiver::checkPktReceive(int seqnum)
{
    for (auto recvSeqNum : this->recvPktSeq)
        if (recvSeqNum == seqnum)
            return true;
    return false;
}
