#include "State.cpp"
#include "TH1F.h"
#include  <TLegend.h>
#include <TF1.h>
#include <TGraphErrors.h>
#include <TMultiGraph.h>
#include <TLatex.h>
#include <TCanvas.h>
#define G_CUT 0.8
using namespace std;
class SpectroscopicFactorHistogram
{
	public:
	vector<TH1F> Histograms;
	vector<int> n;
	vector<int> L;
	vector<float> JP;
	string Reference;
	float maximum;
	TLegend* Legend;//хорошие разработчики, сделали "=" protected...
	void PrintSpectroscopicFactorHistogram();
};
class StateParameters
{
	public:
	double JP;
	int n,l;
	unsigned char couple_flag;//couple_flag показывает, есть ли в "паре" pickup или stripping: 1: pickup only, 2:stripping only, 3:pickup and stripping, 0-undefined
	StateParameters();
	StateParameters(int n,int l,double JP,string c_flag);
	unsigned char GetColor();
	bool CompareQN(StateParameters &s);
	string GetType();
};

class parameters
{
	public:
	unsigned char IncompleteCouplesFlag;//all=1, pickup only=2, stripping only=3, no=4
	vector<StateParameters> SubShellsUsedForOccupancyFit;
	vector<unsigned char> UsedPenaltyFunctionComponents;
	string GetComponentName(unsigned int iterator);
	void ReadParameters(string filename);
	bool CheckStateParameters(StateParameters &s);
	bool CheckOccupancy(StateParameters &s);
	string PrintUsedSubShells();
};

class Experiment
{
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
	///кусок3,6 добавленного кода для нормировки 
	double GetErSumSF(int n,int l,double JP_inp);//Возвращает ошибку суммы СФ для данных n,l,JP
	double GetErSumSF(StateParameters &s);
	///конец кусок3,6 добавленного кода для нормировки 
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
{
	public:
	Experiment Pickup;
	Experiment Stripping;
	parameters par;
	vector<StateParameters> SP;
	vector<StateParameters> SP_centroids;
	vector<double> SPE;//одночастичные энергии
	vector<double> OCC;//квадраты заселенностей
	vector<double> ParticlesAndHolesSum;
	vector<double> PenaltyComponents;
	int Pickup_size;
	int Stripping_size;
	
	///кусок5 добавленного кода для нормировки 
	///сохраним как можно больше вещей в переменные класса, чтобы потом передать функции построения выходного файла
	vector<double> Gp_c;//вектор спектроскопических сил срыва подоболочек для расчёта нормированных Gp_norm_c
	vector<double> Gm_c;//вектор спектроскопических сил подхвата подоболочек для расчёта нормированных Gm_norm_c
	vector<double> er_Gp_c;//вектор ошибок спектроскопических сил срыва подоболочек для расчёта нормированных Gp_norm_c
	vector<double> er_Gm_c;//вектор ошибок спектроскопических сил подхвата подоболочек для расчёта нормированных Gm_norm_c
	vector<double> Gp_alt_c;//вектор спектроскопических сил  подоболочек для расчёта через МНК, делённых на 2j+1 (они должны быть близки к 1)
	vector<double> Gm_alt_c;//вектор спектроскопических сил подоболочек для расчёта через МНК, делённых на 2j+1 (они должны быть близки к 1)
	vector<double> er_Gp_alt_c;//вектор ошибок спектроскопических сил подоболочек для расчёта через МНК, делённых на 2j+1
	vector<double> er_Gm_alt_c;//вектор ошибок спектроскопических сил подоболочек для расчёта через МНК, делённых на 2j+1 
	double fit_a;//сохраняем в объекте класса подобранный 1-ый параметр прямой y = a + b * x
	double fit_b;//сохраняем в объекте класса подобранный 2-ой параметр прямой y = a + b * x
	double er_fit_a;//сохраняем в объекте класса ошибку подобранного 1-го параметра прямой y = a + b * x
	double er_fit_b;//сохраняем в объекте класса ошибку подобранного 2-го параметра прямой y = a + b * x
	double n_p = 1.;//значение нормировочного коэффициента n+ для данной пары экспериментов
	double n_m = 1.;//значение нормировочного коэффициента n- для данной пары экспериментов
	double er_n_p;//значение ошибки нормировочного коэффициента n+ для данной пары экспериментов
	double er_n_m;//значение ошибки нормировочного коэффициента n- для данной пары экспериментов
	vector<double> Gp_norm_c;//вектор нормированных спектроскопических сил  подоболочек  
	vector<double> Gm_norm_c;//вектор нормированных спектроскопических сил подоболочек
	
	TGraphErrors points_G;//объект points_G класса TGraphErrors, точки G до нормировки для посторения нормировочного фита этой пары экспериментов
	TGraphErrors points_G_norm;//объект points_G_norm класса TGraphErrors, точки G после нормировки для посторения нормировочного фита этой пары экспериментов
	TF1 FIT;
	///второй фит:
	TF1 FIT2;//объект FIT класса TF1, он нужен для построения фита
	TF1 FIT3;
	
	vector<double> SPE_norm;//вектор вычисленных одночастичных энергий для каждой подоболочки
	vector<double> OCC_norm;//вектор вычисленных квадратов заселенностей для каждой подоболочки
	vector<double> ParticlesAndHolesSum_norm;
	
	TGraph occupancies_norm;//объект occupancies класса TGraph, это график ддля точек заселённости v^2 для БКШ после нормировки
	TGraph Pickup_norm_occupancies;
	TGraph Stripping_norm_occupancies;
	TGraph Both_norm_occupancies;
	TF1 BCS_norm;//объект BCS класса TF1, A TF1 object is a 1-Dim function - график для кривой линии БКШ фита после нормировки
	
	double Ef_norm;//энергия Ферми, получаемая в фите БКШ
	double Delta_norm;//\Delta^2//дельта в квадрате, получаемая в фите БКШ
	double Ef_error_norm;//ошибка энергии Ферми, получаемая в фите БКШ
	double Delta_error_norm;//ошибка дельты в квадрате, получаемая в фите БКШ
	double Ef_error_max_norm;//максимальная ошибка энергии Ферми, получаемая в фите БКШ
	double Delta_error_max_norm;//максимальная ошибка дельты в квадрате, получаемая в фите БКШ
	//double penalty_norm;//переменная значений функции ошибок?
	
	//void CalcSPE_and_OCC_norm(TCanvas *cc1, TCanvas *cc3);
	string FitResultsInTextForm(char verbose_level=0);
	string FitResultsInTextForm2(char verbose_level=0);
	///конец кусок5 добавленного кода для нормировки

	double Ef;
	double Delta;//\Delta^2
	double Ef_error;
	double Delta_error;
	double Ef_error_max;
	double Delta_error_max;	
	double penalty;
	
	TGraph occupancies;
	TGraph Pickup_occupancies;
	TGraph Stripping_occupancies;
	TGraph Both_occupancies;;
	
	TF1 BCS;
	string PenaltyFunction;
	TH1F BuildPenaltyComponentsHistogram();
	CoupleOfExperiments(Experiment &InpPickup,Experiment &InpStripping);//конструктор, аргументы которого представляют из себя эксперименты по подхвату и срыву
	void GenerateCommonNJPList();
	///
	void CalcSPE_and_OCC(TCanvas *cc1, TCanvas *cc3);
	///
	string ResultsInTextForm(char verbose_level=0);
	void DrawResultsInTextForm(string str);
};
