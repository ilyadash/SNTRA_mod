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
			Histograms[j].SetTitle((Reference+";E, Mev;G").c_str());
			Histograms[j].Draw("histo");
			Legend->AddEntry(&Histograms[j],NLJToString(n[j],L[j], JP[j]).c_str(),"f");
			Histograms[j].GetXaxis()->SetRangeUser(10e-9,maximum_x*1.2);
			Histograms[j].GetYaxis()->SetRangeUser(10e-9,maximum_y*1.2);
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
	cout<<"parameters::ReadParameters has for"<<filename<<" file!"<<"\n";
	ifstream ifs(filename.c_str());
	string line;//переменная считываемой строки
	while(getline(ifs,line))//для каждой строки в файле на диске
	{
		//cout<<"before line = "<<line<<endl;
		TString Tline=TString(line);
		if(Tline.Contains("#")) line=line.substr(0,Tline.First('#'));
		//cout<<"after line = "<<line<<endl;
		stringstream s(line);
		TString tmp;
		s>>tmp;
		//cout<<"tmp = "<<tmp<<endl;
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
				if(tmp2=="NumberOfParticlesInUsedShell")
				{
					UsedPenaltyFunctionComponents.push_back(6);
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
				//cout<<"tmp2 = "<<tmp2<<endl;
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
				//cout<<"tmp2 = "<<tmp2<<endl;
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
				//cout<<"tmp2 = "<<tmp2<<endl;
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

void Experiment::ReadInputFile(string filename)//просто чтение текстового файла с данными. 
{//Сначала поиск ключевых слов, если их нет, то попытка считать строку как состояние, наблюдаемое в эксперименте
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
	SFHistograms.maximum_x=1;
	SFHistograms.maximum_y=1;
	TString name_buffer=" ";
	if (norma!=1) name_buffer=" norm. ";
	SFHistograms.Reference=reference+name_buffer+GetType();
	for(unsigned int i=0;i<SSD.size();i++)
	{//для каждого состояния в объекте SSD (SummarizedSpectroscopicData)
		string name=reference+sprintf("_%d_%d_%d",SSD.States[i].n,SSD.States[i].L,(int)(SSD.States[i].JP*2));
		TH1F histogram(name.c_str(),name.c_str(),100,0,10);
		for(unsigned int j=0;j<States.size();j++)
		{
			if((States[j].n[0]==SSD.States[i].n)&&(States[j].L[0]==SSD.States[i].L)
			&&(States[j].JP[0]==SSD.States[i].JP))
			{
				histogram.SetBinContent(histogram.GetXaxis()->FindFixBin(States[j].Energy/1000),
				States[j].G()*norma);//строим столбик высотой с нормированную спектроскопическую силу
			}
		}
		histogram.SetLineColor(GetColor(SSD.States[i].L, SSD.States[i].JP));
		histogram.SetFillColor(GetColor(SSD.States[i].L, SSD.States[i].JP));
		if(histogram.GetMaximum()>SFHistograms.maximum_y)
		{
			SFHistograms.maximum_y=histogram.GetMaximum();
		}
		if(histogram.GetBinCenter(histogram.FindLastBinAbove(0))>SFHistograms.maximum_x)
		{
			//cout<<"Before: SFHistograms.maximum_x = "<<SFHistograms.maximum_x<<endl;
			SFHistograms.maximum_x=histogram.GetBinCenter(histogram.FindLastBinAbove(0));
			//cout<<"After: SFHistograms.maximum_x = "<<SFHistograms.maximum_x<<endl;
		} 
		SFHistograms.Histograms.push_back(histogram);
		SFHistograms.n.push_back(SSD.States[i].n);
		SFHistograms.L.push_back(SSD.States[i].L);
		SFHistograms.JP.push_back(SSD.States[i].JP);
	}
	return SFHistograms;//возвращает результирующую гистограмму
}//конец метода BuildSpectroscopicFactorHistogram()

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
								exp_tmp.ChangesLog+="ver "+to_string(version_iterator)+": used state "+
								TString::Format("%0.1f",exp_tmp.States[index].Energy)+" keV "+
								NLJToString(exp_tmp.States[index],0)+" SF:"+
								TString::Format("%0.3f",exp_tmp.States[index].SpectroscopicFactor[0]);
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
