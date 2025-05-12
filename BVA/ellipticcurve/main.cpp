#include <iostream>
#include "curve.h"
#include "point.h"
#include <NTL/ZZ_p.h>
#include <stdlib.h>
#include <conio.h>
#include <chrono>

using namespace EllipticCurve;

std::vector<NTL::ZZ> generate_tests(long len, long count) {
	std::vector<NTL::ZZ> tests;
	for (int i = 0; i < count; i++) {
		NTL::ZZ tmp = NTL::RandomBits_ZZ(len);
		tests.push_back(tmp);
	}
	return tests;
}



int main() {
	std::cout << "Enter curve parameters:\n";
	NTL::ZZ p, a, b;
	//std::cout << "p : ";
	//std::cin >> p;
	//std::cout << "a : ";
	//std::cin >> a;
	//std::cout << "b : ";
	//std::cin >> b;
	//p = NTL::ZZ{ NTL::conv<NTL::ZZ>("10683863876144140607596037180633903641062677122953791254380615843457373831710272591247") };
	//a = NTL::ZZ{ NTL::conv<NTL::ZZ>("1122163254750825614190870165471914037080") };
	//b = NTL::ZZ{ NTL::conv<NTL::ZZ>("163837269953416404312883646079501012881") };
	p = NTL::ZZ{ NTL::conv<NTL::ZZ>("115792089237316193816632940749697632222930376074883395431335758373937458794301") };
	a = NTL::ZZ{ NTL::conv<NTL::ZZ>("34334035839408562019501817952950963779927347618196862229609338845347939870735") };
	b = NTL::ZZ{ NTL::conv<NTL::ZZ>("100084083384483170557423172468432397335238482462053505107296731479523599110024") };
	short_Weierstrass_Curve curve{ p, a, b };
	Point<short_Weierstrass_Curve> P{ &curve };
	std::vector<NTL::ZZ> test_data;
	while (1) {
		std::cout << "Management:\n";
		std::cout << "1 - addition points\n";
		std::cout << "2 - checking that the point is on the curve\n";
		std::cout << "3 - find a multiple point\n";
		std::cout << "4 - get random point\n";
		std::cout << "5 - resetting the curve parameters\n";
		std::cout << "6 - generate test\n";
		std::cout << "7 - run test\n";
		std::cout << "8 - exit\n";
		int8_t choice;
		std::cin >> choice;
		std::pair< NTL::ZZ_p, NTL::ZZ_p> cf = curve.get_random_point();
		P = Point<short_Weierstrass_Curve>{ &curve, cf.first, cf.second };
		system("cls");
		if (choice == '1') {
			NTL::ZZ_p x, y;
			std::cout << "Enter two point\n";
			std::cout << "first point\n";
			std::cout << "x-coordinate: ";
			std::cin >> x;
			std::cout << "y-coordinate: ";
			std::cin >> y;
			Point<short_Weierstrass_Curve> P{ &curve, x, y };
			std::cout << "second point\n";
			std::cout << "x-coordinate: ";
			std::cin >> x;
			std::cout << "y-coordinate: ";
			std::cin >> y;
			Point<short_Weierstrass_Curve> Q{ &curve,x,y };
			std::cout << "first + second = " << curve.mix_add(P,Q) << std::endl;
		}
		if (choice == '2') {
			NTL::ZZ_p x, y;
			std::cout << "Enter coordinate point\n";
			std::cout << "x-coordinate: ";
			std::cin >> x;
			std::cout << "y-coordinate: ";
			std::cin >> y;
			Point<short_Weierstrass_Curve> P{ &curve, x, y };
			bool test = curve.point_on_curve(P);
			if (test) {
				std::cout << "a point on the curve\n";
			}
			else {
				std::cout << "the point is not on the curve\n";
			}

		}
		if (choice == '3') {
			NTL::ZZ_p x, y;
			NTL::ZZ k;
			std::cout << "Enter coordinate point\n";
			std::cout << "x-coordinate: ";
			std::cin >> x;
			std::cout << "y-coordinate: ";
			std::cin >> y;
			P = Point<short_Weierstrass_Curve>{ &curve, x, y };
			std::cout << P << std::endl;
			std::cout << "Enter k: ";
			std::cin >> k;
			if (curve.point_on_curve(P)) {
				auto begin = std::chrono::steady_clock::now();
				curve.precompute_point(P, 6);
				auto end = std::chrono::steady_clock::now();
				auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
				std::cout << "The time precompute: " << elapsed_ms.count() << " ms\n";

				std::cout << "sliding window NAF:\n";
				begin = std::chrono::steady_clock::now();
				std::cout << curve.sliding_NAF(P, k, 6) << std::endl;
				end = std::chrono::steady_clock::now();
				elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
				std::cout << "The time sliding window NAF: " << elapsed_ms.count() << " ms\n";

				std::cout << "NAF:\n";
				begin = std::chrono::steady_clock::now();
				std::cout << curve.sliding_NAF(P, k, 1) << std::endl;
				end = std::chrono::steady_clock::now();
				elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
				std::cout << "The time NAF: " << elapsed_ms.count() << " ms\n";

				std::cout << "window NAF:\n";
				begin = std::chrono::steady_clock::now();
				std::cout << curve.window_NAF(P, k, 6) << std::endl;
				end = std::chrono::steady_clock::now();
				elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
				std::cout << "The time window NAF: " << elapsed_ms.count() << " ms\n";

				std::cout << "Binary mul:\n";
				begin = std::chrono::steady_clock::now();
				std::cout << P * k << std::endl;
				end = std::chrono::steady_clock::now();
				elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
				std::cout << "The time binary mul: " << elapsed_ms.count() << " ms\n";
				curve.precompute_clear();
			}
			else
				std::cout << "the point is not on the curve\n";
		}
		if (choice == '4') {
			std::pair< NTL::ZZ_p, NTL::ZZ_p> cf = curve.get_random_point();
			std::cout << "x = " << cf.first << std::endl;
			std::cout << "y = " << cf.second << std::endl;
		}
		if (choice == '5') {
			std::cout << "Enter curve parameters:\n";
			NTL::ZZ p, a, b;
			std::cout << "p : ";
			std::cin >> p;
			std::cout << "a : ";
			std::cin >> a;
			std::cout << "b : ";
			std::cin >> b;
			short_Weierstrass_Curve tmp{ p, a, b };
			curve = tmp;
		}
		if (choice == '6') {
			long len, count;
			std::cout << "Enter enter the length of k (bit)\n";
			std::cout << "length = ";
			std::cin >> len;
			std::cout << "Enter count tests\n";
			std::cout << "count = ";
			std::cin >> count;
			test_data = generate_tests(len, count);
			std::cout << test_data.size() << std::endl;
		}
		if (choice == '7') {
			Point<short_Weierstrass_Curve> tmp{ &curve };
			NTL::ZZ time{ 0 };
			for (auto iter = test_data.begin(), end = test_data.end(); iter != end; iter++) {
				auto start_time = std::chrono::steady_clock::now();
				tmp = P * (*iter);
				auto end_time = std::chrono::steady_clock::now();
				auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
				time += elapsed_ms.count();
			}
			std::cout << "avg time binary mul = " << time / test_data.size() <<" ms" << std::endl;

			time = 0;
			curve.precompute_point(P,7);
			for (auto iter = test_data.begin(), end = test_data.end(); iter != end; iter++) {
				auto start_time = std::chrono::steady_clock::now();
				tmp = curve.sliding_NAF(P, *iter, 7);
				auto end_time = std::chrono::steady_clock::now();
				auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
				time += elapsed_ms.count();
			}
			std::cout << "avg time sliding naf = " << time / test_data.size() << " ms" << std::endl;

			time = 0;
			for (auto iter = test_data.begin(), end = test_data.end(); iter != end; iter++) {
				auto start_time = std::chrono::steady_clock::now();
				tmp = curve.window_NAF(P, *iter, 7);
				auto end_time = std::chrono::steady_clock::now();
				auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
				time += elapsed_ms.count();
			}
			std::cout << "avg time window naf = " << time / test_data.size() << " ms" << std::endl;

			time = 0;
			for (auto iter = test_data.begin(), end = test_data.end(); iter != end; iter++) {
				auto start_time = std::chrono::steady_clock::now();
				tmp = curve.NAF(P, *iter);
				auto end_time = std::chrono::steady_clock::now();
				auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
				time += elapsed_ms.count();
			}
			std::cout << "avg time naf = " << time / test_data.size() << " ms" << std::endl;

			curve.precompute_clear();

		}
		if (choice == '8') {
			break;
		}
		if (choice < '0' || choice > '8') {
			std::cout << "input error\n";
		}

		std::cout << "Enter any key\n";
		getch();
		system("cls");

	}

	
	//std::cout << curve.point_on_curve(P) << std::endl;
	//Point<short_Weierstrass_Curve> R = P * NTL::ZZ{ 2 };
	//std::cout << R << std::endl;
	
	//R = P * NTL::ZZ{ NTL::conv<NTL::ZZ>("65432123456654321123456") } + P * NTL::ZZ{ NTL::conv<NTL::ZZ>("6006") };
	//std::cout << R << std::endl;
	
	/*short_Weierstrass_Curve curve{
		NTL::ZZ{NTL::conv<NTL::ZZ>("1329227995784915872903807060280345027")},
		NTL::ZZ{NTL::conv<NTL::ZZ>("29")},
		NTL::ZZ{NTL::conv<NTL::ZZ>("4")}

	};
	/*std::pair< NTL::ZZ_p, NTL::ZZ_p> cf = curve.get_random_point();
	Point<short_Weierstrass_Curve> P{ curve, cf.first, cf.second };
	std::cout << "P = " << P << std::endl;
	std::cout << "P on curve - " << curve.point_on_curve(P) << std::endl;*/
	/*Point<short_Weierstrass_Curve> P{
			curve,
			NTL::ZZ_p{NTL::conv<NTL::ZZ_p>("47129147451640669861995893401847129088031251986957146322227563792853022162994")},
			NTL::ZZ_p{NTL::conv<NTL::ZZ_p>("26759137557305999312021709794582416111152054791847908575270538922907922166139")}
		};
		Point<short_Weierstrass_Curve> Q{
			curve,
			NTL::ZZ_p{NTL::conv<NTL::ZZ_p>("47129147451640669861995893401847129088031251986957146322227563792853022162994")},
			NTL::ZZ_p{NTL::conv<NTL::ZZ_p>("26759137557305999312021709794582416111152054791847908575270538922907922166139")}
		};
	std::cout << "2 * P = " << P * NTL::ZZ{ 2 } << std::endl;
	std::cout << "3 * P = " << P * NTL::ZZ{ 3 } << std::endl;*/

	/*
	x = 37769757955010115977127304355062928723197738573594517277271664470004063185943
	y = 48032793768151135690291781066295402287164455639138904110329096724933727716580
	
	*/

	//606060606066666666660606060606

	/*NTL::ZZ{NTL::conv<NTL::ZZ>("115792089237316193816632940749697632222930376074883395431335758373937458794301")},
		NTL::ZZ{NTL::conv<NTL::ZZ>("34334035839408562019501817952950963779927347618196862229609338845347939870735")},
		NTL::ZZ{NTL::conv<NTL::ZZ>("100084083384483170557423172468432397335238482462053505107296731479523599110024")}*/

	return 0;
}



////165189498165132165487496513251564984676   9  4
//484984651351351654984   2 5