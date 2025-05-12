#ifndef CURVE_H
#define CURVE_H
#include "point.h"
#include <NTL/ZZ_p.h>
#include <NTL/ZZ.h>
#include <unordered_map> 

namespace EllipticCurve {
	class short_Weierstrass_Curve {
	public:
		short_Weierstrass_Curve(NTL::ZZ, NTL::ZZ, NTL::ZZ);
		std::pair< NTL::ZZ_p, NTL::ZZ_p> get_random_point();
		~short_Weierstrass_Curve();
		NTL::ZZ get_characteristic();

		bool point_on_curve(Point<short_Weierstrass_Curve>& P);

		short_Weierstrass_Curve& operator= (short_Weierstrass_Curve&);

		Point<short_Weierstrass_Curve> add(const Point<short_Weierstrass_Curve>&, const Point<short_Weierstrass_Curve>&);
		Point<short_Weierstrass_Curve> binary_mul(const Point<short_Weierstrass_Curve>&, NTL::ZZ);
		Point<short_Weierstrass_Curve> double_point(const Point<short_Weierstrass_Curve>&);
		Point<short_Weierstrass_Curve> sliding_NAF(const Point<short_Weierstrass_Curve>&, NTL::ZZ, uint8_t);
		Point<short_Weierstrass_Curve> neg(const Point<short_Weierstrass_Curve>&);
		Point<short_Weierstrass_Curve> window_NAF(const Point<short_Weierstrass_Curve>&, NTL::ZZ, uint8_t);
		Point<short_Weierstrass_Curve> mix_add(const Point<short_Weierstrass_Curve>&, const Point<short_Weierstrass_Curve>&);
		Point<short_Weierstrass_Curve> NAF(const Point<short_Weierstrass_Curve>& P, NTL::ZZ k);

		void precompute_point(Point<short_Weierstrass_Curve> P, uint8_t w);
		std::pair< NTL::ZZ_p, NTL::ZZ_p> get_coefficient();
		
		void precompute_clear();
	private:
		NTL::ZZ p;
		NTL::ZZ_p a, b;
		std::unordered_map<int32_t, Point<short_Weierstrass_Curve> > precompute;
		std::unordered_map<int32_t, NTL::ZZ > precompute_2_degree;
	};
}
#endif

//x = 81348641446405008764412822712836816662542348577502498403259296045700740570872
//y = 73471215612997997043724344234563734072961379213982869484091437954610461075588