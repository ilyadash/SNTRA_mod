#include "CoupleOfExperiments.cpp"
using namespace std;

class SetOfExpCouples
{//класс вектора пар комплиментарных экспермиентов срыва-подхвата
	public:
	vector<CoupleOfExperiments*> data;
	TCanvas *cc1=new TCanvas("cc1","cc1",1000,600);//TCanvas - класс cern_root (холст, где всё отрисовывается), первый - для результатов расчёта без нормировки
	TCanvas *cc2=new TCanvas("cc2","cc2",1000,600);//сделаем второй холст для результатов нормировки
	void CreateCouplesOfExperiments(vector<Experiment> &Pickup, vector<Experiment> &Stripping,
	parameters &par);
	void CalcSPE_and_OCC();
	void CalculatePenaltyFunction();
	void PrintCalculationResult(string OutputFileName, string output_dir="output");
	void ArrangeByPenalty();
};

class SetOfNormalisedExpCouples : public SetOfExpCouples
{//класс вектора пар комплиментарных экспермиентов срыва-подхвата
	public:
	vector<NormalisedCoupleOfExperiments*> data;
	void CreateNormalisedCouplesOfExperiments(vector<Experiment> &Pickup, vector<Experiment> &Stripping,
	parameters &par);
	void CalcSPE_and_OCC();
	void CalculatePenaltyFunction();
	void PrintCalculationResult(string OutputFileName, string output_dir="output");
	void PrintFitCalculationResult(string OutputFileName, string output_dir="output");
	void ArrangeByPenalty();
	void InduceNormalisation();
	void ReCalcSPE_and_OCC();
};

