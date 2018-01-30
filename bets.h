#ifndef PROJECT_BETS_H
#define PROJECT_BETS_H

#include <iostream>

using namespace std;

class bet {
    int bet_id , user_id;
    string node_id;
    double stake;
    char result;
public:
    bet(int betID, int userID, string nodeID, double price, char res)
            : bet_id(betID), user_id(userID), node_id(nodeID), stake(price), result(res) {}
    /*getters*/
    int get_bet_id() const { return bet_id; }
    int get_user_id() const { return user_id; }
    string get_node_id() const { return node_id; }
    double get_stake() const { return stake; }
    char get_result() const { return result; }
    /*setters*/
    bool set_result(char res);
    void set_id(int newid) { bet_id = newid; }
    void set_node_id(string newid){ node_id = newid; }
    /*prints*/
    void print(bool called_by_trader = false) const;
    string file_print(void) const;
};

#endif //PROJECT_BETS_H
