#include <vector>
#include <cmath>
using namespace std;
int SearchInVector(vector<double> vec, double value)//Функция, возвращающая позицию искомого элемента в векторе. Возвращает -1, если элемент не найден
{
	int length=vec.size();
	int return_value=-1;
	if(length!=0)
	{
		for(int i=0;i<length;i++)
		{
			if(vec[i]==value)
			{
				return_value=i;
				return return_value;
			}
		}
	}
	return return_value;
}

double GetMaximum(vector<double> v)
{
	if(v.size()==0)
	{
		return NAN;
	}
	double maximum=v[0];
	for(unsigned i=0;i<v.size();i++)
	{
		if(v[i]>maximum)
		{
			maximum=v[i];
		}
	}
	return maximum;
}
double GetMinimum(vector<double> v)
{
	if(v.size()==0)
	{
		return NAN;
	}
	double minimum=v[0];
	for(unsigned i=0;i<v.size();i++)
	{
		if(v[i]<minimum)
		{
			minimum=v[i];
		}
	}
	return minimum;
}

int SearchJP(vector<double> J,vector<int> P, double J_val, int P_val)//Функция, ищущая комбинацию JP в векторах. Возвращает 0, если не найдено
{
	if((J.size()!=0)&&(P.size()!=0))
	{
		for(unsigned int i=0;i<J.size();i++)
		{
			if((J[i]==J_val)&&(P[i]==P_val))
			{
				return 1;
			}
		}
	}
	return 0;
}

double Average(vector<double> &v, int NElements=0)
{
	double result=0;
	unsigned int NumberOfAnalysedElements=0;
	if(NElements>0)
	{
		NumberOfAnalysedElements=NElements;
	}
	else
	{
		NumberOfAnalysedElements=v.size();
	}
	if(NumberOfAnalysedElements==0)
	{
		return 0;
	}
	for(unsigned int i=0;i<NumberOfAnalysedElements;i++)
	{
		result+=v[i];
	}
	return result/NumberOfAnalysedElements;
	
}
