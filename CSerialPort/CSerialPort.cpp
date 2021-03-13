// CSerialPort.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "CSerialPort.h"

CSerialPort::CSerialPort()
	:confirmation_to_ignore(false),
	baud(115200),
	data_bit(8),
	stop_bit(1),
	parity('N')
{
}

CSerialPort::~CSerialPort()
{
}
void CSerialPort::SetSerialPort(const uint8_t portID, bool recSt)
{
	char strComName[16];
	sprintf_s(strComName, (portID < 10 ? "COM%d" : "\\\\.\\COM%d"), portID);
	if (recSt && (w_ser.fd != INVALID_HANDLE_VALUE))
	{
		Close();
	}
	device = strComName;
}
bool CSerialPort::InitSerial(const char* device, int baud, char parity, int data_bit, int stop_bit)
{

	/* Check device argument */
	if (device == NULL || *device == 0) {
		fprintf(stderr, "The device string is empty\n");
		errno = EINVAL;
		return NULL;
	}

	/* Check baud argument */
	if (baud == 0) {
		fprintf(stderr, "The baud rate value must not be zero\n");
		errno = EINVAL;
		return NULL;
	}
	this->device = device;
	this->baud = baud;

	if (parity == 'N' || parity == 'E' || parity == 'O') {
		this->parity = parity;
	}
	else {
		errno = EINVAL;
		return NULL;
	}
	this->data_bit = data_bit;
	this->stop_bit = stop_bit;
	confirmation_to_ignore = FALSE;
	return true;
}
bool CSerialPort::InitSerial(const uint8_t portID, int baud, char parity, int data_bit, int stop_bit)
{
	char strComName[16];
	sprintf_s(strComName, (portID < 10 ? "COM%d" : "\\\\.\\COM%d"), portID);
	return InitSerial(strComName, baud, parity, data_bit, stop_bit);
}
int CSerialPort::Connect()
{
#if defined(_WIN32)
	DCB dcb;
#if _DEBUG && _CONSOLE
	printf("Opening %s at %d bauds (%c, %d, %d)\n",
		device.c_str(), baud, parity,
		data_bit, stop_bit);
#endif // _DEBUG
	/* Set file handle to invalid */
	if (w_ser.fd != INVALID_HANDLE_VALUE)
	{
		Close();
	}
	/* ctx_rtu->device should contain a string like "COMxx:" xx being a decimal
 * number */
	w_ser.fd = CreateFileA(device.c_str(),
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);

	if (w_ser.fd == INVALID_HANDLE_VALUE) {
#if _DEBUG && _CONSOLE
		fprintf(stderr, "ERROR Can't open the device %s (LastError %d)\n",
			device.c_str(), (int)GetLastError());
#endif // _DEBUG
		return -1;
	}
	/* Save params */
	old_dcb.DCBlength = sizeof(DCB);
	if (!GetCommState(w_ser.fd, &old_dcb)) {
#if _DEBUG && _CONSOLE
		fprintf(stderr, "ERROR Error getting configuration (LastError %d)\n",
			(int)GetLastError());
#endif // _DEBUG
		CloseHandle(w_ser.fd);
		w_ser.fd = INVALID_HANDLE_VALUE;
		return -1;
	}
	/* Build new configuration (starting from current settings) */
	dcb = old_dcb;

	/* Speed setting */
	switch (baud) {
	case 110:
		dcb.BaudRate = CBR_110;
		break;
	case 300:
		dcb.BaudRate = CBR_300;
		break;
	case 600:
		dcb.BaudRate = CBR_600;
		break;
	case 1200:
		dcb.BaudRate = CBR_1200;
		break;
	case 2400:
		dcb.BaudRate = CBR_2400;
		break;
	case 4800:
		dcb.BaudRate = CBR_4800;
		break;
	case 9600:
		dcb.BaudRate = CBR_9600;
		break;
	case 14400:
		dcb.BaudRate = CBR_14400;
		break;
	case 19200:
		dcb.BaudRate = CBR_19200;
		break;
	case 38400:
		dcb.BaudRate = CBR_38400;
		break;
	case 57600:
		dcb.BaudRate = CBR_57600;
		break;
	case 115200:
		dcb.BaudRate = CBR_115200;
		break;
	case 230400:
		/* CBR_230400 - not defined */
		dcb.BaudRate = 230400;
		break;
	case 250000:
		dcb.BaudRate = 250000;
		break;
	case 460800:
		dcb.BaudRate = 460800;
		break;
	case 500000:
		dcb.BaudRate = 500000;
		break;
	case 921600:
		dcb.BaudRate = 921600;
		break;
	case 1000000:
		dcb.BaudRate = 1000000;
		break;
	default:
		dcb.BaudRate = CBR_9600;
#if _DEBUG && _CONSOLE
		fprintf(stderr, "WARNING Unknown baud rate %d for %s (B9600 used)\n",
			baud, device.c_str());
#endif // _DEBUG
	}
	/* Data bits */
	switch (data_bit) {
	case 5:
		dcb.ByteSize = 5;
		break;
	case 6:
		dcb.ByteSize = 6;
		break;
	case 7:
		dcb.ByteSize = 7;
		break;
	case 8:
	default:
		dcb.ByteSize = 8;
		break;
	}
	/* Stop bits */
	if (stop_bit == 1)
		dcb.StopBits = ONESTOPBIT;
	else /* 2 */
		dcb.StopBits = TWOSTOPBITS;

	/* Parity */
	if (parity == 'N') {
		dcb.Parity = NOPARITY;
		dcb.fParity = FALSE;
	}
	else if (parity == 'E') {
		dcb.Parity = EVENPARITY;
		dcb.fParity = TRUE;
	}
	else {
		/* odd */
		dcb.Parity = ODDPARITY;
		dcb.fParity = TRUE;
	}

	/* Hardware handshaking left as default settings retrieved */

	/* No software handshaking */
	dcb.fTXContinueOnXoff = TRUE;
	dcb.fOutX = FALSE;
	dcb.fInX = FALSE;

	/* Binary mode (it's the only supported on Windows anyway) */
	dcb.fBinary = TRUE;

	/* Don't want errors to be blocking */
	dcb.fAbortOnError = FALSE;
	/* Setup port */
	if (!SetCommState(w_ser.fd, &dcb)) {
#if _DEBUG && _CONSOLE
		fprintf(stderr, "ERROR Error setting new configuration (LastError %d)\n",
			(int)GetLastError());
#endif // _DEBUG
		CloseHandle(w_ser.fd);
		w_ser.fd = INVALID_HANDLE_VALUE;
		return -1;
	}
#endif
	return 0;
}
void CSerialPort::Close()
{
	/* Revert settings */
	if (!SetCommState(w_ser.fd, &old_dcb)) {

#if _DEBUG && _CONSOLE
		fprintf(stderr, "ERROR Couldn't revert to configuration (LastError %d)\n",
			(int)GetLastError());
#endif // _DEBUG
	}

	if (!CloseHandle(w_ser.fd)) {
#if _DEBUG && _CONSOLE
		fprintf(stderr, "ERROR Error while closing handle (LastError %d)\n",
			(int)GetLastError());
#endif // _DEBUG

	}
	w_ser.n_bytes = NULL;
	w_ser.fd = INVALID_HANDLE_VALUE;
}
int CSerialPort::send(const uint8_t* req, int req_length)
{
#if defined(_WIN32)
	//	modbus_rtu_t* ctx_rtu = ctx->backend_data;
	DWORD n_bytes = 0;
	return (WriteFile(w_ser.fd, req, req_length, &n_bytes, NULL)) ? n_bytes : -1;
#endif
	return 0;
}

int CSerialPort::send(vec8_t& reqvec)
{
#if defined(_WIN32)
	//	modbus_rtu_t* ctx_rtu = ctx->backend_data;
	DWORD n_bytes = 0;
	return (WriteFile(w_ser.fd, reqvec.data(), reqvec.size(), &n_bytes, NULL)) ? n_bytes : -1;
#endif
}

EPortMsg CSerialPort::read(int max_len,
	const struct timeval* tv)
{
	COMMTIMEOUTS comm_to;
	unsigned int msec = 0;

	/* Check if some data still in the buffer to be consumed */
	if (w_ser.n_bytes > 0) {
		return EPortMsg::E_Busy;
	}

	/* Setup timeouts like select() would do.
	   FIXME Please someone on Windows can look at this?
	   Does it possible to use WaitCommEvent?
	   When tv is NULL, MAXDWORD isn't infinite!
	 */
	if (tv == NULL) {
		msec = MAXDWORD;
	}
	else {
		msec = tv->tv_sec * 1000 + tv->tv_usec / 1000;
		if (msec < 1)
			msec = 1;
	}

	comm_to.ReadIntervalTimeout = msec;
	comm_to.ReadTotalTimeoutMultiplier = 0;
	comm_to.ReadTotalTimeoutConstant = msec;
	comm_to.WriteTotalTimeoutMultiplier = 0;
	comm_to.WriteTotalTimeoutConstant = 1000;
	SetCommTimeouts(w_ser.fd, &comm_to);

	/* Read some bytes */
	if ((max_len > PY_BUF_SIZE) || (max_len < 0)) {
		max_len = PY_BUF_SIZE;
	}

	if (ReadFile(w_ser.fd, &w_ser.buf, max_len, &w_ser.n_bytes, NULL)) {
		/* Check if some bytes available */
		if (w_ser.n_bytes > 0) {
			/* Some bytes read */
			return EPortMsg::E_Ok;
		}
		else {
			/* Just timed out */
			return EPortMsg::E_Timeout;
		}
	}
	else {
		/* Some kind of error */
		return EPortMsg::E_ReadErr;
	}
}

EPortMsg CSerialPort::read(vec8_t& rec, const timeval* tv)
{
	EPortMsg irec = read(PY_BUF_SIZE, tv);
	if (irec == EPortMsg::E_Ok)
	{
		rec.resize(w_ser.n_bytes);
		memcpy(rec.data(), w_ser.buf, w_ser.n_bytes);
	}
	return irec;
}
