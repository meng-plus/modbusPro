// CSetupAPI.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
//#include <iostream>
#include <Windows.h>
#include <devguid.h>
#include <SetupAPI.h>
#include "CSetupAPI.h"
#pragma comment (lib, "setupapi.lib")

HDEVINFO         DeviceInfoSet;		//信息结构句柄
SP_DEVINFO_DATA  DeviceInfoData;	//设备信息结构
CSetupAPI::CSetupAPI()
{
	DeviceInfoSet = SetupDiGetClassDevs((LPGUID)&GUID_DEVCLASS_PORTS, // All Classes
		0,
		0,
		DIGCF_PRESENT);//DIGCF_ALLCLASSES); // All devices  
					   /*提供在设备信息结构中使用的控制选项,它可以是*/
					   /* DIGCF_PRESENT - 只返回当前存在的设备。
					   DIGCF_ALLCLASSES - 返回所有已安装的设备。如果这个标志设置了，ClassGuid参数将被忽略。
					   DIGCF_PROFILE - 只返回当前硬件配置文件中的设备。
					   DIGCF_INTERFACEDEVICE - 返回所有支持的设备。
					   DIGCF_DEFAULT - 只返回与系统默认设备相关的设备。 */


	UpData();
}

CSetupAPI::~CSetupAPI()
{

}

uint16_t CSetupAPI::UpData()
{
	BOOL bReult = FALSE;
	DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
	if (DeviceInfoSet == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	//清理数据
	std::vector<std::wstring>().swap(m_ComName);
	/*
	** Enum all ports
	*/

	/*	*函数SP_DEVINFO_DATA一个结构，该结构指定设备信息集中的设备信息元素
	@prama:	DeviceInfoSet 用于返回其的设备信息集的句柄SP_DEVINFO_DATA表示设备信息元素的结构。
	@prama:	index 要检索的设备信息元素的从零到的索引。
	@prama:	DeviceInfoData 指向一个SP_DEVINFO_DATA以接收有关枚举的设备信息元素的信息
	*/
	for (int index = 0; SetupDiEnumDeviceInfo(DeviceInfoSet, index, &DeviceInfoData); index++)
	{
		LPTCH buffer = NULL;
		DWORD  buffersize = 0;
		/*
		** Get friendly name
		*/

		/*	*SetupDiGetDeviceRegistryProperty属性函数检索指定的即插即用设备属性。
		@param:	SPDRP_FRIENDLYNAME指定要检索的属性
		@return: 成功返回：TURE	否则返回FALSE
		*/
		while (!SetupDiGetDeviceRegistryProperty(IN  DeviceInfoSet,/*设备信息集的句柄，其中包含表示要检索即插即用属性的设备的设备信息元素。*/
			IN & DeviceInfoData,/*指向设备结构SP_DEVINFO_DATA的指针，该结构指定DeviceInfoSet 中的设备信息元素。*/
			IN  SPDRP_FRIENDLYNAME,/*函数检索包含REG_SZ友好名称的字符串的一个字符串。*/
			OUT NULL,
			OUT(PBYTE) buffer,/*指向接收正在检索的属性的缓冲区的指针。*/
			IN  buffersize,/*属性缓冲区缓冲区的大小（以字节为单位）*/
			OUT & buffersize))/*指向 DWORD 类型的变量的指针，该变量接收保存请求属性的数据所需的属性缓冲区所需的大小（以字节为单位）*/
		{
			switch (GetLastError())
			{
			case ERROR_INVALID_DATA: break;
			case ERROR_INSUFFICIENT_BUFFER:
				if (buffer)
				{
					LocalFree(buffer);
				}
				buffer = (LPTCH)LocalAlloc(LPTR, buffersize);
				break;
			default:return FALSE; break;
			}
		}

		if (!buffer)
		{
			/*
			** Some device no friendly name, so buffer is null
			** In this situation, we can't compare device ID
			*/
			continue;
		}
		//下面是数据解析过程 主要目的提取出COM号 以及设备名称
		m_ComName.push_back(buffer);

	}
	return bReult;
}
uint16_t CSetupAPI::GetComNumber(bool nUpdata)
{
	if (nUpdata)
	{
		return UpData();
	}
	return (uint16_t)m_ComName.size();
}
const std::vector<std::wstring>& CSetupAPI::GetComNameVec(bool nUpdata)
{
	if (nUpdata)
	{
		UpData();
		return m_ComName;
	}
	return m_ComName;
}
/**根据串口列表顺序 获得对应串口号
* @param idx 串口列表顺序
* @return 串口ID号
*/
uint16_t CSetupAPI::GetComID(uint16_t idx)
{
	std::wstring nName;
	nName = GetComName(idx);
	if (nName.empty())
	{
		return 0;
	}
	else
	{
		return GetComID(nName);
	}
}
/**根据串口名称 解析出对应串口号
* @param idx 串口名称
* @return 串口ID号
*/
constexpr auto afxCOM = L"(COM";
uint16_t CSetupAPI::GetComID(std::wstring& nComName)
{
	int iPortID = 0;
	int dwStart = nComName.find(afxCOM);
	if (dwStart > 0)
	{
#ifdef  UNICODE                     
		swscanf_s(nComName.data() + dwStart, L"(COM%d", &iPortID);
#else   /* UNICODE */              
		sscanf_s(nComName.GetBuffer() + dwStart, "(COM%d", &iPortID);
#endif /* UNICODE */  	
	}
	return iPortID;

}
/**根据串口顺序 获得串口名称
* @param idx 串口顺序
* @return 串口名称
*/
std::wstring CSetupAPI::GetComName(uint16_t idx)
{
	if (m_ComName.size() > idx)
	{
		return m_ComName[idx];
	}
	else
	{
		std::wstring tmp;
		return tmp;
	}
}