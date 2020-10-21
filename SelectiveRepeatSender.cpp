#include "SelectiveRepeatSender.h"
#include "Global.h"
#include "stdafx.h"

SelectiveRepeatSender::~SelectiveRepeatSender()
{
}

bool SelectiveRepeatSender::send(const Message &message)
{
    if (this->waitingState == true)
        return false;
    //make packet
    this->current_packet.acknum = -1; //ignore this
    this->current_packet.seqnum = this->nextSeqNum;
    memcpy(this->current_packet.payload, message.data, sizeof(message.data));
    this->current_packet.checksum = pUtils->calculateCheckSum(this->current_packet);
    //save the packet until receive it ack
    store_packet.push_back(this->current_packet);
    //send the packet
    pUtils->printPacket("Sender send datagram", this->current_packet);
    //start timer of current packet
    pns->startTimer(SENDER, Configuration::TIME_OUT, this->nextSeqNum);
    pns->sendToNetworkLayer(RECEIVER, this->current_packet);
    this->nextSeqNum = Circulate::add(this->size, this->nextSeqNum, 1);
    //judge if the window is full
    if (Circulate::judge_over(this->size, this->window_size, this->base, this->nextSeqNum))
        this->waitingState = true;
    return true;
}

void SelectiveRepeatSender::receive(const Packet &ackPkt)
{
    //calculate vertify code
    int chechSum = pUtils->calculateCheckSum(ackPkt);
    if (chechSum == ackPkt.checksum && Circulate::judge_inner(this->size, this->base, this->nextSeqNum, ackPkt.acknum))
    {
        //note user
        pUtils->printPacket("Sender succesfully receive ack", ackPkt);
        if (!checkAckExistence(ackPkt.acknum))
        {
            //stop timer
            pns->stopTimer(SENDER, ackPkt.acknum);
            //store ack's seqnum
            this->ack_list.push_back(ackPkt.acknum);
            //move the window
            while (checkAckExistence(this->base))
            {
                this->ack_list.remove(this->base);
                this->store_packet.pop_front();
                this->base = Circulate::add(this->size, this->base, 1);
                this->waitingState = false;
            }
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
            //send the packet start timer of current packet
            pUtils->printPacket("Reach the time limit, resend the datagram", pkt);
            pns->sendToNetworkLayer(RECEIVER, pkt);
            pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);
            break;
        }
    }
}

bool SelectiveRepeatSender::getWaitingState()
{
    return this->waitingState;
}

bool SelectiveRepeatSender::checkAckExistence(int ack)
{
    for (auto i : this->ack_list)
    {
        if (i == ack)
            return true;
    }
    return false;
}
