#ifndef PROJECT_HIERARCHY_H
#define PROJECT_HIERARCHY_H

#include <iostream>
#include <list>
#include "utility.h"

using namespace std;

class Category;
class SubCategory;
class Event;
class Market;
class Selection;

class Node {
protected:
    string name, node_id;
    bool hidden;
    Node(string nodeid, string n, bool h) : node_id(nodeid), name(n), hidden(h) {}
    Node(const Node &node, string nodeid = "0", string n = "undefined", bool h = true) : node_id(nodeid) , name(n) , hidden(h) {}
public:
    virtual ~Node() {}
    /*getters*/
    string get_name() const { return name; }
    bool get_hidden() const { return hidden; }
    string get_nodeid() const { return node_id; }
    virtual int num_of_options() const { return -1; }
    /*setters*/
    void set_nodeid(string newnodeid) { node_id = newnodeid; }
    /*Node operations*/
    void rename(string new_name) { name = new_name; }
    void toggle_visibility() { hidden = !hidden; }
    virtual void Vtoggle() = 0;       // toggles visibility for the node's sub-hierarchy as well, if any
    /*prints*/
    virtual void print(bool director) const = 0;
    virtual void print_node_options(bool called_by_director = false) const {}
    virtual string file_print() const = 0;
    /*Node navigation*/
    virtual Node* choose_node(int index, bool called_by_director = false) const { return NULL; }
    virtual void erase_from_list(Node *gone) {}
};


class Selection : public Node {
    double price;
    char result;
public:
    Selection(string nodeid, string sel, double pr, bool h = false,char res = '-')     // '-' means no result yet
            : Node(nodeid, sel, h), price(pr) , result(res) {}
    Selection(const Selection &selection, string nodeid, string name)
            : Node(selection, nodeid, name, true) , result(selection.result) , price(selection.price) {}    /*getters*/
    double get_price() const { return price; }
    char get_result() const { return result; }
    /*setters*/
    void set_result(char res) { result = res; }
    /*prints*/
    void print(bool director) const;
    string file_print() const;
    /*Visibility*/
    void Vtoggle();
};


class Market : public Node {
    list <Selection *> selections;
    bool settled;
public:
    Market(string nodeid, string market, bool h = false,bool S = false) : Node(nodeid, market, h) , settled(S) {}
    Market(const Market &market, string nodeid = "0", string name = "undefined");
    ~Market();
    /*getters*/
    bool get_settled() const { return settled; }
    int num_of_options() const;
    /*setters*/
    void set_settled(bool settled) { Market::settled = settled; }
    void Render_result(string winning_nodeid);
    /*prints*/
    void print(bool director) const;
    void print_node_options(bool called_by_director = false) const;
    string file_print() const;
    /*Market operations*/
    Selection *add_selection(string nodeid, string sel, double pr, bool hidden = false, char result = false);
    void copy_selection(const Selection &selection);
    void new_selection(string n, double pr, bool h);
    /*Node navigation*/
    Selection* choose_node(int index, bool called_by_director = false) const;
    void recount(int i1, int i2, int i3, int i4);
    void erase_from_list(Node *gone);
    /*Visibility*/
    void Vtoggle();
    /*Other*/
    bool H_is_consistent() const;
};


class Event : public Node {
    Date date;
    Time time;
    list <Market *> markets;
public:
    Event(string nodeid, string event, Date d, Time t, bool h = false)
            : Node(nodeid, event, h), date(d) , time(t) {}
    Event(const Event &event, string nodeid = "0", string name = "undefined");
    ~Event();
    /*getters*/
    Date get_date() const { return date; }
    Time get_time() const { return time; }
    int num_of_options() const;
    /*prints*/
    void print(bool director) const;
    void print_node_options(bool called_by_director = false) const;
    string file_print() const;
    /*Event operations*/
    Market *add_market(string nodeid, string mark, bool hidden = false, bool settled = false);
    void copy_market(const Market &market);
    void new_market(string n, bool h);
    /*Node navigation*/
    Market* choose_node(int index, bool called_by_director = false) const;
    void recount(int i1, int i2, int i3);
    void erase_from_list(Node *gone);
    /*Visibility*/
    void Vtoggle();
    /*Other*/
    bool H_is_consistent() const;
};


class SubCategory : public Node {
    list <Event *> events;
public:
    SubCategory(string nodeid, string subcat, bool h = false)
            : Node(nodeid, subcat, h) {}
    SubCategory(const SubCategory &subcategory, string nodeid = "0", string name = "undefined");
    ~SubCategory();
    /*getter*/
    int num_of_options() const;
    /*prints*/
    void print(bool director) const;
    void print_node_options(bool called_by_director = false) const;
    string file_print() const;
    /*SubCategory operations*/
    Event* add_event(string nodeid, string event, Date d, Time t, bool hidden = false);
    void copy_event(const Event &event);
    void new_event(string n, Date d, Time t, bool h);
    /*Node Navigation*/
    Event* choose_node(int index, bool called_by_director = false) const;
    void recount(int i1, int i2);
    void erase_from_list(Node *gone);
    /*Visibility*/
    void Vtoggle();
    /*Other*/
    bool H_is_consistent() const;
};


class Category : public Node {
    list <SubCategory *> subcategories;
public:
    Category(string nodeid, string cat, bool h = false)
            : Node(nodeid, cat, h) {}
    Category(const Category &category, string nodeid = "0", string name = "undefined");
    ~Category();
    /*getter*/
    int num_of_options() const;
    /*prints*/
    void print(bool director) const;
    void print_node_options(bool called_by_director = false) const;
    string file_print() const;
    /*Category operations*/
    SubCategory* add_subcategory(string nodeid, string subcat, bool hidden = false);
    void copy_subcategory(const SubCategory &subcategory);
    void new_subcategory(string n, bool h);
    /*Node Navigation*/
    SubCategory* choose_node(int index, bool called_by_director = false) const;
    void recount(int i1);
    void erase_from_list(Node *gone);
    /*Visibility*/
    void Vtoggle();
    /*Other*/
    bool H_is_consistent() const;
};


class Hierarchy{
    static bool rep_is_fract;      // representation is fractional
    list <Category *> categories;
public:
    ~Hierarchy();
    /*getter*/
    int num_of_options() const;
    /*static methods*/
    static bool get_rep_is_fract() { return rep_is_fract; }
    static void toggle(){ rep_is_fract = !rep_is_fract; }
    /*prints*/
    void print(bool called_by_director = true) const;
    void print_node_options(bool called_by_director = false) const;
    string file_print() const;
    /*Hierarchy operations*/
    Category* add_category(string nodeid, string cat, bool hidden = false);
    void new_category(string n, bool h);
    void copy_category(const Category &category);
    /*Node navigation*/
    Category* choose_category(int index, bool called_by_director) const;
    void recount();
    void erase_from_list(Category *gone);
    /*Other*/
    bool H_is_consistent() const;
};

#endif //PROJECT_HIERARCHY_H