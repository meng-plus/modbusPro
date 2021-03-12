// Test-CSerialPort.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
// Note：Win平台下的测试代码，测试串口数据的收发功能，部分源码摘自libmodbus
//测试场景为 单串口的 短接Rx Tx 自动发送10个数据 随即正确接收 则表明正常
#include <sstream> 
#include <iostream>
#include "../CSetupAPI/CSetupAPI.h"
#include "../CSerialPort/CSerialPort.h"

int main()
{
	/*获取串口列表*/
	CSetupAPI mCom;
	uint16_t num = mCom.GetComNumber();
	auto nComName = mCom.GetComNameVec();
	std::wstringstream ss;
	ss << "检测到串口数量：" << mCom.GetComNumber() << std::endl;
	for (auto& var : nComName)
	{
		ss << "名称：" << var << std::endl;
		ss << "ID：" << mCom.GetComID(var) << std::endl;
	}
	std::wcout << ss.str() << std::endl;
	/*获取打开指定串口*/
	int portId;
	puts("请输入串口号");
	std::cin >>portId;
	CSerialPort m_Port;
	m_Port.InitSerial(portId, 9600, 'N', 8, 1);
	if (m_Port.Connect() == NULL)
	{
		std::cout << "连接成功 测试发送" << std::endl;
		SerialDataVec mSendData;
		mSendData.reserve(10);
		for (char i = 0; i < 10; i++)
		{
			mSendData.push_back(i * 2);
		}
		m_Port.send(mSendData);
		mSendData.clear();
		Sleep(10);
		timeval val = { 0,5 };
		int ilen = m_Port.read(mSendData, &val);
		printf("数据长度 %d\r\n", ilen);
		for (auto& var : mSendData)
		{
			std::cout <<int(var) << std::endl;
		}
	}
	else
	{
		std::cout <<"指定串口无法打开" << std::endl;
	}
	int rec=getchar();
}