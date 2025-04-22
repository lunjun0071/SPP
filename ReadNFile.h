#pragma once
#include"spp_per.h"
class ReadNFile
{
public:
	ReadNFile(std::string filename);
	explicit ReadNFile();
	ReadNFile(const ReadNFile& nfile);
	~ReadNFile();
	nav_head Readnav_head();
	std::vector<nav_body>ReadNRecord(double ver);
private:
	std::string _filename;
};