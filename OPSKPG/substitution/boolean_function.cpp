#include "boolean_function.hpp"
#include <algorithm>


/*
Coordinate function 0: 1  + x4 + x2x3 + x2x4 + x1x2x4 + x1x3x4
Coordinate function 1: 0  + x1 + x3 + x4 + x1x2 + x2x3 + x1x2x3 + x1x2x4  
Coordinate function 2: 1  + x1 + x2 + x1x2 + x1x3 + x2x4 + x1x2x3 + x1x3x4
Coordinate function 3: 1  + x3 + x1x2 + x3x4 + x1x2x3 + x1x3x4 + x2x3x4 
*/
Boolean_function::Boolean_function(uint16_t truth_table){
    std::vector<uint8_t> coordinate_functions (16, 0);
    coordinate_functions[0] = (uint8_t)(truth_table >> 15);
    if(coordinate_functions[0])
        func_.push_back(std::bitset<4>(0));
    
    coordinate_functions[1] = (uint8_t)((uint16_t)(truth_table << 8) >> 15) ^ coordinate_functions[0];
    if(coordinate_functions[1])
        func_.push_back(std::bitset<4>(0b1000));
    coordinate_functions[2] = (uint8_t)((uint16_t)(truth_table << 4) >> 15) ^ coordinate_functions[0];
    if(coordinate_functions[2])
        func_.push_back(std::bitset<4>(0b0100));
    coordinate_functions[3] = (uint8_t)((uint16_t)(truth_table << 2) >> 15) ^ coordinate_functions[0];
    if(coordinate_functions[3])
        func_.push_back(std::bitset<4>(0b0010));
    coordinate_functions[4] = (uint8_t)((uint16_t)(truth_table << 1) >> 15) ^ coordinate_functions[0];
    if(coordinate_functions[4])
        func_.push_back(std::bitset<4>(0b0001));

    coordinate_functions[5] = (uint8_t)((uint16_t)(truth_table << 12) >> 15) ^ coordinate_functions[0] ^ coordinate_functions[1] ^ coordinate_functions[2];
    if(coordinate_functions[5])
        func_.push_back(std::bitset<4>(0b1100));
    coordinate_functions[6] = (uint8_t)((uint16_t)(truth_table << 10) >> 15) ^ coordinate_functions[0] ^ coordinate_functions[1] ^ coordinate_functions[3];
    if(coordinate_functions[6])
        func_.push_back(std::bitset<4>(0b1010));
    coordinate_functions[7] = (uint8_t)((uint16_t)(truth_table << 9) >> 15) ^ coordinate_functions[0] ^ coordinate_functions[1] ^ coordinate_functions[4];
    if(coordinate_functions[7])
        func_.push_back(std::bitset<4>(0b1001));
    coordinate_functions[8] = (uint8_t)((uint16_t)(truth_table << 6) >> 15) ^ coordinate_functions[0] ^ coordinate_functions[2] ^ coordinate_functions[3];
    if(coordinate_functions[8])
        func_.push_back(std::bitset<4>(0b0110));
    coordinate_functions[9] = (uint8_t)((uint16_t)(truth_table << 5) >> 15) ^ coordinate_functions[0] ^ coordinate_functions[2] ^ coordinate_functions[4];
    if(coordinate_functions[9])
        func_.push_back(std::bitset<4>(0b0101));
    coordinate_functions[10] = (uint8_t)((uint16_t)(truth_table << 3) >> 15) ^ coordinate_functions[0] ^ coordinate_functions[3] ^ coordinate_functions[4];
    if(coordinate_functions[10])
        func_.push_back(std::bitset<4>(0b0011));
            
    coordinate_functions[11] = (uint8_t)((uint16_t)(truth_table << 14) >> 15) ^ coordinate_functions[0] ^ coordinate_functions[1] ^ coordinate_functions[2] ^ 
                                          coordinate_functions[3] ^ coordinate_functions[5] ^ coordinate_functions[6] ^ coordinate_functions[8];
    if(coordinate_functions[11])
        func_.push_back(std::bitset<4>(0b1110));
    coordinate_functions[12] = (uint8_t)((uint16_t)(truth_table << 13) >> 15) ^ coordinate_functions[0] ^ coordinate_functions[1] ^ coordinate_functions[2] ^ 
                                          coordinate_functions[4] ^ coordinate_functions[5] ^ coordinate_functions[7] ^ coordinate_functions[9];
    if(coordinate_functions[12])
        func_.push_back(std::bitset<4>(0b1101));
    coordinate_functions[13] = (uint8_t)((uint16_t)(truth_table << 11) >> 15) ^ coordinate_functions[0] ^ coordinate_functions[1] ^ coordinate_functions[3] ^ 
                                          coordinate_functions[4] ^ coordinate_functions[6] ^ coordinate_functions[7] ^ coordinate_functions[10];
    if(coordinate_functions[13])
        func_.push_back(std::bitset<4>(0b1011));
    coordinate_functions[14] = (uint8_t)((uint16_t)(truth_table << 7) >> 15) ^ coordinate_functions[0] ^ coordinate_functions[2] ^ coordinate_functions[3] ^ 
                                          coordinate_functions[4] ^ coordinate_functions[8] ^ coordinate_functions[9] ^ coordinate_functions[10];
    if(coordinate_functions[14])
        func_.push_back(std::bitset<4>(0b0111));
            
    coordinate_functions[15] = (uint8_t)((uint16_t)(truth_table << 15) >> 15) ^ coordinate_functions[0] ^ coordinate_functions[1] ^ coordinate_functions[2] ^ coordinate_functions[3] ^
                                          coordinate_functions[4] ^ coordinate_functions[5] ^ coordinate_functions[6] ^ coordinate_functions[7] ^ coordinate_functions[8] ^
                                          coordinate_functions[9] ^ coordinate_functions[10] ^ coordinate_functions[11] ^ coordinate_functions[12] ^ coordinate_functions[13] ^
                                          coordinate_functions[14];
    if(coordinate_functions[15])
        func_.push_back(std::bitset<4>(0b1111));
}


uint8_t Boolean_function::eval(uint8_t x){
    uint8_t result = 0;
    int i = 0;
    if(func_[0] == std::bitset<4>(0)){
        result = 1;
        i = 1;
    }
    std::bitset<4> input(x);
    for(; i < func_.size(); i++){
        std::bitset<4> temp = input & func_[i];
        if(temp == func_[i]){
            result ^= 1;
        }
    }
    return result;
}


uint8_t Boolean_function::degree() {
    uint8_t max = 0;
    for(int i = 0; i < func_.size(); i++){
        if(max < func_[i].count())
            max = func_[i].count();
    }
    return max;
}

std::ostream& operator<<(std::ostream& os, const Boolean_function& obj) {
    std::string poly = "";
    for(int i = 0; i < obj.func_.size(); i++){
        switch (obj.func_[i].to_ulong())
        {
        case 0:
            poly += "1";
            break;
        case 0b0001:
            poly += "x4";
            break;
        case 0b0010:
            poly += "x3";
            break;
        case 0b0100:
            poly += "x2";
            break;
        case 0b1000:
            poly += "x1";
            break;
        case 0b1100:
            poly += "x1x2";
            break;
        case 0b1010:
            poly += "x1x3";
            break;
        case 0b1001:
            poly += "x1x4";
            break;
        case 0b0110:
            poly += "x2x3";
            break;
        case 0b0101:
            poly += "x2x4";
            break;
        case 0b0011:
            poly += "x3x4";
            break;
        case 0b1110:
            poly += "x1x2x3";
            break;
        case 0b1101:
            poly += "x1x2x4";
            break;
        case 0b1011:
            poly += "x1x3x4";
            break;
        case 0b0111:
            poly += "x2x3x4";
            break;
        case 0b1111:
            poly += "x1x2x3x4";
            break;
        default:
            break;
        }

        if(i != obj.func_.size() - 1){
            poly += " + ";
        }
    }
    os << poly;
    return os;
}


Boolean_function Boolean_function::operator+(const Boolean_function& other){
    Boolean_function temp{func_};
    for(int i = 0; i < other.func_.size(); i++){
        auto it = std::find(temp.func_.begin(), temp.func_.end(), other.func_[i]); 
        if( it == temp.func_.end()){
            temp.push_back(other.func_[i]);
        }
        else{
            temp.func_.erase(it);
        }
    }
    return temp;
}


void Boolean_function::push_back(std::bitset<4> new_set){
    func_.push_back(new_set);
}

Boolean_function& Boolean_function::operator=(Boolean_function const &rhs) {
    if (this != &rhs) {
        func_ = rhs.func_;
    }
    return *this;
}

std::vector<std::bitset<4>> Boolean_function::create_input(uint8_t x){
    std::vector<std::bitset<4>> result;
    for(uint8_t i = 1; i < 16; ++i){
        if((i & x) == i){
            result.push_back(i);
        }
    }
    return result;
}