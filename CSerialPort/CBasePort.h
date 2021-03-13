#pragma once
#include <vector>
#include <winsock.h>
typedef std::vector<uint8_t> vec8_t;
enum class EPortMsg :char
{
	E_Busy,
	E_Ok,
	E_Timeout,
	E_ReadErr
};
class CBasePort
{
public:
	CBasePort();
	virtual~CBasePort();
	/*用户继承后，在此处添加初始化代码*/
	virtual void Init() {};
	/**启动设备连接，使用前应当执行InitSerial
	*@return 0 连接成功 -1失败
	*/
	virtual int Connect() = 0;

	virtual void Close() = 0;

	/*发送数据
	* @param req 数据起始地址
	* @param req_length 数据长度
	*/
	virtual int send(vec8_t& reqvec) = 0;
	/*接收数据
	* @return 接收到数据长度
	*/
	virtual EPortMsg read(vec8_t& rec, const struct timeval* tv = NULL) = 0;
private:

};
