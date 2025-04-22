#include<iostream>
#include<stdio.h>
#include"spp_per.h"
#include<string>
#include<vector>
#include"position.h"
#include"ReadFile.h"
using namespace std;
int main()
{
	//Êý¾Ý¶ÁÈ¡
	string N = "cut01680.14n";
	string O = "cut01680.14o";//wuhn1230.16o cut01680.14o
	ReadFile r(O, N);
	r._nfile.Readnav_head();
	OutputResult(r, "result1", 10);

	return 0;
}