#include <iostream>
#include <fstream>
#include <boost/dynamic_bitset.hpp>

namespace{
    struct generator{
        generator(uint32_t start_state, uint32_t characteristic_poly){
            uint32_t size = 0, test = characteristic_poly;
            while (test){
                test >>= 1;
                size++;
            }
            poly = boost::dynamic_bitset<>(size-1, characteristic_poly);
            state = boost::dynamic_bitset<>(size-1, start_state);
            scalar = boost::dynamic_bitset<>(size-1);
        };
        uint8_t next(){
            uint8_t out = state[0];
            uint8_t in = get_bit();
            state >>= 1;
            state[state.size()-1] = in;
            return out;
        }
        void print_data(){
            std::cout << "poly = " << poly << std::endl;
            std::cout << "state = " << state << std::endl;
        }
    private:
        boost::dynamic_bitset<> poly; //84163
        boost::dynamic_bitset<> state;
        boost::dynamic_bitset<> scalar;
        uint8_t get_bit(){
            scalar = poly & state;
            return (scalar.count() & 1) ? 1 : 0;
        }
    };
}


int main(int, char**){
    uint32_t len, start_state, poly;
    std::ofstream out("/home/qwe/Desktop/4lab/program1/sequences.txt");
    std::ofstream test_out("/home/qwe/Desktop/4lab/program1/sequences_test.txt");
    std::cout << "Enter characteristic polynomial: ";
    std::cin >> poly;
    std::cout << "Enter start state: ";
    std::cin >> start_state;
    std::cout << "Enter sequens len: ";
    std::cin >> len;
    generator gen(start_state, poly);
    gen.print_data();
    for(uint32_t i = 0; i < len; ++i){
        uint8_t bit = gen.next();
        out << +bit;
        test_out << +bit << ",";
    }
    out.close();
    test_out.close();
}
