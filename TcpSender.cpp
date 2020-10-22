#include "TcpSender.h"
#include "stdafx.h"
#include "Global.h"

TcpSender::~TcpSender()
{
}

bool TcpSender::send(const Message& message)
{
	return false;
}

void TcpSender::receive(const Packet& ackPkt)
{
}

void TcpSender::timeoutHandler(int seqNum)
{
}

bool TcpSender::getWaitingState()
{
	return false;
}
