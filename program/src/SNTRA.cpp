#include <stdio.h>
#include "InputData.hh"
//#include "VectorLib.cpp"
#include "Experiment.cpp"
//#include <vector>
//#include <cstring>
#include <sstream>
#include <iostream>
#include <fstream>
//#include "ExtStrings.cpp"
#include "PrimitiveShellModel.cpp"
//root-зависимые библиотеки:
#include <TGraph.h>
#include <TFile.h>
#include <TSystem.h>
#include <TSystem.h>
#include <TSystemDirectory.h>
#include <TSystemFile.h>
#include <TLine.h>
#include <TMultiGraph.h>

using namespace std;

TCanvas *cc1=new TCanvas("cc1","cc1");//TCanvas - класс cern_root (холст, где всё отрисовывается), первый - для результатов расчёта без нормировки
TCanvas *cc2=new TCanvas("cc2","cc2");//сделаем второй холст для результатов нормировки
TCanvas *cc3=new TCanvas("cc3","cc3");//сделаем третий холст для результатов применения нормировки

vector<string> ListFiles(string mask)
{cout<<"vector<string> ListFiles(string mask) has started!"<<"\n";
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
		//if((GetFileName(s)[0]>='.')&&(GetFileName(s)[0]<='z'))
		FileNames.push_back(s);
		//cout<<s<<"\n";
	}
	return FileNames;
	cout<<"vector<string> ListFiles(string mask) has ended!"<<"\n";
}

vector<string> ListFiles(string dirname, string ext) //функция ... , выводит названия входных файлов в терминал
{cout<<"vector<string> ListFiles("<<dirname<<", "<<ext<<")  has started!"<<"\n";
	TSystemDirectory dir(dirname.c_str(), dirname.c_str()); 
	TList *files = dir.GetListOfFiles(); 
	vector<string> result;
	if(files) 
	{
		TSystemFile *file; 
		TString fname; 
		TIter next(files);
		while((file=(TSystemFile*)next()))
		{
			fname = file->GetName(); 
			if(!file->IsDirectory() && fname.EndsWith(ext.c_str()))
			{ 
				result.push_back(dirname+(string)fname); 
				cout<<(string)fname<<"\n";
			} 
		} 
	}
	else
	{
		cout<<" *** Error! ListFiles(): TList *files = dir.GetListOfFiles() returned false!"<<endl;
	}
	cout<<"vector<string> ListFiles(string dirname, string ext) has ended!"<<"\n";
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
		FileNames=ListFiles(path,ext); 
	}
	////cout<<"size="<<FileNames.size()<<"\n";
	for(unsigned int i=0;i<FileNames.size();i++)
	{
		Experiment E;
		E.ReadInputFile(FileNames[i]);
		//cout<<E.GetType()<<"\n";
		int Z,A,ParticleType;//ParticleType-передача нейтрона (0) или протона(1)
		string type;//pickup/stripping
		GetAZ(E.Nucleus,Z,A);
		//cout<<E.Nucleus<<" "<<Z<<" "<<A<<"\n";
		ParceReaction(E.reaction,type,ParticleType);
				
		if(ParticleType==1)
		{///уже здесь энергии считываются неправильно:!!!
			E.BA1=GetSeparationEnergy(Z+1, A+1, 1,1)*1000;
			//cout<<"SeparationEnergy(Z+1, A+1, 1,1)="<<E.BA1<<"\n";
			E.BA=GetSeparationEnergy(Z, A, 1,1)*1000;
			//cout<<"SeparationEnergy(Z, A, 1,1="<<E.BA<<"\n";
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
		if(NumberOfPickupStatesMax<v[i].Pickup.size())//GetNlevels())
		{
			//NumberOfPickupStatesMax=v[i].Pickup.GetNlevels();
			NumberOfPickupStatesMax=v[i].Pickup.size();
		}
		if(NumberOfStrippingStatesMax<v[i].Stripping.size())//GetNlevels())
		{
			NumberOfStrippingStatesMax=v[i].Stripping.size();//)//GetNlevels();
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
///кусок
void CalculatePenaltyFunction_norm(vector<CoupleOfExperiments> &v)//функция для расчёта штрафной функции
{cout<<"void CalculatePenaltyFunction_norm has started!"<<"\n";
	float MaxEfError,MaxDeltaError;
	int NumberOfPickupStatesMax=0, NumberOfStrippingStatesMax=0, AverageNumberOfCalculatedStates=0;

	for(int i=0;i<v.size();i++)
	{	
		v[i].PenaltyComponents.resize(0);
		if(MaxEfError<v[i].Ef_error_norm)
		{
			MaxEfError=v[i].Ef_error_norm;
		}
		if(MaxDeltaError<v[i].Delta_error_norm)
		{
			MaxDeltaError=v[i].Delta_error_norm;
		}
		if(NumberOfPickupStatesMax<v[i].Pickup.size())//GetNlevels())
		{
			NumberOfPickupStatesMax=v[i].Pickup.size();
		}
		if(NumberOfStrippingStatesMax<v[i].Stripping.size())//GetNlevels())
		{
			NumberOfStrippingStatesMax=v[i].Stripping.size();//)//GetNlevels();
		}
		AverageNumberOfCalculatedStates+=v[i].SPE.size();
	}

	if(v.size()>0) AverageNumberOfCalculatedStates = round(AverageNumberOfCalculatedStates/v.size());
	else return;
	
	for(unsigned int i=0;i<v.size();i++)
	{
		//
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
					v[i].PenaltyComponents.push_back(v[i].Ef_error_norm/MaxEfError);
				}
			}
			else if(v[i].par.UsedPenaltyFunctionComponents[j]==5)
			{
				if((MaxEfError!=0)&&(MaxDeltaError!=0))
				{
					v[i].PenaltyComponents.push_back(v[i].Delta_error_norm/MaxDeltaError);
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
	
}//конец void CalculatePenaltyFunction_norm
///конец куска
void PrintCalculationResult(vector<CoupleOfExperiments> v, string OutputFileName)
{//на вход подаётся вектор пар экспериментов (вектор объектов CoupleOfExperiments) и название выходных файлов .pdf .txt OutputFileName
	cout<<"void PrintCalculationResult has started!"<<"\n";
	ofstream OutputTextFile((OutputFileName+".txt").c_str());
	cc1->Print((OutputFileName+".pdf[").c_str(),"pdf");
	for(unsigned int i=0;i<v.size();i++)//для каждой пары экспериментов во входном векторе v
	{
		SpectroscopicFactorHistogram HistPickup=v[i].Pickup.BuildSpectroscopicFactorHistogram();
		SpectroscopicFactorHistogram HistStrip=v[i].Stripping.BuildSpectroscopicFactorHistogram();
		cc1->Clear();//Delete all pad primitives
		cc1->Divide(3,2);//разделить Pad на 3 независимые области по вертикали и на 2 по горизонтали (всего 6 областей)
		
		cc1->cd(1);//переходим к Pad1
		//gPad->SetLogy(1);
		HistPickup.PrintSpectroscopicFactorHistogram();//рисуем гистограмму для эксперимента подхвата
		
		cc1->cd(2);//переходим к Pad2
		TMultiGraph mgr;
		v[i].occupancies.SetTitle("Occupancy;E,keV;v^2");
		mgr.SetTitle("Occupancy;E, keV;v^{2}");
		mgr.Add(&v[i].Pickup_occupancies);
		mgr.Add(&v[i].Stripping_occupancies);
		mgr.Add(&v[i].Both_occupancies);
		mgr.Draw("ap");
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
		///
		//v[i].DrawResultsInTextForm());
		v[i].DrawResultsInTextForm(v[i].ResultsInTextForm());
		///
		cc1->Print((OutputFileName+".pdf").c_str(),"pdf");
		gPad->Update();
		
	}
	cc1->Print((OutputFileName+".pdf]").c_str(),"pdf");
	cout<<"void PrintCalculationResult has ended!"<<"\n";
}

///кусок9 кода, добавленного для нормировки СС 
void PrintFitCalculationResult(vector<CoupleOfExperiments> v, string OutputFileName)//функция записывает результаты нормировки в выходные файлы .txt и .pdf
{//на вход подаётся вектор пар экспериментов (вектор объектов CoupleOfExperiments) и название выходных файлов .pdf .txt OutputFileName
	ofstream OutputTextFile((OutputFileName+".txt").c_str());//создаём .txt файл с выходными данными
	cc2->Print((OutputFileName+".pdf[").c_str(),"pdf");//создаём .pdf файл с выходными данными, который сейчас будем наполнять графиками и текстом
	for(unsigned int i=0;i<v.size();i++)//для каждой пары экспериментов во входном векторе v
	{
		SpectroscopicFactorHistogram HistPickup=v[i].Pickup.BuildSpectroscopicFactorHistogram();//создаём гистограммы спектроскопических факторов, которые будем отрисовывать на холсте
		SpectroscopicFactorHistogram HistStrip=v[i].Stripping.BuildSpectroscopicFactorHistogram();//для срыва и подхвата, всего 2 гистограммы
		SpectroscopicFactorHistogram HistNormPickup=v[i].Pickup.BuildNormSpectroscopicFactorHistogram(v[i].n_m);//создаём гистограммы нормированных спектроскопических факторов, которые будем отрисовывать на холсте
		SpectroscopicFactorHistogram HistNormStrip=v[i].Stripping.BuildNormSpectroscopicFactorHistogram(v[i].n_p);//для срыва и подхвата, всего 2 гистограммы, итого 4 гистограммы на холсте
		//FitGraph NormFit=BuildNormFit(v[i]);
		
		cc2->Clear();//Delete all pad primitives
		cc2->Divide(3,2);//разделить Pad на 3 независимые области по вертикали и на 2 по горизонтали (всего 6 областей)
		
		cc2->cd(1);//переходим к Pad1
		HistPickup.PrintSpectroscopicFactorHistogram();//рисуем гистограмму для эксперимента подхвата
		
		cc2->cd(2);//переходим к Pad2
		HistNormPickup.PrintSpectroscopicFactorHistogram();//рисуем гистограмму для эксперимента нормированного подхвата
		
		cc2->cd(3);//переходим к Pad3	
		v[i].points_G.SetTitle("Fit graph;G^-/2j+1;G^+/2j+1");//здесь и далее рисуем график фита; устанавливаем заголовок сверху графика
		v[i].points_G.SetMarkerStyle(28);//устанавливаем стиль маркеров фитируемых точек
		//v[i].points_G.ComputeRange(0,0,GetMaximum(v[i].Gm_alt_c)+0.1,GetMaximum(v[i].Gp_alt_c)+0.1);//изменим границы оси
		//v[i].points_G.SetMarkerColor();//было бы неплохо сделать цвет каждой точки соответвующим цвету подоболочки на гистограммах
		v[i].points_G.GetXaxis()->SetLimits(0.,1.5);
		v[i].points_G.Draw("AP");	
		v[i].points_G.SetMinimum(0.);
		v[i].points_G.SetMaximum(1.5);
		
		v[i].points_G_norm.SetMarkerColor(kBlue);//меняем цвет маркеров на синий, чтобы выделялись
		v[i].points_G_norm.SetMarkerStyle(29);
		v[i].points_G_norm.Draw("P");
		
		//v[i].FIT.SetRange(0,0,GetMaximum(v[i].Gm_alt_c)+1,GetMaximum(v[i].Gp_alt_c)+1);	
		v[i].FIT.Draw("l same");//"SAME" - superimpose on top of existing picture, "L" - connect all computed points with a straight line  
		///для построения второй линии фита:
		v[i].FIT2.Draw("l same");
		///для построения третей линии фита:
		v[i].FIT3.SetLineColor(1);
		v[i].FIT3.Draw("l same");
		
		cc2->cd(4);//переходим к Pad4
		HistStrip.PrintSpectroscopicFactorHistogram();//рисуем гистограмму для эксперимента срыва
		
		string TextOutput=v[i].FitResultsInTextForm(1);
		stringstream s(TextOutput);
		OutputTextFile<<TextOutput<<"\n";//записывем в текстовый файл результаты расчёта
		
		cc2->cd(5);//переходим к Pad5
		HistNormStrip.PrintSpectroscopicFactorHistogram();//рисуем гистограмму для нормированного эксперимента срыва
		
		cc2->cd(6);//переходим к Pad6
		v[i].DrawResultsInTextForm(v[i].FitResultsInTextForm());//выводим текст справа внизу
		
		cc2->Print((OutputFileName+".pdf").c_str(),"pdf");//сохраняем всё в .pdf файл
		gPad->Update();//обновим текущую область построения
		
	}
	cc2->Print((OutputFileName+".pdf]").c_str(),"pdf");//сохраняем всё в .pdf файл (в третий раз?)
}

void PrintFitCalculationResult2(vector<CoupleOfExperiments> v, string OutputFileName)//функция записывает результаты нормировки в выходные файлы .txt и .pdf
{//на вход подаётся вектор пар экспериментов (вектор объектов CoupleOfExperiments) и название выходных файлов .pdf .txt OutputFileName
	ofstream OutputTextFile((OutputFileName+".txt").c_str());//создаём .txt файл с выходными данными
	cc3->Print((OutputFileName+".pdf[").c_str(),"pdf");//создаём .pdf файл с выходными данными, который сейчас будем наполнять графиками и текстом
	for(unsigned int i=0;i<v.size();i++)//для каждой пары экспериментов во входном векторе v
	{	//cout<< "I started pdf writing for the pair!!!!\n";
		SpectroscopicFactorHistogram HistNormPickup=v[i].Pickup.BuildNormSpectroscopicFactorHistogram(v[i].n_m);//создаём гистограммы нормированных спектроскопических факторов, которые будем отрисовывать на холсте
		SpectroscopicFactorHistogram HistNormStrip=v[i].Stripping.BuildNormSpectroscopicFactorHistogram(v[i].n_p);//для срыва и подхвата, всего 2 гистограммы, итого 2 гистограммы на холсте
		cc3->Clear();//Delete all pad primitives
		cc3->Divide(3,2);//разделить Pad на 3 независимые области по вертикали и на 2 по горизонтали (всего 6 областей)
		//cout<< "I divided pads!!!!\n";
		
		cc3->cd(1);//переходим к Pad1
		//cout<< "I moved to Pad1!!!!\n";
		HistNormPickup.PrintSpectroscopicFactorHistogram();//рисуем гистограмму для эксперимента нормированного подхвата
		//cout<< "I draw first histogram (Pad1)!!!!\n";
		
		cc3->cd(2);//переходим к Pad2
		TMultiGraph mgr;
		v[i].occupancies_norm.SetTitle("Occupancy;E,keV;v^2");
		mgr.Add(&v[i].Pickup_norm_occupancies);
		mgr.Add(&v[i].Stripping_norm_occupancies);
		mgr.Add(&v[i].Both_norm_occupancies);
		mgr.Draw("ap");
		v[i].occupancies_norm.Draw("p");
		v[i].BCS_norm.Draw("l same");
		/*v[i].occupancies_norm.SetTitle("Occupancy_norm;E,keV;v^2");//нарисуем график заселённости после нормировки
		//cout<< "I set occupancies_norm title (Pad2)!!!!\n";
		v[i].occupancies_norm.Draw("ap");
		//cout<< "I draw occupancies_norm (Pad2)!!!!\n";
		v[i].BCS_norm.Draw("l same");//"SAME" - superimpose on top of existing picture, "L" - connect all computed points with a straight line  
		//cout<< "I draw BCS_norm (Pad2)!!!!\n";*/
		
		cc3->cd(3);//переходим к Pad3
		TH1F PenaltyComponents=v[i].BuildPenaltyComponentsHistogram();
		gPad->SetLogy(1);//Set Lin/Log scale for Y, value = 1 Y scale will be logarithmic (base 10)
		PenaltyComponents.Draw();
		
		cc3->cd(4);//переходим к Pad4
		HistNormStrip.PrintSpectroscopicFactorHistogram();//рисуем гистограмму для нормированного эксперимента срыва
		
		string TextOutput=v[i].FitResultsInTextForm2(1);
		stringstream s(TextOutput);
		OutputTextFile<<TextOutput<<"\n";//записывем в текстовый файл результаты расчёта

		cc3->cd(5);//переходим к Pad5
		TGraph* gr=new TGraph();//"h1","Calculated shell scheme;1 ;E, keV",10,0,1);
		gr->SetPoint(0,0,0);
		gr->SetMinimum(GetMinimum(v[i].SPE_norm)-1000);
		gr->SetMaximum(GetMaximum(v[i].SPE_norm)+1000);
		gr->Draw("ap");

		for(unsigned int j=0;j<v[i].SPE.size();j++)
		{
			TLine line;
			TText txt;
			int color=v[i].SP_centroids[j].GetColor();
			line.SetLineColor(color);
			line.DrawLine(0.1,v[i].SPE_norm[j],0.7,v[i].SPE_norm[j]);
			txt.SetTextColor(color);
			txt.DrawText(0.8,v[i].SPE_norm[j], NLJToString(v[i].SP_centroids[j].n,v[i].SP_centroids[j].l,v[i].SP_centroids[j].JP).c_str());
		}

		cc3->cd(6);//переходим к Pad6
		v[i].DrawResultsInTextForm(v[i].FitResultsInTextForm2());
		cc3->Print((OutputFileName+".pdf").c_str(),"pdf");
		
		gPad->Update();
		//cout<< "I updated gPad!!!!\n";
	}
	cc3->Print((OutputFileName+".pdf]").c_str(),"pdf");
	//cout<< "I did final print dunno what it is!!!!\n";
}
///конец кусок9 кода, добавленного для нормировки СС

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

void SNTRA(string PathToFiles, string particle="", int ListFilesFlag=0)
{cout<<"void SNTRA has started!"<<"\n";
	vector<Experiment> Pickup;//создаём вектор всех экспериментов подхвата
	vector<Experiment> Stripping;//создаём вектор всех экспериментов срыва
	
	ReadFilesInDirectory(PathToFiles,Pickup,Stripping,particle,ListFilesFlag);//считаем поготовленные файлы данных с диска
	parameters par;
	stringstream s1(PathToFiles);
	string ParFileName;
	s1>>ParFileName;
	par.ReadParameters(ParFileName+"parameters.par");//считаем пользовательские параметры из файла parameters.par на диске
	par.CoutParameters();//выведем считанные параметры в терминал
	vector<CoupleOfExperiments> CE=CreateCouplesOfExperiments(Pickup,Stripping,par);
	//TCanvas *cc1=new TCanvas("cc1","cc1");
	for(unsigned int i=0;i<CE.size();i++)//для каждой пары срыв-подхват в векторе CE
	{
		CE[i].CalcSPE_and_OCC(cc1, cc3);//применяем метод для расчёта одночастичной энергии и нормированной заселённости на подоболочке для пары экпериментов 
		///кусок10 кода, добавленного для нормировки СС
		//CE[i].solveLinear_mod();//применяем метод для расчёта фита через МНК
		//CE[i].CalcSPE_and_OCC_norm(cc3);//применяем метод для расчёта одночастичной энергии и нормированной заселённости на подоболочке для пары экпериментов после нормировки
		///конец кусок10 кода, добавленного для нормировки СС
		//cout<<CE[i].SPE.size()<<" "<<CE[i].EJP.size()<<" "<<CE[i].n.size()<<"\n";
		for(int j=0;j<CE[i].SPE.size();j++)
		{
			//cout<<CE[i].SPE[j]<<" "<<CE[i].n[j]<<" "<<CE[i].EJP[j]<<"\n"; 
		}
		////cout<<CE[i]<<"\n";
	}
	string OutputFileName;//создаём строку с именем выходного файла для результата расчёта SNTRA до нормировки
	///кусок11 кода, добавленного для нормировки СС
	string OutputFileName2;//создаём строку с именем выходного файла для результата нормировки
	string OutputFileName3;//создаём строку с именем второго выходного файла для результата нормировки
	///конец кусок11 кода, добавленного для нормировки СС
	if((Pickup.size()>0)&&(Stripping.size()>0))//если есть хоть 1 эксперимент срыва и хоть 1 эксперимент подхвата, то
	{//задаём имя выходного pdf файла в строку OutputFileName
		OutputFileName=Pickup[0].Nucleus+"_"+Pickup[0].particle;//OutputFileName равен название ядра _ налетающий нуклон
		///кусок12 кода, добавленного для нормировки СС
		OutputFileName2=OutputFileName+"_norm";//OutputFileName2 равен название ядра _ налетающий нуклон_norm
		OutputFileName3=OutputFileName+"_norm2";//OutputFileName3 равен название ядра _ налетающий нуклон_norm2
		///конец кусок12 кода, добавленного для нормировки СС
	}//получаем для название типа "32S_neutron" для pdf и txt файлов
	else//иначе
	{
		return ;//заканчиваем нашу функцию SNTRA_v2 здесь
	}
	CalculatePenaltyFunction(CE);//применяем функцию для вычисления штрафной функции
	ArrangeByPenalty(CE);//применяем функцию для сортировки нашего вывода по возрастанию значения штрафной функции
	PrintCalculationResult(CE,OutputFileName);//записывает результат ранжировки для пары экспериментов CE в выходные файлы .txt и .pdf
	///кусок13 кода, добавленного для нормировки СС
	PrintFitCalculationResult(CE,OutputFileName2);//записывает результат нормировки для пары экспериментов CE в выходные файлы .txt и .pdf
	CalculatePenaltyFunction_norm(CE);//применяем функцию для вычисления штрафной функции
	ArrangeByPenalty(CE);//применяем функцию для сортировки нашего вывода по возрастанию значения штрафной функции
	PrintFitCalculationResult2(CE,OutputFileName3);//записывает результат воздействия нормировки для пары экспериментов CE в выходные файлы .txt и .pdf
	///конец кусок13 кода, добавленного для нормировки СС
	cout<<"void SNTRA has ended!"<<"\n";
}//конец функции void SNTRA

int main(int argc, char** argv)//главная функция, принимает аргументы из терминала при вызове SNTRA пользователем
{//argc (argument count) и argv (argument vector) - число переданных строк в main через argv и массив переданных в main строк
	string path=argv[1];//так как при запуске SNTRA, например: ./SNTRA ../34S_Neutron/ txt, мы передаём ей директори с входными файлами и тип файлов
	string ext=argv[2];//то очевидно мы сохраняем тоже самое в path и ext, соответственно
	cout<<"Got path to input files: "<<path+" "+ext<<"\n";//выводим путь к директории входных файлов и их расширение (.txt) (см. при запуске SNTRA в терминале это первая строка)
	SNTRA(path+" "+ext,"",1);//вызываем нашу функцию SNTRA, передавая ей путь ко входным файлам, их расширение и ListFilesFlag=1
}
