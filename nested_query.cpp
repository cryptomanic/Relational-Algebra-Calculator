#include <bits/stdc++.h>
#include "postfix.h"

using namespace std;

// to store table
typedef vector<vector<string> > vvs;

// to store table and it's name
typedef pair<vector<vector<string> >, string> pvvs;

/*
    return an object of pvvs
    which stores table in the form of vvs and
    table name stored in table_name
*/
pvvs table_to_vvs(string table_name) {

    // open file
    fstream ob;
    ob.open(table_name + ".csv", ios::in);

    // throw error if file does not exist
    if(! ob.is_open()) {
        cerr<<"Table doesn't exist";
        throw -1;
    }

    // line by line read the file and store it in vvs
    string line, field;
    vvs result;
    while(getline(ob, line)) {

        vector<string> row;
        istringstream s(line);
        while(getline(s, field, ',')) {
            row.push_back(field);
        }

        result.push_back(row);
    }

    // close connection
    ob.close();
    return make_pair(result, table_name);
}

/*
    remove duplicate rows from input table
*/
vvs unique_rows(vvs table) {
    vvs result;
    result.push_back(table[0]);

    set<vector<string> > unique_rows;

    for(int i = 1; i < table.size();i++) {
        unique_rows.insert(table[i]);
    }

    for(auto it = unique_rows.begin(); it != unique_rows.end();it++) {
        result.push_back(*it);
    }

    return result;
}

/*
    remove spaces from query string
*/
string remove_spaces(string query) {
    string result;
    for(int i = 0;i < query.size();i++) {
        if(query[i] != ' ') result += query[i];
    }
    return result;
}

/*
    parameters
    ctable: a table
    query: must be of the form "cname1,cname2,cname3,cname4,cname5"
*/
pvvs project(pvvs ctable, string query) {
    vvs table = ctable.first;

    // map column name to column index
    map<string, int> colname_to_number;
    for(int i = 0;i < table[0].size(); i++) {
        colname_to_number[table[0][i]] = i;
    }

    vector<int> column_index;
    string column; int i = 0;

    // iterate through each column mentioned in the query and store its column index
    // as per colname_to_number in the vector column_index
    while(i < query.size()) {
          if(query[i] == ',') {
              // column to be projected must be present in the table
              if(colname_to_number.find(column) == colname_to_number.end()) {
                cerr<<"Column "<<column<<" is not present";
                throw -1;
              }
              column_index.push_back(colname_to_number[column]);
              column.clear();
          } else {
             column += query[i];
          }
          i++;
    }

    // column to be projected must be present in the table
    if(colname_to_number.find(column) == colname_to_number.end()) {
        cerr<<"Column "<<column<<" is not present";
        throw -1;
    }
    column_index.push_back(colname_to_number[column]);

    vvs result;

    // for each row of table extract values for all the index
    // stored in column_index
    for(int i = 0;i < table.size();i++) {
        vector<string> row;
        for(int j = 0;j < column_index.size();j++) {
            row.push_back(table[i][column_index[j]]);
        }
        result.push_back(row);
    }

    return make_pair(unique_rows(result), "");
}

/*
    display table on the console
    each row in new line
    columns in a row separated by space
*/
void display_vvs(vvs table) {
    for(int i=0;i<table.size();i++) {
        for(int j=0;j<table[0].size();j++) {
            cout<<table[i][j]<<" ";
        } cout<<"\n";
    }
}

/*
    select rows from a given table
    which satisfy the conditions stored
    in the query

    query: e.g. "A>B|(C<D&E=F)|age*5<30"
*/
pvvs select(pvvs ctable, string query) {
    vvs table = ctable.first;
    set<char> valid_operator;

    // operator supported
    valid_operator.insert('>');
    valid_operator.insert('=');
    valid_operator.insert('&');
    valid_operator.insert('+');
    valid_operator.insert('-');
    valid_operator.insert('*');
    valid_operator.insert(')');
    valid_operator.insert('(');
    valid_operator.insert('!');
    valid_operator.insert('|');
    valid_operator.insert('<');

    char sym = 'A'; // alphabet mapped to string
    map<char,string> alpha_to_string;

    // query : credits * 5 < 17 & (deptname = "Computer" | deptname = "Biology")
    // new_query : A * B < C & (D = E | F = G)
    string new_query;
    string valid;

    for(int i = 0;i < query.size();i++) {
         if(valid_operator.find(query[i]) == valid_operator.end()) {
            valid += query[i];
         }
         else if(valid.size() > 0) {
             alpha_to_string.insert(make_pair(sym,valid));
             new_query += sym;
             sym++;
             valid.clear();
             new_query += query[i];
         }
         else {
            new_query += query[i];
         }
     }

     if(valid.size() > 0) {
        alpha_to_string.insert(make_pair(sym, valid));
        new_query += sym;
        sym++;
     }

     int qsize = new_query.size(), j;
     char sym_temp = sym;

     // store postfix expression
     char postfix[qsize + 1];
     for(j = 0;j < qsize; j++) {
        postfix[j] = new_query[j];
     } postfix[j] = '\0';

     infixToPostfix(postfix);

     // evaluate the postfix expression

     string postfix_exp;
     for(int i = 0;postfix[i] != '\0';i++){
       postfix_exp += postfix[i];
     }

     map<string, int> colname_to_number;
     for(int i = 0;i < table[0].size(); i++) {
        colname_to_number[table[0][i]] = i;
     }

     vvs result;
     result.push_back(table[0]);


     for(int i = 1;i < table.size();i++) {
        vector<string> row = table[i];

        // time to solve postfix expression
        stack<char> solve_postfix;
        solve_postfix.push(postfix_exp[0]);

        int k = 1;
        while(not (solve_postfix.empty()) and k < postfix_exp.size()) {
            char ch = postfix_exp[k++];
            if(valid_operator.count(ch) == 0) {
                solve_postfix.push(ch);
            } else {
                // unary operator
                if(ch == '!') {
                    char zerone = solve_postfix.top();
                    solve_postfix.pop();
                    bool value = !(zerone - 48);
                    solve_postfix.push('0' + value);
                } else { // binary operator
                    char x = solve_postfix.top();
                    solve_postfix.pop();
                    char y = solve_postfix.top();
                    solve_postfix.pop();

                    string val1; val1 += x;
                    string val2; val2 += y;

                    if(alpha_to_string.find(x) != alpha_to_string.end()) {
                        val1 = alpha_to_string[x];
                    }
                    if(alpha_to_string.find(y) != alpha_to_string.end()) {
                        val2 = alpha_to_string[y];
                    }

                    if(colname_to_number.find(val1) != colname_to_number.end()) {
                        val1 = row[colname_to_number[val1]];
                    }
                    if(colname_to_number.find(val2) != colname_to_number.end()) {
                        val2 = row[colname_to_number[val2]];
                    }

                    if(val1[0] == '"'){
                        val1 = val1.substr(1, val1.size()-2);
                    }
                    if(val2[0] == '"'){
                        val2 = val2.substr(1, val2.size()-2);
                    }

                    if(ch == '&') {
                        solve_postfix.push('0' + ((val1[0]-48) & (val2[0]-48)));
                    }
                    if(ch == '|') {
                        solve_postfix.push('0' + ((val1[0]-48) | (val2[0]-48)));
                    }
                    if(ch == '=') {
                        if(val1 == val2) solve_postfix.push('1');
                        else solve_postfix.push('0');
                    }
                    if(ch == '>') {
                        int u = atoi(val1.c_str());
                        int v = atoi(val2.c_str());
                        bool ans = v > u;
                        solve_postfix.push(ans + '0');
                    }
                    if(ch == '<') {
                        int u = atoi(val1.c_str());
                        int v = atoi(val2.c_str());
                        bool ans = v < u;
                        solve_postfix.push(ans + '0');
                    }

                    if(ch == '+') {
                        int u = atoi(val1.c_str());
                        int v = atoi(val2.c_str());
                        int ans = v + u;
                        alpha_to_string.insert(make_pair(sym, to_string(ans)));
                        solve_postfix.push(sym);
                        sym++;
                    }
                    if(ch == '*') {
                        int u = atoi(val1.c_str());
                        int v = atoi(val2.c_str());
                        int ans = v * u;
                        alpha_to_string.insert(make_pair(sym, to_string(ans)));
                        solve_postfix.push(sym);
                        sym++;
                    }
                    if(ch == '-') {
                        int u = atoi(val1.c_str());
                        int v = atoi(val2.c_str());
                        int ans = u - v;
                        alpha_to_string.insert(make_pair(sym, to_string(ans)));
                        solve_postfix.push(sym);
                        sym++;
                    }

                  }
            }
        }

        if(solve_postfix.top() == '1') {
            result.push_back(row);
        }

        for(char i = sym_temp;i < sym;i++){
            alpha_to_string.erase(i);
        }
        sym = sym_temp;
     }

     return make_pair(result, "");
}

/*
    combined rows of input tables
    if they satisfy two constraints:
        1. Number of columns in both table must be same
        2. Corresponding column name must be same
*/
pvvs _union(pvvs ctable1, pvvs ctable2) {
    vvs table1 = ctable1.first;
    vvs table2 = ctable2.first;
    if(table1[0].size() != table2[0].size()) {
        cerr<<"Number of columns must be same";
        throw -1;
    }

    for(int i = 0;i < table1[0].size();i++) {
        if(table1[0][i]  != table2[0][i]) {
           cerr<<"Column "<<table1[0][i]<<" and "<<table2[0][i]<<" are different";
           throw -1;
        }
    }

    // 1. create a table composed of rows from both tables
    // 2. remove duplicate rows
    set<vector<string> > unique_rows;
    for(int i = 1;i < table1.size();i++) {
        unique_rows.insert(table1[i]);
    }
    for(int i = 1;i < table2.size();i++) {
        unique_rows.insert(table2[i]);
    }

    vvs result;
    result.push_back(table1[0]);

    for(auto it = unique_rows.begin(); it != unique_rows.end();it++) {
        result.push_back(*it);
    }

    return make_pair(result, "");
}

/*
    remove rows of ctable1 which are present in ctable2
    Two constraints must be satisfied
        1. Number of columns in both table must be same
        2. Corresponding column name must be same
*/
pvvs difference(pvvs ctable1, pvvs ctable2) {
    vvs table1 = ctable1.first;
    vvs table2 = ctable2.first;
    if(table1[0].size() != table2[0].size()) {
        cerr<<"Number of columns must be same";
        throw -1;
    }

    for(int i = 0;i < table1[0].size();i++) {
        if(table1[0][i]  != table2[0][i]) {
           cerr<<"Column "<<table1[0][i]<<" and "<<table2[0][i]<<" are different";
            throw -1;
        }
    }

    set<vector<string> > rows;
    for(int i = 1;i < table2.size();i++) {
        rows.insert(table2[i]);
    }

    vvs result;
    result.push_back(table1[0]);

    for(int i = 1;i < table1.size();i++) {
        if(rows.find(table1[i]) == rows.end()) {
            result.push_back(table1[i]);
        }
    }

    return make_pair(result, "");
}

/*
    query: table_newname, cname1, cname2, cname3, ...
    task: change table name and columns name of the input table
    constraints: first entry in the query is new name of the table,
                 rest entries may or may not be present,
                 if present number of entries except first one
                 must be equal to number of columns in the table
*/
pvvs rename(pvvs ctable, string query) {
    string column; int i = 0;
    vector<string> new_names;
    while(i < query.size()) {
          if(query[i] == ',') {
              new_names.push_back(column);
              column.clear();
          } else {
             column += query[i];
          }
          i++;
    } new_names.push_back(column);

    for(int i = 0; i < new_names.size();i++) {
        if(new_names[i] == "") {
            cerr<<"New table name or column name cannot be empty";
            throw -1;
        }
    }

    if(new_names.size() == 1) {
        ctable.second = new_names[0];
        return ctable;
    }

    if(ctable.first[0].size() != new_names.size()-1) {
        cerr<<"Number of columns in the query != Number of columns in the table";
        throw -1;
    }

    ctable.second = new_names[0];
    for(int i=1;i<new_names.size();i++) {
        ctable.first[0][i-1] = new_names[i];
    }

    return ctable;
}

/*
    arguments: 2 tables
    either column names of both tables are different
    or their names are different
*/
pvvs cross_product(pvvs ctable1, pvvs ctable2) {
    vvs table1 = ctable1.first; string tname1 = ctable1.second;
    vvs table2 = ctable2.first; string tname2 = ctable2.second;

    // check for same column_name
    set<string> colnames;
    for(int i = 0;i < table2[0].size();i++) {
        colnames.insert(table2[0][i]);
    }

    vector<string> new_colnames;
    for(int i = 0;i < table1[0].size();i++) {
        if(colnames.find(table1[0][i]) != colnames.end()) {
            if(tname1 == tname2 || tname1 == "" || tname2 == "") {
                cerr<<"Cannot perform crossproduct, same column_name exist in both tables";
                cerr<<"\nHint: Use rename operation before cross product";
                throw -1;
            }
            break;
        }
    }

    for(int i = 0;i < table1[0].size();i++) {
        if(colnames.find(table1[0][i]) != colnames.end()) {
            new_colnames.push_back(tname1 + '.' + table1[0][i]);
        } else {
            new_colnames.push_back(table1[0][i]);
        }
    }

    colnames.clear();

    for(int i = 0;i < table1[0].size();i++) {
        colnames.insert(table1[0][i]);
    }

    for(int i = 0;i < table2[0].size();i++) {
        if(colnames.find(table2[0][i]) != colnames.end()) {
            new_colnames.push_back(tname2 + '.' + table2[0][i]);
        } else {
            new_colnames.push_back(table2[0][i]);
        }
    }

    vvs result;
    result.push_back(new_colnames);

    for(int i = 1;i < table1.size();i++) {
        for(int j = 1; j < table2.size();j++) {
            vector<string> combine_rows;
            combine_rows.reserve(table1[i].size() + table2[j].size());
            combine_rows.insert(combine_rows.end(), table1[i].begin(), table1[i].end());
            combine_rows.insert(combine_rows.end(), table2[j].begin(), table2[j].end());
            result.push_back(combine_rows);
        }
    }

    return make_pair(result, "");
}

pvvs general_query(string query) {
    query = remove_spaces(query);

    // project operation
    if(query.size() > 3 and query[0] == '#') {
        if(query[1] != '(') {
            cerr<<"Invalid syntax";
            throw -1;
        }

        stack<char> valid_paranth;
        valid_paranth.push('(');
        int i = 2;

        while(i < query.size()) {
            if(query[i] == '(') {
                valid_paranth.push('(');
            }
            if(query[i] == ')') {
                if(valid_paranth.empty()) {
                    cerr<<"Invalid syntax";
                    throw -1;
                }
                valid_paranth.pop();
            }

            if(valid_paranth.empty()) break;
            i++;
        }

        if(not valid_paranth.empty()) {
            cerr<<"Invalid syntax";
            throw -1;
        }

        pvvs temp_table = general_query(query.substr(2, i-2));

        if(! (i+2 <= query.size()-2)) {
            cerr<<"Invalid syntax";
            throw -1;
        }

        if(not(query[i+1] == '(' and query[query.size()-1] == ')')) {
            cerr<<"Invalid syntax";
            throw -1;
        }

        return project(temp_table, query.substr(i+2, query.size()-i-3));
    }

    // select operation
    if(query.size() > 3 and query[0] == '$') {
        if(query[1] != '(') {
            cerr<<"Invalid syntax";
            throw -1;
        }
        stack<char> valid_paranth;
        valid_paranth.push('(');
        int i = 2;

        while(i < query.size()) {
            if(query[i] == '(') {
                valid_paranth.push('(');
            }
            if(query[i] == ')') {
                if(valid_paranth.empty()) {
                    cerr<<"Invalid syntax";
                    throw -1;
                }
                valid_paranth.pop();
            }

            if(valid_paranth.empty()) break;
            i++;
        }

        if(not valid_paranth.empty()) {
            cerr<<"Invalid syntax";
            throw -1;
        }

        pvvs temp_table = general_query(query.substr(2, i-2));

        if(! (i+2 <= query.size()-2)) {
            cerr<<"Invalid syntax";
            throw -1;
        }

        if(not(query[i+1] == '(' and query[query.size()-1] == ')')) {
            cerr<<"Invalid syntax";
            throw -1;
        }

        return select(temp_table, query.substr(i+2, query.size()-i-3));
    }

    // union operation
    if(query.size() > 3 and query[0] == 'U') {
        if(query[1] != '(') {
            cerr<<"Invalid syntax";
            throw -1;
        }
        stack<char> valid_paranth;
        valid_paranth.push('(');
        int i = 2;

        while(i < query.size()) {
            if(query[i] == '(') {
                valid_paranth.push('(');
            }
            if(query[i] == ')') {
                if(valid_paranth.empty()) {
                    cerr<<"Invalid syntax";
                    throw -1;
                }
                valid_paranth.pop();
            }

            if(valid_paranth.empty()) break;
            i++;
        }

        if(not valid_paranth.empty()) {
            cerr<<"Invalid syntax";
            throw -1;
        }

        pvvs temp_table1 = general_query(query.substr(2, i-2));

        if(! (i+2 <= query.size()-2)) {
            cerr<<"Invalid syntax";
            throw -1;
        }

        if(not(query[i+1] == '(' and query[query.size()-1] == ')')) {
            cerr<<"Invalid syntax";
            throw -1;
        }

        pvvs temp_table2 = general_query(query.substr(i+2, query.size()-i-3));
        return _union(temp_table1, temp_table2);
    }

    // difference operation
    if(query.size() > 3 and query[0] == 'D') {
        if(query[1] != '(') {
            cerr<<"Invalid syntax";
            throw -1;
        }
        stack<char> valid_paranth;
        valid_paranth.push('(');
        int i = 2;

        while(i < query.size()) {
            if(query[i] == '(') {
                valid_paranth.push('(');
            }
            if(query[i] == ')') {
                if(valid_paranth.empty()) {
                    cerr<<"Invalid syntax";
                    throw -1;
                }
                valid_paranth.pop();
            }

            if(valid_paranth.empty()) break;
            i++;
        }

        if(not valid_paranth.empty()) {
            cerr<<"Invalid syntax";
            throw -1;
        }

        pvvs temp_table1 = general_query(query.substr(2, i-2));

        if(! (i+2 <= query.size()-2)) {
            cerr<<"Invalid syntax";
            throw -1;
        }

        if(not(query[i+1] == '(' and query[query.size()-1] == ')')) {
            cerr<<"Invalid syntax";
            throw -1;
        }

        pvvs temp_table2 = general_query(query.substr(i+2, query.size()-i-3));
        return difference(temp_table1, temp_table2);
    }

    // rename operation
    if(query.size() > 3 and query[0] == 'R') {
        if(query[1] != '(') {
            cerr<<"Invalid syntax";
            throw -1;
        }
        stack<char> valid_paranth;
        valid_paranth.push('(');
        int i = 2;

        while(i < query.size()) {
            if(query[i] == '(') {
                valid_paranth.push('(');
            }
            if(query[i] == ')') {
                if(valid_paranth.empty()) {
                    cerr<<"Invalid syntax";
                    throw -1;
                }
                valid_paranth.pop();
            }

            if(valid_paranth.empty()) break;
            i++;
        }

        if(not valid_paranth.empty()) {
            cerr<<"Invalid syntax";
            throw -1;
        }

        pvvs temp_table = general_query(query.substr(2, i-2));

        if(! (i+2 <= query.size()-2)) {
            cerr<<"Invalid syntax";
            throw -1;
        }

        if(not(query[i+1] == '(' and query[query.size()-1] == ')')) {
            cerr<<"Invalid syntax";
            throw -1;
        }

        return rename(temp_table, query.substr(i+2, query.size()-i-3));
    }

    // cartesian product
    if(query.size() > 3 and query[0] == 'C') {
        if(query[1] != '(') {
            cerr<<"Invalid syntax";
            throw -1;
        }
        stack<char> valid_paranth;
        valid_paranth.push('(');
        int i = 2;

        while(i < query.size()) {
            if(query[i] == '(') {
                valid_paranth.push('(');
            }
            if(query[i] == ')') {
                if(valid_paranth.empty()) {
                    cerr<<"Invalid syntax";
                    throw -1;
                }
                valid_paranth.pop();
            }

            if(valid_paranth.empty()) break;
            i++;
        }

        if(not valid_paranth.empty()) {
            cerr<<"Invalid syntax";
            throw -1;
        }

        pvvs temp_table1 = general_query(query.substr(2, i-2));

        if(! (i+2 <= query.size()-2)) {
            cerr<<"Invalid syntax";
            throw -1;
        }

        if(not(query[i+1] == '(' and query[query.size()-1] == ')')) {
            cerr<<"Invalid syntax";
            throw -1;
        }

        pvvs temp_table2 = general_query(query.substr(i+2, query.size()-i-3));
        return cross_product(temp_table1, temp_table2);
    }

    return table_to_vvs(query);
}

int main() {
    string query;
    //freopen("queries","r",stdin);
    //freopen("output.out","w",stdout);
    while(true) {
        getline(cin, query);
        try {
            pvvs table = general_query(query);
            display_vvs(table.first);
            cout<<"\n$"<<table.second<<"$"<<endl;
        } catch (int e) {
            cout<<"\n$$\n";
        }
    }
    return 0;
}
