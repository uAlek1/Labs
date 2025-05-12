#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <NTL/GF2.h>
#include <NTL/GF2X.h>
#include <NTL/mat_GF2.h>

using GF = NTL::GF2;
using R = NTL::GF2X;
using matrix = NTL::mat_GF2;

namespace Gankel_determinant{
    matrix create_matrix(std::vector<uint8_t> sequence, int t, int k){
        matrix m;
        m.SetDims(k, k);
        for(int i = 0; i < k; ++i){
            for(int j = 0; j < k; ++j){
                m[i][j] = sequence[t + i + j];
            }
        }
        return m;
    }

    void print_matrix(matrix in){
        for(int i = 0; i < in.NumRows(); ++i){
            for(int j = 0; j < in.NumCols(); ++j){
                std::cout << in[i][j] << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    int grade_degree(std::vector<uint8_t> sequence){
        uint8_t flag = 0;
        for(int k = 1; k < 32; ++k){
            for(int t = 0; t < sequence.size() - 2*k + 2; ++t){
                matrix in = create_matrix(sequence, t, k);
                GF det = NTL::determinant(in);
                if(det != 0){
                    flag = 1;
                    break;
                }
            }
            if(!flag){
                return k - 1;
            }
            flag = 0;
        }
        return -1;
    }
}

namespace BM {
    R Berlekamp_Massey(std::vector<uint8_t> sequence, int k){
        R r0, r1, r, v, v0{GF(0)}, v1{GF(1)}, q;
        NTL::SetCoeff(r0, 2*k, GF(1));
        for(int i = 0, end = 2*k; i!=end; ++i){
            NTL::SetCoeff(r1, i, GF(sequence[i]));
        }
        int deg = NTL::deg(r1);
        while(k <= deg){
            NTL::DivRem(q, r, r0, r1);
            v = v0 - q*v1;
            v0 = v1; v1 = v;
            r0 = r1; r1 = r;
            deg = NTL::deg(r1);
        }
        int d = std::max(NTL::deg(v1), NTL::deg(r1) + 1);
        R min_poly;
        for(int i = 0; i <= d; ++i){
            NTL::SetCoeff(min_poly, d-i, v1[i]);
        }
        return min_poly;
    }

    void print_poly(R poly){
        int d = NTL::deg(poly) + 1;
        std::string res = "";
        if(poly[0] != 0){
            res += "1 + ";
        }
        for(int i = 1; i < d; ++i){
            if(poly[i] != 0){
                res += ("x**" + std::to_string(i) + " + ");
            }
        }

        std::cout << res.erase(res.size() - 3) << std::endl;
    }
}


int main(int, char**){
    std::vector<uint8_t> sequence;
    std::ifstream in("/home/qwe/Desktop/4lab/program1/sequences.txt");
    std::istream_iterator<uint8_t> input(in);
    std::copy(input, std::istream_iterator<uint8_t>(),std::back_inserter(sequence));
    in.close();
    int k, L;
    std::cout << "enter k: ";
    std::cin >> k;
    std::cout << "enter L: ";
    std::cin >> L;
    sequence.erase(sequence.begin() + L, sequence.end());
    int top = Gankel_determinant::grade_degree(sequence);
    std::cout << "after Gankel determinant, top estimate degree poly =  " << top << std::endl;
    if(L < 2*k){
        std::cout << "error\n";
        return -1;
    }
    R min_poly = BM::Berlekamp_Massey(sequence, k);
    BM::print_poly(min_poly);
    return 0;
}
