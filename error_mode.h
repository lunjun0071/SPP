#pragma once
#include"tools.h"
#include"spp_per.h"
#include"Matrix.h"

//∑µªÿŒ¿–«÷”≤Ó
int ephclk(gtime_t time, gtime_t ephtime, std::string prn, const std::vector<nav_body>& nav, double* dt, const char* p);
//º∆À„Œ¿–«÷”≤Ó
extern double eph2clk(gtime_t time, const nav_body eph);
//π€≤‚‘Î…˘
extern double varerr(double el);