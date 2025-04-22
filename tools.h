#pragma once
#include"spp_per.h"
#include"Matrix.h"
#include"EString.h"
#include"error_mode.h"


static char const *obscodes[] = {       /* observation code strings */

    ""  ,"1C","1P","1W","1Y", "1M","1N","1S","1L","1E", /*  0- 9 */
    "1A","1B","1X","1Z","2C", "2D","2S","2L","2X","2P", /* 10-19 */
    "2W","2Y","2M","2N","5I", "5Q","5X","7I","7Q","7X", /* 20-29 */
    "6A","6B","6C","6X","6Z", "6S","6L","8L","8Q","8X", /* 30-39 */
    "2I","2Q","6I","6Q","3I", "3Q","3X","1I","1Q","5A", /* 40-49 */
    "5B","5C","9A","9B","9C", "9X","1D","5D","5P","5Z", /* 50-59 */
    "6E","7D","7P","7Z","8D", "8P","4A","4B","4X",""    /* 60-69 */
};

static char codepris[7][MAXFREQ][16] = {  /* code priority for each freq-index */
   /*    0         1          2          3         4         5     */
    {"CPYWMNSL","PYWCMNDLSX","IQX"     ,""       ,""       ,""      ,""}, /* GPS */
    {"CPABX"   ,"PCABX"     ,"IQX"     ,""       ,""       ,""      ,""}, /* GLO */
    {"CABXZ"   ,"IQX"       ,"IQX"     ,"ABCXZ"  ,"IQX"    ,""      ,""}, /* GAL */
    {"CLSXZ"   ,"LSX"       ,"IQXDPZ"  ,"LSXEZ"  ,""       ,""      ,""}, /* QZS */
    {"C"       ,"IQX"       ,""        ,""       ,""       ,""      ,""}, /* SBS */
    {"IQXDPAN" ,"IQXDPZ"    ,"DPX"     ,"IQXA"   ,"DPX"    ,""      ,""}, /* BDS */
    {"ABCX"    ,"ABCX"      ,""        ,""       ,""       ,""      ,""}  /* IRN */
};

static double leaps[MAXLEAPS + 1][7] = { /* leap seconds (y,m,d,h,m,s,utc-gpst) */
    {2017,1,1,0,0,0,-18},
    {2015,7,1,0,0,0,-17},
    {2012,7,1,0,0,0,-16},
    {2009,1,1,0,0,0,-15},
    {2006,1,1,0,0,0,-14},
    {1999,1,1,0,0,0,-13},
    {1997,7,1,0,0,0,-12},
    {1996,1,1,0,0,0,-11},
    {1994,7,1,0,0,0,-10},
    {1993,7,1,0,0,0, -9},
    {1992,7,1,0,0,0, -8},
    {1991,1,1,0,0,0, -7},
    {1990,1,1,0,0,0, -6},
    {1988,1,1,0,0,0, -5},
    {1985,7,1,0,0,0, -4},
    {1983,7,1,0,0,0, -3},
    {1982,7,1,0,0,0, -2},
    {1981,7,1,0,0,0, -1},
    {0}
};



//为矩阵开辟空间
extern double* mat(int n, int m);
//从字符串中提取子字符串并将其转换为数字
extern double str2num(const char* s, int i, int n);
//将UTC时转换为1970年以来的秒数
extern gtime_t epoch2time(const double* ep);
//UTC转GPS时，以gtime_t格式表达
extern gtime_t utc2gpst(gtime_t t);
extern void time2epoch(gtime_t t, double* ep);
//Time转UTC数组形式
double *Time2utc(Time time);
//返回两个gtime_t对象相减的值
extern double timediff(gtime_t t1, gtime_t t2);
//跟据卫星prn号首字节，返回对于的卫星系统
int satsys(const char* prn);
//根据code的值，返回一个指向obscodes[code]的指针
const char* code2obs(uint8_t code);
//返回各系统载波频率和载波类型代号
static int code2freq_GPS(uint8_t code, double* freq);
static int code2freq_GLO(uint8_t code,int fcn, double* freq);
static int code2freq_GAL(uint8_t code, double* freq);
static int code2freq_QZS(uint8_t code, double* freq);
static int code2freq_SBS(uint8_t code, double* freq);
static int code2freq_BDS(uint8_t code, double* freq);
static int code2freq_IRN(uint8_t code, double* freq);
//返回当前系统载波类别，如GPS：L1,L2,L5
extern int code2idx(int sys, uint8_t code);
//返回输入的信号编码在obscodes数组中的位置
extern uint8_t obs2ccode(const char* obs);
//获取各系统信道优先级
extern int getcodepri(int sys, uint8_t code);
//设置信号指标
void set_index(int sys, char tobs[MAXOBSTYPE][4], sigind_t* ind);
//科学计数法转换
std::string replaceDWithE(const std::string& str);
//空间直角坐标系转大地坐标系
BLHPoint XYZToBLH(const Point& point);
//将源字符串 src 复制到目标字符串 dst，但会去除末尾的空格，并确保目标字符串以空字符 '\0' 结尾
void setstr(char* dst, const char* src, int n);
//空间直角坐标系转导航系
bool XYZToENU(const Point& station, const SatPoint& satpoint, ENUPoint& ENU);
//观测文件检测
bool CheckDatas(obs_epoch& oData);
//获取最小卫星间隔
int MinOFNums(const std::vector<double>& nums);
//计算卫星高度角，并对观测数据进行处理
double Elevation(const Point& station, const SatPoint &satpoint);
//电离层延迟改正(Klobuchar模型)
double Ionmodel(const nav_head& nData, const obs_epoch& oData, const Point& station, const SatPoint& satpoint);
//对流层延迟改正
double Trop(const Point& station, const SatPoint& satpoint);
//获取历元时间数据
bool getobstime(obs_epoch& epochdata, const std::string line);
//获取观测文件数据
inline obsd_t getobsdata(const std::string line, sigind_t* index, const char* str);
//gtime_t转GTime
GTime time2gpst(gtime_t gtime);
//选择最佳星历
const nav_body *seleph(gtime_t time, const char* p, std::string prn, const std::vector<nav_body>& nav);


