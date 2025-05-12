#ifndef BOOLEAN_FUNCTION_HPP
#define BOOLEAN_FUNCTION_HPP

#include <iostream>
#include <vector>
#include <bitset>

class Boolean_function {
        std::vector<std::bitset<4>> func_;
        void push_back(std::bitset<4> monom);
    public: 
        Boolean_function(){};
        Boolean_function(uint16_t truth_table);
        Boolean_function(std::vector<std::bitset<4>> func) : func_(func){ };
        std::vector<std::bitset<4>> create_input(uint8_t x);
        uint8_t eval(uint8_t x);
        uint8_t degree();
        Boolean_function operator+(const Boolean_function& other);
        friend std::ostream& operator<<(std::ostream& os, const Boolean_function& obj);
        Boolean_function& operator=(Boolean_function const &rhs);
};

#endif

