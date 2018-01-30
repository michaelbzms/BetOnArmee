#include <iostream>
#include <fstream>
#include <list>
#include <stack>
#include "users.h"

using namespace std;

void fix_for_linux(string &str){                     // to combat the EOL issue with getline on linux
    if ( str[str.length()-1]=='\r' ) str = str.substr(0, str.length()-1);
}

/*USERS*/
bool user_in_logs(string username){                  // Checks if a username exists in users.csv directly
    ifstream userlogs("./SystemFiles/users.csv");
    if ( !userlogs ){
        cerr << " <!> Opening user.csv was unsuccessful\n";
        return false;
    }
    string field;
    getline(userlogs,field,'\n');                    // ignore first line as it countains headers
    while ( !userlogs.eof() ){
        getline(userlogs,field,'|');                 // field = <user_id>  and the '|' is "passed through"
        userlogs >> field;                           // now field = <username> . I used the operator>> here so that field has no whitespaces and because the username cannot contain any whitespace
        if ( username == field ){
            return true;
        }
        getline(userlogs,field,'\n');                // we do not need to read any more on this line
    }
    return false;
}

RegisteredUser *get_user(int ID) {                   // Returns a RegisteredUser * to the user with the given ID - returns NULL if no such ID exists
    ifstream userlogs("./SystemFiles/users.csv");
    if ( !userlogs ){
        cerr << " <!> Opening user.csv was unsuccessful\n";
        return NULL;
    }
    string field , username , full_name , password , Lreason;
    char status = '0';
    int type = 0 , id;
    double balance = -1.0;
    list <double> freebets;
    bool found = false;
    getline(userlogs,field,'\n');                    // ignore first line (it contains headers)
    while ( !userlogs.eof() ){
        userlogs >> id;
        if ( ID == id ){
            found = true;
            getline(userlogs,field,'|');             // field = *white_spaces*  and the '|' is "passed through"
            userlogs >> username;                    // With ">>" , username has no white spaces
            getline(userlogs,field,'|');             // pass username field
            while ( userlogs.peek()==' ' ) userlogs.ignore(1);  // ignore white space
            getline(userlogs,full_name,'|');         // pass full name field , but store it as well
            fix_for_linux(full_name);
            userlogs >> password;
            getline(userlogs,field,'|');             // pass password field
            userlogs >> type;                        // read 'int type'
            if ( type==1 ) {                         // if user is a Punter
                getline(userlogs, field, '|');       // pass type field
                while ( userlogs.peek()==' ' ) userlogs.ignore(1);
                userlogs >> status;
                while ( userlogs.peek()==' ' || userlogs.peek()==',' ){ userlogs.ignore(1); }
                getline(userlogs, Lreason, '|');     // pass status field , bust store Lreason - Locked reason
                fix_for_linux(Lreason);
                userlogs >> balance;
                getline(userlogs,field,'|');         // pass balance field
                double fbet;
                while ( true ){
                    while ( userlogs.peek()==' ' || userlogs.peek()==',' ){ userlogs.ignore(1); }  // ignore white spaces and commas
                    if ( userlogs.peek()<'0' || userlogs.peek()>'9' ) break;
                    userlogs >> fbet;
                    freebets.push_back(fbet);
                }
            }
            break;
        }
        else {
            getline(userlogs, field, '\n');          // it's not this user so we do not need to read any more on this line
        }
    }
    if ( !found ) return NULL;
    RegisteredUser *USER;
    switch ( type ){                                 // allocates the 'right' type of RegisterdUser for the to-be-returned object according to 'int type' read (this is to achieve polymorphism)
        case 1:
            USER = new Punter(ID,username ,password,full_name,Lreason,status,balance,freebets);
            break;
        case 2:
            USER = new Trader(ID,username ,password,full_name);
            break;
        case 3:
            USER = new Director(ID,username ,password,full_name);
            break;
        default:
            cerr << " <!> Unknown user type in users.csv\n";
            USER = NULL;
            break;
    }
    return USER;
}

int register_punter(string username,string fullname,string password){   // registers a Punter into the System by directly adding him on users.csv
    int id;
    {   // find the id for the new user
        ifstream idsource("./SystemFiles/users.csv");
        int idcounter = 0;
        string trash;
        while ( !idsource.eof() ){
            getline(idsource,trash,'\n');
            if ( trash.length()>1 ) idcounter++;     // if is just in case we read an empty line (or '\r' for that matter)
        }
        id = idcounter;
    }
    ofstream userlogs("./SystemFiles/users.csv",ios::app);         // ios::app (append) to write on the end of the file (this way we also do not lose previous contents)
    if ( !userlogs ) {
        cerr << " <!> Opening user.csv was unsuccessful\n";
        return -1;
    }
    userlogs << id << "\t| " << username << " | " << fullname << " | " << password << " | " << 1 << " | " << "A" << " | " << 0.0 << " |" << endl;
    return id;
}

bool save_users(list<class RegisteredUser *> userlist){         // recreates users.csv from a list of users
    if ( remove("./SystemFiles/users.csv") != 0 ) {                // remove old users.csv
        perror("Error deleting previous users.csv file");
        return false;
    }
    ofstream logs("./SystemFiles/users.csv");                      // make a new one
    logs << "user_id | username | full name | password | type | status | balance | freebets" << endl;
    int id = 1;
    for ( list<RegisteredUser *>::iterator  p = userlist.begin()  ;  p != userlist.end()  ;  p++ ){
        (*p)->set_id(id++);                          // this action might be unnecessary whilst users cannot "un-register" but it is still safer to do so
        logs << (*p)->file_print() << endl;
    }
    return true;
}

/*BETS*/
bet *get_bet(int ID) {                              // returns a bet * to the bet with the given id in bets.csv - returns NULL if that bet does not exist
    ifstream betlogs("./SystemFiles/bets.csv");
    if ( !betlogs ){
        cerr << " <!> Opening bets.csv was unsuccessful\n";
        return NULL;
    }
    string field;
    getline(betlogs,field,'\n');                    // ignore first line as it only contains headers
    int bet_id = -1 , user_id = -1;
    string node_id = "";
    double stake = -1.0;
    char result = '-';
    bool found = false;
    while ( !betlogs.eof() ){
        betlogs >> bet_id;
        if ( bet_id == ID ){                        // this is the bet we were looking for
            found = true;
            getline(betlogs,field,'|');             // pass bet_id field
            betlogs >> user_id;
            getline(betlogs,field,'|');             // pass user_id field
            betlogs >> node_id;                     // node_id is a string  , ex.: "1.2.1.1.2" , with no whitespaces
            getline(betlogs,field,'|');             // pass node_id field
            betlogs >> stake;
            getline(betlogs,field,'|');             // pass stake field
            while ( betlogs.peek()==' ' ) betlogs.ignore(1); // ignore whitespace
            betlogs >> result;
            break;
        }
        else{
            getline(betlogs,field,'\n');            // ignore this line
        }
    }
    if ( !found ) return NULL;
    bet *Bet = new bet(bet_id,user_id,node_id,stake,result);
    return Bet;
}

void add_bet(int user_id,string node_id, double stake){    // adds a bet directly in bets.csv
    int id;
    {   //  find the id for the new user
        ifstream idsource("./SystemFiles/bets.csv");
        int idcounter = 0;
        string trash;
        while ( !idsource.eof() ){
            getline(idsource,trash,'\n');
            if ( trash.length()>1 ) idcounter++;    // just in case we read an empty (or '\r') line
        }
        id = idcounter;
    }
    ofstream betlogs("./SystemFiles/bets.csv",ios::app);          // ios::app (append) to write on the end of the file (this way we also do not lose previous contents)
    if ( !betlogs ) {
        cerr << " <!> Opening bets.csv was unsuccessful\n";
        return;
    }
    betlogs << id << " \t| " << user_id << "\t| " <<  node_id << " | " << stake << " | " << endl;
}

bool save_bets(list <struct bet *> betlist) {
    if ( remove("./SystemFiles/bets.csv") != 0 ) {                // remove old bets.csv
        perror("Error deleting previous bets.csv file");
        return false;
    }
    ofstream bets("./SystemFiles/bets.csv");                      // create a new one
    bets << "bet_id  | user_id | node_id   | stake | result" << endl;
    for ( list<bet *>::iterator p = betlist.begin()  ;  p != betlist.end()  ;  p++ ){
        bets << (*p)->file_print();
    }
    return true;
}

/*HIERARCHY*/
void Check_visibility(ifstream &Hlogs, string &hidden) {    // reads a string parameter in "[ ]" in an ifstream and ignores whitespaces after the "]"
    if (Hlogs.peek() == '[' ) {
        Hlogs.ignore(1);
        getline(Hlogs, hidden, ']');
        while (Hlogs.peek() == ' ') Hlogs.ignore(1);        //ignore whitespace
    }
}

void Check_parameters(ifstream &Hlogs,string &param1,string &param2){  // checks up to two parameters in "[ ]" , ignoring whitespaces after the "]"s
    Check_visibility(Hlogs,param1);
    Check_visibility(Hlogs,param2);
}

Hierarchy *load_hierarchy(void){                    // returns a Hierarchy * to a 'class Hierarchy object' made according to hierarchy.dat ( which must be strictly as stated in README)
    ifstream Hlogs("./SystemFiles/hierarchy.dat");
    if ( !Hlogs ) {
        cerr << " <!> Opening hierarchy.dat was unsuccessful\n";
        return NULL;
    }
    int integer;                                    // we use it to read integers of whom the values need not be used
    string name , nodeid;
    Category *catptr = NULL;
    SubCategory *subcatptr = NULL;
    Event *eveptr = NULL;
    Market *marptr = NULL;
    Hierarchy *H = new Hierarchy();
    while ( !Hlogs.eof() && Hlogs.peek()!='\n' ) {  // read line-by-line
        Hlogs >> integer;
        if ( Hlogs.fail() ){                        // If there is no number to read then we must have reached the end of the hierarchy
            Hlogs.clear();
            break;
        }
        nodeid = itos(integer);
        if ( Hlogs.peek() != '.' ) {                // reading a category
            while ( Hlogs.peek()==' ' ) Hlogs.ignore(1);  //ignore whitespace
            string hidden = "";
            Check_visibility(Hlogs, hidden);
            getline(Hlogs,name,'\n');
            fix_for_linux(name);
            catptr = H->add_category(nodeid,name,( hidden=="H" || hidden=="Hidden"));
        }
        else{
            Hlogs.ignore(1);                        // ignore '.'
            Hlogs >> integer;
            nodeid += "." + itos(integer);
            if ( Hlogs.peek()!='.' ){               // reading a subcategory
                while ( Hlogs.peek()==' ' ) Hlogs.ignore(1);  //ignore whitespace
                string hidden = "";
                Check_visibility(Hlogs, hidden);
                getline(Hlogs,name,'\n');
                fix_for_linux(name);
                subcatptr = catptr->add_subcategory(nodeid,name,( hidden=="H" || hidden=="Hidden"));
            }
            else{
                Hlogs.ignore(1);
                Hlogs >> integer;
                nodeid += "." + itos(integer);
                if ( Hlogs.peek()!='.' ){           // reading an event
                    while ( Hlogs.peek()==' ' ) Hlogs.ignore(1);  //ignore whitespace
                    string hidden = "";
                    Check_visibility(Hlogs, hidden);
                    getline(Hlogs,name,'|');        //  "|" separates the event's name from its date and (afterwards) its time, ex : " <name> | dd/mm/yyyy hh:mm "
                    fix_for_linux(name);
                    int day , month , year;
                    Hlogs >> day;
                    Hlogs.ignore(1); // ignore '/'
                    Hlogs >> month;
                    Hlogs.ignore(1);
                    Hlogs >> year;
                    Date date(day,month,year);
                    while ( Hlogs.peek() == ' ' ) Hlogs.ignore(1);  // ignore white space
                    int hours , minutes;
                    Hlogs >> hours;
                    Hlogs.ignore(1); // ignore ':'
                    Hlogs >> minutes;
                    Time time(hours,minutes);
                    eveptr = subcatptr->add_event(nodeid,name,date,time,(hidden=="H" || hidden=="Hidden"));
                    getline(Hlogs,name,'\n');       // ignore the rest of the line
                }
                else{
                    Hlogs.ignore(1);
                    Hlogs >> integer;
                    nodeid += "." + itos(integer);
                    if ( Hlogs.peek()!='.' ){       // reading a market
                        while ( Hlogs.peek()==' ' ) Hlogs.ignore(1);  //ignore whitespace
                        string param1 , param2;
                        param1 = param2 = "";
                        Check_parameters(Hlogs, param1,param2);
                        getline(Hlogs,name,'\n');
                        fix_for_linux(name);
                        marptr = eveptr->add_market(nodeid, name,
                                                    (param1 == "H" || param2 == "H" || param1 == "Hidden" || param2 == "Hidden"),
                                                    (param1=="S" || param2=="S" || param1=="Settled" || param2=="Settled" ));
                    }
                    else{                           // reading a selection
                        Hlogs.ignore(1);
                        Hlogs >> integer;
                        nodeid += "." + itos(integer);
                        while ( Hlogs.peek()==' ' ) Hlogs.ignore(1);  //ignore whitespace
                        string param1 , param2;
                        param1 = param2 = "";
                        Check_parameters(Hlogs, param1,param2);
                        getline(Hlogs,name,'#');
                        int a , b;                  // Hierarchy.dat must have prices in fractional form : a/b
                        Hlogs >> a;
                        Hlogs.ignore(1);            // ignore '/'
                        Hlogs >> b;
                        double price = fract_to_real(a,b);     // converts fractional to real representation for price , as we store price (in class Hierarchy) as a double
                        char result;
                        if ( param1=="V" || param2=="V" || param1=="Voided" || param2=="Voided" ) result = 'V';
                        else if ( param1=="W" || param2=="W" || param1=="Win" || param2=="Win" ) result = 'W';
                        else if  ( param1=="L" || param2=="L" || param1=="Lose" || param2=="Loss" ) result = 'L';
                        else result = '-';
                        marptr->add_selection(nodeid, name, price,
                                              (param1 == "H" || param2 == "H" || param1 == "Hidden" || param2 == "Hidden"), result);
                        getline(Hlogs,name,'\n');   // ignore the rest of the line
                    }
                }
            }
        }
    }
    return H;
}

bool save_hierarchy(Hierarchy *H){
    if ( remove("./SystemFiles/hierarchy.dat") != 0 ) {           // remove old hierarchy
        perror("Error deleting previous hierarchy.dat file");
        return false;
    }
    ofstream Hdata("./SystemFiles/hierarchy.dat");                // make a new one
    Hdata << H->file_print();
    return true;
}

/*HISTORY*/
void add_to_history(string action) {
    ofstream history("./SystemFiles/audit.log",ios::app);         // ios::app (append) to write on the end of the file (this way we also do not lose previous contents)
    if ( !history ) {
        cerr << " <!> Opening audit.log was unsuccessful\n";
        return;
    }
    history << action << endl;
}

string print_last_n_actions(const unsigned int n){  // returns the last 20 actions in audit.log in the form of a string (actions seperated by '\n') , using a LIFO stack
    ifstream history("./SystemFiles/audit.log");
    if ( !history ) {
        cerr << " <!> Opening audit.log was unsuccessful\n";
        return "";
    }
    stack <string> S;
    while ( !history.eof() ){
        string temp;
        getline(history,temp,'\n');
        if ( temp.length()>1 ) S.push(temp);        // ">1" so it doesnt push an empty line
    }
    string out = "";
    if ( S.empty() ){
        out = "There are no log records of any system actions\n";
        return out;
    }
    for ( int i = 0 ; i<n && !S.empty() ; i++ ){
        out += S.top() + "\n";
        S.pop();
    }
    return out;
}

double load_profit() {                              // returns the profit of the system so far if found in profit.txt , else 0.0
    ifstream P("./SystemFiles/profit.txt");
    if ( !P ){
        cerr << " <!> Opening profit.txt was unsuccessful.System profit was set to 0\n";
        return 0.0;
    }
    double profit;
    P >> profit;
    if ( P.fail() ) return 0.0;
    return profit;
}

void save_profit(double p) {
    if ( remove("./SystemFiles/profit.txt") != 0 ) {
        perror("Error deleting previous profit.txt file.");
        return;
    }
    ofstream P("./SystemFiles/profit.txt");
    if ( !P ){
        cout << " <!> Creating profit.txt was unsuccessful";
        return;
    }
    P << p;
}
