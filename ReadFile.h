#pragma once
#include "ReadNFile.h"
#include "ReadOFile.h"
#include <string>
class ReadFile
{
public:
	ReadFile(std::string ofilename, std::string nfilename);
	~ReadFile();
	ReadNFile _nfile;
	ReadOFile _ofile;
};