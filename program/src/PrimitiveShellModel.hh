#include <stdio.h>
#include <vector>
#include <cstring>
#include <sstream>
#include <iostream>
#include <fstream>
#include <cmath>



class SubShell //подоболочка со соответсвующими характеристиками по ОМО
{
	public:
	TString name;//название подоболочки
	char UseFlag;
	double JP;// спин-четность
	int l; //орбитальный момент
	int n; //главное квантовое число
	double GetJP();
	int GetN();
	int GetL();
	void Update();
};
