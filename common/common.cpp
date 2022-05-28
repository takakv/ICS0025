#include "common.h"

static const char* NamesFile = "Birds.txt"; // http://www.jimpalt.org/birdwatcher/name.html 941 names of different birds, ASCII code, OD OA after each row
default_random_engine Generator;
static uniform_int_distribution<int> NameDistribution;
static vector<string> Names;
uniform_int_distribution<int> GroupDistribution('A', 'Z');
uniform_int_distribution<int> SubgroupDistribution(0, 99);
uniform_int_distribution<int> VariantDistribution(0, 1000);

void PrepareNames()
{
	ifstream file;
	file.open(NamesFile, fstream::in);
	if (!file.good())
		throw exception("Unable to open the names file");
	char buf[100];
	while (file.good())
	{
		file.getline(buf, 100);
		if (buf[0])
			Names.push_back(string(buf));
	}
	file.close();
	uniform_int_distribution<int> distr(0, Names.size() - 1);
	NameDistribution = distr;
}

string CreateRandomName()
{
	int i = NameDistribution(Generator);
	return Names[i];
}