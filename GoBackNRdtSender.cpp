#include "GoBackNRdtSender.h"
#include "stdafx.h"
#include "Global.h"

# pragma warning (disable:4819)

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
    pUtils->printPacket("Sender send datagram", this->current_packet);
    if (this->base == this->nextseqnum)
    {
        this->timer_seqnum = this->current_packet.seqnum;
        pns->startTimer(SENDER, Configuration::TIME_OUT, this->timer_seqnum);
    }
    pns->sendToNetworkLayer(RECEIVER, this->current_packet);
    this->nextseqnum = Circulate::add(this->size, this->nextseqnum, 1);
    //judge if window is full
    if (Circulate::judge_over(this->size, this->window_size, this->base, this->nextseqnum))
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
        pUtils->printPacket("Sender receive ack successfully", ackpkt);
        if ((ackpkt.acknum != -1)&&(Circulate::judge_bigger(this->size, this->base, this->nextseqnum, ackpkt.acknum)))
        {
            this->base = Circulate::add(this->size, ackpkt.acknum, 1);
            this->waitingState = false;
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
}

void GoBackNRdtSender::timeoutHandler(int seqnum)
{
    pns->stopTimer(SENDER, this->timer_seqnum);
    for (auto pkt : this->store_packet)
    {
        pUtils->printPacket("Reach time limit, resent data segment", pkt);
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
