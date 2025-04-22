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
	//获取星历中编号为PRN的所有卫星星历数据
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

//计算卫星位置
SatPoint SatPosition(const Time& obstime, gtime_t Sectime,const Point& stationPoint, double pL, const nav_body& nData, double& SatCLK, double LEAPSeconds, double Rr)
{
	SatPoint satpoint;
	satpoint.PRN = nData.PRN;
	double TransTime0 = 0, TransTime1 = 0;
	//信号接收时刻
	GTime GobsTime = obstime.UTC2GTime();
	//卫星信号传播时间
	TransTime1 = pL / C_V - Rr + SatCLK;
	//卫星信号传播时间需要收敛
	while (fabs(TransTime0-TransTime1)>1.0e-10)
	{
		TransTime0 = TransTime1;
		//计算卫星平均角速度
		double n0 = sqrt(GM) / pow(nData.sqrtA, 3);//参考时刻TOE的平均角速度
		double n = n0 + nData.DetaN;
		//卫星发送信号的时刻=接收机接收时刻-信号传播时间
		GTime SendTime;//信号发射时刻
		if ((GobsTime.seconds - TransTime1) < 0)//如果结果为负，则表示跨越了一周的时间边界
		{
			SendTime.week = GobsTime.week - 1;
			SendTime.seconds = GobsTime.seconds - TransTime1 + 7 * 86400;
		}
		else
		{
			SendTime.week = GobsTime.week;
			SendTime.seconds = GobsTime.seconds - TransTime1;
		}
		//TransTime归化时刻
		double TransTime = (SendTime.week - nData.GPSWeek) * 7 * 86400 + (SendTime.seconds - nData.TOE);
		//顾及一周（604800）开始或结束
		if (TransTime > 302400) TransTime -= 604800;
		if (TransTime < -302400) TransTime += 604800;
		double M = nData.M0 + n * TransTime;
		//计算偏近点角
		double E = M;
		double E0 = 0;
		while (fabs(E-E0)>1.0e-14)//牛顿-拉夫森法
		{
			E0 = E;
			E = E - (M + nData.e * sin(E0) - E0) / (nData.e * cos(E0) - 1);
		}
		//计算真近点角
		double f = atan2(sqrt(1.0 - nData.e * nData.e) * sin(E), cos(E) - nData.e);
		//5.计算升交角距
		double u1 = nData.omega + f;
		//6.计算摄动改正项
		double kesi_u = nData.Cuc * cos(2 * u1) + nData.Cus * sin(2 * u1);
		double kesi_r = nData.Crc * cos(2 * u1) + nData.Crs * sin(2 * u1);
		double kesi_i = nData.Cic * cos(2 * u1) + nData.Cis * sin(2 * u1);
		//7.对 u' r' i'进行摄动改正
		double u = u1 + kesi_u;
		double r = nData.sqrtA * nData.sqrtA * (1 - nData.e * cos(E)) + kesi_r;
		double i = nData.i0 + kesi_i + nData.IDOT * TransTime;
		//8.计算卫星在轨道面坐标系中的位置
		double sat_x = r * cos(u);
		double sat_y = r * sin(u);
		//9.计算观测瞬间升交点的经度L
		double L = nData.OMEGA + (nData.OMEGA_DOT - We) * TransTime - We * nData.TOE;
		//10.计算卫星在瞬时地球坐标系中的位置
		satpoint.point.x = sat_x * cos(L) - sat_y * cos(i) * sin(L);
		satpoint.point.y = sat_x * sin(L) + sat_y * cos(i) * cos(L);
		satpoint.point.z = sat_y * sin(i);
		//11.地球自转改正,将卫星位置归化到接受信号时刻的位置
		double deta_a = We * TransTime1;
		double earth_x = satpoint.point.x, earth_y = satpoint.point.y;
		satpoint.point.x = earth_x * cos(deta_a) + earth_y * sin(deta_a);
		satpoint.point.y = earth_y * cos(deta_a) - earth_x * sin(deta_a);
		satpoint.point.z = satpoint.point.z;
		//计算相对论效应
		double deta_tr = -2 * sqrt(GM) * nData.sqrtA / pow(C_V, 2) * nData.e * sin(E);
		//计算卫星钟差
		double deta_toc = (SendTime.week - (nData.TOC).UTC2GTime().week) * 7 * 86400 + (SendTime.seconds - (nData.TOC).UTC2GTime().seconds);
		SatCLK = nData.ClkBias + nData.ClkDrift * deta_toc + nData.ClkDriftRate * pow(deta_toc, 2) + deta_tr;
		//13.再次计算信号传播时间，考虑卫星钟差
        //卫星到接收机观测距离
		double sat2rec_x2 = pow((satpoint.point.x - stationPoint.x), 2);
		double sat2rec_y2 = pow((satpoint.point.y - stationPoint.y), 2);
		double sat2rec_z2 = pow((satpoint.point.z - stationPoint.z), 2);
		double pL1 = sqrt(sat2rec_x2 + sat2rec_y2 + sat2rec_z2);
		//更新信号传播时间，考虑卫星钟差、接收机钟差
		TransTime1 = pL1 / C_V + SatCLK - Rr;

	}
	return satpoint;
}

//3.0版本卫星位置计算
void ephsatposs(gtime_t time,const nav_body nData, SatPoint &satpoint ,double *vare,double*dts)
{
	satpoint.PRN = nData.PRN;
	double Secweek = time2gpst(time).seconds;//卫星信号发射时刻周内秒
	double tk = Secweek - nData.TOE;//规化时刻
	//顾及一周（604800）开始或结束
	if (tk > 302400) tk -= 604800;
	if (tk < -302400) tk += 604800;

	//计算卫星平均角速度
	double n0 = sqrt(GM) / pow(nData.sqrtA, 3);//参考时刻TOE的平均角速度
	double n = n0 + nData.DetaN;

	double M = nData.M0 + n * tk;
	//计算偏近点角
	double E = M;
	double E0 = 0;
	while (fabs(E - E0) > 1.0e-14)//牛顿-拉夫森法
	{
		E0 = E;
		E = E - (M + nData.e * sin(E0) - E0) / (nData.e * cos(E0) - 1);
	}
	//计算真近点角
	double f = atan2(sqrt(1.0 - nData.e * nData.e) * sin(E), cos(E) - nData.e);
	//5.计算升交角距
	double u1 = nData.omega + f;
	//6.计算摄动改正项
	double kesi_u = nData.Cuc * cos(2 * u1) + nData.Cus * sin(2 * u1);
	double kesi_r = nData.Crc * cos(2 * u1) + nData.Crs * sin(2 * u1);
	double kesi_i = nData.Cic * cos(2 * u1) + nData.Cis * sin(2 * u1);
	//7.对 u' r' i'进行摄动改正
	double u = u1 + kesi_u;
	double r = nData.sqrtA * nData.sqrtA * (1 - nData.e * cos(E)) + kesi_r;
	double i = nData.i0 + kesi_i + nData.IDOT * tk;
	//8.计算卫星在轨道面坐标系中的位置
	double sat_x = r * cos(u);
	double sat_y = r * sin(u);
	//9.计算观测瞬间升交点的经度L
	double L = nData.OMEGA + (nData.OMEGA_DOT - We) * tk - We * nData.TOE;
	//10.计算卫星在瞬时地球坐标系中的位置
	satpoint.point.x = sat_x * cos(L) - sat_y * cos(i) * sin(L);
	satpoint.point.y = sat_x * sin(L) + sat_y * cos(i) * cos(L);
	satpoint.point.z = sat_y * sin(i);
	//11.地球自转改正,将卫星位置归化到接受信号时刻的位置
	double deta_a = We * tk;
	double earth_x = satpoint.point.x, earth_y = satpoint.point.y;
	satpoint.point.x = earth_x * cos(deta_a) + earth_y * sin(deta_a);
	satpoint.point.y = earth_y * cos(deta_a) - earth_x * sin(deta_a);
	satpoint.point.z = satpoint.point.z;
	//卫星钟差相对论改正
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
		const nav_body* eph=NULL;//最佳星历
		const char* p = temp.c_str();
		double Pobs = 0.0;
		double dt = 0.0;//卫星钟差

		if ((*p) != 'G') continue;
		for (int j = 0; j < NFREQ; j++) if ((Pobs = oData.AllTypeDatas_3[i].P[j]) != 0) break;
		time = oData.epochSecondtime - (Pobs / C_V);//信号发射时刻

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

	//观测值检测
	if (!CheckDatas(oData)) return false;
	//初始化卫星钟差
	for (vector<nav_body>::size_type i = 0; i < nData.size(); i++) SatCLKs[nData[i].PRN] = 0.0;
	//控制迭代次数
	int iter_count = 0;

	while (fabs(Position.x - Position1.x) > 1e-8 || fabs(Position.y - Position1.y) > 1e-8 || fabs(Position.z - Position1.z) > 1e-8)
	{
		//构建方程系数
		vector<double> vB; vector<double> vL;
		double Pobs = 0.0; double dt = 0.0; double vare = 0.0; 
		//电离层，对流层计算方差
		double vion, vtrp = 0.0; double var; double azel; double sig;
		SatPoint satpoint; double ver = stof(version);
		if (++iter_count > 50) break;
		Position1 = Position;
		for (int i = 0; i < oData.satsums; i++)
		{
			if (ver <= 2.99)
			{
				if (oData.AllTypeDatas_2[C1][i].PRN.substr(0, 1) != "G") continue;
				//构建观测时间选择最佳轨道卫星
				nav_body _nData = GetNFileByobsTime(const_cast<Time&>(oData.gtime).UTC2GTime(), nData, oData.AllTypeDatas_2[C1].at(i).PRN);
				dt=SatCLKs[_nData.PRN];
				Pobs = oData.AllTypeDatas_2[C1].at(i).Obs;
				//卫星位置计算
				satpoint = SatPosition(oData.gtime, oData.epochSecondtime, Position, Pobs, _nData, dt, LeapSeconds, Rr);
				//观测值剔除
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
				time = oData.epochSecondtime - (Pobs / C_V);//信号发射时刻

				if (!ephclk(time, oData.epochSecondtime, oData.AllTypeDatas_3.at(i).PRN, nData, &dt,p)) {
					continue;
				}
				time = time - dt;
				const nav_body* eph = seleph(time, p, prn, nData);
				ephsatposs(time, *eph, satpoint, &vare,&dt);
			}
			//截止高度角
			azel = Elevation(Position, satpoint);
			if (azel< elvation * PI / 180) continue;
			//电离层延迟改正
			double ION = Ionmodel(n_head_data, oData, Position, satpoint);
			Pobs -= ION; vion = SQR(ION * ERR_BRDCI);
			//对流层改正
			double trop = Trop(Position, satpoint); vtrp = SQR(trop * ERR_SAAS);
			Pobs -= trop; 
			//误差方差
			var = varerr(azel) + vare + vion + vtrp;
			sig = sqrt(var);//中误差


			//构建观测方程
			double deta_x = satpoint.point.x - Position.x;
			double deta_y = satpoint.point.y - Position.y;
			double deta_z = satpoint.point.z - Position.z;
			double Rj= sqrt(deta_x * deta_x + deta_y * deta_y + deta_z * deta_z);
			//观测方程线性化
			double li = deta_x / Rj;
			double mi = deta_y / Rj;
			double ni = deta_z / Rj;
			//观测残差方程
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
		//最小二乘法求参
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
	//读取星历和观测值数据
	char tobs[NUMSYS][MAXOBSTYPE][4] = { {""} };//存储3.0版本O文件信号类型列表
	nav_head n_head_Data = read._nfile.Readnav_head();
	vector<nav_body> n_body_Data = read._nfile.ReadNRecord(n_head_Data.version);
	obs_head o_head_Data = read._ofile.ReadOheader(tobs);
	std::vector<obs_epoch>o_body_Data = read._ofile.ReadOData(o_head_Data, tobs);
	//接收机钟差、卫星钟差
	double Rr = 0;
	map<string, double> satCLKs;
	//输出每个历元的观测结果
	ofstream ResFile(output);
	cout << "历元总数： " << o_body_Data.size() << endl;
	if (!ResFile.is_open())
	{
		return false;
	}
	ResFile << setw(16) << "观测历元" << setw(18) << "卫星数" << setw(8) << "X" << setw(16) << "Y" << setw(12) << "Z" << setw(23) << "接收机钟差\n";
	for (vector<obs_epoch>::size_type i = 0; i < o_body_Data.size(); i++)
	{
		cout << "正在处理： " << i << endl;
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