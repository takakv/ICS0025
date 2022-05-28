#pragma once
#include <map>
#include <vector>
#include <optional>

#include "../../common/common.h"

class Data
{
private:
    map<char, map<int, vector<Item*>*>*> DataStructure;
public:
    Data();
    Data(int n);
    ~Data();

    void PrintAll();
    void PrintGroup(char c);
    void PrintSubgroup(char c, int i);
    void PrintItem(char c, int i, string s);

    size_t CountItems();
    size_t CountGroupItems(char c);
    size_t CountSubgroupItems(char c, int i);

    map<int, vector<Item*>*>* GetGroup(char c);
    vector <Item*>* GetSubgroup(char c, int i);
    Item* GetItem(char c, int i, string s);

    map<int, vector<Item*>*>* InsertGroup(char c, initializer_list<int> subgroups, initializer_list<initializer_list<tuple<string, optional<variant<Date, Time>>>>> items);
    vector<Item*>* InsertSubgroup(char c, int i, initializer_list<tuple<string, optional<variant<Date, Time>>>> items);
    Item* InsertItem(char c, int i, string s, optional<variant<Date, Time>> v = nullopt);

    bool RemoveGroup(char c);
    bool RemoveItem(char c, int i, string s);
    bool RemoveSubgroup(char c, int i);
};