/**************************************
*描述：获取串口列表接口，将串口清单放置在字符串容器里
*环境：win7~win10 VS2019下开发 x86  unicode
*
*作者：cmm
*日期：2021年3月10日
* 版本：Ver.1.0.0
***************************************/
#pragma once


#include <string>
#include <vector>
class  CSetupAPI
{
public:
	CSetupAPI();
	virtual ~CSetupAPI();
	/**刷新串口清单
	*	@return 串口的数量
	*	@note 本函数自动获取串口名及串口编号
	*/
	uint16_t UpData();
	/**获取当前的串口数量
	* @param nUpdata 标识更新串口列表后再获得数量
	* @return 存在的串口数量
	*/
	uint16_t GetComNumber(bool nUpdata = false);
	/**获取当前的串口列表
	* @param nUpdata 标识更新串口列表后再获得数量
	* @return 存在的串口列表
	*/
	const std::vector<std::wstring>& GetComNameVec(bool nUpdata = false);
	/**根据串口列表顺序 获得对应串口号
	* @param idx 串口列表顺序
	* @return 串口ID号
	*/
	uint16_t GetComID(uint16_t idx);
	/**根据串口名称 解析出对应串口号
	* @param idx 串口名称
	* @return 串口ID号
	*/
	uint16_t GetComID(std::wstring& nComName);
	/**根据串口顺序 获得串口名称
	* @param idx 串口顺序
	* @return 串口名称
	*/
	std::wstring GetComName(uint16_t idx);
private:
	std::vector<std::wstring> m_ComName;
};


