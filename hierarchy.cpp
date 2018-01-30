#include "hierarchy.h"

bool Hierarchy::rep_is_fract = true;     // true by default

/// Reminder - Hierarchy levels:
/// Category --> SubCategory --> Event --> Market --> Selection

/* CONSTRUCTORS & DESTRUCTORS */
/* Copy Constructors, where they exist, copy the characteristics of the desired object,
 * appending " - Copied" to its name and allocating memory for the creation of the new sub-hierarchy
 * (instead of copying pointers to the original one). In order to achieve that, a couple of arguments
 * are required, which do have (unused) default values (in .h) in accordance with the definition of CCs. */
Hierarchy::~Hierarchy() {
    for ( list<Category *>::iterator p = categories.begin() ;  p != categories.end() ; p++ ){
        delete (*p);
    }
}

Category::Category(const Category &category, string nodeid, string name)
        : Node(category, nodeid, name, true) {
    for (list<SubCategory *>::const_iterator p = category.subcategories.begin();  p != category.subcategories.end();  p++) {
        SubCategory *temp = (*p);
        subcategories.push_back(new SubCategory(*temp, node_id + "." + itos(subcategories.size()+1), (*p)->get_name()));
    }
}

Category::~Category() {
    for (list<SubCategory *>::const_iterator p = subcategories.begin();  p != subcategories.end();  p++) {
        delete (*p);
    }
}

SubCategory::SubCategory(const SubCategory &subcategory, string nodeid, string name)
        : Node(subcategory, nodeid, name, true) {
    for (list<Event *>::const_iterator p = subcategory.events.begin();  p != subcategory.events.end();  p++) {
        Event *temp = (*p);
        events.push_back(new Event(*temp, node_id + "." + itos(events.size()+1), (*p)->get_name()));
    }
}

SubCategory::~SubCategory() {
    for (list<Event *>::const_iterator p = events.begin();  p != events.end();  p++) {
        delete (*p);
    }
}

Event::Event(const Event &event, string nodeid, string name)
        : Node(event, nodeid, name, true), date(event.date), time(event.time) {
    for (list<Market *>::const_iterator p = event.markets.begin();  p != event.markets.end();  p++) {
        Market *temp = (*p);
        markets.push_back(new Market(*temp, node_id + "." + itos(markets.size()+1), (*p)->get_name()));
    }
}

Event::~Event() {
    for (list<Market *>::const_iterator p = markets.begin();  p != markets.end();  p++) {
        delete (*p);
    }
}

Market::Market(const Market &market, string nodeid, string name)
        : Node(market, nodeid, name, true) , settled(market.settled) {
    for (list<Selection *>::const_iterator p = market.selections.begin();  p != market.selections.end();  p++) {
        Selection *temp = (*p);
        selections.push_back(new Selection(*temp, node_id + "." + itos(selections.size()+1), (*p)->get_name()));
    }
}

Market::~Market() {
    for (list<Selection *>::const_iterator p = selections.begin();  p != selections.end();  p++) {
        delete (*p);
    }
}

/* ADD NODE */
Category *Hierarchy::add_category(string nodeid, string cat, bool hidden) {
    Category *new_cat = new Category(nodeid, cat, hidden);
    categories.push_back(new_cat);
    return new_cat;
}

SubCategory *Category::add_subcategory(string nodeid, string subcat, bool hidden) {
    SubCategory *new_subcat = new SubCategory(nodeid, subcat, hidden);
    subcategories.push_back(new_subcat);
    return new_subcat;
}

Event *SubCategory::add_event(string nodeid, string event, Date d, Time t, bool hidden) {
    Event *new_event = new Event(nodeid, event, d, t, hidden);
    events.push_back(new_event);
    return new_event;
}

Market *Event::add_market(string nodeid, string mark, bool hidden, bool settled) {
    Market *new_mark = new Market(nodeid, mark, hidden,settled);
    markets.push_back(new_mark);
    return new_mark;
}

Selection *Market::add_selection(string nodeid, string sel, double pr, bool hidden, char result) {
    Selection *new_sel = new Selection(nodeid, sel, pr, hidden,result);
    selections.push_back(new_sel);
    return new_sel;
}

/* NEW & COPY */
void Hierarchy::new_category(string n, bool h) {
    categories.push_back(new Category(itos(categories.size()+1), n, h));
}

void Hierarchy::copy_category(const Category &category) {
    categories.push_back(new Category(category, itos(categories.size()+1), category.get_name()+" - Copied"));
}

void Category::new_subcategory(string n, bool h) {
    subcategories.push_back(new SubCategory(node_id + "." + itos(subcategories.size()+1), n, h));
}

void Category::copy_subcategory(const SubCategory &subcategory) {
    subcategories.push_back(new SubCategory(subcategory, node_id + "." + itos(subcategories.size()+1), subcategory.get_name()+" - Copied"));
}

void SubCategory::new_event(string n, Date d, Time t, bool h) {
    events.push_back(new Event(node_id + "." + itos(events.size()+1), n, d, t, h));
}

void SubCategory::copy_event(const Event &event) {
    events.push_back(new Event(event, node_id + "." + itos(events.size()+1), event.get_name()+" - Copied"));
}

void Event::new_market(string n, bool h) {
    markets.push_back(new Market(node_id + "." + itos(markets.size()+1), n, h));
}

void Event::copy_market(const Market &market) {
    markets.push_back(new Market(market, node_id + "." + itos(markets.size()+1), market.get_name()+" - Copied"));
}

void Market::new_selection(string n, double pr, bool h) {
    selections.push_back(new Selection(node_id + "." + itos(selections.size()+1), n, pr, h));
}

void Market::copy_selection(const Selection &selection) {
    selections.push_back(new Selection(selection, node_id + "." + itos(selections.size()+1), selection.get_name()+" - Copied"));
}

/* CHOOSE NODE */
/* Choosing the n-th (where n=index) node of the next hierarchy level.
 * Results may vary when called by a director, depending on the existence of hidden nodes
 * (selectable for directors but not for anyone else). */
Category *Hierarchy::choose_category(int index, bool called_by_director) const {
    int i = 1;
    Category *category = NULL;
    for (list<Category *>::const_iterator p = categories.begin();  p != categories.end();  p++) {
        if (!(*p)->get_hidden() || called_by_director) {
            if (i++ == index) {
                category = (*p);
                break;
            }
        }
    }
    return category;
}

SubCategory *Category::choose_node(int index, bool called_by_director) const {
    int i = 1;
    SubCategory *subcategory = NULL;
    for (list<SubCategory *>::const_iterator p = subcategories.begin();  p != subcategories.end();  p++) {
        if (!(*p)->get_hidden() || called_by_director) {
            if (i++ == index) {
                subcategory = (*p);
                break;
            }
        }
    }
    return subcategory;
}

Event *SubCategory::choose_node(int index, bool called_by_director) const {
    int i = 1;
    Event *event = NULL;
    for (list<Event *>::const_iterator p = events.begin();  p != events.end();  p++) {
        if (! (*p)->get_hidden() || called_by_director) {
            if (i++ == index) {
                event = (*p);
                break;
            }
        }
    }
    return event;
}

Market *Event::choose_node(int index, bool called_by_director) const {
    int i = 1;
    Market *market = NULL;
    for (list<Market *>::const_iterator p = markets.begin();  p != markets.end();  p++) {
        if (! (*p)->get_hidden() || called_by_director) {
            if (i++ == index) {
                market = (*p);
                break;
            }
        }
    }
    return market;
}

Selection *Market::choose_node(int index, bool called_by_director) const {
    int i = 1;
    Selection *selection = NULL;
    for (list<Selection *>::const_iterator p = selections.begin();  p != selections.end();  p++) {
        if (! (*p)->get_hidden() || called_by_director) {
            if (i++ == index) {
                selection = (*p);
                break;
            }
        }
    }
    return selection;
}

/* CHANGE VISIBILITY */
// Vtoggle() changes visibility for this, as well as it's sub-hierarchy, if any
void Category::Vtoggle(){
    toggle_visibility();
    for ( list<SubCategory *>::iterator p = subcategories.begin() ; p != subcategories.end() ; p++ ){
        (*p)->Vtoggle();
    }
}

void SubCategory::Vtoggle(){
    toggle_visibility();
    for ( list<Event *>::iterator p = events.begin() ; p != events.end() ; p++ ){
        (*p)->Vtoggle();
    }
}

void Event::Vtoggle(){
    toggle_visibility();
    for ( list<Market *>::iterator p = markets.begin() ; p != markets.end() ; p++ ){
        (*p)->Vtoggle();
    }
}

void Market::Vtoggle(){
    toggle_visibility();
    for ( list<Selection *>::iterator p = selections.begin() ; p != selections.end() ; p++ ){
        (*p)->Vtoggle();
    }
}

void Selection::Vtoggle(){
    toggle_visibility();
}

/* FILE PRINTS */
string Hierarchy::file_print() const {
    string file_str = "";
    for (list<Category *>::const_iterator p = categories.begin();  p != categories.end();  p++) {
        file_str += (*p)->file_print();
    }
    return file_str;
}

string Category::file_print(void) const {
    string file_str = node_id + " " + ( get_hidden() ? "[H] " : "") + name + "\n";
    for (list<SubCategory *>::const_iterator p = subcategories.begin();  p != subcategories.end();  p++) {
        file_str += (*p)->file_print();
    }
    return file_str;
}

string SubCategory::file_print(void) const {
    string file_str = node_id + " " + ( get_hidden() ? "[H] " : "") + name + "\n";
    for (list<Event *>::const_iterator p = events.begin();  p != events.end();  p++) {
        file_str += (*p)->file_print();
    }
    return file_str;
}

string Event::file_print(void) const {
    string file_str = node_id + " " + ( get_hidden() ? "[H] " : "") + name + "| " + date.print() + " " + time.print() + "\n";
    for (list<Market *>::const_iterator p = markets.begin();  p != markets.end();  p++) {
        file_str += (*p)->file_print();
    }
    return file_str;
}

string Market::file_print(void) const {
    string file_str = node_id + " " + ( get_hidden() ? "[H] " : "") + ( get_settled() ? "[S] " : "" ) + name + "\n";
    for (list<Selection *>::const_iterator p = selections.begin();  p != selections.end();  p++) {
        file_str += (*p)->file_print();
    }
    return file_str;
}

string Selection::file_print(void) const {
    int a, b;
    string file_str = node_id + " " + ( get_hidden() ? "[H] " : "");
    if ( get_result()=='V' ) file_str += "[V] ";
    else if ( get_result()=='W' ) file_str += "[W] ";
    else if ( get_result()=='L' ) file_str += "[L] ";
    file_str += name + "#";
    real_to_fract(a, b, price);
    file_str += itos(a) + "/" + itos(b) + "\n";
    return file_str;
}

/* PRINT NODE OPTIONS*/
/* Printing all visible options of the next hierarchy level, along with a counter.
 * It also prints any invisible options if and only if it has been called by a director.
 * For use of the printed counter refer to "CHOOSE NODE". */
void Hierarchy::print_node_options(bool called_by_director) const {
    cout << "Available options at \"Home\":" << endl;
    int i = 1;
    for (list<Category *>::const_iterator p = categories.begin(); p != categories.end(); p++) {
        if (!(*p)->get_hidden() || called_by_director) {
            cout << i++ << ". " << ( (*p)->get_hidden() ? "[Hidden] " : "" ) << (*p)->get_name() << endl;
        }
    }
}

void Category::print_node_options(bool called_by_director) const {
    cout << "Available options at \"" << name << "\":" << endl;
    int i = 1;
    for (list<SubCategory *>::const_iterator p = subcategories.begin(); p != subcategories.end(); p++) {
        if (!(*p)->get_hidden() || called_by_director) {
            cout << i++ << ". " << ( (*p)->get_hidden() ? "[Hidden] " : "" ) << (*p)->get_name() << endl;
        }
    }
}

void SubCategory::print_node_options(bool called_by_director) const {
    cout << "Available options at \"" << name << "\":" << endl;
    int i = 1;
    for (list<Event *>::const_iterator p = events.begin(); p != events.end(); p++) {
        if (!(*p)->get_hidden() || called_by_director) {
            cout << i++ << ". " << ( (*p)->get_hidden() ? "[Hidden] " : "" ) << (*p)->get_name();
            cout << " | " << ((*p)->get_date()).print() << " " << ((*p)->get_time()).print() << endl;
        }
    }
}

void Event::print_node_options(bool called_by_director) const {
    cout << "Available options at \"" << name << "\":" << endl;
    int i = 1;
    for (list<Market *>::const_iterator p = markets.begin(); p != markets.end(); p++) {
        if (!(*p)->get_hidden() || called_by_director) {
            cout << i++ << ". " << ( (*p)->get_hidden() ? "[Hidden] " : "" )
                 << ( (*p)->get_settled() ? "[Settled] " : "" ) << (*p)->get_name() << endl;
        }
    }
}

void Market::print_node_options(bool called_by_director) const {
    cout << "Available options at \"" << name << "\":" << endl;
    int i = 1;
    for (list<Selection *>::const_iterator p = selections.begin(); p != selections.end(); p++) {
        if (!(*p)->get_hidden() || called_by_director) {
            cout << i++ << ". " << ( (*p)->get_hidden() ? "[Hidden] " : "" );
            if ((*p)->get_result() == 'W') cout << "[Won] ";
            else if ((*p)->get_result() == 'L') cout << "[Lost] ";
            else if ((*p)->get_result() == 'V') cout << "[Voided] ";
            cout << (*p)->get_name() << " - ";
            if (Hierarchy::get_rep_is_fract()) {
                int a, b;
                real_to_fract(a, b, (*p)->get_price());
                cout << a << "/" << b << endl;
            } else {
                cout << (*p)->get_price() << endl;
            }
        }
    }
}

/* PRETTY PRINT - Can be accessed by entering "PP" */
/* These functions print the entirety of the hierarchy on the screen.
 * Apart from helping with debugging, it currently serves no other purpose.
 * It is pretty though... */
void Hierarchy::print(bool called_by_director) const {
    for (list<Category *>::const_iterator p = categories.begin();  p != categories.end();  p++) {
        if (!(*p)->get_hidden() || called_by_director) {
            (*p)->print(called_by_director);
        }
    }
}

void Category::print(bool called_by_director) const {
    // if (hidden && !called_by_director) then printing for this function wouldn't have been called
    cout << node_id << " ";
    if (hidden) cout << "[Hidden] ";
    cout << name << endl << "\\" << endl;
    for (list<SubCategory *>::const_iterator p = subcategories.begin();  p != subcategories.end();  p++) {
        if (!(*p)->get_hidden() || called_by_director) {
            (*p)->print(called_by_director);
        }
    }
}

void SubCategory::print(bool called_by_director) const {
    cout << "\\ " << node_id << " ";
    if (hidden) cout << "[Hidden] ";
    cout << name << endl << " \\" << endl;
    for (list<Event *>::const_iterator p = events.begin();  p != events.end();  p++) {
        if (!(*p)->get_hidden() || called_by_director) {
            (*p)->print(called_by_director);
        }
    }
}

void Event::print(bool called_by_director) const {
    cout << " |\\ " << node_id << " ";
    if (hidden) cout << "[Hidden] ";
    cout << name << endl << " | \\" << endl;
    for (list<Market *>::const_iterator p = markets.begin();  p != markets.end();  p++) {
        if (!(*p)->get_hidden() || called_by_director) {
            (*p)->print(called_by_director);
        }
    }
}

void Market::print(bool called_by_director) const {
    cout << " |  \\ " << node_id << " ";
    if (hidden) cout << "[Hidden] ";
    cout << name << endl;
    for (list<Selection *>::const_iterator p = selections.begin();  p != selections.end();  p++) {
        if (!(*p)->get_hidden() || called_by_director) {
            (*p)->print(called_by_director);
        }
    }
}

void Selection::print(bool called_by_director) const {
    cout << " |  |\\ " << node_id << " ";
    if ( hidden ) cout << "[Hidden] ";
    cout << name << " - ";
    if (Hierarchy::get_rep_is_fract()) {
        int a, b;
        real_to_fract(a, b, price);
        cout << a << "/" << b << endl;
    } else {
        cout << price << endl;
    }
}

/* RECOUNTS */
// Corrects nodes' id after a Delete
void Hierarchy::recount() {
    int i1, i2, i3, i4, i5, counter = 1;
    for ( list<Category *>::iterator p = categories.begin() ; p != categories.end() ; p++ , counter++){
        break_node_id((*p)->get_nodeid(),i1,i2,i3,i4,i5);
        (*p)->set_nodeid(concatinate_node_id(counter,i2,i3,i4,i5));  // i2,i3,i4,i5 here is -1
        (*p)->recount(counter);
    }
}

void Category::recount(int i1) {
    int i2, i3, i4, i5, trash, counter = 1;
    for ( list<SubCategory *>::iterator p = subcategories.begin() ; p != subcategories.end() ; p++ , counter++){
        break_node_id((*p)->get_nodeid(),trash,i2,i3,i4,i5);
        (*p)->set_nodeid(concatinate_node_id(i1,counter,i3,i4,i5));  // i3,i4,i5 here is -1
        (*p)->recount(i1, counter);
    }
}

void SubCategory::recount(int i1, int i2) {
    int i3, i4, i5, trash, counter = 1;
    for ( list<Event *>::iterator p = events.begin() ; p != events.end() ; p++ , counter++){
        break_node_id((*p)->get_nodeid(),trash,trash,i3,i4,i5);
        (*p)->set_nodeid(concatinate_node_id(i1,i2,counter,i4,i5));  // i4,i5 here is -1
        (*p)->recount(i1, i2, counter);
    }
}

void Event::recount(int i1, int i2, int i3) {
    int i4, i5, trash, counter = 1;
    for ( list<Market *>::iterator p = markets.begin() ; p != markets.end() ; p++ , counter++){
        break_node_id((*p)->get_nodeid(),trash,trash,trash,i4,i5);
        (*p)->set_nodeid(concatinate_node_id(i1,i2,i3,counter,i5));  // i5 here is -1
        (*p)->recount(i1, i2, i3, counter);
    }
}

void Market::recount(int i1, int i2, int i3, int i4) {
    int i5, trash, counter = 1;
    for ( list<Selection *>::iterator p = selections.begin() ; p != selections.end() ; p++ , counter++){
        break_node_id((*p)->get_nodeid(),trash,trash,trash,trash,i5);
        (*p)->set_nodeid(concatinate_node_id(i1,i2,i3,i4,counter));
    }
}

/* ERASE_FROM_LISTS */
void Hierarchy::erase_from_list(Category *gone) {
    for ( list<Category *>::iterator p = categories.begin() ; p != categories.end() ; p++){
        if ( *p == gone ){
            categories.erase(p);
            break;
        }
    }
}

void Category::erase_from_list(Node *gone) {
    for ( list<SubCategory *>::iterator p = subcategories.begin() ; p != subcategories.end() ; p++){
        if ( *p == gone ){
            subcategories.erase(p);
            break;
        }
    }
}

void SubCategory::erase_from_list(Node *gone) {
    for ( list<Event *>::iterator p = events.begin() ; p != events.end() ; p++){
        if ( *p == gone ){
            events.erase(p);
            break;
        }
    }
}

void Event::erase_from_list(Node *gone) {
    for ( list<Market *>::iterator p = markets.begin() ; p != markets.end() ; p++){
        if ( *p == gone ){
            markets.erase(p);
            break;
        }
    }
}

void Market::erase_from_list(Node *gone) {
    for ( list<Selection*>::iterator p = selections.begin() ; p != selections.end() ; p++){
        if ( *p == gone ){
            selections.erase(p);
            break;
        }
    }
}

/* NUMBER OF VISIBLE OPTIONS OF A NODE*/
int Market::num_of_options() const {
    int num = 0;
    for (list<Selection *>::const_iterator p = selections.begin();  p != selections.end();  p++) {
        if ( !(*p)->get_hidden() && (*p)->get_result()!='V' ) {              // Voided selections are treated like hidden nodes !
            num++;
        }
    }
    return num;
}

int Event::num_of_options() const {
    int num = 0;
    for (list<Market *>::const_iterator p = markets.begin();  p != markets.end();  p++) {
        if (!(*p)->get_hidden()) {
            num++;
        }
    }
    return num;
}

int SubCategory::num_of_options() const {
    int num = 0;
    for (list<Event *>::const_iterator p = events.begin();  p != events.end();  p++) {
        if (!(*p)->get_hidden()) {
            num++;
        }
    }
    return num;
}

int Category::num_of_options() const {
    int num = 0;
    for (list<SubCategory *>::const_iterator p = subcategories.begin();  p != subcategories.end();  p++) {
        if (!(*p)->get_hidden()) {
            num++;
        }
    }
    return num;
}

int Hierarchy::num_of_options() const {
    int num = 0;
    for (list<Category *>::const_iterator p = categories.begin();  p != categories.end();  p++) {
        if (!(*p)->get_hidden()) {
            num++;
        }
    }
    return num;
}

/* HIERARCHY_IS_CONSISTENT */
/* Hierarchy is defined as consistent when every visible node (obviously excluding selections)
 * contains at least 1 visible option and every visible market at least 2 visible selections. */
bool Hierarchy::H_is_consistent() const{
    for (list<Category *>::const_iterator p = categories.begin();  p != categories.end();  p++) {
           if ( ! (*p)->get_hidden() && !(*p)->H_is_consistent() ) return false;
    }
    return true;
}

bool Category::H_is_consistent() const{
    bool consistency = true , found_one = false;
    for (list<SubCategory *>::const_iterator p = subcategories.begin();  p != subcategories.end();  p++) {
        if ( ! (*p)->get_hidden() ){
            found_one = true;
            consistency = consistency && (*p)->H_is_consistent();
        }
    }
    return ( consistency && found_one);
}

bool SubCategory::H_is_consistent() const{
    bool consistency = true , found_one = false;
    for (list<Event *>::const_iterator p = events.begin();  p != events.end();  p++) {
        if ( ! (*p)->get_hidden() ){
            found_one = true;
            consistency = consistency && (*p)->H_is_consistent();
        }
    }
    return ( consistency && found_one);
}

bool Event::H_is_consistent() const{
    bool consistency = true , found_one = false;
    for (list<Market *>::const_iterator p = markets.begin();  p != markets.end();  p++) {
        if ( ! (*p)->get_hidden() ){
            found_one = true;
            consistency = consistency && (*p)->H_is_consistent();
        }
    }
    return ( consistency && found_one);
}

bool Market::H_is_consistent() const{
    int counter = 0;
    for (list<Selection *>::const_iterator p = selections.begin();  p != selections.end();  p++) {
        if ( ! (*p)->get_hidden() ){
            counter++;
        }
    }
    return (counter>=2);                                   // in order for a Hierarchy to be consistent every market must have at least 2 (NOT hidden) selections
}

/* REMAINING FUNCTIONS */
void Market::Render_result(string winning_nodeid) {        // updates the selection's result based on which selection won
    for ( list<Selection *>::iterator p = selections.begin() ; p != selections.end() ; p++  ){
        if ( winning_nodeid == (*p)->get_nodeid() )(*p)->set_result('W');
        else (*p)->set_result('L');
    }
}
