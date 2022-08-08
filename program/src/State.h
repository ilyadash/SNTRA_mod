#pragma once
#include "VectorLib.cpp"
#include <iostream>
#include "Parser.cpp"
using namespace std;

vector<TString> AllPrimitiveSubShellsList={
	"1s1/2", "1p3/2", "1p1/2",
	"1d5/2", "2s1/2", "1d3/2",
	"1f7/2", "2p3/2", "1f5/2", "2p1/2",
	"1g9/2", "1g7/2", "2d5/2", "2d3/2", "3s1/2",
	//"1h11/2", "1h9/2", "2f7/2", "2f5/2", "3p3/2", "3p1/2",
	//"1i13/2", "2g9/2", "3d5/2", "1i11/2", "2g7/2", "4s1/2", "3d3/2",
	//"1j15/2"
	};

class StateParameters//класс параметров состояний, измеренных в эксперименте
{
	public:
	double JP;
	int n,l;
	unsigned char couple_flag;//couple_flag показывает, есть ли в "паре" экспериментов pickup или stripping: 1: pickup only, 2:stripping only, 3:pickup and stripping, 0-undefined

	StateParameters();
	StateParameters(int n, int l, double JP, string couple_flag="0", bool to_be_drawn=1);

	unsigned char GetColor();
	void GetQN(int &n_out, int &l_out, double &JP_out);
	bool CompareQN(StateParameters &s);//CompareQN=CompareQauntumNumbers, функция сравнивает значения квантовых чисел двух подоболочек (сравнение равенства подоболочек)
	bool ToBeDrawn=1;//флаг отрисовки на холсте
	bool CheckIfIncludedIn(vector<StateParameters> SubShellsVec);//функция проверяет, встречается ли хоть 1 раз в поданном векторе это состояние
	string GetType();
	bool GetToBeDrawnFlag();
	void SetToBeDrawnFlag(bool flag);
	unsigned char GetCoupleFlag();
	void SetCoupleFlag(unsigned char flag);
	TString GetNLJ();
	void Cout();
};

vector<StateParameters> VectorConvertTStringToStateParameters(vector<TString> v)
{
	//cout<<"VectorConvertTStringToStateParameters has started!\n";
	vector<StateParameters> result;
	for(unsigned int i=0;i<v.size();i++)
	{
		int n, l;
		float JP;
		TStringToNLJ(v[i], n, l, JP);
		//cout<<"VectorConvertTStringToStateParameters creating StateParameters s!\n";
		StateParameters s(n, l, JP, "0");
		//cout<<"VectorConvertTStringToStateParameters creating pushing s!\n";
		result.push_back(s);
	}
	//cout<<"VectorConvertTStringToStateParameters has finished!\n";
	return result;
}

vector<StateParameters> AllPrimitiveSubShells=VectorConvertTStringToStateParameters(AllPrimitiveSubShellsList);

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
{//класс обсчитанных значений для подоболочки данной пары экспериментов
	public:
	int n, L;//главное квантовое число и орб.момент
	double JP;//спин и четность уровня, соответствующего центроиду
	double C;//центроид
	double SumG;//сумма СФ, соответствующая данному JP
	///
	double er_SumG;//ошибка суммы СФ, соответствующая данной подоболочке
	double er_C;//ошибка центроида
	///
	SummarizedSpectroscopicState();
	SummarizedSpectroscopicState(int n_value,int L_value,double JP_value);
	SummarizedSpectroscopicState(int n_value,int L_value,double JP_value, double C_value,double SumG_value);
	const bool Compare(SummarizedSpectroscopicState &S);
	const bool Compare(State &S, int N_index=0, int JP_index=0);
	const bool Compare(int n_value, int L_value, double JP_value);
	const bool operator == (SummarizedSpectroscopicState &v2);
};

class SummarizedSpectroscopicData
{//класс резульаттов расчёта
	public:
	vector<SummarizedSpectroscopicState> States;
	void CreateJPList(State &s);
	void Calculate(vector<State> &states);
	SummarizedSpectroscopicState GetState(int n, int l, double JP);
	int size();
};
