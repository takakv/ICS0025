#include "Main.h"

const Date Begin = Date(1, 1, 2018), End = Date(31, 12, 2018);

Data::Data(int n)
{
    for (int i = 0; i < n; ++i)
    {
        Item* item = new Item();
        char group = item->GetGroup();
        int subgroup = item->GetSubgroup();

        // If the group doesn't exist, create subgroup and group.
        // Add item to subgroup.
        if (DataStructure.count(group) == 0)
        {
            auto item_v = new vector<Item*>{ item };
            auto sg_map = new map<int, vector<Item*>*>{ {subgroup, item_v} };
            DataStructure.insert({ group, sg_map });
        }
        // Create subgroup, add to group. Add item to subgroup.
        else if (DataStructure[group]->count(subgroup) == 0)
        {
            auto item_v = new vector<Item*>{ item };
            DataStructure[group]->insert({ subgroup, item_v });
        }
        // Add item directly to subgroup.
        else
        {
            // The pointer-access is necessary to properly reach the list.
            (*DataStructure[group])[subgroup]->push_back(item);
        }
    }
}

Data::Data() {}

Data::~Data()
{
    while (DataStructure.begin() != DataStructure.end())
    {
        auto group = DataStructure.begin();
        RemoveGroup(group->first);
    }
}

map<int, vector<Item*>*>* Data::GetGroup(char c)
{
    return DataStructure.count(c) == 0 ? nullptr : DataStructure[c];
}

vector<Item*>* Data::GetSubgroup(char c, int i)
{
    return DataStructure.count(c) == 0 ? nullptr :
        DataStructure[c]->count(i) == 0 ? nullptr :
        (*DataStructure[c])[i];
}

Item* Data::GetItem(char c, int i, string s)
{
    if (DataStructure.count(c) == 0 || DataStructure[c]->count(i) == 0)
        return nullptr;
    for (auto const& item : *(*DataStructure[c])[i])
        if (item->GetName() == s) return item;
    return nullptr;
}

void Data::PrintAll()
{
    for (auto const& group : DataStructure) PrintGroup(group.first);
}

void Data::PrintGroup(char c)
{
    if (DataStructure.count(c) == 0)
        throw invalid_argument(string("No such group '") + c + ("'\n"));

    cout << c << ":\n";
    for (auto const& subgroup : *DataStructure[c])
        PrintSubgroup(c, subgroup.first);
    cout << endl;
}

bool sort_by_name(Item* item1, Item* item2)
{
    return item1->GetName().compare(item2->GetName()) < 0;
}

struct make_timestamp_string
{
    string operator()(Date t) const { return t.ToString(); }
    string operator()(Time t) const { return t.ToString(); }
};

void Data::PrintSubgroup(char c, int i)
{
    if (DataStructure.count(c) == 0)
        throw invalid_argument(string("No such group '") + c + ("'\n"));
    if (DataStructure[c]->count(i) == 0)
        throw invalid_argument(
            string("No such subgroup '") + c + to_string(i) + ("'\n")
        );

    vector<Item*>* v = (*DataStructure[c])[i];
    sort(v->begin(), v->end(), sort_by_name);

    for (auto const& item : *v)
        cout << setw(2) << item->GetSubgroup() << ": "
            << item->GetName() << " "
            << visit(make_timestamp_string(), item->GetTimestamp()) << endl;
}

void Data::PrintItem(char c, int i, string s)
{
    Item* item = GetItem(c, i, s);

    if (item == nullptr)
        throw invalid_argument(
            string("No such item '") + s + "' in " + c + to_string(i) + "\n"
        );

    cout << item->GetGroup() << item->GetSubgroup() << ": "
        << item->GetName() << " "
        << visit(make_timestamp_string(), item->GetTimestamp()) << endl;
}

int Data::CountItems()
{
    int count = 0;
    for (auto const& group : DataStructure) count += CountGroupItems(group.first);
    return count;
}

int Data::CountGroupItems(char c)
{
    if (DataStructure.count(c) == 0) return 0;

    int count = 0;
    for (auto const& subgroup : *DataStructure[c])
        count += CountSubgroupItems(c, subgroup.first);
    return count;
}

int Data::CountSubgroupItems(char c, int i)
{
    if (DataStructure.count(c) == 0 || DataStructure[c]->count(i) == 0)
        return 0;

    int count = 0;
    vector<Item*>* v = (*DataStructure[c])[i];
    for (auto const& item : *v) ++count;
    return count;
}

inline bool invalid_group(char c)
{
    return !(c > 'A' && c < 'Z');
}

Item* Data::InsertItem(char c, int i, string s, optional<variant<Date, Time>> v)
{
    if (s.empty() || invalid_group(c)) return nullptr;

    Item* item = GetItem(c, i, s);
    if (item != nullptr) return nullptr;

    variant<Date, Time> t;
    t = v ? *v : Date::CreateRandomDate(Begin, End);

    item = new Item(c, i, s, t);

    auto group = DataStructure.find(c);
    if (group == DataStructure.end())
    {
        auto item_v = new vector<Item*>{ item };
        auto sg_map = new map<int, vector<Item*>*>{ {item->GetSubgroup(), item_v} };
        DataStructure.insert({ c, sg_map });
        return item;
    }

    auto subgroup = DataStructure[c]->find(i);
    if (subgroup == DataStructure[c]->end())
    {
        auto item_v = new vector<Item*>{ item };
        DataStructure[c]->insert({ i, item_v });
        return item;
    }

    (*DataStructure[c])[i]->push_back(item);
    return item;
}

vector<Item*>* CreateSubgroup(char c, int i,
    initializer_list<tuple<string, optional<variant<Date, Time>>>> items,
    map<char, map<int, vector<Item*>*>*> DataStructure)
{
    auto subgroup = new vector<Item*>;

    // Verify if all items are correctly formatted.
    for (auto& item : items)
    {
        string name = get<0>(item);
        // We must free the memory.
        if (name.empty())
        {
            vector<Item*>().swap(*subgroup);
            delete subgroup;
            subgroup = nullptr;
            return subgroup;
        }

        variant<Date, Time> t;
        t = get<1>(item) ? *get<1>(item) : Date::CreateRandomDate(Begin, End);
        subgroup->push_back(new Item(c, i, name, t));
    }

    return subgroup;
}

vector<Item*>* Data::InsertSubgroup(char c, int i,
    initializer_list<tuple<string, optional<variant<Date, Time>>>> items)
{
    if (invalid_group(c)) return nullptr;

    // The subgroup must not exist.
    if (DataStructure.count(c) != 0 && DataStructure[c]->count(i) != 0)
        return nullptr;

    vector<Item*>* subgroup = CreateSubgroup(c, i, items, DataStructure);
    if (subgroup == nullptr) return subgroup;
    
    if (DataStructure.count(c) == 0)
    {
        auto sg_map = new map<int, vector<Item*>*>{ {i, subgroup} };
        DataStructure.insert({ c, sg_map });
        return subgroup;
    }

    // The group must exist to reach here.
    DataStructure[c]->insert({ i, subgroup });
    return subgroup;
}

map<int, vector<Item*>*>* Data::InsertGroup(char c, initializer_list<int> subgroups,
    initializer_list<initializer_list<tuple<string, optional<variant<Date, Time>>>>> items)
{
    if (invalid_group(c) || DataStructure.count(c) > 0) return nullptr;

    auto group = new map<int, vector<Item*>*>;

    for (int i = 0; i < subgroups.size(); ++i)
    {
        int subgroup_id = *(subgroups.begin() + i);
        initializer_list<tuple<string, optional<variant<Date, Time>>>> current_items = *(items.begin() + i);

        // Build the subgroup from items and subgroup identifier.
        vector<Item*>* subgroup = CreateSubgroup(c, subgroup_id, current_items, DataStructure);

        // We must free the memory.
        if (subgroup == nullptr) {
            // Release already created subgroups in the group.
            for (int j = 0; j < i; ++j) {
                vector<Item*>().swap(*(*DataStructure[c])[subgroup_id]);
                (*DataStructure[c])[subgroup_id] = nullptr;
            }

            return nullptr;
        }

        (*group)[subgroup_id] = subgroup;
    }

    DataStructure[c] = group;
    return group;
}

/*
    The approach of cascading is cleaner here than bubbling up
    because larger elements still need to free smaller elements within.
    Bubbling the task upwards causes function recursion.
*/

bool Data::RemoveItem(char c, int i, string s)
{
    Item* item = GetItem(c, i, s);
    if (item == nullptr) return false;

    auto subgroup = GetSubgroup(c, i);

    // Delete element from subgroup.
    auto it = find(subgroup->begin(), subgroup->end(), item);
    subgroup->erase(it);
    delete item;

    if (!subgroup->empty()) return true;
    auto group = GetGroup(c);

    // If subgroup is empty, delete subgroup from group.
    subgroup->clear();
    group->erase(i);
    delete subgroup;

    if (!DataStructure[c]->empty()) return true;

    // If group is empty, delete it from the DS.
    DataStructure[c]->clear();
    DataStructure.erase(c);
    delete group;

    return true;
}

bool Data::RemoveSubgroup(char c, int i)
{
    auto subgroup = GetSubgroup(c, i);
    if (subgroup == nullptr) return false;

    while (subgroup && !subgroup->empty())
        RemoveItem(c, i, subgroup->front()->GetName());

    return true;
}

bool Data::RemoveGroup(char c)
{
    auto group = GetGroup(c);
    if (group == nullptr) return false;

    // Don't use a while loop because checking for existence of group
    // yields garbage after freeing DataStrcuture[c] and checking for
    // DataStructure[c] recreates a key for c.
    int og_size = group->size();
    for (int i = 0; i < og_size; ++i)
        RemoveSubgroup(c, group->begin()->first);

    return true;
}