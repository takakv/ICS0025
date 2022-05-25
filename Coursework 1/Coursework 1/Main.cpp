#include "Main.h"

int main()
{
	PrepareNames();
	Test();
	return 0;
}

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

void Test()
{
	Data* pData = new Data(20);
	pData->PrintAll();
	cout << endl;
	
/* The first 20 items (groups, subgroups and names must be as below, the dates and times, of course, will differ)
A:
2: Great Crested Grebe 14:59:29
6: Baikal Teal 08 Sep 2018
B:
73: Ruddy Turnstone 03 Dec 2018
C:
6: Redwing 14 May 2018
26: Moorland Francolin 16:08:29
E:
50: Two-barred Greenish Warbler 04 Jan 2018
H:
80: Red-billed Tropicbird 00:25:60
I:
26: Cuckoo 04 Aug 2018
J:
2: Summer Tanager 21:08:38
K:
3: Song Sparrow 27 Feb 2018
27: Brambling 16 Mar 2018
L:
4: Grey-breasted Spurfowl 06:30:17
76: Alpine Swift 18 May 2018
O:
71: Guillemot 30 Sep 2018
P:
9: Black-throated Green Warbler 01 May 2018
18: Upland Sandpiper 02:55:38
79: Icterine Warbler 14:11:13
T:
93: Nubian Nightjar 13 Aug 2018
U:
91: Crag Martin 06:22:34
V:
61: Lapland Bunting 15 Sep 2018
*/

	cout << (pData->InsertItem('E', 86, "Kaljutuvi") ? "Inserted" : "Not inserted") << endl;
/* In those tests I used bird names from Estonian. You are free to replace them with any strings. But be careful, if
    you, for example, replace "Kaljutuvi" with "Rock dove", do it everywhere in this function. */
	cout << (pData->InsertItem('E', 86, "Koskel", Time()) ? "Inserted" : "Not inserted") << endl;
	cout << (pData->InsertItem('E', 86, "Kassikakk", Time(15, 36, 10)) ? "Inserted" : "Not inserted") << endl;
	cout << (pData->InsertItem('E', 86, "Turteltuvi", Date()) ? "Inserted" : "Not inserted") << endl;
	cout << (pData->InsertItem('E', 86, "Leevike", Date(25, 10, 2021)) ? "Inserted" : "Not inserted") << endl;
	cout << (pData->InsertItem('F', 87, "Ristpart") ? "Inserted" : "Not inserted") << endl;
	cout << (pData->InsertItem('B', 73, "Varblane") ? "Inserted" : "Not inserted") << endl;
	cout << (pData->InsertItem('#', 73, "Varblane") ? "Inserted" : "Not inserted") << endl;
	cout << (pData->InsertItem('B', 120, "Varblane") ? "Inserted" : "Not inserted") << endl;
	cout << (pData->InsertItem('B', 73, "") ? "Inserted" : "Not inserted") << endl;
	cout << endl;

	pData->PrintItem('H', 80, "Red-billed Tropicbird");
	pData->PrintItem('E', 86, "Leevike");
	try
	{
		pData->PrintItem('E', 10, "XXX");
	}
	catch (const invalid_argument& e)
	{
		cout << e.what();
	}
	try
	{
		pData->PrintItem('Z', 10, "XXX");
	}
	catch (const invalid_argument& e)
	{
		cout << e.what();
	}
	cout << endl;
	
	cout << "Items in group E subgroup 86: " << pData->CountSubgroupItems('E', 86) << endl;
	cout << "Items in group F subgroup 87: " << pData->CountSubgroupItems('F', 87) << endl;
	cout << "Items in group B subgroup 73: " << pData->CountSubgroupItems('B', 73) << endl;
	cout << "Items in group X subgroup 10: " << pData->CountSubgroupItems('X', 10) << endl;
	cout << "Items in group V subgroup 62: " << pData->CountSubgroupItems('V', 62) << endl;
	cout << endl;

	pData->PrintSubgroup('E', 86);
	pData->PrintSubgroup('F', 87);
	pData->PrintSubgroup('B', 73);
	try
	{
		pData->PrintSubgroup('X', 10);
	}
	catch (const invalid_argument& e)
	{
		cout << e.what();
	}
	try
	{
		pData->PrintSubgroup('V', 62);
	}
	catch (const invalid_argument& e)
	{
		cout << e.what();
	}
	cout << endl;

	cout << "Items in group E: " << pData->CountGroupItems('E') << endl;
	cout << "Items in group F: " << pData->CountGroupItems('F') << endl;
	cout << "Items in group B: " << pData->CountGroupItems('B') << endl;
	cout << endl;

	pData->PrintGroup('E');
	pData->PrintGroup('F');
	pData->PrintGroup('B');
	try
	{
		pData->PrintGroup('Z');
	}
	catch (const invalid_argument& e)
	{
		cout << e.what();
	}
	cout << endl;

	cout << (pData->InsertSubgroup('T', 94, { { "Piiritaja", Date() }, { "Meigas", Date(25, 10, 2021) }, { "Rasvatihane", nullopt },
		                                      { "Punarind", Time(16, 33, 45) }, { "Turteltuvi", Time(16, 33, 45) } }) ?
		     "Inserted" : "Not inserted") << endl;
	cout << (pData->InsertSubgroup('T', 93, { { "Rohevint", Date() } }) ? "Inserted" : "Not inserted") << endl;
	cout << (pData->InsertSubgroup('Q', 1, { { "Luik", Date() } }) ? "Inserted" : "Not inserted") << endl;
	cout << (pData->InsertSubgroup('W', 110, { { "Rohevint", Date() } }) ? "Inserted" : "Not inserted") << endl;
	cout << (pData->InsertSubgroup('#', 10, { { "Rohevint", Date() } }) ? "Inserted" : "Not inserted") << endl;
	cout << (pData->InsertSubgroup('W', 10, { { "Rohevint", Date() }, { "", nullopt }, { "Sootihane", Time() } }) ? "Inserted" : "Not inserted") << endl;
	pData->PrintGroup('T');
	pData->PrintGroup('Q');
	cout << endl;

	tuple<string, optional<variant<Date, Time>>> t1("Sinikael", Date(16, 9, 2021)), t2("Sinitihane", Time(16, 25, 21));
	tuple<string, optional<variant<Date, Time>>> t3("Luik", nullopt);
	tuple<string, optional<variant<Date, Time>>> t4("Nepp", Date()), t5("Lagle", Time()), t6("Metsis", nullopt);
	cout << (pData->InsertGroup('M', { 10, 20, 30 },
		{
			{ t1, t2 },
			{ t3 },
			{ t4, t5, t6 }
		}) ? "Inserted" : "Not inserted") << endl;
	cout << (pData->InsertGroup('F', { 10, 20, 30 },
		{
			{ t1, t2 },
			{ t3 },
			{ t4, t5, t6 }
		}) ? "Inserted" : "Not inserted") << endl;
	cout << (pData->InsertGroup('#', { 10, 20, 30 },
		{
			{ t1, t2 },
			{ t3 },
			{ t4, t5, t6 }
		}) ? "Inserted" : "Not inserted") << endl;
	cout << (pData->InsertGroup('W', { 10, 120, 30 },
		{
			{ t1, t2 },
			{ t3 },
			{ t4, t5, t6 }
		}) ? "Inserted" : "Not inserted") << endl;
	cout << (pData->InsertGroup('W', { 10, 20, 30 },
		{
			{ t1, t2 },
			{ { "", Date() } },
			{ t4, t5, t6 }
		}) ? "Inserted" : "Not inserted") << endl;
	cout << (pData->InsertGroup('W', { 10, 30 },
		{
			{ t1, t2 },
			{ t3 },
			{ t4, t5, t6 }
		}) ? "Inserted" : "Not inserted") << endl;
	cout << (pData->InsertGroup('W', { 10, 20, 30 },
		{
			{ t1, t2 },
			{ t4, t5, t6 }
		}) ? "Inserted" : "Not inserted") << endl;
	pData->PrintGroup('M');
	cout << endl;

	cout << (pData->RemoveItem('I', 26, "Cuckoo") ? "Removed" : "Not removed") << endl;
	cout << (pData->RemoveItem('K', 27, "Brambling") ? "Removed" : "Not removed") << endl;
	cout << (pData->RemoveItem('E', 86, "Kassikakk") ? "Removed" : "Not removed") << endl;
	cout << (pData->RemoveItem('E', 10, "XXX") ? "Removed" : "Not removed") << endl;
	pData->PrintGroup('K');
	pData->PrintGroup('E');
	try
	{
		pData->PrintGroup('I');
	}
	catch (const invalid_argument& e)
	{
		cout << e.what();
	}
	cout << endl;

	cout << (pData->RemoveSubgroup('O', 71) ? "Removed" : "Not removed") << endl;
	cout << (pData->RemoveSubgroup('E', 50) ? "Removed" : "Not removed") << endl;
	cout << (pData->RemoveSubgroup('E', 10) ? "Removed" : "Not removed") << endl;
	cout << (pData->RemoveSubgroup('U', 1) ? "Removed" : "Not removed") << endl;
	cout << (pData->RemoveSubgroup('#', 1) ? "Removed" : "Not removed") << endl;
	pData->PrintGroup('E');
	try
	{
		pData->PrintGroup('O');
	}
	catch (const invalid_argument& e)
	{
		cout << e.what();
	}
	cout << endl;
	
	cout << (pData->RemoveGroup('P') ? "Removed" : "Not removed") << endl;
	cout << (pData->RemoveGroup('U') ? "Removed" : "Not removed") << endl;
	cout << (pData->RemoveGroup('O') ? "Removed" : "Not removed") << endl;
	pData->PrintAll();
	cout << "The data structure contains " << pData->CountItems() << " items" << endl;
	delete pData;
}