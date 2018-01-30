#ifndef PROJECT_FILE_MANAGER_H
#define PROJECT_FILE_MANAGER_H

#include <iostream>
#include <list>
#include "users.h"
#include "hierarchy.h"

using namespace std;

// Function descriptions and comments are on the .cpp file

/*users*/
bool user_in_logs(string username);
int register_punter(string username,string fullname,string password);
class RegisteredUser *get_user(int ID);                   // searches for given user with id==ID in users.cvs and returns a dynamically allocated object (from the users.h classes) according to its 'int type'
bool save_users(list<class RegisteredUser *> userlist);

/*bets*/
class bet *get_bet(int ID);
void add_bet(int user_id,string node_id, double stake);
bool save_bets(list <class bet *> betlist);

/*hierarchy*/
Hierarchy *load_hierarchy(void);
bool save_hierarchy(Hierarchy *);

/*history*/
void add_to_history(string action);
string print_last_n_actions(const unsigned int n);

/*profit*/
double load_profit();
void save_profit(double p);

#endif