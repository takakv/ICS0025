#pragma once
#include <map>
#include <vector>
#include <optional>

#include "Item.h"

class Data
{
private:
    map<char, map<int, vector<Item*>*>*> DataStructure;
public:
    Data(int n);
    Data();
    ~Data();
    void PrintAll();
    int CountItems();
    map<int, vector<Item*>*>* GetGroup(char c);
    void PrintGroup(char c);
    int CountGroupItems(char c);
    vector <Item*>* GetSubgroup(char c, int i);
    void PrintSubgroup(char c, int i);
    int CountSubgroupItems(char c, int i);
    Item* GetItem(char c, int i, string s);
    void PrintItem(char c, int i, string s);
    Item* InsertItem(char c, int i, string s, optional<variant<Date, Time>> v = nullopt);
    vector<Item*>* InsertSubgroup(char c, int i, initializer_list<tuple<string, optional<variant<Date, Time>>>> items);
    map<int, vector<Item*>*>* InsertGroup(char c, initializer_list<int> subgroups, initializer_list<initializer_list<tuple<string, optional<variant<Date, Time>>>>> items);
    bool RemoveItem(char c, int i, string s);
    bool RemoveSubgroup(char c, int i);
    bool RemoveGroup(char c);
};