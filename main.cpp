﻿// main.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Global.h"
#include "RdtSender.h"
#include "RdtReceiver.h"
#include "StopWaitRdtSender.h"
#include "StopWaitRdtReceiver.h"
#include "GoBackNRdtSender.h"
#include "GoBackNRdtReceiver.h"
#include "SelectiveRepeatSender.h"
#include "SelectiveRepeatReceiver.h"
#include "TcpSender.h"
#include "TcpReceiver.h"

int main(int argc, char *argv[])
{
	// RdtSender* ps = new StopWaitRdtSender();
	// RdtReceiver* pr = new StopWaitRdtReceiver();
	// RdtSender *ps = new GoBackNRdtSender();
	// RdtReceiver *pr = new GoBackNRdtReceiver();
	// RdtSender *ps = new SelectiveRepeatSender();
	// RdtReceiver *pr = new SelectiveRepeatReceiver();
	RdtSender *ps = new TcpSender();
	RdtReceiver *pr = new TcpReceiver();
	//	pns->setRunMode(0);  //VERBOS模式
	pns->setRunMode(1); //安静模式
	pns->init();
	pns->setRtdSender(ps);
	pns->setRtdReceiver(pr);
	pns->setInputFile("D:\\Users\\10724\\Desktop\\exp2\\input.txt");
	pns->setOutputFile("D:\\Users\\10724\\Desktop\\exp2\\output.txt");

	pns->start();

	delete ps;
	delete pr;
	delete pUtils; //指向唯一的工具类实例，只在main函数结束前delete
	delete pns;	   //指向唯一的模拟网络环境类实例，只在main函数结束前delete

	return 0;
}
