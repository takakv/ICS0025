#pragma once
#include <variant>

#include "Date.h"
#include "Time.h"

class Item
{
private:
	char Group;     // Any from range 'A'...'Z'
	int Subgroup;   // Any from range 0...99
	string Name;    // Any, but not empty 
	variant<Date, Time> Timestamp; // Any object of class Date or class Time
public:
	Item();
	Item(char, int, string, Date);
	Item(char, int, string, Time);
	Item(char, int, string, variant<Date, Time>);
	Item(const Item&);
	~Item() = default;
	char GetGroup();
	int GetSubgroup();
	string GetName();
	variant<Date, Time> GetTimestamp();
};
