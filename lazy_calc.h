#ifndef LAZY_CALC_H_
#define LAZY_CALC_H_

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <map>
#include <set>
#include <istream>


/* Keeping operations in a struct rather than
   in a vector */
struct operation
{
    std::string r1, op, r2;
    operation(std::vector<std::string> const& v) :
        r1{v[0]}, op{v[1]}, r2{v[2]} {}
};

class Calculator
{
public:
    Calculator() {}
    Calculator(std::istream* is) : is{is} {}
    
    /* Checks if a string can safely be converted to int */
    bool is_number(std::string const& str);

    /* Remove special chars, make all lower case */
    void sanitize_str(std::string& str);

    /* "revenue add 200" --> {"revenue", "add", "200"} */
    std::vector<std::string> input_to_vector(std::string const& str);

    /* Returns false if user inputs invalid command */
    bool is_valid_command(std::vector<std::string> const& v);

    /* Called at print, performs the evaluation of register */
    int eval(std::string const& register_name);

    /* Handles everything in the calculator. 
       'is' either cin or file depending on argc*/
    int main_loop();

private:
    // file or std::cin
    std::istream* is;

    // e.g. result add revenue
    std::vector<operation> operations;

    // keeping track of which registers we have
    std::set<std::string> register_names;

    // needed at print, maps name to value
    std::map<std::string, int> register_map;

    // prevents cycles in evaluation
    std::set<std::string> processed_regs;
};

#endif