#include <stdio.h>
#include "InputData.hh"
#include "Experiment.cpp"
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
//глобальные переменные:
TCanvas *cc1=new TCanvas("cc1","cc1",1000,600);//TCanvas - класс cern_root (холст, где всё отрисовывается), первый - для результатов расчёта без нормировки
TCanvas *cc2=new TCanvas("cc2","cc2",1000,600);//сделаем второй холст для результатов нормировки

vector<string> ListFiles(string mask)//функция для считывания текстовых файлов без расширения(?)
{cout<<"vector<string> ListFiles(string "<<mask<<") has started!"<<"\n";
	vector<string> FileNames;
	string s;
	FILE* fp;
	char result [1000];
	fp = popen(("ls "+mask).c_str(),"r");
	fread(result,1,sizeof(result),fp);
	s=result;
	fclose (fp);
	stringstream ss(s);
	while(ss)
	{
		s.resize(0);
		ss>>s;
		FileNames.push_back(s);
	}
	return FileNames;
	cout<<"vector<string> ListFiles(string "<<mask<<") has ended!"<<"\n";
}

vector<string> ListFiles(string dirname, string ext) //функция ... , выводит названия входных файлов в терминал
{cout<<"vector<string> ListFiles("<<dirname<<", "<<ext<<")  has started!"<<"\n";
	TSystemDirectory dir(dirname.c_str(), dirname.c_str()); 
	TList *files = dir.GetListOfFiles(); 
	vector<string> result;
	if(files) 
	{
		//cout<<"ListFiles("<<dirname<<", "<<ext<<") found some file candidates \n";
		TSystemFile *file; 
		TString fname; 
		TIter next(files);
		while((file=(TSystemFile*)next()))
		{
			//cout<<"ListFiles("<<dirname<<", "<<ext<<") moving in candidates iteration\n";
			fname = file->GetName(); 
			//cout<<"ListFiles("<<dirname<<", "<<ext<<") chacking file "<<file->GetName()<<" extension\n";
			if(!file->IsDirectory() && fname.EndsWith(ext.c_str()))
			{ 
				result.push_back(dirname+(string)fname); 
				//cout<<"ListFiles("<<dirname<<", "<<ext<<") found file "<<(string)fname<<"\n";
			} 
		} 
	}
	else
	{
		cout<<" *** Error! ListFiles(): TList *files = dir.GetListOfFiles() returned false!"<<endl;
	}
	cout<<"vector<string> ListFiles("<<dirname<<", "<<ext<<") has ended!"<<"\n";
	return result;
}

vector<CoupleOfExperiments> CreateCouplesOfExperiments(vector<Experiment> &Pickup, vector<Experiment> &Stripping, parameters &par)//функция создаёт вектор всех вариантов пар экспириментов срыв-подхват (вектор объектов CoupleOfExperiments);
{//функции на вход подаются вектор всех экспериментов срыва и вектор всех экспериментов подхвата (их адреса?)
	cout<<"CreateCouplesOfExperiments has started!"<<endl;
	vector<CoupleOfExperiments> result;
	for(unsigned int i=0;i<Pickup.size();i++)
	{
		cout<<"Got number "<<i+1<<" pickup "<<Pickup[i].reference<<"\n";
		for(unsigned int j=0;j<Stripping.size();j++)
		{
			cout<<"Got number "<<j+1<<" stripping "<<Stripping[j].reference<<"\n";
			CoupleOfExperiments CE(Pickup[i],Stripping[j]);
			CE.par=par;
			result.push_back(CE);
		}
	}
	return result;//возвращаем результирующий вектор пар
	cout<<"CreateCouplesOfExperiments has ended!"<<endl;
}//конец функции vector<CoupleOfExperiments> CreateCouplesOfExperiments

vector<NormalisedCoupleOfExperiments> CreateNormalisedCouplesOfExperiments(vector<Experiment> &Pickup, vector<Experiment> &Stripping, parameters &par)//функция создаёт вектор всех вариантов пар экспириментов срыв-подхват (вектор объектов CoupleOfExperiments);
{//функции на вход подаются вектор всех экспериментов срыва и вектор всех экспериментов подхвата (их адреса?)
	cout<<"CreateNormalisedCouplesOfExperiments has started!"<<endl;
	vector<NormalisedCoupleOfExperiments> result;
	if (Pickup.size()==0) cerr<<"	*** Error! There are no Pickup experiments!"<<endl;
	for(unsigned int i=0;i<Pickup.size();i++)
	{
		cout<<"Got number "<<i+1<<" pickup "<<Pickup[i].reference<<"\n";
		for(unsigned int j=0;j<Stripping.size();j++)
		{
			cout<<"Got number "<<j+1<<" stripping "<<Stripping[j].reference<<"\n";
			NormalisedCoupleOfExperiments CE(Pickup[i],Stripping[j]);
			CE.par=par;
			result.push_back(CE);
		}
	}
	return result;//возвращаем результирующий вектор пар
	cout<<"CreateNormalisedCouplesOfExperiments has ended!"<<endl;
}//конец функции vector<CoupleOfExperiments> CreateCouplesOfExperiments

void ReadFilesInDirectory(string PathToFiles, vector<Experiment> &Pickup, vector<Experiment> &Stripping, string particle, int ListFilesFlag=0)
{cout<<"void ReadFilesInDirectory(...) has started!"<<"\n";
	vector<string> FileNames;
	if(ListFilesFlag==0)
	{
		FileNames=ListFiles(PathToFiles);
	}
	else
	{
		stringstream ss(PathToFiles);
		string path,ext;
		ss>>path;
		ss>>ext;
		if(ext=="txt")
		{
			//если файлы имеют формат txt, то запустить крипт в директории расположения по переводу xls в txt
			///не готово, пользователь сам должен сконвертировать файлы заранее с помощью скрипта
			/*TString output_dir_cmd1=TString::Format("cd ",dirname.c_str());
			cout<<"ReadFilesInDirectory(...) executing terminal command '"<<output_dir_cmd1<<"'"<<"\n";
			gSystem->Exec(output_dir_cmd1.Data());*/
			/*TString output_dir_cmd2=TString::Format(". %sconvertFiles.sh",dirname.c_str());
			cout<<"ReadFilesInDirectory(...) executing terminal command '"<<output_dir_cmd2<<"'"<<"\n";
			gSystem->Exec(output_dir_cmd2.Data());*/
		}//*/
		FileNames=ListFiles(path,ext); 
	}
	////cout<<"size="<<FileNames.size()<<"\n";
	for(unsigned int i=0;i<FileNames.size();i++)
	{
		cout<<"ReadFilesInDirectory() is reading file "<<FileNames[i]<<"\n";
		Experiment E;
		E.ReadInputFile(FileNames[i]);///нужно реализовать/модифицировать метод для csv файлов
		//cout<<E.GetType()<<"\n";
		int Z,A,ParticleType;//ParticleType-передача нейтрона (0) или протона(1)
		string type;//pickup/stripping
		GetAZ(E.Nucleus,Z,A);
		ParceReaction(E.reaction,type,ParticleType);
				
		if(ParticleType==1)
		{///уже здесь энергии считываются неправильно:!!!
			E.BA1=GetSeparationEnergy(Z+1, A+1, 1,1)*1000;
			E.BA=GetSeparationEnergy(Z, A, 1,1)*1000;
			E.particle="proton";
		}
		else
		{
			E.BA1=GetSeparationEnergy(Z, A+1, 0,1)*1000;
			E.BA=GetSeparationEnergy(Z, A, 0,1)*1000;
			E.particle="neutron";
		}
		
		E.ProcessExperimentalData();
		
		if((E.particle==particle)||(particle==""))
		{
			if(E.GetType()=="pickup")
			{
				Pickup.push_back(E);
			}
			else if(E.GetType()=="stripping")
			{
				Stripping.push_back(E);
			}
			SplitExperiments(Pickup);
			SplitExperiments(Stripping);	
		}
	}
	cout<<"void ReadFilesInDirectory has ended!"<<"\n";
}

void CalculatePenaltyFunction(vector<NormalisedCoupleOfExperiments> &v)//функция для расчёта штрафной функции
{cout<<"void CalculatePenaltyFunction has started!"<<"\n";
	float MaxEfError,MaxDeltaError;
	int NumberOfPickupStatesMax=0, NumberOfStrippingStatesMax=0, AverageNumberOfCalculatedStates=0;
	for(int i=0;i<v.size();i++)
	{//определяем в этом цикле максималные ошибки, кол-ва состояний, для дальнейшей оценки каждой пары относительно максимума
		if(MaxEfError<v[i].Ef_error)
		{
			MaxEfError=v[i].Ef_error;
		}
		if(MaxDeltaError<v[i].Delta_error)
		{
			MaxDeltaError=v[i].Delta_error;
		}
		if(NumberOfPickupStatesMax<v[i].Pickup.size())//GetNCalculatedLevels())
		{
			//NumberOfPickupStatesMax=v[i].Pickup.GetNCalculatedLevels();
			NumberOfPickupStatesMax=v[i].Pickup.size();
		}
		if(NumberOfStrippingStatesMax<v[i].Stripping.size())//GetNCalculatedLevels())
		{
			NumberOfStrippingStatesMax=v[i].Stripping.size();//)//GetNCalculatedLevels();
		}
		AverageNumberOfCalculatedStates+=v[i].SPE.size();
	}
	if(v.size()>0)
	AverageNumberOfCalculatedStates=round(AverageNumberOfCalculatedStates/v.size());
	else
	return;
	
	for(unsigned int i=0;i<v.size();i++)
	{
		//v[i].PenaltyComponents.resize(v[i].par.UsedPenaltyFunctionComponents.size());
		//cout<<"size "<<v[i].par.UsedPenaltyFunctionComponents.size()<<"\n";
		for(unsigned int j=0;j<v[i].par.UsedPenaltyFunctionComponents.size();j++)
		{
			//cout<<"comp "<<(int)v[i].par.UsedPenaltyFunctionComponents[j]<<"\n";
			if(v[i].par.UsedPenaltyFunctionComponents[j]==1)
			{
				v[i].PenaltyComponents.push_back(abs(1-Average(v[i].ParticlesAndHolesSum)));
			}
			else if(v[i].par.UsedPenaltyFunctionComponents[j]==2)
			{
				v[i].PenaltyComponents.push_back(1-((float)v[i].Pickup.size()/NumberOfPickupStatesMax));
			}
			else if(v[i].par.UsedPenaltyFunctionComponents[j]==3)
			{
				v[i].PenaltyComponents.push_back(1-((float)v[i].Stripping.size()/NumberOfStrippingStatesMax));
			}
			else if(v[i].par.UsedPenaltyFunctionComponents[j]==4)
			{
				if((MaxEfError!=0)&&(MaxDeltaError!=0))
				{
					v[i].PenaltyComponents.push_back(v[i].Ef_error/MaxEfError);
				}
			}
			else if(v[i].par.UsedPenaltyFunctionComponents[j]==5)
			{
				if((MaxEfError!=0)&&(MaxDeltaError!=0))
				{
					v[i].PenaltyComponents.push_back(v[i].Delta_error/MaxDeltaError);
				}
			}
		}
		
		//cout<<v[i].Pickup.reference<<" "<<v[i].Stripping.reference<<"\n";
		for(unsigned int j=0;j<v[i].PenaltyComponents.size();j++)
		{
			//cout<<"p["<<j<<"]="<<v[i].PenaltyComponents[j]<<" "<<Average(v[i].ParticlesAndHolesSum)<<"\n";
			v[i].penalty+=v[i].PenaltyComponents[j];
		}
		v[i].penalty=v[i].penalty/v[i].PenaltyComponents.size();
	}
	
}//конец void CalculatePenaltyFunction

void CalculatePenaltyFunction(vector<CoupleOfExperiments> &v)//функция для расчёта штрафной функции
{cout<<"void CalculatePenaltyFunction has started!"<<"\n";
	float MaxEfError,MaxDeltaError;
	int NumberOfPickupStatesMax=0, NumberOfStrippingStatesMax=0, AverageNumberOfCalculatedStates=0;
	for(int i=0;i<v.size();i++)
	{//определяем в этом цикле максималные ошибки, кол-ва состояний, для дальнейшей оценки каждой пары относительно максимума
		if(MaxEfError<v[i].Ef_error)
		{
			MaxEfError=v[i].Ef_error;
		}
		if(MaxDeltaError<v[i].Delta_error)
		{
			MaxDeltaError=v[i].Delta_error;
		}
		if(NumberOfPickupStatesMax<v[i].Pickup.size())//GetNCalculatedLevels())
		{
			//NumberOfPickupStatesMax=v[i].Pickup.GetNCalculatedLevels();
			NumberOfPickupStatesMax=v[i].Pickup.size();
		}
		if(NumberOfStrippingStatesMax<v[i].Stripping.size())//GetNCalculatedLevels())
		{
			NumberOfStrippingStatesMax=v[i].Stripping.size();//)//GetNCalculatedLevels();
		}
		AverageNumberOfCalculatedStates+=v[i].SPE.size();
	}
	if(v.size()>0)
	AverageNumberOfCalculatedStates=round(AverageNumberOfCalculatedStates/v.size());
	else
	return;
	
	for(unsigned int i=0;i<v.size();i++)
	{
		//v[i].PenaltyComponents.resize(v[i].par.UsedPenaltyFunctionComponents.size());
		//cout<<"size "<<v[i].par.UsedPenaltyFunctionComponents.size()<<"\n";
		for(unsigned int j=0;j<v[i].par.UsedPenaltyFunctionComponents.size();j++)
		{
			//cout<<"comp "<<(int)v[i].par.UsedPenaltyFunctionComponents[j]<<"\n";
			if(v[i].par.UsedPenaltyFunctionComponents[j]==1)
			{
				v[i].PenaltyComponents.push_back(abs(1-Average(v[i].ParticlesAndHolesSum)));
			}
			else if(v[i].par.UsedPenaltyFunctionComponents[j]==2)
			{
				v[i].PenaltyComponents.push_back(1-((float)v[i].Pickup.size()/NumberOfPickupStatesMax));
			}
			else if(v[i].par.UsedPenaltyFunctionComponents[j]==3)
			{
				v[i].PenaltyComponents.push_back(1-((float)v[i].Stripping.size()/NumberOfStrippingStatesMax));
			}
			else if(v[i].par.UsedPenaltyFunctionComponents[j]==4)
			{
				if((MaxEfError!=0)&&(MaxDeltaError!=0))
				{
					v[i].PenaltyComponents.push_back(v[i].Ef_error/MaxEfError);
				}
			}
			else if(v[i].par.UsedPenaltyFunctionComponents[j]==5)
			{
				if((MaxEfError!=0)&&(MaxDeltaError!=0))
				{
					v[i].PenaltyComponents.push_back(v[i].Delta_error/MaxDeltaError);
				}
			}
		}
		
		//cout<<v[i].Pickup.reference<<" "<<v[i].Stripping.reference<<"\n";
		for(unsigned int j=0;j<v[i].PenaltyComponents.size();j++)
		{
			//cout<<"p["<<j<<"]="<<v[i].PenaltyComponents[j]<<" "<<Average(v[i].ParticlesAndHolesSum)<<"\n";
			v[i].penalty+=v[i].PenaltyComponents[j];
		}
		v[i].penalty=v[i].penalty/v[i].PenaltyComponents.size();
	}
	
}//конец void CalculatePenaltyFunction

void PrintCalculationResult(vector<CoupleOfExperiments> v, string OutputFileName, string output_dir="output")
{//на вход подаётся вектор пар экспериментов и название выходных файлов .pdf .txt OutputFileName(вектор объектов CoupleOfExperiments), функция записывает результаты нормировки в выходные файлы .txt и .pdf
	cout<<"void PrintCalculationResult has started!"<<"\n";
	OutputFileName=string(output_dir)+"/"+OutputFileName;
	ofstream OutputTextFile((OutputFileName+".txt").c_str());//создаём .txt файл с выходными данными
	cc1->Print((OutputFileName+".pdf[").c_str(),"pdf");//создаём .pdf файл с выходными данными, который сейчас будем наполнять графиками и текстом
	for(unsigned int i=0;i<v.size();i++)//для каждой пары экспериментов во входном векторе v
	{	//cout<< "I started pdf writing for the pair!!!!\n";
		SpectroscopicFactorHistogram HistPickup=v[i].Pickup.BuildSpectroscopicFactorHistogram(v[i].n_m);//создаём гистограммы нормированных спектроскопических факторов, которые будем отрисовывать на холсте
		SpectroscopicFactorHistogram HistStrip=v[i].Stripping.BuildSpectroscopicFactorHistogram(v[i].n_p);//для срыва и подхвата, всего 2 гистограммы, итого 2 гистограммы на холсте
		cc1->Clear();//Delete all pad primitives
		cc1->Divide(3,2);//разделить Pad на 3 независимые области по вертикали и на 2 по горизонтали (всего 6 областей)
		//cout<< "I divided pads!!!!\n";
		
		cc1->cd(1);//переходим к Pad1
		HistPickup.PrintSpectroscopicFactorHistogram();//рисуем гистограмму для эксперимента подхвата
		
		cc1->cd(2);//переходим к Pad2
		TMultiGraph* mgr=new TMultiGraph();
		//v[i].occupancies.SetTitle("Occupancy;E,keV;v^2");
		mgr->Add(&v[i].Pickup_occupancies);
		mgr->Add(&v[i].Stripping_occupancies);
		mgr->Add(&v[i].Both_occupancies);
		mgr->SetTitle("Occupancy; E, keV; v^{2}");
		mgr->Draw("ap");
		v[i].occupancies.Draw("p same");//отрисуем заселённости, которые использовались в фите БКШ поверх остальных (выделим их крестами)
		v[i].BCS.Draw("l same");//отрисуем кривую фита БКШ на том же Pad
		mgr->SetTitle("Occupancy; E, keV; v^{2}");
		gPad->Modified();
		gPad->Update();
		
		cc1->cd(3);
		TH1F PenaltyComponents=v[i].BuildPenaltyComponentsHistogram();
		gPad->SetLogy(1);
		PenaltyComponents.Draw();
		
		cc1->cd(4);//переходим к Pad4
		//gPad->SetLogy(1);
		HistStrip.PrintSpectroscopicFactorHistogram();//рисуем гистограмму для эксперимента срыва
		string TextOutput=v[i].ResultsInTextForm(1);
		stringstream s(TextOutput);
		OutputTextFile<<TextOutput<<"\n";//записывем в текстовый файл результаты расчёта
		
		cc1->cd(5);//переходим к Pad5
		TGraph* gr=new TGraph();//"h1","Calculated shell scheme;1 ;E, keV",10,0,1);
		gr->SetTitle("Calculated shell scheme;  ;E, keV");
		gr->SetPoint(0,0,0);
		gr->SetMinimum(GetMinimum(v[i].SPE)-1000);
		gr->SetMaximum(GetMaximum(v[i].SPE)+1000);
		gr->Draw("ap");
		for(unsigned int j=0;j<v[i].SPE.size();j++)
		{
			TLine line;
			TText txt;
			int color=v[i].SP_centroids[j].GetColor();
			line.SetLineColor(color);
			if(v[i].SP_centroids[j].GetToBeDrawnFlag()==1)
			{
				line.DrawLine(0.1,v[i].SPE[j],0.7,v[i].SPE[j]);
				txt.SetTextColor(color);
				txt.DrawText(0.8,v[i].SPE[j], NLJToString(v[i].SP_centroids[j].n,v[i].SP_centroids[j].l,v[i].SP_centroids[j].JP).c_str());
			}
		}
		cc1->cd(6);//переходим к Pad6
		v[i].DrawResultsInTextForm(v[i].ResultsInTextForm());
		cc1->Print((OutputFileName+".pdf").c_str(),"pdf");
		gPad->Update();
		
	}
	cc1->Print((OutputFileName+".pdf]").c_str(),"pdf");
	cout<<"void PrintCalculationResult has ended!"<<"\n";
}

void PrintCalculationResult(vector<NormalisedCoupleOfExperiments> v, string OutputFileName, string output_dir="output")
{//на вход подаётся вектор пар экспериментов и название выходных файлов .pdf .txt OutputFileName(вектор объектов CoupleOfExperiments), функция записывает результаты нормировки в выходные файлы .txt и .pdf
	cout<<"void PrintCalculationResult has started!"<<"\n";
	OutputFileName=string(output_dir)+"/"+OutputFileName;
	ofstream OutputTextFile((OutputFileName+".txt").c_str());//создаём .txt файл с выходными данными
	cc1->Print((OutputFileName+".pdf[").c_str(),"pdf");//создаём .pdf файл с выходными данными, который сейчас будем наполнять графиками и текстом
	for(unsigned int i=0;i<v.size();i++)//для каждой пары экспериментов во входном векторе v
	{	//cout<< "I started pdf writing for the pair!!!!\n";
		TMultiGraph* mgr=new TMultiGraph();
		SpectroscopicFactorHistogram HistPickup=v[i].Pickup.BuildSpectroscopicFactorHistogram(v[i].n_m);//создаём гистограммы нормированных спектроскопических факторов, которые будем отрисовывать на холсте
		SpectroscopicFactorHistogram HistStrip=v[i].Stripping.BuildSpectroscopicFactorHistogram(v[i].n_p);//для срыва и подхвата, всего 2 гистограммы, итого 2 гистограммы на холсте
		cc1->Clear();//Delete all pad primitives
		cc1->Divide(3,2);//разделить Pad на 3 независимые области по вертикали и на 2 по горизонтали (всего 6 областей)
		//cout<< "I divided pads!!!!\n";
		
		cc1->cd(1);//переходим к Pad1
		HistPickup.PrintSpectroscopicFactorHistogram();//рисуем гистограмму для эксперимента подхвата
		
		cc1->cd(2);//переходим к Pad2
		v[i].occupancies.SetTitle("Occupancy;E,keV;v^2");
		//mgr->SetTitle("Occupancy;E, keV;v^{2}");
		mgr->Add(&v[i].Pickup_occupancies);
		mgr->Add(&v[i].Stripping_occupancies);
		mgr->Add(&v[i].Both_occupancies);
		mgr->Draw("ap");
		v[i].occupancies.Draw("p same");//отрисуем заселённости, которые использовались в фите БКШ поверх остальных (выделим их крестами)
		v[i].BCS.Draw("l same");//отрисуем кривую фита БКШ на том же Pad
		
		cc1->cd(3);
		TH1F PenaltyComponents=v[i].BuildPenaltyComponentsHistogram();
		gPad->SetLogy(1);
		PenaltyComponents.Draw();
		
		cc1->cd(4);//переходим к Pad4
		//gPad->SetLogy(1);
		HistStrip.PrintSpectroscopicFactorHistogram();//рисуем гистограмму для эксперимента срыва
		string TextOutput=v[i].ResultsInTextForm(1);
		stringstream s(TextOutput);
		OutputTextFile<<TextOutput<<"\n";//записывем в текстовый файл результаты расчёта
		
		cc1->cd(5);//переходим к Pad5
		TGraph* gr=new TGraph();//"h1","Calculated shell scheme;1 ;E, keV",10,0,1);
		gr->SetTitle("Calculated shell scheme;  ;E, keV");
		gr->SetPoint(0,0,0);
		gr->SetMinimum(GetMinimum(v[i].SPE)-1000);
		gr->SetMaximum(GetMaximum(v[i].SPE)+1000);
		gr->Draw("ap");
		for(unsigned int j=0;j<v[i].SPE.size();j++)
		{
			TLine line;
			TText txt;
			int color=v[i].SP_centroids[j].GetColor();
			line.SetLineColor(color);
			if(v[i].SP_centroids[j].GetToBeDrawnFlag()==1)
			{
				line.DrawLine(0.1,v[i].SPE[j],0.7,v[i].SPE[j]);
				txt.SetTextColor(color);
				txt.DrawText(0.8,v[i].SPE[j], NLJToString(v[i].SP_centroids[j].n,v[i].SP_centroids[j].l,v[i].SP_centroids[j].JP).c_str());
			}
		}
		cc1->cd(6);//переходим к Pad6
		v[i].DrawResultsInTextForm(v[i].ResultsInTextForm());
		cc1->Print((OutputFileName+".pdf").c_str(),"pdf");
		gPad->Update();
	}
	cc1->Print((OutputFileName+".pdf]").c_str(),"pdf");
	cout<<"void PrintCalculationResult has ended!"<<"\n";
}
/*
void PrintFitCalculationResult(vector<CoupleOfExperiments> v, vector<NormalisedCoupleOfExperiments> v_norm, string OutputFileName, string output_dir="output")//функция записывает результаты нормировки в выходные файлы .txt и .pdf
{//на вход подаётся вектор пар экспериментов (вектор объектов CoupleOfExperiments)
	//и название выходных файлов .pdf .txt OutputFileName
	cout<<"void PrintFitCalculationResult has started!"<<endl;
	OutputFileName=string(output_dir)+"/"+OutputFileName;
	cout<<"void PrintFitCalculationResult will save results in "<<OutputFileName<<endl;
	ofstream OutputTextFile((OutputFileName+".txt").c_str());//создаём .txt файл с выходными данными
	cc2->Print((OutputFileName+".pdf[").c_str(),"pdf");//создаём .pdf файл с выходными данными, который сейчас будем наполнять графиками и текстом
	for(unsigned int i=0;i<v.size();i++)//для каждой пары экспериментов во входном векторе v
	{
		cout<<"working with "<<i<<" pair!"<<endl;
		SpectroscopicFactorHistogram HistPickup=v[i].Pickup.BuildSpectroscopicFactorHistogram();//создаём гистограммы спектроскопических факторов, которые будем отрисовывать на холсте
		SpectroscopicFactorHistogram HistStrip=v[i].Stripping.BuildSpectroscopicFactorHistogram();//для срыва и подхвата, всего 2 гистограммы
		SpectroscopicFactorHistogram HistNormPickup=v_norm[i].Pickup.BuildSpectroscopicFactorHistogram(v_norm[i].n_m);//создаём гистограммы нормированных спектроскопических факторов, которые будем отрисовывать на холсте
		SpectroscopicFactorHistogram HistNormStrip=v_norm[i].Stripping.BuildSpectroscopicFactorHistogram(v_norm[i].n_p);//для срыва и подхвата, всего 2 гистограммы, итого 4 гистограммы на холсте
		
		cc2->Clear();//Delete all pad primitives
		cc2->Divide(3,2);//разделить Pad на 3 независимые области по вертикали и на 2 по горизонтали (всего 6 областей)
		cout<<"Going to Pad1!"<<endl;
		cc2->cd(1);//переходим к Pad1
		HistPickup.PrintSpectroscopicFactorHistogram();//рисуем гистограмму для эксперимента подхвата
		cout<<"Going to Pad2!"<<endl;
		cc2->cd(2);//переходим к Pad2
		HistNormPickup.PrintSpectroscopicFactorHistogram();//рисуем гистограмму для эксперимента нормированного подхвата
		cout<<"Going to Pad3!"<<endl;
		cc2->cd(3);//переходим к Pad3	
		v[i].points_G.SetTitle("Fit graph;G^-/2j+1;G^+/2j+1");//здесь и далее рисуем график фита; устанавливаем заголовок сверху графика
		v[i].points_G.SetMarkerStyle(28);//устанавливаем стиль маркеров фитируемых точек
		//v[i].points_G.ComputeRange(0,0,GetMaximum(v[i].Gm_alt_c)+0.1,GetMaximum(v[i].Gp_alt_c)+0.1);//изменим границы оси
		//v[i].points_G.SetMarkerColor();//было бы неплохо сделать цвет каждой точки соответвующим цвету подоболочки на гистограммах
		v[i].points_G.GetXaxis()->SetLimits(0.,1.5);
		v[i].points_G.Draw("AP");	
		v[i].points_G.SetMinimum(0.);
		v[i].points_G.SetMaximum(1.5);
		
		v_norm[i].points_G.SetMarkerColor(kBlue);//меняем цвет маркеров на синий, чтобы выделялись
		v_norm[i].points_G.SetMarkerStyle(29);
		v_norm[i].points_G.Draw("AP SAME");
		
		//v[i].FIT.SetRange(0,0,GetMaximum(v[i].Gm_alt_c)+1,GetMaximum(v[i].Gp_alt_c)+1);	
		v_norm[i].FIT.Draw("l same");//"SAME" - superimpose on top of existing picture, "L" - connect all computed points with a straight line  
		///для построения второй линии фита:
		v_norm[i].FIT2.Draw("l same");
		///для построения третей линии фита:
		v_norm[i].FIT3.SetLineColor(1);
		v_norm[i].FIT3.Draw("l same");
		gPad->BuildLegend();
		cout<<"Going to Pad4!"<<endl;
		cc2->cd(4);//переходим к Pad4
		HistStrip.PrintSpectroscopicFactorHistogram();//рисуем гистограмму для эксперимента срыва
		
		string TextOutput=v_norm[i].FitResultsInTextForm(1);
		stringstream s(TextOutput);
		OutputTextFile<<TextOutput<<"\n";//записывем в текстовый файл результаты расчёта
		cout<<"Going to Pad5!"<<endl;
		cc2->cd(5);//переходим к Pad5
		HistNormStrip.PrintSpectroscopicFactorHistogram();//рисуем гистограмму для нормированного эксперимента срыва
		cout<<"Going to Pad6!"<<endl;
		cc2->cd(6);//переходим к Pad6
		v_norm[i].DrawResultsInTextForm(TextOutput);//выводим текст справа внизу
		cout<<"Printing in pdf file!"<<endl;
		cc2->Print((OutputFileName+".pdf").c_str(),"pdf");//сохраняем всё в .pdf файл
		cout<<"gPad->Update()!"<<endl;
		gPad->Update();//обновим текущую область построения
		cc2->Clear();
	}
	cc2->Print((OutputFileName+".pdf]").c_str(),"pdf");//сохраняем всё в .pdf файл (в третий раз?)
}
*/
void PrintFitCalculationResult(vector<NormalisedCoupleOfExperiments> v_norm, string OutputFileName, string output_dir="output")//функция записывает результаты нормировки в выходные файлы .txt и .pdf
{//на вход подаётся вектор пар экспериментов (вектор объектов CoupleOfExperiments)
	//и название выходных файлов .pdf .txt OutputFileName
	cout<<"void PrintFitCalculationResult has started!"<<endl;
	OutputFileName=string(output_dir)+"/"+OutputFileName;
	cout<<"void PrintFitCalculationResult will save results in "<<OutputFileName<<endl;
	ofstream OutputTextFile((OutputFileName+".txt").c_str());//создаём .txt файл с выходными данными
	cc2->Print((OutputFileName+".pdf[").c_str(),"pdf");//создаём .pdf файл с выходными данными, который сейчас будем наполнять графиками и текстом
	for(unsigned int i=0;i<v_norm.size();i++)//для каждой пары экспериментов во входном векторе v
	{
		cout<<"working with "<<i<<" pair!"<<endl;
		SpectroscopicFactorHistogram HistPickup=v_norm[i].Pickup.BuildSpectroscopicFactorHistogram(1);//создаём гистограммы спектроскопических факторов, которые будем отрисовывать на холсте
		SpectroscopicFactorHistogram HistStrip=v_norm[i].Stripping.BuildSpectroscopicFactorHistogram(1);//для срыва и подхвата, всего 2 гистограммы
		SpectroscopicFactorHistogram HistNormPickup=v_norm[i].Pickup.BuildSpectroscopicFactorHistogram(v_norm[i].n_m);//создаём гистограммы нормированных спектроскопических факторов, которые будем отрисовывать на холсте
		SpectroscopicFactorHistogram HistNormStrip=v_norm[i].Stripping.BuildSpectroscopicFactorHistogram(v_norm[i].n_p);//для срыва и подхвата, всего 2 гистограммы, итого 4 гистограммы на холсте
		
		cc2->Clear();//Delete all pad primitives
		cc2->Divide(3,2);//разделить Pad на 3 независимые области по вертикали и на 2 по горизонтали (всего 6 областей)
		cout<<"Going to Pad1!"<<endl;
		cc2->cd(1);//переходим к Pad1
		HistPickup.PrintSpectroscopicFactorHistogram();//рисуем гистограмму для эксперимента подхвата
		cout<<"Going to Pad2!"<<endl;
		cc2->cd(2);//переходим к Pad2
		HistNormPickup.PrintSpectroscopicFactorHistogram();//рисуем гистограмму для эксперимента нормированного подхвата
		cout<<"Going to Pad3!"<<endl;
		cc2->cd(3);//переходим к Pad3
		TGraphErrors gr_before_norm=TGraphErrors();
		for(unsigned int j=0;j<v_norm[i].points_G.GetN();j++)//для каждой пары срыв-подхват в векторе CE
		{	
			gr_before_norm.SetPoint(j,v_norm[i].points_G.GetPointX(j)/
			v_norm[i].n_m,v_norm[i].points_G.GetPointY(j)/v_norm[i].n_p);
			gr_before_norm.SetPointError(j,v_norm[i].points_G.GetErrorX(j)/
			v_norm[i].n_m,v_norm[i].points_G.GetErrorY(j)/v_norm[i].n_p);
		}	
		gr_before_norm.SetTitle("Fit graph;G^-/2j+1;G^+/2j+1");//здесь и далее рисуем график фита; устанавливаем заголовок сверху графика
		gr_before_norm.SetMarkerStyle(28);//устанавливаем стиль маркеров фитируемых точек
		//gr_before_norm.ComputeRange(0,0,GetMaximum(v[i].Gm_alt_c)+0.1,GetMaximum(v[i].Gp_alt_c)+0.1);//изменим границы оси
		//gr_before_norm.SetMarkerColor();//было бы неплохо сделать цвет каждой точки соответвующим цвету подоболочки на гистограммах
		gr_before_norm.GetXaxis()->SetLimits(0.,1.5);
		gr_before_norm.Draw("AP");	
		gr_before_norm.SetMinimum(0.);
		gr_before_norm.SetMaximum(1.5);
		
		v_norm[i].points_G.SetMarkerColor(kBlue);//меняем цвет маркеров на синий, чтобы выделялись
		v_norm[i].points_G.SetMarkerStyle(29);
		v_norm[i].points_G.Draw("AP SAME");
		
		//v[i].FIT.SetRange(0,0,GetMaximum(v[i].Gm_alt_c)+1,GetMaximum(v[i].Gp_alt_c)+1);	
		v_norm[i].FIT.Draw("l same");//"SAME" - superimpose on top of existing picture, "L" - connect all computed points with a straight line  
		///для построения второй линии фита:
		v_norm[i].FIT2.Draw("l same");
		///для построения третей линии фита:
		v_norm[i].FIT3.SetLineColor(1);
		v_norm[i].FIT3.Draw("l same");
		gPad->BuildLegend();
		cout<<"Going to Pad4!"<<endl;
		cc2->cd(4);//переходим к Pad4
		HistStrip.PrintSpectroscopicFactorHistogram();//рисуем гистограмму для эксперимента срыва
		
		string TextOutput=v_norm[i].FitResultsInTextForm(1);
		stringstream s(TextOutput);
		OutputTextFile<<TextOutput<<"\n";//записывем в текстовый файл результаты расчёта
		cout<<"Going to Pad5!"<<endl;
		cc2->cd(5);//переходим к Pad5
		HistNormStrip.PrintSpectroscopicFactorHistogram();//рисуем гистограмму для нормированного эксперимента срыва
		cout<<"Going to Pad6!"<<endl;
		cc2->cd(6);//переходим к Pad6
		TextOutput=v_norm[i].FitResultsInTextForm(0);
		v_norm[i].DrawResultsInTextForm(TextOutput);//выводим текст справа внизу
		cout<<"Printing in pdf file!"<<endl;
		cc2->Print((OutputFileName+".pdf").c_str(),"pdf");//сохраняем всё в .pdf файл
		cout<<"gPad->Update()!"<<endl;
		gPad->Update();//обновим текущую область построения
		cc2->Clear();
	}
	cc2->Print((OutputFileName+".pdf]").c_str(),"pdf");//сохраняем всё в .pdf файл (в третий раз?)
}

///перебор по возрастанию штрафной функции не изменялся, хотя мог быть изменён для ранжирования по успешности нормировки СС:
void ArrangeByPenalty(vector<CoupleOfExperiments> &v)//функция меняяет соседние в векторе пары экспериментов, до тех пор, пока они не будут отранжированы по возрастанию функции ошибок
{//функция сортирует пары экспериментов в поданном векторе по убыванию соответсвующей штрафной функции//двойной перебор нужен чтобы сравнить каждый элемент вектора с каждым, происходит сортировка по значению штрафной функции
	for(unsigned int i=0;i<v.size();i++)//для каждого элемента i массива
	{
		int NumberOfExcanges=0;//переменная для числа перестановок
		for(unsigned int j=0;j<v.size()-i-1;j++)//для каждого элемента массива j, который имеет номер в векторе меньше i
		{
			if(v[j].penalty>v[j+1].penalty)//если функция ошибок пары экспериментов j больше, чем у пары j+1
			{//стандартные действия для смены мест двух соседних эелементов массива/вектора
				CoupleOfExperiments tmp=v[j];//создаём буферную пару для перемещения пар j и j+1, равную паре j, чтобы её сохранить
				v[j]=v[j+1];//сохраняем в j пару j+1
				v[j+1]=tmp;//сохраняем в j+1 буфер, который равен паре j
				NumberOfExcanges++;//считаем число перестановок, увеличивая каждый раз NumberOfExcanges на 1
			}
		}
		if(NumberOfExcanges==0)//если не было совершенно перестановок, NumberOfExcanges осталось равной 0
		{
			return;//то функция просто ничего не делает
		}
	}
}//конец void ArrangeByPenalty

void ArrangeByPenalty(vector<NormalisedCoupleOfExperiments> &v)//функция меняяет соседние в векторе пары экспериментов, до тех пор, пока они не будут отранжированы по возрастанию функции ошибок
{//функция сортирует пары экспериментов в поданном векторе по убыванию соответсвующей штрафной функции//двойной перебор нужен чтобы сравнить каждый элемент вектора с каждым, происходит сортировка по значению штрафной функции
	for(unsigned int i=0;i<v.size();i++)//для каждого элемента i массива
	{
		int NumberOfExcanges=0;//переменная для числа перестановок
		for(unsigned int j=0;j<v.size()-i-1;j++)//для каждого элемента массива j, который имеет номер в векторе меньше i
		{
			if(v[j].penalty>v[j+1].penalty)//если функция ошибок пары экспериментов j больше, чем у пары j+1
			{//стандартные действия для смены мест двух соседних эелементов массива/вектора
				NormalisedCoupleOfExperiments tmp=v[j];//создаём буферную пару для перемещения пар j и j+1, равную паре j, чтобы её сохранить
				v[j]=v[j+1];//сохраняем в j пару j+1
				v[j+1]=tmp;//сохраняем в j+1 буфер, который равен паре j
				NumberOfExcanges++;//считаем число перестановок, увеличивая каждый раз NumberOfExcanges на 1
			}
		}
		if(NumberOfExcanges==0)//если не было совершенно перестановок, NumberOfExcanges осталось равной 0
		{
			return;//то функция просто ничего не делает
		}
	}
}//конец void ArrangeByPenalty

void SNTRA(string PathToFiles, string particle="", int ListFilesFlag=0, string output_dir_path="output")
{cout<<"void SNTRA has started!"<<"\n";
	vector<Experiment> Pickup;//создаём вектор всех экспериментов подхвата
	vector<Experiment> Stripping;//создаём вектор всех экспериментов срыва
	
	ReadFilesInDirectory(PathToFiles,Pickup,Stripping,particle,ListFilesFlag);//считаем поготовленные файлы данных с диска
	cout<<"vector<Experiment> Pickup.size() = "<<Pickup.size()<<"\n";
	cout<<"vector<Experiment> Stripping.size() = "<<Stripping.size()<<"\n";
	cout<<"Creating parameters object!"<<"\n";
	parameters par=parameters();
	cout<<"Creating stringstream object!"<<"\n";
	stringstream s1(PathToFiles);
	string ParFileName;
	s1>>ParFileName;
	
	par.ReadParameters(ParFileName+"parameters.par");//считаем пользовательские параметры из файла parameters.par на диске
	par.Cout();//выведем считанные параметры в терминал
	
	//vector<CoupleOfExperiments> CE=CreateCouplesOfExperiments(Pickup,Stripping,par);
	cout<<"Creating parameters vector<NormalisedCoupleOfExperiments> CE_norm!"<<"\n";
	vector<NormalisedCoupleOfExperiments> CE_norm=CreateNormalisedCouplesOfExperiments(Pickup,Stripping,par);
	/*for(unsigned int i=0;i<CE.size();i++)//для каждой пары срыв-подхват в векторе CE
	{
		CE[i].CalcSPE_and_OCC();//применяем метод для расчёта одночастичной энергии и нормированной заселённости на подоболочке для пары экпериментов 
	}*/
	cout<<"Use CalcSPE_and_OCC() for every element in vector<NormalisedCoupleOfExperiments>!"<<"\n";
	for(unsigned int i=0;i<CE_norm.size();i++)//для каждой пары срыв-подхват в векторе CE
	{	
		cout<<"Starting CE_norm["<<i<<"].CalcSPE_and_OCC()!"<<"\n";
		CE_norm[i].CalcSPE_and_OCC();
	}
	string OutputFileName;//создаём строку с именем выходного файла для результата расчёта SNTRA до нормировки
	string OutputFileName2;//создаём строку с именем выходного файла для результата нормировки
	string OutputFileName3;//создаём строку с именем второго выходного файла для результата нормировки
	if((Pickup.size()>0)&&(Stripping.size()>0))//если есть хоть 1 эксперимент срыва и хоть 1 эксперимент подхвата, то
	{//задаём имя выходного pdf файла в строку OutputFileName
		OutputFileName=Pickup[0].Nucleus+"_"+Pickup[0].particle;//OutputFileName равен название ядра _ налетающий нуклон
		OutputFileName2=OutputFileName+"_norm";//OutputFileName2 равен название ядра _ налетающий нуклон_norm
		OutputFileName3=OutputFileName+"_norm2";//OutputFileName3 равен название ядра _ налетающий нуклон_norm2
	}//получаем для название типа "32S_neutron" для pdf и txt файлов
	else//иначе
	{
		return ;//заканчиваем нашу функцию SNTRA здесь
	}
	cout<<"Use CalculatePenaltyFunction(CE_norm)!"<<"\n";
	CalculatePenaltyFunction(CE_norm);//применяем функцию для вычисления штрафной функции
	ArrangeByPenalty(CE_norm);//применяем функцию для сортировки нашего вывода по возрастанию значения штрафной функции
	PrintCalculationResult(CE_norm,OutputFileName,output_dir_path);//записывает результат ранжировки для пары экспериментов CE в выходные файлы .txt и .pdf
	
	for(unsigned int i=0;i<CE_norm.size();i++)//для каждой пары срыв-подхват в векторе CE
	{	
		CE_norm[i].InduceNormalisation();
		CE_norm[i].ReCalcSPE_and_OCC();
	}
	PrintFitCalculationResult(CE_norm,OutputFileName2,output_dir_path);//записывает результат нормировки для пары экспериментов CE в выходные файлы .txt и .pdf

	CalculatePenaltyFunction(CE_norm);//применяем функцию для вычисления штрафной функции
	ArrangeByPenalty(CE_norm);//применяем функцию для сортировки нашего вывода по возрастанию значения штрафной функции
	PrintCalculationResult(CE_norm,OutputFileName3,output_dir_path);//записывает результат воздействия нормировки для пары экспериментов CE в выходные файлы .txt и .pdf

	cout<<"void SNTRA has ended!"<<"\n";
}//конец функции void SNTRA

int main(int argc, char** argv)//главная функция, принимает аргументы из терминала при вызове SNTRA пользователем
{//argc (argument count) и argv (argument vector) - число переданных строк в main через argv и массив переданных в main строк
	gStyle->SetLabelSize(0.05,"xyz");
	gStyle->SetTextSize(0.05);
	gStyle->SetLegendTextSize(0.06);
	gStyle->SetTitleSize(0.03,"xyz");//*/
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
