#ifndef PROJECT_UTILITY_H
#define PROJECT_UTILITY_H

#include <iostream>

using namespace std;

/*Declarations*/
double fract_to_real(int a, int b);
void real_to_fract(int &a,int &b,double real);
string itos(int integer);
string dtos(double real);
string ctos(char character);
void break_node_id(string node, int &i1, int &i2, int &i3, int &i4, int &i5);
string concatinate_node_id(int i1,int i2,int i3,int i4,int i5);

string n_times_char(int times,char ch);

inline bool meets_password_specs(const string password){
    if ( password.length()<6 || password.length()>20 ) {
        cout << "Password must be between 6 and 20 characters long\n";
        return false;
    }
    return true;
}

inline void ignore_cin_line(void){   // inline functions have internal linkage and thus can be included in a .h file
    string trash;
    getline(cin,trash,'\n');        // ignore everything until the first '\n'
}

struct Date{
    Date(int d = 1, int m = 1, int y = 1970) : day(d) , month(m) , year(y) { }
    int day , month , year;
    static Date read_date();
    string print(void) const {
        return ( itos(day) + "/" + itos(month) + "/" + itos(year) );
    }
};

struct Time{
    int hours , minutes;
    static Time read_time();
    Time(int h = 0, int m = 0) : hours(h) , minutes(m) { }
    string print(void) const {
        string Hours = itos(hours);
        string Minutes = itos(minutes);
        return ( ( Hours.length()<2 ? "0" : "")+ Hours + ":" + ( Minutes.length()<2 ? "0" : "" ) + Minutes );
    }
};

#endif //PROJECT_UTILITY_H
