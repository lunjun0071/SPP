#pragma once
#include <fstream>
#include <iomanip>
#include"spp_per.h"
namespace EString
{
	std::string TrimLeft(const std::string& str);
	std::string TrimRight(const std::string& str);
	std::string Trim(const std::string& str);
	std::ofstream& OutPut(std::ofstream& ofs, const obs_epoch& oData, double Rr, const Point* result = nullptr);
}