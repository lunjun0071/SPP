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

#define C_V  2.99792458E8 //����//���٣�m��
#define GM 398600500000000//������������
#define math_e 2.718281828459 //eֵ
#define PI 3.141592653589793
#define We 7.292115E-5 //������ת���ٶ�
#define MAX_OBS 3000
#define MAXOBS      96                  /* max number of obs in an epoch */
#define MAX_SAT 50
#define NUMSYS      7                   /* number of systems */
#define MAXOBSTYPE  64                  /* max number of obs type in RINEX */
#define MAXCODE     68                  /* max number of obs code */
#define MAXFREQ     7                   /* ����ز����� */
#define MAXLEAPS    64                  /* max number of leap seconds table */
#define ESTRING_H_
#define WeekSecond 604800 //һ��604800��
#define LMAX 40 //����������Ͻ���
#define WGS84A  6378137.0     //WGS84����ϵ������
#define WGS84f  1/298.257223565 //WGS84����ϵƫ����
#define NFREQ       3                   /* �Զ�����ֵ����code2idx�������ܹ�����������ϵͳ��ЩƵ�ε��ز��ܲ��붨λ */
#define NEXOBS      0                   /* number of extended obs codes ��������Ϊ0��ζ��ֻʹ��3�ֹ۲�ֵ���ж�λ */

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
typedef unsigned short     uint16_t;   //�����ͣ�2�ֽ�
static const double gpst0[] = { 1980,1, 6,0,0,0 }; /* gps time reference */



typedef struct Point  //�ռ�����ϵ
{
	double x = 0;
	double y = 0;
	double z = 0;
}Point;

struct SatPoint  //��������
{
	Point point;
	std::string PRN = "";
};

struct ENUPoint  //վ�ĵ�ƽ����ϵ
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

struct BLHPoint  //�������ϵ,��λΪ����
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
//3.0�汾O�ļ��۲��������ȼ�ɸѡ
typedef struct {                        /* signal index type */
	int n;                              /* number of index ���ջ�ʵ�ʽ��ܵ����ź��������� */
	int idx[MAXOBSTYPE];                /* signal freq-index */
	int pos[MAXOBSTYPE];                /* signal index in obs data (-1:no) */
	uint8_t pri[MAXOBSTYPE];           /* signal priority (15-0) */
	uint8_t type[MAXOBSTYPE];           /* type (0:C,1:L,2:D,3:S) */
	uint8_t code[MAXOBSTYPE];           /* obs-code (CODE_L??) */
	double shift[MAXOBSTYPE];           /* phase shift (cycle) */
} sigind_t;
// ����ͷ�ļ��ṹ��
typedef struct nav_head
{
	double version;  //RINEX��ʽ�İ汾��
	char type;       //�ļ�����
	char system;     //�ļ���������ϵͳ
	double IonAlpha[4];  //�����еĵ�������A1-A4
	double IonBeta[4];   //�����еĵ�������B2-B4
	double utc_gps[8];  /* GPS delta-UTC parameters {A0,A1,Tot,WNt,dt_LS,WN_LSF,DN,dt_LSF} */
	double DeltaUtaA[2]; //���ڼ���UTCʱ������ʱ�Ķ���ʽϵ��A1,A2
	double DeltaUtcT;   //���ڼ���UTCʱ������ʱ�Ĳο�ʱ��
	double DeltaUtcW;  //���ڼ���UTCʱ������ʱ �ο�����
	double LeapSeconds;  //���� ����UTC��GPSʱ�Ĳ���
}nav_head, * pnav_head;

typedef struct gtime_t {
	time_t time; //��ʾ1970��������������
	double second;//�� double ���ͱ�ʾ���� 1s ��ʱ��
	gtime_t operator+(double sec)//��������
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
	gtime_t operator-(double sec)//��������
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

struct GTime  //GPSʱ
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

//�������ݽṹ��
typedef struct nav_body
{
	//���ݿ��һ�����ݣ�
	std::string PRN;//����PRN��
	//��Ԫ��TOC�������ӵĲο�ʱ��
	Time TOC; //�����ӵĲο�ʱ��
	double ClkBias; //  �����ӵ�ƫ��s
	double ClkDrift;  //  �����ӵ�Ư�� s/s
	double ClkDriftRate;  //  �����ӵ�Ư���ٶ�s/(s2)

	//���ݿ�ڶ������ݣ�
	double IODE;//���ݡ���������ʱ��(��������)
	double Crs;//����뾶�����ҵ��͸�������������λ��m��
	double DetaN;//����ƽ���˶����������ֵ֮��(rad/s)
	double M0;//�ο�ʱ���ƽ�����(rad)

	//���ݿ���������ݣ�
	double Cuc;//ά�ȷ��ǵ����ҵ��͸���������(rad)
	double e;//���ƫ����
	double Cus;//������ǵ����ҵ��͸���������(rad)
	double sqrtA;//������ƽ����

	//���ݿ���������ݣ�
	double TOE;//�����Ĳο�ʱ��(GPS������)
	double Cic;//�����ǵ����ҵ��͸���������(rad)
	double OMEGA;//�ο�ʱ�̵�������ྭ
	double Cis;//ά����ǵ����ҵ��͸���������(rad)

	//���ݿ���������ݣ�
	double i0;//�ο�ʱ��Ĺ�����(rad)
	double Crc;//���ƽ�������ҵ��͸���������(m)
	double omega;//���ص�Ǿ�
	double OMEGA_DOT;//������ྭ�仯��(rad)

	//���ݿ���������ݣ�
	double IDOT;//���ص�Ǿ�(rad/s)
	double L2;//L2�ϵ���
	double GPSWeek;//GPS��,��TOEһͬ��ʾ
	double L2_P;//L2,p�����ݱ��

	//���ݿ����������
	double SatAccuracy;//���Ǿ���
	double SatHealthy;//���ǽ���״̬
	double TGD;//sec
	double IODC;//�ӵ���������

	//���ݿ�ڰ�������
	double TransmissionTimeofMessage;//���ķ���ʱ��
	double FitInterval;//�������
	double Reamrk1;//��
	double Remark2;//��

}nav_body, * pnav_body;


//�۲�ֵͷ�ļ�
typedef struct obs_head
{
	std::string version;
	char type; //�ļ�����
	char sattype; //��������
	std::string markername; //��վ��
	std::string markernumber;//��վ���
	std::string observer;//�۲�������
	std::string agency;//�۲ⵥλ
	std::string rec;//���ջ����к�
	std::string rectype;//���ջ��ͺ�
	std::string recvers;//���ջ��汾��
	float antenna_h;//���߸�
	float antenna_e;//���߶������
	float antenna_n;//���߱������
	float interval;//��Ԫ���
	std::string ant;//�������к�
	std::string anttype;//�����ͺ�
	Point PXYZ; //��վ��������
	int sigsnums;//GPS���ջ��ź������������Ｔ����M������Ҳֻ����GPS�ź�����
	std::vector<std::string> sigtypes; //�ź��б�
	Time gtime;//��һ���۲��¼ʱ�̣�GPSʱ
}obs_head, * pobs_head;

enum ObsType {
	C1,
	P1,
	P2,
	L1,
	L2 = 4,
};

typedef struct ASatData //ÿ�����ǵĹ۲�ֵ�Լ����
{
	std::string PRN; //���Ǳ��
	double Obs; //�۲�ֵ
}ASatData;

typedef struct {
	std::string PRN; //���Ǳ��
	uint16_t SNR[NFREQ + NEXOBS]; /* signal strength (0.001 dBHz) */
	uint8_t  LLI[NFREQ + NEXOBS]; /* loss of lock indicator */
	uint8_t code[NFREQ + NEXOBS]; /* code indicator (CODE_???) */
	double L[NFREQ + NEXOBS]; /* observation data carrier-phase (cycle) */
	double P[NFREQ + NEXOBS]; /* observation data pseudorange (m) */
	float  D[NFREQ + NEXOBS]; /* observation data doppler frequency (Hz) */
}obsd_t;

//�۲�ֵ��Ԫ���ݽṹ��
typedef struct obs_epoch
{
	Time gtime; //��Ԫʱ��
	gtime_t epochSecondtime;
	int satsums;//��������
	int flag;//��Ԫ��־��0��ʾ������1��ʾ��ǰ��Ԫ����һ��Ԫ�����쳣
	std::map<ObsType, std::vector<ASatData>> AllTypeDatas_2;  //rinex2�汾�������������͵Ĺ۲�ֵ
	std::vector<obsd_t> AllTypeDatas_3;//rinex3�汾�����о���ɸѡ�Ĺ۲�ֵ
}obs_epoch, * pobs_epoch;



