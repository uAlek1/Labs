#ifndef POINT_H
#define POINT_H
#include <NTL/ZZ_p.h>
#include <vector>
#include <iostream>

namespace EllipticCurve {

	template <typename T>
	class Point {
	public:
		Point() {
			curve = nullptr;
		}
		Point(T* curve, std::vector<NTL::ZZ_p> coordinate) : curve{ curve }, coordinate{ coordinate } {
		}
		Point(T* curve) : curve{ curve } {
			coordinate = { NTL::ZZ_p{0}, NTL::ZZ_p{1}, NTL::ZZ_p{0}, NTL::ZZ_p{0}, NTL::ZZ_p{0} };
		}
		Point(T* curve, NTL::ZZ_p x, NTL::ZZ_p y) : curve{ curve } {
			coordinate = { x, y, NTL::ZZ_p{1}, NTL::ZZ_p{1}, NTL::ZZ_p{1} };
		}

		Point<T> operator+(const Point<T>& x) const {
			return curve->add(*this, x);
		}
		Point<T> operator-(const Point<T>& x) const {
			Point<T> x_neg = curve->neg(x);
			return curve->add(*this, x_neg);
		}
		Point<T> operator*(const NTL::ZZ n){
			return curve->binary_mul(*this, n);
		}
		Point<T>& operator=(const Point<T>& x) {
			coordinate = x.get_coordinate();
			curve = x.get_curve();
			return *this;
		}
		friend std::ostream& operator<<(std::ostream& os, const Point<T>& P) {
			Point<T> Q = P;
			if (!P.is_zero()) {
				Q = P.normilize();
			}
			std::vector<NTL::ZZ_p> coor = Q.get_coordinate();
			os << '(' << coor[0] << ',' << coor[1] << ',' << coor[2] << ')';
			return os;
		}

		std::vector<NTL::ZZ_p> get_coordinate() const {
			return coordinate;
		}
		T* get_curve() const {
			return curve;
		}
		~Point() {
			coordinate.clear();
		}

		bool is_zero() const{
			return  coordinate[2] == 0;
		}

		Point<T> normilize() const{
			NTL::ZZ_p inv_zz = inv(coordinate[3]);
			NTL::ZZ_p inv_zzz = inv(coordinate[4]);
			return Point<T>{ curve, { coordinate[0] * inv_zz, coordinate[1] * inv_zzz, NTL::ZZ_p{1}, NTL::ZZ_p{1}, NTL::ZZ_p{1} }};
		}
	private:
		std::vector<NTL::ZZ_p> coordinate;
		T* curve;
	};
}
#endif;