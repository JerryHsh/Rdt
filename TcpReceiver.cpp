#include "TcpReceiver.h"
#include "stdafx.h"
#include "Global.h"

TcpReceiver::TcpReceiver() : recvBase(0)
{
    this->ackPkt.seqnum = -1; //ignore this segment
    this->ackPkt.acknum = 0;  //expect packet seq num
    for (int i = 0; i < Configuration::PAYLOAD_SIZE; i++)
    {
        this->ackPkt.payload[i] = '.';
    }
    this->ackPkt.checksum = pUtils->calculateCheckSum(this->ackPkt);
}

TcpReceiver::~TcpReceiver()
{
}

void TcpReceiver::receive(const Packet &packet)
{
    int checkSum = pUtils->calculateCheckSum(packet);
    if (checkSum == packet.checksum)
    {
        if (checkPktReceive(packet.seqnum) || !Circulate::judge_inner(this->size, this->recvBase, Circulate::add(this->size, this->recvBase, this->window_size), packet.seqnum))
        {
            this->ackPkt.acknum = this->recvBase;
            this->ackPkt.checksum = pUtils->calculateCheckSum(this->ackPkt);
            pUtils->printPacket("Receiver already have this pkt", packet);
            pns->sendToNetworkLayer(SENDER, this->ackPkt);
        }
        else
        {
            Message msg;
            pUtils->printPacket("Receiver receive packet successfully", packet);
            if (packet.seqnum == this->recvBase)
            {
                this->storePacket.push_front(packet);
                while (checkPktReceive(this->recvBase))
                {
                    memcpy(msg.data, storePacket.front().payload, sizeof(storePacket.front().payload));
                    pns->delivertoAppLayer(RECEIVER, msg);
                    this->storePacket.pop_front();
                    this->recvBase = Circulate::add(this->size, this->recvBase, 1);
                }
                this->ackPkt.acknum = this->recvBase;
                this->ackPkt.checksum = pUtils->calculateCheckSum(this->ackPkt);
                pns->sendToNetworkLayer(SENDER, this->ackPkt);
            }
            else
            {
                for (auto iter = storePacket.begin();;iter++)
                {
                    if (iter == storePacket.end())
                    {
                        storePacket.insert(iter, packet);
                        break;
                    }
                    if (iter->seqnum > packet.seqnum)
                    {
                        storePacket.insert(iter, packet);
                        break;
                    }
                }
                this->ackPkt.acknum = this->recvBase;
                this->ackPkt.checksum = pUtils->calculateCheckSum(this->ackPkt);
                pns->sendToNetworkLayer(SENDER, this->ackPkt);
            }
        }
    }
}

bool TcpReceiver::checkPktReceive(int seqNum)
{
    for (auto pkt : this->storePacket)
        if (seqNum == pkt.seqnum)
            return true;
    return false;
}
