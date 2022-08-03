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
{//класс гистограмм спектроскопических сил (?)
	public:
	vector<TH1F> Histograms;//
	vector<int> n;
	vector<int> L;
	vector<float> JP;
	string Reference;
	float maximum;
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

	parameters();

	string GetComponentName(unsigned int iterator);
	void ReadParameters(string filename);//метод считывает параметры из файла на диске
	bool CheckStateParameters(StateParameters &s);// ?
	bool CheckBelonging(StateParameters &s, vector<StateParameters> &v);// ?
	void PrintUsedSubShells();
	void Cout();//метод выводит в терминал считанные в класс параметры расчёта
};

class Experiment
{//класс набора экспериментальных данных из одной работы (?)
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
	//vector<TH1F> BuildSpectroscopicFactorHistogram(float &maximum)
	SpectroscopicFactorHistogram BuildSpectroscopicFactorHistogram(double norma);
};

class CoupleOfExperiments
{//класс пары комплиментарных экспермиентов срыва-подхвата
	public:
	
	Experiment Pickup;//эксперимент подхвата
	Experiment Stripping;//эксперимент срыва
	parameters par;//считанные пользовательские параметры
	vector<StateParameters> SP;//все состояния, которые указаны в паре экспериментов. каждое состояние знает, где он встречалось, в strip, pickup или обоих (?)
	vector<StateParameters> SP_centroids;//состояния, для которых были вычеслены центроиды C_nlm
	vector<double> SPE;//одночастичные энергии
	vector<double> OCC;//квадраты заселенностей
	vector<double> ParticlesAndHolesSum;//
	vector<double> PenaltyComponents;//
	int Pickup_size;//
	int Stripping_size;//

	double n_p = 1.;//значение нормировочного коэффициента n+ для данной пары экспериментов
	double n_m = 1.;//значение нормировочного коэффициента n- для данной пары экспериментов
	
	vector<double> Gp_c;//вектор спектроскопических сил срыва подоболочек 
	vector<double> Gm_c;//вектор спектроскопических сил подхвата подоболочек 
	vector<double> er_Gp_c;//вектор ошибок спектроскопических сил срыва подоболочек для расчёта нормированных Gp_norm_c
	vector<double> er_Gm_c;//вектор ошибок спектроскопических сил подхвата подоболочек для расчёта нормированных Gm_norm_c
	vector<double> Gp_alt_c;//вектор спектроскопических сил  подоболочек для расчёта через МНК, делённых на 2j+1 (они должны быть близки к 1)
	vector<double> Gm_alt_c;//вектор спектроскопических сил подоболочек для расчёта через МНК, делённых на 2j+1 (они должны быть близки к 1)
	vector<double> er_Gp_alt_c;//вектор ошибок спектроскопических сил подоболочек для расчёта через МНК, делённых на 2j+1
	vector<double> er_Gm_alt_c;//вектор ошибок спектроскопических сил подоболочек для расчёта через МНК, делённых на 2j+1 
	
	TGraphErrors points_G;//объект points_G класса TGraphErrors, точки G+- (спектроскопических сил) в графике
	int p_size=0;//кол-во точек в графиках ,которые должны использоваться в нормировке
	
	double Ef;//энергия Ферми, получаемая в фите БКШ
	double Delta;//\Delta^2//дельта в квадрате, получаемая в фите БКШ
	double Ef_error;//ошибка энергии Ферми, получаемая в фите БКШ
	double Delta_error;//ошибка дельты в квадрате, получаемая в фите БКШ
	double Ef_error_max;//максимальная ошибка энергии Ферми, получаемая в фите БКШ
	double Delta_error_max;	//максимальная ошибка дельты в квадрате, получаемая в фите БКШ
	double penalty;

	//графики заселённостей состояний от энергии, использованных для фита БКШ:
	TGraph occupancies;
	TGraph Pickup_occupancies;
	TGraph Stripping_occupancies;
	TGraph Both_occupancies;;
	
	TF1 BCS;//фит заселённостей в зависисмости от энергии (фит БКШ)
	string PenaltyFunction;//
	TH1F BuildPenaltyComponentsHistogram();

	CoupleOfExperiments(Experiment &InpPickup,Experiment &InpStripping);//конструктор, аргументы которого представляют из себя эксперименты по подхвату и срыву
	
	void GenerateCommonNJPList();//метод заполняет vector<StateParameters> SP данного объекта, генерирует список совпадающих состояний в экспериментах срыва подхвата
	void CalcSPE_and_OCC();
	void ClearCalcResults();
	virtual string ResultsInTextForm(char verbose_level=0);
	void DrawResultsInTextForm(string str);
};

class NormalisedCoupleOfExperiments: public CoupleOfExperiments
{//класс пары комплиментарных экспермиентов срыва-подхвата, которые будут/уже пронормированы
	//нужен для полиморфизма методов
	public:
	bool NormalisationWasTried=0;
	double fit_a;//сохраняем в объекте класса подобранный 1-ый параметр прямой y = a + b * x
	double fit_b;//сохраняем в объекте класса подобранный 2-ой параметр прямой y = a + b * x
	double er_fit_a;//сохраняем в объекте класса ошибку подобранного 1-го параметра прямой y = a + b * x
	double er_fit_b;//сохраняем в объекте класса ошибку подобранного 2-го параметра прямой y = a + b * x
	double er_n_p=0;//значение ошибки нормировочного коэффициента n+ для данной пары экспериментов
	double er_n_m=0;//значение ошибки нормировочного коэффициента n- для данной пары экспериментов

	TGraphErrors points_G_norm;
	TF1 FIT,FIT2,FIT3;//прямые фитов, использованные в процессе нормировки (для отрисовки в файле нормировки)

	NormalisedCoupleOfExperiments(Experiment &InpPickup,Experiment &InpStripping);

	void InduceNormalisation();//функция, которая нормализует данную пару экспериментов, обновляя значения аргументов
	void ReCalcSPE_and_OCC();//функция дял перерасчёта величин одночастичных энергий, заселённостей после нормализации
	string FitResultsInTextForm(char verbose_level=0);
	virtual string ResultsInTextForm(char verbose_level=0);
};
