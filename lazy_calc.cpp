#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <map>
#include <set>
#include "lazy_calc.h"


bool Calculator::is_number(std::string const& str) {
    std::string::const_iterator it = str.begin();
    while (it != str.end() && isdigit(*it)) ++it;

    // true if we made it through:
    return !str.empty() && it == str.end();
}

void Calculator::sanitize_str(std::string& str) {
    // remove special characters
    str.erase(std::remove_if(str.begin(), str.end(),
                [](char c) {return !(isalnum(c)) && c != ' ';}), str.end());
    
    // case unsensitivity
    std::transform(str.begin(), str.end(), 
        str.begin(), ::tolower);
}

std::vector<std::string> Calculator::input_to_vector(std::string const& str) {
    std::string word{};
    std::vector<std::string> v{};
    std::istringstream ss{str};

    while (ss >> word) {
        v.push_back(word);
    }
    return v;
}

bool Calculator::is_valid_command(std::vector<std::string> const& v) {
    size_t n = v.size();
    std::vector<std::string> allowed_ops{"add", "subtract", "multiply"};

    // str is sanitized, so we know its lowercase
    if (n == 1) {  // quit
        return v[0] == "quit";
    }
    if (n == 2) {  // print <reg>
        bool reg_exists = (bool) register_names.count(v[1]);
        return v[0] == "print" && !is_number(v[1]) && reg_exists;
    }
    if (n == 3) {  // <reg> <operation> <value/reg>
        bool is_allowed_operation = std::find(
            allowed_ops.begin(), allowed_ops.end(), v[1]) != allowed_ops.end();
        return is_allowed_operation && !is_number(v[0]);
    }

    return false;
}

int Calculator::eval(std::string const& r) {
    
    if (is_number(r)) {  // base case, e.g. "A add 2" and we eval 2
        return std::stoi(r);
    }
    if (register_map.count(r)) {
        return register_map[r];
    }
    if (processed_regs.count(r)) {
        // Dont want this, since it means we get in a processing cycle 
        std::cerr << "You supplied illogical calculations :(" << std::endl;
        exit(1);  // can't return 1 here as we are in a helper function 
    }
    // but NOW we can process it
    processed_regs.insert(r);

    int res{};

    // here comes the "recursive" evaluation, with the base case being r2 is an int.
    for (auto& c : operations) {
        if (c.r1 == r) {  // process relevant operations (where result will impact r)
            if (c.op == "add") { res += eval(c.r2); }
            else if (c.op == "subtract") { res -= eval(c.r2); }
            else if (c.op == "multiply") { res *= eval(c.r2); } 
        } 
    }
    return register_map[r] = res;
}

int Calculator::main_loop() {
    std::string line;
    while (std::getline(*is, line)) {
        sanitize_str(line);
        std::vector<std::string> v{input_to_vector(line)};
        
        if (!is_valid_command(v)) {
            std::cerr << "Invalid command" << std::endl;
            continue;  // skip this invalid command
        }

        /* Every command that comes through here is valid */

        size_t n = v.size();
        if (n == 1) {  // quit
            return 0;
        }
        if (n == 2) {  // print
            // these are used for the recursive calculations, 
            // so when new print comes, reset them!
            register_map.clear();
            processed_regs.clear();
            
            // print the actual (lazy) evaluated value of the register
            std::cout << eval(v[1]) << std::endl;

        } 
        if (n == 3) {
            // put reg1 and reg2/val in register set
            register_names.insert(v[0]);
            register_names.insert(v[2]);
            
            operations.push_back(operation{v});
        }
    }
}

int main(int argc, char* argv[]) {
    // input stream can be file or std::cin
    std::istream* is;
    std::ifstream file;

    if (argc == 1) {

        is = &std::cin;

    } else if (argc == 2) {
        
        file.open(argv[1]);
        
        if (!file) {
           
            std::cerr << "Invalid file name" << std::endl;
            return 1;
        }
        is = &file;

    } else {
        std::cerr << "Too many args" << std::endl;
        return 1;
    }

    Calculator c{is};
    
    // could just return main_loop here but want to make sure file is not open
    int res = c.main_loop();

    if (file) {
        file.close();
    }

    return res;
    
}