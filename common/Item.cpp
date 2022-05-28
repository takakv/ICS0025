#include "common.h"

extern default_random_engine Generator;
extern uniform_int_distribution<int> GroupDistribution;
extern uniform_int_distribution<int> SubgroupDistribution;
extern uniform_int_distribution<int> VariantDistribution;
const Date Begin = Date(1, 1, 2018), End = Date(31, 12, 2018);

Item::Item()
{ // creates a pseudo-random item
	Group = (char)GroupDistribution(Generator);
	Subgroup = SubgroupDistribution(Generator);
	Name = ::CreateRandomName();
	int var = VariantDistribution(Generator);
	if (var % 2)
	{
		Timestamp = Date::CreateRandomDate(Begin, End);
	}
	else
	{
		Timestamp = Time::CreateRandomTime();
	}
}

Item::Item(char Group, int Subgroup, string Name, Date Timestamp)
{
	this->Group = Group;
	this->Subgroup = Subgroup;
	this->Name = Name;
	this->Timestamp = Timestamp;
}

Item::Item(char Group, int Subgroup, string Name, Time Timestamp)
{
	this->Group = Group;
	this->Subgroup = Subgroup;
	this->Name = Name;
	this->Timestamp = Timestamp;
}

Item::Item(char Group, int Subgroup, string Name, variant<Date, Time> Timestamp)
{
	this->Group = Group;
	this->Subgroup = Subgroup;
	this->Name = Name;
	this->Timestamp = Timestamp;
}

Item::Item(const Item& Source)
{
	this->Group = Source.Group;
	this->Subgroup = Source.Subgroup;
	this->Name = Source.Name;
	this->Timestamp = Source.Timestamp;
}

char Item::GetGroup()
{
	return Group;
}

int Item::GetSubgroup()
{
	return Subgroup;
}

string Item::GetName()
{
	return Name;
}

variant<Date, Time> Item::GetTimestamp()
{
	return Timestamp;
}