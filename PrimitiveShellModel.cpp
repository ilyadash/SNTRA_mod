#include <stdio.h>
#include <vector>
#include <cstring>
#include <sstream>
#include <iostream>
#include <fstream>
#include <cmath>

double PrimitiveShellModelOccupancy(int Z, int A, int n, int l, float j, char Particle)
{
	int N[]={1,1,1,1,2,1,1,2,1,2,1,2,1,1,2,3};
	int L[]={0,1,1,2,0,2,3,1,3,1,4,2,4,5,2,0};
	float J[]={0.5,1.5,0.5,2.5,0.5,1.5,3.5,1.5,2.5,0.5,4.5,2.5,3.5,5.5,1.5,0.5};
	float occ=0;
	int Nn=A-Z;
	int iterator=0;
	if(Particle=='n')
	{
		while(Nn>0)
		{
			Nn-=2*J[iterator]+1;
			if(Nn>0)
			{
				occ=1;
			}
			else
			{
				occ=Nn/(2*J[iterator]+1)+1;
			}
			if((N[iterator]==n)&&(L[iterator]==l)&&(J[iterator]==abs(j)))
			{
				return occ;
			}
			iterator++;
		}
	}
	if(Particle=='p')
	{
		while(Z>0)
		{
			Z-=2*J[iterator]+1;
			if(Z>0)
			{
				occ=1;
			}
			else
			{
				occ=Z/(2*J[iterator]+1)+1;
			}
			if((N[iterator]==n)&&(L[iterator]==l)&&(J[iterator]==abs(j)))
			{
				return occ;
			}
			iterator++;
		}
	}
	return 0;
} 
