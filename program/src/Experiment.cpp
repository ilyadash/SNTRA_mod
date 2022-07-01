#include "Experiment.hh"
template <typename T>// объявление параметра шаблона функции

string TurnFlagInString(T flag_ch, string opt="CouplesFlag")//функция просто переводит значение флага из unsigned char в string
{ //all - использовать все//pickup - только из pickup//stripping - только из stripping//no - использовать только те состояния, которые есть и в pickup,и в stripping
	vector<string> results={""};
	if (opt=="CouplesFlag") results = {"","all","pickup","stripping","no"};
	if (opt=="PenaltyFunction") results = {"","a_ij","NPickupMax","NStrippingMax","EF_err","Delta_err"};
	cout<<"Input flag="<<flag_ch<<endl;
	unsigned int flag = (unsigned int)flag_ch;
	cout<<"Converted flag="<<flag<<endl;
	if ((flag>=0)&&(flag>=results.size()))
	{
		return results[flag];
	}
	else
	{
		cout<<"TurnFlagInString error: input flag is out of range!"<<endl;
		return "";
	}
}

void SetTGraphLimits(TGraph &gr,float xmin,float xmax,float ymin, float ymax)
{
	gr.Draw("AP");
	gr.GetXaxis()->SetLimits(xmin*0.8,xmax*1.2);
	gr.SetMinimum(ymin*0.8);
	gr.SetMaximum(ymax*1.2);
	gr.SetMarkerStyle(8);
}

void SpectroscopicFactorHistogram::PrintSpectroscopicFactorHistogram()
{
	Legend=new TLegend(0.7,0.7,0.9,0.9);
	for(unsigned int j=0;j<Histograms.size();j++)
	{
		if(j==0)
		{
			Histograms[j].SetTitle((Reference+";E,kev;G").c_str());
			Histograms[j].Draw("histo");
			Legend->AddEntry(&Histograms[j],NLJToString(n[j],L[j], JP[j]).c_str(),"f");
			Histograms[j].GetYaxis()->SetRangeUser(10e-9,maximum*1.5);
		}
		else
		{
			Legend->AddEntry(&Histograms[j],NLJToString(n[j],L[j], JP[j]).c_str(),"f");
			Histograms[j].Draw("histo same");
		}
		Legend->Draw();
	}
}

parameters::parameters()
{
	cout<<"parameters::parameters has started!"<<"\n";
	IncompleteCouplesFlag=1;//all=1, pickup only=2, stripping only=3, no=4//флаг использования пар экпериментов разных типов для расчёта
	LimitedSubShellsUsedInDrawing=0;//флаг отрисовки только выбранных пользователем в параметрах подоболочек
	SubShellsUsedInAllCalculations=AllPrimitiveSubShells;// подоболочки, которые используются во всех вычислениях, а остальные будут игнорироваться (?)
	SubShellsUsedForOccupancyFit=AllPrimitiveSubShells;// подоболочки, которые используются в фите БКШ
	SubShellsUsedInDrawing=AllPrimitiveSubShells;//подоболочки, которые должны отрисовываться на холсте (в энергетическом спектре, в фите БКШ)
	SubShellsUsedForNormalisation=VectorConvertTStringToStateParameters({"2s1/2","1d5/2","1d3/2"});//подоболочки, для которых выписываются и решаются уравнения, нужные для нахождения нормировочных коэффициентов
	UsedPenaltyFunctionComponents={1,2,3,4,5};
}

void parameters::ReadParameters(string filename)
{
	cout<<"parameters::ReadParameters has started!"<<"\n";
	ifstream ifs(filename.c_str());
	string line;//переменная считываемой строки
	while(getline(ifs,line))//для каждой строки в файле на диске
	{
		stringstream s(line);
		string tmp;
		s>>tmp;
		if(tmp=="UseIncompleteCouples:")//чтение критерия отбора пар экспериментов по полноте
		{
			s>>tmp;
			if(tmp=="all")
			{
				IncompleteCouplesFlag=1;
			}
			else if(tmp=="pickup")
			{
				IncompleteCouplesFlag=2;
			}
			else if(tmp=="stripping")
			{
				IncompleteCouplesFlag=3;
			}
			else if(tmp=="no")
			{
				IncompleteCouplesFlag=4;
			}
		}
		else if(tmp=="UsedPenaltyFunctionComponents:")//чтение параметра компонент штрафной функции
		{
			UsedPenaltyFunctionComponents.resize(0);
			string tmp2;
			while(s)
			{
				tmp2.resize(0);
				s>>tmp2;
				cout<<"Found component "<<tmp2<<" of penalty function."<<endl;
				if(tmp2=="a_ij")
				{
					UsedPenaltyFunctionComponents.push_back(1);
				}
				if(tmp2=="NPickupMax")
				{
					UsedPenaltyFunctionComponents.push_back(2);
				}
				if(tmp2=="NStrippingMax")
				{
					UsedPenaltyFunctionComponents.push_back(3);
				}
				if(tmp2=="EF_err")
				{
					UsedPenaltyFunctionComponents.push_back(4);
				}
				if(tmp2=="Delta_err")
				{
					UsedPenaltyFunctionComponents.push_back(5);
				}
			}
			for(unsigned int i=0;i<UsedPenaltyFunctionComponents.size();i++)
			{
				cout<<"Component of penalty function no."<<int(UsedPenaltyFunctionComponents[i])<<" will be used."<<endl;
			}
		}
		else if(tmp=="SubShellsUsedForOccupancyFit:")//чтение параметра подоболочек, используемых в фите БКШ
		{
			SubShellsUsedForOccupancyFit.resize(0);
			string tmp2;
			while(s)
			{
				s>>tmp2;
				int n,l;
				float JP;
				if(StringToNLJ(tmp2,n,l,JP))
				{
					StateParameters sp(n,l,JP,"both");
					if(CheckBelonging(sp,this->SubShellsUsedForOccupancyFit)==0)
					{
						SubShellsUsedForOccupancyFit.push_back(sp);
					}
				}
			}
			for(unsigned int i=0;i<SubShellsUsedForOccupancyFit.size();i++)
			{
				cout<<SubShellsUsedForOccupancyFit[i].GetNLJ()<<" will be used in BCS fit."<<endl;
			}
		}
		else if(tmp=="SubShellsUsedInDrawing:")//чтение параметра подоболочек, используемых в отрисовке на холсте
		{
			SubShellsUsedInDrawing.resize(0);
			LimitedSubShellsUsedInDrawing=1;
			string tmp2;
			while(s)
			{
				s>>tmp2;
				int n,l;
				float JP;
				if(StringToNLJ(tmp2,n,l,JP))
				{
					StateParameters sp(n,l,JP,"both");
					if(CheckBelonging(sp,this->SubShellsUsedInDrawing)==0)
					{
						SubShellsUsedInDrawing.push_back(sp);
					}
				}
			}
			for(unsigned int i=0;i<SubShellsUsedInDrawing.size();i++)
			{
				cout<<SubShellsUsedInDrawing[i].GetNLJ()<<" will be drawn in pdf files."<<endl;
			}
		}
		else if(tmp=="SubShellsUsedForNormalisation:")//чтение параметра подоболочек, используемых в отрисовке на холсте
		{
			SubShellsUsedForNormalisation.resize(0);
			string tmp2;
			while(s)
			{
				s>>tmp2;
				int n,l;
				float JP;
				if(StringToNLJ(tmp2,n,l,JP))
				{
					StateParameters sp(n,l,JP,"both");
					if(CheckBelonging(sp,this->SubShellsUsedForNormalisation)==0)
					{
						SubShellsUsedForNormalisation.push_back(sp);
					}
				}
			}
			for(unsigned int i=0;i<SubShellsUsedForNormalisation.size();i++)
			{
				cout<<SubShellsUsedForNormalisation[i].GetNLJ()<<" will be used in normalisation procedure."<<endl;
			}
		}
	}
}

void parameters::Cout()//метод выводит в терминал считанные в класс параметры расчёта
{
	cout<<"parameters::CoutParameters() has started!"<<"\n";
	cout<<"IncompleteCouplesFlag="<<(int)IncompleteCouplesFlag<<"\n";
	cout<<"LimitedSubShellsUsedInDrawing="<<LimitedSubShellsUsedInDrawing<<"\n";
	cout<<"SubShellsUsedInAllCalculations="<<"\n";
	for(int i=0;i<SubShellsUsedInAllCalculations.size();i++)
	{
		SubShellsUsedInAllCalculations[i].Cout();
	}
	cout<<endl;
	cout<<"SubShellsUsedForOccupancyFit="<<"\n";
	for(int i=0;i<SubShellsUsedForOccupancyFit.size();i++)
	{
		SubShellsUsedForOccupancyFit[i].Cout();
	}
	cout<<endl;
	cout<<"SubShellsUsedInDrawing="<<"\n";
	for(int i=0;i<SubShellsUsedInDrawing.size();i++)
	{
		SubShellsUsedInDrawing[i].Cout();
	}
	cout<<endl;
	cout<<"SubShellsUsedForNormalisation="<<"\n";
	for(int i=0;i<SubShellsUsedForNormalisation.size();i++)
	{
		SubShellsUsedForNormalisation[i].Cout();
	}
	cout<<endl;
	cout<<"PenaltyFunctionComponents: "<<endl;
	for(int i=0;i<UsedPenaltyFunctionComponents.size();i++)
	{
		//cout<<"Component number "<<i<<": "<<TurnFlagInString((int)UsedPenaltyFunctionComponents[i])<<endl;
		cout<<"Component number "<<i<<": "<<(int)UsedPenaltyFunctionComponents[i]<<endl;
	}
}

bool parameters::CheckStateParameters(StateParameters &s)
{
	if((IncompleteCouplesFlag==1)&&(s.couple_flag==1||s.couple_flag==2||s.couple_flag==3))
	{
		return true;
	}
	else if((IncompleteCouplesFlag==2)&&(s.couple_flag==1||s.couple_flag==3))
	{
		return true;
	}
	else if((IncompleteCouplesFlag==3)&&(s.couple_flag==2||s.couple_flag==3))
	{
		return true;
	}
	else if((IncompleteCouplesFlag==4)&&(s.couple_flag==3))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool parameters::CheckBelonging(StateParameters &s, vector<StateParameters> &v)
{
	for(unsigned int i=0;i<v.size();i++)
	{
		if(v[i].CompareQN(s))
		{
			return true;
		}
	}
	return false;
}

void parameters::PrintUsedSubShells()
{
	stringstream s;
	for(unsigned int i=0;i<SubShellsUsedForOccupancyFit.size();i++)
	{
		s<<"s:"<<SubShellsUsedForOccupancyFit[i].n<<"("<<SubShellsUsedForOccupancyFit[i].l<<")"<<SubShellsUsedForOccupancyFit[i].JP<<" ";
	}
}

string parameters::GetComponentName(unsigned int iterator)
{
	if(iterator<UsedPenaltyFunctionComponents.size())
	{
		if(UsedPenaltyFunctionComponents[iterator]==1)
		{
			return "a_ij";
		}
		if(UsedPenaltyFunctionComponents[iterator]==2)
		{
			return "NPickupMax";
		}
		if(UsedPenaltyFunctionComponents[iterator]==3)
		{
			return "NStrippingMax";
		}
		if(UsedPenaltyFunctionComponents[iterator]==4)
		{
			return "EF_err";
		}
		if(UsedPenaltyFunctionComponents[iterator]==5)
		{
			return "Delta_err";
		}
	}
	return "unknown";
}

int Experiment::GetColor(int L, float JP)
{
	if(L==2)
	{
		if(abs(JP)==2.5)
		{
			return kGreen+2;
		}
		else if(abs(JP)==1.5)
		{
			return kGreen+1;
		}
	}
	else if(L==0)
	{
		return kGreen; 
	}
	else if(L==3)
	{
		if(abs(JP)==3.5)
		{
			return kBlue+2;
		}
		else if(abs(JP)==2.5)
		{
			return kBlue-2;
		}
	}
	else if(L==1)
	{
		if(abs(JP)==1.5)
		{
			return kRed-6;
		}
		else if(abs(JP)==0.5)
		{
			return kRed-7;
		} 
	}
	else if(L==4)
	{
		if(abs(JP)==4.5)
		{
			return kRed-10;
		}
		else if(abs(JP)==3.5)
		{
			return kRed-9;
		}
	}
	cout<<"Error: Experiment::GetColor cannot return color for this L, JP, returns black!"<<endl;
	return kBlack;
}

Experiment::Experiment()
{
	E_iterator=0; n_iterator=1; L_iterator=2; JP_iterator=3; SF_iterator=4;
}

string Experiment::GetType()
{
	if(type==1)
	{
		return "stripping";
	}
	else if(type==0)
	{
		return "pickup";
	}
	cout<<"Error: string Experiment::GetType() cannot get reaction type!"<<"\n";
	return "error";
}

void Experiment::ReadInputFile(string filename)//просто чтение файла с данными. Сначала поиск ключевых слов, если их нет, то попытка считать строку как состояние, наблюдаемое в эксперименте
{
	ifstream ifs(filename.c_str());
	string line;
	while(getline(ifs,line))
	{
		//cout<<line<<"\n";
		std::stringstream sstr(line);
		string line_tmp;
		sstr>>line_tmp;
		if((line_tmp=="Reference:")||(line_tmp=="Author:"))
		{
			sstr>>reference;
		}
		if(line_tmp=="Nucleus:")
		{
			sstr>>Nucleus;
		}
		if(line_tmp=="Type:")
		{
			string typeR;
			sstr>>typeR;
			if(typeR=="stripping")
			{
				type=1;
			}
			if(typeR=="pickup")
			{
				type=0;
			}
		}
		else if(line_tmp=="Reaction:")
		{
			sstr>>reaction;
		}
		else if(line_tmp=="ProjectileEnergy:")
		{
			sstr>>ProjectileEnergy;
		}
		else if(line_tmp=="BA:")
		{
			sstr>>BA;
			if(BA<100)
			{
				BA=BA*1000;
			}
		}
		else if(line_tmp=="BA1:")
		{
			sstr>>BA1;
			if(BA1<100)
			{
				BA1=BA1*1000;
			}
		}
		else if(line_tmp=="JP0:")
		{
			sstr>>JP0;
		}
		else
		{
			if((line_tmp[0]>='0')&&(line_tmp[0]<='9')&&((line_tmp[0]!='*')))
			{
				State s_tmp(line);
				s_tmp.UseFlag=1;
				if(s_tmp.Good())
				{	
					if((s_tmp.JP.size()>1)||(s_tmp.L.size()>1)||(s_tmp.n.size()>1)||(s_tmp.SpectroscopicFactor.size()>1))
					{
						IndexesOfMultipleStates.push_back(States.size());
						cout<<"Multiple"<<IndexesOfMultipleStates.size();
					}
					States.push_back(s_tmp);
					States[States.size()-1].type=type;
					States[States.size()-1].JP0=JP0;
				}
			}
			else if((line_tmp[1]>='0')&&(line_tmp[1]<='9')&&((line_tmp[0]=='*')))
			{
				line_tmp.erase(0,1);
				
				State s_tmp(line);
				s_tmp.UseFlag=0;
				cout<<s_tmp.Good()<<"*\n";
				if(s_tmp.Good())
				{	
					States.push_back(s_tmp);
					States[States.size()-1].type=type;
					States[States.size()-1].JP0=JP0;
				}					
			}
			else
			{
				int iterator=0;
				stringstream str_stream(line);
				while(str_stream)
				{
					string tmp_string;
					str_stream>>tmp_string;
					if(tmp_string=="E")
					{
						E_iterator=iterator;
					}
					if(tmp_string=="n")
					{
						n_iterator=iterator;
					}
					if(tmp_string=="l")
					{
						L_iterator=iterator;
					}
					if(tmp_string=="JP")
					{
						JP_iterator=iterator;
					}
					if(tmp_string=="C2S")
					{
						SF_iterator=iterator;
					}
					iterator++;
					if(iterator>4)
					{
						break;
					}
				}
			}
		}
	}
}

void Experiment::ProcessExperimentalData()//надо будет добавить перебор n и j
{
	SSD.Calculate(States);
}	

double Experiment::GetCentroid(int n,int l,double JP_inp)//Возвращает центроид для данных JP
{
	return SSD.GetState(n,l,JP_inp).C;
}

double Experiment::GetSumSF(int n,int l,double JP_inp)//Возвращает сумму СФ для данных JP
{
	return SSD.GetState(n,l,JP_inp).SumG;
}

double Experiment::GetSumSF(StateParameters &s)
{
	// if (SSD.GetState(s.n,s.l,s.JP).SumG<0.1) cout<<"Error Experiment::GetSumSF() SumG is less than 0.1!"<<endl;
	return SSD.GetState(s.n,s.l,s.JP).SumG;
}

double Experiment::GetErSumSF(int n,int l,double JP_inp)//Возвращает ошибку суммы СФ для данных n,l,JP
{
	return SSD.GetState(n,l,JP_inp).er_SumG;
}

double Experiment::GetErSumSF(StateParameters &s)
{
	return SSD.GetState(s.n,s.l,s.JP).er_SumG;
}

double Experiment::GetCentroid(StateParameters &s)
{
	return SSD.GetState(s.n,s.l,s.JP).C;
}

int Experiment::GetNCalculatedLevels()//возвращает число уровней, для которых вычислены центроиды
{
	return SSD.size();
}

int Experiment::size()//возвращает число состояний, зарегистрированных в эксперименте
{
	return States.size();
}

SummarizedSpectroscopicState&  Experiment::operator [] (int index)
{
	if(index<SSD.States.size())
	{
		return SSD.States[index];
	}
	else
	{
		cout<<reference<<" SummarizedSpectroscopicState index error: index="<<index<<" States.size()="<<SSD.States.size()<<"\n";
		SummarizedSpectroscopicState s1;
		return s1;
	}
}

SpectroscopicFactorHistogram Experiment::BuildSpectroscopicFactorHistogram(double norma=1)//метод для заполнения гистограммы нормированными данными экперимента
{	//возвращает объект класса SpectroscopicFactorHistogram с данными нормированного (*norma) объекта класса Experiment для дальнейшего построения
	SpectroscopicFactorHistogram SFHistograms;//создаём пустую гистограмму, которую будем заполнять данными
	if (norma<0)
	{
		cout<<"	*** Error! Experiment::BuildSpectroscopicFactorHistogram got negative normalisation coefficient! Return empty SpectroscopicFactorHistogram!"<<endl;
		return SFHistograms;
	} 
	SFHistograms.maximum=0;
	TString name_buffer=" ";
	if (norma!=1) name_buffer=" norm. ";
	SFHistograms.Reference=reference+name_buffer+GetType();
	for(unsigned int i=0;i<SSD.size();i++)//для каждого состояния в объекте SSD (SummarizedSpectroscopicData)
	{
		string name=reference+sprintf("_%d_%d_%d",SSD.States[i].n,SSD.States[i].L,(int)(SSD.States[i].JP*2));//имя гистограмы?
		TH1F histogram(name.c_str(),name.c_str(),100,0,10000);
		for(unsigned int j=0;j<States.size();j++)
		{
			if((States[j].n[0]==SSD.States[i].n)&&(States[j].L[0]==SSD.States[i].L)&&(States[j].JP[0]==SSD.States[i].JP))
			histogram.SetBinContent(histogram.GetXaxis()->FindFixBin(States[j].Energy),States[j].G()*norma);//строим столбик высотой с нормированную спектроскопическую силу?
		}
		histogram.SetLineColor(GetColor(SSD.States[i].L, SSD.States[i].JP));
		histogram.SetFillColor(GetColor(SSD.States[i].L, SSD.States[i].JP));
		if(histogram.GetMaximum()>SFHistograms.maximum)
		{
			SFHistograms.maximum=histogram.GetMaximum();
		} 
		SFHistograms.Histograms.push_back(histogram);
		SFHistograms.n.push_back(SSD.States[i].n);
		SFHistograms.L.push_back(SSD.States[i].L);
		SFHistograms.JP.push_back(SSD.States[i].JP);
	}
	return SFHistograms;//возвращает результирующую гистограмму
}//конец метода BuildSpectroscopicFactorHistogram()

TH1F CoupleOfExperiments::BuildPenaltyComponentsHistogram()
{
	TH1F result("PFC","Penalty function components",PenaltyComponents.size()+1,0,PenaltyComponents.size()+1);
	result.GetYaxis()->SetRangeUser(10e-6,1);
	for(unsigned int i=0;i<PenaltyComponents.size();i++)
	{
		result.SetBinContent(i+1,PenaltyComponents[i]);
		result.GetXaxis()->SetBinLabel(i+1,par.GetComponentName(i).c_str());
	}
	return result;
}

CoupleOfExperiments::CoupleOfExperiments(Experiment &InpPickup,Experiment &InpStripping)//конструктор, аргументы которого представляют из себя эксперименты по подхвату и срыву
{
	Pickup=InpPickup;
	Stripping=InpStripping;
}

void CoupleOfExperiments::GenerateCommonNJPList()
{
	cout<<"CoupleOfExperiments::GenerateCommonNJPList() has started!"<<endl;
	par.Cout();
	for(int i=0;i<Pickup.GetNCalculatedLevels();i++)
	{
		unsigned char flag=0;
		for(int j=0;j<Stripping.GetNCalculatedLevels();j++)
		{
			if(Pickup[i].Compare(Stripping[j]))
			{
				StateParameters s(Pickup[i].n,Pickup[i].L,Pickup[i].JP,"both");
				if (par.LimitedSubShellsUsedInDrawing==1)//если пользователь указывал в файле параметров подоболочки для отрисовки
				{
					cout<<"Check no "<<i<<":"<<endl;
					s.Cout();
					if(s.CheckIfIncludedIn(par.SubShellsUsedInDrawing)) 
					{
						cout<<"s is included in par.SubShellsUsedInDrawing!"<<endl;
						//cout<<"SP_centroids["<<i<<"].GetToBeDrawnFlag()=="<<SP_centroids[i].GetToBeDrawnFlag()<<endl;
						s.SetToBeDrawnFlag(1);//средни указанных для отрисовки есть данное состояние, то выставим ему флаг, что его нужно отрисовывать
						cout<<"s.GetToBeDrawnFlag()=="<<s.GetToBeDrawnFlag()<<endl;
					}
					else  
					{
						cout<<"s is NOT included in par.SubShellsUsedInDrawing!"<<endl;
						s.SetToBeDrawnFlag(0);//иначе укажем флаг, что отрисовывать не надо
						cout<<"s.GetToBeDrawnFlag()=="<<s.GetToBeDrawnFlag()<<endl;
					}
				}
				SP.push_back(s);
				flag=1;
			}
		}
		if((flag==0)&&((par.IncompleteCouplesFlag==2)||(par.IncompleteCouplesFlag==1)))
		{
			StateParameters s(Pickup[i].n,Pickup[i].L,Pickup[i].JP,"pickup");
			cout<<"Check no "<<i<<":"<<endl;
			s.Cout();
			if(s.CheckIfIncludedIn(par.SubShellsUsedInDrawing)) 
			{
				cout<<"s is included in par.SubShellsUsedInDrawing!"<<endl;
				//cout<<"SP_centroids["<<i<<"].GetToBeDrawnFlag()=="<<SP_centroids[i].GetToBeDrawnFlag()<<endl;
				s.SetToBeDrawnFlag(1);//средни указанных для отрисовки есть данное состояние, то выставим ему флаг, что его нужно отрисовывать
				cout<<"s.GetToBeDrawnFlag()=="<<s.GetToBeDrawnFlag()<<endl;
			}
			else  
			{
				cout<<"s is NOT included in par.SubShellsUsedInDrawing!"<<endl;
				s.SetToBeDrawnFlag(0);//иначе укажем флаг, что отрисовывать не надо
				cout<<"s.GetToBeDrawnFlag()=="<<s.GetToBeDrawnFlag()<<endl;
			}
			SP.push_back(s);
		}
		
	}//не особо эффективный алгоритм поиска отсутствия совпадений, но сейчас скорость не особо принципиальна
	for(int i=0;i<Stripping.GetNCalculatedLevels();i++)
	{
		unsigned char flag=0;
		for(int j=0;j<Pickup.GetNCalculatedLevels();j++)
		{
			if(Pickup[j].Compare(Stripping[i]))
			{
				flag=1;
			}
		}
		if((flag==0)&&((par.IncompleteCouplesFlag==3)||(par.IncompleteCouplesFlag==1)))
		{
			StateParameters s(Stripping[i].n,Stripping[i].L,Stripping[i].JP,"stripping");
			cout<<"Check no "<<i<<":"<<endl;
			s.Cout();
			if (par.LimitedSubShellsUsedInDrawing==1)//если пользователь указывал в файле параметров подоболочки для отрисовки
			{
				if(s.CheckIfIncludedIn(par.SubShellsUsedInDrawing)) 
				{
					cout<<"s is included in par.SubShellsUsedInDrawing!"<<endl;
					//cout<<"SP_centroids["<<i<<"].GetToBeDrawnFlag()=="<<SP_centroids[i].GetToBeDrawnFlag()<<endl;
					s.SetToBeDrawnFlag(1);//средни указанных для отрисовки есть данное состояние, то выставим ему флаг, что его нужно отрисовывать
					cout<<"s.GetToBeDrawnFlag()=="<<s.GetToBeDrawnFlag()<<endl;
				}
				else  
				{
					cout<<"s is NOT included in par.SubShellsUsedInDrawing!"<<endl;
					s.SetToBeDrawnFlag(0);//иначе укажем флаг, что отрисовывать не надо
					cout<<"s.GetToBeDrawnFlag()=="<<s.GetToBeDrawnFlag()<<endl;
				}
			}
			SP.push_back(s);
		}		
	}
	cout<<"CoupleOfExperiments::GenerateCommonNJPList() has ended!"<<endl;
}

void CoupleOfExperiments::CalcSPE_and_OCC()//функция рассчитывает одночастичную энергию, 
{//формирует графики для отрисовки и т.д.
	cout<<"CoupleOfExperiments::CalcSPE_and_OCC has started!"<<endl;
	GenerateCommonNJPList();//
	vector<double> OccupanciesForBCSFit;//отдельные векторы заселенностей для аппроксимации БКШ
	vector<double> EnergiesForBCSFit;
	for(int i=0;i<SP.size();i++)
	{
		double C_pickup=Pickup.GetCentroid(SP[i]);
		double C_stripping=Stripping.GetCentroid(SP[i]);	
		cout<<NLJToString(SP[i].n,SP[i].l,SP[i].JP)<<"C_pickup="<<C_pickup<<" C_stripping="<<C_stripping<<"\n"; 
		if((C_stripping!=-1)&&(C_pickup!=-1)&&(!isnan(C_stripping))&&(!isnan(C_pickup)))//индусский fix, потом проверить, что генерирует nan
		{
			double E_pickup=-Pickup.BA-C_pickup;//Диплом Марковой М.Л., ф-ла 4
			double E_stripping=-Stripping.BA1+C_stripping;//Диплом Марковой М.Л., ф-ла 5
			cout<<NLJToString(SP[i].n,SP[i].l,SP[i].JP)<<"E_pickup="<<E_pickup<<" E_stripping="<<E_stripping<<"\n"; 
			double SPE_tmp=(Pickup.GetSumSF(SP[i])*E_pickup+Stripping.GetSumSF(SP[i])*E_stripping)/(Pickup.GetSumSF(SP[i])+Stripping.GetSumSF(SP[i]));
			double OCC_tmp=(Pickup.GetSumSF(SP[i])-Stripping.GetSumSF(SP[i])+2*abs(SP[i].JP)+1)/(2*(2*abs(SP[i].JP)+1));
			cout<<NLJToString(SP[i].n,SP[i].l,SP[i].JP)<<" pickup_sumG = "<<Pickup.GetSumSF(SP[i])<<" stripping_sumG ="<<Stripping.GetSumSF(SP[i])<<"\n";
			cout<<NLJToString(SP[i].n,SP[i].l,SP[i].JP)<<" pickup_C = "<<C_pickup<<" stripping_C = "<<C_stripping<<"\n";
			SPE.push_back(SPE_tmp);//Диплом Марковой М.Л., ф-ла 17
			OCC.push_back(OCC_tmp);//Диплом Марковой М.Л., ф-ла 18
			ParticlesAndHolesSum.push_back((Pickup.GetSumSF(SP[i])+Stripping.GetSumSF(SP[i]))/(2*abs(SP[i].JP)+1));
			SP_centroids.push_back(SP[i]);
			if(par.CheckBelonging(SP[i],par.SubShellsUsedForOccupancyFit))
			{
				OccupanciesForBCSFit.push_back(OCC_tmp);//отдельные векторы заселенностей для аппроксимации БКШ
				EnergiesForBCSFit.push_back(SPE_tmp);
			}
		}
		else
		{
			cout<<"C_stripping or C_pickup is incorrect (not NaN or -1!?)! Add SPE = 0 and OCC = 0!\n"; 
			SPE.push_back(0);
			OCC.push_back(0);
		}
	}
	occupancies=TGraph(OccupanciesForBCSFit.size(),&EnergiesForBCSFit[0],&OccupanciesForBCSFit[0]);
	BCS=TF1("BCS","0.5*(1-(x-[0])/(sqrt((x-[0])^2+[1]^2)))",-50000,0);
	BCS.SetParameter(0,-8000);
	BCS.SetParameter(1,15000);
	float min_E,max_E,min_OCC,max_OCC;
	for(unsigned int i=0;i<OCC.size();i++)
	{
		//cout<<"SP_centroids["<<i<<"]==";
		//SP_centroids[i].Cout();
		//cout<<"SP_centroids["<<i<<"].GetType()=="<<SP_centroids[i].GetType()<<endl;
		//cout<<"SP_centroids["<<i<<"].GetToBeDrawnFlag()=="<<SP_centroids[i].GetToBeDrawnFlag()<<endl;
		if(SP_centroids[i].GetType()=="pickup")
		{
			if (SP_centroids[i].GetToBeDrawnFlag()) 
			{
				Pickup_occupancies.SetPoint(Pickup_occupancies.GetN(),SPE[i],OCC[i]);
			}
		}
		else if(SP_centroids[i].GetType()=="stripping")
		{
			if (SP_centroids[i].GetToBeDrawnFlag()) 
			{
				Stripping_occupancies.SetPoint(Stripping_occupancies.GetN(),SPE[i],OCC[i]);
			}
		}
		else if(SP_centroids[i].GetType()=="both")
		{
			if (SP_centroids[i].GetToBeDrawnFlag()) 
			{
				Both_occupancies.SetPoint(Both_occupancies.GetN(),SPE[i],OCC[i]);
			}	
		}
		if(min_E>SPE[i])
		{
			min_E=SPE[i];
		}
		if(max_E<SPE[i])
		{
			max_E=SPE[i];
		}
		if(min_OCC>OCC[i])
		{
			min_OCC=OCC[i];
		}
		if(max_OCC<OCC[i])
		{
			max_OCC=OCC[i];
		}
	}
	
	SetTGraphLimits(Pickup_occupancies,min_E,max_E,min_OCC,max_OCC);
	Pickup_occupancies.SetMarkerColor(4);
	SetTGraphLimits(Stripping_occupancies,min_E,max_E,min_OCC,max_OCC);
	Stripping_occupancies.SetMarkerColor(2);
	SetTGraphLimits(Both_occupancies,min_E,max_E,min_OCC,max_OCC);
	Both_occupancies.SetMarkerColor(1);
	//occupancies.Draw("AP");	
	occupancies.SetMarkerStyle(28);
	occupancies.SetMarkerSize(2);
	occupancies.Fit(&BCS,"M");//профитируем функцией в приближении БКШ
	//BCS.Draw("l same");
	
	Ef=BCS.GetParameter(0);
	Ef_error=BCS.GetParError(0);
	Delta=BCS.GetParameter(1);
	Delta_error=BCS.GetParError(1);
	
	for(int i=0;i<SP.size();i++)//цикл for; для каждой подоболочки:
	{
		Gp_c.push_back(Stripping.GetSumSF(SP[i]));//формируем вектор из сумм СС срыва для каждой подоболочки
		Gm_c.push_back(Pickup.GetSumSF(SP[i]));//формируем вектор из сумм СС подхвата для каждой подоболочки
		er_Gp_c.push_back(Stripping.GetErSumSF(SP[i]));//формируем вектор из ошибок сумм СС срыва для каждой подоболочки
		er_Gm_c.push_back(Pickup.GetErSumSF(SP[i]));//формируем вектор из ошибок сумм СС подхвата для каждой подоболочки
		Gp_alt_c.push_back(Stripping.GetSumSF(SP[i])/(2*abs(SP[i].JP)+1));//формируем вектор из делённых на (2j+1) сумм СС срыва для каждой подоболочки
		Gm_alt_c.push_back(Pickup.GetSumSF(SP[i])/(2*abs(SP[i].JP)+1));//формируем вектор из делённых на (2j+1) сумм СС подхвата для каждой подоболочки
		er_Gp_alt_c.push_back(Stripping.GetErSumSF(SP[i])/(2*abs(SP[i].JP)+1));//формируем вектор из делённых на (2j+1) ошибок сумм СС срыва для каждой подоболочки
		er_Gm_alt_c.push_back(Pickup.GetErSumSF(SP[i])/(2*abs(SP[i].JP)+1));//формируем вектор из делённых на (2j+1) сумм СС подхвата для каждой подоболочки
	}
	cout<<"CoupleOfExperiments::CalcSPE_and_OCC has ended!"<<endl;
}

void CoupleOfExperiments::ClearCalcResults()
{
	cout<<"CoupleOfExperiments::ClearCalcResults() has started!"<<endl;
	//очищаем аттрибуты объекта - вектора вычисленных величин:
	SP.resize(0);
	SP_centroids.resize(0);
	SPE.resize(0);
	OCC.resize(0);
	ParticlesAndHolesSum.resize(0);
	//OccupanciesForBCSFit.resize(0);//отдельные векторы заселенностей для аппроксимации БКШ
	//EnergiesForBCSFit.resize(0);
	
	Gp_c.resize(0);//вектор спектроскопических сил срыва подоболочек 
	Gm_c.resize(0);//вектор спектроскопических сил подхвата подоболочек 
	er_Gp_c.resize(0);//вектор ошибок спектроскопических сил срыва подоболочек для расчёта нормированных Gp_norm_c
	er_Gm_c.resize(0);//вектор ошибок спектроскопических сил подхвата подоболочек для расчёта нормированных Gm_norm_c
	Gp_alt_c.resize(0);//вектор спектроскопических сил  подоболочек для расчёта через МНК, делённых на 2j+1 (они должны быть близки к 1)
	Gm_alt_c.resize(0);//вектор спектроскопических сил подоболочек для расчёта через МНК, делённых на 2j+1 (они должны быть близки к 1)
	er_Gp_alt_c.resize(0);//вектор ошибок спектроскопических сил подоболочек для расчёта через МНК, делённых на 2j+1
	er_Gm_alt_c.resize(0);
	
	//заменяем графики заселённостей на пустые:
	occupancies=TGraph();//график заселённостей состояний от энергии, использованных для фита БКШ
	Pickup_occupancies=TGraph();
	Stripping_occupancies=TGraph();
	Both_occupancies=TGraph();
	cout<<"CoupleOfExperiments::ClearCalcResults() has ended!"<<endl;
}

string CoupleOfExperiments::ResultsInTextForm(char verbose_level)
{
	stringstream s;
	if(verbose_level==0)
	{
		s<<"Experiment: "<<Pickup.reference<<" ("<<Pickup.size()<<") "<<Stripping.reference<<" ("<<Stripping.size()<<") \n";
	}
	else if(verbose_level==1)
	{
		s<<"Experiment(pickup): "<<Pickup.reference<<" ("<<Pickup.size()<<")\n";
		s<<Pickup.ChangesLog<<"\n";
		s<<"Experiment(stripping): "<<Stripping.reference<<" ("<<Stripping.size()<<")\n";
		s<<Stripping.ChangesLog<<"\n";
	}
	
	s<<Pickup.particle<<" transfer\n";
	//s<<Pickup.particle[0]<<" separation energy A:"<<Pickup.BA<<", A+1: "<<Pickup.BA1<<"\n";
	s<<"E_F: "<<Ef<<" #pm "<<Ef_error<<"  keV \n #Delta: "<<Delta<<" #pm "<<Delta_error<<" keV\n";
	s<<"penalty: "<<penalty<<"\n";
	s<<"SPE,keV nlj OCC #frac{G^{+}+G^{-}}{2J+1}\n";
	for(unsigned int i=0;i<SPE.size();i++)
	{
		if (SP_centroids[i].GetToBeDrawnFlag()) s<<SPE[i]<<" "<<NLJToString(SP_centroids[i].n,SP_centroids[i].l,SP_centroids[i].JP)<<" "<<OCC[i]<<" "<<ParticlesAndHolesSum[i]<<"\n";
	}
	return s.str();
}

void CoupleOfExperiments::DrawResultsInTextForm(string str)
{
	//stringstream s(ResultsInTextForm());
	stringstream s(str);
	///
	string LatexLineTmp;
	TLatex latex;
	float x=0.05,y=0.9, step=-0.08;
	while(getline(s,LatexLineTmp))
	{
		latex.DrawLatexNDC(x,y,LatexLineTmp.c_str());
		y+=step;
	}
}

NormalisedCoupleOfExperiments::NormalisedCoupleOfExperiments(Experiment &InpPickup, Experiment &InpStripping)
:CoupleOfExperiments(InpPickup, InpStripping)//уточняем,какой конструктор родительского класса нужно вызывать при использовании этого конструктора
{
	//Pickup=InpPickup;
	//Stripping=InpStripping;
}

void NormalisedCoupleOfExperiments::InduceNormalisation()
{
	cout<<"NormalisedCoupleOfExperiments::InduceNormalisation() has started!"<<endl;
	NormalisationWasTried=1;
	///подготовительная работа для реализации фитирования закончена, теперь фитируем:
	vector<double> OccupanciesForNormFit;//отдельный вектор заселённостей для аппроксимации нормировки (пока совпадают с набором для аппроксимации БКШ)
	vector<double> EnergiesForNormFit;//отдельный вектор энергий для аппроксимации нормировки (пока совпадают с набором для аппроксимации БКШ)
	//cout << "||||||| solveLinear_mod started working!" << endl;//оглашает, что эта подпрограмма начала работать
	//cout << "Perform the fit  y = a + b * x in Minuit" << endl;//оглашает с помощью какой Root штуки берём фит (Minuit)
	//int size = SP.size();//число G нормированных на 2j+1 в векторе (т.е. число подоболочек)
	//int p_size = OccupanciesForNormFit.size();//число фитируемых точек совпадает с числом учитываемых подоболочек
	cout << "par.SubShellsUsedForNormalisation.size() = "<<par.SubShellsUsedForNormalisation.size()<<endl;
	cout << "SP.size() = "<<SP.size()<<endl;
	for(int i=0;i<SP.size();i++)//цикл for; для каждой подоболочки:
	{	
		if(par.CheckBelonging(SP[i],par.SubShellsUsedForNormalisation))///здесь можно заменить на отдельную функцию проверки подоболочки на принадлежность к списку для участия в фите нормировки
		{
			cout << "Got state parameters included in UsedForNormalisation list."<<endl;
			p_size++;
		}
	}
	if ((p_size < 2) || (p_size>SP.size()))//но если точек оказалось меньше двух или точек полагается больше, чем есть подоболочек
	{
		cout << "Note: Wrong number of Normalisation FIT points: "<<p_size<<". Fit will not be performed! Return!"<<endl;
		return;
	}//то возьмём число точек равным числу подооболочек во входных данных
	else
	{
		cout << "Number of Normalisation FIT points: "<<p_size<<". Fit will be performed! Continue!"<<endl;
	}
	cout << "Creating arrays with points."<<endl;
	//массивы с координатами точек:
	Double_t x[p_size], x2[p_size];//x координаты точек 
	Double_t y[p_size], y2[p_size];//y координаты точек 
	Double_t xe[p_size], xe2[p_size];//ошибки по x 
	Double_t ye[p_size], ye2[p_size];//ошибки по y
    cout << "Filling arrays with points."<<endl;
	for(int i=0;i<SP.size();i++)//цикл для заполнения координат точек и их ошибок
	{
		if(par.CheckBelonging(SP[i],par.SubShellsUsedForNormalisation))
		{
			x[i] = Gm_alt_c[i]; 
			y[i] = Gp_alt_c[i];
			xe[i] = er_Gm_alt_c[i]; 
			ye[i] = er_Gp_alt_c[i];
		}
	}
	///Вариант с усреднением двух фитов:
	//cout << "|||||| Fiting by Minuit through TGraph:" << endl;//огласим, что используем для фита
	TGraphErrors *gr1 = new TGraphErrors(p_size,x,y,0,ye);
	TGraphErrors *gr2 = new TGraphErrors(p_size,y,x,0,xe);//A TGraphErrors is a TGraph with error bars. 
	//В аргументах: чило точек, координаты точек по х, координаты точек по у, ошибки точек по х, ошибки точек по у
	//если передавать ошибки по x, то фит сработает неправильно, поэтому обнуляем их (очень жаль, но пока не решено)
	cout << "Creating TF1 functions."<<endl;
	TF1 *f1 = new TF1("f1","pol1");//A TF1 object is a 1-Dim function, f1 - её название, тип pol1 - линейная функция: y=a*x+b, определяем ей от 0 до максимального G^- + 0.1
	TF1 *f2 = new TF1("f2","pol1");
	//A TF1 object is a 1-Dim function defined between a lower and upper limit.
	cout << "Starting first Fit."<<endl;
	gr1->Fit("f1");//фитируем; если с параметром Q: Fit("FIT","Q"); - quiet, отключить вывод параметров фита
	cout << "Starting second Fit."<<endl;
	gr2->Fit("f2");
	cout << "Getting resulting parameters of the Fits."<<endl;
	fit_a = f1->GetParameter(0);//возвращаем подобранный 1-ый параметр прямой 1-го фита
	fit_b = f1->GetParameter(1);//возвращаем подобранный 2-ой параметр прямой 2-го фита
	double fit_a2 = f2->GetParameter(0);
	double fit_b2 = f2->GetParameter(1);
	er_fit_a = f1->GetParError(0);//возвращаем ошибку подобранного 1-го параметра прямой 
	er_fit_b = f1->GetParError(1);//возвращаем ошибку подобранного 2-го параметра прямой 
	double er_fit_a2 = f2->GetParError(0);
	double er_fit_b2 = f2->GetParError(1);
	cout << "n+ = " << 1/fit_a << " and " << -fit_b2/fit_a2 << endl;
	cout << "n- = " << 1/fit_a2 << " and " << -fit_b/fit_a << endl;
	n_p = (1/fit_a-fit_b2/fit_a2)/2;//находим первый нормировочный параметр n+ через среднее двух фитов
	n_m = (1/fit_a2-fit_b/fit_a)/2;//находим второй нормировочный параметр n- через среднее двух фитов
		
	if ((n_p < 0.5) || (n_m < 0.5) || (isnan(n_p)) || (isnan(n_m)))//если фит вышел плохой, то n+ и n- не имеют смысла, 
	{//так что приравниваем их к 1
		cout<<"	Note: NormalisedCoupleOfExperiments::InduceNormalisation() normalisation procedure has failed!"<<endl;
		cout<<"n_p = "<<n_p<<"; n_m = "<<n_m<<endl;
		n_p = 1.; 
		n_m =1.;
		cout<<"n_p and n_m will be turned into 1!"<<endl;
	}
	else
	{
		cout<<"	Note: NormalisedCoupleOfExperiments::InduceNormalisation() normalisation procedure has been sucsesfull!"<<endl;
	}	
	er_n_p = 1/2*sqrt((er_fit_a/(fit_a*fit_a))*(er_fit_a/(fit_a*fit_a))+(er_fit_b2/fit_a2)*(er_fit_b2/fit_a2)+(fit_b2/(fit_a2*fit_a2)*er_fit_a2)*(fit_b2/(fit_a2*fit_a2)*er_fit_a2));//находим ошибку первого нормировочного параметра n+ 
	er_n_m = 1/2*sqrt((er_fit_b/fit_a)*(er_fit_b/fit_a)+(fit_b/(fit_a*fit_a)*er_fit_a)*(fit_b/(fit_a*fit_a)*er_fit_a)+er_fit_a2/(fit_a2*fit_a2)*er_fit_a2/(fit_a2*fit_a2));//находим ошибку второго нормировочного параметр n- 
	cout<<"	Normalisation result:"<<endl;
	cout << "n+ = " << n_p << "+/-" << er_n_p << endl;//выводим первый нормировочный параметр n+ 
	cout << "n- = " << n_m << "+/-" << er_n_m << endl;//выводим второй нормировочный параметр n-
	er_fit_a = er_n_p/(n_p*n_p);//
	er_fit_b = sqrt((er_n_p/n_p)*(er_n_p/n_p)+(er_n_m*n_m/n_p/n_p)*(er_n_m*n_m/n_p/n_p));
		
	for(int j=0;j<p_size;j++)//цикл для заполнения векторов новых координат для нормированных сил
	{
		x2[j] = Gm_alt_c[j]*n_m; 
		y2[j] = Gp_alt_c[j]*n_p;
		xe2[j] = er_Gm_alt_c[j]*n_m; 
		ye2[j] = er_Gp_alt_c[j]*n_p;
	}
	for(int j=0;j<SP.size();j++)//цикл для заполнения векторов новых, нормированных СС
	{
		Gp_c[j]=n_p*Gp_c[j];//вычисляем вектор новых, нормированных СС для срыва
		cout << "G+_norm[" << j << "] = " << Gp_c[j] << endl;
		Gm_c[j]=n_m*Gm_c[j];//вычисляем вектор новых, нормированных СС для подхвата
		cout << "G-_norm[" << j << "] = " << Gm_c[j] << endl;
	}
	//cout << "I finished Gp_norm and Gm_norm!!! Now solveLinear_mod() ends!!!" << endl;
	points_G=TGraphErrors(p_size,x,y,0,ye);//создаём график точек, которые мы фитировали
	points_G_norm=TGraphErrors(p_size,x2,y2,0,ye2);//создаём график точек, которые мы получили из фита
	FIT=TF1("FIT","pol1",0,GetMaximum(Gm_alt_c)+1);//создаём линию для изображения фита
	FIT.SetParameter(0,fit_a);//устанавливаем свободный член прямой, которую будем строить
	FIT.SetParameter(1,fit_b);//устанавливаем коэффициента наклона прямой, которую будем строить
	///вторая линия фита:
	FIT2=TF1("FIT","pol1",0,GetMaximum(Gm_alt_c)+1);//создаём линию для изображения фита
	FIT2.SetParameter(0,-fit_a2/fit_b2);//устанавливаем свободный член прямой, которую будем строить
	FIT2.SetParameter(1,1/fit_b2);//устанавливаем коэффициента наклона прямой, которую будем строить
	///третья линия фита (усреднённая):
	FIT3=TF1("FIT","pol1",0,GetMaximum(Gm_alt_c)+1);//создаём линию для изображения фита
	FIT3.SetParameter(0,1/n_p);//устанавливаем свободный член прямой, которую будем строить
	FIT3.SetParameter(1,-n_m/n_p);//устанавливаем коэффициента наклона прямой, которую будем строить
}

void NormalisedCoupleOfExperiments::ReCalcSPE_and_OCC()
{
	cout<<"NormalisedCoupleOfExperiments::ReCalcSPE_and_OCC has started!"<<endl;
	GenerateCommonNJPList();
	vector<double> OccupanciesForBCSFit;//отдельные векторы заселенностей для аппроксимации БКШ
	vector<double> EnergiesForBCSFit;
	///применяем расчитанные коэффициенты нормировки n_m, n_p:
	this->ClearCalcResults();
	//рассчитываем величины и заполняем вектора заново:
	for(int i=0;i<SP.size();i++)//цикл for; для каждой подоболочки:
	{
		double C_pickup=Pickup.GetCentroid(SP[i]);//записываем значение центроида для эксперимента по подхвату в переменную C_pickup
		double C_stripping=Stripping.GetCentroid(SP[i]);//записываем значение центроида для эксперимента по срыву в переменную C_stripping	
			
		if((C_stripping!=-1)&&(C_pickup!=-1)&&(!isnan(C_stripping))&&(!isnan(C_pickup)))//индусский fix, потом проверить, что генерирует nan
		{
			double E_pickup=-Pickup.BA-C_pickup;//Диплом Марковой М.Л., ф-ла 4//вычисление "одночастичной" для подхвата с использованием энергии отрыва нуклона
			double E_stripping=-Stripping.BA1+C_stripping;//Диплом Марковой М.Л., ф-ла 5//вычисление "одночастичной" для срыва с использованием энергии отрыва нуклона
			double SPE_tmp=(Pickup.GetSumSF(SP[i])*E_pickup*n_m+Stripping.GetSumSF(SP[i])*E_stripping*n_p)/(Pickup.GetSumSF(SP[i])*n_m+Stripping.GetSumSF(SP[i])*n_p);//Диплом Марковой М.Л., ф-ла 17 //вычисление одночастичной энергии после нормировки
			double OCC_tmp=(Pickup.GetSumSF(SP[i])*n_m-Stripping.GetSumSF(SP[i])*n_p+2*abs(SP[i].JP)+1)/(2*(2*abs(SP[i].JP)+1));//Диплом Марковой М.Л., ф-ла 18 //это v^2_{nlj} после нормировки	
			ParticlesAndHolesSum.push_back((n_m*Pickup.GetSumSF(SP[i])+n_p*Stripping.GetSumSF(SP[i]))/(2*abs(SP[i].JP)+1));
				
			SPE.push_back(SPE_tmp);//Диплом Марковой М.Л., ф-ла 17
			OCC.push_back(OCC_tmp);//Диплом Марковой М.Л., ф-ла 18
			SP_centroids.push_back(SP[i]);
			if(par.CheckBelonging(SP[i],par.SubShellsUsedForOccupancyFit))
			{
				OccupanciesForBCSFit.push_back(OCC_tmp);//отдельные векторы заселенностей для аппроксимации БКШ
				EnergiesForBCSFit.push_back(SPE_tmp);
			}
		}
		else//если проверка на существование центроидов у экспериментов (индусский fix) не вернёт истину, то
		{
			SPE.push_back(0);//добавляем в вектор одночастичных энергий 0, вместо рассчитанного значения
			OCC.push_back(0);//добавляем в вектор заселённостей 0, вместо рассчитанного значения
		}
	}//конец цикла for
	cout<<"SPE.size() = "<<SPE.size()<<endl;
	occupancies=TGraph(OccupanciesForBCSFit.size(),&EnergiesForBCSFit[0],&OccupanciesForBCSFit[0]);
	BCS=TF1("BCS_norm","0.5*(1-(x-[0])/(sqrt((x-[0])^2+[1]^2)))",-50000,0);
	BCS.SetParameter(0,-8000);
	BCS.SetParameter(1,15000);
	float min_E,max_E,min_OCC,max_OCC;
	for(unsigned int i=0;i<OCC.size();i++)
	{
		if(SP_centroids[i].GetType()=="pickup")
		{
			Pickup_occupancies.SetPoint(Pickup_occupancies.GetN(),SPE[i],OCC[i]);
		}
		else if(SP_centroids[i].GetType()=="stripping")
		{
			Stripping_occupancies.SetPoint(Stripping_occupancies.GetN(),SPE[i],OCC[i]);
		}
		else if(SP_centroids[i].GetType()=="both")
		{
			Both_occupancies.SetPoint(Both_occupancies.GetN(),SPE[i],OCC[i]);
		}
		if(min_E>SPE[i])
		{
			min_E=SPE[i];
		}
		if(max_E<SPE[i])
		{
			max_E=SPE[i];
		}
		if(min_OCC>OCC[i])
		{
			min_OCC=OCC[i];
		}
		if(max_OCC<OCC[i])
		{
			max_OCC=OCC[i];
		}
	}
	
	SetTGraphLimits(Pickup_occupancies,min_E,max_E,min_OCC,max_OCC);
	Pickup_occupancies.SetMarkerColor(4);
	SetTGraphLimits(Stripping_occupancies,min_E,max_E,min_OCC,max_OCC);
	Stripping_occupancies.SetMarkerColor(2);
	SetTGraphLimits(Both_occupancies,min_E,max_E,min_OCC,max_OCC);
	Both_occupancies.SetMarkerColor(1);
	occupancies.Draw("AP");	
	occupancies.SetMarkerStyle(28);
	occupancies.SetMarkerSize(2);
	occupancies.Fit(&BCS,"M");
	BCS.Draw("l same");
	
	Ef=BCS.GetParameter(0);
	Ef_error=BCS.GetParError(0);
	Delta=BCS.GetParameter(1);
	Delta_error=BCS.GetParError(1);
	
	for(int i=0;i<SP.size();i++)//цикл for; для каждой подоболочки:
	{
		Gp_c.push_back(Stripping.GetSumSF(SP[i]));//формируем вектор из сумм СС срыва для каждой подоболочки
		Gm_c.push_back(Pickup.GetSumSF(SP[i]));//формируем вектор из сумм СС подхвата для каждой подоболочки
		er_Gp_c.push_back(Stripping.GetErSumSF(SP[i]));//формируем вектор из ошибок сумм СС срыва для каждой подоболочки
		er_Gm_c.push_back(Pickup.GetErSumSF(SP[i]));//формируем вектор из ошибок сумм СС подхвата для каждой подоболочки
		Gp_alt_c.push_back(Stripping.GetSumSF(SP[i])/(2*abs(SP[i].JP)+1));//формируем вектор из делённых на (2j+1) сумм СС срыва для каждой подоболочки
		Gm_alt_c.push_back(Pickup.GetSumSF(SP[i])/(2*abs(SP[i].JP)+1));//формируем вектор из делённых на (2j+1) сумм СС подхвата для каждой подоболочки
		er_Gp_alt_c.push_back(Stripping.GetErSumSF(SP[i])/(2*abs(SP[i].JP)+1));//формируем вектор из делённых на (2j+1) ошибок сумм СС срыва для каждой подоболочки
		er_Gm_alt_c.push_back(Pickup.GetErSumSF(SP[i])/(2*abs(SP[i].JP)+1));//формируем вектор из делённых на (2j+1) сумм СС подхвата для каждой подоболочки
	}
	cout<<"SPE.size() = "<<SPE.size()<<endl;
	cout<<"NormalisedCoupleOfExperiments::ReCalcSPE_and_OCC has ended!"<<endl;
}

string NormalisedCoupleOfExperiments::FitResultsInTextForm(char verbose_level)//функция записывает в строку результаты нормировки и принятые параметры (строка оказывается слева внизу каждого выходного pdf файла)
{
	cout<<"FitResultsInTextForm has started!"<<endl;
	stringstream s;//задаём строку, куда всё будем сохранять
	if(verbose_level==0)
	{
		s<<"Experiment: "<<Pickup.reference<<" ("<<Pickup.size()<<") "<<Stripping.reference<<" ("<<Stripping.size()<<") \n";
	}
	else if(verbose_level==1)
	{
		s<<"Experiment(pickup): "<<Pickup.reference<<" ("<<Pickup.size()<<")\n";
		s<<Pickup.ChangesLog<<"\n";
		s<<"Experiment(stripping): "<<Stripping.reference<<" ("<<Stripping.size()<<")\n";
		s<<Stripping.ChangesLog<<"\n";
	}
	unsigned int k=0;	
	s<<"n^{+}G*^{+} + n^{-}G*^{-} = 1 equations:\n";//укажем вид уравнений, которые потом преобразовывались для МНК
	cout<<"Will draw n^{+}G*^{+} + n^{-}G*^{-} = 1 equations"<<endl;
	cout<<"SPE.size() = "<<SPE.size()<<endl;
	for(unsigned int i=0;i<SPE.size();i++)
	{//выведем все уравнения для подоболочек, которые мы должны были использовать для нахождения n+ и n-:
		cout<<"Got n="<<SP_centroids[i].n<<"; l="<<SP_centroids[i].l<<"; JP="<<SP_centroids[i].JP<<endl;
		cout<<"par.SubShellsUsedForNormalisation.size() = "<<par.SubShellsUsedForNormalisation.size()<<endl;
		cout<<"par.CheckBelonging(SP["<<i<<"],par.SubShellsUsedForNormalisation = "<<par.CheckBelonging(SP[i],par.SubShellsUsedForNormalisation)<<endl;
		if(par.CheckBelonging(SP[i],par.SubShellsUsedForNormalisation))
		{	
			k++;
			cout<<"Got n="<<SP_centroids[i].n<<"; l="<<SP_centroids[i].l<<"; JP="<<SP_centroids[i].JP<<endl;
			s<<NLJToString(SP_centroids[i].n,SP_centroids[i].l,SP_centroids[i].JP)<<": n^{+} "<<Gp_alt_c[i]<<" +"<<" n^{-} "<<Gm_alt_c[i]<<" = 1\n";
			cout<<"Drawing data for "<<NLJToString(SP_centroids[i].n,SP_centroids[i].l,SP_centroids[i].JP)<<endl;
		}
	}
	if ((n_p==1.)&&(n_m==1.))//если нормировочные коэффициенты равны 1, то логично, что нормировки не происходило
	{
		s<<"Normalization was not perfomed.\n";
	}//сообщим об этом в .pdf файле
	else
	{
		s<<"Normalization of "<< k <<" subshells result:\n";//в этой строке укажем параметр, который ставили для ограничения числа точек в фите
		s<<"n^{+} = "<<n_p<<" #pm "<<er_n_p<<" (for stripping)\n";//выведем n+ с его ошибкой
		s<<"n^{-} = "<<n_m<<" #pm "<<er_n_m<<" (for pick-up)\n";//выведем n- с его ошибкой
		for(unsigned int i=0;i<SPE.size();i++)
		{//запишем получившиеся после нормировки спектроскопические силы, они должны быть ближе к ОМО, ради этого всё затевалось
			s<<NLJToString(SP_centroids[i].n,SP_centroids[i].l,SP_centroids[i].JP)<<" G^{+},G^{-}: "<<
			TString::Format(".2%f",Gp_c[i]/n_p)<<"->"<<TString::Format(".2%f",Gp_c[i])<<" "<<
			TString::Format(".2%f",Gm_c[i]/n_m)<<"->"<<TString::Format(".2%f",Gm_c[i])<<"\n";
		}
	}
	return s.str();//вернём строку, где всё сохранили
}//конец метода FitResultsInTextForm
	
string NormalisedCoupleOfExperiments::ResultsInTextForm(char verbose_level)//функция записывает в строку результаты и принятые параметры (строка оказывается слева внизу каждого выходного pdf файла)
{//cout<< "FitResultsInTextForm2 started working!!!!!\n";
	if(NormalisationWasTried==0)
	{
		return CoupleOfExperiments::ResultsInTextForm(verbose_level);
	}
	else
	{
		stringstream s;//задаём строку, куда всё будем сохранять
		if(verbose_level==0)
		{
			s<<"Experiment: "<<Pickup.reference<<" ("<<Pickup.size()<<") "<<Stripping.reference<<" ("<<Stripping.size()<<") \n";
		}
		else if(verbose_level==1)
		{
			s<<"Experiment(pickup): "<<Pickup.reference<<" ("<<Pickup.size()<<")\n";
			s<<Pickup.ChangesLog<<"\n";
			s<<"Experiment(stripping): "<<Stripping.reference<<" ("<<Stripping.size()<<")\n";
			s<<Stripping.ChangesLog<<"\n";
		}
		if ((n_p==1.)&&(n_m==1.))//если нормировочные колэффициенты равны 1, то логично, что нормировки не происходило
		{	
			s<<"penalty: "<<penalty<<"\n";
			s<<"Normalization was not perfomed.\n";
		}//сообщим об этом в .pdf файле
		else
		{	//cout<< "FitResultsInTextForm2 write result in s!!!!!\n";
				
			s<<Pickup.particle<<" transfer\n";
			s<<"n^{+} = "<<n_p<<" #pm "<<er_n_p<<" n^{-} = "<<n_m<<" #pm "<<er_n_m<<endl;//выведем n+ и n- с их ошибками
			s<<"penalty: "<<penalty<<"\n";
			s<<"E_F: "<<Ef<<" #pm "<<Ef_error<<"  keV \n #Delta: "<<Delta<<" #pm "<<Delta_error<<" keV\n";
			s<<"SPE,keV nlj OCC #frac{G^{+}+G^{-}}{2J+1}\n";
			for(unsigned int i=0;i<SPE.size();i++)
			{//cout<< "FitResultsInTextForm2 write result for " << i << " time in s!!!!!\n";
				s<<SPE[i]<<" "<<NLJToString(SP_centroids[i].n,SP_centroids[i].l,SP_centroids[i].JP)<<" "<<OCC[i]<<" "<<ParticlesAndHolesSum[i]<<"\n";//запишем одночастичную энергию, nlj подоболочки, заселённость, сумму частиц и дырок из экспериментов
			}
			//cout<< "FitResultsInTextForm2 has written result in s!!!!!\n";
		}
		return s.str();//вернём строку, где всё сохранили
		//cout<< "ResultsInTextForm returned s and exit!!!!!\n";
	}
}//конец метода ResultsInTextForm
