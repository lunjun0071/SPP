#pragma once
#include "ReadFile.h"
#include"spp_per.h"


nav_body GetNFileByobsTime(const GTime& GobsTime, const std::vector<nav_body>& nDatas, std::string PRN);
SatPoint SatPosition(const Time& obstime, gtime_t Sectime, const Point& stationPoint, double pL, const nav_body& nData, double& SatCLK, double LEAPSeconds, double Rr);
bool CalculationPostion(Point XYZ, nav_head& n_head_Data, obs_head& o_head_data, obs_epoch& oData, Point& Position, const vector<nav_body>& nData, double LeapSeconds, double& Rr,
	std::map<string, double>& SatCLKs, std::string version, double elvation);
bool OutputResult(ReadFile read, std::string output, double evevation);