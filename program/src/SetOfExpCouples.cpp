#include "SetOfExpCouples.h"

void SetOfExpCouples::CreateCouplesOfExperiments(vector<Experiment> &Pickup,
vector<Experiment> &Stripping, parameters &par)//функция создаёт вектор всех вариантов пар экспириментов срыв-подхват (вектор объектов CoupleOfExperiments);
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
	data = result;//сохраняем результирующий вектор пар
	cout<<"CreateCouplesOfExperiments has ended!"<<endl;
}//конец функции vector<CoupleOfExperiments> CreateCouplesOfExperiments



//SetOfExpCouples
