// CModbus_master.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "CModbus_master.h"

CModbus_master::CModbus_master(CBasePort* nPort)
	:pPort(nPort), m_tv({ 0,100 })
{
}

CModbus_master::~CModbus_master()
{
	if (pPort)
	{
		delete pPort;
		pPort = NULL;
	}
}
ERecMsg CModbus_master::ReadAndAnaylse(vec8_t& nRec)
{
	ERecMsg msg;
	uint8_t nlastCmd;
	nlastCmd = nRec[1] & 0x7F;
	EPortMsg erec = pPort->read(nRec, &m_tv);
	switch (erec)
	{
	case EPortMsg::E_Ok:
		msg = checkCrc16(nRec);
		if (msg == ERecMsg::E_OK)
		{
			if ((nRec[1] & 0x7F) != nlastCmd)
			{
				msg = ERecMsg::E_Unknown;
			}
			else if (nRec[1] & 0x80)
			{
				switch (nRec[2] & 0x0F)
				{
				case 1:msg = ERecMsg::E_Err01; break;
				case 2:msg = ERecMsg::E_Err02; break;
				case 3:msg = ERecMsg::E_Err03; break;
				case 4:msg = ERecMsg::E_Err04; break;
				default:
					msg = ERecMsg::E_chcekErr;
					break;
				}
			}
		}
		break;
	default:
		msg = ERecMsg::E_TimeOut;
		break;
	}
	return msg;
}
void CModbus_master::OperatingAddNum(uint8_t& nDriveId, EModbusCmd cmd, uint16_t& AddressSt, uint16_t& number, vec8_t& nRec)
{
	nRec.resize(0);
	nRec.reserve(8);
	nRec.push_back(nDriveId);
	nRec.push_back(uint8_t(cmd));
	nRec.push_back((AddressSt >> 8) & 0xFF);
	nRec.push_back((AddressSt) & 0xFF);
	nRec.push_back((number >> 8) & 0xFF);
	nRec.push_back((number) & 0xFF);
	crc16(nRec);
}
ERecMsg CModbus_master::readOutputBit(uint8_t nDriveId, uint16_t AddressSt, uint16_t number, vec8_t& nRec)
{
	ERecMsg msg = ERecMsg::E_TimeOut;
	OperatingAddNum(nDriveId, EModbusCmd::Enum_ROStat, AddressSt, number, nRec);
	if (pPort != NULL)
	{
		pPort->send(nRec);
		msg = ReadAndAnaylse(nRec);
	}
	else
	{
		msg = ERecMsg::E_NOPORT;
	}
	return msg;
}
ERecMsg CModbus_master::readInputBit(uint8_t nDriveId, uint16_t AddressSt, uint16_t number, vec8_t& nRec)
{
	ERecMsg msg = ERecMsg::E_TimeOut;
	OperatingAddNum(nDriveId, EModbusCmd::Enum_RIDis, AddressSt, number, nRec);
	if (pPort != NULL)
	{
		pPort->send(nRec);
		msg = ReadAndAnaylse(nRec);
	}
	else
	{
		msg = ERecMsg::E_NOPORT;
	}
	return msg;
}
ERecMsg CModbus_master::readHoldRegister(uint8_t nDriveId, uint16_t AddressSt, uint16_t number, vec8_t& nRec)
{
	ERecMsg msg = ERecMsg::E_TimeOut;
	OperatingAddNum(nDriveId, EModbusCmd::Enum_ROReg, AddressSt, number, nRec);
	if (pPort != NULL)
	{
		pPort->send(nRec);
		msg = ReadAndAnaylse(nRec);
	}
	else
	{
		msg = ERecMsg::E_NOPORT;
	}
	return msg;
}
ERecMsg CModbus_master::readInputRegister(uint8_t nDriveId, uint16_t AddressSt, uint16_t number, vec8_t& nRec)
{
	ERecMsg msg = ERecMsg::E_TimeOut;
	OperatingAddNum(nDriveId, EModbusCmd::Enum_RIReg, AddressSt, number, nRec);
	if (pPort != NULL)
	{
		pPort->send(nRec);
		msg = ReadAndAnaylse(nRec);
	}
	else
	{
		msg = ERecMsg::E_NOPORT;
	}
	return msg;
}
ERecMsg CModbus_master::writeSinglebit(uint8_t nDriveId, uint16_t AddressSt, uint16_t value, vec8_t& nRec)
{
	ERecMsg msg = ERecMsg::E_TimeOut;
	value = value != 0x0000 ? 0xFF00 : 0x0000;
	OperatingAddNum(nDriveId, EModbusCmd::Enum_WSOut, AddressSt, value, nRec);
	if (pPort != NULL)
	{
		pPort->send(nRec);
		msg = ReadAndAnaylse(nRec);
	}
	else
	{
		msg = ERecMsg::E_NOPORT;
	}
	return msg;
}
ERecMsg CModbus_master::writeSingleRegister(uint8_t nDriveId, uint16_t AddressSt, uint16_t value, vec8_t& nRec)
{
	ERecMsg msg = ERecMsg::E_TimeOut;
	value = value != 0x0000 ? 0xFF00 : 0x0000;
	OperatingAddNum(nDriveId, EModbusCmd::Enum_WSReg, AddressSt, value, nRec);
	if (pPort != NULL)
	{
		pPort->send(nRec);
		msg = ReadAndAnaylse(nRec);
	}
	else
	{
		msg = ERecMsg::E_NOPORT;
	}
	return msg;
}
ERecMsg CModbus_master::readException(uint8_t nDriveId, vec8_t& nRec)
{
	ERecMsg msg = ERecMsg::E_TimeOut;
	nRec.resize(0);
	nRec.reserve(5);
	nRec.push_back(nDriveId);
	nRec.push_back(uint8_t(EModbusCmd::Enum_Exception));
	crc16(nRec);
	if (pPort != NULL)
	{
		pPort->send(nRec);
		msg = ReadAndAnaylse(nRec);
	}
	else
	{
		msg = ERecMsg::E_NOPORT;
	}
	return msg;
}
ERecMsg CModbus_master::getCommunicationEventCount(uint8_t nDriveId, vec8_t& nRec)
{
	ERecMsg msg = ERecMsg::E_TimeOut;
	nRec.resize(0);
	nRec.reserve(5);
	nRec.push_back(nDriveId);
	nRec.push_back(uint8_t(EModbusCmd::Enum_GCECout));
	crc16(nRec);
	if (pPort != NULL)
	{
		pPort->send(nRec);
		msg = ReadAndAnaylse(nRec);
	}
	else
	{
		msg = ERecMsg::E_NOPORT;
	}
	return msg;
}
void CModbus_master::crc16(vec8_t& nDataVec)
{
	uint16_t crc = crc16(nDataVec.data(), uint16_t(nDataVec.size()));
	nDataVec.push_back(crc & 0xFF);
	nDataVec.push_back((crc >> 8) & 0xFF);
}
//RTU模式
 //【地址】【功能码】【数据量】【数据1】【…】【数据n】【CRC低字节】【CRC高字节】
uint16_t CModbus_master::crc16(const uint8_t* chData, uint16_t uNo)
{
	uint16_t crc = 0xffff;
	uint16_t i, j;
	for (i = 0; i < uNo; i++)
	{
		crc ^= chData[i];
		for (j = 0; j < 8; j++)
		{
			if (crc & 1)
			{
				crc >>= 1;
				crc ^= 0xA001;
			}
			else
				crc >>= 1;
		}
	}
	return (crc);
}

ERecMsg CModbus_master::checkCrc16(const vec8_t& nDataVec)
{
	uint16_t len = (uint16_t)nDataVec.size();
	if (len < 2)
	{
		return ERecMsg::E_chcekErr;
	}
	int crc = crc16(nDataVec.data(), len - 2);
	if (((crc & 0xFF) == nDataVec[len - 2]) &&
		(((crc >> 8) & 0xFF) == nDataVec[len - 1]))
	{
		return ERecMsg::E_OK;
	}
	return ERecMsg::E_chcekErr;
}
