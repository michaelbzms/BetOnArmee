#include <iostream>
#include <sstream>
#include "utility.h"

using namespace std;

Date Date::read_date() {
    int d, m, y;
    cout << "Please enter day: ";
    cin >> d;
    while (d < 1 || d > 31) {
        cin.clear();
        ignore_cin_line();
        cout << "Please enter an integer between 1 and 31 for day: ";
        cin >> d;
    }
    cout << "Please enter month: ";
    cin >> m;
    while (m < 1 || m > 12) {
        cin.clear();
        ignore_cin_line();
        cout << "Please enter an integer between 1 and 12 for month: ";
        cin >> m;
    }
    cout << "Please enter year: ";
    cin >> y;
    while (y < 0) {
        cin.clear();
        ignore_cin_line();
        cout << "I am sorry but this system does not support events that happened B.C. Try again: ";
        cin >> y;
    }
    return Date(d, m, y);
}

Time Time::read_time() {
    int h, m;
    cout << "Please enter hours: ";
    cin >> h;
    while (h < 0 || h > 23) {
        cin.clear();
        ignore_cin_line();
        cout << "Please enter an integer between 0 and 23 for hours: ";
        cin >> h;
    }
    cout << "Please enter minutes: ";
    cin >> m;
    while (m < 0 || m > 59) {
        cin.clear();
        ignore_cin_line();
        cout << "Please enter an integer between 0 and 59 for minutes: ";
        cin >> m;
    }
    return Time(h, m);
}

double fract_to_real(int a, int b) {    // "a/b"
    return 1 + ((double ) a)/b;
}

long gcd(long a, long b) {      // using Euclid's algorithm to calculate GCD
    if (a == 0)
        return b;
    else if (b == 0)
        return a;

    if (a < b)
        return gcd(a, b % a);
    else
        return gcd(b, a % b);
}

void real_to_fract(int &a,int &b,double real){
    real--;
    real *= 1000;
    int i = (int) real;
    if ( i % 10 >= 5 ) i+=10;
    i/=10;
    double newreal = ((double) i)/100;
    a = (int) (newreal*100);
    b = 100;
    int GCD;
    GCD = gcd(a,b);   // if there is none it will be 1
    a /= GCD;
    b /= GCD;
}

string itos(int integer){
    stringstream ss;
    ss << integer;
    return ss.str();
}

string dtos(double real){
    stringstream ss;
    ss << real;
    return ss.str();
}

string ctos(char character){
    stringstream ss;
    ss << character;
    return ss.str();
}

void break_node_id(string node, int &i1, int &i2, int &i3, int &i4, int &i5) {
    i1 = i2 = i3 = i4 = i5 = -1;
    stringstream nodeid;
    nodeid << node;
    nodeid >> i1;
    if ( nodeid.peek()=='.' ){
        nodeid.ignore(1);
        nodeid >> i2;
        if ( nodeid.peek()=='.' ){
            nodeid.ignore(1);
            nodeid >> i3;
            if ( nodeid.peek()=='.' ){
                nodeid.ignore(1);
                nodeid >> i4;
                if ( nodeid.peek()=='.' ){
                    nodeid.ignore(1);
                    nodeid >> i5;
                }
            }
        }
    }
}

string concatinate_node_id(int i1,int i2,int i3,int i4,int i5){
    string nodeid = itos(i1);
    if ( i2!=-1 ){
        nodeid += "." + itos(i2);
        if ( i3!=-1 ){
            nodeid += "." + itos(i3);
            if (  i4!=-1 ){
                nodeid += "." + itos(i4);
                if ( i5!=-1 ){
                    nodeid += "." + itos(i5);
                }
            }
        }
    }
    return nodeid;
}

string n_times_char(int times,char ch){
    if ( times <=0 ) return "";
    stringstream ss;
    while ( times-- > 0 ){
        ss << ch;
    }
    return ss.str();
}