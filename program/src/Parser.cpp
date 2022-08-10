#include <stdio.h>
#include <vector>
#include <cstring>
#include <sstream>
#include <iostream>
#include <fstream>
#include <cmath>
#include "ExtStrings.cpp"
#include "Constants.cpp"
#include "TString.h"

#define AngularMomentumSize 7

const int Z_number[]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111};
const string Atomic_symbols[]={"H","He","Li","Be","B","C","N","O","F","Ne","Na","Mg","Al","Si","P","S","Cl","Ar","K","Ca","Sc","Ti","V","Cr","Mn","Fe","Co","Ni","Cu","Zn","Ga","Ge","As","Se","Br","Kr","Rb","Sr","Y","Zr","Nb","Mo","Tc","Ru","Rh","Pd","Ag","Cd","In","Sn","Sb","Te","I","Xe","Cs","Ba","La","Ce","Pr","Nd","Pm","Sm","Eu","Gd","Tb","Dy","Ho","Er","Tm","Yb","Lu","Hf","Ta","W","Re","Os","Ir","Pt","Au","Hg","Tl","Pb","Bi","Po","At","Rn","Fr","Ra","Ac","Th","Pa","U","Np","Pu","Am","Cm","Bk","Cf","Es","Fm","Md","No","Lr","Rf","Db","Sg","Bh","Hs","Mt","Ds","Rg"};
const char AngularMomentum[]={'s','p','d','f','g','h','i'};

int FindInVector(int element,vector<int> v) {
	for(unsigned int i=0;i<v.size();i++) {
		if(v[i]==element) {
			return i;
		}
	}
	return -1;
}

string NLJToString(int n, int L, float J) {
	string result;
	J=abs(J);
	if(abs(L-J)!=0.5) {
		result=sprintf("Error!n=%d l=%d j=%d/2",n,L,(int)(J*2));
		return result;
	}
	else {
		if((L>6)||(L<0)) {
			result=sprintf("Unknown L!n=%d l=%d j=%d/2",n,L,(int)(J*2));
		}
		else {
			result=sprintf("%d%c%d/2",n,AngularMomentum[L],(int)(J*2));
		}
		return result;
	}
}

int StringToNLJ(string s, int &n, int &l, float &JP)
{
	//cout<<"StringToNLJ has started for "<<s<<"!"<<"\n";
	if(s.size()>4)
	{
		if(s[0]>='1'&&s[0]<='9')
		{
			n=atoi(&s[0]);
			//cout<<"n = "<<n<<"\n";
		}
		else
		{
			cout<<"	***Error! StringToNLJ has got too strange n="<<s[0]<<"! Return 0!"<<"\n";
			return 0;
		}
		for(unsigned int i=0;i<7;i++)
		{
			if(s[1]==AngularMomentum[i])
			{
				//cout<<"s[1] = "<<s[1]<<"\n";
				l=i;
				//cout<<"l = "<<l<<"\n";
			}
		}
		s.erase(0,2);
		//cout<<"After erase:\n";
		//cout<<"s = "<<s<<"\n";
		if((s[2]>='1'&&s[2]<='9')&&(s[4]>='1'&&s[4]<='9'))
		{
			//cout<<"s[2]="<<s[2]<<"\n";
			JP=pow(-1,l)*((float)atoi(&s[0]))/((float)atoi(&s[2]));
			if (s[5]>='1'&&s[5]<='9')
			{
				JP=pow(-1,l)*((float)atoi(&s[0]))/(10*(float)atoi(&s[2])+(float)atoi(&s[3]));
			}
			//cout<<"JP="<<JP<<"\n";
			return 1;
		}
		else
		{
			cout<<"	***Error! StringToNLJ has got too strange JP! Return 0!"<<"\n";
			return 0;
		}
	}
	else
	{
		cout<<"	***Error! StringToNLJ has got too short input string! Return 1!"<<"\n";
		return 1;
	}
	
}

int TStringToNLJ(TString s, int &n, int &l, float &JP)
{
	//cout<<"TStringToNLJ has started!"<<"\n";
	//cout<<"s.Data() == "<<s.Data()<<"\n";
	string str(s.Data());
	//cout<<"str == "<<str<<"\n";
	return StringToNLJ(str, n, l, JP);
}

string LToString(int L)
{
	string result;
	if((L>6)||(L<0))
	{
		result=sprintf("Unknown L! l=%d",L);
	}
	else
	{
		result+=AngularMomentum[L];
	}
	return result;
}

vector<int> StringLToInt(string L_string)
{
	vector<int> result;
	for(unsigned int i=0;i<L_string.size();i++)
	{
		for(unsigned int j=0;j<AngularMomentumSize;j++)
		{
			if(L_string[i]==AngularMomentum[j])
			{
				result.push_back(j);
			}
		}
	}
	return result;
}

void GetAZ(string nucleus, int &Z, int &A)
{
	string mass;
	string name;
	if(nucleus.size()==0)
	{
		cout<<"Fatal error: nucleus is not defined!\n";
		return ;
	}
	
	if(nucleus=="p")
	{
		Z=1;A=1; return;
	}
	if(nucleus=="n")
	{
		Z=0;A=1; return;
	}
	if(nucleus=="d")
	{
		Z=1;A=2; return;
	}
	if(nucleus=="t")
	{
		Z=1;A=3; return;
	}
	if(nucleus=="a")
	{
		Z=2;A=4; return;
	}
	for(unsigned int i=0;i<nucleus.size();i++)
	{
		if((nucleus[i]>='A')&&(nucleus[i]<='z'))
		{
			name+=nucleus[i];
		}
		if((nucleus[i]>='0')&&(nucleus[i]<='9'))
		{
			mass+=nucleus[i];
		}
	}
	for(unsigned int i=0;i<111;i++)
	{
		if(Atomic_symbols[i]==name)
		{
			Z=Z_number[i];
			A=atoi(mass.c_str());
			return ;
		}
	}
}

double GetNuclearMass(string nucleus)
{
	int A,Z;
	GetAZ(nucleus,Z,A);
	ifstream MassFile(sprintf("audi/z%03d",Z).c_str());
	string str;
	while(getline(MassFile,str))
	{
		int a=atoi(GetField(str,2).c_str());;
		if(a==A)
		{
			return atof(GetField(str,3).c_str())*u;
		}
	}
}

double GetNuclearMass(int Z, int A)
{
	ifstream MassFile(sprintf("audi/z%03d",Z).c_str());
	if(!MassFile)
	{
		cout<<"Error:file \""+sprintf("audi/z%03d",Z)+"\" not found \n"; 
	}
	string str;
	while(getline(MassFile,str))
	{
		int a=atoi(GetField(str,2).c_str());;
		if(a==A)
		{
			return atof(GetField(str,3).c_str())*u;
		}
	}
	cout<<"Error: double GetNuclearMass(int Z, int A) cannot return nuclear mass, returns 0! \n"; 
	return 0;
}

double GetSeparationEnergy(string nucleus, string particle="n")
{
	double nucleus_mass, particle_mass, product_mass;
	
	cout<<"1"<<nucleus<<"\n";
	
	int nucleus_A, nucleus_Z, product_A, product_Z, particle_A, particle_Z;
	if(particle=="n")
	{
		particle_mass=Mn;
		particle_A=1;
		particle_Z=0;
	}
	if(particle=="p")
	{
		particle_mass=Mp;
		particle_A=1;
		particle_Z=1;
	}
	if((particle!="p")&&(particle!="n"))
	{
		GetAZ(particle, particle_Z, particle_A);
		particle_mass=GetNuclearMass(particle_Z, particle_A)-particle_Z*Me;
	}
	GetAZ(nucleus, nucleus_Z, nucleus_A);
	nucleus_mass=GetNuclearMass(nucleus_Z, nucleus_A)-nucleus_Z*Me;
	product_Z=nucleus_Z-particle_Z;
	product_A=nucleus_A-particle_A;
	if((product_Z<0)||(product_A<0))
	{
		cout<<"Error: product characteristics are not physical: product_Z="<<product_Z<<" product_A="<<product_A<<"\n";
		return 0;
	}
	product_mass=GetNuclearMass(product_Z, product_A)-product_Z*Me;
	return product_mass-nucleus_mass+particle_mass;
}

double GetSeparationEnergy(int nucleus_Z, int nucleus_A, int particle_Z, int particle_A)
{
	double nucleus_mass, particle_mass, product_mass;
	int product_A, product_Z;
	particle_mass=GetNuclearMass(particle_Z, particle_A)-particle_Z*Me;
	nucleus_mass=GetNuclearMass(nucleus_Z, nucleus_A)-nucleus_Z*Me;
	product_Z=nucleus_Z-particle_Z;
	product_A=nucleus_A-particle_A;
	if((product_Z<0)||(product_A<0))
	{
		cout<<"Error: product characteristics are not physical: product_Z="<<product_Z<<" product_A="<<product_A<<"\n";
		return 0;
	}
	product_mass=GetNuclearMass(product_Z, product_A)-product_Z*Me;
	return product_mass-nucleus_mass+particle_mass;
}

void ParceReaction(string reaction, string &type, int &ParticleType)
{
	string particles[2];
	int iterator=0;
	for(int i=0;i<reaction.size();i++)
	{
		if(((reaction[i]>='0')&&(reaction[i]<='9'))||((reaction[i]>='A')&&(reaction[i]<='z')))
		{
			particles[iterator]+=reaction[i];
		}
		else
		{
			if((reaction[i]=='.')||(reaction[i]==','))
			{
				iterator++;
				if(iterator>1)
				{
					return;
				}
			}
		}
	}
	int Z_projectile,A_projectile,Z_product, A_product;
	GetAZ(particles[0],Z_projectile,A_projectile);
	GetAZ(particles[1],Z_product,A_product);
	if(A_projectile-A_product>0)
	{
		type="stripping";
	}
	else
	{
		type="pickup";
	}
	if(Z_projectile-Z_product!=0)
	{
		ParticleType=1;
	}
	else
	{
		ParticleType=0;
	}
}
