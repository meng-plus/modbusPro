// Test-CModbus_master.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "CUserSerialPort.h"
#include "CUserSetupAPI.h"
#include "../CModbus_master/CModbus_master.h"

int main()
{
    CUserSetupAPI mSetupAPI;
    mSetupAPI.Test();
    CUserSerialPort* pPort = new CUserSerialPort();
    int portId;
    puts("请输入串口号");
    std::cin >> portId;
    pPort->SetSerialPort(portId);//选择端口号
    int rec=pPort->Connect();//启动设备连接
    if (rec==-1)
    {
        printf("连接设备失败\r\n");
        return -1;
    }
    CModbus_master mModbus(pPort);
    vec8_t mData;

    int delays=10;
    while (delays)
    {
        printf("\r\n%d delay Rec。。。",delays);
        delays--;
        Sleep(1000);
    }
    //test code
    ERecMsg msg= mModbus.readOutputBit(0x12, 0x0123, 0x12, mData);
    if (msg== ERecMsg::E_OK)
    {
        std::cout << "接收数据清单" << std::endl;
        for (auto& i : mData)
        {
            printf("%02x ", i);
        }
    }
    std::cout << "Hello World!\n";
}