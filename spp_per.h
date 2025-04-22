#pragma once
#include<iostream>
#include<string>
#include<vector>
#include<map>
#include <mutex>
#include<fstream>
#include <iomanip>
using namespace std;

#define SQR(x)   ((x)*(x))

#define STD_BRDCCLK 30.0          /* error of broadcast clock (m) */
#define ERR_BRDCI   0.5         /* broadcast iono model error factor */
#define ERR_SAAS    0.3         /* saastamoinen model error std (m) */

#define C_V  2.99792458E8 //光速//光速（m）
#define GM 398600500000000//地心引力常数
#define math_e 2.718281828459 //e值
#define PI 3.141592653589793
#define We 7.292115E-5 //地球自转角速度
#define MAX_OBS 3000
#define MAXOBS      96                  /* max number of obs in an epoch */
#define MAX_SAT 50
#define NUMSYS      7                   /* number of systems */
#define MAXOBSTYPE  64                  /* max number of obs type in RINEX */
#define MAXCODE     68                  /* max number of obs code */
#define MAXFREQ     7                   /* 最大载波类型 */
#define MAXLEAPS    64                  /* max number of leap seconds table */
#define ESTRING_H_
#define WeekSecond 604800 //一周604800秒
#define LMAX 40 //拉格朗日拟合阶数
#define WGS84A  6378137.0     //WGS84坐标系长半轴
#define WGS84f  1/298.257223565 //WGS84坐标系偏心率
#define NFREQ       3                   /* 自定义数值，与code2idx函数紧密关联，决定各系统哪些频段的载波能参与定位 */
#define NEXOBS      0                   /* number of extended obs codes 这里设置为0意味着只使用3种观测值进行定位 */

#define SYS_NONE    0x00                /* navigation system: none */
#define SYS_GPS     0x01                /* navigation system: GPS */
#define SYS_SBS     0x02                /* navigation system: SBAS */
#define SYS_GLO     0x04                /* navigation system: GLONASS */
#define SYS_GAL     0x08                /* navigation system: Galileo */
#define SYS_QZS     0x10                /* navigation system: QZSS */
#define SYS_CMP     0x20                /* navigation system: BeiDou */
#define SYS_IRN     0x40                /* navigation system: IRNS */
#define SYS_LEO     0x80                /* navigation system: LEO */
#define SYS_ALL     0xFF                /* navigation system: all */

#define CODE_NONE   0                   /* obs code: none or unknown */

#define FREQ1       1.57542E9           /* L1/E1/B1C  frequency (Hz) */
#define FREQ2       1.22760E9           /* L2         frequency (Hz) */
#define FREQ5       1.17645E9           /* L5/E5a/B2a frequency (Hz) */
#define FREQ6       1.27875E9           /* E6/L6  frequency (Hz) */
#define FREQ7       1.20714E9           /* E5b    frequency (Hz) */
#define FREQ8       1.191795E9          /* E5a+b  frequency (Hz) */
#define FREQ9       2.492028E9          /* S      frequency (Hz) */
#define FREQ1_GLO   1.60200E9           /* GLONASS G1 base frequency (Hz) */
#define DFRQ1_GLO   0.56250E6           /* GLONASS G1 bias frequency (Hz/n) */
#define FREQ2_GLO   1.24600E9           /* GLONASS G2 base frequency (Hz) */
#define DFRQ2_GLO   0.43750E6           /* GLONASS G2 bias frequency (Hz/n) */
#define FREQ3_GLO   1.202025E9          /* GLONASS G3 frequency (Hz) */
#define FREQ1a_GLO  1.600995E9          /* GLONASS G1a frequency (Hz) */
#define FREQ2a_GLO  1.248060E9          /* GLONASS G2a frequency (Hz) */
#define FREQ1_CMP   1.561098E9          /* BDS B1I     frequency (Hz) */
#define FREQ2_CMP   1.20714E9           /* BDS B2I/B2b frequency (Hz) */
#define FREQ3_CMP   1.26852E9           /* BDS B3      frequency (Hz) */

#define MAXDTOE     7200.0              /* max time difference to GPS Toe (s) */
#define MAXDTOE_QZS 7200.0              /* max time difference to QZSS Toe (s) */
#define MAXDTOE_GAL 14400.0             /* max time difference to Galileo Toe (s) */
#define MAXDTOE_CMP 21600.0             /* max time difference to BeiDou Toe (s) */
#define MAXDTOE_GLO 1800.0              /* max time difference to GLONASS Toe (s) */
#define MAXDTOE_IRN 7200.0              /* max time difference to IRNSS Toe (s) */
#define MAXDTOE_SBS 360.0               /* max time difference to SBAS Toe (s) */
#define MAXDTOE_S   86400.0             /* max time difference to ephem toe (s) for other */

#define SNR_UNIT    0.001               /* SNR unit (dBHz) */

static const char syscodes[] = "GREJSCI"; /* satellite system codes */
const char obstypecodes[] = "CLDS";    /* observation type codes */
typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;   //短整型，2字节
static const double gpst0[] = { 1980,1, 6,0,0,0 }; /* gps time reference */



typedef struct Point  //空间坐标系
{
	double x = 0;
	double y = 0;
	double z = 0;
}Point;

struct SatPoint  //卫星坐标
{
	Point point;
	std::string PRN = "";
};

struct ENUPoint  //站心地平坐标系
{
	double E;
	double N;
	double U;
	ENUPoint()
	{
		E = 0;
		N = 0;
		U = 0;
	}
};

struct BLHPoint  //大地坐标系,单位为弧度
{
	double B;
	double L;
	double H;
	BLHPoint()
	{
		B = 0;
		L = 0;
		H = 0;
	}
};
//3.0版本O文件观测数据优先级筛选
typedef struct {                        /* signal index type */
	int n;                              /* number of index 接收机实际接受到的信号类型数量 */
	int idx[MAXOBSTYPE];                /* signal freq-index */
	int pos[MAXOBSTYPE];                /* signal index in obs data (-1:no) */
	uint8_t pri[MAXOBSTYPE];           /* signal priority (15-0) */
	uint8_t type[MAXOBSTYPE];           /* type (0:C,1:L,2:D,3:S) */
	uint8_t code[MAXOBSTYPE];           /* obs-code (CODE_L??) */
	double shift[MAXOBSTYPE];           /* phase shift (cycle) */
} sigind_t;
// 导航头文件结构体
typedef struct nav_head
{
	double version;  //RINEX格式的版本号
	char type;       //文件类型
	char system;     //文件所属卫星系统
	double IonAlpha[4];  //星历中的电离层参数A1-A4
	double IonBeta[4];   //星历中的电离层参数B2-B4
	double utc_gps[8];  /* GPS delta-UTC parameters {A0,A1,Tot,WNt,dt_LS,WN_LSF,DN,dt_LSF} */
	double DeltaUtaA[2]; //用于计算UTC时的历书时的多项式系数A1,A2
	double DeltaUtcT;   //用于计算UTC时的历书时的参考时刻
	double DeltaUtcW;  //用于计算UTC时的历书时 参考周数
	double LeapSeconds;  //跳秒 计算UTC和GPS时的差异
}nav_head, * pnav_head;

typedef struct gtime_t {
	time_t time; //表示1970年以来的整秒数
	double second;//用 double 类型表示不到 1s 的时间
	gtime_t operator+(double sec)//增加秒数
	{
		gtime_t temp;
		double tt;
		this->second += sec; 
		tt = floor(this->second); this->time += (int)tt;
		temp.time = this->time;
		this->second -= tt;
		temp.second = this->second;
		return temp;
	}
	gtime_t operator-(double sec)//减少秒数
	{
		gtime_t temp;
		double tt;
		tt = floor(sec);
		double ts = this->second - (sec - tt);
		temp.time = this->time - (int)tt;
		if (ts >= 0) {
			temp.second = this->second - ts;
			return temp;
		}
		else {
			temp.time -= 1; temp.second = 1.0 + ts;
			return temp;
		}
	}

}gtime_t;

struct GTime  //GPS时
{
	int week;
	double seconds;
	double operator- (const GTime& Gtime);
};

struct LargnageCoeff
{
	GTime gtime;
	Point point;
};

typedef struct Time
{
	int year;
	int month;
	int day;
	int hour;
	int minute;
	double second;
	Time(int Y = 0, int M = 0, int D = 0, int H = 0, int MIN = 0, double S = 0)
		:year(Y), month(M), day(D), hour(H), minute(MIN), second(S) {}
	Time operator+(double seconds);
	double operator-(const Time&);
	Time operator-(double seconds);
	friend std::ostream& operator <<(std::ostream& os, const Time& time);
	GTime UTC2GTime() const;
}Time;

//导航数据结构体
typedef struct nav_body
{
	//数据块第一行内容：
	std::string PRN;//卫星PRN号
	//历元：TOC中卫星钟的参考时刻
	Time TOC; //卫星钟的参考时刻
	double ClkBias; //  卫星钟的偏差s
	double ClkDrift;  //  卫星钟的漂移 s/s
	double ClkDriftRate;  //  卫星钟的漂移速度s/(s2)

	//数据块第二行内容：
	double IODE;//数据、星历发布时间(数据期龄)
	double Crs;//轨道半径的正弦调和改正项的振幅（单位：m）
	double DetaN;//卫星平均运动速率与计算值之差(rad/s)
	double M0;//参考时间的平近点角(rad)

	//数据块第三行内容：
	double Cuc;//维度幅角的余弦调和改正项的振幅(rad)
	double e;//轨道偏心率
	double Cus;//轨道幅角的正弦调和改正项的振幅(rad)
	double sqrtA;//长半轴平方根

	//数据块第四行内容：
	double TOE;//星历的参考时刻(GPS周内秒)
	double Cic;//轨道倾角的余弦调和改正项的振幅(rad)
	double OMEGA;//参考时刻的升交点赤经
	double Cis;//维度倾角的正弦调和改正项的振幅(rad)

	//数据块第五行内容：
	double i0;//参考时间的轨道倾角(rad)
	double Crc;//轨道平径的余弦调和改正项的振幅(m)
	double omega;//近地点角距
	double OMEGA_DOT;//升交点赤经变化率(rad)

	//数据块第六行内容：
	double IDOT;//近地点角距(rad/s)
	double L2;//L2上的码
	double GPSWeek;//GPS周,于TOE一同表示
	double L2_P;//L2,p码数据标记

	//数据块第七行内容
	double SatAccuracy;//卫星精度
	double SatHealthy;//卫星健康状态
	double TGD;//sec
	double IODC;//钟的数据龄期

	//数据块第八行内容
	double TransmissionTimeofMessage;//电文发送时间
	double FitInterval;//拟合区间
	double Reamrk1;//空
	double Remark2;//空

}nav_body, * pnav_body;


//观测值头文件
typedef struct obs_head
{
	std::string version;
	char type; //文件类型
	char sattype; //卫星类型
	std::string markername; //测站名
	std::string markernumber;//测站编号
	std::string observer;//观测人姓名
	std::string agency;//观测单位
	std::string rec;//接收机序列号
	std::string rectype;//接收机型号
	std::string recvers;//接收机版本号
	float antenna_h;//天线高
	float antenna_e;//天线东向分量
	float antenna_n;//天线北向分量
	float interval;//历元间隔
	std::string ant;//天线序列号
	std::string anttype;//天线型号
	Point PXYZ; //测站近似坐标
	int sigsnums;//GPS接收机信号类型数，这里即便是M，我们也只考虑GPS信号类型
	std::vector<std::string> sigtypes; //信号列表
	Time gtime;//第一个观测记录时刻，GPS时
}obs_head, * pobs_head;

enum ObsType {
	C1,
	P1,
	P2,
	L1,
	L2 = 4,
};

typedef struct ASatData //每颗卫星的观测值以及编号
{
	std::string PRN; //卫星编号
	double Obs; //观测值
}ASatData;

typedef struct {
	std::string PRN; //卫星编号
	uint16_t SNR[NFREQ + NEXOBS]; /* signal strength (0.001 dBHz) */
	uint8_t  LLI[NFREQ + NEXOBS]; /* loss of lock indicator */
	uint8_t code[NFREQ + NEXOBS]; /* code indicator (CODE_???) */
	double L[NFREQ + NEXOBS]; /* observation data carrier-phase (cycle) */
	double P[NFREQ + NEXOBS]; /* observation data pseudorange (m) */
	float  D[NFREQ + NEXOBS]; /* observation data doppler frequency (Hz) */
}obsd_t;

//观测值历元数据结构体
typedef struct obs_epoch
{
	Time gtime; //历元时刻
	gtime_t epochSecondtime;
	int satsums;//卫星数量
	int flag;//历元标志，0表示正常，1表示当前历元与上一历元发生异常
	std::map<ObsType, std::vector<ASatData>> AllTypeDatas_2;  //rinex2版本星历中所有类型的观测值
	std::vector<obsd_t> AllTypeDatas_3;//rinex3版本星历中经过筛选的观测值
}obs_epoch, * pobs_epoch;



