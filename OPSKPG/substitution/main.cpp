#include "boolean_function.hpp"
#include <algorithm>

template<typename T>
using Matrix = std::vector<std::vector<T>>;

namespace{

    template<typename T>
    void print_matrix(T table){
        for(auto i = table.begin(), end_i = table.end(); i != end_i; i++){
            
            for(auto j = (*i).begin(), end_j = (*i).end(); j != end_j; j++){
                std::cout<< +(*j) <<'\t';
            }
            std::cout<<std::endl;
        }
    }

    Matrix<uint8_t> table_differential(std::vector<uint8_t> s_box){

        Matrix<uint8_t> result_matrix(static_cast<uint8_t>(s_box.size()), 
                                std::vector<uint8_t>(static_cast<uint8_t>(s_box.size()), 0));

        for(uint8_t a = 0, end_a = result_matrix.size(); a != end_a; a++){
            for(uint8_t b = 0, end_b = result_matrix[a].size(); b != end_b; b++){
                for(uint8_t x = 0, x_end = s_box.size(); x < x_end; x++){
                    if(s_box[(x^a)] == (s_box[x] ^ b)){
                        result_matrix[a][b]+=1;
                    }
                }
            }
        }
        return result_matrix;
    }

    uint8_t apply_mask(uint8_t value, uint8_t mask){
        uint8_t interValue = value & mask;
        uint8_t total = 0;
    
        while(interValue > 0){
            uint8_t temp = interValue & 1;    
            interValue >>= 1;
            if (temp) 
                total = total ^ 1;
        } 
        return total;   
    }
    
    Matrix<double> linear_approximation(std::vector<uint8_t> s_box){

        Matrix<double> result_matrix(static_cast<uint8_t>(s_box.size()), 
                                std::vector<double>(static_cast<uint8_t>(s_box.size()), 0));

        for(uint8_t a = 0, end_a = result_matrix.size(); a != end_a; a++){
            for(uint8_t b = 0, end_b = result_matrix[a].size(); b != end_b; b++){
                for(uint8_t x = 0, x_end = s_box.size(); x < x_end; x++){
                    if(apply_mask(x,a) == apply_mask(s_box[x],b)){
                        result_matrix[a][b]+=1;
                    }
                }
                result_matrix[a][b] /= result_matrix.size();
                result_matrix[a][b] -= 0.5;
            }
        }
        return result_matrix;
    }

    uint8_t degree_s_box(std::vector<Boolean_function> func){
        uint8_t min_degree = 255;
        uint8_t qwe = 0;
        for(uint8_t i = 1, end = 16; i != end; i++){
            uint8_t j = i, k = 3;
            Boolean_function temp;
            while(j){
                if(j&1){
                    temp = temp + func[k];
                }
                j>>=1;
                k--;
            }
            //std::cout << "i = " << +i <<" ;temp = " << temp << std::endl;
            j =  temp.degree();   
            if(min_degree > j){
                qwe = i;
                min_degree = j;
            }
        }
        return min_degree;
    }

    int8_t Walsh_Adamar_transform(Boolean_function f, uint8_t u){
        int8_t sum = 0;
        for(int i = 0; i < 16; i++){
            uint8_t ev = f.eval(i);
            uint8_t scalar = apply_mask(u, i);
            if((ev+scalar) & 1)
                sum -= 1;
            else
                sum += 1;
        }
        return sum;
    }
    uint8_t nonlinear_func(Boolean_function f){
        uint8_t nonlinear = 8;
        uint8_t max = 0;

        for(int u = 0; u < 16; u++){
            int8_t tr = Walsh_Adamar_transform(f, u);
            if((uint8_t)std::abs(tr) > max){
                max = (uint8_t)std::abs(tr);
            }
        }
        return nonlinear - (max / 2);

    }
    uint8_t nonlinear_s_box(std::vector<Boolean_function> func){
        uint8_t nonlinear_min = 255;
        for(uint8_t i = 1, end = 16; i != end; i++){
            uint8_t j = i, k = 3;
            Boolean_function temp;
            while(j){
                if(j&1){
                    temp = temp + func[k];
                }
                j>>=1;
                k--;
            }
            j =  nonlinear_func(temp);   
            if(nonlinear_min > j){
                nonlinear_min = j;
            }
        }
        return nonlinear_min;
    }

    double max_preob(Matrix<double> table){
        double max = 0;
        for(int i = 1; i < table.size(); i++){
            auto test = std::max_element(table[i].begin()+1, table[i].end(), [](double a, double b){
                                                                    return std::abs(a) < std::abs(b);
                                                                });
            if(*test > max){
                max = *test;
            }
        }
        return max;
    }
}

int main(int, char**){
    
    std::vector<uint8_t> s_box {11, 7, 14, 3, 9, 15, 0, 6, 13, 1, 10, 12, 8, 2, 4, 5};
   // std::vector<uint8_t> s_box {6,3,8,5,15,1,12,4,7,2,11,13,0,14,10,9};
    std::vector<uint16_t> truth_table;
    truth_table.push_back(0b1010110010111000);
    truth_table.push_back(0b0110010110010011);
    truth_table.push_back(0b1111010100100100);
    truth_table.push_back(0b1101110011000001);

    /*truth_table.push_back(0b0010101000110111);
    truth_table.push_back(0b1001101110010100);
    truth_table.push_back(0b1100100011100110);
    truth_table.push_back(0b0101110010110001);*/
    std::vector<Boolean_function> func;
    func.push_back(Boolean_function(truth_table[0]));
    func.push_back(Boolean_function(truth_table[1]));
    func.push_back(Boolean_function(truth_table[2]));
    func.push_back(Boolean_function(truth_table[3]));
    std::cout << "The resulting polynomials:\n";
    for(auto it = func.begin(), end = func.end(); it!=end; it++){
        std::cout << *it << std::endl;
    }

    std::cout << "degree s-box : " << +degree_s_box(func) << std::endl;
    std::cout << "nonlinear : " << +nonlinear_s_box(func) << std::endl;

    Matrix<double> linear_approx_table = linear_approximation(s_box);
    std::cout << "linear appro\n";
    print_matrix(linear_approx_table);
    std::cout<< max_preob(linear_approx_table) << std::endl;
    Matrix<uint8_t> diff_table = table_differential(s_box);
    std::cout<<"dif table\n";
    print_matrix(diff_table);


    Boolean_function test{};
    std::vector<std::bitset<4>> qwerty = test.create_input(15);
    std::cout<<"poly = ";
    for(auto it = qwerty.begin(), eit = qwerty.end(); it != eit; ++it){
        std::cout<< (*it) << "  ";
    }
    std::cout<<std::endl;
}
// 5 =  0101 \
11 = 1011 \
15 = 1111