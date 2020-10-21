#include "SelectiveRepeatReceiver.h"
#include "stdafx.h"
#include "Global.h"

SelectiveRepeatReceiver::SelectiveRepeatReceiver() : recvbase(0)
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
        pUtils->printPacket("Receiver successfully receive data segment", packet);
        //send ack no matter if this packet is in the window
        lastAckPkt.acknum = packet.seqnum;
        lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
        pUtils->printPacket("Receiver send ack segment", lastAckPkt);
        pns->sendToNetworkLayer(SENDER, lastAckPkt);
        if (Circulate::judge_inner(this->size, this->recvbase, Circulate::add(this->size, this->recvbase, this->window_size), packet.seqnum)&& !checkPktReceive(packet.seqnum))
        {
            Message msg;
            
                this->recvseq_list.push_back(packet.seqnum);

                if (store_packet.empty())
                {
                    store_packet.push_back(packet);
                }
                else
                {
                    for (auto iter = store_packet.begin();;)
                    {
                        if (Circulate::judge_bigger(this->size, this->recvbase, iter->seqnum, packet.seqnum))
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
                memcpy(msg.data, this->store_packet.front().payload, sizeof(this->store_packet.front().payload));
                pns->delivertoAppLayer(RECEIVER, msg);
                this->store_packet.pop_front();
                this->recvseq_list.remove(this->recvbase);
                this->recvbase = Circulate::add(this->size, this->recvbase, 1);
            }
        }
    }
    else
        pUtils->printPacket("Receiver wrong, wrong checkSUm", packet);
}

bool SelectiveRepeatReceiver::checkPktReceive(int seqnum)
{
    for (auto recvSeqNum : this->recvseq_list)
        if (recvSeqNum == seqnum)
            return true;
    return false;
}
