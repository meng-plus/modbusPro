// Test-CSetupAPI.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//  测试CSetupAPI 代码 控制台给出当前串口数量和清单
#include <sstream> 
#include <iostream>
#include "../CSetupAPI/CSetupAPI.h"
int main()
{
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
	std::cout << "测试代码结束\n 回车键关闭程序";
	int tmp=getchar();
}