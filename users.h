#ifndef PROJECT_USERS_H
#define PROJECT_USERS_H

#include <iostream>
#include <list>
#include "System.h"
#include "bets.h"

using namespace std;

class RegisteredUser {                                                          // abstract class
protected:
    int id;
    string username, password, full_name;
public:
    RegisteredUser(int ID, string uname, string pword, string fname)
        : id(ID) , username(uname) , password(pword) , full_name(fname) {}
    virtual ~RegisteredUser() {}
    /*getters*/
    string get_username() { return username; }
    string get_full_name() { return full_name; }
    string get_password() { return password; }
    int get_id() { return id; }
    virtual double get_balance() { return -1.0; }
    virtual string file_print();
    virtual char get_status() { return 'A'; }
    virtual string get_lock_reason() { return ""; }
    /*setters*/
    void set_id(int ID) { id = ID; }
    /*prints*/
    virtual void print_user_options(bool market_level) const = 0;
    virtual void print_info(const int *length) const;
    virtual void pretty_print(const class System& S) const { cout << "Illegal user option\n"; }
    /*Navigation*/
    virtual void print_hierarchy_options(const class System &S) const;
    virtual void print_node_options(const Node *current) const;
    virtual Category *choose_category(class System &S, int inp);
    virtual Node *choose_node(class Node *current, int inp);
    /*Account options for a registered user*/
    virtual void Account(class System &S) { cout << "Illegal user option\n"; }
    bool Rename(class System &S);
    bool Password();
    /*virtual functions for Punter*/
    virtual void keep_track_of_path(class System &S, Node *category, Node *subcategory, Node *event, Node *market) {}       // only keep track of the path of Punters
    virtual void increase_balance(double amount){ cout << "Illegal user option\n"; }
    virtual bool add_freebet(double price){ return false; }
    virtual bool remove_freebet(double price){ return false; }
    virtual void save_changes(class System &S);
    virtual void Bets(const class System &S) const { cout << "Illegal user option\n"; }
    virtual bool lock_user(string &name) { cout << "Cannot lock/unlock a non-Punter user\n"; return false; }  // can only lock Punters
    virtual void Place(class System &S, string node_id, Market *market) { cout << "Illegal user option\n"; }
    /*virtual functions for trader*/
    virtual void show_bets(class System &S) const { cout << "Illegal user option\n"; }
    virtual bool Freebets(class System &S) { cout << "Illegal user option\n"; return false; }
    virtual void Void(class System &S, Market *market, bool called_by_director = false) { cout << "Illegal user option\n"; }
    virtual void Settle(class System &S, Market *market, bool called_by_director) { cout << "Illegal user option\n"; }
    /*virtual functions for director*/
    virtual void Save(class System &S) { cout << "Illegal user option\n"; }
    virtual void Users(class System &S) { cout << "Illegal user option\n"; }
    virtual bool Visibility(class System &S, Node *current, Market *market) { cout << "Illegal user option\n"; return false; }
    virtual bool Rename(class System &S, Node *current) { cout << "Illegal user option\n"; return false; }
    virtual bool New(class System &S, Category *category, SubCategory *subcategory, Event *event, Market *market, bool hidden) { cout << "Illegal user option\n"; return false; }
    virtual bool Copy(class System &S, Node *current, Category *category, SubCategory *subcategory, Event *event, Market *market) { cout << "Illegal user option\n"; return false; }
    virtual bool Delete(class System &S, Node *current, Category *category, SubCategory *subcategory, Event *event, Market *market) { cout << "Illegal user option\n"; return false; }
    virtual void Logs(class System &S) { cout << "Illegal user option\n"; }
    virtual void System_profit(const class System &S) { cout << "Illegal user option"; }
};


class Punter : public RegisteredUser {
    char status;
    string lock_reason;
    struct {                                                                    // only used to group this data (there was proven no need for an "abstraction layer"-class for wallet)
        double balance;
        list <double> freebets;
    } Wallet;
public:
    Punter(int ID, string uname, string pword, string fname, string lreason,
           char stat, double money, list <double> fbets)
            : RegisteredUser(ID, uname, pword, fname), lock_reason(lreason), status(stat) {
        Wallet.balance = money;
        Wallet.freebets = fbets;
    }
    ~Punter() { Wallet.freebets.clear(); }
    void keep_track_of_path(class System &S, Node *category, Node *subcategory, Node *event, Node *market);     // keeps track of a Punter's traversing through the Hierarchy and adds his location in the system logs
    /*getters*/
    char get_status() { return status; }
    string get_lock_reason() { return lock_reason; }
    double get_balance() { return Wallet.balance; }
    /*setters*/
    bool lock_user(string &name);
    void increase_balance(double amount){ Wallet.balance += amount; }
    /*prints*/
    void print_info(const int *length) const;
    string file_print();
    void print_freebets() const;
    void print_user_options(bool market_level) const;
    /*Punter's Account*/
    void Account(class System &S);
    void Bets(const class System &S) const;
    double Deposit();
    bool add_freebet(double price);
    bool remove_freebet(double price);
    /*Punter actions*/
    void Place(class System &S, string node_id,Market *);
    double cash_coupon(int num);
};


class Trader : public RegisteredUser {
public:
    Trader(int ID, string uname, string pword, string fname)
            : RegisteredUser(ID, uname, pword, fname) {}
    /*prints*/
    void print_info(const int *length) const;
    string file_print();
    void print_user_options(bool market_level) const;
    /*Trader actions*/
    void show_bets(class System &S) const;
    bool Freebets(class System &S);
    void Void(class System &S, Market *market, bool called_by_director);
    void Settle(class System &S, Market *market, bool called_by_director);
};


class Director : public Trader {
public:
    Director(int ID, string uname, string pword, string fname)
            : Trader(ID, uname, pword, fname) {}
    void save_changes(class System &S) {}                     // overridden to do nothing - a director has to manually save changes (if he wants to)
    /*prints*/
    void pretty_print(const class System& S) const;
    void print_info(const int *length) const;
    string file_print();
    void print_user_options(bool market_level) const;
    void print_hierarchy_options(const class System &S) const;
    void print_node_options(const Node *current) const;
    /*Navigation*/
    Category *choose_category(class System &S, int inp);      // overridden because a Director can navigate through hidden nodes as well
    Node *choose_node(class Node *current, int inp);          // likewise ^^
    /*Director Actions*/
    void Save(class System &S);
    bool Visibility(class System &S, Node *current, Market *market);
    bool Rename(class System &S, Node *current);
    bool New(class System &S, Category *category, SubCategory *subcategory, Event *event, Market *market, bool hidden);
    bool Copy(class System &S, Node *current, Category *category, SubCategory *subcategory, Event *event, Market *market);
    bool Delete(class System &S, Node *current, Category *category, SubCategory *subcategory, Event *event, Market *market);
    void Users(class System &S);
    void Void(class System &S, Market *market, bool called_by_director);
    void Settle(class System &S, Market *market, bool called_by_director);
    void Logs(class System &S);
    void System_profit(const class System &S);
};


#endif