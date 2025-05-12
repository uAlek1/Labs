#include "curve.h"
#include <bitset>
#include <algorithm>

using namespace EllipticCurve;

namespace {
    int32_t mod2w(NTL::ZZ& k, uint8_t& w) {
        uint8_t low_w_bits = trunc_long(k, w);
        uint8_t sign = NTL::bit(low_w_bits, w - 1);
        if (sign) {
            int8_t ret_value = low_w_bits;
            // making the number negative
            for (auto i = 0; i < 8 - w; i++) {
                ret_value = ret_value ^ (1 << (8 - i - 1));
            }
            return ret_value;
        }
        else
            return low_w_bits;
    }

    std::vector<int8_t> compute_NAF(NTL::ZZ k, uint8_t w) {
        std::vector<int8_t> NAF;
        uint32_t i = 0;
        while (k >= 1) {
            if (NTL::IsOdd(k)) {
                int32_t ki = mod2w(k, w);
                NAF.push_back(ki);
                k = k - ki;
            }
            else {
                NAF.push_back(0);
            }
            k = k / 2;
            i++;
        }
        std::reverse(NAF.begin(), NAF.end());
        return NAF;
    }
}


short_Weierstrass_Curve::short_Weierstrass_Curve(NTL::ZZ modules, NTL::ZZ a_coof, NTL::ZZ b_coof) : p{ modules }{
    NTL::ZZ_p::init(p);
    a = NTL::to_ZZ_p(a_coof);
    b = NTL::to_ZZ_p(b_coof);
    NTL::ZZ_p discr = -16 * (4 * a * a * a + 27 * b * b);
    if (discr == 0) {
        std::cout << "Error curve\n";
        std::cout << "resetting the curve parameters\n";
    }

}

std::pair< NTL::ZZ_p, NTL::ZZ_p> short_Weierstrass_Curve::get_random_point() {
    NTL::ZZ_p xCoordinate, rightPartEC;
    while (1) {
        random(xCoordinate);
        rightPartEC = xCoordinate * xCoordinate * xCoordinate + a * xCoordinate + b;
        if (Jacobi(NTL::conv<NTL::ZZ>(rightPartEC), p) >= 0) {
            NTL::ZZ y;
            y = NTL::SqrRootMod(NTL::conv<NTL::ZZ>(rightPartEC), NTL::conv<NTL::ZZ>(p));
            return std::pair< NTL::ZZ_p, NTL::ZZ_p>(xCoordinate, NTL::to_ZZ_p(y));
        }
    }
}

Point<short_Weierstrass_Curve> short_Weierstrass_Curve::double_point(const Point<short_Weierstrass_Curve>& P) {
    std::vector<NTL::ZZ_p> coordinate = P.get_coordinate();
    if (coordinate[1] == 0) {
        return Point<short_Weierstrass_Curve>{ P.get_curve() };
    }
    NTL::ZZ_p s = 4 * coordinate[0] * coordinate[1] * coordinate[1];
    NTL::ZZ_p m = 3 * coordinate[0] * coordinate[0] + a * coordinate[4] * coordinate[2];
    NTL::ZZ_p x = m * m - 2 * s;

    NTL::ZZ_p y = m * (s - x) - 8 * power(coordinate[1], 4);
    NTL::ZZ_p z = 2 * coordinate[1] * coordinate[2];
    NTL::ZZ_p zz = z * z;
    NTL::ZZ_p zzz = zz * z;
    return Point< short_Weierstrass_Curve>{ P.get_curve(),{x,y,z,zz,zzz } };
}

Point<short_Weierstrass_Curve> short_Weierstrass_Curve::add(const Point<short_Weierstrass_Curve>& P, const Point<short_Weierstrass_Curve>& Q) {
    
    /*if (P.get_curve() != Q.get_curve()) {
        std::cout << "error, raznie crivie\n";
    }*/
    if (P.is_zero()) {
        return Q;
    }
    if (Q.is_zero()) {
        return P;
    }

    std::vector<NTL::ZZ_p> coordinate_P = P.get_coordinate();
    std::vector<NTL::ZZ_p> coordinate_Q = Q.get_coordinate();
    NTL::ZZ_p U1 = coordinate_P[0] * coordinate_Q[3];
    NTL::ZZ_p U2 = coordinate_Q[0] * coordinate_P[3]; 
    NTL::ZZ_p S1 = coordinate_P[1] * coordinate_Q[4];
    NTL::ZZ_p S2 = coordinate_Q[1] * coordinate_P[4];

    if (U1 == U2) {
        if (S1 != S2) {
            return Point<short_Weierstrass_Curve>{ P.get_curve() };
        }
        else {
            return double_point(P);
        }
    }

    NTL::ZZ_p H = (U2 - U1);
    NTL::ZZ_p R = (S2 - S1);
    NTL::ZZ_p x = (R * R - H * H * H - 2 * U1 * H * H);
    NTL::ZZ_p y = (R * (U1 * H * H - x) - S1 * H * H * H);
    NTL::ZZ_p z = (H * coordinate_P[2] * coordinate_Q[2]);
    NTL::ZZ_p zz = (z * z);
    NTL::ZZ_p zzz = (zz * z);
    return Point< short_Weierstrass_Curve>{ P.get_curve(), {x,y,z,zz,zzz} };
}

NTL::ZZ short_Weierstrass_Curve::get_characteristic() {
    return p;
}

short_Weierstrass_Curve::~short_Weierstrass_Curve() {

}

bool short_Weierstrass_Curve::point_on_curve(Point<short_Weierstrass_Curve>& P) {
    std::vector<NTL::ZZ_p> coef = P.get_coordinate();
    return (coef[1] * coef[1] == coef[0] * coef[0] * coef[0] + a * coef[0] + b);
}


Point<short_Weierstrass_Curve> short_Weierstrass_Curve::binary_mul(const Point<short_Weierstrass_Curve>& P, NTL::ZZ n) {
    // binary mul from right to left
    if (P.is_zero()) {
        return P;
    }
    Point<short_Weierstrass_Curve> Q{P.get_curve()};
    Point<short_Weierstrass_Curve> P_tmp = P;
    while (n != 0) {
        if (NTL::IsOdd(n)) {
            Q = Q + P_tmp;
        }
        P_tmp = double_point(P_tmp);
        n >>= 1;
    }
    return Q;
}


Point<short_Weierstrass_Curve> short_Weierstrass_Curve::sliding_NAF(const Point<short_Weierstrass_Curve>& P, NTL::ZZ k, uint8_t w) {
    std::vector<int8_t> NAF = compute_NAF(k, 2);
    Point<short_Weierstrass_Curve> Q{ P.get_curve() };
    int32_t u = 0, s = 0;
    for (int32_t i = 0; i < NAF.size(); i = i + s) {
        if (NAF[i] == 0) {
            Q = Q + Q;
            s = 1;
            u = 0;
        }
        else {
            if (i + w < NAF.size())
                s = w;
            else
                s = NAF.size() - i;

            while (NAF[i + s - 1] == 0 && s > 1) {
                s--;
            }
            for (int j = 0; j < s; j++) {
                u += (NAF[i + j] * (1 << (s - j - 1)));
            }
            Q = Q * precompute_2_degree[s];
        }
        
        if (u > 0) {
            Q = Q + precompute[u];
        }
        if (u < 0) {
            Q = Q - precompute[-u];
        }
    }

    return Q;


}

Point<short_Weierstrass_Curve> short_Weierstrass_Curve::neg(const Point<short_Weierstrass_Curve>& P) {
    std::vector<NTL::ZZ_p> coordinate = P.get_coordinate();
    coordinate[1] = -coordinate[1];
    return Point<short_Weierstrass_Curve> {P.get_curve(), coordinate};
}

short_Weierstrass_Curve& short_Weierstrass_Curve::operator= (short_Weierstrass_Curve& curve) {
    p = curve.get_characteristic();
    std::pair< NTL::ZZ_p, NTL::ZZ_p>ab =  curve.get_coefficient();
    a = ab.first;
    b = ab.second;
    return *this;
}

std::pair< NTL::ZZ_p, NTL::ZZ_p> short_Weierstrass_Curve::get_coefficient() {
    return std::pair< NTL::ZZ_p, NTL::ZZ_p>(a,b);
}

void short_Weierstrass_Curve::precompute_point(Point<short_Weierstrass_Curve> P, uint8_t w) {
    int32_t end;
    if (w & 1) {
        end = 2 * (pow(2, w) + 1) / 3 - 1;
    }
    else {
        end = 2 * (pow(2, w) - 1) / 3 - 1;
    }
    Point<short_Weierstrass_Curve> tmp{ P.get_curve() };
    for (int32_t i = 1; i <= end; i += 2) {
        tmp = binary_mul(P, NTL::ZZ{ i });
        precompute.insert(std::pair<int32_t, Point<short_Weierstrass_Curve>>(i, tmp));
    }
    for (int32_t i = 1; i <= w; i++) {
        NTL::ZZ tmp{ 1 << i };
        precompute_2_degree.insert(std::pair<int32_t, NTL::ZZ>(i,tmp));
    }
}

Point<short_Weierstrass_Curve> short_Weierstrass_Curve::window_NAF(const Point<short_Weierstrass_Curve>& P, NTL::ZZ k, uint8_t w) {
    std::vector<int8_t> NAF = compute_NAF(k, w);
    Point<short_Weierstrass_Curve> Q{ P.get_curve() };
    for (int32_t i = 0; i < NAF.size(); i++) {
        Q = Q + Q;
        if (NAF[i] > 0) {
            Q = Q + precompute[NAF[i]];
        }
        if (NAF[i] < 0) {
            Q = Q - precompute[-NAF[i]];
        }
    }

    return Q;
}
Point<short_Weierstrass_Curve> short_Weierstrass_Curve::NAF(const Point<short_Weierstrass_Curve>& P, NTL::ZZ k) {
    std::vector<int8_t> NAF = compute_NAF(k, 2);
    Point<short_Weierstrass_Curve> Q{ P.get_curve() };
    for (int32_t i = 0; i < NAF.size(); i++) {
        Q = Q + Q;
        if (NAF[i] > 0) {
            Q = Q + P;
        }
        if (NAF[i] < 0) {
            Q = Q - P;
        }
    }

    return Q;
}


void short_Weierstrass_Curve::precompute_clear() {
    precompute.clear();
}

Point<short_Weierstrass_Curve> short_Weierstrass_Curve::mix_add(const Point<short_Weierstrass_Curve>& P, const Point<short_Weierstrass_Curve>& Q) {

    if (P.get_curve() != Q.get_curve()) {
        std::cout << "error, raznie crivie\n";
    }
    if (P.is_zero()) {
        return Q;
    }
    if (Q.is_zero()) {
        return P;
    }

    std::vector<NTL::ZZ_p> coordinate_P = P.get_coordinate();
    std::vector<NTL::ZZ_p> coordinate_Q = Q.get_coordinate();
    NTL::ZZ_p U1 = coordinate_P[0];
    NTL::ZZ_p U2 = coordinate_Q[0] * coordinate_P[3];
    NTL::ZZ_p S1 = coordinate_P[1];
    NTL::ZZ_p S2 = coordinate_Q[1] * coordinate_P[4];

    if (U1 == U2) {
        if (S1 != S2) {
            return Point<short_Weierstrass_Curve>{ P.get_curve() };
        }
        else {
            return double_point(P);
        }
    }

    NTL::ZZ_p H = (U2 - U1);
    NTL::ZZ_p R = (S2 - S1);
    NTL::ZZ_p x = (R * R - H * H * H - 2 * U1 * H * H);
    NTL::ZZ_p y = (R * (U1 * H * H - x) - S1 * H * H * H);
    NTL::ZZ_p z = (H * coordinate_P[2]);
    NTL::ZZ_p zz = (z * z);
    NTL::ZZ_p zzz = (zz * z);
    return Point< short_Weierstrass_Curve>{ P.get_curve(), { x,y,z,zz,zzz } };
}