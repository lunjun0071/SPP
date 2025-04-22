#include "EString.h"
using namespace std;
//EString函数实现
string EString::TrimRight(const string& str)//去除字符串末尾的空格，并返回处理后的字符串
{
	string strcopy = str;
	return strcopy.erase(strcopy.find_last_not_of(' ') + 1, strcopy.size() - strcopy.find_last_not_of(' ') - 1);
}
string EString::TrimLeft(const string& str)//使用 erase 函数去除字符串开头的空格，并返回处理后的字符串
{
	string strcopy = str;
	return strcopy.erase(0, strcopy.find_first_not_of(' '));
}
string EString::Trim(const string& str)//去除字符串两端的空格，并返回处理后的字符串
{
	return TrimRight(TrimLeft(str));
}
//将数据输出到文件流中
std::ofstream& EString::OutPut(std::ofstream& ofs, const obs_epoch& oData, double Rr, const Point* result)
{
	if (result == nullptr)
	{
		ofs << "该历元数据有误：";
		ofs << oData.gtime << std::endl;
	}
	else
	{
		ofs << oData.gtime;
		ofs << std::setw(4) << oData.satsums;
		ofs << std::setw(16) << std::fixed << std::setprecision(4) << result->x
			<< std::setw(15) << std::fixed << std::setprecision(4) << result->y
			<< std::setw(15) << std::fixed << std::setprecision(4) << result->z
			<< std::setw(16) << std::fixed << std::setprecision(10) << Rr << std::endl;
	}
	return ofs;
}
