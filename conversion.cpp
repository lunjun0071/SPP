#include"spp_per.h"
#include <iomanip>
#include"tools.h"
using namespace std;
using std::string;

//将世界协调时（UTC）转换为儒略日（Julian Day）
double UTC2JD(const Time& time)
{
	double hour = time.hour + time.minute / 60.0 + time.second / 3600.0;
	return 1721013.5 + 367 * time.year - int(7.0 / 4 * (time.year + (time.month + 9) / 12)) + time.day + hour / 24 + (275 * time.month / 9);
}

double  GTime::operator- (const GTime& Gtime)
{
	//顾及一周（604800）开始或结束
	double TransTime = (week - Gtime.week) * WeekSecond + (seconds - Gtime.seconds);
	if (TransTime > WeekSecond / 2.0)   TransTime -= WeekSecond;
	if (TransTime < -WeekSecond / 2.0)  TransTime += WeekSecond;
	return TransTime;
}

//Time数组中的运算符重载
Time Time::operator+(double seconds)
{
	Time addTime(year, month, day, hour, minute, second + seconds);
	while (addTime.second >= 60)
	{
		addTime.second -= 60;
		addTime.minute += 1;
		while (addTime.minute >= 60)
		{
			addTime.hour += 1;
			while (addTime.hour >= 24)
			{
				addTime.day += 1;
			}
		}
	}
	return addTime;
}
double Time::operator-(const Time& time)
{
	return (UTC2JD(*this) - UTC2JD(time)) * 24 * 3600 + this->second - time.second;
}
Time Time::operator-(double seconds)
{
	Time insTime(year, month, day, hour, minute, second - seconds);
	while (insTime.second < 0)
	{
		insTime.second += 60;
		insTime.minute -= 1;
		while (insTime.minute < 0)
		{
			insTime.minute += 60;
			insTime.hour -= 1;
			while (insTime.hour < 0)
			{
				insTime.hour += 24;
				insTime.day -= 1;
			}
		}
	}
	return insTime;
}
std::ostream& operator <<(std::ostream& os, const Time& time)
{
	os << std::setw(4) << time.year << std::setw(3) << time.month << std::setw(3) << time.day
		<< std::setw(3) << time.hour << std::setw(3) << time.minute << std::setw(11) << std::fixed << std::setprecision(7)
		<< time.second;
	return os;
}
//将UTC时转换为1970年以来的秒数
extern gtime_t epoch2time(const double *ep)
{
	const int doy[] = { 1,32,60,91,121,152,182,213,244,274,305,335 };//某个月的第一天是一年中的哪一天
	gtime_t time = { 0 };
	int days,sec,year = (int)ep[0], mon = (int)ep[1], day = (int)ep[2];

	if (year < 1970 || 2099 < year || mon < 1 || 12 < mon) return time;

	days = (year - 1970) * 365 + (year - 1969) / 4 + doy[mon - 1] + day - 2 + (year % 4 == 0 && mon >= 3 ? 1 : 0);
	sec = (int)floor(ep[5]);//不足一秒的部分取整，floor:向下取整
	time.time = (time_t)days * 86400 + (int)ep[3] * 3600 + (int)ep[4] * 60 + sec;
	time.second = ep[5] - sec;//不足一秒的部分
	return time;
}

//UTC转GPS时，以gtime_t格式表达
extern gtime_t utc2gpst(gtime_t t)
{
	int i;

	for (i = 0; leaps[i][0] > 0; i++) {
		if (timediff(t, epoch2time(leaps[i])) >= 0) return t - leaps[i][6];
	}
	return t;
}

//转UTC时
extern void time2epoch(gtime_t t, double* ep)
{
	const int mday[] = { /* # of days in a month */
		31,28,31,30,31,30,31,31,30,31,30,31,31,28,31,30,31,30,31,31,30,31,30,31,
		31,29,31,30,31,30,31,31,30,31,30,31,31,28,31,30,31,30,31,31,30,31,30,31
	};
	int days, sec, mon, day;

	/* leap year if year%4==0 in 1901-2099 */
	days = (int)(t.time / 86400);
	sec = (int)(t.time - (time_t)days * 86400);
	for (day = days % 1461, mon = 0; mon < 48; mon++) {
		if (day >= mday[mon]) day -= mday[mon]; else break;
	}
	ep[0] = 1970 + days / 1461 * 4 + mon / 12; ep[1] = mon % 12 + 1; ep[2] = day + 1;
	ep[3] = sec / 3600; ep[4] = sec % 3600 / 60; ep[5] = sec % 60 + t.second;
}
//gtime_t转GTime
GTime time2gpst(gtime_t gtime) {
	GTime temp;
	gtime_t t0 = epoch2time(gpst0);
	time_t sec = gtime.time - t0.time;
	temp.week = (int)(sec / (86400 * 7));
	temp.seconds = (double)(sec - (double)temp.week * 86400 * 7) + gtime.second;
	return temp;
}

//Time转UTC数组形式
double* Time2utc(Time time)
{
	double* ep = (double*)malloc(6 * sizeof(double));
	if (ep == NULL) return NULL;
	ep[0] = time.year; ep[1] = time.month; ep[2] = time.day;
	ep[3] = time.hour; ep[4] = time.minute; ep[5] = time.second;
	return ep;
}

//返回两个gtime_t对象相减的值
extern double timediff(gtime_t t1, gtime_t t2)
{
	return difftime(t1.time, t2.time) + t1.second - t2.second;
}

//世界协调时（UTC）转换为 GPS 时间（GTime）
GTime Time::UTC2GTime() const
{
	GTime gt;
	//跳秒
	/*Time leapTime(this->year, leaps[this->year - 2016][1], leaps[this->year - 2016][2], leaps[this->year - 2016][3], leaps[this->year - 2016][4], leaps[this->year - 2016][5]);
	int leapseconds = (*this - leapTime) > 0 ? leaps[this->year - 2016][6] : 0;*/
	double JD = UTC2JD(*this);
	gt.week = int((JD - 2444244.5) / 7.0);
	gt.seconds = (JD - 2444244.5 - gt.week * 7.0) * 86400.0;
	return gt;
}



