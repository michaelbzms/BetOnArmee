#include <iostream>
#include "System.h"

/*Local function declarations*/
RegisteredUser *register_user(System &S);
RegisteredUser *login_user(System &S);
void Navigate(System &system,RegisteredUser *user);
void print_location(const string SystemName, Node *category, Node *subcategory, Node *event, Node *market);
void print_user_options(const RegisteredUser *user, bool home_level, bool market_level);
void print_node_options(const System &system, const RegisteredUser *user, const Node *current);

using namespace std;


int main(int argc , char *argv[]) {
    System system("BetOnArme");
    int i;
    bool reg = false;
    for ( i=1 ; i<argc ; i++ ){             // check for execution parameters
        if ( argv[i][0]=='-' && argv[i][1]=='R' ){
            reg = true;                     // register
        }
    }
    RegisteredUser *user = NULL;
    if ( reg ){
        user = register_user(system);       // returns NULL if registration failed
        if ( user == NULL ){
            cout << "Registration failed.\n";
            return -1;
        }
    }
    else{
        user = login_user(system);          // returns NULL if entering as guest
        if ( user != NULL && user->get_status()=='L' ){
            cout << "Your account has been locked : " << user->get_lock_reason() << endl;
            return -2;
        }
    }
    // if user == NULL at this point then user entered as guest
    Navigate(system, user);                 // this function handles all input and navigation
    if ( user!=NULL ){
        user->save_changes(system);         // saves changes (without asking) only for Punter's and Trader's actions (Directors have to manually perform a Save)
        add_to_history(("User " + user->get_username() + " logged out"));      // Log-in and out messages must be added regardless of saving changes or not
    }
    return 0;
}



RegisteredUser *register_user(System &S) {
    RegisteredUser *user;
    string username, password, full_name;
    cout << "Please enter your full name , a username and a password\n"
         << "Username and password must not contain white spaces (everything afterwards will be ignored) and must not be empty\n"
         << "Password must be between 6 and 20 characters\n\n";
    cout << "Full name: ";
    getline(cin, full_name, '\n');
    cout << "Username: ";
    cin >> username;
    ignore_cin_line();
    if ( user_in_logs(username) ) {
        cout << "\nGiven username is already taken.\n";
        return NULL;
    }
    cout << "Password: ";
    cin >> password;
    ignore_cin_line();
    while ( !meets_password_specs(password) ) {
        cout << "Password: ";
        cin >> password;
        ignore_cin_line();
    }
    int ID = register_punter(username, full_name, password);     // registers new punter in users.csv and returns its ID
    user = new Punter(ID,username ,password,full_name,"",'A',0.0,list<double>());
    S.add_user(user);                                            // add user to system's user list
    cout << endl << "Registration successful!" << endl;
    add_to_history("A Punter named " + full_name + " was registered with the username \"" + username + "\"");
    return user;
}

RegisteredUser *login_user(System &S) {
    cout << "\nWelcome to " << S.get_name() << " - where betting is a solid experience\n\n Please enter your username and password to continue . If you do not already have an account , then enter guest:guest or a line_change (Enter) to enter as a guest.\n\n";
    RegisteredUser *user = NULL;
    string username , password;
    bool done = false;
    while ( !done ) {
        done = true;
        cout << "Username: ";
        if ( cin.peek()=='\n' ) return NULL;
        cin >> username;
        ignore_cin_line();
        if ( username=="guest:guest" ){
            return NULL;
        }
        if ( !user_in_logs(username) ){
            cout << "There is no registered user with that username. Please try again:\n\n";
            done = false;
        }
    }
    done = false;
    while ( !done ){
        done = true;
        cout << "\nPassword: ";
        cin >> password;
        ignore_cin_line();
        user = S.find_user(username);           // it is certain that user != NULL here ( !user_in_logs == false )
        if ( ! ( user->get_password() == password ) ){
            cout << "Incorrect password. Please try again:\n";
            done = false;
        }
    }
    if (user != NULL)
        add_to_history("User " + username + " logged in");
    return user;
}



void Navigate(System &system,RegisteredUser *user) {
    Node *current = NULL;                        // note: they all have to be 'Node *' because the rest are assigned to 'Node *current' (otherwise we would need downcasting)
    Node *category = NULL;
    Node *subcategory = NULL;
    Node *event = NULL;
    Node *market = NULL;
    bool changed_location = true;
    while (true) {
        cout << "_____________________________________________________________";
        if ( category!=NULL && user!=NULL && changed_location ) user->keep_track_of_path(system, category, subcategory, event, market);
        print_location(system.get_name(), category, subcategory, event, market);
        cout << "Price representation: " << ((Hierarchy::get_rep_is_fract()) ? "fractional" : "decimal") << endl;
        print_user_options(user, (current==NULL), (market!=NULL));
        print_node_options(system, user, current);
        int inp;
        cin >> inp;
        if (!cin.fail()) {                      // If input is an integer: change node
            changed_location = true;
            cin.clear();
            if (market != NULL) {               // already at last(Market) level cannot change node
                cout << "Illegal option (you are at market level)\n";
            } else {
                Node *new_current = NULL;
                if (current == NULL) {          // we're at Home level
                    if (user == NULL)
                        new_current = system.choose_category(inp);
                    else
                        new_current = user->choose_category(system, inp);
                    category = new_current;
                } else {
                    if (user == NULL)
                        new_current = current->choose_node(inp);
                    else
                        new_current = user->choose_node(current, inp);
                    if (subcategory == NULL) {          // Category level
                        subcategory = new_current;
                    } else if (event == NULL) {         // SubCategory level
                        event = new_current;
                    } else if (market == NULL) {        // Event level
                        market = new_current;
                    }
                }
                if (new_current == NULL) cout << "Node given does not exist\n";
                else current = new_current;       // to navigate through the Hierarchy we use the Node *current to point at the node the user is at
            }
        } else {                                  // If input was not an integer (=> cin.fail()==true) : user options menu
            changed_location = false;
            cin.clear();                          // reset fail bit from failing to read int
            string input;
            cin >> input;
            ignore_cin_line();
            if ( ( input == "Ba" || input == "Back" ) && current != NULL) {
                if (market != NULL) {             // Market level
                    current = event;
                    market = NULL;
                } else if (event != NULL) {       // Event level
                    current = subcategory;
                    event = NULL;
                } else if (subcategory != NULL) { // Subcategory level
                    current = category;
                    subcategory = NULL;
                } else if (category != NULL) {    // Category level
                    current = NULL;
                    category = NULL;
                }
            } else if (input == "H" || input == "Home") {
                current = category = subcategory = event = market = NULL;
            } else if (input == "T" || input == "Toggle") {
                Hierarchy::toggle();
                cout << "Price representation changed successfully\n";
            } else if (input == "X" || input == "Exit") {
                cout << "Exiting the System...\n";
                break;
            } else if (user != NULL) {                                     // user is a RegisteredUser
                if ( input == "Be" || input == "Bets" ) {                  // Trader options
                    user->show_bets(system);
                } else if ( input == "F" || input == "Freebets" ) {
                    user->Freebets(system);
                } else if ( (input == "Vo" || input == "Void") && market!=NULL ) {
                    user->Void(system,(Market *) market);
                } else if ( (input == "Se" || input == "Settle") && market!=NULL ) {
                    user->Settle(system, (Market *) market, false);
                } else if (input == "A" || input == "Account") {           // Punter options
                    user->Account(system);
                } else if ((input == "P" || input == "Place") && market != NULL) {
                    user->Place(system, market->get_nodeid(),(Market *) market);
                } else if ( input == "Sa" || input == "Save" ){            // Director options
                    user->Save(system);
                } else if ( input == "L" || input == "Logs" ){
                    user->Logs(system);
                } else if ( input == "U" || input == "Users" ){
                    user->Users(system);
                } else if ( input == "Vi" || input == "Visibility" ){
                    user->Visibility(system, current, (Market *) market);
                } else if ( input == "R" || input == "Rename" ){
                    user->Rename(system, current);
                } else if ( input == "N" || input == "New" ){
                    bool hidden = true;
                    if (market != NULL && market->num_of_options() >= 1) hidden = false;     // after new there will be at least 2 Selections in this Market, therefore new selection doesn't have to be hidden
                    user->New(system, (Category *) category, (SubCategory *) subcategory, (Event *) event, (Market *) market, hidden);
                } else if ( input == "C" || input == "Copy" ){
                    user->Copy(system, current, (Category *) category, (SubCategory *) subcategory, (Event *) event, (Market *) market);
                } else if ( input == "D" || input == "Delete" ) {
                    user->Delete(system, current, (Category *) category, (SubCategory *) subcategory, (Event *) event, (Market *) market);
                } else if ( input == "SP" || input == "System_profit" ){
                    user->System_profit(system);
                } else if ( input == "PP" || input == "Pretty_print" ){   // "Easter Egg" - hidden command (was mainly used for debugging)
                    user->pretty_print(system);
                }
                else {
                    cout << "Illegal user option\n";
                }
            } else {                                                      // user is guest and gave an invalid input as user option
                cout << "Illegal user option\n";
            }
        }
    }
}

void print_location(const string SystemName, Node *category, Node *subcategory, Node *event, Node *market) {
    cout << endl << endl << "Location:/" << SystemName;
    if (category != NULL) {
        cout << "/" + category->get_name();
        if (subcategory != NULL) {
            cout << "/" + subcategory->get_name();
            if (event != NULL) {
                cout << "/" + event->get_name();
                if (market != NULL) {
                    cout << "/" + market->get_name();
                }
            }
        }
    }
    cout << endl;
}

void print_user_options(const RegisteredUser *user, bool home_level, bool market_level) {
    cout << "Choose:" << endl;
    if (! market_level)                                         // if not at Market level
        cout << "the ID of the node you wish to visit" << endl;
    if (! home_level)                                           // if not at Home level
        cout << "Ba (Back), to return to the previous node level" << endl;
    cout << "H (Home), to return to home screen" << endl;
    cout << "T (Toggle), to toggle the representation of values (decimal/fractional)" << endl;
    cout << "X (Exit), to exit the system" << endl;
    if (user != NULL) user->print_user_options(market_level);   // print options for a registered user (according to its type - polymorphism)
    cout << endl;
}

void print_node_options(const System &system, const RegisteredUser *user, const Node *current) {
    if (current == NULL) {                                      // if at Home level
        if (user != NULL) user->print_hierarchy_options(system);
        else system.print_hierarchy_options();
    } else {
        if (user != NULL) user->print_node_options(current);
        else current->print_node_options();
    }
    cout << endl;
}