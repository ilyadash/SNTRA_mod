#include "SetOfExpCouples.h"
#include "TLine.h"

void SetOfExpCouples::CreateCouplesOfExperiments(vector<Experiment> &Pickup,
vector<Experiment> &Stripping, parameters &par) {
	//функция создаёт вектор всех вариантов пар экспириментов срыв-подхват
	//(вектор объектов CoupleOfExperiments);
	//функции на вход подаются вектор всех экспериментов срыва и вектор всех
	//экспериментов подхвата (их адреса?)
	cout<<"void SetOfExpCouples::CreateCouplesOfExperiments has started!"<<endl;
	for(unsigned int i=0;i<Pickup.size();i++) {
		cout<<"Got number "<<i+1<<" pickup "<<Pickup[i].reference<<"\n";
		for(unsigned int j=0;j<Stripping.size();j++) {
			cout<<"Got number "<<j+1<<" stripping "<<Stripping[j].reference<<"\n";
			CoupleOfExperiments CE(Pickup[i],Stripping[j]);
			CE.par=par;
			data.push_back(CE);
		}
	}
	cout<<"CreateCouplesOfExperiments has ended!"<<endl;
}//конец функции vector<CoupleOfExperiments> CreateCouplesOfExperiments
///перебор по возрастанию штрафной функции не изменялся,
///хотя мог быть изменён для ранжирования по успешности нормировки СС:
void SetOfExpCouples::ArrangeByPenalty() {
	//функция меняяет соседние в векторе пары экспериментов, до тех пор,
	//пока они не будут отранжированы по возрастанию функции ошибок
	//функция сортирует пары экспериментов в поданном векторе по убыванию соответсвующей штрафной функции
	//двойной перебор нужен чтобы сравнить каждый элемент вектора с каждым, происходит сортировка
	//по значению штрафной функции
	cout<<"void SetOfExpCouples::ArrangeByPenalty() has started!"<<"\n";
	for(unsigned int i=0;i<data.size();i++) {//для каждого элемента i массива
		int NumberOfExcanges=0;//переменная для числа перестановок
		for(unsigned int j=0;j<data.size()-i-1;j++)//для каждого элемента массива j, который имеет номер в векторе меньше i
		{
			if(data[j].penalty>data[j+1].penalty)//если функция ошибок пары экспериментов j больше, чем у пары j+1
			{//стандартные действия для смены мест двух соседних эелементов массива/вектора
				CoupleOfExperiments tmp=data[j];//создаём буферную пару для перемещения пар j и j+1, равную паре j, чтобы её сохранить
				data[j]=data[j+1];//сохраняем в j пару j+1
				data[j+1]=tmp;//сохраняем в j+1 буфер, который равен паре j
				NumberOfExcanges++;//считаем число перестановок, увеличивая каждый раз NumberOfExcanges на 1
			}
		}
		if(NumberOfExcanges==0)//если не было совершенно перестановок, NumberOfExcanges осталось равной 0
		{
			return;//то функция просто ничего не делает
		}
	}
}//конец void ArrangeByPenalty
void SetOfExpCouples::CalcSPE_and_OCC() {
	cout<<"void SetOfExpCouples::CalcSPE_and_OCC() has started!"<<"\n";
	cout<<"data.size() = "<<data.size()<<"\n";
	for(unsigned int i=0;i<data.size();i++) {//для каждой пары срыв-подхват в векторе CE
		data[i].CalcSPE_and_OCC();
	}
}
void SetOfExpCouples::CalculatePenaltyFunction() {//функция для расчёта штрафной функции
cout<<"void SetOfExpCouples::CalculatePenaltyFunction() has started!"<<"\n";
	float MaxEfError,MaxDeltaError;
	int NumberOfPickupStatesMax=0, NumberOfStrippingStatesMax=0, AverageNumberOfCalculatedStates=0;
	for(int i=0;i<data.size();i++) {
		//определяем в этом цикле максималные ошибки, кол-ва состояний,
		//для дальнейшей оценки каждой пары относительно максимума
		if(MaxEfError<data[i].Ef_error){
			MaxEfError=data[i].Ef_error;
		}
		if(MaxDeltaError<data[i].Delta_error){
			MaxDeltaError=data[i].Delta_error;
		}
		if(NumberOfPickupStatesMax<data[i].Pickup.size()){//GetNCalculatedLevels())
			//NumberOfPickupStatesMax=data[i].Pickup.GetNCalculatedLevels();
			NumberOfPickupStatesMax=data[i].Pickup.size();
		}
		if(NumberOfStrippingStatesMax<data[i].Stripping.size()){//GetNCalculatedLevels())
			NumberOfStrippingStatesMax=data[i].Stripping.size();//)//GetNCalculatedLevels();
		}
		AverageNumberOfCalculatedStates+=data[i].SPE.size();
	}
	if(data.size()>0)
	AverageNumberOfCalculatedStates=round(AverageNumberOfCalculatedStates/data.size());
	else{
		cerr<<"	*** Error! CalculatePenaltyFunction() got empty input vector!"<<"\n";
		return;
	}
	for(unsigned int i=0;i<data.size();i++) {
		data[i].PenaltyComponents.resize(0);
		for(unsigned int j=0;j<data[i].par.UsedPenaltyFunctionComponents.size();j++) {
			if(data[i].par.UsedPenaltyFunctionComponents[j]==1){//отклонение измеренного числа вакансий на подоболочах от ОМО
				data[i].PenaltyComponents.push_back(abs(1-Average(data[i].ParticlesAndHolesSum)));
			}
			else if(data[i].par.UsedPenaltyFunctionComponents[j]==2) {
				data[i].PenaltyComponents.push_back(1-((float)data[i].Pickup.size()/NumberOfPickupStatesMax));
			}
			else if(data[i].par.UsedPenaltyFunctionComponents[j]==3) {
				data[i].PenaltyComponents.push_back(1-((float)data[i].Stripping.size()/NumberOfStrippingStatesMax));
			}
			else if(data[i].par.UsedPenaltyFunctionComponents[j]==4) {
				if((MaxEfError!=0)&&(MaxDeltaError!=0)){
					data[i].PenaltyComponents.push_back(data[i].Ef_error/MaxEfError);
				}
			}
			else if(data[i].par.UsedPenaltyFunctionComponents[j]==5) {
				if((MaxEfError!=0)&&(MaxDeltaError!=0)){
					data[i].PenaltyComponents.push_back(data[i].Delta_error/MaxDeltaError);
				}
			}
			else if(data[i].par.UsedPenaltyFunctionComponents[j]==6) {
				if(data[i].par.NParticlesInShell>0){
					data[i].PenaltyComponents.push_back(abs(1-Sum(data[i].Particles))/data[i].par.NParticlesInShell);
				}
			}
			else if(data[i].par.UsedPenaltyFunctionComponents[j]==7) {
				if(data[i].par.NHolesInShell>0){
					data[i].PenaltyComponents.push_back(abs(1-Sum(data[i].Holes))/data[i].par.NHolesInShell);
				}
			}
		}
		for(unsigned int j=0;j<data[i].PenaltyComponents.size();j++) {
			data[i].PenaltyComponents[j]=(data[i].PenaltyComponents[j]+1/data[i].PenaltyComponents.size())*
			data[i].par.weights[j]/Sum(data[i].par.weights);///to do: продумать формулу для применения весов штрафной функции
		}
		for(unsigned int j=0;j<data[i].PenaltyComponents.size();j++) {
			data[i].penalty+=data[i].PenaltyComponents[j];
		}
		//data[i].penalty=data[i].penalty/data[i].PenaltyComponents.size();
	}
	
}//конец void CalculatePenaltyFunction
void SetOfExpCouples::PrintCalculationResult(string OutputFileName, string output_dir) {
	//на вход подаётся вектор пар экспериментов и название выходных
	//файлов .pdf .txt OutputFileName(вектор объектов CoupleOfExperiments),
	//функция записывает результаты нормировки в выходные файлы .txt и .pdf
	cout<<"void SetOfExpCouples::PrintCalculationResult() has started!"<<"\n";
	OutputFileName=string(output_dir)+"/"+OutputFileName;
	ofstream OutputTextFile((OutputFileName+".txt").c_str());//создаём .txt файл с выходными данными
	cc1->Print((OutputFileName+".pdf[").c_str(),"pdf");//создаём .pdf файл с выходными данными, который сейчас будем наполнять графиками и текстом
	for(unsigned int i=0;i<data.size();i++) {//для каждой пары экспериментов во входном векторе v
		SpectroscopicFactorHistogram HistPickup=data[i].Pickup.BuildSpectroscopicFactorHistogram(data[i].n_m);//создаём гистограммы нормированных спектроскопических факторов, которые будем отрисовывать на холсте
		SpectroscopicFactorHistogram HistStrip=data[i].Stripping.BuildSpectroscopicFactorHistogram(data[i].n_p);//для срыва и подхвата, всего 2 гистограммы, итого 2 гистограммы на холсте
		cc1->Clear();//Delete all pad primitives
		cc1->Divide(3,2);//разделить Pad на 3 независимые области по вертикали и на 2 по горизонтали (всего 6 областей)

		cc1->cd(1);//переходим к Pad1
		HistPickup.PrintSpectroscopicFactorHistogram();//рисуем гистограмму для эксперимента подхвата
		
		cc1->cd(2);//переходим к Pad2
		TMultiGraph* mgr=new TMultiGraph();
		//data[i].occupancies.SetTitle("Occupancy;E,keV;v^2");
		mgr->Add(&data[i].Pickup_occupancies);
		mgr->Add(&data[i].Stripping_occupancies);
		mgr->Add(&data[i].Both_occupancies);
		mgr->SetTitle("Occupancy with BCS fit; E, MeV; v^{2}");
		mgr->Draw("ap");
		data[i].occupancies.Draw("p same");//отрисуем заселённости, которые использовались в фите БКШ поверх остальных (выделим их крестами)
		data[i].BCS.Draw("l same");//отрисуем кривую фита БКШ на том же Pad
		mgr->SetTitle("Occupancy with BCS fit; E, MeV; v^{2}");
		gPad->Modified();
		gPad->Update();
		cc1->cd(3);
		data[i].BuildPenaltyComponentsHistogram();
		data[i].DrawPenaltyComponentsHistogram("logy");
		cc1->cd(4);//переходим к Pad4
		//gPad->SetLogy(1);
		HistStrip.PrintSpectroscopicFactorHistogram();//рисуем гистограмму для эксперимента срыва
		string TextOutput=data[i].ResultsInTextForm(1);
		stringstream s(TextOutput);
		OutputTextFile<<TextOutput<<"\n";//записывем в текстовый файл результаты расчёта
		cc1->cd(5);//переходим к Pad5
		TGraph* gr=new TGraph();//"h1","Calculated shell scheme;1 ;E, keV",10,0,1);
		gr->SetTitle("Calculated shell scheme;  ;E, keV");
		gr->SetPoint(0,0,0);
		gr->SetMinimum(GetMinimum(data[i].SPE)/1000-1);
		gr->SetMaximum(GetMaximum(data[i].SPE)/1000*1.1);
		gr->Draw("ap");
		for(unsigned int j=0;j<data[i].SPE.size();j++){
			TLine line;
			TText txt;
			int color=data[i].SP_centroids[j].GetColor();
			line.SetLineColor(color);
			if(data[i].SP_centroids[j].GetToBeDrawnFlag()==1) {
				line.DrawLine(0.1,data[i].SPE[j]/1000,0.7,data[i].SPE[j]/1000);
				txt.SetTextColor(color);
				txt.DrawText(0.8,data[i].SPE[j]/1000, data[i].SP_centroids[j].GetNLJ().Data());
			}
		}

		cc1->cd(6);//переходим к Pad6
		data[i].DrawResultsInTextForm(data[i].ResultsInTextForm());
		cc1->Print((OutputFileName+".pdf").c_str(),"pdf");
		gPad->Update();
	}
	cc1->Print((OutputFileName+".pdf]").c_str(),"pdf");
	cout<<"void PrintCalculationResult has ended!"<<"\n";
}

void SetOfNormalisedExpCouples::CreateNormalisedCouplesOfExperiments(vector<Experiment> &Pickup,
vector<Experiment> &Stripping, parameters &par) {
	//функция создаёт вектор всех вариантов пар экспириментов срыв-подхват (вектор объектов CoupleOfExperiments);
	//функции на вход подаются вектор всех экспериментов срыва и вектор всех экспериментов подхвата (их адреса?)
	cout<<"void SetOfNormalisedExpCouples::CreateNormalisedCouplesOfExperiments has started!"<<endl;
	if (Pickup.size()==0) cerr<<"	*** Error! There are no Pickup experiments!"<<endl;
	for(unsigned int i=0;i<Pickup.size();i++)
	{
		cout<<"Got number "<<i+1<<" pickup "<<Pickup[i].reference<<"\n";
		for(unsigned int j=0;j<Stripping.size();j++)
		{
			cout<<"Got number "<<j+1<<" stripping "<<Stripping[j].reference<<"\n";
			NormalisedCoupleOfExperiments CE(Pickup[i],Stripping[j]);
			CE.par=par;
			data.push_back(CE);
		}
	}
	cout<<"data.size() = "<<data.size()<<"\n";
	cout<<"void SetOfNormalisedExpCouples::CreateNormalisedCouplesOfExperiments has ended!"<<endl;
}
void SetOfNormalisedExpCouples::CalcSPE_and_OCC() {
	cout<<"void SetOfNormalisedExpCouples::CalcSPE_and_OCC() has started!"<<"\n";
	cout<<"data.size() = "<<data.size()<<"\n";
	for(unsigned int i=0;i<data.size();i++) {//для каждой пары срыв-подхват в векторе CE
		data[i].CalcSPE_and_OCC();
	}
}
void SetOfNormalisedExpCouples::CalculatePenaltyFunction() {//функция для расчёта штрафной функции
cout<<"void SetOfNormalisedExpCouples::CalculatePenaltyFunction() has started!"<<"\n";
	float MaxEfError,MaxDeltaError;
	int NumberOfPickupStatesMax=0, NumberOfStrippingStatesMax=0, AverageNumberOfCalculatedStates=0;
	for(int i=0;i<data.size();i++)
	{//определяем в этом цикле максималные ошибки, кол-ва состояний, для дальнейшей оценки каждой пары относительно максимума
		if(MaxEfError<data[i].Ef_error)
		{
			MaxEfError=data[i].Ef_error;
		}
		if(MaxDeltaError<data[i].Delta_error)
		{
			MaxDeltaError=data[i].Delta_error;
		}
		if(NumberOfPickupStatesMax<data[i].Pickup.size())//GetNCalculatedLevels())
		{
			//NumberOfPickupStatesMax=data[i].Pickup.GetNCalculatedLevels();
			NumberOfPickupStatesMax=data[i].Pickup.size();
		}
		if(NumberOfStrippingStatesMax<data[i].Stripping.size())//GetNCalculatedLevels())
		{
			NumberOfStrippingStatesMax=data[i].Stripping.size();//)//GetNCalculatedLevels();
		}
		AverageNumberOfCalculatedStates+=data[i].SPE.size();
	}
	if(data.size()>0)
	AverageNumberOfCalculatedStates=round(AverageNumberOfCalculatedStates/data.size());
	else{
		cerr<<"	*** Error! CalculatePenaltyFunction() got empty input vector!"<<"\n";
		return;
	};
	
	for(unsigned int i=0;i<data.size();i++){
		data[i].PenaltyComponents.resize(0);
		//cout<<"data[i].PenaltyComponents.size() = "<<data[i].PenaltyComponents.size()<<"\n";
		//cout<<"size to be = "<<data[i].par.UsedPenaltyFunctionComponents.size()<<"\n";
		for(unsigned int j=0;j<data[i].par.UsedPenaltyFunctionComponents.size();j++){
			//cout<<"comp "<<(int)data[i].par.UsedPenaltyFunctionComponents[j]<<"\n";
			if(data[i].par.UsedPenaltyFunctionComponents[j]==1) {
				data[i].PenaltyComponents.push_back(abs(1-Average(data[i].ParticlesAndHolesSum)));
			}
			else if(data[i].par.UsedPenaltyFunctionComponents[j]==2) {
				data[i].PenaltyComponents.push_back(1-((float)data[i].Pickup.size()/NumberOfPickupStatesMax));
			}
			else if(data[i].par.UsedPenaltyFunctionComponents[j]==3) {
				data[i].PenaltyComponents.push_back(1-((float)data[i].Stripping.size()/NumberOfStrippingStatesMax));
			}
			else if(data[i].par.UsedPenaltyFunctionComponents[j]==4) {
				if((MaxEfError!=0)&&(MaxDeltaError!=0)) {
					data[i].PenaltyComponents.push_back(data[i].Ef_error/MaxEfError);
				}
			}
			else if(data[i].par.UsedPenaltyFunctionComponents[j]==5) {
				if((MaxEfError!=0)&&(MaxDeltaError!=0)) {
					data[i].PenaltyComponents.push_back(data[i].Delta_error/MaxDeltaError);
				}
			}
		}
		for(unsigned int j=0;j<data[i].PenaltyComponents.size();j++) {
			data[i].PenaltyComponents[j]=(data[i].PenaltyComponents[j]+1/data[i].PenaltyComponents.size())*
			data[i].par.weights[j];///to do: продумать формулу для применения весов штрафной функции
		}
		for(unsigned int j=0;j<data[i].PenaltyComponents.size();j++) {
			data[i].penalty+=data[i].PenaltyComponents[j];
		}
		//data[i].penalty=data[i].penalty/data[i].PenaltyComponents.size();
	}
	
}//конец void CalculatePenaltyFunction
void SetOfNormalisedExpCouples::PrintCalculationResult(string OutputFileName, string output_dir) {
//на вход подаётся вектор пар экспериментов и название выходных файлов .pdf .txt OutputFileName(вектор объектов CoupleOfExperiments), функция записывает результаты нормировки в выходные файлы .txt и .pdf
	cout<<"void SetOfNormalisedExpCouples::PrintCalculationResult has started!"<<"\n";
	OutputFileName=string(output_dir)+"/"+OutputFileName;
	ofstream OutputTextFile((OutputFileName+".txt").c_str());//создаём .txt файл с выходными данными
	cc1->Print((OutputFileName+".pdf[").c_str(),"pdf");//создаём .pdf файл с выходными данными, который сейчас будем наполнять графиками и текстом
	for(unsigned int i=0;i<data.size();i++)//для каждой пары экспериментов во входном векторе v
	{	//cout<< "I started pdf writing for the pair!!!!\n";
		TMultiGraph* mgr=new TMultiGraph();
		SpectroscopicFactorHistogram HistPickup=data[i].Pickup.BuildSpectroscopicFactorHistogram(data[i].n_m);//создаём гистограммы нормированных спектроскопических факторов, которые будем отрисовывать на холсте
		SpectroscopicFactorHistogram HistStrip=data[i].Stripping.BuildSpectroscopicFactorHistogram(data[i].n_p);//для срыва и подхвата, всего 2 гистограммы, итого 2 гистограммы на холсте
		cc1->Clear();//Delete all pad primitives
		cc1->Divide(3,2);//разделить Pad на 3 независимые области по вертикали и на 2 по горизонтали (всего 6 областей)
		cc1->cd(1);//переходим к Pad1
		HistPickup.PrintSpectroscopicFactorHistogram();//рисуем гистограмму для эксперимента подхвата
		cc1->cd(2);//переходим к Pad2
		data[i].occupancies.SetTitle("Occupancy with BCS fit;E, MeV;v^2");
		//mgr->SetTitle("Occupancy;E, keV;v^{2}");
		mgr->Add(&data[i].Pickup_occupancies);
		mgr->Add(&data[i].Stripping_occupancies);
		mgr->Add(&data[i].Both_occupancies);
		mgr->Draw("ap");
		data[i].occupancies.Draw("p same");//отрисуем заселённости, которые использовались в фите БКШ поверх остальных (выделим их крестами)
		data[i].BCS.Draw("l same");//отрисуем кривую фита БКШ на том же Pad
		mgr->SetTitle("Occupancy with BCS fit; E, MeV; v^{2}");
		gPad->Modified();
		gPad->Update();
		cc1->cd(3);
		data[i].BuildPenaltyComponentsHistogram();
		data[i].DrawPenaltyComponentsHistogram("logy");
		cc1->cd(4);//переходим к Pad4
		//gPad->SetLogy(1);
		HistStrip.PrintSpectroscopicFactorHistogram();//рисуем гистограмму для эксперимента срыва
		string TextOutput=data[i].ResultsInTextForm(1);
		stringstream s(TextOutput);
		OutputTextFile<<TextOutput<<"\n";//записывем в текстовый файл результаты расчёта
		cc1->cd(5);//переходим к Pad5
		TGraph* gr=new TGraph();//"h1","Calculated shell scheme;1 ;E, keV",10,0,1);
		gr->SetTitle("Calculated shell scheme;  ;E, MeV");
		gr->SetPoint(0,0,0);
		gr->SetMinimum(GetMinimum(data[i].SPE)/1000-1);
		gr->SetMaximum(GetMaximum(data[i].SPE)/1000*1.1);
		gr->Draw("ap");
		for(unsigned int j=0;j<data[i].SPE.size();j++)
		{
			TLine line;
			TText txt;
			int color=data[i].SP_centroids[j].GetColor();
			line.SetLineColor(color);
			if(data[i].SP_centroids[j].GetToBeDrawnFlag()==1)
			{
				line.DrawLine(0.1,data[i].SPE[j]/1000,0.7,data[i].SPE[j]/1000);
				txt.SetTextColor(color);
				txt.DrawText(0.8,data[i].SPE[j]/1000, data[i].SP_centroids[j].GetNLJ().Data());
			}
		}
		cc1->cd(6);//переходим к Pad6
		data[i].DrawResultsInTextForm(data[i].ResultsInTextForm());
		cc1->Print((OutputFileName+".pdf").c_str(),"pdf");
		gPad->Update();
	}
	cc1->Print((OutputFileName+".pdf]").c_str(),"pdf");
	cout<<"void PrintCalculationResult has ended!"<<"\n";
}
//*/
void SetOfNormalisedExpCouples::PrintFitCalculationResult(string OutputFileName, string output_dir) {//функция записывает результаты нормировки в выходные файлы .txt и .pdf
//на вход подаётся вектор пар экспериментов (вектор объектов CoupleOfExperiments)
	//и название выходных файлов .pdf .txt OutputFileName
	cout<<"void SetOfNormalisedExpCouples::PrintFitCalculationResult has started!"<<endl;
	OutputFileName=string(output_dir)+"/"+OutputFileName;
	cout<<"void PrintFitCalculationResult will save results in "<<OutputFileName<<endl;
	ofstream OutputTextFile((OutputFileName+".txt").c_str());//создаём .txt файл с выходными данными
	cc2->Print((OutputFileName+".pdf[").c_str(),"pdf");//создаём .pdf файл с выходными данными, который сейчас будем наполнять графиками и текстом
	for(unsigned int i=0;i<data.size();i++)//для каждой пары экспериментов во входном векторе v
	{
		cout<<"working with "<<i<<" pair!"<<endl;
		SpectroscopicFactorHistogram HistPickup=data[i].Pickup.BuildSpectroscopicFactorHistogram(1);//создаём гистограммы спектроскопических факторов, которые будем отрисовывать на холсте
		SpectroscopicFactorHistogram HistStrip=data[i].Stripping.BuildSpectroscopicFactorHistogram(1);//для срыва и подхвата, всего 2 гистограммы
		SpectroscopicFactorHistogram HistNormPickup=data[i].Pickup.BuildSpectroscopicFactorHistogram(data[i].n_m);//создаём гистограммы нормированных спектроскопических факторов, которые будем отрисовывать на холсте
		SpectroscopicFactorHistogram HistNormStrip=data[i].Stripping.BuildSpectroscopicFactorHistogram(data[i].n_p);//для срыва и подхвата, всего 2 гистограммы, итого 4 гистограммы на холсте
		
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
		for(unsigned int j=0;j<data[i].points_G.GetN();j++)//для каждой пары срыв-подхват в векторе CE
		{	
			gr_before_norm.SetPoint(j,data[i].points_G.GetPointX(j)/
			data[i].n_m,data[i].points_G.GetPointY(j)/data[i].n_p);
			gr_before_norm.SetPointError(j,data[i].points_G.GetErrorX(j)/
			data[i].n_m,data[i].points_G.GetErrorY(j)/data[i].n_p);
		}	
		gr_before_norm.SetTitle("Fit graph;G^-/2j+1;G^+/2j+1");//здесь и далее рисуем график фита; устанавливаем заголовок сверху графика
		gr_before_norm.SetMarkerStyle(28);//устанавливаем стиль маркеров фитируемых точек
		//gr_before_norm.ComputeRange(0,0,GetMaximum(data[i].Gm_alt_c)+0.1,GetMaximum(data[i].Gp_alt_c)+0.1);//изменим границы оси
		//gr_before_norm.SetMarkerColor();//было бы неплохо сделать цвет каждой точки соответвующим цвету подоболочки на гистограммах
		gr_before_norm.GetXaxis()->SetLimits(0.,1.5);
		gr_before_norm.Draw("AP");	
		gr_before_norm.SetMinimum(0.);
		gr_before_norm.SetMaximum(1.5);
		
		data[i].points_G.SetMarkerColor(kBlue);//меняем цвет маркеров на синий, чтобы выделялись
		data[i].points_G.SetMarkerStyle(29);
		data[i].points_G.Draw("AP SAME");
		
		//data[i].FIT.SetRange(0,0,GetMaximum(data[i].Gm_alt_c)+1,GetMaximum(data[i].Gp_alt_c)+1);	
		data[i].FIT.Draw("l same");//"SAME" - superimpose on top of existing picture, "L" - connect all computed points with a straight line  
		///для построения второй линии фита:
		data[i].FIT2.Draw("l same");
		///для построения третей линии фита:
		data[i].FIT3.SetLineColor(1);
		data[i].FIT3.Draw("l same");
		gPad->BuildLegend();
		cout<<"Going to Pad4!"<<endl;
		cc2->cd(4);//переходим к Pad4
		HistStrip.PrintSpectroscopicFactorHistogram();//рисуем гистограмму для эксперимента срыва
		
		string TextOutput=data[i].FitResultsInTextForm(1);
		stringstream s(TextOutput);
		OutputTextFile<<TextOutput<<"\n";//записывем в текстовый файл результаты расчёта
		cout<<"Going to Pad5!"<<endl;
		cc2->cd(5);//переходим к Pad5
		HistNormStrip.PrintSpectroscopicFactorHistogram();//рисуем гистограмму для нормированного эксперимента срыва
		cout<<"Going to Pad6!"<<endl;
		cc2->cd(6);//переходим к Pad6
		TextOutput=data[i].FitResultsInTextForm(0);
		data[i].DrawResultsInTextForm(TextOutput);//выводим текст справа внизу
		cout<<"Printing in pdf file!"<<endl;
		cc2->Print((OutputFileName+".pdf").c_str(),"pdf");//сохраняем всё в .pdf файл
		cout<<"gPad->Update()!"<<endl;
		gPad->Update();//обновим текущую область построения
		cc2->Clear();
	}
	cc2->Print((OutputFileName+".pdf]").c_str(),"pdf");//сохраняем всё в .pdf файл (в третий раз?)
}
void SetOfNormalisedExpCouples::ArrangeByPenalty() {
	//функция меняяет соседние в векторе пары экспериментов,
	//до тех пор, пока они не будут отранжированы по возрастанию функции ошибок
	//функция сортирует пары экспериментов в поданном векторе по убыванию соответсвующей штрафной функции
	//двойной перебор нужен чтобы сравнить каждый элемент вектора с каждым, происходит сортировка по значению штрафной функции
	cout<<"void SetOfNormalisedExpCouples::ArrangeByPenalty() has started!"<<endl;
	for(unsigned int i=0;i<data.size();i++) {//для каждого элемента i массива
		int NumberOfExcanges=0;//переменная для числа перестановок
		for(unsigned int j=0;j<data.size()-i-1;j++)//для каждого элемента массива j, который имеет номер в векторе меньше i
		{
			if(data[j].penalty>data[j+1].penalty)//если функция ошибок пары экспериментов j больше, чем у пары j+1
			{//стандартные действия для смены мест двух соседних эелементов массива/вектора
				NormalisedCoupleOfExperiments tmp=data[j];//создаём буферную пару для перемещения пар j и j+1, равную паре j, чтобы её сохранить
				data[j]=data[j+1];//сохраняем в j пару j+1
				data[j+1]=tmp;//сохраняем в j+1 буфер, который равен паре j
				NumberOfExcanges++;//считаем число перестановок, увеличивая каждый раз NumberOfExcanges на 1
			}
		}
		if(NumberOfExcanges==0)//если не было совершенно перестановок, NumberOfExcanges осталось равной 0
		{
			return;//то функция просто ничего не делает
		}
	}
}//конец void ArrangeByPenalty
void SetOfNormalisedExpCouples::InduceNormalisation() {
	cout<<"void SetOfNormalisedExpCouples::InduceNormalisation() has started!"<<endl;
	for(unsigned int i=0;i<data.size();i++) {//для каждой пары срыв-подхват в векторе CE
		data[i].InduceNormalisation();
	}
}
void SetOfNormalisedExpCouples::ReCalcSPE_and_OCC() {
	cout<<"void SetOfNormalisedExpCouples::ReCalcSPE_and_OCC() has started!"<<endl;
	for(unsigned int i=0;i<data.size();i++) {//для каждой пары срыв-подхват в векторе CE
		data[i].ReCalcSPE_and_OCC();
	}
}

