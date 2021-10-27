#pragma once
#include <vector>
#include <string>

using namespace std;

class InputData
{
	public:
	string Nucleus;//название ядра
	double BA;//Энергия отделения нуклона от ядра А
	double BA1;//Энергия отделения нуклона от ядра А+1
	vector<string> PickupFileNames;//массив имен файлов для pickup
	vector<string> StrippingFileNames;//массив имен файлов для stripping
};
