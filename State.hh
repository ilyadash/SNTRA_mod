#pragma once
#include "VectorLib.cpp"
#include <iostream>
#include "Parser.cpp"
using namespace std;
class State //отдельное состояние, зарегистрированное в эксперименте
{
	public:
	char type;//тип: 0->pickup, 1->stripping
	char UseFlag;
	double Energy;//энергия состояния
	vector<double> JP;// вектор из спинов-четностей, 1/2^+ -> 0.5; 1/2^- -> -0.5
	vector<int> L; // орбитальный момент
	vector<int> n;//главное квантовое число
	double JP0; //спин-четность основного состояния
	vector<double> SpectroscopicFactor;// спектроскопический фактор (C^2S)
	char BadFlag=0;
	double G(int JP_Index=0,int SF_Index=0);
	double GetJP(int JP_Index=0);
	double GetN(int N_Index=0);
	int GetL(int L_index=0);	
	State();
	State(string InputString,int E_iterator=0, int n_iterator=1, int L_iterator=2, int JP_iterator=3, int SF_iterator=4);
	int Good();
};

ostream& operator << (ostream &s, State &st)
{
	s<<(int)st.type<<" "<<st.Energy<<" "<<st.n[0]<<" "<<st.L[0]<<" "<<st.JP[0]<<" "<<st.SpectroscopicFactor[0]<<"\n";
	return s;
}

class SummarizedSpectroscopicState
{
	public:
	double JP;//спин и четность уровня, соответствующего центроиду
	double C;//центроид
	double SumG;//сумма СФ, соответствующая данному JP
	///
	double er_SumG;//ошибка суммы СФ, соответствующая данной подоболочке
	double er_C;//ошибка центроида
	///
	int n, L;//главное квантовое число и орб.момент
	SummarizedSpectroscopicState();
	SummarizedSpectroscopicState(int n_value,int L_value,double JP_value);
	SummarizedSpectroscopicState(int n_value,int L_value,double JP_value, double C_value,double SumG_value);
	const bool Compare(SummarizedSpectroscopicState &S);
	const bool Compare(State &S, int N_index=0, int JP_index=0);
	const bool Compare(int n_value, int L_value, double JP_value);
	const bool operator == (SummarizedSpectroscopicState &v2);
};

class SummarizedSpectroscopicData
{
	public:
	
	vector<SummarizedSpectroscopicState> States;
	
	void CreateJPList(State &s);
	void Calculate(vector<State> &states);
	SummarizedSpectroscopicState GetState(int n, int l, double JP);
	int size();
};
