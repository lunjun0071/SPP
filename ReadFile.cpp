#include"ReadFile.h"


ReadFile::ReadFile(std::string ofilename, std::string nfilename)
{
	_ofile = ReadOFile(ofilename);
	_nfile = ReadNFile(nfilename);
}
ReadFile::~ReadFile()
{
}

// 替换科学计数法中的 'D' 为 'E'，然后转换为双精度浮点数
std::string replaceDWithE(const std::string& str) {
    std::string temp = str;
    size_t pos = temp.find('D');
    if (pos != std::string::npos) {
        temp.replace(pos, 1, "E");
    }
    return temp;
}