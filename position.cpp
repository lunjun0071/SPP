#include <cmath>
#include <fstream>
#include <iomanip>
#include"ReadFile.h"
#include"Matrix.h"
#include"tools.h"
#include"spp_per.h"
using namespace std;

nav_body GetNFileByobsTime(const GTime& GobsTime, const vector<nav_body>& nDatas, string PRN)
{
	//��ȡ�����б��ΪPRN������������������
	vector<double> timeInterval;
	vector<nav_body> waitGet;
	for (vector<nav_body>::size_type i = 0; i < nDatas.size(); i++)
	{
		if (nDatas.at(i).PRN == PRN)
		{
			double timebias = fabs(nDatas.at(i).TOE - GobsTime.seconds) + fabs(nDatas.at(i).GPSWeek - GobsTime.week) * WeekSecond;
			timeInterval.push_back(timebias);
			waitGet.push_back(nDatas.at(i));
		}
	}
	return waitGet.at(MinOFNums(timeInterval));
}

//��������λ��
SatPoint SatPosition(const Time& obstime, gtime_t Sectime,const Point& stationPoint, double pL, const nav_body& nData, double& SatCLK, double LEAPSeconds, double Rr)
{
	SatPoint satpoint;
	satpoint.PRN = nData.PRN;
	double TransTime0 = 0, TransTime1 = 0;
	//�źŽ���ʱ��
	GTime GobsTime = obstime.UTC2GTime();
	//�����źŴ���ʱ��
	TransTime1 = pL / C_V - Rr + SatCLK;
	//�����źŴ���ʱ����Ҫ����
	while (fabs(TransTime0-TransTime1)>1.0e-10)
	{
		TransTime0 = TransTime1;
		//��������ƽ�����ٶ�
		double n0 = sqrt(GM) / pow(nData.sqrtA, 3);//�ο�ʱ��TOE��ƽ�����ٶ�
		double n = n0 + nData.DetaN;
		//���Ƿ����źŵ�ʱ��=���ջ�����ʱ��-�źŴ���ʱ��
		GTime SendTime;//�źŷ���ʱ��
		if ((GobsTime.seconds - TransTime1) < 0)//������Ϊ�������ʾ��Խ��һ�ܵ�ʱ��߽�
		{
			SendTime.week = GobsTime.week - 1;
			SendTime.seconds = GobsTime.seconds - TransTime1 + 7 * 86400;
		}
		else
		{
			SendTime.week = GobsTime.week;
			SendTime.seconds = GobsTime.seconds - TransTime1;
		}
		//TransTime�黯ʱ��
		double TransTime = (SendTime.week - nData.GPSWeek) * 7 * 86400 + (SendTime.seconds - nData.TOE);
		//�˼�һ�ܣ�604800����ʼ�����
		if (TransTime > 302400) TransTime -= 604800;
		if (TransTime < -302400) TransTime += 604800;
		double M = nData.M0 + n * TransTime;
		//����ƫ�����
		double E = M;
		double E0 = 0;
		while (fabs(E-E0)>1.0e-14)//ţ��-����ɭ��
		{
			E0 = E;
			E = E - (M + nData.e * sin(E0) - E0) / (nData.e * cos(E0) - 1);
		}
		//����������
		double f = atan2(sqrt(1.0 - nData.e * nData.e) * sin(E), cos(E) - nData.e);
		//5.���������Ǿ�
		double u1 = nData.omega + f;
		//6.�����㶯������
		double kesi_u = nData.Cuc * cos(2 * u1) + nData.Cus * sin(2 * u1);
		double kesi_r = nData.Crc * cos(2 * u1) + nData.Crs * sin(2 * u1);
		double kesi_i = nData.Cic * cos(2 * u1) + nData.Cis * sin(2 * u1);
		//7.�� u' r' i'�����㶯����
		double u = u1 + kesi_u;
		double r = nData.sqrtA * nData.sqrtA * (1 - nData.e * cos(E)) + kesi_r;
		double i = nData.i0 + kesi_i + nData.IDOT * TransTime;
		//8.���������ڹ��������ϵ�е�λ��
		double sat_x = r * cos(u);
		double sat_y = r * sin(u);
		//9.����۲�˲��������ľ���L
		double L = nData.OMEGA + (nData.OMEGA_DOT - We) * TransTime - We * nData.TOE;
		//10.����������˲ʱ��������ϵ�е�λ��
		satpoint.point.x = sat_x * cos(L) - sat_y * cos(i) * sin(L);
		satpoint.point.y = sat_x * sin(L) + sat_y * cos(i) * cos(L);
		satpoint.point.z = sat_y * sin(i);
		//11.������ת����,������λ�ù黯�������ź�ʱ�̵�λ��
		double deta_a = We * TransTime1;
		double earth_x = satpoint.point.x, earth_y = satpoint.point.y;
		satpoint.point.x = earth_x * cos(deta_a) + earth_y * sin(deta_a);
		satpoint.point.y = earth_y * cos(deta_a) - earth_x * sin(deta_a);
		satpoint.point.z = satpoint.point.z;
		//���������ЧӦ
		double deta_tr = -2 * sqrt(GM) * nData.sqrtA / pow(C_V, 2) * nData.e * sin(E);
		//���������Ӳ�
		double deta_toc = (SendTime.week - (nData.TOC).UTC2GTime().week) * 7 * 86400 + (SendTime.seconds - (nData.TOC).UTC2GTime().seconds);
		SatCLK = nData.ClkBias + nData.ClkDrift * deta_toc + nData.ClkDriftRate * pow(deta_toc, 2) + deta_tr;
		//13.�ٴμ����źŴ���ʱ�䣬���������Ӳ�
        //���ǵ����ջ��۲����
		double sat2rec_x2 = pow((satpoint.point.x - stationPoint.x), 2);
		double sat2rec_y2 = pow((satpoint.point.y - stationPoint.y), 2);
		double sat2rec_z2 = pow((satpoint.point.z - stationPoint.z), 2);
		double pL1 = sqrt(sat2rec_x2 + sat2rec_y2 + sat2rec_z2);
		//�����źŴ���ʱ�䣬���������Ӳ���ջ��Ӳ�
		TransTime1 = pL1 / C_V + SatCLK - Rr;

	}
	return satpoint;
}

//3.0�汾����λ�ü���
void ephsatposs(gtime_t time,const nav_body nData, SatPoint &satpoint ,double *vare,double*dts)
{
	satpoint.PRN = nData.PRN;
	double Secweek = time2gpst(time).seconds;//�����źŷ���ʱ��������
	double tk = Secweek - nData.TOE;//�滯ʱ��
	//�˼�һ�ܣ�604800����ʼ�����
	if (tk > 302400) tk -= 604800;
	if (tk < -302400) tk += 604800;

	//��������ƽ�����ٶ�
	double n0 = sqrt(GM) / pow(nData.sqrtA, 3);//�ο�ʱ��TOE��ƽ�����ٶ�
	double n = n0 + nData.DetaN;

	double M = nData.M0 + n * tk;
	//����ƫ�����
	double E = M;
	double E0 = 0;
	while (fabs(E - E0) > 1.0e-14)//ţ��-����ɭ��
	{
		E0 = E;
		E = E - (M + nData.e * sin(E0) - E0) / (nData.e * cos(E0) - 1);
	}
	//����������
	double f = atan2(sqrt(1.0 - nData.e * nData.e) * sin(E), cos(E) - nData.e);
	//5.���������Ǿ�
	double u1 = nData.omega + f;
	//6.�����㶯������
	double kesi_u = nData.Cuc * cos(2 * u1) + nData.Cus * sin(2 * u1);
	double kesi_r = nData.Crc * cos(2 * u1) + nData.Crs * sin(2 * u1);
	double kesi_i = nData.Cic * cos(2 * u1) + nData.Cis * sin(2 * u1);
	//7.�� u' r' i'�����㶯����
	double u = u1 + kesi_u;
	double r = nData.sqrtA * nData.sqrtA * (1 - nData.e * cos(E)) + kesi_r;
	double i = nData.i0 + kesi_i + nData.IDOT * tk;
	//8.���������ڹ��������ϵ�е�λ��
	double sat_x = r * cos(u);
	double sat_y = r * sin(u);
	//9.����۲�˲��������ľ���L
	double L = nData.OMEGA + (nData.OMEGA_DOT - We) * tk - We * nData.TOE;
	//10.����������˲ʱ��������ϵ�е�λ��
	satpoint.point.x = sat_x * cos(L) - sat_y * cos(i) * sin(L);
	satpoint.point.y = sat_x * sin(L) + sat_y * cos(i) * cos(L);
	satpoint.point.z = sat_y * sin(i);
	//11.������ת����,������λ�ù黯�������ź�ʱ�̵�λ��
	double deta_a = We * tk;
	double earth_x = satpoint.point.x, earth_y = satpoint.point.y;
	satpoint.point.x = earth_x * cos(deta_a) + earth_y * sin(deta_a);
	satpoint.point.y = earth_y * cos(deta_a) - earth_x * sin(deta_a);
	satpoint.point.z = satpoint.point.z;
	//�����Ӳ�����۸���
	double *ep = Time2utc(nData.TOC);
	gtime_t Toctime_g = epoch2time(ep);
	double TocSecWeek = time2gpst(Toctime_g).seconds;
	tk = Secweek - TocSecWeek;
	*dts = nData.ClkBias + nData.ClkDrift * tk + nData.ClkDriftRate * pow(tk, 2);
	*dts-= 2 * sqrt(GM) * nData.sqrtA / pow(C_V, 2) * nData.e * sin(E);
	if (ep != NULL) free(ep);
	*vare = SQR(2.4);
}

int pntpost(nav_head& n_head_Data, obs_head& o_head_data, obs_epoch& oData, const vector<nav_body>& nData, string version, double elvation)
{
	int n = oData.satsums;
	double* rs = mat(6, n); double* dts=mat(2, n); Matrix var(1, n);
	gtime_t time = { 0 };
	SatPoint satpoint;

	for (int i = 0; i < oData.satsums; i++) {

		string prn = oData.AllTypeDatas_3.at(i).PRN;
		string temp = prn.substr(0, 1);
		const nav_body* eph=NULL;//�������
		const char* p = temp.c_str();
		double Pobs = 0.0;
		double dt = 0.0;//�����Ӳ�

		if ((*p) != 'G') continue;
		for (int j = 0; j < NFREQ; j++) if ((Pobs = oData.AllTypeDatas_3[i].P[j]) != 0) break;
		time = oData.epochSecondtime - (Pobs / C_V);//�źŷ���ʱ��

		if (!ephclk(time, oData.epochSecondtime, oData.AllTypeDatas_3.at(i).PRN, nData, &dt, p)) {
			continue;
		}
		time = time - dt;

		//ephsatposs(time, *eph, satpoint, dts + i * 2);
	}
	return 0;
}

bool CalculationPostion(Point XYZ, nav_head &n_head_Data,obs_head & o_head_data ,obs_epoch& oData, Point& Position, const vector<nav_body>& nData, double LeapSeconds,double& Rr,
	map<string, double>& SatCLKs, string version,double elvation)
{
	Point Position1;
	Position = XYZ;
	int n = oData.satsums;
	nav_head n_head_data = n_head_Data;
	double* rs = mat(6, n); double* dts = mat(2, n); 
	gtime_t time={0};

	//�۲�ֵ���
	if (!CheckDatas(oData)) return false;
	//��ʼ�������Ӳ�
	for (vector<nav_body>::size_type i = 0; i < nData.size(); i++) SatCLKs[nData[i].PRN] = 0.0;
	//���Ƶ�������
	int iter_count = 0;

	while (fabs(Position.x - Position1.x) > 1e-8 || fabs(Position.y - Position1.y) > 1e-8 || fabs(Position.z - Position1.z) > 1e-8)
	{
		//��������ϵ��
		vector<double> vB; vector<double> vL;
		double Pobs = 0.0; double dt = 0.0; double vare = 0.0; 
		//����㣬��������㷽��
		double vion, vtrp = 0.0; double var; double azel; double sig;
		SatPoint satpoint; double ver = stof(version);
		if (++iter_count > 50) break;
		Position1 = Position;
		for (int i = 0; i < oData.satsums; i++)
		{
			if (ver <= 2.99)
			{
				if (oData.AllTypeDatas_2[C1][i].PRN.substr(0, 1) != "G") continue;
				//�����۲�ʱ��ѡ����ѹ������
				nav_body _nData = GetNFileByobsTime(const_cast<Time&>(oData.gtime).UTC2GTime(), nData, oData.AllTypeDatas_2[C1].at(i).PRN);
				dt=SatCLKs[_nData.PRN];
				Pobs = oData.AllTypeDatas_2[C1].at(i).Obs;
				//����λ�ü���
				satpoint = SatPosition(oData.gtime, oData.epochSecondtime, Position, Pobs, _nData, dt, LeapSeconds, Rr);
				//�۲�ֵ�޳�
				if (oData.AllTypeDatas_2[C1].at(i).Obs != 0)
				{
					satpoint = SatPosition(oData.gtime, oData.epochSecondtime, Position, Pobs, _nData, dt, LeapSeconds, Rr);
				}
			}
			else
			{
				string prn = oData.AllTypeDatas_3.at(i).PRN;
				string temp = prn.substr(0, 1);

				const char* p = temp.c_str();

				if ((*p) != 'G') continue;
				for (int j = 0; j < NFREQ; j++) if ((Pobs = oData.AllTypeDatas_3[i].P[j]) != 0) break;
				time = oData.epochSecondtime - (Pobs / C_V);//�źŷ���ʱ��

				if (!ephclk(time, oData.epochSecondtime, oData.AllTypeDatas_3.at(i).PRN, nData, &dt,p)) {
					continue;
				}
				time = time - dt;
				const nav_body* eph = seleph(time, p, prn, nData);
				ephsatposs(time, *eph, satpoint, &vare,&dt);
			}
			//��ֹ�߶Ƚ�
			azel = Elevation(Position, satpoint);
			if (azel< elvation * PI / 180) continue;
			//������ӳٸ���
			double ION = Ionmodel(n_head_data, oData, Position, satpoint);
			Pobs -= ION; vion = SQR(ION * ERR_BRDCI);
			//���������
			double trop = Trop(Position, satpoint); vtrp = SQR(trop * ERR_SAAS);
			Pobs -= trop; 
			//����
			var = varerr(azel) + vare + vion + vtrp;
			sig = sqrt(var);//�����


			//�����۲ⷽ��
			double deta_x = satpoint.point.x - Position.x;
			double deta_y = satpoint.point.y - Position.y;
			double deta_z = satpoint.point.z - Position.z;
			double Rj= sqrt(deta_x * deta_x + deta_y * deta_y + deta_z * deta_z);
			//�۲ⷽ�����Ի�
			double li = deta_x / Rj;
			double mi = deta_y / Rj;
			double ni = deta_z / Rj;
			//�۲�в��
			double l = Pobs - Rj + C_V * dt - C_V * Rr;
			vB.push_back(-li);
			vB.push_back(-mi);
			vB.push_back(-ni);
			vB.push_back(1.0);
			vL.push_back(l);
		}
		if (vL.size() < 4) return false;
		Matrix B(&vB[0], vB.size() / 4, 4); B = B*(1 / sig);
		Matrix L(&vL[0], vL.size(), 1); L = L * (1 / sig);
		Matrix detaX(4, 1);
		//��С���˷����
		detaX = (B.Trans() * B).Inverse() * (B.Trans() * L);


		Position.x += detaX.get(0, 0);
		Position.y += detaX.get(1, 0);
		Position.z += detaX.get(2, 0);
		Rr += detaX.get(3, 0) / C_V;
	}
	return true;
}

bool OutputResult(ReadFile read, string output, double evevation)

{
	//��ȡ�����͹۲�ֵ����
	char tobs[NUMSYS][MAXOBSTYPE][4] = { {""} };//�洢3.0�汾O�ļ��ź������б�
	nav_head n_head_Data = read._nfile.Readnav_head();
	vector<nav_body> n_body_Data = read._nfile.ReadNRecord(n_head_Data.version);
	obs_head o_head_Data = read._ofile.ReadOheader(tobs);
	std::vector<obs_epoch>o_body_Data = read._ofile.ReadOData(o_head_Data, tobs);
	//���ջ��Ӳ�����Ӳ�
	double Rr = 0;
	map<string, double> satCLKs;
	//���ÿ����Ԫ�Ĺ۲���
	ofstream ResFile(output);
	cout << "��Ԫ������ " << o_body_Data.size() << endl;
	if (!ResFile.is_open())
	{
		return false;
	}
	ResFile << setw(16) << "�۲���Ԫ" << setw(18) << "������" << setw(8) << "X" << setw(16) << "Y" << setw(12) << "Z" << setw(23) << "���ջ��Ӳ�\n";
	for (vector<obs_epoch>::size_type i = 0; i < o_body_Data.size(); i++)
	{
		cout << "���ڴ��� " << i << endl;
		Point result;
		if (CalculationPostion(o_head_Data.PXYZ, n_head_Data, o_head_Data,o_body_Data[i], result, n_body_Data, n_head_Data.LeapSeconds, Rr, satCLKs,o_head_Data.version, evevation))
		{
			EString::OutPut(ResFile, o_body_Data[i], Rr, &result);
		}
		else
		{
			EString::OutPut(ResFile, o_body_Data[i], Rr);
		}
	}
	ResFile.flush();
	ResFile.close();

}