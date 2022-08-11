#include "State.cpp"
#include "TH1F.h"
#include <TLegend.h>
#include <TF1.h>
#include <TGraphErrors.h>
#include <TMultiGraph.h>
#include <TLatex.h>
#include <TCanvas.h>
#include <TString.h>
#define G_CUT 0.8 //значение для ???
using namespace std;

class SpectroscopicFactorHistogram
{//класс гистограмм спектроскопических сил 
	public:
	vector<TH1F> Histograms;//
	vector<int> n;
	vector<int> L;
	vector<float> JP;
	string Reference;
	float maximum_x,maximum_y;
	TLegend* Legend;//хорошие разработчики, сделали "=" protected...
	void PrintSpectroscopicFactorHistogram();
};

class parameters//класс пользовательских параметров расчёта
{
	public:
	unsigned char IncompleteCouplesFlag;//all=1, pickup only=2, stripping only=3, no=4//флаг использования пар экпериментов разных типов для расчёта
	bool LimitedSubShellsUsedInDrawing;//флаг отрисовки только выбранных пользователем в параметрах подоболочек
	vector<StateParameters> SubShellsUsedInAllCalculations;// подоболочки, которые используются во всех вычислениях, а остальные будут игнорироваться (?)
	vector<StateParameters> SubShellsUsedForOccupancyFit;// подоболочки, которые используются в фите БКШ
	vector<StateParameters> SubShellsUsedInDrawing;//подоболочки, которые должны отрисовываться на холсте (в энергетическом спектре, в фите БКШ)
	vector<StateParameters> SubShellsUsedForNormalisation;//подоболочки, для которых выписываются и решаются уравнения, нужные для нахождения нормировочных коэффициентов
	vector<unsigned char> UsedPenaltyFunctionComponents;
	unsigned int NParticlesInShell;
	unsigned int NHolesInShell;

	parameters();

	string GetComponentName(unsigned int iterator);
	void ReadParameters(string filename);//метод считывает параметры из файла на диске
	bool CheckStateParameters(StateParameters &s);// ?
	bool CheckBelonging(StateParameters &s, vector<StateParameters> &v);// ?
	void PrintUsedSubShells();
	void Cout();//метод выводит в терминал считанные в класс параметры расчёта
};

class Experiment
{//класс набора экспериментальных данных из одной работы 
	public:
	string reference;
	string reaction;
	string particle;
	string Nucleus;
	float ProjectileEnergy;
	char type;
	double JP0;
	//parameters *par;
	
	vector<State> States;
	vector<int> IndexesOfMultipleStates;
	SummarizedSpectroscopicData SSD;
	int E_iterator, n_iterator,  L_iterator, JP_iterator, SF_iterator;
	double BA;//Энергия отделения нуклона от ядра А
	double BA1;//Энергия отделения нуклона от ядра А+1
	
	string ChangesLog;
	int GetColor(int L, float JP);
	Experiment();
	string GetType();
	
	void ReadInputFile(string filename);//просто чтение файла с данными. Сначала поиск ключевых слов, если их нет, то попытка считать строку как состояние, наблюдаемое в эксперименте
	void ProcessExperimentalData();//надо будет добавить перебор n и j
	double GetCentroid(int n,int l,double JP_inp);//Возвращает центроид для данных JP
	double GetSumSF(int n,int l,double JP_inp);//Возвращает сумму СФ для данных JP	
	double GetErSumSF(int n,int l,double JP_inp);//Возвращает ошибку суммы СФ для данных n,l,JP
	double GetErSumSF(StateParameters &s); 
	double GetCentroid(StateParameters &s);//Возвращает центроид для данных StateParameters
	double GetSumSF(StateParameters &s);//Возвращает сумму СФ для данных StateParameters
	
	int GetNCalculatedLevels();//возвращает число уровней, для которых вычислены центроиды
	int size();//возвращает число состояний, зарегистрированных в эксперименте
	SummarizedSpectroscopicState& operator [] (int index);
	SpectroscopicFactorHistogram BuildSpectroscopicFactorHistogram(double norma);
};

