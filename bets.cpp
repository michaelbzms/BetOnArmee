#include "bets.h"
#include "utility.h"

string bet::file_print(void) const {
    return ( itos(bet_id) + " \t| " + itos(user_id) + "\t| " + node_id + " | " +
             dtos(stake) + " | " + ctos(result) + "\n");
}

bool bet::set_result(char res) {
    if (result != '-')
        return false;
    result = res;
    return true;
}

void bet::print(bool called_by_trader) const {
    if (called_by_trader) cout << "user id = " << user_id << ", ";
    cout << "node id = " << node_id << ", "
         << "price = " << stake << ", "
         << "result = " << result << endl;
}