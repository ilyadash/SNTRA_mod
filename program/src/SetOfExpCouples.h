#include "CoupleOfExperiments.cpp"
using namespace std;

class SetOfExpCouples
{//класс вектора пар комплиментарных экспермиентов срыва-подхвата
	public:
	vector<CoupleOfExperiments> data;
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
	vector<NormalisedCoupleOfExperiments> data;
	void CreateNormalisedCouplesOfExperiments(vector<Experiment> &Pickup, vector<Experiment> &Stripping,
	parameters &par);
	void CalculatePenaltyFunction();
	void PrintCalculationResult(string OutputFileName, string output_dir="output");
	void PrintFitCalculationResult(string OutputFileName, string output_dir="output");
	void ArrangeByPenalty();
	void InduceNormalisation();
	void ReCalcSPE_and_OCC();
};

/*class NormalisedCoupleOfExperiments: public CoupleOfExperiments
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
*/
