#include "CoupleOfExperiments.h"

CoupleOfExperiments::CoupleOfExperiments() {
	
}
CoupleOfExperiments::CoupleOfExperiments(Experiment &InpPickup,Experiment &InpStripping){
	Pickup=InpPickup;
	Stripping=InpStripping;
}
void CoupleOfExperiments::BuildPenaltyComponentsHistogram() {
	TH1F result("PFC","Penalty function components",PenaltyComponents.size(),0,PenaltyComponents.size()+1);
	for(unsigned int i=0;i<PenaltyComponents.size();i++) {
		result.SetBinContent(i+1,PenaltyComponents[i]);
		result.GetXaxis()->SetBinLabel(i+1,par.GetComponentName(i).c_str());
	}
	PenaltyComponentsHistogram = result;
}
void CoupleOfExperiments::DrawPenaltyComponentsHistogram(TString opt) {
	if(opt == "logy") {
		PenaltyComponentsHistogram.GetYaxis()->SetRangeUser(10e-4,1);
		gPad->SetLogy(1);
	}
	else PenaltyComponentsHistogram.GetYaxis()->SetRangeUser(0,1);
	PenaltyComponentsHistogram.Draw();
}
void CoupleOfExperiments::GenerateCommonNJPList() {
	cout<<"CoupleOfExperiments::GenerateCommonNJPList() has started!"<<endl;
	par.Cout();
	for(int i=0;i<Pickup.GetNCalculatedLevels();i++) {
		unsigned char flag=0;
		for(int j=0;j<Stripping.GetNCalculatedLevels();j++) {
			if(Pickup[i].Compare(Stripping[j])) {
				StateParameters s(Pickup[i].n,Pickup[i].L,Pickup[i].JP,"both");
				if (par.LimitedSubShellsUsedInDrawing==1) {//если пользователь указывал в файле параметров подоболочки для отрисовки
					//cout<<"Check no "<<i<<":"<<endl;
					s.Cout();
					if(s.CheckIfIncludedIn(par.SubShellsUsedInDrawing)) {
						//cout<<"s is included in par.SubShellsUsedInDrawing!"<<endl;
						s.SetToBeDrawnFlag(1);//средни указанных для отрисовки есть данное состояние, то выставим ему флаг, что его нужно отрисовывать
						//cout<<"s.GetToBeDrawnFlag()=="<<s.GetToBeDrawnFlag()<<endl;
					}
					else  {
						//cout<<"s is NOT included in par.SubShellsUsedInDrawing!"<<endl;
						s.SetToBeDrawnFlag(0);//иначе укажем флаг, что отрисовывать не надо
						//cout<<"s.GetToBeDrawnFlag()=="<<s.GetToBeDrawnFlag()<<endl;
					}
				}
				SP.push_back(s);
				flag=1;
			}
		}
		if((flag==0)&&((par.IncompleteCouplesFlag==2)||(par.IncompleteCouplesFlag==1))) {
			StateParameters s(Pickup[i].n,Pickup[i].L,Pickup[i].JP,"pickup");
			cout<<"Check no "<<i<<":"<<endl;
			s.Cout();
			if(s.CheckIfIncludedIn(par.SubShellsUsedInDrawing)) {
				//cout<<"s is included in par.SubShellsUsedInDrawing!"<<endl;
				s.SetToBeDrawnFlag(1);//средни указанных для отрисовки есть данное состояние, то выставим ему флаг, что его нужно отрисовывать
				//cout<<"s.GetToBeDrawnFlag()=="<<s.GetToBeDrawnFlag()<<endl;
			}
			else {
				//cout<<"s is NOT included in par.SubShellsUsedInDrawing!"<<endl;
				s.SetToBeDrawnFlag(0);//иначе укажем флаг, что отрисовывать не надо
				//cout<<"s.GetToBeDrawnFlag()=="<<s.GetToBeDrawnFlag()<<endl;
			}
			SP.push_back(s);
		}
		
	}//не особо эффективный алгоритм поиска отсутствия совпадений, но сейчас скорость не особо принципиальна
	for(int i=0;i<Stripping.GetNCalculatedLevels();i++) {
		unsigned char flag=0;
		for(int j=0;j<Pickup.GetNCalculatedLevels();j++) {
			if(Pickup[j].Compare(Stripping[i])) {
				flag=1;
			}
		}
		if((flag==0)&&((par.IncompleteCouplesFlag==3)||(par.IncompleteCouplesFlag==1))) {
			StateParameters s(Stripping[i].n,Stripping[i].L,Stripping[i].JP,"stripping");
			//cout<<"Check no "<<i<<":"<<endl;
			s.Cout();
			if (par.LimitedSubShellsUsedInDrawing==1) {//если пользователь указывал в файле параметров подоболочки для отрисовки
				if(s.CheckIfIncludedIn(par.SubShellsUsedInDrawing)) {
					//cout<<"s is included in par.SubShellsUsedInDrawing!"<<endl;
					//cout<<"SP_centroids["<<i<<"].GetToBeDrawnFlag()=="<<SP_centroids[i].GetToBeDrawnFlag()<<endl;
					s.SetToBeDrawnFlag(1);//средни указанных для отрисовки есть данное состояние, то выставим ему флаг, что его нужно отрисовывать
					//cout<<"s.GetToBeDrawnFlag()=="<<s.GetToBeDrawnFlag()<<endl;
				}
				else {
					//cout<<"s is NOT included in par.SubShellsUsedInDrawing!"<<endl;
					s.SetToBeDrawnFlag(0);//иначе укажем флаг, что отрисовывать не надо
					//cout<<"s.GetToBeDrawnFlag()=="<<s.GetToBeDrawnFlag()<<endl;
				}
			}
			SP.push_back(s);
		}		
	}
	cout<<"CoupleOfExperiments::GenerateCommonNJPList() has ended! SP.size()="<<SP.size()<<endl;
}
void CoupleOfExperiments::CalcSPE_and_OCC() {//функция рассчитывает одночастичную энергию, 
//формирует графики для отрисовки и т.д.
	cout<<"CoupleOfExperiments::CalcSPE_and_OCC has started!"<<endl;
	GenerateCommonNJPList();//
	vector<double> OccupanciesForBCSFit;//отдельные векторы заселенностей для аппроксимации БКШ
	vector<double> EnergiesForBCSFit;
	for(int i=0;i<SP.size();i++) {
		double C_pickup=Pickup.GetCentroid(SP[i]);
		double C_stripping=Stripping.GetCentroid(SP[i]);	
		cout<<NLJToString(SP[i].n,SP[i].l,SP[i].JP)<<"C_pickup="<<C_pickup<<" C_stripping="<<C_stripping<<"\n"; 
		if((C_stripping!=-1)&&(C_pickup!=-1)&&(!isnan(C_stripping))&&(!isnan(C_pickup))) {	
			//индусский fix, потом проверить, что генерирует nan
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
			Particles.push_back(Pickup.GetSumSF(SP[i]));
			Holes.push_back(Stripping.GetSumSF(SP[i]));
			SP_centroids.push_back(SP[i]);
			if(par.CheckBelonging(SP[i],par.SubShellsUsedForOccupancyFit)) {
				OccupanciesForBCSFit.push_back(OCC_tmp);//отдельные векторы заселенностей для аппроксимации БКШ
				EnergiesForBCSFit.push_back(SPE_tmp/1000);
			}
		}
		else {
			cout<<"C_stripping or C_pickup is incorrect (not NaN or -1!?)! Add SPE = 0 and OCC = 0!\n"; 
			SPE.push_back(0);
			OCC.push_back(0);
		}
	}
	occupancies=TGraph(OccupanciesForBCSFit.size(),&EnergiesForBCSFit[0],&OccupanciesForBCSFit[0]);
	BCS=TF1("BCS","0.5*(1-(x-[0])/(sqrt((x-[0])^2+[1]^2)))",-50,0);
	BCS.SetParameter(0,-8);
	BCS.SetParameter(1,15);
	float min_E,max_E,min_OCC,max_OCC;
	for(unsigned int i=0;i<OCC.size();i++) {
		//cout<<"SP_centroids["<<i<<"]==";
		//SP_centroids[i].Cout();
		//cout<<"SP_centroids["<<i<<"].GetType()=="<<SP_centroids[i].GetType()<<endl;
		//cout<<"SP_centroids["<<i<<"].GetToBeDrawnFlag()=="<<SP_centroids[i].GetToBeDrawnFlag()<<endl;
		if (SP_centroids[i].GetToBeDrawnFlag()) {
			if(SP_centroids[i].GetType()=="pickup") {
				Pickup_occupancies.SetPoint(Pickup_occupancies.GetN(),SPE[i]/1000,OCC[i]);
			}
			else if(SP_centroids[i].GetType()=="stripping") {
				Stripping_occupancies.SetPoint(Stripping_occupancies.GetN(),SPE[i]/1000,OCC[i]);
			}
			else if(SP_centroids[i].GetType()=="both") {
				Both_occupancies.SetPoint(Both_occupancies.GetN(),SPE[i]/1000,OCC[i]);	
			}
		}
		if(min_E>SPE[i]) min_E=SPE[i];
		if(max_E<SPE[i]) max_E=SPE[i];
		if(min_OCC>OCC[i]) min_OCC=OCC[i];
		if(max_OCC<OCC[i]) max_OCC=OCC[i];
	}
	
	SetTGraphLimits(Pickup_occupancies,min_E,max_E,min_OCC,max_OCC);
	Pickup_occupancies.SetMarkerColor(4);
	SetTGraphLimits(Stripping_occupancies,min_E,max_E,min_OCC,max_OCC);
	Stripping_occupancies.SetMarkerColor(2);
	SetTGraphLimits(Both_occupancies,min_E,max_E,min_OCC,max_OCC);
	Both_occupancies.SetMarkerColor(1);	
	occupancies.SetMarkerStyle(28);
	occupancies.SetMarkerSize(2);
	occupancies.Fit(&BCS,"M");//профитируем функцией в приближении БКШ
	
	Ef=BCS.GetParameter(0);
	Ef_error=BCS.GetParError(0);
	Delta=BCS.GetParameter(1);
	Delta_error=BCS.GetParError(1);
	
	for(int i=0;i<SP.size();i++){//цикл for; для каждой подоболочки:
		Gp_c.push_back(Stripping.GetSumSF(SP[i]));//формируем вектор из сумм СС срыва для каждой подоболочки
		Gm_c.push_back(Pickup.GetSumSF(SP[i]));//формируем вектор из сумм СС подхвата для каждой подоболочки
		er_Gp_c.push_back(Stripping.GetErSumSF(SP[i]));//формируем вектор из ошибок сумм СС срыва для каждой подоболочки
		er_Gm_c.push_back(Pickup.GetErSumSF(SP[i]));//формируем вектор из ошибок сумм СС подхвата для каждой подоболочки
		Gp_alt_c.push_back(Stripping.GetSumSF(SP[i])/(2*abs(SP[i].JP)+1));//формируем вектор из делённых на (2j+1) сумм СС срыва для каждой подоболочки
		Gm_alt_c.push_back(Pickup.GetSumSF(SP[i])/(2*abs(SP[i].JP)+1));//формируем вектор из делённых на (2j+1) сумм СС подхвата для каждой подоболочки
		er_Gp_alt_c.push_back(Stripping.GetErSumSF(SP[i])/(2*abs(SP[i].JP)+1));//формируем вектор из делённых на (2j+1) ошибок сумм СС срыва для каждой подоболочки
		er_Gm_alt_c.push_back(Pickup.GetErSumSF(SP[i])/(2*abs(SP[i].JP)+1));//формируем вектор из делённых на (2j+1) сумм СС подхвата для каждой подоболочки
	}
	cout<<"CoupleOfExperiments::CalcSPE_and_OCC has ended! SP.size()="<<SP.size()<<endl;
}
void CoupleOfExperiments::ClearCalcResults() {
	cout<<"CoupleOfExperiments::ClearCalcResults() has started!"<<endl;
	//очищаем аттрибуты объекта - вектора вычисленных величин:
	//SP.resize(0);
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
string CoupleOfExperiments::ResultsInTextForm(char verbose_level) {
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
void CoupleOfExperiments::DrawResultsInTextForm(string str) {
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

NormalisedCoupleOfExperiments::NormalisedCoupleOfExperiments(){}
NormalisedCoupleOfExperiments::NormalisedCoupleOfExperiments(Experiment &InpPickup, Experiment &InpStripping)
:CoupleOfExperiments(InpPickup, InpStripping) {}
NormalisedCoupleOfExperiments& NormalisedCoupleOfExperiments::operator= (const CoupleOfExperiments &cop) {
	cout<<"NormalisedCoupleOfExperiments operator= CoupleOfExperiments has started!"<<endl;
	Pickup = cop.Pickup;
	cout<<"Pickup = cop.Pickup; finished!"<<endl;
	Stripping = cop.Stripping;//эксперимент срыва
	cout<<"Stripping = cop.Stripping; finished!"<<endl;
	par = cop.par;//считанные пользовательские параметры
	cout<<"par = cop.par; finished!"<<endl;
	SP = cop.SP;//все состояния, которые указаны в паре экспериментов. каждое состояние знает, где он встречалось, в strip, pickup или обоих (?)
	SP_centroids = cop.SP_centroids;//состояния, для которых были вычеслены центроиды C_nlm
	SPE = cop.SPE;//одночастичные энергии
	OCC = cop.OCC;//квадраты заселенностей
	ParticlesAndHolesSum = cop.ParticlesAndHolesSum;//
	Particles = cop.Particles;//
	Holes = cop.Holes;//
	PenaltyComponents = cop.PenaltyComponents;//
	Pickup_size = cop.Pickup_size;//
	Stripping_size = cop.Stripping_size;//
	cout<<"Stripping_size = cop.Stripping_size; finished!"<<endl;
	Gp_c = cop.Gp_c;//вектор спектроскопических сил срыва подоболочек 
	Gm_c = cop.Gm_c;//вектор спектроскопических сил подхвата подоболочек 
	er_Gp_c = cop.er_Gp_c;//вектор ошибок спектроскопических сил срыва подоболочек для расчёта нормированных Gp_norm_c
	er_Gm_c = cop.er_Gm_c;//вектор ошибок спектроскопических сил подхвата подоболочек для расчёта нормированных Gm_norm_c
	Gp_alt_c = cop.Gp_alt_c;//вектор спектроскопических сил  подоболочек для расчёта через МНК, делённых на 2j+1 (они должны быть близки к 1)
	Gm_alt_c = cop.Gm_alt_c;//вектор спектроскопических сил подоболочек для расчёта через МНК, делённых на 2j+1 (они должны быть близки к 1)
	er_Gp_alt_c = cop.er_Gp_alt_c;//вектор ошибок спектроскопических сил подоболочек для расчёта через МНК, делённых на 2j+1
	er_Gm_alt_c = cop.er_Gm_alt_c;//вектор ошибок спектроскопических сил подоболочек для расчёта через МНК, делённых на 2j+1 
	
	points_G = cop.points_G;//объект points_G класса TGraphErrors, точки G+- (спектроскопических сил) в графике
	p_size = cop.p_size;//кол-во точек в графиках ,которые должны использоваться в нормировке
	cout<<"p_size = cop.p_size; finished!"<<endl;
	Ef = cop.Ef;//энергия Ферми, получаемая в фите БКШ
	Delta = cop.Delta ;//\Delta^2//дельта в квадрате, получаемая в фите БКШ
	Ef_error = cop.Ef_error;//ошибка энергии Ферми, получаемая в фите БКШ
	Delta_error = cop.Delta_error;//ошибка дельты в квадрате, получаемая в фите БКШ
	Ef_error_max = cop.Ef_error_max;//максимальная ошибка энергии Ферми, получаемая в фите БКШ
	Delta_error_max = cop.Delta_error_max;	//максимальная ошибка дельты в квадрате, получаемая в фите БКШ
	penalty = cop.penalty;
	cout<<"penalty = cop.penalty;; finished!"<<endl;
	//графики заселённостей состояний от энергии, использованных для фита БКШ:
	occupancies = cop.occupancies;
	Pickup_occupancies = cop.Pickup_occupancies;
	Stripping_occupancies = cop.Stripping_occupancies;
	Both_occupancies = cop.Both_occupancies;
	cout<<"Both_occupancies = cop.Both_occupancies; finished!"<<endl;
	BCS = cop.BCS;//фит заселённостей в зависисмости от энергии (фит БКШ)
	cout<<"BCS = cop.BCS; finished!"<<endl;
	PenaltyComponentsHistogram = cop.PenaltyComponentsHistogram;
	cout<<"PenaltyComponentsHistogram = cop.PenaltyComponentsHistogram; finished!"<<endl;
	PenaltyFunction = cop.PenaltyFunction;//
	cout<<"PenaltyFunction = cop.PenaltyFunction; finished!"<<endl;
	return *this;
}
void NormalisedCoupleOfExperiments::InduceNormalisation() {
	cout<<"NormalisedCoupleOfExperiments::InduceNormalisation() has started!"<<endl;
	NormalisationWasTried=1;
	vector<double> OccupanciesForNormFit;//отдельный вектор заселённостей для
	//аппроксимации нормировки (пока совпадают с набором для аппроксимации БКШ)
	vector<double> EnergiesForNormFit;//отдельный вектор энергий для
	//аппроксимации нормировки (пока совпадают с набором для аппроксимации БКШ)
	cout << "NormalisedCoupleOfExperiments::InduceNormalisation(): par.SubShellsUsedForNormalisation.size() = "<<par.SubShellsUsedForNormalisation.size()<<endl;
	cout << "NormalisedCoupleOfExperiments::InduceNormalisation(): SP.size() = "<<SP.size()<<endl;
	for(int i=0;i<SP.size();i++) {//цикл for; для каждой подоболочки:
		if(par.CheckBelonging(SP[i],par.SubShellsUsedForNormalisation)) {
			///здесь можно заменить на отдельную функцию проверки подоболочки
			///на принадлежность к списку для участия в фите нормировки
			cout << "Got state parameters included in UsedForNormalisation list."<<endl;
			p_size++;
		}
	}
	if ((p_size < 2) || (p_size>SP.size())) {
		//но если точек оказалось меньше двух или точек полагается больше, чем есть подоболочек
		cout << "Note: Wrong number of Normalisation FIT points: "<<p_size<<". Fit will not be performed! Return!"<<endl;
		return;
	}//то возьмём число точек равным числу подооболочек во входных данных
	else{
		cout << "Number of Normalisation FIT points: "<<p_size<<". Fit will be performed! Continue!"<<endl;
	}
	cout << "Creating arrays with points."<<endl;
	//массивы с координатами точек:
	Double_t x[p_size], x2[p_size];//x координаты точек 
	Double_t y[p_size], y2[p_size];//y координаты точек 
	Double_t xe[p_size], xe2[p_size];//ошибки по x 
	Double_t ye[p_size], ye2[p_size];//ошибки по y
    cout << "Filling arrays with points."<<endl;
	for(int i=0;i<SP.size();i++) {//цикл для заполнения координат точек и их ошибок
		if(par.CheckBelonging(SP[i],par.SubShellsUsedForNormalisation)) {
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
		
	if ((n_p < 0.5) || (n_m < 0.5) || (isnan(n_p)) || (isnan(n_m))) {
		//если фит вышел плохой, то n+ и n- не имеют смысла, 
		//так что приравниваем их к 1
		cout<<"	Note: NormalisedCoupleOfExperiments::InduceNormalisation() normalisation procedure has failed!"<<endl;
		cout<<"n_p = "<<n_p<<"; n_m = "<<n_m<<endl;
		n_p = 1.; 
		n_m =1.;
		cout<<"n_p and n_m will be turned into 1!"<<endl;
	}
	else {
		cout<<"	Note: NormalisedCoupleOfExperiments::InduceNormalisation() normalisation procedure has been sucsesfull!"<<endl;
	}	
	er_n_p = 1/2*sqrt((er_fit_a/(fit_a*fit_a))*(er_fit_a/(fit_a*fit_a))+(er_fit_b2/fit_a2)*(er_fit_b2/fit_a2)+(fit_b2/(fit_a2*fit_a2)*er_fit_a2)*(fit_b2/(fit_a2*fit_a2)*er_fit_a2));//находим ошибку первого нормировочного параметра n+ 
	er_n_m = 1/2*sqrt((er_fit_b/fit_a)*(er_fit_b/fit_a)+(fit_b/(fit_a*fit_a)*er_fit_a)*(fit_b/(fit_a*fit_a)*er_fit_a)+er_fit_a2/(fit_a2*fit_a2)*er_fit_a2/(fit_a2*fit_a2));//находим ошибку второго нормировочного параметр n- 
	cout<<"	Normalisation result:"<<endl;
	cout << "n+ = " << n_p << "+/-" << er_n_p << endl;//выводим первый нормировочный параметр n+ 
	cout << "n- = " << n_m << "+/-" << er_n_m << endl;//выводим второй нормировочный параметр n-
	er_fit_a = er_n_p/(n_p*n_p);//
	er_fit_b = sqrt((er_n_p/n_p)*(er_n_p/n_p)+(er_n_m*n_m/n_p/n_p)*(er_n_m*n_m/n_p/n_p));
		
	for(int j=0;j<p_size;j++) {//цикл для заполнения векторов новых координат для нормированных сил
		x2[j] = Gm_alt_c[j]*n_m; 
		y2[j] = Gp_alt_c[j]*n_p;
		xe2[j] = er_Gm_alt_c[j]*n_m; 
		ye2[j] = er_Gp_alt_c[j]*n_p;
	}
	for(int j=0;j<SP.size();j++) {//цикл для заполнения векторов новых, нормированных СС
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
	cout << "SP.size() = "<<SP.size()<<endl;
}
void NormalisedCoupleOfExperiments::ReCalcSPE_and_OCC() {
	cout<<"NormalisedCoupleOfExperiments::ReCalcSPE_and_OCC has started!"<<endl;
	//GenerateCommonNJPList();///эта штука удваивает вектор SP, записывая те же самые состояния
	vector<double> OccupanciesForBCSFit;//отдельные векторы заселенностей для аппроксимации БКШ
	vector<double> EnergiesForBCSFit;
	///применяем расчитанные коэффициенты нормировки n_m, n_p:
	this->ClearCalcResults();
	//рассчитываем величины и заполняем вектора заново:
	for(int i=0;i<SP.size();i++) {//цикл for; для каждой подоболочки:
		double C_pickup=Pickup.GetCentroid(SP[i]);//записываем значение центроида для эксперимента по подхвату в переменную C_pickup
		double C_stripping=Stripping.GetCentroid(SP[i]);//записываем значение центроида для эксперимента по срыву в переменную C_stripping	
		cout<<"C_pickup = "<<C_pickup<<"; C_stripping = "<<C_stripping<<endl;		
		if((C_stripping!=-1)&&(C_pickup!=-1)&&(!isnan(C_stripping))&&(!isnan(C_pickup))) {
			//индусский fix, потом проверить, что генерирует nan
			double E_pickup=-Pickup.BA-C_pickup;//Диплом Марковой М.Л., ф-ла 4//вычисление "одночастичной" для подхвата с использованием энергии отрыва нуклона
			double E_stripping=-Stripping.BA1+C_stripping;//Диплом Марковой М.Л., ф-ла 5//вычисление "одночастичной" для срыва с использованием энергии отрыва нуклона
			double SPE_tmp=(Pickup.GetSumSF(SP[i])*E_pickup*n_m+Stripping.GetSumSF(SP[i])*E_stripping*n_p)
			/(Pickup.GetSumSF(SP[i])*n_m+Stripping.GetSumSF(SP[i])*n_p);//Диплом Марковой М.Л., ф-ла 17 //вычисление одночастичной энергии после нормировки
			double OCC_tmp=(Pickup.GetSumSF(SP[i])*n_m-Stripping.GetSumSF(SP[i])*n_p+2*abs(SP[i].JP)+1)
			/(2*(2*abs(SP[i].JP)+1));//Диплом Марковой М.Л., ф-ла 18 //это v^2_{nlj} после нормировки	
			ParticlesAndHolesSum.push_back((n_m*Pickup.GetSumSF(SP[i])+n_p*Stripping.GetSumSF(SP[i]))
			/(2*abs(SP[i].JP)+1));
			cout<<"SPE_tmp = "<<SPE_tmp<<endl;	
			SPE.push_back(SPE_tmp);//Диплом Марковой М.Л., ф-ла 17
			OCC.push_back(OCC_tmp);//Диплом Марковой М.Л., ф-ла 18
			SP_centroids.push_back(SP[i]);
			if(par.CheckBelonging(SP[i],par.SubShellsUsedForOccupancyFit)) {
				OccupanciesForBCSFit.push_back(OCC_tmp);//отдельные векторы заселенностей для аппроксимации БКШ
				EnergiesForBCSFit.push_back(SPE_tmp/1000);
			}
		}
		else {//если проверка на существование центроидов у экспериментов (индусский fix) не вернёт истину, то
			SPE.push_back(0);//добавляем в вектор одночастичных энергий 0, вместо рассчитанного значения
			OCC.push_back(0);//добавляем в вектор заселённостей 0, вместо рассчитанного значения
		}
	}//конец цикла for
	occupancies=TGraph(OccupanciesForBCSFit.size(),&EnergiesForBCSFit[0],&OccupanciesForBCSFit[0]);
	BCS=TF1("BCS_norm","0.5*(1-(x-[0])/(sqrt((x-[0])^2+[1]^2)))",-50,0);
	BCS.SetParameter(0,-8);
	BCS.SetParameter(1,15);
	float min_E,max_E,min_OCC,max_OCC;
	for(unsigned int i=0;i<OCC.size();i++) {
		if (SP_centroids[i].GetToBeDrawnFlag()) {	
			if(SP_centroids[i].GetType()=="pickup") {
				Pickup_occupancies.SetPoint(Pickup_occupancies.GetN(),SPE[i]/1000,OCC[i]);
			}
			else if(SP_centroids[i].GetType()=="stripping") {
				Stripping_occupancies.SetPoint(Stripping_occupancies.GetN(),SPE[i]/1000,OCC[i]);
			}
			else if(SP_centroids[i].GetType()=="both") {
				Both_occupancies.SetPoint(Both_occupancies.GetN(),SPE[i]/1000,OCC[i]);
			}
		}	
		if(min_E>SPE[i]) min_E=SPE[i];
		if(max_E<SPE[i]) max_E=SPE[i];
		if(min_OCC>OCC[i]) min_OCC=OCC[i];
		if(max_OCC<OCC[i]) max_OCC=OCC[i];
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
	
	for(int i=0;i<SP.size();i++) {//цикл for; для каждой подоболочки:
		Gp_c.push_back(Stripping.GetSumSF(SP[i]));//формируем вектор из сумм СС срыва для каждой подоболочки
		Gm_c.push_back(Pickup.GetSumSF(SP[i]));//формируем вектор из сумм СС подхвата для каждой подоболочки
		er_Gp_c.push_back(Stripping.GetErSumSF(SP[i]));//формируем вектор из ошибок сумм СС срыва для каждой подоболочки
		er_Gm_c.push_back(Pickup.GetErSumSF(SP[i]));//формируем вектор из ошибок сумм СС подхвата для каждой подоболочки
		Gp_alt_c.push_back(Stripping.GetSumSF(SP[i])/(2*abs(SP[i].JP)+1));//формируем вектор из делённых на (2j+1) сумм СС срыва для каждой подоболочки
		Gm_alt_c.push_back(Pickup.GetSumSF(SP[i])/(2*abs(SP[i].JP)+1));//формируем вектор из делённых на (2j+1) сумм СС подхвата для каждой подоболочки
		er_Gp_alt_c.push_back(Stripping.GetErSumSF(SP[i])/(2*abs(SP[i].JP)+1));//формируем вектор из делённых на (2j+1) ошибок сумм СС срыва для каждой подоболочки
		er_Gm_alt_c.push_back(Pickup.GetErSumSF(SP[i])/(2*abs(SP[i].JP)+1));//формируем вектор из делённых на (2j+1) сумм СС подхвата для каждой подоболочки
	}
	cout<<"NormalisedCoupleOfExperiments::ReCalcSPE_and_OCC has ended!"<<endl;
}
string NormalisedCoupleOfExperiments::FitResultsInTextForm(char verbose_level) {
	//функция записывает в строку результаты нормировки и принятые параметры (строка оказывается слева внизу каждого выходного pdf файла)
	cout<<"FitResultsInTextForm has started!"<<endl;
	stringstream s;//задаём строку, куда всё будем сохранять
	if(verbose_level==0){
		s<<"Experiment: "<<Pickup.reference<<" ("<<Pickup.size()<<") "
		<<Stripping.reference<<" ("<<Stripping.size()<<") \n";
	}
	else if(verbose_level==1){
		s<<"Experiment(pickup): "<<Pickup.reference<<" ("<<Pickup.size()<<")\n";
		s<<Pickup.ChangesLog<<"\n";
		s<<"Experiment(stripping): "<<Stripping.reference<<" ("<<Stripping.size()<<")\n";
		s<<Stripping.ChangesLog<<"\n";
	}
	unsigned int k=0;
	//укажем вид уравнений, которые потом преобразовывались для МНК	
	s<<"n^{+}G*^{+} + n^{-}G*^{-} = 1 equations:\n";
	cout<<"Will draw n^{+}G*^{+} + n^{-}G*^{-} = 1 equations"<<endl;
	cout<<"SPE.size() = "<<SPE.size()<<endl;
	cout<<"SP_centroids.size() = "<<SP_centroids.size()<<endl;
	for(unsigned int i=0;i<SP_centroids.size();i++){
		//выведем все уравнения для подоболочек, которые мы должны были использовать для нахождения n+ и n-:
		if(par.CheckBelonging(SP[i],par.SubShellsUsedForNormalisation)){	
			k++;
			//cout<<"Got n="<<SP_centroids[i].n<<"; l="<<SP_centroids[i].l<<"; JP="<<SP_centroids[i].JP<<endl;
			s<<NLJToString(SP_centroids[i].n,SP_centroids[i].l,SP_centroids[i].JP)<<": n^{+} "
			<<TString::Format("%1.2f",Gp_alt_c[i])<<" +"<<" n^{-} "<<TString::Format("%1.2f",Gm_alt_c[i])<<" = 1\n";
			//cout<<"Drawing data for "<<NLJToString(SP_centroids[i].n,SP_centroids[i].l,SP_centroids[i].JP)<<endl;
		}
	}
	if ((n_p==1.)&&(n_m==1.))//если нормировочные коэффициенты равны 1, то логично, что нормировки не происходило
	{
		s<<"Normalization was not perfomed.\n";
	}//сообщим об этом в .pdf файле
	else
	{
		s<<"Normalization of "<< k <<" subshells result:\n";//в этой строке укажем параметр, который ставили для ограничения числа точек в фите
		s<<"n^{+} = "<<TString::Format("%2.2f",n_p)<<" #pm "
		<<TString::Format("%2.2f",er_n_p)<<" (for stripping)\n";//выведем n+ с его ошибкой
		s<<"n^{-} = "<<TString::Format("%2.2f",n_m)<<" #pm "
		<<TString::Format("%2.2f",er_n_m)<<" (for pick-up)\n";//выведем n- с его ошибкой
		for(unsigned int i=0;i<SPE.size();i++)
		{//запишем получившиеся после нормировки спектроскопические силы, они должны быть ближе к ОМО, ради этого всё затевалось
			if(par.CheckBelonging(SP[i],par.SubShellsUsedInDrawing))
			{
				s<<NLJToString(SP_centroids[i].n,SP_centroids[i].l,SP_centroids[i].JP)<<" G^{+},G^{-}: "<<
				TString::Format("%2.2f",Gp_c[i]/n_p)<<" -> "<<TString::Format("%2.2f",Gp_c[i])<<", "<<
				TString::Format("%2.2f",Gm_c[i]/n_m)<<" -> "<<TString::Format("%2.2f",Gm_c[i])<<"\n";
			}
		}
	}
	return s.str();//вернём строку, где всё сохранили
}//конец метода FitResultsInTextForm
string NormalisedCoupleOfExperiments::ResultsInTextForm(char verbose_level) {//функция записывает в строку результаты и принятые параметры (строка оказывается слева внизу каждого выходного pdf файла)
	if(NormalisationWasTried==0){
		return CoupleOfExperiments::ResultsInTextForm(verbose_level);
	}
	else {
		stringstream s;//задаём строку, куда всё будем сохранять
		if(verbose_level==0) {
			s<<"Experiment: "<<Pickup.reference<<" ("<<Pickup.size()<<") "<<Stripping.reference<<" ("
			<<Stripping.size()<<") \n";
		}
		else if(verbose_level==1) {
			s<<"Experiment(pickup): "<<Pickup.reference<<" ("<<Pickup.size()<<")\n";
			s<<Pickup.ChangesLog<<"\n";
			s<<"Experiment(stripping): "<<Stripping.reference<<" ("<<Stripping.size()<<")\n";
			s<<Stripping.ChangesLog<<"\n";
		}
		if ((n_p==1.)&&(n_m==1.)) {
			//если нормировочные колэффициенты равны 1, то логично, что нормировки не происходило	
			s<<"penalty: "<<penalty<<"\n";
			s<<"Normalization was not perfomed.\n";
		}//сообщим об этом в .pdf файле
		else {	
			s<<Pickup.particle<<" transfer\n";
			//выведем n+ и n- с их ошибками:
			s<<"n^{+} = "<<TString::Format("%0.2f",n_p)<<" #pm "<<TString::Format("%0.2f",er_n_p)
			<<" n^{-} = "<<TString::Format("%0.2f",n_m)<<" #pm "<<TString::Format("%0.2f",er_n_m)<<endl;
			s<<"penalty: "<<penalty<<"\n";
			s<<"E_F: "<<Ef<<" #pm "<<Ef_error<<"  keV \n #Delta: "<<Delta<<" #pm "<<Delta_error<<" keV\n";
			s<<"SPE,keV nlj OCC #frac{G^{+}+G^{-}}{2J+1}\n";
			for(unsigned int i=0;i<SPE.size();i++)
			{//запишем одночастичную энергию, nlj подоболочки, заселённость,
				//сумму частиц и дырок из экспериментов:
				if(par.CheckBelonging(SP[i],par.SubShellsUsedInDrawing))
				{
					s<<SPE[i]<<" "<<NLJToString(SP_centroids[i].n,SP_centroids[i].l,SP_centroids[i].JP)<<" "
					<<OCC[i]<<" "<<ParticlesAndHolesSum[i]<<"\n";
				}
			}
		}
		return s.str();//вернём строку, где всё сохранили
	}
}//конец метода ResultsInTextForm
