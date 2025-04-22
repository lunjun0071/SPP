#include"error_mode.h"




//计算卫星钟差
extern double eph2clk(gtime_t time, const nav_body eph)
{
	double t, ts;
	int i;
	gtime_t toc = {0};
	double *temp = Time2utc(eph.TOC); 
	if (temp != NULL) {
		toc = epoch2time(temp);
		free(temp);
	}

	t = ts = timediff(time, toc);

	for (i = 0; i < 2; i++) {
		t = ts - (eph.ClkBias + eph.ClkDrift * t + eph.ClkDriftRate * t * t);
	}
	return eph.ClkBias + eph.ClkDrift * t + eph.ClkDriftRate * t * t;
}
//返回卫星钟差
int ephclk(gtime_t time, gtime_t ephtime,std::string prn, const std::vector<nav_body>& nav, double *dt, const char *p)
{
	int sys;
	const nav_body* eph;
	sys = satsys(p);
	if (sys == SYS_GPS || sys == SYS_GAL || sys == SYS_QZS || sys == SYS_CMP || sys == SYS_IRN)
	{
		eph = seleph(ephtime, p, prn, nav);//time为信号发射时刻用来求钟差，ephtime为观测时刻，用来选择最佳星历
		*dt = eph2clk(time, *eph);
	}
	return 1;
}

//观测噪声
double varerr(double el)
{
	double fact = 100.0;
	double varr= (SQR(0.003) + SQR(0.003) / sin(el));;
	return SQR(fact) * varr; //伪距高度角加权模型
}