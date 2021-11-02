#include <cstring>
//файл содержит функцию, которая переводит поданный полный момент J и чётность P в соответвующую поданной строке inpstr
void StringToJP(string inpstr, double &J, int &P)
{
	J=0;
	P=0;
	if(!strcmp(inpstr.c_str(),"1s1/2"))
	{
		J=0.5;
		P=1;
	}
	if((!strcmp(inpstr.c_str(),"1p1/2"))||(!strcmp(inpstr.c_str(),"2p1/2")))
	{
		J=0.5;
		P=-1;
	}
	if((!strcmp(inpstr.c_str(),"1p3/2"))||(!strcmp(inpstr.c_str(),"2p3/2")))
	{
		J=1.5;
		P=-1;
	}
	if(!strcmp(inpstr.c_str(),"1d5/2"))
	{
		J=2.5;
		P=1;
	}
	if(!strcmp(inpstr.c_str(),"1d3/2"))
	{
		J=1.5;
		P=-1;
	}
	if(!strcmp(inpstr.c_str(),"2s1/2"))
	{
		J=0.5;
		P=1;
	}
	if(!strcmp(inpstr.c_str(),"1f7/2"))
	{
		J=3.5;
		P=-1;
	}
	if(!strcmp(inpstr.c_str(),"1f5/2"))
	{
		J=2.5;
		P=-1;
	}
	if(!strcmp(inpstr.c_str(),"1g7/2"))
	{
		J=3.5;
		P=1;
	}
	if(!strcmp(inpstr.c_str(),"1g9/2"))
	{
		J=4.5;
		P=1;
	}
}
