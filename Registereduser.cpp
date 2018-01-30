#include <iostream>
#include <list>
#include "System.h"

using namespace std;

/* RegisteredUser functions */
string RegisteredUser::file_print(){
    return itos(id) + " | " + username + " | " + full_name + "| " + password + " | ";
}

bool RegisteredUser::Rename(class System &S) {
    string new_name;
    cout << "Please enter new username: " ;
    cin >> new_name;
    ignore_cin_line();
    if ( S.find_user(new_name)!=NULL || new_name=="guest" || username.length()==0 ) {
        cout << "\nUsername already exists or it is not acceptable" << endl;
        return false;
    }
    username = new_name;
    cout << "\nRename was successful!" << endl;
    return true;
}

bool RegisteredUser::Password() {
    string pass1, pass2;
    cout << "Please enter new password: ";
    cin >> pass1;
    ignore_cin_line();
    cout << "\nPlease enter password again: ";
    cin >> pass2;
    ignore_cin_line();
    if (pass1 != pass2) {
        cout << "\nPasswords don't match!" << endl;
        return false;
    }
    else if ( !meets_password_specs(pass1) ){
        return false;
    }
    password = pass1;
    cout << "\nPassword changed successfully!" << endl;
    return true;
}

void RegisteredUser::print_info(const int *length) const {
    cout << id << n_times_char(length[0] - itos(id).length(),' ') << " | "
         << username << n_times_char(length[1]-username.length(),' ') << " | "
         << full_name << n_times_char(length[2]-full_name.length(),' ') << " | "
         << password << n_times_char(length[3]-password.length(),' ') << " | ";
}

void RegisteredUser::print_hierarchy_options(const class System &S) const {
    S.print_hierarchy_options();
}

void RegisteredUser::print_node_options(const Node *current) const {
    current->print_node_options();
}

Category* RegisteredUser::choose_category(class System &S, int inp) {
    return S.choose_category(inp);
}

Node* RegisteredUser::choose_node(Node *current, int inp) {
    return current->choose_node(inp);
}

void RegisteredUser::save_changes(class System &S) {
    S.save();
}



