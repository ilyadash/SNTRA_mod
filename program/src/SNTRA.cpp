#include <stdio.h>
#include "InputData.h"
#include "SetOfExpCouples.cpp"
#include <sstream>
#include <iostream>
#include <fstream>
#include "PrimitiveShellModel.cpp"
//root-зависимые библиотеки:
#include <TGraph.h>
#include <TFile.h>
#include <TSystem.h>
#include <TSystemDirectory.h>
#include <TSystemFile.h>
#include <TLine.h>
#include <TMultiGraph.h>
#include <TROOT.h>
#include <TStyle.h>

using namespace std;

vector<string> ListFiles(string mask) {
	//функция для считывания текстовых файлов без расширения(?)
	cout<<"vector<string> ListFiles(string "<<mask<<") has started!"<<"\n";
	vector<string> FileNames;
	string s;
	FILE* fp;
	char result [1000];
	fp = popen(("ls "+mask).c_str(),"r");
	fread(result,1,sizeof(result),fp);
	s=result;
	fclose (fp);
	stringstream ss(s);
	while(ss) {
		s.resize(0);
		ss>>s;
		FileNames.push_back(s);
	}
	return FileNames;
	cout<<"vector<string> ListFiles(string "<<mask<<") has ended!"<<"\n";
}

vector<string> ListFiles(string dirname, string ext) {//функция ... , выводит названия входных файлов в терминал
	cout<<"vector<string> ListFiles("<<dirname<<", "<<ext<<")  has started!"<<"\n";
	TSystemDirectory dir(dirname.c_str(), dirname.c_str()); 
	TList *files = dir.GetListOfFiles(); 
	vector<string> result;
	if(files) {
		//cout<<"ListFiles("<<dirname<<", "<<ext<<") found some file candidates \n";
		TSystemFile *file; 
		TString fname; 
		TIter next(files);
		while((file=(TSystemFile*)next())) {
			//cout<<"ListFiles("<<dirname<<", "<<ext<<") moving in candidates iteration\n";
			fname = file->GetName(); 
			//cout<<"ListFiles("<<dirname<<", "<<ext<<") checking file "<<file->GetName()<<" extension\n";
			if(!file->IsDirectory() && fname.EndsWith(ext.c_str())) { 
				result.push_back(dirname+(string)fname); 
				//cout<<"ListFiles("<<dirname<<", "<<ext<<") found file "<<(string)fname<<"\n";
			} 
		} 
	}
	else {
		cout<<" *** Error! ListFiles(): TList *files = dir.GetListOfFiles() returned false!"<<endl;
	}
	cout<<"vector<string> ListFiles("<<dirname<<", "<<ext<<") has ended!"<<"\n";
	return result;
}

void ParseCSVFile(string PathToFile) {
	//функция будет парсить пользовательский файл csv, если в нём есть слова в "" с цифра,цифра внутри
	//- удалить кавычки, заменить , на .
}

void ReadFilesInDirectory(string PathToFiles, vector<Experiment> &Pickup, vector<Experiment> &Stripping,
string particle, int ListFilesFlag=0) {
	cout<<"void ReadFilesInDirectory(...) has started!"<<"\n";
	vector<string> FileNames;
	if(ListFilesFlag==0) {
		FileNames=ListFiles(PathToFiles);
	}
	else {
		stringstream ss(PathToFiles);
		string path,ext;
		ss>>path;
		ss>>ext;
		if(ext=="txt") {
			//если файлы имеют формат txt, то запустить крипт в директории расположения по переводу xls в txt
			///не готово, пользователь сам должен сконвертировать файлы заранее с помощью скрипта
			TString output_dir_cmd1=TString::Format("cd %s",path.c_str());
			cout<<"ReadFilesInDirectory(...) executing terminal command '"<<output_dir_cmd1<<"'"<<"\n";
			gSystem->Exec(output_dir_cmd1.Data());
			gSystem->Exec("pwd");
			gSystem->Exec("./convertFiles.sh");
			//gSystem->Exec("./convertFiles.sh");//*/
			/*TString output_dir_cmd2=TString::Format(". %sconvertFiles.sh",dirname.c_str());
			cout<<"ReadFilesInDirectory(...) executing terminal command '"<<output_dir_cmd2<<"'"<<"\n";
			gSystem->Exec(output_dir_cmd2.Data());*/
		}//*/
		FileNames=ListFiles(path,ext); 
	}
	for(unsigned int i=0;i<FileNames.size();i++) {
		cout<<"ReadFilesInDirectory() is reading file "<<FileNames[i]<<"\n";
		Experiment E;
		E.ReadInputFile(FileNames[i]);///нужно реализовать/модифицировать метод для csv файлов
		int Z,A,ParticleType;//ParticleType-передача нейтрона (0) или протона(1)
		string type;//pickup/stripping
		GetAZ(E.Nucleus,Z,A);
		ParceReaction(E.reaction,type,ParticleType);		
		if(ParticleType==1) {
			E.BA1=GetSeparationEnergy(Z+1, A+1, 1,1)*1000;
			E.BA=GetSeparationEnergy(Z, A, 1,1)*1000;
			E.EF_exp = GetFermiEnergy(Z,A, 1, 1)*1000;
			E.particle="proton";
		}
		else {
			E.BA1=GetSeparationEnergy(Z, A+1, 0,1)*1000;
			E.BA=GetSeparationEnergy(Z, A, 0,1)*1000;
			E.EF_exp = GetFermiEnergy(Z,A, 0, 1)*1000;
			E.particle="neutron";
		}
		E.ProcessExperimentalData();
		if((E.particle==particle)||(particle=="")) {
			if(E.GetType()=="pickup") {
				Pickup.push_back(E);
			}
			else if(E.GetType()=="stripping") {
				Stripping.push_back(E);
			}
			SplitExperiments(Pickup);
			SplitExperiments(Stripping);	
		}
	}
	cout<<"void ReadFilesInDirectory has ended!"<<"\n";
}

void SNTRA(string PathToFiles, string particle="", int ListFilesFlag=0, string output_dir_path="output")
{cout<<"void SNTRA has started!"<<"\n";
	vector<Experiment> Pickup;//создаём вектор всех экспериментов подхвата
	vector<Experiment> Stripping;//создаём вектор всех экспериментов срыва
	ReadFilesInDirectory(PathToFiles,Pickup,Stripping,particle,ListFilesFlag);//считаем поготовленные файлы данных с диска
	parameters par=parameters();
	stringstream s1(PathToFiles);
	string ParFileName;
	s1>>ParFileName;
	par.ReadParameters(ParFileName+"parameters.par");//считаем пользовательские параметры из файла parameters.par на диске
	//par.Cout();//выведем считанные параметры в терминал
	
	SetOfNormalisedExpCouples Analysis;
	Analysis.CreateCouplesOfExperiments(Pickup,Stripping,par);
	//cout<<"Analysis.data.size() = "<<Analysis.data.size()<<endl;
	//cout<<"Analysis.SetOfExpCouples::data.size() = "<<Analysis.SetOfExpCouples::data.size()<<endl;
	Analysis.CalcSPE_and_OCC();
	
	string OutputFileName;//создаём строку с именем выходного файла для результата расчёта SNTRA до нормировки
	string OutputFileName2;//создаём строку с именем выходного файла для результата нормировки
	string OutputFileName3;//создаём строку с именем второго выходного файла для результата нормировки
	if((Pickup.size()>0)&&(Stripping.size()>0))//если есть хоть 1 эксперимент срыва и хоть 1 эксперимент подхвата, то
	{//задаём имя выходного pdf файла в строку OutputFileName
		OutputFileName=Pickup[0].Nucleus+"_"+Pickup[0].particle;//OutputFileName равен название ядра _ налетающий нуклон
		OutputFileName2=OutputFileName+"_norm";//OutputFileName2 равен название ядра _ налетающий нуклон_norm
		OutputFileName3=OutputFileName+"_norm2";//OutputFileName3 равен название ядра _ налетающий нуклон_norm2
	}//получаем для название типа "32S_neutron" для pdf и txt файлов
	else {
		return ;//заканчиваем нашу функцию SNTRA здесь
	}
	Analysis.CalculatePenaltyFunction();//применяем функцию для вычисления штрафной функции
	Analysis.ArrangeByPenalty();//применяем функцию для сортировки нашего вывода по возрастанию значения штрафной функции
	Analysis.PrintCalculationResult(OutputFileName,output_dir_path);//записывает результат ранжировки для пары экспериментов CE в выходные файлы .txt и .pdf

	Analysis.UpdateNormalisedCouplesOfExperiments();
	Analysis.InduceNormalisation();
	Analysis.ReCalcSPE_and_OCC();
	Analysis.PrintFitCalculationResult(OutputFileName2,output_dir_path);//записывает результат нормировки для пары экспериментов CE в выходные файлы .txt и .pdf

	Analysis.UpdateCouplesOfExperiments();
	Analysis.CalculatePenaltyFunction();//применяем функцию для вычисления штрафной функции
	Analysis.ArrangeByPenalty();//применяем функцию для сортировки нашего вывода по возрастанию значения штрафной функции
	Analysis.PrintCalculationResult(OutputFileName3,output_dir_path);//записывает результат воздействия нормировки для пары экспериментов CE в выходные файлы .txt и .pdf
	cout<<"void SNTRA has ended!"<<"\n";
}//конец функции void SNTRA*/

int main(int argc, char** argv)//главная функция, принимает аргументы из терминала при вызове SNTRA пользователем
{//argc (argument count) и argv (argument vector) - число переданных строк в main через argv и массив переданных в main строк
	gStyle->SetLabelSize(0.05,"xyz");
	gStyle->SetTextSize(0.05);
	gStyle->SetLegendTextSize(0.04);
	gStyle->SetTitleSize(0.04,"xyz");//*/
	gStyle->SetOptStat("");
	TString output_dir=argv[3];
	if(output_dir=="") output_dir="output"; 
	TString output_dir_cmd=TString::Format("mkdir -v %s",output_dir.Data());
	gSystem->Exec(output_dir_cmd.Data());
	string path=argv[1];//так как при запуске SNTRA, например: ./SNTRA ../34S_Neutron/ txt,
	//мы передаём ей директори с входными файлами и тип файлов
	string ext=argv[2];//то очевидно мы сохраняем тоже самое в path и ext, соответственно
	cout<<"Got path to input files: "<<path+" "+ext<<"\n";//выводим путь к директории входных файлов
	//и их расширение (.txt) (см. при запуске SNTRA в терминале это первая строка)
	cout<<"Got path to output files: "<<output_dir<<"\n";
	SNTRA(path+" "+ext,"",1, string(output_dir));//вызываем нашу функцию SNTRA, передавая ей путь ко входным файлам,
	//их расширение и ListFilesFlag=1
}
