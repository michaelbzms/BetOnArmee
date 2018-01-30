#include <iostream>
#include <list>
#include "System.h"
#include "users.h"

using namespace std;

string Punter::file_print(){
    string output = RegisteredUser::file_print();
    output += "1 | " + ctos(status);
    if ( status=='L' ) output += ", " + lock_reason;
    else output += " ";
    output += "| " + dtos(Wallet.balance) + " | ";
    for ( list<double>::iterator p = Wallet.freebets.begin()  ;  p != Wallet.freebets.end()  ;  p++ ){
        if ( p != Wallet.freebets.begin() ) output += " , ";
        output += dtos(*p);
    }
    return output;
}

void Punter::print_freebets() const {
    char ch = 'a';
    for (list<double>::const_iterator fb = Wallet.freebets.begin(); fb != Wallet.freebets.end(); fb++) {
        if ( fb != Wallet.freebets.begin() ) cout << " , ";
        cout << ch++ << ". " << (*fb);
    }
}

bool Punter::add_freebet(double price) {
    if ( Wallet.freebets.size() >= 26 ) return false;            // There are only 26 latin characters - We take it that a Punter cannot have more than 26 freebets
    Wallet.freebets.push_back(price);
    return true;
}

bool Punter::remove_freebet(double price) {
    for (list<double>::iterator fb = Wallet.freebets.begin();
         fb != Wallet.freebets.end(); fb++) {
        if ((*fb) == price) {
            Wallet.freebets.erase(fb);
            return true;
        }
    }
    return false;
}

void Punter::Account(class System &S) {
    cout << "Punter: " << username << endl;
    cout << "Balance: " << Wallet.balance << endl;
    cout << "Available coupons (freebets): ";
    print_freebets();
    cout << endl << endl << "Options:" << endl
         << "R (Rename), to change your username" << endl
         << "P (Password), to change your password" << endl
         << "B (Bets), to print your bet history" << endl
         << "D (Deposit), to increase your balance " << endl << endl;
    string input;
    cin >> input;
    ignore_cin_line();
    if (input == "R" || input == "Rename") {
        string previous = username;
        if ( Rename(S) )
            S.add_to_logs("Punter "+previous+" renamed his username to "+username);
    } else if ( input == "P" || input == "Password" ){
        if ( Password() )
            S.add_to_logs("Punter "+username+" changed his password");
    } else if ( input == "B" || input == "Bets" ){
        Bets(S);
        S.add_to_logs("Punter "+username+" saw his/hers bet history");
    } else if ( input == "D" || input == "Deposit" ){
        double money = Deposit();
        S.add_to_logs("Punter "+username+" deposited"+dtos(money)+"euros to his balance");
    } else{
        cout << "Illegal option\n";
    }
}

void Punter::Bets(const System &S) const {
    S.show_bets_of_user(id, false);
}

double Punter::Deposit() {
    double money;
    cout << "Please enter amount (in euros) to deposit:" << endl;
    cin >> money;
    if (money <= 0.0) {
        cout << "Invalid amount. Deposit failed." << endl;
        return -1.0;
    }
    increase_balance(money);
    cout << money << " euros were successfully deposited!" << endl;
    return money;
}

void Punter::print_info(const int *length) const {
    RegisteredUser::print_info(length);
    cout << "1" << n_times_char(length[4]-1,' ') << " | " << status;
    if ( status=='L' ) {
        cout << ", " << lock_reason << n_times_char(length[5]-(3+lock_reason.length()),' ');
    }
    else cout << n_times_char(length[5]-1,' ');
    cout << " | " << Wallet.balance << n_times_char(length[6]-dtos(Wallet.balance).length(),' ') << " | ";
    print_freebets();
    cout << endl;
}

void Punter::print_user_options(bool market_level) const {
    cout << "A (Account), to manage user's account" << endl;
    if ( market_level ){
        cout << "P (Place), to place a bet on a selection" << endl;
    }
}

bool Punter::lock_user(string &name) {        // called by a director for a Punter . Returns true for locking , false for unlocking a punter
    bool locked;
    name = username;
    string reason;
    if ( status=='A' ){
        status = 'L';
        cout << "Reason for locking this user: ";
        getline(cin,reason,'\n');
        if ( reason.length()==0 ) reason = "N/A";
        cout << "Successfully locked this punter\n";
        locked = true;
    }
    else{      // status == 'L'
        status = 'A';
        cout << "Comment about unlocking this user: ";
        getline(cin,reason,'\n');
        cout << "Successfully unlocked this punter\n";
        locked = false;
    }
    lock_reason = reason;
    return locked;
}


void Punter::Place(class System &S, string node_id, Market *market) {
    if ( market->get_settled() ){
        cout << "This market is already settled and therefore no bets are allowed\n";
        return;
    }
    cout << "Your balance is: " << Wallet.balance << endl;
    cout << "Available coupons are: ";
    print_freebets();
    cout << "\nChoose the ID of the selection that you wish to bet on: ";
    int nodeID;
    string cancel;
    cin >> nodeID;
    if ( cin.fail() ){
        cin.clear();
        cin >> cancel;
        ignore_cin_line();
        if ( cancel == "cancel" ) return;
        else{
            cout << "\nIllegal input.\n";
            return;
        }
    }
    Selection *selection = market->choose_node(nodeID);
    if ( selection==NULL ){
        cout << "Selection does not exist\n";
        return;
    }
    else if ( selection->get_result()=='V' ){
        cout << "This selection is voided and therefore you cannot bet on it\n";
        return;
    }
    ignore_cin_line();
    cout << "\nPlease enter the amount of money you wish to bet or the (lowercase) letter corresponding to the coupon you wish to use: ";
    double input_money , bet_money = 0.0 ;
    char coupon;
    bool again;
    do{
        again = false;
        cin >> input_money;
        if (cin.fail()) {
            cin.clear();
            cin >> coupon;
            ignore_cin_line();
            int num_of_coupon = coupon - 'a' + 1;
            bet_money = cash_coupon(num_of_coupon);
            if ( bet_money == -1 ) return;
        } else {
            ignore_cin_line();
            if ( input_money <= 0 ){
                cout << "You can only bet a positive amount. Please try again: ";
                again = true;
            }
            if ( Wallet.balance > 0 &&  Wallet.balance < input_money ) {
                cout << "Insufficient funds. Please enter a smaller amount: ";
                again = true;
            }
            else if ( Wallet.balance == 0 ){
                cout << "Your balance is zero. Please refill your wallet in your Account options\n";
                return;
            }
            else{
                bet_money = input_money;
            }
        }
    } while ( again );
    // Now to actually place the bet:
    Wallet.balance -= bet_money;
    S.add_bet(id, node_id+"."+itos(nodeID), bet_money);
    cout << "Bet successfully placed. Good luck!\n";
    S.add_to_logs("Punter "+username+" placed a bet with "+dtos(bet_money)+" euros on the Selection \""+market->get_name()+"\"/\""+selection->get_name()+"\" with price "+dtos(selection->get_price()));
}

double Punter::cash_coupon(int num) {
    int counter = 1;
    for ( list<double>::iterator p = Wallet.freebets.begin() ; p != Wallet.freebets.end() ; p++ ){
        if ( counter++ == num ){
            double value = (*p);
            Wallet.freebets.erase(p);
            return value;
        }
    }
    cout << "There is no such coupon to cash\n";
    return -1.0;
}

void Punter::keep_track_of_path(System &S, Node *category, Node *subcategory, Node *event, Node *market) {
    string path = "The punter " + username + " visited the node at Location:/" + S.get_name();
    if (category != NULL) {
        path += "/" + category->get_name();
        if (subcategory != NULL) {
            path += "/" + subcategory->get_name();
            if (event != NULL) {
                path += "/" + event->get_name();
                if (market != NULL) {
                    path += "/" + market->get_name();
                }
            }
        }
    }
    S.add_to_logs(path);
}

