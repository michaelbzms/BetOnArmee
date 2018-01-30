#include <iostream>
#include "System.h"

using namespace std;

System::System(const string SystemName,double per) : name(SystemName) , percent(per) {        // calls file_manager functions to initialize System's data members
    cout << "Initializing system...\n";
    profit = load_profit();
    int id = 1;
    RegisteredUser *user;
    while ( (user = get_user(id++))!=NULL ){
        userlist.push_back(user);
    }
    id = 1;
    bet *Bet;
    while ( (Bet = get_bet(id++))!=NULL ){
        betlist.push_back(Bet);
    }
    hierarchy = load_hierarchy();
    cout << "System initialized!\n";
}

System::~System(){
    cout << "Shutting down system...\n";
    for ( list<RegisteredUser *>::iterator p = userlist.begin() ;  p != userlist.end() ; p++ ){
        delete (*p);
    }
    userlist.clear();
    for ( list<bet *>::iterator p = betlist.begin() ; p != betlist.end() ; p++ ){
        delete (*p);
    }
    betlist.clear();
    delete hierarchy;
    cout << "Thank you for visiting "<< name <<"!\n";
}

bool System::save_userlogs() {
    cout << "Saving changes to users.csv...\n";
    if ( save_users(userlist) ) {
        cout << "Save successful!\n";
        return true;
    }
    cout << "Error whilst saving changes to users.csv\n";
    return false;
}

bool System::save_betlogs() {
    cout << "Saving changes to bets.csv...\n";
    if ( save_bets(betlist) ){
        cout << "Save successful!\n";
        return true;
    }
    cout << "Error whilst saving changes to bets.csv\n";
    return false;
}

bool System::save_Hlogs() {
    cout << "Saving changes to hierarchy.dat...\n";
    if ( save_hierarchy(hierarchy) ){
        cout << "Save successful!\n";
        return true;
    }
    cout << "Error whilst saving changes to hierarchy.dat\n";
    return false;
}

bool System::save() {
    save_profit(profit);                                         // save profits on the file on every save to make sure that they are stored even if the program crashes unexpectedly ( as opposed to saving them once on System's Destructor per say )
    string action;
    while ( ( action = get_from_logs() ) != "empty" ){
        add_to_history(action);
    }
    return ( save_userlogs() && save_betlogs() && save_Hlogs() );
}

RegisteredUser *System::find_user(string uname) {
    RegisteredUser *user = NULL;
    for (list<RegisteredUser *>::iterator u = userlist.begin(); u != userlist.end(); u++) {
        if ((*u)->get_username() == uname) {
            user = *u;
            break;
        }
    }
    return user;
}

RegisteredUser *System::find_user(int id) {
    RegisteredUser *user = NULL;
    for (list<RegisteredUser *>::iterator u = userlist.begin(); u != userlist.end(); u++) {
        if ((*u)->get_id() == id) {
            user = *u;
            break;
        }
    }
    return user;
}

void System::add_user(RegisteredUser *newuser) {
    userlist.push_back(newuser);
}

void System::view_users(string substring) const {
    int length[7];
    for ( int i = 0 ; i<7 ; i++ ) length[i] = get_user_field_max_length(i);
    cout << "user_id" << n_times_char(length[0]-7,' ') << " | username" << n_times_char(length[1]-8,' ')
         << " | full name" << n_times_char(length[2]-9,' ') << " | password" << n_times_char(length[3]-8,' ')
         << " | type" << n_times_char(length[4]-4,' ') << " | status" << n_times_char(length[5]-6,' ')
         << " | balance" << n_times_char(length[6]-7,' ') << " | freebets" << endl;
    for ( int i = 0 ; i<7 ; i++ ) cout << n_times_char(length[i],'-')  << "-+-";
    cout << "------------\n";                                   // just some arbitery number for freebets (last column)
    for (list<RegisteredUser *>::const_iterator p = userlist.begin(); p != userlist.end(); p++) {
        if ( substring=="" || ((*p)->get_username()).find(substring) != string::npos ) {
            (*p)->print_info(length);                           // prints user info in a line , taking into account the max length of each field
        }
    }
}

void System::show_bets(int num) const {
    int counter = 1;
    for ( list<bet *>::const_iterator p = betlist.begin() ; p != betlist.end() ; p++ , counter++){
        if ( (counter) >= num ) break;
        cout << "Bet#" << counter << " : ";
        (*p)->print(true);
    }
}

bool System::grant_coupon_to(string username, double worth) {
    for ( list<RegisteredUser *>::iterator p = userlist.begin() ;  p != userlist.end() ; p++ ){
        if ( (*p)->get_username() == username ){
            return (*p)->add_freebet(worth);
        }
    }
    return false;
}

void System::add_bet(int user_id,string node_id,double stake) {
    bet *newbet = new bet(betlist.size()+1,user_id,node_id,stake,'-');
    betlist.push_back(newbet);
}

void System::print_hierarchy(bool director) const {
    hierarchy->print(director);
}

void System::print_hierarchy_options(bool called_by_director) const {
    hierarchy->print_node_options(called_by_director);
}

Category *System::choose_category(int inp, bool called_by_director) {
    return hierarchy->choose_category(inp, called_by_director);
}

void System::new_category(string name, bool h) {
    hierarchy->new_category(name, h);
}

void System::copy_category(const Category &category) {
    hierarchy->copy_category(category);
}

void System::show_bets_of_user(int id, bool called_by_director) const {
    int counter = 1;
    for (list<bet *>::const_reverse_iterator b = betlist.rbegin();  b != betlist.rend();  b++) {
        // printing all the user's bets starting from the latest in order to achieve chronological order
        if (id == (*b)->get_user_id()) {
            cout << "Bet#" << counter++ << " : ";
            (*b)->print(called_by_director);            // never called by a director
        }
    }
}

void System::recount() {
    hierarchy->recount();
}

void System::adjust_bets(string nodeid) {
    for ( list<bet *>::iterator p = betlist.begin() ; p != betlist.end() ;){
        string temp = (*p)->get_node_id();
        temp.assign(temp, 0, nodeid.length());
        if ( nodeid == temp ) {                                 // compare the deleted Node's 'nodeid' with that of bets and if the bet was on a Selection that got deleted then:
            Void_forced((*p)->get_node_id());                   // 1.make necessary adjaustments (similar to Voiding a bet) before deleting it
            delete (*p);                                        // 2.delete it
            p = betlist.erase(p);                               // 3.and erase it from the betlist
        }
        else{
            p++;
        }
    }
    bet_recount();
    int i1,i2,i3,i4,i5,b1,b2,b3,b4,b5;
    break_node_id(nodeid,i1,i2,i3,i4,i5);                       // breaks the string nodeid to five integers (see utility.h)
    if ( i2==-1 ){                                              // deleted a category node
        for ( list<bet *>::iterator p = betlist.begin() ; p != betlist.end() ; p++ ){
            break_node_id((*p)->get_node_id(),b1,b2,b3,b4,b5);
            if ( i1 < b1 ) (*p)->set_node_id(concatinate_node_id(b1-1,b2,b3,b4,b5));
        }
    }
    else if ( i3==-1 ){                                         // deleted a subcategory node
        for ( list<bet *>::iterator p = betlist.begin() ; p != betlist.end() ; p++ ){
            break_node_id((*p)->get_node_id(),b1,b2,b3,b4,b5);
            if ( i2 < b2 ) (*p)->set_node_id(concatinate_node_id(b1,b2-1,b3,b4,b5));
        }
    }
    else if ( i4==-1 ){                                         // deleted an event node
        for ( list<bet *>::iterator p = betlist.begin() ; p != betlist.end() ; p++ ){
            break_node_id((*p)->get_node_id(),b1,b2,b3,b4,b5);
            if ( i3 < b3 ) (*p)->set_node_id(concatinate_node_id(b1,b2,b3-1,b4,b5));
        }
    }
    else if ( i5==-1 ){                                         // deleted a market node
        for ( list<bet *>::iterator p = betlist.begin() ; p != betlist.end() ; p++ ){
            break_node_id((*p)->get_node_id(),b1,b2,b3,b4,b5);
            if ( i4 < b4 ) (*p)->set_node_id(concatinate_node_id(b1,b2,b3,b4-1,b5));
        }
    }
    else{                                                       // deleted a selection node
        for ( list<bet *>::iterator p = betlist.begin() ; p != betlist.end() ; p++ ){
            break_node_id((*p)->get_node_id(),b1,b2,b3,b4,b5);
            if ( i5 < b5) (*p)->set_node_id(concatinate_node_id(b1,b2,b3,b4,b5-1));
        }
    }
}

void System::erase_from_list(Category *gone) {
    hierarchy->erase_from_list(gone);
}

void System::bet_recount() {
    int counter = 1;
    for ( list<bet *>::iterator p = betlist.begin() ; p != betlist.end() ; p++ , counter++ ){
        (*p)->set_id(counter);
    }
}

/// Reminder: A director cannot settle/void a hidden market/selection

void System::Void(Market *market, Selection *selection) {
    for ( list<bet *>::iterator p = betlist.begin() ; p != betlist.end() ; p++  ){      // for every bet in betlist
        if ( (*p)->get_node_id() == selection->get_nodeid() ){              // if a bet was made in desired selection
            (*p)->set_result('V');                                          // void bet
            RegisteredUser *user = find_user((*p)->get_user_id());          // find user who placed the bet
            if ( user!=NULL ){
                user->increase_balance((*p)->get_stake());                  // refund user
            }
            else { cout << "User is not registered anymore\n"; }
        }
    }
    selection->set_result('V');        // void selection
    cout << "Desired selection voided successfully!\n";
}

double System::Settle(Market *market, Selection *selection) {
    double system_profit = 0.0;
    for ( list<bet *>::iterator p = betlist.begin() ; p != betlist.end() ; p++  ){
        string temp = (*p)->get_node_id();
        temp.assign(temp, 0, market->get_nodeid().length());
        if ( market->get_nodeid() == temp ) {                           // if bet was placed on market "market"
            if ( (*p)->get_node_id() == selection->get_nodeid() ) {
                (*p)->set_result('W');
                RegisteredUser *user = find_user((*p)->get_user_id());          // find user who placed the winning bet
                if (user != NULL) {
                    double investment = (*p)->get_stake();
                    double user_profit = (selection->get_price()) * investment;
                    system_profit = user_profit * percent;
                    user->increase_balance(investment + user_profit - system_profit);          // reward user
                    profit += system_profit;             // increase system profit as a percentage of user's winnings
                } else { cout << "User is not registered anymore\n"; }
            } else if ( (*p)->get_result()!='V' ) {                     // only if it hasn't already been voided
                (*p)->set_result('L');
            }
        }
    }
    market->Render_result(selection->get_nodeid());                     // update the market's selections with winning/losing result
    market->set_settled(true);
    cout << "Market settled successfully!\n";
    return system_profit;
}

void System::Void_forced(string node_id) {                             // this is just for System::adjust_bets
    for ( list<bet *>::iterator p = betlist.begin() ; p != betlist.end() ; p++  ){
        if ( (*p)->get_node_id() == node_id ){
            (*p)->set_result('V');
            RegisteredUser *user = find_user((*p)->get_user_id());
            if ( user!=NULL ){
                user->increase_balance((*p)->get_stake());             // takes his money back
            }
            else { cerr << "User is not registered anymore\n"; }
        }
    }
}

string System::get_from_logs() {
    if ( !logs.empty() ) {
        string temp = logs.front();
        logs.pop();
        return temp;
    }
    else return "empty";       // returns :empty" if stack is empty
}

void System::add_to_logs(string action) { logs.push(action); }

bool System::H_is_consistent() const { return hierarchy->H_is_consistent(); }

int System::get_user_field_max_length(const int field) const {
    int max , temp;
    switch (field){
        case 0:                // id
            max = 7;           // because that's how long "user_id" is
            for ( list<RegisteredUser *>::const_iterator p = userlist.begin() ; p != userlist.end() ; p++ ) {
                if ( ( temp = itos( (*p)->get_id() ).length() ) > max ) max = temp;
            }
            break;
        case 1:                // username
            max = 8;           // because that's how long "username" is
            for ( list<RegisteredUser *>::const_iterator p = userlist.begin() ; p != userlist.end() ; p++ ) {
                if ( ( temp = (*p)->get_username().length() ) > max ) max = temp;
            }
            break;
        case 2:                // full name
            max = 9;           // because that's how long "full name" is
            for ( list<RegisteredUser *>::const_iterator p = userlist.begin(); p != userlist.end() ; p++ ) {
                if ( ( temp = (*p)->get_full_name().length() ) > max ) max = temp;
            }
            break;
        case 3:                // password
            max = 8;           // because that's how long "password" is
            for ( list<RegisteredUser *>::const_iterator p = userlist.begin() ; p != userlist.end() ; p++ ) {
                if ( ( temp = (*p)->get_password().length() ) > max ) max = temp;
            }
            break;
        case 4:                // type
            max = 4;           // because that is how long type is and unless we had 10000+ types , this covers our needs
            break;
        case 5:                // status
            max = 6;           // because that's how long "status" is
            for ( list<RegisteredUser *>::const_iterator p = userlist.begin() ; p != userlist.end() ; p++ ) {
                if ( ( temp = (*p)->get_lock_reason().length() ) > max ) max = temp;   // this works for all users
            }
            max += 3;          // "+ 3" because of the "L, "
            break;
        case 6:                // balance
            max = 7;           // because that's how long "balance" is
            for ( list<RegisteredUser *>::const_iterator p = userlist.begin() ; p != userlist.end() ; p++ ) {
                if ( ( temp = dtos( (*p)->get_balance() ).length() ) > max ) max = temp;
            }
            break;
        default:
            max = -1;
            break;
    }
    return max;
}
