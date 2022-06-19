#include "State.cpp"
#include "TH1F.h"
#include  <TLegend.h>
#include <TF1.h>
#include <TGraphErrors.h>
#include <TMultiGraph.h>
#include <TLatex.h>
#include <TCanvas.h>
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

class StateParameters//класс парасметров состояний, измеренных в эксперименте
{
	public:
	double JP;
	int n,l;
	unsigned char couple_flag;//couple_flag показывает, есть ли в "паре" экспериментов pickup или stripping: 1: pickup only, 2:stripping only, 3:pickup and stripping, 0-undefined
	StateParameters();
	StateParameters(int n, int l, double JP, string c_flag, bool to_be_drawn=1);
	unsigned char GetColor();
	void GetQN(int &n_out, int &l_out, double &JP_out);
	bool CompareQN(StateParameters &s);//CompareQN=CompareQauntumNumbers, функция сравнивает значения квантовых чисел двух подоболочек (сравнение равенства подоболочек)
	bool ToBeDrawn;//флаг отрисовки на холсте
	bool CheckIfIncludedIn(vector<StateParameters> SubShellsVec);//функция проверяет, встречается ли хоть 1 раз в поданном векторе это состояние
	string GetType();
	bool GetToBeDrawnFlag();
	void SetToBeDrawnFlag(bool flag);
	unsigned char GetCoupleFlag();
	void SetCoupleFlag(unsigned char flag);
	TString GetNLJ();
};

class parameters//класс пользовательских параметров расчёта
{
	public:
	unsigned char IncompleteCouplesFlag;//all=1, pickup only=2, stripping only=3, no=4//флаг использования пар экпериментов разных типов для расчёта
	bool LimitedSubShellsUsedInDrawing=0;//флаг отрисовки только выбранных пользователем в параметрах подоболочек
	vector<StateParameters> SubShellsUsedInAllCalculations;
	vector<StateParameters> SubShellsUsedForOccupancyFit;// подоболочки, которые используются в фите БКШ
	vector<StateParameters> SubShellsUsedInDrawing;//подоболочки, которые должны отрисовываться на холсте (в энергетическом спектре, в фите БКШ)
	vector<unsigned char> UsedPenaltyFunctionComponents;
	string GetComponentName(unsigned int iterator);
	void ReadParameters(string filename);//метод считывает параметры из файла на диске
	void CoutParameters();//метод выводит в терминал считанные в класс параметры расчёта
	bool CheckStateParameters(StateParameters &s);// ?
	bool CheckOccupancy(StateParameters &s);// ?
	void PrintUsedSubShells();
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
	
	int GetNlevels();//возвращает число уровней, для которых вычислены центроиды

	int size();//возвращает число состояний, зарегистрированных в эксперименте
	int SSSsize();
	SummarizedSpectroscopicState& operator [] (int index);
	//vector<TH1F> BuildSpectroscopicFactorHistogram(float &maximum)
	SpectroscopicFactorHistogram BuildSpectroscopicFactorHistogram();
	SpectroscopicFactorHistogram BuildNormSpectroscopicFactorHistogram(double norma);
};

vector<Experiment> SplitExperiment(Experiment &BExperiment)
{
	vector<Experiment> result;
	int version_iterator=0;
	for(unsigned i1=0;i1<BExperiment.IndexesOfMultipleStates.size();i1++)
	{
		int index=BExperiment.IndexesOfMultipleStates[i1];
		for(unsigned int i=0;i<BExperiment.States[index].n.size();i++)
		{
			for(unsigned int j=0;j<BExperiment.States[index].L.size();j++)
			{
				for(unsigned int k=0;k<BExperiment.States[index].JP.size();k++)
				{
					for(unsigned int p=0;p<BExperiment.States[index].SpectroscopicFactor.size();p++)
					{
						if((BExperiment.States[index].n.size()>1)||(BExperiment.States[index].L.size()>1)||(BExperiment.States[index].JP.size()>1)||(BExperiment.States[index].SpectroscopicFactor.size()>1))
						{
							if(BExperiment.States[index].G(k,p)>G_CUT)
							{
								Experiment exp_tmp=BExperiment;
								exp_tmp.States[index].n[0]=BExperiment.States[index].n[i];
								exp_tmp.States[index].L[0]=BExperiment.States[index].L[j];
								exp_tmp.States[index].JP[0]=BExperiment.States[index].JP[k];
								exp_tmp.States[index].SpectroscopicFactor[0]=BExperiment.States[index].SpectroscopicFactor[p];
								exp_tmp.States[index].n.resize(1);
								exp_tmp.States[index].L.resize(1);
								exp_tmp.States[index].JP.resize(1);
								exp_tmp.States[index].SpectroscopicFactor.resize(1);
								version_iterator++;
								exp_tmp.ChangesLog+="ver "+to_string(version_iterator)+": used state "+to_string(exp_tmp.States[index].Energy)+" keV "+NLJToString(exp_tmp.States[index].n[0],exp_tmp.States[index].L[0], exp_tmp.States[index].JP[0])+" SF:"+to_string(exp_tmp.States[index].SpectroscopicFactor[0])+"\n";
								exp_tmp.reference+="_ver"+to_string(version_iterator);
								result.push_back(exp_tmp);
							}
						}
					}
				}
			}
		}
	}
	return result;	
}

void SplitExperiments(vector<Experiment> &Experiments)
{
	int size=Experiments.size();
	for(unsigned int i=0;i<size;i++)
	{
		//cout<<Experiments[i].reference<<"MSize:"<<Experiments[i].IndexesOfMultipleStates.size()<<"\n";
		if((Experiments[i].IndexesOfMultipleStates.size()>0)&&(Experiments[i].IndexesOfMultipleStates.size()<6))
		{
			vector<Experiment> v_Exp=SplitExperiment(Experiments[i]);
			if(v_Exp.size()>0)
			{
				Experiments[i]=v_Exp[0];
				for(unsigned int j=1;j<v_Exp.size();j++)
				{
					Experiments.push_back(v_Exp[j]);
				}
			}
		}
	}
}

class CoupleOfExperiments
{//класс пары комплиментарных экспермиентов срыва-подхвата
	public:
	Experiment Pickup;//эксперимент подхвата
	Experiment Stripping;//эксперимент срыва
	parameters par;//считанные пользовательские параметры
	vector<StateParameters> SP;//
	vector<StateParameters> SP_centroids;//состояния, для которых были вычеслены центроиды C_nlm
	vector<double> SPE;//одночастичные энергии
	vector<double> OCC;//квадраты заселенностей
	vector<double> ParticlesAndHolesSum;//
	vector<double> PenaltyComponents;//
	int Pickup_size;//
	int Stripping_size;//
	
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
	
	TGraph occupancies;//график заселённостей состояний от энергии, использованных для фита БКШ
	TGraph Pickup_occupancies;
	TGraph Stripping_occupancies;
	TGraph Both_occupancies;;
	
	TF1 BCS;//фит заселённостей в зависисмости от энергии (фит БКШ)
	string PenaltyFunction;//
	TH1F BuildPenaltyComponentsHistogram();
	CoupleOfExperiments(Experiment &InpPickup,Experiment &InpStripping);//конструктор, аргументы которого представляют из себя эксперименты по подхвату и срыву
	void GenerateCommonNJPList();
	void CalcSPE_and_OCC(TCanvas *cc1, TCanvas *cc3);
	void ClearCalcResults();
	string ResultsInTextForm(char verbose_level=0);
	void DrawResultsInTextForm(string str);
};

class NormalisedCoupleOfExperiments: public CoupleOfExperiments
{//класс пары комплиментарных экспермиентов срыва-подхвата, которые будут/уже пронормированы
	//нужен для полиморфизма методов
	public:
	double fit_a;//сохраняем в объекте класса подобранный 1-ый параметр прямой y = a + b * x
	double fit_b;//сохраняем в объекте класса подобранный 2-ой параметр прямой y = a + b * x
	double er_fit_a;//сохраняем в объекте класса ошибку подобранного 1-го параметра прямой y = a + b * x
	double er_fit_b;//сохраняем в объекте класса ошибку подобранного 2-го параметра прямой y = a + b * x
	double n_p = 1.;//значение нормировочного коэффициента n+ для данной пары экспериментов
	double n_m = 1.;//значение нормировочного коэффициента n- для данной пары экспериментов
	double er_n_p=0;//значение ошибки нормировочного коэффициента n+ для данной пары экспериментов
	double er_n_m=0;//значение ошибки нормировочного коэффициента n- для данной пары экспериментов
	
	TF1 FIT,FIT2,FIT3;//прямые фитов, использованные в процессе нормировки (для отрисовки в файле нормировки)
	void InduceNormalisation();//функция, которая нормализует данную пару экспериментов, обновляя значения аргументов
	void ReCalcSPE_and_OCC();
	string FitResultsInTextForm(char verbose_level=0);
	string FitResultsInTextForm2(char verbose_level=0);
};
