#include"ReadFile.h"


ReadFile::ReadFile(std::string ofilename, std::string nfilename)
{
	_ofile = ReadOFile(ofilename);
	_nfile = ReadNFile(nfilename);
}
ReadFile::~ReadFile()
{
}

// �滻��ѧ�������е� 'D' Ϊ 'E'��Ȼ��ת��Ϊ˫���ȸ�����
std::string replaceDWithE(const std::string& str) {
    std::string temp = str;
    size_t pos = temp.find('D');
    if (pos != std::string::npos) {
        temp.replace(pos, 1, "E");
    }
    return temp;
}