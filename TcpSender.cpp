#include "TcpSender.h"
#include "stdafx.h"
#include "Global.h"

TcpSender::~TcpSender()
{
}

bool TcpSender::send(const Message &message)
{
	//make send packet
	if (this->waitingState == true)
		return false;
	this->current_packet.seqnum = nextSeqNum;
	this->current_packet.acknum = -1; //ignore this segment
	memcpy(this->current_packet.payload, message.data, sizeof(message.data));
	this->current_packet.checksum = pUtils->calculateCheckSum(this->current_packet);
	//save send packet
	this->store_packet.push_back(this->current_packet);
	//start timer
	if (this->base == this->nextSeqNum)
		pns->startTimer(SENDER, Configuration::TIME_OUT, nextSeqNum);
	//send packet
	pns->sendToNetworkLayer(RECEIVER, this->current_packet);
	pUtils->printPacket("@Sender send datagram", this->current_packet);
	this->nextSeqNum = Circulate::add(this->size, this->nextSeqNum, 1);
	//judge if the window if full
	if (Circulate::judge_over(this->size, this->window_size, this->base, this->nextSeqNum))
		this->waitingState = true;
	return true;
}

void TcpSender::receive(const Packet &ackPkt)
{
	int checkSum = pUtils->calculateCheckSum(ackPkt);
	if (checkSum == ackPkt.checksum)
	{
		pUtils->printPacket("$Sender receive packet successfully", ackPkt);
		if (Circulate::judge_inner(this->size, this->base, Circulate::add(this->size, this->nextSeqNum, 1), ackPkt.acknum))
		{
			if (this->base == ackPkt.acknum)
			{
				this->ackCount++;
				if (ackCount == 3)
				{
					this->ackCount = 0;
					pns->stopTimer(SENDER, this->base);
					pns->sendToNetworkLayer(RECEIVER, this->store_packet.front());
					pUtils->printPacket("!@Triple ack,sender resend datagram", this->store_packet.front());
					pns->startTimer(SENDER, Configuration::TIME_OUT, this->base);
				}
			}
			else
			{
				pns->stopTimer(SENDER, this->base);
				this->base = ackPkt.acknum;
				this->ackCount = 0;
				while (!this->store_packet.empty())
				{
					if (this->store_packet.front().seqnum == this->base)
						break;
					this->store_packet.pop_front();
				}
				if (!this->store_packet.empty())
					pns->startTimer(SENDER, Configuration::TIME_OUT, this->base);
				this->waitingState = false;
			}
		}
	}
}

void TcpSender::timeoutHandler(int seqNum)
{
	pns->stopTimer(SENDER, seqNum);
	pns->sendToNetworkLayer(RECEIVER, this->store_packet.front());
	pUtils->printPacket("!@Reach time limit,sender resend datagram", this->store_packet.front());
	pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);
}

bool TcpSender::getWaitingState()
{
	return this->waitingState;
}

bool TcpSender::checkExistence(int seq)
{
	for (auto pkt : this->store_packet)
		if (pkt.seqnum == seq)
			return true;
	return false;
}
