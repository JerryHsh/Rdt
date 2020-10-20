#include "SelectiveRepeatSender.h"
#include "Global.h"
#include "StopWaitRdtSender.h"

SelectiveRepeatSender::~SelectiveRepeatSender()
{
}

bool SelectiveRepeatSender::send(const Message &message)
{
    if (this->waitingState == false)
        return false;
    //make packet
    this->current_packet.acknum = -1; //ignore this
    this->current_packet.seqnum = this->next_seqnum;
    memcpy(this->current_packet.payload, message.data, sizeof(message.data));
    this->current_packet.checksum = pUtils->calculateCheckSum(this->current_packet);
    //save the packet until receive it ack
    store_packet.push_back(this->current_packet);
    //send the packet
    pUtils->printPacket("发送方发送报文", this->current_packet);
    //start timer of current packet
    pns->startTimer(SENDER, Configuration::TIME_OUT, this->next_seqnum);
    pns->sendToNetworkLayer(RECEIVER, this->current_packet);
    this->next_seqnum++;
    //judge if the window is full
    if (this->next_seqnum >= this->base + this->N)
        this->waitingState = true;
    return true;
}

void SelectiveRepeatSender::receive(const Packet &ackPkt)
{
    //calculate vertify code
    int chechSum = pUtils->calculateCheckSum(ackPkt);
    if (chechSum == ackPkt.checksum && ackPkt.seqnum >= this->base)
    {

        //stop timer and note user
        pUtils->printPacket("发送方正确收到确认", ackPkt);
        pns->stopTimer(SENDER, ackPkt.seqnum);
        //store ack's seqnum
        this->ack_pkt_num.push_back(ackPkt.seqnum);
        //move the window
        while (checkPktAck(this->base))
        {
            this->ack_pkt_num.remove(this->base);
            this->store_packet.pop_front();
            this->base++;
            this->waitingState = false;
        }
    }
}

void SelectiveRepeatSender::timeoutHandler(int seqNum)
{
    for (auto pkt : store_packet)
    {
        if (pkt.seqnum == seqNum)
        {
            pns->stopTimer(SENDER, seqNum);
            pUtils->printPacket("发送方定时器时间到，重发上次发送的报文", pkt);
            pns->sendToNetworkLayer(SENDER, pkt);
            pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);
        }
    }
}

bool SelectiveRepeatSender::getWaitingState()
{
    return this->waitingState;
}

bool SelectiveRepeatSender::checkPktAck(int seqnum)
{
    for (auto ackSeqNum : this->ack_pkt_num)
        if (ackSeqNum == seqnum)
            return true;
    return false;
}
