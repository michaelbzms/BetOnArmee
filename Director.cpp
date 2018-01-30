#include <iostream>
#include <list>
#include "System.h"

using namespace std;

bool cancel_check(string s);                                                    // local function

string Director::file_print(){
    return RegisteredUser::file_print() + "3 | | |";
}

void Director::print_info(const int *length) const {
    RegisteredUser::print_info(length);
    cout << "3" << n_times_char(length[4]-1,' ') << " | "
         << n_times_char(length[5],' ') << " | "
         << n_times_char(length[6],' ') << " | " << endl;
}

void Director::pretty_print(const class System& S) const{
    S.print_hierarchy(true);
}

void Director::print_user_options(bool market_level) const {
    Trader::print_user_options(market_level);
    cout << "Sa (Save), to save and synchronizing system files" << endl;
    cout << endl << "Management:" << endl;
    cout << "L (Logs), to print the last 25 system actions" << endl;
    cout << "U (Users), to manage users" << endl;
    cout << "Vi (Visibility), to toggle the visibility of a node" << endl;
    cout << "R (Rename), to rename a node" << endl;
    cout << "N (New), to create a new node" << endl;
    cout << "C (Copy), to copy a node (as hidden)" << endl;
    cout << "D (Delete), to delete a node" << endl;
    cout << "SP (System_profit), to see the profit of the system" << endl;
}

void Director::print_hierarchy_options(const class System &S) const {
    S.print_hierarchy_options(true);
}

void Director::print_node_options(const Node *current) const {
    current->print_node_options(true);
}

Category* Director::choose_category(class System &S, int inp) {
    return S.choose_category(inp, true);
}

Node* Director::choose_node(Node *current, int inp) {
    return current->choose_node(inp, true);
}

void Director::Save(class System &S) {
    if ( S.save() ) {
        cout << "System changes saved successfully\n";
    }
    else cout << "Save failed!\n";
}

bool Director::Visibility(class System &S, Node *current, Market *market) {
    if (current == NULL)
        print_hierarchy_options(S);
    else
        print_node_options(current);
    cout << "Please enter the ID of the node you wish to change visibility of: ";
    int num;
    cin >> num;
    if (cin.fail()) {      // not an int
        cin.clear();
        cout << "Illegal option" << endl;
        return false;
    }
    // Finding requested node:
    Node *temp_node;
    if (current == NULL)
        temp_node = choose_category(S, num);
    else
        temp_node = choose_node(current, num);
    if (temp_node == NULL) {
        cout << "Node given does not exist\n";
        return false;
    }
    // Changing visibility:
    bool used_Vtoggle = false;
    if (market != NULL) {
        temp_node->toggle_visibility();
    } else {
        cout << "Do you also want to change visibility for this node's sub-hierarchy? (Y/N)" << endl;
        string confirm;
        cin >> confirm;
        ignore_cin_line();
        if (confirm == "N" || confirm == "n") {
            temp_node->toggle_visibility();             // Toggles only current node's visibility
        } else if (confirm != "Y" && confirm != "y") {
            cout << "Illegal option" << endl;
            return false;
        }
        else{
            temp_node->Vtoggle();       // Toggles visibility for current node as well as its sub-hierarchy
            used_Vtoggle = true;
        }
    }
    if ( ! S.H_is_consistent() ){                  // Ensures that the Director does not make the hierarchy inconsistent
        // Then we toggle everything that was just toggled, so as to return hierarchy to its previous (consistent) state
        cout << "You cannot change the visibility of that node as it would leave the hierarchy inconsistent!\n";
        if ( used_Vtoggle ) temp_node->Vtoggle();
        else temp_node->toggle_visibility();
        return false;
    }
    cout << "Node visibility changed successfully!" << endl;
    S.add_to_logs("Director "+username+" changed the visibility of the node "+temp_node->get_name());
    return true;
}

bool Director::Rename(class System &S, Node *current) {
    if (current == NULL)
        print_hierarchy_options(S);
    else
        print_node_options(current);
    cout << "Please enter the ID of the node you wish to rename, or enter (C/A) Cancel/Abort to cancel this process: ";
    int num;
    cin >> num;
    if (cin.fail()) {      // not an int
        cin.clear();
        string input;
        cin >> input;
        if (cancel_check(input))
            cout << "Node renaming was cancelled" << endl;
        else
            cout << "Illegal option" << endl;
        return false;
    }
    // Finding requested node:
    Node *temp_node;
    if (current == NULL)
        temp_node = choose_category(S, num);
    else
        temp_node = choose_node(current, num);
    if (temp_node == NULL) {
        cout << "Node given does not exist\n";
        return false;
    }
    // Renaming node:
    cout << "Please enter a new name for the selected node, or enter (C/A) Cancel/Abort to cancel this process: ";
    string input , previous;
    ignore_cin_line();
    getline(cin,input,'\n');
    if (cancel_check(input)) {
        cout << "Node renaming was cancelled" << endl;
        return false;
    }
    previous = temp_node->get_name();
    temp_node->rename(input);
    cout << "Node renaming was successful!" << endl;
    S.add_to_logs("Director "+username+" renamed the node "+previous+" to "+input);
    return true;
}

bool Director::New(class System &S, Category *category, SubCategory *subcategory, Event *event, Market *market, bool hidden) {
    cout << "Please enter a name for the new node, or enter (C/A) Cancel/Abort to cancel this process: ";
    string name;
    getline(cin, name, '\n');
    if (cancel_check(name)) {
        cout << "Node creation was cancelled" << endl;
        return false;
    }
    string kind , info ="";
    if (category == NULL) {             // creating Category
        kind = "category";
        S.new_category(name, hidden);
    } else if (subcategory == NULL){    // creating SubCategory
        kind = "subcategory";
        category->new_subcategory(name, hidden);
    } else if (event == NULL) {         // creating Event and requesting its Date and Time
        kind = "event";
        cout << "Please specify the date for the new Event as requested below" << endl;
        Date d = Date::read_date();
        cout << "Please specify the time for the new Event as requested below " << endl;
        Time t = Time::read_time();
        subcategory->new_event(name, d, t, hidden);
        info = " at "+d.print()+" , "+t.print();
    } else if (market == NULL) {       // creating Market
        kind = "market";
        event->new_market(name, hidden);
    } else {                           // (Market != NULL), creating Selection and requesting its price
        if ( market->get_settled() ){
            cout << "This market is settled. Cannot add a new selection now\n";
            return false;
        }
        kind = "selection";
        cout << "Please enter a price in decimal form (only) for the new Selection: ";
        double pr;
        cin >> pr;
        while (pr < 1) {
            cin.clear();
            ignore_cin_line();
            cout << "Price in decimal for price must be greater than 1. Price: ";
            cin >> pr;
        }
        ignore_cin_line();
        info = " with price "+dtos(pr);
        market->new_selection(name, pr, hidden);
    }
    cout << "Node creation was successful!" << endl;
    S.add_to_logs("Director "+username+" created a new "+kind+" by the name "+name+info);
    return true;
}

bool Director::Copy(class System &S, Node *current, Category *category, SubCategory *subcategory, Event *event, Market *market) {
    if (current == NULL)
        print_hierarchy_options(S);
    else
        print_node_options(current);
    cout << "Please enter the ID of the node you wish to copy, or enter (C/A) Cancel/Abort to cancel this process: ";
    int num;
    cin >> num;
    if (cin.fail()) {      // not an int
        cin.clear();
        string input;
        cin >> input;
        if (cancel_check(input))
            cout << "Node copying was cancelled" << endl;
        else
            cout << "Illegal option" << endl;
        return false;
    }
    // Finding requested node and its type:
    Node *choice;
    if ( current==NULL ) choice = choose_category(S,num);
    else choice = choose_node(current,num);
    if ( choice == NULL ) {
        cout << "Node given does not exist\n";
        return false;
    }
    // Copying node:
    string kind , name;                // used for log
    name = choice->get_name();
    if (category == NULL) {            // copying Category
        kind = "category";
        S.copy_category(*((Category *)choice));
    } else if (subcategory == NULL) {   // copying SubCategory
        kind = "subcategory";
        category->copy_subcategory(*((SubCategory *)choice));
    } else if (event == NULL) {         // copying Event
        kind = "event";
        subcategory->copy_event(*((Event *)choice));
    } else if (market == NULL) {        // copying Market
        kind = "market";
        event->copy_market(*((Market *)choice));
    } else {                            // (Market != NULL), copying Selection
        kind = "event";
        market->copy_selection(*((Selection *)choice));
    }
    cout << "Node copying was successful!" << endl;
    S.add_to_logs("Director "+username+" copied the "+kind+" "+name);
    return true;
}

bool Director::Delete(class System &S, Node *current, Category *category, SubCategory *subcategory, Event *event, Market *market) {
    /* If the path to current node contains any hidden nodes then Delete can always be executed.
     * Otherwise, consistency checks must be made so as to ensure that no node is left without
     * at least 1 visible option (or market without 2 visible selections). */
    if (current == NULL)
        print_hierarchy_options(S);
    else
        print_node_options(current);
    cout << "Please enter the ID of the node you wish to delete, or enter (C/A) Cancel/Abort to cancel this process: ";
    int num;
    cin >> num;
    if (cin.fail()) {      // not an int
        cin.clear();
        string input;
        cin >> input;
        if (cancel_check(input))
            cout << "Node deleting was cancelled" << endl;
        else
            cout << "Illegal option" << endl;
        return false;
    }
    // Finding requested node and its type:
    Node *choice;
    if ( current==NULL ) choice = choose_category(S,num);
    else choice = choose_node(current, num);
    if ( choice == NULL ) {
        cout << "Node given does not exist\n";
        return false;
    }
    // check if delete is possible
    if ( !choice->get_hidden() && ( market==NULL || ((Selection *)choice)->get_result()!='V' ) ) {
        // because we can delete any hidden Node , without checking the following ( a voided selection is treated as a hidden node)
        bool hidden_path = false;
        if (category != NULL && category->get_hidden()) hidden_path = true;
        else if (subcategory != NULL && subcategory->get_hidden()) hidden_path = true;
        else if (event != NULL && event->get_hidden()) hidden_path = true;
        else if (market != NULL && market->get_hidden()) hidden_path = true;
        if ((current == NULL && S.num_of_options() < 2) ||
            (current != NULL && current->num_of_options() < 2 && !hidden_path)) {
            cout << "Cannot delete this node as doing so would leave no visible options in this (visible) level!\n";
            return false;
        }
        if (market != NULL && market->num_of_options() < 3 && !hidden_path) {   // delete a selection
            cout << "Cannot delete a selection in this market as doing so would leave only one visible selection!\n";
            return false;
        }
    }
    // Delete confirmation:
    cout << "Are you sure you wish to delete node " << num << "? (Y/N)" << endl;
    string confirm;
    cin >> confirm;
    ignore_cin_line();
    if (confirm == "N" || confirm == "n") {
        cout << "Node deleting was cancelled" << endl;
        return false;
    } else if (confirm != "Y" && confirm != "y") {
        cout << "Illegal option" << endl;
        return false;
    }
    /* At this point, we delete the requested node and any bets made on selections belonging to it.
     * We also correct (recount) hierarchy nodes' id and correctly adjust any bets affected. */
    string kind , name;                 // used for log
    name = choice->get_name();
    S.adjust_bets(choice->get_nodeid());
    delete choice;
    if (category == NULL){              // deleting Category
        kind = "category";
        S.erase_from_list((Category *) choice);
    } else if (subcategory == NULL) {   // deleting SubCategory
        kind = "subcategory";
        current->erase_from_list((SubCategory *) choice);
    } else if (event == NULL) {         // deleting Event
        kind = "event";
        current->erase_from_list((Event *) choice);
    } else if (market == NULL) {        // deleting Market
        kind = "market";
        current->erase_from_list((Market *) choice);
    } else {                            // (Market != NULL), deleting Selection
        kind = "selection";
        current->erase_from_list((Selection *) choice);
    }
    S.recount();                         // recount all node ids in the Hierarchy
    cout << "Node deleting was successful!" << endl;
    S.add_to_logs("Director "+username+" deleted the "+kind+" "+name);
    return true;
}

void Director::Users(class System &S) {
    cout << "Options:\n" << "V (View), to show the user table\n"
         << "S (Search), to search for a user\n" << "L (Lock), to change the lock-status of a user\n";
    string input;
    cin >> input;
    ignore_cin_line();
    if ( input == "V" || input == "View" ){
        S.view_users();
        S.add_to_logs("Director "+username+" saw the users registered to the system");
    } else if ( input == "S" || input == "Search" ){
        string substring;
        cout << "Please write a part of the username to search to proceed: ";
        cin >> substring;
        ignore_cin_line();
        cout << endl;
        S.view_users(substring);
        S.add_to_logs("Director "+username+" searched for all registered users with the substring: "+substring);
    } else if (  input == "L" || input == "Lock" ){
        string username;
        cout << "Please enter the desired username: ";
        cin >> username;
        ignore_cin_line();
        RegisteredUser *user = S.find_user(username);
        if ( user==NULL ){
            cout << "There is no such user registered on the system\n";
        }
        else {
            string Pname;
            bool locked = user->lock_user(Pname);
            S.add_to_logs("Director " + username + (locked ? " locked" : " unlocked") + " the punter " + Pname);
        }
    } else{
        cout << "Illegal option\n";
    }
}

void Director::Void(class System &S, Market *market, bool called_by_director) {
    Trader::Void(S,market,true);
}

void Director::Settle(class System &S, Market *market, bool called_by_director) {
    Trader::Settle(S, market, true);
}

void Director::Logs(System &S) {
    cout << print_last_n_actions(25);
    S.add_to_logs("Director "+username+" saw the last 25 actions in the system's logs");
}

void Director::System_profit(const System &S){
    cout << "\nThe system profit is: " << S.get_profit() << endl;
}

bool cancel_check(string s) {       // case insensitive check for Cancel/Abort
    for (int i = 0; i < s.length(); i++) {
        s[i] = (char) tolower(s[i]);
    }
    return (s == "c" || s == "a" || s == "cancel" || s == "abort");
}
