#pragma once
#include"tools.h"
#include"spp_per.h"
#include"Matrix.h"

//���������Ӳ�
int ephclk(gtime_t time, gtime_t ephtime, std::string prn, const std::vector<nav_body>& nav, double* dt, const char* p);
//���������Ӳ�
extern double eph2clk(gtime_t time, const nav_body eph);
//�۲�����
extern double varerr(double el);