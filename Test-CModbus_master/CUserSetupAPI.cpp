#include "CUserSetupAPI.h"
#include <sstream> 
#include <iostream>
void CUserSetupAPI::Test()
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
}
