#pragma once

#include "spp_per.h"
#include "EString.h"
#include <fstream>
#include <string>
class ReadOFile
{
public:
	ReadOFile(std::string ofile);
	ReadOFile(const ReadOFile& ofile);
	explicit ReadOFile();
	~ReadOFile();
	obs_head ReadOheader(char tobs[][MAXOBSTYPE][4]);
	std::vector<obs_epoch>ReadOData(const obs_head &o_head_Data, char tobs[][MAXOBSTYPE][4]);
private:
	std::string _ofile;
};