#include<iostream>
#include<fstream>
#include"tools.h"
#include"ReadOFile.h"
using namespace std;


ReadOFile::ReadOFile(string ofile):_ofile(ofile){}
ReadOFile::ReadOFile(){}
ReadOFile::ReadOFile(const ReadOFile& ofile):_ofile(ofile._ofile){}
ReadOFile::~ReadOFile(){}

//��ȡO�ļ�ͷ
obs_head ReadOFile::ReadOheader(char tobs[][MAXOBSTYPE][4])
{
	obs_head header;
	ifstream fread(_ofile);

	if (!fread.is_open())
	{
		cout<< "�۲�ֵ�ļ���ʧ��\n" << endl;
		exit(1);
	}
	string line;
	while (getline(fread,line))
	{
		//�����!=string::npos
        //find���Ҳ���ʱ���᷵��-1�����Ƿ���һ����������ص�unsigened int
        //���ֱ��if��find��Խ�������ȥ
		if (line.find("END OF HEADER") != string::npos) break;
		if (line.find("COMMENT") != string::npos) continue;
		if (line.find("RINEX VERSION / TYPE") != string::npos)
		{
			header.version = line.substr(5, 4);
			header.type = line.substr(20, 1)[0];//ͨ��[0]��ȡ�ַ����ĵ�һ���ַ�
			header.sattype = line.substr(40, 1)[0];
		}
		if (line.find("MARKER NAME") != string::npos)
		{
			header.markername = EString::Trim(line.substr(0, 60));//Trim��TrimRight����δ��ȥ���ַ����ж������
		}
		if (line.find("MARKER NUMBER") != string::npos)
		{
			header.markernumber = EString::TrimRight(line.substr(0, 60));
		}
		if (line.find("OBSERVER / AGENCY") != string::npos)
		{
			header.observer = EString::Trim(line.substr(0, 20));
			header.agency = EString::Trim(line.substr(20, 40));
		}
		if (line.find("ANT # / TYPE") != string::npos)
		{
			header.ant = EString::Trim(line.substr(0, 20));
			header.anttype = EString::Trim(line.substr(20, 20));
		}
		if (line.find("REC # / TYPE / VERS") != string::npos)
		{
			header.rec = EString::TrimRight(line.substr(0, 20));
			header.rectype = EString::TrimRight(line.substr(20, 20));
			header.recvers = EString::TrimRight(line.substr(40, 20));
		}
		if (line.find("APPROX POSITION XYZ") != string::npos)
		{
			header.PXYZ.x = stod(line.substr(0, 14));
			header.PXYZ.y = stod(line.substr(14, 14));
			header.PXYZ.z = stod(line.substr(28, 14));
		}
		if (line.find("ANTENNA: DELTA H/E/N") != string::npos)
		{
			//α�ݵ��㶨λ����Ҫ��������ƫ����������ֻ��Ҫ���߸�
			header.antenna_h = stof(line.substr(0, 14));
			header.antenna_e = stof(line.substr(21, 10));
			header.antenna_n = stof(line.substr(35, 10));
		}
		if (line.find("# / TYPES OF OBSERV") != string::npos)//ver.2
		{
			header.sigsnums = atoi(line.substr(0, 7).c_str());//substr ���ص���һ�� string ���͵Ķ��󣬶� atoi ������Ҫ���� C �����ַ���
			//��ˣ�ʹ�� c_str() �� string ���͵����ַ���ת��Ϊ C �����ַ�����
			if (header.sigsnums <= 9) //���ź����Ͳ���9ʱ
			{
				for (int i = 0; i < header.sigsnums; i++)
				{
					header.sigtypes.push_back(EString::Trim(line.substr(10 + 6 * i, 2)));
				}
			}
			else //���ź����ͳ���9ʱ���м�¼
			{
				for (int i = 0; i < 9; i++)
				{
					header.sigtypes.push_back(EString::Trim(line.substr(10 + 6 * i, 2)));
				}
				getline(fread, line);//�ٶ�һ��
				for (int i = 0; i < header.sigsnums-9; i++)
				{
					header.sigtypes.push_back(EString::Trim(line.substr(10 + 6 * i, 2)));
				}
			}
		}
		else if (line.find("SYS / # / OBS TYPES") != string::npos)//ver.3
		{
			header.sigsnums = atoi(line.substr(1, 5).c_str());
			const char str = *(line.substr(0, 1).c_str());
			const char* p;
			int i, j, nt, n,k;
			if (!(p = strchr(syscodes, str)))//pָ��ǰ����ϵͳ��ʶ����syscodes"GREJSCI"��������е�λ��
			{
				cout << "�޿���ϵͳ����" << endl;
				exit(1);
			}
			i = (int)(p - syscodes);//���� p �� syscodes ��Ԫ��֮��ľ��루���ַ�Ϊ��λ������ p ָ���Ԫ���� syscodes �����е�λ������
			n = header.sigsnums;
			for (j = nt = 0, k = 7; j < n; j++, k += 4) {
				if (k > 58) {   //����߽磬�ٶ�һ��
					if (!getline(fread, line)) break;
					k = 7;
				}
				if (nt < MAXOBSTYPE - 1) setstr(tobs[i][nt++], line.substr(k, 3).c_str(), 3);
			}
			*tobs[i][nt] = '\0';

			/* change BDS B1 code: 3.02 */
			double ver = stof(header.version);
			if (i == 5 && fabs(ver - 3.02) < 1e-3) {
				for (j = 0; j < nt; j++) if (tobs[i][j][1] == '1') tobs[i][j][1] = '2';
			}
		}
		if (line.find("TIME OF FIRST OBS") != string::npos)
		{
			header.gtime.year = atoi(line.substr(0, 6).c_str());
			header.gtime.month = atoi(line.substr(6, 6).c_str());
			header.gtime.day = atoi(line.substr(12, 6).c_str());
			header.gtime.hour = atoi(line.substr(18, 6).c_str());
			header.gtime.minute = atoi(line.substr(24, 6).c_str());
			header.gtime.second = atof(line.substr(30, 13).c_str());
		}
		if (line.find("INTERVAL") != string::npos)
		{
			header.interval = stof(line.substr(0, 10));
		}
	}
	fread.close();
	return header;
}

//��ȡo�ļ��۲�ֵ
std::vector<obs_epoch>ReadOFile::ReadOData(const obs_head &o_head_Data, char tobs[][MAXOBSTYPE][4])
{
	vector<obs_epoch> epochdatas;
	ifstream fread(_ofile);
	obs_head header = o_head_Data;
	if (!fread.is_open()) cout << "O�ļ���ʧ��" << endl;
	//�����ļ�ͷ
	string line;
	while (getline(fread, line))
	{
		if (line.find(("END OF HEADER")) != string::npos)
			break;
	}
	double ver = stof(header.version);
	//��ȡ�۲�����
	if (ver < 3)
	{
		while (getline(fread, line))
		{
			//�����ǰ�е�ǰ 80 ���ַ�Ϊ�գ����еĳ���Ϊ 0��������������
			if (line.substr(0, 80).size() == 0) continue;
			obs_epoch epochdata;//�洢����Ԫ�۲�����
			vector<string> prns;//����PRN���б�
			int year = atoi(line.substr(1, 2).c_str());
			if (year >= 80 && year <= 99) year += 1900;
			else if (year >= 0 && year <= 80) year += 2000;
			epochdata.gtime.year = year;
			epochdata.gtime.month = atoi(line.substr(4, 2).c_str());
			epochdata.gtime.day = atoi(line.substr(7, 2).c_str());
			epochdata.gtime.hour = atoi(line.substr(10, 2).c_str());
			epochdata.gtime.minute = atoi(line.substr(13, 2).c_str());
			epochdata.gtime.second = atof(line.substr(15, 11).c_str());
			epochdata.flag = atoi(line.substr(28, 1).c_str()); //��Ԫ��־
			epochdata.satsums = atoi(line.substr(29, 3).c_str()); //��������
			double epochtime[6] = { epochdata.gtime.year,epochdata.gtime.month,epochdata.gtime.day,
	                               epochdata.gtime.hour,epochdata.gtime.minute,epochdata.gtime.second };
			epochdata.epochSecondtime = epoch2time(epochtime);
			if (epochdata.flag == 1 || epochdata.flag == 4) //����Ԫ�����쳣��ȥ
			{
				for (int i = 0; i < epochdata.satsums; i++)
					getline(fread, line);
				continue;
			}
			string strPRN = EString::TrimRight(line).substr(32, line.size() - 32);
			int linesToRead = (epochdata.satsums - 1) / 12;
			for (int i = 0; i < linesToRead; i++)
			{
				getline(fread, line);
				strPRN += EString::Trim(line);
			}

			//����PRN�б�
			for (string::size_type i = 0; i < strPRN.size(); i += 3)
			{
				string g_prn = strPRN.substr(i, 3);
				if (' ' == g_prn.at(1))
				{
					g_prn.at(1) = '0';
				}
				prns.push_back(g_prn);
			}
			int c1Pos = -1, p1Pos = -1, p2Pos = -1, l1Pos = -1, l2Pos = -1;
			//C1�۲�ֵ��λ��,��0��ʼ
			for (vector<string>::size_type i = 0; i < header.sigtypes.size(); i++)
			{
				if (header.sigtypes.at(i) == "C1")
				{
					c1Pos = i;
					continue;
				}
				if (header.sigtypes.at(i) == "P1")
				{
					p1Pos = i;
					continue;
				}
				if (header.sigtypes.at(i) == "P2")
				{
					p2Pos = i;
					continue;
				}
				if (header.sigtypes.at(i) == "L1")
				{
					l1Pos = i;
					continue;
				}
				if (header.sigtypes.at(i) == "L2")
				{
					l2Pos = i;
					continue;
				}
			}
			//һ����Ԫ�������ǵ��������͵Ĺ۲�����
			for (int i = 0; i < epochdata.satsums; i++)
			{
				//�۲�ֵ���ͳ���5�������м�¼
				int multiply = header.sigsnums % 5 == 0 ? header.sigsnums / 5 : header.sigsnums / 5 + 1;
				string strOrx;
				while (multiply)
				{
					getline(fread, line);
					strOrx += line + string(' ', 80 - line.size());//��ȫ��ʽ
					multiply--;
				}
				//��ȡ�۲�ֵ
				if (c1Pos != -1 && strOrx.substr(c1Pos * 16, 14) != string(' ', 14))
				{
					ASatData asatdata;//��¼ÿ�����ǵĹ۲�ֵ�Լ����
					double obs = atof(strOrx.substr(c1Pos * 16, 14).c_str()) == 0 ? 0 : atof(strOrx.substr(c1Pos * 16, 14).c_str());
					asatdata.Obs = obs;
					asatdata.PRN = prns.at(i);
					epochdata.AllTypeDatas_2[C1].push_back(asatdata);//����C1�۲�ֵ
				}
				if (p1Pos != -1 && strOrx.substr(p1Pos * 16, 14) != string(' ', 14))
				{
					ASatData asatdata;
					double obs = atof(strOrx.substr(p1Pos * 16, 14).c_str()) == 0 ? 0 : atof(strOrx.substr(p1Pos * 16, 14).c_str());
					asatdata.Obs = obs;
					asatdata.PRN = prns.at(i);
					epochdata.AllTypeDatas_2[P1].push_back(asatdata); //����P1�۲�ֵ
				}
				if (p2Pos != -1 && strOrx.substr(p2Pos * 16, 14) != string(' ', 14))
				{
					ASatData asatdata;
					double obs = atof(strOrx.substr(p2Pos * 16, 14).c_str()) == 0 ? 0 : atof(strOrx.substr(p2Pos * 16, 14).c_str());
					asatdata.Obs = obs;
					asatdata.PRN = prns.at(i);
					epochdata.AllTypeDatas_2[P2].push_back(asatdata); //����P2�۲�ֵ
				}
				if (l1Pos != -1 && strOrx.substr(l1Pos * 16, 14) != string(' ', 14))
				{
					ASatData asatdata;
					double obs = atof(strOrx.substr(l1Pos * 16, 14).c_str()) == 0 ? 0 : atof(strOrx.substr(l1Pos * 16, 14).c_str());
					asatdata.Obs = obs;
					asatdata.PRN = prns.at(i);
					epochdata.AllTypeDatas_2[L1].push_back(asatdata); //����L1�۲�ֵ
				}
				if (l2Pos != -1 && strOrx.substr(l2Pos * 16, 14) != string(' ', 14))
				{
					ASatData asatdata;
					double obs = atof(strOrx.substr(l2Pos * 16, 14).c_str()) == 0 ? 0 : atof(strOrx.substr(l2Pos * 16, 14).c_str());
					asatdata.Obs = obs;
					asatdata.PRN = prns.at(i);
					epochdata.AllTypeDatas_2[L2].push_back(asatdata); //����L2�۲�ֵ
				}
			}
			epochdatas.push_back(epochdata);
		}
		fread.close();
		return epochdatas;
	}
	else
	{
		sigind_t indx[NUMSYS] = { {0} };
		int nsys = NUMSYS;
		if (nsys >= 1) set_index(SYS_GPS, tobs[0], indx  );
		if (nsys >= 2) set_index(SYS_GLO, tobs[1], indx+1);
		if (nsys >= 3) set_index(SYS_GAL, tobs[2], indx+2);
		if (nsys >= 4) set_index(SYS_QZS, tobs[3], indx+3);
		if (nsys >= 5) set_index(SYS_SBS, tobs[4], indx+4);
		if (nsys >= 6) set_index(SYS_CMP, tobs[5], indx+5);
		if (nsys >= 7) set_index(SYS_IRN, tobs[6], indx+6);
		while (getline(fread, line))
		{
			obs_epoch epochdata;//�洢����Ԫ�۲�����


			//�����ǰ�е�ǰ 80 ���ַ�Ϊ�գ����еĳ���Ϊ 0��������������
			if (line.empty()) continue; // ��������
			if (line.find('>') != string::npos)
			{
				getobstime(epochdata, line);
				double epochtime[6] = { epochdata.gtime.year,epochdata.gtime.month,epochdata.gtime.day,
					epochdata.gtime.hour,epochdata.gtime.minute,epochdata.gtime.second };
				epochdata.epochSecondtime = epoch2time(epochtime);
				if (epochdata.satsums == 0) getline(fread, line);
				epochdata.flag = atoi(line.substr(31, 1).c_str());
				if (epochdata.flag == 1 || epochdata.flag == 4) //����Ԫ�����쳣��ȥ
				{
					for (int i = 0; i < epochdata.satsums; i++)
						getline(fread, line);
					continue;
				}
				getline(fread, line);
			}
			for (int i = 0; i < epochdata.satsums; ++i)
			{
				//ASatData asatdata;
				obsd_t satdata;
				string PRN = line.substr(0, 3);
				string str = line.substr(0, 1);
				const char* p = str.c_str();

				satdata = getobsdata(line, indx, p);
				satdata.PRN = PRN;

				epochdata.AllTypeDatas_3.push_back(satdata);

				if (i < epochdata.satsums - 1) {
					if (!std::getline(fread, line)) break;
				}
			}
			epochdatas.push_back(epochdata);
		}

		fread.close();
		return epochdatas;
	}
}

//��ȡ��Ԫʱ������
bool getobstime(obs_epoch& epochdata, const string line)
{
	epochdata.gtime.year = atoi(line.substr(2, 4).c_str());
	epochdata.gtime.month = atoi(line.substr(7, 3).c_str());
	epochdata.gtime.day = atoi(line.substr(10, 2).c_str());
	epochdata.gtime.hour = atoi(line.substr(13, 2).c_str());
	epochdata.gtime.minute = atoi(line.substr(16, 2).c_str());
	epochdata.gtime.second = atof(line.substr(19, 10).c_str());
	epochdata.satsums = atoi(line.substr(33, 2).c_str());
	return true;
}

//��ȡ�۲�ֵ����
inline obsd_t getobsdata(const string line, sigind_t* index, const char* str) 
{
	obsd_t satdata;
	sigind_t* ind;
	double val[MAXOBSTYPE] = { 0 };//��ʱ�洢��ǰ��������
	uint8_t lli[MAXOBSTYPE] = { 0 };
	int i, j, p[MAXOBSTYPE];
	switch (satsys(str)) {
	case SYS_GLO: ind = index + 1; break;
	case SYS_GAL: ind = index + 2; break;
	case SYS_QZS: ind = index + 3; break;
	case SYS_SBS: ind = index + 4; break;
	case SYS_CMP: ind = index + 5; break;
	case SYS_IRN: ind = index + 6; break;
	default:      ind = index; break;
	}
	//string strOrx = line;
	//int multiply = line.size() - (ind->n * 16 + 3);
	//if (multiply < 0) {
	//	strOrx += string(abs(multiply), ' '); // ��ȫ��ʽ
	//}
	for (i = 0, j = 3; i < ind->n; i++, j += 16)
	{
		//val[i] = stof(strOrx.substr(j, 14));
		//lli[i] = (uint8_t)stof(strOrx.substr(j+14, 1));
		val[i] = (float)str2num(line.c_str(), j, 14);
		lli[i] = (uint8_t)str2num(line.c_str(), j + 14, 1) & 3;
	}

	for (i = 0; i < NFREQ + NEXOBS; i++)
	{
		satdata.P[i] = satdata.L[i] = 0.0; satdata.D[i] = 0.0f;
		satdata.SNR[i] = satdata.LLI[i] = satdata.code[i] = 0;

	}

	for (i = 0; i < ind->n; i++)
	{
		p[i] = ind->pos[i];
	}
	//����۲�����
	for (i = 0; i < ind->n; i++)
	{
		if (p[i] < 0 || val[i] == 0) continue;//�޳����ȼ�Ϊ-1������ȱʧ�Ĺ۲�ֵ
		switch (ind->type[i])
		{
			case 0: satdata.P[p[i]] = val[i]; satdata.code[p[i]] = ind->code[i]; break;
			case 1: satdata.L[p[i]] = val[i]; satdata.LLI[p[i]] = lli[i]; break;
			case 2: satdata.D[p[i]] = val[i]; (float)val[i]; break;
			case 3: satdata.SNR[p[i]] = (uint16_t)(val[i] / SNR_UNIT + 0.5); break;
			default:
			break;
		}
	}
	return satdata;
}