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



//Ϊ���󿪱ٿռ�
extern double* mat(int n, int m);
//���ַ�������ȡ���ַ���������ת��Ϊ����
extern double str2num(const char* s, int i, int n);
//��UTCʱת��Ϊ1970������������
extern gtime_t epoch2time(const double* ep);
//UTCתGPSʱ����gtime_t��ʽ���
extern gtime_t utc2gpst(gtime_t t);
extern void time2epoch(gtime_t t, double* ep);
//TimeתUTC������ʽ
double *Time2utc(Time time);
//��������gtime_t���������ֵ
extern double timediff(gtime_t t1, gtime_t t2);
//��������prn�����ֽڣ����ض��ڵ�����ϵͳ
int satsys(const char* prn);
//����code��ֵ������һ��ָ��obscodes[code]��ָ��
const char* code2obs(uint8_t code);
//���ظ�ϵͳ�ز�Ƶ�ʺ��ز����ʹ���
static int code2freq_GPS(uint8_t code, double* freq);
static int code2freq_GLO(uint8_t code,int fcn, double* freq);
static int code2freq_GAL(uint8_t code, double* freq);
static int code2freq_QZS(uint8_t code, double* freq);
static int code2freq_SBS(uint8_t code, double* freq);
static int code2freq_BDS(uint8_t code, double* freq);
static int code2freq_IRN(uint8_t code, double* freq);
//���ص�ǰϵͳ�ز������GPS��L1,L2,L5
extern int code2idx(int sys, uint8_t code);
//����������źű�����obscodes�����е�λ��
extern uint8_t obs2ccode(const char* obs);
//��ȡ��ϵͳ�ŵ����ȼ�
extern int getcodepri(int sys, uint8_t code);
//�����ź�ָ��
void set_index(int sys, char tobs[MAXOBSTYPE][4], sigind_t* ind);
//��ѧ������ת��
std::string replaceDWithE(const std::string& str);
//�ռ�ֱ������ϵת�������ϵ
BLHPoint XYZToBLH(const Point& point);
//��Դ�ַ��� src ���Ƶ�Ŀ���ַ��� dst������ȥ��ĩβ�Ŀո񣬲�ȷ��Ŀ���ַ����Կ��ַ� '\0' ��β
void setstr(char* dst, const char* src, int n);
//�ռ�ֱ������ϵת����ϵ
bool XYZToENU(const Point& station, const SatPoint& satpoint, ENUPoint& ENU);
//�۲��ļ����
bool CheckDatas(obs_epoch& oData);
//��ȡ��С���Ǽ��
int MinOFNums(const std::vector<double>& nums);
//�������Ǹ߶Ƚǣ����Թ۲����ݽ��д���
double Elevation(const Point& station, const SatPoint &satpoint);
//������ӳٸ���(Klobucharģ��)
double Ionmodel(const nav_head& nData, const obs_epoch& oData, const Point& station, const SatPoint& satpoint);
//�������ӳٸ���
double Trop(const Point& station, const SatPoint& satpoint);
//��ȡ��Ԫʱ������
bool getobstime(obs_epoch& epochdata, const std::string line);
//��ȡ�۲��ļ�����
inline obsd_t getobsdata(const std::string line, sigind_t* index, const char* str);
//gtime_tתGTime
GTime time2gpst(gtime_t gtime);
//ѡ���������
const nav_body *seleph(gtime_t time, const char* p, std::string prn, const std::vector<nav_body>& nav);


