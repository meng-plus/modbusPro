#pragma once
#include "../CSerialPort/CSerialPort.h"
class CUserSerialPort :
	public CSerialPort
{
public:
	CUserSerialPort();
	virtual ~CUserSerialPort();
	virtual void Init();
private:

};


