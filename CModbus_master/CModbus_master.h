#pragma once
#include "../CSerialPort/CBasePort.h"

typedef enum class enumFunCode
{
	//功能码
	Enum_BroadCast = 0x00,
	//bit
	Enum_ROStat = 0x01,/*读输出量状态*/
	Enum_RIDis = 0x02,/*读输入离散量*/
	Enum_WSOut = 0x05,/*写单个输出量*/
	Enum_WMOut = 0x0F,/*写多个输出量*/
	//byte
	Enum_RIReg = 0x04,/*读输入寄存器*/
	Enum_ROReg = 0x03,/*读输出寄存器*/
	Enum_WSReg = 0x06,/*写单个寄存器*/
	Enum_WMReg = 0x10,/*写多个寄存器*/
	//Enum_RWReg = 0x17,/*读 / 写寄存器*/
	Enum_MWReg = 0x16,/*屏蔽写寄存器*/
	//Other
	Enum_GCECout = 0x0B,/*获取通信事件计数*/
	Enum_Exception=0x07,/*获取异常状态*/
} EModbusCmd;
enum class ERecMsg :char
{
	E_OK,
	E_Unknown,
	E_chcekErr,
	E_TimeOut,
	E_NOPORT,
	E_Err01,
	E_Err02,
	E_Err03,
	E_Err04
};

class CModbus_master
{
public:
	CModbus_master() :pPort(NULL), m_tv({0,100}) {};
	CModbus_master(CBasePort* nPort);
	virtual ~CModbus_master();
public:
	void SetBasePort(CBasePort* pPort) { this->pPort = pPort; }
	CBasePort* GetBasePort() { return pPort; }
	void SetReadDelayTime(timeval& ntv) { m_tv = ntv; }
	ERecMsg ReadAndAnaylse(vec8_t& nRec);
	/*常规的读操作
	* @param nDriveId 从机设备地址1~247
	* @param cmd 操控命令
	* @param AddressSt 起始地址0x0000~0xffff
	* @param number 线圈数量1~2000
	* @return 见EReMsg
	*/
	void OperatingAddNum(uint8_t& nDriveId, EModbusCmd cmd, uint16_t& AddressSt, uint16_t& number, vec8_t& nRec);
	/*获取错误信息(未开发)*/
	int GetLastError(std::string& nError) {};
	/*读命令 01读线圈
	* @param nDriveId 从机设备地址1~247
	* @param AddressSt 起始地址0x0000~0xffff
	* @param number 线圈数量1~2000
	* @return 见EReMsg
	*/
	ERecMsg readOutputBit(uint8_t nDriveId, uint16_t AddressSt, uint16_t number, vec8_t& nRec);
	/*读命令 02读输入离散量
	* @param nDriveId 从机设备地址1~247
	* @param AddressSt 起始地址0x0000~0xffff
	* @param number 离散量输入数量1~2000
	* @return 见EReMsg
	*/
	ERecMsg readInputBit(uint8_t nDriveId, uint16_t AddressSt, uint16_t number, vec8_t& nRec);

	/*读命令 03读保持寄存器
	* @param nDriveId 从机设备地址1~247
	* @param AddressSt 起始地址0x0000~0xffff
	* @param number 寄存器数量1~125
	* @return 见EReMsg
	*/
	ERecMsg readHoldRegister(uint8_t nDriveId, uint16_t AddressSt, uint16_t number, vec8_t& nRec);

	/*读命令 04读输入寄存器
	* @param nDriveId 从机设备地址1~247
	* @param AddressSt 起始地址0x0000~0xffff
	* @param number 寄存器数量1~125
	* @return 见EReMsg
	*/
	ERecMsg readInputRegister(uint8_t nDriveId, uint16_t AddressSt, uint16_t number, vec8_t& nRec);
	/*读命令 05写单个线圈
	* @param nDriveId 从机设备地址1~247
	* @param AddressSt 起始地址0x0000~0xffff
	* @param value 数据0xFF00 0x0000
	* @return 见EReMsg
	*/
	ERecMsg writeSinglebit(uint8_t nDriveId, uint16_t AddressSt, uint16_t value, vec8_t& nRec);
	/*读命令 06写单个寄存器
		* @param nDriveId 从机设备地址1~247
		* @param AddressSt 起始地址0x0000~0xffff
		* @param value 寄存器数据
		* @return 见EReMsg
		*/
	ERecMsg writeSingleRegister(uint8_t nDriveId, uint16_t AddressSt, uint16_t value, vec8_t& nRec);
	/*读命令 07读取异常状态，仅用于串行链路
	* @param nDriveId 从机设备地址1~247
	* @return 见EReMsg
	*/
	ERecMsg readException(uint8_t nDriveId, vec8_t& nRec);
	/*读命令 0B读取异常状态，仅用于串行链路
	* @param nDriveId 从机设备地址1~247
	* @return 见EReMsg
	*/
	ERecMsg getCommunicationEventCount(uint8_t nDriveId, vec8_t& nRec);
protected:
	//RTU模式
 //【地址】【功能码】【数据量】【数据1】【…】【数据n】【CRC低字节】【CRC高字节】
	static	void crc16(vec8_t& nDataVec);
	static	uint16_t crc16(const uint8_t* buffer, uint16_t buffer_length);
	static ERecMsg checkCrc16(const vec8_t& nDataVec);
private:
	/*read DelayTime*/
	timeval m_tv;
	/*通信接口*/
	CBasePort* pPort;
};
