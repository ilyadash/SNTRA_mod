#include <cstring>
#include <sstream>
#include <stdarg.h> 

using namespace std;

/*string RmSymbolsFromEnd(string line, int nsymb)
{
	for(int i=0;i<nsymb;i++)
	{
		if(line.size()>0)
		line.pop_back();
	}
	return line;
}*/

string InvertString(string line)
{
	string line2;
	stringstream s;
	for(unsigned int i=0;i<line.size();i++)
	{
		s<<line[line.size()-i-1];	
	}
	s>>line2;
	return line2;
}

string GetFileType(string line)
{
	string type;
	stringstream s;
	for(unsigned int i=0;i<line.size();i++)
	{
		if(line[line.size()-i-1]!='.')
		{
			s<<line[line.size()-i-1];
		}
		else
		{
			s>>type;
			return InvertString(type);
		}
	
	}
}

float GetEnergyFromFileName(string line)
{
	float E=0;
	stringstream s;
	int flag=0;
	for(unsigned int i=0;i<line.size();i++)
	{
		if(i>1)
		{
			if(line[i-1]=='_')
			{
				if((line[i]>='0')&&(line[i]<='9'))
				{
					flag++;
				}
			}
		}

		if(flag==1)
		{
			s<<line[i];
		}	
	}
	s>>E;
	return E*1000;
}

string GetPath(string line)
{
	stringstream s;
	string path;
	int flag_slash=0;
	for(unsigned int i=0;i<line.size();i++)
	{
		if(line[line.size()-i-1]=='/')
		{
			flag_slash=1;
		}
		if(flag_slash==1)
		{
			s<<line[line.size()-i-1];
		}
	}
	s>>path;
	return InvertString(path);
}
string GetFileName(string line)
{
	stringstream s;
	string name;
	int flag_dot=0;
	int dot_iterator=0;
	int first_dot_iterator=0;
	for(unsigned int i=0;i<line.size();i++)
	{
		if(line[line.size()-i-1]=='.')
		{
			flag_dot=1;
			if(dot_iterator==0)
			{
				first_dot_iterator=i;
			}
			dot_iterator++;
		}
		if(line[line.size()-i-1]=='/')
		{
			s>>name;
			return InvertString(name);
		}
		//if((flag_dot==1)&&(line[line.size()-i-1]!='.'))
		if((flag_dot==1)&&(i!=first_dot_iterator))
		{
			s<<line[line.size()-i-1];
		}
	}
	s>>name;
	return InvertString(name);
}
string GetFileNameWithoutErrFlag(string line)
{
	string name=GetFileName(line);
	stringstream ss(name);
	string tmp;
	for(int i=0;i<5;i++)
	{
		string tmp1;
		getline(ss,tmp1,'_');
		tmp+=tmp1;
	}
	return tmp;
}
string sprintf(string stemplate,...)
{
	char s[1000];
	va_list arg;
	int done;
	va_start (arg, stemplate.c_str());
	done = vsprintf (s, stemplate.c_str(), arg);
	va_end (arg);
	string str=s;
	return str;
}
string GetField(string input, int FieldNumber)
{
	stringstream ss(input);
	string tmp;
	for(int i=0;i<FieldNumber;i++)
	{
		string tmp1;
		ss>>tmp1;
		tmp=tmp1;
	}
	return tmp;
}
