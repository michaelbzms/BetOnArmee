#ifndef PROJECT_SYSTEM_H
#define PROJECT_SYSTEM_H

#include <iostream>
#include <list>
#include <queue>
#include "hierarchy.h"
#include "users.h"
#include "file_manager.h"

using namespace std;


class System{
public:
    System(const string,double per = 0.05);
    ~System();
    /*getters*/
    string get_name() const { return name; }
    double get_profit() const { return profit; }
    int get_user_field_max_length(const int field) const;    // (For Users->View of Director actions) returns the maximum length of a selected field on the "user table" (just like users.csv)
    /*Punter*/
    void add_bet(int user_id,string node_id,double stake);   // adds a bet on betlist
    /*Trader*/
    void show_bets(int num) const;                           // shows last num bets
    void show_bets_of_user(int id, bool called_by_director) const;                    // shows all bets of the user with the given id
    bool grant_coupon_to(string username, double worth);
    void Void(Market *market, Selection *selection);
    void Void_forced(string node_id);                        // an alternative version of Void ; used when deleting a selection, so Punters that have bet on it, don't lose their money
    double Settle(Market *market, Selection *selection);
    /*Director*/
    bool save();
    bool save_userlogs();
    bool save_betlogs();
    bool save_Hlogs();
    void view_users(string substring = "") const;            // prints a table of users that have the given substring on their username or all of the users if given substring is the trivial one ("")
    void new_category(string name, bool h);
    void copy_category(const Category &category);
    /*Userlist operations*/
    void add_user(RegisteredUser *newuser);
    RegisteredUser* find_user(string uname);
    RegisteredUser* find_user(int id);
    /*Hierarchy Operations*/
    void print_hierarchy(bool called_by_director) const;
    void print_hierarchy_options(bool called_by_director = false) const;
    int num_of_options() { return hierarchy->num_of_options(); }     // returns the number of VISIBLE categories of the Hierarchy
    bool H_is_consistent() const;                            // check if the hierarchy is consistent (= each VISIBLE category has at least 1 VISIBLE subcategory, 1 VISIBLE event, 1 VISIBLE market, 2 VISIBLE selections)
    /*Node Navigation*/
    Category *choose_category(int inp, bool called_by_director = false);
    /*Clean up after deletion of a Node*/
    void recount();                                          // calls hierarchy::recount() which recounts the whole Hierarchy
    void bet_recount();                                      // recounts bet ids on betlist (necessary when a Selection upon a bet is placed gets deleted , at which point we also delete the bet with adjust_bets())
    void adjust_bets(string nodeid);                         // deletes bets upon a deleted (selection) Node (after "Void_forced()"-ing them) and adjusts the nodeid of the rest of the bets to accommodate for the (not necessarily a selection) Node deletion
    void erase_from_list(Category *gone);                    // calls Hierarchy::erase_from_list(...), which erases the list node of the deleted category
    /*Logs*/
    void add_to_logs(string action);
    string get_from_logs();
private:
    const string name;
    double percent , profit;
    list <RegisteredUser *> userlist;
    list <bet *> betlist;
    Hierarchy *hierarchy;
    queue <string> logs;                                      // history logs are kept in a FIFO queue and will be written on audit.log on system save. This is so that Unsaved actions are not (mistakenly) written on audit.logs
};


#endif //PROJECT_SYSTEM_H