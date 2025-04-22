#include"EString.h"
#include"ReadNFile.h"
#include"tools.h"
using namespace std;

#define MAXRINEX 84//一行最大字符数（大于80均可）


//构造函数实现
ReadNFile::ReadNFile(std::string filename) :_filename(filename){}

ReadNFile::ReadNFile(){}

ReadNFile::~ReadNFile(){}

ReadNFile::ReadNFile(const ReadNFile& nfile) : _filename(nfile._filename){}


//提取导航电文头文件
nav_head ReadNFile::Readnav_head()
{
	nav_head header;
	ifstream fread(_filename);//创建只读文件流fread
	if (!fread.is_open())
	{
		cout << "文件打开失败!\n" << endl;
		exit(1);
	}
	string line;
	while (getline(fread, line))
	{
		if (line.find("COMMENT") != string::npos) continue;
		if (line.find("RINEX VERSION / TYPE") != string::npos)
		{
			header.version = stof(line.substr(5, 4));
			header.type = line[20];
		}
		if (line.find("ION ALPHA") != string::npos) //ver.2
		{
			for (int i = 0; i < 4; i++)
			{
				header.IonAlpha[i] = stof(replaceDWithE(line.substr(2 + 12 * i, 12)));
			}
		}
		if (line.find("ION BETA") != string::npos)//ver.2
		{
			for (int i = 0; i < 4; i++)
			{
				header.IonBeta[i] = stof(replaceDWithE(line.substr(2 + 12 * i, 12)));
			}
		}
		if (line.find("DELTA-UTC: A0,A1,T,W") != string::npos)//ver.2
		{
			header.DeltaUtaA[0] = stof(replaceDWithE(line.substr(3, 19)));
			header.DeltaUtaA[1] = stof(replaceDWithE(line.substr(22, 19)));
			header.DeltaUtcT = stof(replaceDWithE(line.substr(41, 9)));
			header.DeltaUtcW = stof(replaceDWithE(line.substr(50, 9)));
		}
		if (line.find("IONOSPHERIC CORR") !=string::npos) //ver.3
		{
			if (line.find("GPSA") != string::npos)
			{
				for (int i = 0, j = 5; i < 4; i++, j += 12)
				{
					header.IonAlpha[i]= stof(replaceDWithE(line.substr(j, 12)));
				}
			}
			if (line.find("GPSB") != string::npos)
			{
				for (int i = 0, j = 5; i < 4; i++, j += 12)
				{
					header.IonBeta[i] = stof(replaceDWithE(line.substr(j, 12)));
				}
			}
		}
		//读取时间系统改正参数
		if (line.find("TIME SYSTEM CORR") != string::npos) //ver.3
		{
			if (line.find("GPUT") != string::npos)
			{
				header.utc_gps[0] = stof(replaceDWithE(line.substr(5, 17)));
				header.utc_gps[1] = stof(replaceDWithE(line.substr(22, 16)));
				header.utc_gps[2] = stof(replaceDWithE(line.substr(38, 7)));
				header.utc_gps[3] = stof(replaceDWithE(line.substr(45, 5)));
			}
		}
		if (line.find("LEAP SECONDS") != string::npos)
		{
			//header.LeapSeconds = stof(replaceDWithE(line.substr(0, 6)));
			header.utc_gps[4] = stof(replaceDWithE(line.substr(0, 6)));
			header.utc_gps[5] = stof(replaceDWithE(line.substr(6, 6)));
			header.utc_gps[6] = stof(replaceDWithE(line.substr(12, 6)));
			header.utc_gps[7] = stof(replaceDWithE(line.substr(18, 6)));
		}
		if (line.find("END OF HEADER") != string::npos) break;
	}
	fread.close();
	return header;
}
//提取导航电文卫星轨道参数
vector<nav_body>ReadNFile::ReadNRecord(double ver)
{
	vector<nav_body> records;
	fstream fread(_filename);
	string line;
	//跳过文件头
	while (getline(fread,line))
	{
		if (line.find("END OF HEADER") != string::npos)
		{
			break;
		}
	}
	//开始读取卫星轨道数据
	while (getline(fread,line))
	{
		nav_body record;//临时结构体传参
		if (ver >= 3)
		{
			record.PRN = line.substr(0, 3);
			record.TOC.year = stoi(line.substr(4, 4));
			record.TOC.month = stoi(line.substr(9, 2));
			record.TOC.day = stoi(line.substr(12, 2));
			record.TOC.hour = stoi(line.substr(15, 2));
			record.TOC.minute = stoi(line.substr(18, 2));
			record.TOC.second = stod(line.substr(21, 2));
			record.ClkBias = stod(replaceDWithE(line.substr(24, 19)));
			record.ClkDrift = stod(replaceDWithE(line.substr(43, 19)));
			record.ClkDriftRate = stod(replaceDWithE(line.substr(62, 19)));
			//广播轨道1
			getline(fread, line);
			record.IODE = stod(replaceDWithE(line.substr(5, 19)));
			record.Crs = stod(replaceDWithE(line.substr(24, 19)));
			record.DetaN = stod(replaceDWithE(line.substr(43, 19)));
			record.M0 = stod(replaceDWithE(line.substr(62, 19)));
			//广播轨道2
			getline(fread, line);
			record.Cuc = stod(replaceDWithE(line.substr(5, 19)));
			record.e = stod(replaceDWithE(line.substr(24, 19)));
			record.Cus = stod(replaceDWithE(line.substr(43, 19)));
			record.sqrtA = stod(replaceDWithE(line.substr(62, 19)));
			//广播轨道3
			getline(fread, line);
			record.TOE = stod(replaceDWithE(line.substr(5, 19)));
			record.Cic = stod(replaceDWithE(line.substr(24, 19)));
			record.OMEGA = stod(replaceDWithE(line.substr(43, 19)));
			record.Cis = stod(replaceDWithE(line.substr(62, 19)));
			//广播轨道4
			getline(fread, line);
			record.i0 = stod(replaceDWithE(line.substr(5, 19)));
			record.Crc = stod(replaceDWithE(line.substr(24, 19)));
			record.omega = stod(replaceDWithE(line.substr(43, 19)));
			record.OMEGA_DOT = stod(replaceDWithE(line.substr(62, 19)));
			//广播轨道5
			getline(fread, line);
			record.IDOT = stod(replaceDWithE(line.substr(5, 19)));
			record.L2 = stod(replaceDWithE(line.substr(24, 19)));
			record.GPSWeek = int(stod(replaceDWithE(line.substr(43, 19))));
			record.L2_P = stod(replaceDWithE(line.substr(62, 19)));
			//广播轨道6
			getline(fread, line);
			record.SatAccuracy = stod(replaceDWithE(line.substr(5, 19)));
			record.SatHealthy = stod(replaceDWithE(line.substr(24, 19)));
			record.TGD = stod(replaceDWithE(line.substr(43, 19)));
			record.IODC = stod(replaceDWithE(line.substr(62, 19)));
			//广播轨道7
			getline(fread, line);
			record.TransmissionTimeofMessage = stod(replaceDWithE(line.substr(4, 19)));
			record.FitInterval = atof(line.substr(22, 19).size() == 0 ? "0" : line.substr(23, 19).c_str());
			if (line.substr(42, 38).size() > 0)
			{
				record.Reamrk1 = atof(line.substr(42, 19).c_str());
				record.Remark2 = atof(line.substr(61, 19).c_str());
			}
			else
			{
				record.Reamrk1 = 0;
				record.Remark2 = 0;
			}
			records.push_back(record);//将每个历元的临时结构体传入动态数组
		}
		else
		{ 
			record.PRN = stoi(line.substr(0, 2)) < 10 ? "G0" + line.substr(1, 1) : "G" + line.substr(0, 2);
			record.TOC.year = stoi(line.substr(3, 2)) >= 0 && stoi(line.substr(3, 2)) < 80 ? stoi(line.substr(3, 2)) + 2000 : stoi(line.substr(3, 2)) + 1900;
			record.TOC.month = stoi(line.substr(6, 2));
			record.TOC.day = stoi(line.substr(9, 2));
			record.TOC.hour = stoi(line.substr(12, 2));
			record.TOC.minute = stoi(line.substr(15, 2));
			record.TOC.second = stod(line.substr(17, 5));
			record.ClkBias = stod(replaceDWithE(line.substr(22, 19)));
			record.ClkDrift = stod(replaceDWithE(line.substr(41, 19)));
			record.ClkDriftRate = stod(replaceDWithE(line.substr(60, 19)));
			//广播轨道1
			getline(fread, line);
			record.IODE = stod(replaceDWithE(line.substr(3, 19)));
			record.Crs = stod(replaceDWithE(line.substr(22, 19)));
			record.DetaN = stod(replaceDWithE(line.substr(41, 19)));
			record.M0 = stod(replaceDWithE(line.substr(60, 19)));
			//广播轨道2
			getline(fread, line);
			record.Cuc = stod(replaceDWithE(line.substr(3, 19)));
			record.e = stod(replaceDWithE(line.substr(22, 19)));
			record.Cus = stod(replaceDWithE(line.substr(41, 19)));
			record.sqrtA = stod(replaceDWithE(line.substr(60, 19)));
			//广播轨道3
			getline(fread, line);
			record.TOE = stod(replaceDWithE(line.substr(3, 19)));
			record.Cic = stod(replaceDWithE(line.substr(22, 19)));
			record.OMEGA = stod(replaceDWithE(line.substr(41, 19)));
			record.Cis = stod(replaceDWithE(line.substr(60, 19)));
			//广播轨道4
			getline(fread, line);
			record.i0 = stod(replaceDWithE(line.substr(3, 19)));
			record.Crc = stod(replaceDWithE(line.substr(22, 19)));
			record.omega = stod(replaceDWithE(line.substr(41, 19)));
			record.OMEGA_DOT = stod(replaceDWithE(line.substr(60, 19)));
			//广播轨道5
			getline(fread, line);
			record.IDOT = stod(replaceDWithE(line.substr(3, 19)));
			record.L2 = stod(replaceDWithE(line.substr(22, 19)));
			record.GPSWeek = int(stod(replaceDWithE(line.substr(41, 19))));
			record.L2_P = stod(replaceDWithE(line.substr(60, 19)));
			//广播轨道6
			getline(fread, line);
			record.SatAccuracy = stod(replaceDWithE(line.substr(3, 19)));
			record.SatHealthy = stod(replaceDWithE(line.substr(22, 19)));
			record.TGD = stod(replaceDWithE(line.substr(41, 19)));
			record.IODC = stod(replaceDWithE(line.substr(60, 19)));
			//广播轨道7
			getline(fread, line);
			record.TransmissionTimeofMessage = stod(replaceDWithE(line.substr(3, 19)));
			record.FitInterval = atof(line.substr(22, 19).size() == 0 ? "0" : line.substr(22, 19).c_str());
			if (line.substr(41, 38).size() > 0)
			{
				record.Reamrk1 = atof(line.substr(41, 19).c_str());
				record.Remark2 = atof(line.substr(60, 19).c_str());
			}
			else
			{
				record.Reamrk1 = 0;
				record.Remark2 = 0;
			}
			records.push_back(record);//将每个历元的临时结构体传入动态数组
		}

	}
	fread.close();
	return records;
}