#include"tools.h"
#include"Matrix.h"

using namespace std;



/* string to number ------------------------------------------------------------
* convert substring in string to number
* args   : char   *s        I   string ("... nnn.nnn ...")
*          int    i,n       I   substring position and width
* return : converted number (0.0:error)
*-----------------------------------------------------------------------------*/
extern double str2num(const char *s, int i, int n)
{
    double value;
    char str[256],*p=str;
    
    if (i<0||(int)strlen(s)<i||(int)sizeof(str)-1<n) return 0.0;
    for (s+=i;*s&&--n>=0;s++) *p++=*s=='d'||*s=='D'?'E':*s;
    *p='\0';
    return sscanf_s(str,"%lf",&value)==1?value:0.0;
}
//Ϊ���󿪱ٿռ�
extern double* mat(int n, int m)
{
	double* p;
	if (n <= 0 || m <= 0) return NULL;
	if (!(p = (double*)malloc(sizeof(double) * n * m))) {
		cout << "matrix memory allocation error" << endl;
	}
	return p;
}

//��������prn�����ֽڣ����ض��ڵ�����ϵͳ
int satsys(const char* prn)
{
	int sys = SYS_NONE;
	switch (*prn)
	{
		case 'G': return sys = SYS_GPS; break;
		case 'R': return sys = SYS_GLO; break;
		case 'E': return sys = SYS_GAL; break;
		case 'J': return sys = SYS_QZS; break;
		case 'S': return sys = SYS_SBS; break;
		case 'C': return sys = SYS_CMP; break;
		case 'I': return sys = SYS_IRN; break;
		default: break;
	}
}

//����code��ֵ������һ��ָ��obscodes[code]��ָ��
const char* code2obs(uint8_t code)
{
	if (code <= CODE_NONE || MAXCODE < code) return NULL;
	return obscodes[code];
}


//���ظ�ϵͳ�ز�Ƶ�ʺ��ز����ʹ���
static int code2freq_GPS(uint8_t code, double* freq)
{
	const char* obs = code2obs(code);

	switch (obs[0])
	{
		case '1': *freq = FREQ1; return 0; //L1
		case '2': *freq = FREQ2; return 1; //L2
		case '5': *freq = FREQ5; return 2; //L5
	}
	return -1;
}
/* GLONASS obs code to frequency ---------------------------------------------*/
static int code2freq_GLO(uint8_t code, int fcn, double* freq)
{
	const char* obs = code2obs(code);

	if (fcn < -7 || fcn>6) return -1;

	switch (obs[0]) {
	case '1': *freq = FREQ1_GLO + DFRQ1_GLO * fcn; return 0; /* G1 */
	case '2': *freq = FREQ2_GLO + DFRQ2_GLO * fcn; return 1; /* G2 */
	case '3': *freq = FREQ3_GLO;               return 2; /* G3 */
	case '4': *freq = FREQ1a_GLO;              return 0; /* G1a */
	case '6': *freq = FREQ2a_GLO;              return 1; /* G2a */
	}
	return -1;
}
/* Galileo obs code to frequency ---------------------------------------------*/
static int code2freq_GAL(uint8_t code, double* freq)
{
	const char* obs = code2obs(code);

	switch (obs[0]) {
	case '1': *freq = FREQ1; return 0; /* E1 */
	case '7': *freq = FREQ7; return 1; /* E5b */
	case '5': *freq = FREQ5; return 2; /* E5a */
	case '6': *freq = FREQ6; return 3; /* E6 */
	case '8': *freq = FREQ8; return 4; /* E5ab */
	}
	return -1;
}
/* QZSS obs code to frequency ------------------------------------------------*/
static int code2freq_QZS(uint8_t code, double* freq)
{
	const char* obs = code2obs(code);

	switch (obs[0]) {
	case '1': *freq = FREQ1; return 0; /* L1 */
	case '2': *freq = FREQ2; return 1; /* L2 */
	case '5': *freq = FREQ5; return 2; /* L5 */
	case '6': *freq = FREQ6; return 3; /* L6 */
	}
	return -1;
}

/* SBAS obs code to frequency ------------------------------------------------*/
static int code2freq_SBS(uint8_t code, double* freq)
{
	const char* obs = code2obs(code);

	switch (obs[0]) {
	case '1': *freq = FREQ1; return 0; /* L1 */
	case '5': *freq = FREQ5; return 1; /* L5 */
	}
	return -1;
}
/* BDS obs code to frequency -------------------------------------------------*/
static int code2freq_BDS(uint8_t code, double* freq)
{
	const char* obs = code2obs(code);

	switch (obs[0]) {
	case '1': *freq = FREQ1;     return 0; /* B1C */
	case '2': *freq = FREQ1_CMP; return 0; /* B1I */
	case '7': *freq = FREQ2_CMP; return 1; /* B2I/B2b */
	case '5': *freq = FREQ5;     return 2; /* B2a */
	case '6': *freq = FREQ3_CMP; return 3; /* B3 */
	case '8': *freq = FREQ8;     return 4; /* B2ab */
	}
	return -1;
}
/* NavIC obs code to frequency -----------------------------------------------*/
static int code2freq_IRN(uint8_t code, double* freq)
{
	const char* obs = code2obs(code);

	switch (obs[0]) {
	case '5': *freq = FREQ5; return 0; /* L5 */
	case '9': *freq = FREQ9; return 1; /* S */
	}
	return -1;
}

//����������źű�����obscodes�����е�λ��
extern uint8_t obs2ccode(const char* obs)
{
	int i;

	for (i = 1; *obscodes[i]; i++)
	{
		//��� obscodes[i] ���� obs���򷵻�ֵ���� 0��ִ��return (uint8_t)i����������������ѭ��
		if (strcmp(obscodes[i], obs)) continue;
		return (uint8_t)i;
	}
	return CODE_NONE;
}

//���ص�ǰϵͳ�ز������GPS��L1,L2,L5
extern int code2idx(int sys, uint8_t code)
{
	double freq;

	switch (sys)
	{
		case SYS_GPS: return code2freq_GPS(code, &freq);
		case SYS_GLO: return code2freq_GLO(code,0, &freq);
		case SYS_GAL: return code2freq_GAL(code, &freq);
		case SYS_QZS: return code2freq_QZS(code, &freq);
		case SYS_SBS: return code2freq_SBS(code, &freq);
		case SYS_CMP: return code2freq_BDS(code, &freq);
		case SYS_IRN: return code2freq_IRN(code, &freq);
	}
	return -1;
}

//��ȡ��ϵͳ�ŵ����ȼ�
extern int getcodepri(int sys,uint8_t code)
{
	const char* p;
	const char* obs;
	int i, j;
	switch (sys)
	{
		case SYS_GPS: i = 0; break;
		case SYS_GLO: i = 1; break;
		case SYS_GAL: i = 2; break;
		case SYS_QZS: i = 3; break;
		case SYS_SBS: i = 4; break;
		case SYS_CMP: i = 5; break;
		case SYS_IRN: i = 6; break;
		default: return 0;
	}
	//j�����ز����ͣ�����GPS��L1��L2��L5���ֱ��Ӧ0��1��2
	if ((j = code2idx(sys, code)) < 0) return 0;
	obs = code2obs(code);//��ȡ�ŵ����硰1C��2P��

	return (p = strchr(codepris[i][j], obs[1])) ? 14 - (int)(p - codepris[i][j]) : 0;
}

//�����źŸ���ָ�꣬�Թ۲�ֵ����ɸѡ
void set_index(int sys, char tobs[MAXOBSTYPE][4], sigind_t* ind)
{
	const char* p;
	int i, j, k, n;
	for (i = n = 0; *tobs[i]; i++, n++)
	{
		ind->code[i] = obs2ccode(tobs[i] + 1);//������ĽṹΪ: C1X'\0' tobs[i]+1��������������һ���ַ�
		//strchr�ҵ��ַ���һ�γ��ֵ�λ��,���������λ��
		ind->type[i] = (p = strchr(obstypecodes, tobs[i][0])) ? (int)(p - obstypecodes) : 0;
		ind->idx[i] = code2idx(sys, ind->code[i]);//��ȡ�ز�Ƶ�ʴ���,�ز����ȼ�
		ind->pri[i] = getcodepri(sys, ind->code[i]);//�ŵ����ȼ�������һ�����������14����С0
		for (int temp = 0; temp < MAXOBSTYPE; temp++)
		{
			ind->pos[temp] = -1;//Ƶ�����ȼ�����ͳһ��ʼ��Ϊ-1
		}
	}
	//ɸѡ����ϵͳ��Ƶ���ز�������ŵ�
	for (i = 0; i < NFREQ; i++)
	{
		for (j = 0,k = -1; j < n; j++) {
			if (ind->idx[j] == i && ind->pri[j] && (k<0 || ind->pri[j]>ind->pri[k])) { //����&&ind->pri[j]��������ɸѡ�������ֵ
				k = j;
			}
		}
		if (k < 0) continue;

		for (j = 0; j < n; j++)
		{
			if (ind->code[j] == ind->code[k]) ind->pos[j] = i; //�ۺϿ����ز����ŵ�������ȼ���0��ߣ�1��2��֮
		}
	}
	ind->n = n;
}

//�۲�ֵ���
bool CheckDatas(obs_epoch& oData)
{
	if (oData.satsums < 4)
	{
		return false;
	}
	return true;
}

//�ж������Ƿ�Ϊ������
double norm(const double* vec, int length)
{
	double sum = 0.0;
	for (int i = 0; i < length; i++)
	{
		sum += vec[i] * vec[i];
	}
	return sqrt(sum);
}

//��Դ�ַ��� src ���Ƶ�Ŀ���ַ��� dst������ȥ��ĩβ�Ŀո񣬲�ȷ��Ŀ���ַ����Կ��ַ� '\0' ��β
void setstr(char* dst, const char* src, int n)
{
	char* p = dst;
	const char* q = src;
	while (*q && q < src + n) *p++ = *q++;
	*p-- = '\0';
	while (p >= dst && *p == ' ') *p-- = '\0';
}

//ѡ���������
const nav_body *seleph(gtime_t time, const char* p, string prn, const vector<nav_body>& nav)
{
	double t, tmax, tmin;
	int i, j = -1, sys;
	sys = satsys(p);
	switch (sys) {
	case SYS_GPS: tmax = MAXDTOE + 1.0; break;
	case SYS_GAL: tmax = MAXDTOE_GAL;  break;
	case SYS_QZS: tmax = MAXDTOE_QZS + 1.0; break;
	case SYS_CMP: tmax = MAXDTOE_CMP + 1.0;  break;
	case SYS_IRN: tmax = MAXDTOE_IRN + 1.0;  break;
	default: tmax = MAXDTOE + 1.0; break;
	} 
	tmin = tmax + 1.0;
	double weekSec = time2gpst(time).seconds;//������
	for (i=0;i<nav.size();i++)
	{
		if (nav.at(i).PRN != prn) continue;
		if ((t = fabs(weekSec - nav.at(i).TOE)) > tmax) continue;
		if (t <= tmin) { j = i; tmin = t; }//Ѱ�Ҿ���۲�ʱ������ĵ�����������
	}
	return &(nav.at(j));
}

//�ռ�ֱ������ϵת�������ϵ
BLHPoint XYZToBLH(const Point& point)
{
	BLHPoint BLH;
	double B = 0, B1 = 1;
	double e2 = 2 * WGS84f - WGS84f * WGS84f;//������ĵ�һƫ����ƽ��
	auto N = [e2](double B)// Lambda ���ʽ�����ڼ���î��Ȧ�뾶
	{
		return WGS84A / sqrt(1 - e2 * sin(B) * sin(B));
	};
	while (fabs(B1-B)>1e-8)
	{
		B1 = B;
		B = atan((point.z + N(B) * e2 * sin(B)) / sqrt(pow(point.x, 2) + pow(point.y, 2)));
	}
	double L = atan(point.y / point.x);
	double H = sqrt(pow(point.x, 2) + pow(point.y, 2)) / cos(B) - N(B);
	BLH.B = B;
	BLH.H = H;
	BLH.L = L;
	return BLH;
}

//�ռ�ֱ������ϵת����ϵ
bool XYZToENU(const Point& station, const SatPoint& satpoint, ENUPoint& ENU)
{
	//����������ģֵ
	double distance = 0;
	if (sqrt(pow(satpoint.point.x, 2) + pow(satpoint.point.y, 2) + pow(satpoint.point.z, 2)) < WGS84A) return false;
	double r_vector[3];
	r_vector[0] = satpoint.point.x - station.x;
	r_vector[1] = satpoint.point.y - station.y;
	r_vector[2] = satpoint.point.z - station.z;
	//���ؾ�
	distance = sqrt(pow(r_vector[0], 2) + pow(r_vector[1], 2) + pow(r_vector[2], 2));
	for (int i = 0; i < 3; i++)
	{
		r_vector[i] /= distance;//��һ������
	}
	//����ת������
	BLHPoint rBLH = XYZToBLH(station);
	double E[9];
	E[0] = -sin(rBLH.L);             E[3] = cos(rBLH.L);              E[6] = 0.0;
	E[1] = -sin(rBLH.B) * cos(rBLH.L); E[4] = -sin(rBLH.B) * sin(rBLH.L); E[7] = cos(rBLH.B);
	E[2] = cos(rBLH.B) * cos(rBLH.L);  E[5] = cos(rBLH.B) * sin(rBLH.L);  E[8] = sin(rBLH.B);
	Matrix enu = Matrix(E, 3, 3) * Matrix(r_vector, 3, 1);

	ENU.E = enu.get(0, 0);
	ENU.N = enu.get(1, 0);
	ENU.U = enu.get(2, 0);
	return true;
}

//��ȡ��С���Ǽ��
int MinOFNums(const vector<double>& nums)
{
	double min = nums[0];
	int index = 0;
	for (vector<double>::size_type i = 0; i < nums.size(); i++)
	{
		if (nums.at(i) < min)
		{
			index = i;
			min = nums.at(i);
		}
	}
	return index;
}

//�������Ǹ߶Ƚǣ����Թ۲����ݽ��д���
double Elevation(const Point& station, const SatPoint &satpoint)
{
	ENUPoint enu;
	if (!XYZToENU(station, satpoint, enu)) return true;//���ת��ʧ�ܣ���ͨ�� return true �˳���ǰ����
	//��λ��
	double az = pow(enu.E, 2) + pow(enu.N, 2) < 1E-12 ? 0.0 : atan2(enu.E, enu.N);
	if (az < 0.0) az += 2 * PI;
	//�߶Ƚ�
	double el = asin(enu.U);
	return el;
}
//������ӳٸ���(Klobucharģ��)
double Ionmodel(const nav_head& nData, const obs_epoch& oData, const Point& station, const SatPoint& satpoint)
{
	//Ĭ�ϵ����������飬���û�û���ṩ��������ʱʹ��
	constexpr double ionAlpha_default[4] = { 0.1118E-07, -0.7451E-08, -0.5961E-07, 0.1192E-06 };
	constexpr double ionBeta_default[4] = { 0.1167E+06, -0.2294E+06, -0.1311E+06, 0.1049E+07 };
	BLHPoint blh = XYZToBLH(station);
	ENUPoint enu;

	double ionAlpha[4];  //�����еĵ�������A1-A4
	std::copy(std::begin(nData.IonAlpha), std::end(nData.IonAlpha), ionAlpha);

	double ionBeta[4];   //�����еĵ�������B1-B4
	std::copy(std::begin(nData.IonBeta), std::end(nData.IonBeta), ionBeta);
	if (!XYZToENU(station, satpoint, enu)) return 0;
	//���Ƿ�λ��
	double az = pow(enu.E, 2) + pow(enu.N, 2) < 1E-12 ? 0.0 : atan2(enu.E, enu.N);
	if (az < 0.0) az += 2 * PI;
	//���Ǹ߶Ƚ�
	double azel = asin(enu.U);
	if(blh.H < -1E3 || azel<=0) return 0.0;
	//�������������ģ��Ϊ 0��ʹ��Ĭ�ϵ�������
	if (norm(ionAlpha, 4) + norm(ionBeta, 4) <= 0.0)
	{
		std::copy(std::begin(ionAlpha_default), std::end(ionAlpha_default), ionAlpha);
		std::copy(std::begin(ionBeta_default), std::end(ionBeta_default), ionBeta);
	}
	//������Ľ�
	double psi = 0.0137 / (azel / PI + 0.11) - 0.022;
	//���㴩�̵����γ��
	double phi = blh.B/PI + psi * cos(az);
	if (phi > 0.416) phi = 0.416;// ����γ�ȷ�Χ�� [-0.416, 0.416]
	//���㴩�̵������
	double lam = blh.L / PI + psi * sin(az) / cos(phi * PI);
	phi += 0.064 * cos((lam - 1.617) * PI);  // ���㴩�̵�ش�γ��
	GTime GobsTime = oData.gtime.UTC2GTime();
	//���㴩�̵�ط�ʱ
	double tt = 43200.0 * lam + GobsTime.seconds;
	tt -= floor(tt / 86400.0) * 86400.0;  // 0<=tt<86400
	//����ͶӰϵ��
	double f = 1.0 * 16.0 * pow(0.53 - azel / PI, 3.0);
	//������� amp ������ per
	double amp = ionAlpha[0] + phi * (ionAlpha[1] + phi * (ionAlpha[2] + phi * ionAlpha[3]));
	double per = ionBeta[0] + phi * (ionBeta[1] + phi * (ionBeta[2] + phi * ionBeta[3]));
	amp = std::max(0.0, amp);
	per = std::max(72000.0, per);
	//���������ӳ�
	double x = 2.0 * PI * (tt - 50400.0) / per;

	return C_V * f * (fabs(x) < 1.57 ? 5E-9 + amp * (1.0 + x * x * (-0.5 + x * x / 24.0)) : 5E-9);
}

//�������ӳٸ���
double Trop(const Point& station, const SatPoint& satpoint)
{
	BLHPoint blh = XYZToBLH(station);
	ENUPoint enu;
	if (!XYZToENU(station, satpoint, enu)) return 0;
	//��ƽ���¶�
	const double temp0 = 15.0;
	double hgt, pres, temp, e, z, trph, trpw;
	// ���߶Ⱥ�γ�ȵ���Ч�ԣ�����Ч�򷵻�0
	if (blh.H < -100.0 || 1E4 < blh.B || blh.L <= 0) return 0.0;
	//��׼������ģ�ͼ���
	hgt = blh.H < 0.0 ? 0.0 : blh.H;
	//������ѹ����λ����
	pres = 1013.25 * pow(1.0 - 2.2557E-5 * hgt, 5.2568);
	//�����¶ȣ���λ������
	temp = temp0 - 6.5E-3 * hgt + 273.16;
	//����ˮ��ѹ����λ����
	e = 6.108 * 0.5 * exp((17.15 * temp - 4684.0) / (temp - 38.45));
	// Saastamoinenģ�ͼ���������ӳ�
	z = PI / 2.0 - asin(enu.U);
	//����ɷ����ӳ�
	trph = 0.0022768 * pres / (1.0 - 0.00266 * cos(2.0 * blh.B) - 0.00028 * hgt / 1E3) / cos(z);
	//����ʪ�ӳٷ���
	trpw = 0.002277 * (1255.2 / temp + 0.05) * e / cos(z);

	return trph + trpw;
}