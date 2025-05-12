#include <iostream>
#include <fstream>
#include "big.h"
#include "ecn.h"
#include <vector>
#include <chrono>

Miracl precision(500, 10);

std::vector<uint8_t> getRandS() {
	std::vector<uint8_t> result;
	uint8_t m = 16, a = 5, c = 7;
	srand(time(0));
	uint8_t tmp = rand();
	tmp <<= 4;
	tmp >>= 4;
	result.push_back(tmp);
	for (int i = 1; i < m; i++) {
		tmp = (a * result[i - 1] + c);
		tmp = tmp << 4;
		tmp = tmp >> 4;
		result.push_back(tmp);
	}
	return result;
}

uint16_t f(std::vector<uint8_t> s, std::vector<uint8_t> sRand, uint8_t x) {
	return sRand[s[x]];
}

uint16_t Brent(std::vector<uint8_t> s, std::vector<uint8_t> sRand, uint16_t x) {
	uint16_t c, a;
	uint32_t n = 1, t = 1;
	c = x;
	a = f(s, sRand, c);
	std::cout << "start iter\n";
	std::cout << "c = " << c << std::endl;
	std::cout << a;
	while (1) {
		if (a == c) {
			std::cout << "tau = 1\n";
			return 1;
		}
		if (n == t) {
			std::cout << "\niter\n";
			c = a;
			t <<= 1;
			std::cout << "c = " << c << std::endl;
		}
		a = f(s, sRand, a);
		std::cout << a << ' ';
		n += 1;
		if (n >= (3 * t) / 4 && a == c) {
			break;
		}
	}
	std::cout << "exit cicle\n";
	uint32_t tau = 1;
	a = f(s, sRand, c);
	while (a != c) {
		a = f(s, sRand, a);
		tau += 1;
	}
	std::cout << "tau = " << tau << std::endl;

	std::cout << "posledovatelnost'\n";
	c = x;
	a = f(s, sRand, c);
	std::cout <<c <<' ' << a << " ";
	for (int i = 0; i < 20; i++) {
		a = f(s, sRand, a);
		std::cout << a << ' ';
	}

	std::cout << std::endl;
	return tau;
}


struct T
{
	ECn point;
	Big alpha;
	Big beta;

	T(ECn P, Big a, Big b){
		point = P;
		alpha = a;
		beta = b;
	}
};
std::vector<T> setPoints;
uint16_t f(ECn R) {
	Big x;
	R.getx(x);
	return x % 500;
}
uint64_t v(uint64_t r) {
	uint64_t s = 0;
	while (!(r & 1)) {
		r >>= 1;
		s++;
	}
	return s;
}
Big Gosper() {
	char pString[] = "3294216091";
	char mString[] = "1647090571";
	char xPString[] = "1343026426";
	char yPString[] = "2940538751";
	char xQString[] = "1606805100";
	char yQString[] = "3188929115";
	Big a(11), b(11), p(pString), xP(xPString), yP(yPString),
		xQ(xQString), yQ(yQString), m(mString);
	//std::ofstream file("C:\\Users\\alust\\source\\repos\\BVA2\\BVA2\\s.txt");
	/*char pString[] = "1264736038258721544432888496790694991";
    char qString[] = "702040259";
    char xPString[] = "993603374990895673316245812332861613";
    char yPString[] = "264419074466187872158704941697991832";
    char aString[] = "265650828522621884751814088649699757";
    char bString[] = "106166816813909454726076272656867529";*/
	/*char pString[] = "801758384607216412550221478044128847";
	char qString[] = "30698370539";
	char xPString[] = "529230763066480266597748892762292160";
	char yPString[] = "550454430014762030706185785134389718";
	char aString[] = "448282689484591754589306602456436183";
	char bString[] = "157602992008573735403998908570163206";*/

	/*char pString[] = "20073632066125574435678097206663863";
	char qString[] = "114441376873457";
	char xPString[] = "15171884549307607214453052210953062";
	char yPString[] = "387139961412689035648822185852705";
	char aString[] = "12233931199138854386417127880988847";
	char bString[] = "3924631627418033886371083593449742";*/

	//char pString[] = "620040169478814080347598404495459111";
	//char qString[] = "1021037108842083268541";
	//char xPString[] = "58325308743583271404548098476346733";
	//char yPString[] = "605321016455792699649104093755388458";
	//char aString[] = "255069583759808943428847587319973234";
	//char bString[] = "86728302837179467347529252629522477"; //2**70
	std::vector<T> points;
	/*Big a(aString), b(bString), p(pString), xP(xPString), yP(yPString),
		m(qString), d(14515);*/
	ecurve(a, b, p, MR_AFFINE);
	ECn P(xP, yP);
	//ECn Q = d*P;
	ECn Q(xQ, yQ);
	char choice;
	std::cout << "enter choice:\n1 - random data\n2 - data from file\nchoice: ";
	std::cin >> choice;
	if (choice == '1') {
		std::ofstream file("C:\\Users\\alust\\source\\repos\\BVA2\\BVA2\\s.txt");
		for (int i = 0; i < 500; i++) {
			Big randAlpha = rand(p);
			Big randBeta = rand(p);
			file << randAlpha;
			file << " ";
			file << randBeta;
			file << "\n";
			ECn randPoint = mul(randAlpha, P, randBeta, Q);
			setPoints.push_back(T(randPoint, randAlpha, randBeta));
		}
		file.close();
	}
	else {
		std::ifstream file("C:\\Users\\alust\\source\\repos\\BVA2\\BVA2\\s.txt");
		for (int i = 0; i < 500; i++) {
			Big randAlpha;
			Big randBeta;
			file >> randAlpha;
			file >> randBeta;
			ECn randPoint = mul(randAlpha, P, randBeta, Q);
			setPoints.push_back(T(randPoint, randAlpha, randBeta));

		}
		file.close();
	}
	auto start = std::chrono::system_clock::now();
	ECn R = mul(5, P, 4, Q);
	Big alpha(5), beta(4);
	points.push_back(T(R, alpha, beta));
	uint64_t n = 1, t = 1, k = 0;
	
	while (1) {

		uint16_t j = f(R);
		R += setPoints[j].point;
		alpha += setPoints[j].alpha;
		beta += setPoints[j].beta;

		for(int64_t q = t - 1; q >= 0; q--) {
			if (points[q].point == R) {
				Big inv = inverse(points[q].beta - beta, m);
				Big razn = (alpha - points[q].alpha);
				if (razn < 0) {
					razn += m;
				}
				auto end = std::chrono::system_clock::now();
				std::cout << std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << "s" << std::endl;
				return  (razn * inv) % m;;
			}

		}
		n++;
		k = v(n);
		if (k == t) {
			t++;
		}
		if (k > points.size() - 1) {
			points.push_back(T(R, alpha, beta));
		}
		else {
			points[k] = T(R, alpha, beta);
		}

	}

	return -1;

}

int main() {
	/*std::ifstream file("C:\\Users\\alust\\source\\repos\\BVA2\\BVA2\\test.txt");
	std::vector<uint8_t> s;
	uint16_t tmp = 0;
	for (int i = 0; i < 16; i++) {
		file >> tmp;
		s.push_back(tmp);
	}
	file.close();
	std::vector<uint8_t> sRand = getRandS();
	std::cout << "enter  a0: ";
	std::cin >> tmp;
	tmp = Brent(s, sRand, tmp);*/
	std::cout << "d = " << Gosper() << std::endl;
	return 0;
}
