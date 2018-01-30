#include <iostream>
#include "System.h"

using namespace std;

string Trader::file_print(){
    return RegisteredUser::file_print() + "2 | | |";
}

void Trader::show_bets(System &S) const {
    S.show_bets(20);
    S.add_to_logs("Trader/Director "+username+" saw the last 20 bets on the system");
}

void Trader::print_info(const int *length) const {
    RegisteredUser::print_info(length);
    cout << "2" << n_times_char(length[4]-1,' ') << " | "
         << n_times_char(length[5],' ') << " | "
         << n_times_char(length[6],' ') << " | " << endl;
}

void Trader::print_user_options(bool market_level) const {
    cout << "Be (Bets), to print the last 20 bets" << endl;
    cout << "F (Freebets), to grant a coupon to a punter" << endl;
    if ( market_level ){
        cout << "Vo (Void), to cancel a selection" << endl
             << "Se (Settle), to settle this market" << endl;
    }
}

bool Trader::Freebets(class System &S) {
    string username;
    cout << "Please enter the username of the user you wish to grant a coupon to : ";
    cin >> username;
    ignore_cin_line();
    double worth;
    cout << "\nPlease enter the value of the coupon: ";
    cin >> worth;
    ignore_cin_line();
    if ( S.grant_coupon_to(username,worth) ) {
        cout << "\nSuccessfully granted a coupon of " << worth << " euros to " << username << endl;
        S.add_to_logs("Trader/Director " + RegisteredUser::username + " granted a coupon of " + dtos(worth) + " euros to punter " + username);
        return true;
    }
    cout << "There is no registered user with that username or that user already exceeds the maximum amount of coupons allowed. Procedure failed.\n";
    return false;
}

void Trader::Void(class System &S, Market *market, bool called_by_director) {
    if ( market->num_of_options() <= 2 ){
        cout << "Cannot void a selection in this market as doing so would leave only one left\n";
        return;
    }
    cout << "Please choose the selection you wish to Void: ";
    market->print_node_options(called_by_director);
    int choice;
    cin >> choice;
    if ( cin.fail() ){
        cout << "Invalid input\n";
        return;
    }
    Selection *selection = market->choose_node(choice,called_by_director);
    if ( selection==NULL ){
        cout << "Node given does not exist\n";
        return;
    }
    else if ( selection->get_result()=='W' ||  selection->get_result()=='L' || selection->get_result()=='V' ){  // check if the selection is already determined
        cout << "The result of this selection has already been determined\n";
        return;
    }
    else if ( selection->get_hidden() ){             // could only be the case for a director
        cout << "Cannot void a selection that is hidden\n";
        return;
    }
    S.Void(market, selection);
    S.add_to_logs((!called_by_director ? "Trader" : "Director ") + username + " voided the selection \"" +
                          market->get_name() + "\"/\"" + selection->get_name() + "\"");     // assuming that the full path is not important for system logs
    S.save();
    cout << "Selection voided successfully!\n";
}

void Trader::Settle(class System &S, Market *market, bool called_by_director) {
    if ( market->get_settled() ){
        cout << "This market is already settled\n";
        return;
    }
    cout << "Please choose the selection of this market that won:\n";
    if ( called_by_director ) cout << "Keep in mind that no hidden selection can be the winning one\n";
    market->print_node_options(called_by_director);
    int choice;
    cin >> choice;
    if ( cin.fail() ){
        cout << "Invalid input\n";
        return;
    }
    Selection *winning_selection = market->choose_node(choice,called_by_director);
    if ( winning_selection==NULL ){
        cout << "Node given does not exist" << (called_by_director ? " or is hidden" : "" )   << ". Settle failed\n";
        return;
    }
    else if ( winning_selection->get_hidden() ){                // Directors are not allowed to set a selection that is hidden as the winning selection
        cout << "Selection chosen is hidden. Settle failed\n";
        return;
    }
    else if ( winning_selection->get_result()=='V' ){
        cout << "Selection chosen is Voided. Settle failed\n";
        return;
    }
    double system_profit = S.Settle(market, winning_selection);
    S.add_to_logs(( !called_by_director ? "Trader " : "Director ")+username+" settled the market \""+market->get_name()+"\" "
            "where the winning selection was "+winning_selection->get_name()+" for a total system profit of "+dtos(system_profit)+" euros");
    S.save();                         // System::Settle cannot fail
    cout << "Market settled successfully!\n";
}
