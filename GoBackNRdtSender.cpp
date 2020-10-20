#include "GoBackNRdtSender.h"
#include "stdafx.h"
#include "Global.h"

bool GoBackNRdtSender::send(const Message &message)
{
    //window is full then refuse data
    if (this->waitingState)
        return false;

    //make the packet
    this->current_packet.acknum = -1; //ignore acknum
    this->current_packet.seqnum = this->nextseqnum;
    memcpy(this->current_packet.payload, message.data, sizeof(message.data));
    this->current_packet.checksum = pUtils->calculateCheckSum(this->current_packet);
    //store the packet which has been sent but hasn't receive ack
    this->store_packet.push_back(this->current_packet);
    //send the packet
    pUtils->printPacket("发送方发送报文", this->current_packet);
    if (this->base == this->nextseqnum)
    {
        this->timer_seqnum = this->current_packet.seqnum;
        pns->startTimer(SENDER, Configuration::TIME_OUT, this->timer_seqnum);
    }
    pns->sendToNetworkLayer(RECEIVER, this->current_packet);
    this->nextseqnum++;
    //judge if window is full
    if (this->nextseqnum >= this->base + this->N)
        this->waitingState = true;
    return true;
}

void GoBackNRdtSender::receive(const Packet &ackpkt)
{
    //计算校验和
    int checkSum = pUtils->calculateCheckSum(ackpkt);
    //若校验和正确 （累计确认）
    if (checkSum == ackpkt.checksum)
    {
        this->base = ackpkt.seqnum + 1;

        if (this->base == this->nextseqnum)
        {
            this->store_packet.clear();
            pns->stopTimer(SENDER, this->timer_seqnum);
        }
        else
        {
            while (this->store_packet.front().seqnum != this->base && !this->store_packet.empty())
            {
                this->store_packet.pop_front();
            }
            pns->stopTimer(SENDER, this->timer_seqnum);
            this->timer_seqnum = this->base;
            pns->startTimer(SENDER, Configuration::TIME_OUT, this->timer_seqnum);
        }
    }
}

void GoBackNRdtSender::timeoutHandler(int seqnum)
{
    pns->stopTimer(SENDER, this->timer_seqnum);
    for (auto pkt : this->store_packet)
    {
        pUtils->printPacket("发送方定时器时间到，重发上次发送的报文", pkt);
        pns->sendToNetworkLayer(RECEIVER, pkt);
    }
    pns->startTimer(SENDER, Configuration::TIME_OUT, this->timer_seqnum);
}

bool GoBackNRdtSender::getWaitingState()
{
    return this->waitingState;
}

GoBackNRdtSender::~GoBackNRdtSender()
{
}
