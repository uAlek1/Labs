#include <fstream>
#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <set>
#include <random>
struct PRNG
{
	std::mt19937 engine;
};
void initGenerator(PRNG& generator)
{
	std::random_device device;
	generator.engine.seed(device());
}

unsigned long long random(PRNG& generator, unsigned long long minValue, unsigned long long maxValue)
{

	std::uniform_int_distribution<unsigned long long> distribution(minValue, maxValue);
	return distribution(generator.engine);
}

template<typename T>
struct math_vector {
	std::vector<T> vec;

	math_vector(std::vector<T> cf) {
		vec = cf;
	}
	math_vector(int len) {
		vec = std::vector<T>(len, 0);
	}

	math_vector<T>& operator=(math_vector<T> const& rhs) {
		if (this != &rhs) {
			vec = rhs.vec;
		}
		return *this;
	};
	template<typename U>
	math_vector<T>& operator=(math_vector<U>& rhs) {
		for (int i = 0; i < rhs.vec.size(); i++) {
			vec[i] = static_cast<T>(rhs.vec[i]);
		}
		return *this;
	};

	math_vector operator+(const math_vector& right) const {
		if (vec.size() != right.vec.size()) {
			throw std::exception();
		}
		std::vector<T> tmp;
		for (auto vec_el = vec.begin(), end = vec.end(), right_el = right.vec.begin(); vec_el != end; vec_el++, right_el++) {
			tmp.push_back(*vec_el + *right_el);
		}
		return math_vector(tmp);
	}
	math_vector operator-(const math_vector& right) const {
		if (vec.size() != right.vec.size()) {
			throw std::exception();
		}
		math_vector tmp(right.vec.size());
		for (int i = 0, end = right.vec.size(); i<end;i++){
			tmp.vec[i] = -right.vec[i];
		}
		return *this + tmp;
	}

	T operator*(const math_vector<T>& right) const {
		if (vec.size() != right.vec.size()) {
			throw std::exception();
		}
		T tmp = 0;
		for (auto vec_el = vec.begin(), end = vec.end(), right_el = right.vec.begin(); vec_el != end; vec_el++, right_el++) {
			tmp += *vec_el * *right_el;
		}
		return tmp;
	}

	math_vector operator*(T cf) const {
		math_vector tmp(vec);
		for (int i = 0; i < vec.size(); i++) {
			tmp.vec[i] = vec[i] * cf;
		}
		return tmp;
	}

	double norm() {
		double scalar = *this * *this;
		return sqrt(scalar);
	}
	int size() {
		return vec.size();
	}

	void print() {
		std::cout << vec[0];
		for (auto cur = vec.begin()+1, end = vec.end(); cur != end; cur++) {
			std::cout <<", "<< *cur;
		}
		std::cout << std::endl;
	}
};

template<typename T>
void orthogonalization(std::vector<math_vector<T>>& basis, std::vector<math_vector<double>>& ort_basis, std::vector<std::vector<double>>& mu) {
	ort_basis[0] = basis[0];
	for (int i = 1; i < basis.size(); i++) {
		ort_basis[i] = basis[i];
		{
			const math_vector<double> tmp = ort_basis[i];
			for (int j = 0; j < i; j++) {
				mu[i][j] = tmp * ort_basis[j] / (ort_basis[j] * ort_basis[j]);
				ort_basis[i] = ort_basis[i] - (ort_basis[j] * mu[i][j]);
			}
		}
	}
}

void LLL_cast(std::vector<math_vector<int>>& basis) {
	std::cout << std::endl;
	double eps = 0.00000000001;
	std::vector<std::vector<double>> mu(basis.size(), std::vector<double>(basis.size(), 1));
	std::vector<math_vector<double>> orthogonal_basis(basis.size(), math_vector<double>(basis[0].size()));
	std::vector<double> B(basis.size());
	orthogonalization(basis, orthogonal_basis, mu);
	for (int i = 0; i < basis.size(); i++) {
		B[i] = orthogonal_basis[i] * orthogonal_basis[i];
	}

	int k = 1, n = basis.size();

	while (k < n) {
		if (std::abs(mu[k][k - 1]) - 0.5 > eps) {
			int r = static_cast<int>(std::round(mu[k][k - 1]));
			basis[k] = basis[k] - (basis[k - 1] * r);
			for (int j = 0; j < k - 1; j++) {
				mu[k][j] = mu[k][j] - (r * mu[k - 1][j]);
			}
			mu[k][k - 1] = mu[k][k - 1] - r;
		}

		double right_part = (0.75 - (mu[k][k - 1] * mu[k][k - 1])) * B[k - 1];
		if (B[k] - right_part >= eps) {
			for (int l = k - 2; l >= 0; l--) {
				if (std::abs(mu[k][l]) - 0.5 > eps) {
					int r = static_cast<int>(std::round(mu[k][l]));
					basis[k] = basis[k] - (basis[l] * r);
					for (int j = 0; j < l; j++) {
						mu[k][j] = mu[k][j] - (r * mu[l][j]);
					}
					mu[k][l] = mu[k][l] - r;
				}
			}
			k++;
		}
		else {
			double m = mu[k][k - 1];
			double b = B[k] + m * m * B[k - 1];
			mu[k][k - 1] = m * B[k - 1] / b;
			B[k] = B[k - 1] * B[k] / b;
			B[k - 1] = b;
			std::swap(basis[k], basis[k - 1]);

			for (int j = 0; j < k - 1; j++) {
				std::swap(mu[k][j], mu[k - 1][j]);
			}
			for (int s = k + 1; s < n; s++) {
				double t = mu[s][k];
				mu[s][k] = mu[s][k - 1] - m * t;
				mu[s][k - 1] = t + mu[k][k - 1] * mu[s][k];
			}
			k = std::max(k - 1, 1);
		}
	}
}

bool LLL_test(std::vector<math_vector<int>>& basis) {
	std::vector<std::vector<double>> mu(basis.size(), std::vector<double>(basis.size(), 1));
	std::vector<math_vector<double>> orthogonal_basis(basis.size(), math_vector<double>(basis[0].size()));
	orthogonalization(basis, orthogonal_basis, mu);
	for (int i = 1; i < basis.size(); i++) {
		for (int j = 0; j < i; j++) {
			if (mu[i][j] > 0.5 || mu[i][j] < -0.5) {
				return false;
			}
		}
	}

	std::cout << "pass mu\n";
	for (int i = 1; i < basis.size(); i++) {
		math_vector<double> left = orthogonal_basis[i] + (orthogonal_basis[i - 1] * mu[i][i - 1]);
		if (left * left < 0.75 * (orthogonal_basis[i - 1] * orthogonal_basis[i - 1])) {
			return false;
		}
	}
	std::cout << "pass proj\n";

	/*for (int i = 1; i < basis.size(); i++) {
		double b_i = orthogonal_basis[i] * orthogonal_basis[i];
		for (int j = 0; j < i; j++) {
			double left = orthogonal_basis[j] * orthogonal_basis[j];
			if (left > b_i * std::pow(2, i - j)) {
				return false;
			}
		}
	}
	std::cout << "pass b_i\n";

	for (int j = 0; j < basis.size() - 1; j++) {
		double b_j = orthogonal_basis[j] * orthogonal_basis[j];
		for (int i = j+1; i < basis.size(); i++) {
			double right = basis[i] * basis[i];
			if (b_j > right * std::pow(2, i)) {
				std::cout << i << " " << j << std::endl;
				return false;
			}
		}
	}

	std::cout << "pass b_j\n";*/
	return true;

}

void LLL_cast(std::vector<math_vector<double>>& basis) {
	double eps = 0.00000001;
	std::vector<std::vector<double>> mu(basis.size(), std::vector<double>(basis.size(), 1));
	std::vector<math_vector<double>> orthogonal_basis(basis.size(), math_vector<double>(basis[0].size()));
	std::vector<double> B(basis.size());
	orthogonalization(basis, orthogonal_basis, mu);
	/*for (int i = 0; i < mu.size(); i++) {

	}*/
	for (int i = 0; i < basis.size(); i++) {
		B[i] = orthogonal_basis[i] * orthogonal_basis[i];
	}

	int k = 1, n = basis.size();

	while (k < n) {
		if (std::abs(mu[k][k - 1]) - 0.5 > eps) {
			long long r = static_cast<long long> (std::round(mu[k][k - 1]));
			basis[k] = basis[k] - (basis[k - 1] * static_cast<double>(r));
			for (int j = 0; j < k - 1; j++) {
				mu[k][j] = mu[k][j] - (r * mu[k - 1][j]);
			}
			mu[k][k - 1] = mu[k][k - 1] - r;
		}

		double right_part = (0.75 - (mu[k][k - 1] * mu[k][k - 1])) * B[k - 1];
		if (B[k] - right_part >= eps) {
			for (int l = k - 2; l >= 0; l--) {
				if (std::abs(mu[k][l]) - 0.5 > eps) {
					long long r = static_cast<long long>(std::round(mu[k][l]));
					basis[k] = basis[k] - (basis[l] * static_cast<double>(r));
					for (int j = 0; j < l; j++) {
						mu[k][j] = mu[k][j] - (r * mu[l][j]);
					}
					mu[k][l] = mu[k][l] - r;
				}
			}
			k++;
		}
		else {
			double m = mu[k][k - 1];
			double b = B[k] + m * m * B[k - 1];
			mu[k][k - 1] = m * B[k - 1] / b;
			B[k] = B[k - 1] * B[k] / b;
			B[k - 1] = b;
			std::swap(basis[k], basis[k - 1]);

			for (int j = 0; j < k - 1; j++) {
				std::swap(mu[k][j], mu[k - 1][j]);
			}
			for (int s = k + 1; s < n; s++) {
				double t = mu[s][k];
				mu[s][k] = mu[s][k - 1] - m * t;
				mu[s][k - 1] = t + mu[k][k - 1] * mu[s][k];
			}
			k = std::max(k - 1, 1);
		}
	}
}


std::vector<int> solve(std::vector<unsigned long long> a, unsigned long long s) {
	double eps = 0.0000001;
	int m = static_cast<int>(std::ceil(0.5 * sqrt(a.size())));
	std::vector<math_vector<double>> basis(a.size() + 1, math_vector<double>(a.size() + 1));
	for (int i = 0; i < a.size(); i++) {
		math_vector<double> tmp(a.size() + 1);
		tmp.vec[i] = 1;
		unsigned long long temp = m * a[i];
		tmp.vec[a.size()] = static_cast<double>(temp);
		basis[i] = tmp;
	}
	std::vector<double> tmp(a.size() + 1, 0.5);
	unsigned long long temp = m * s;
	tmp[a.size()] = static_cast<double>(temp);
	basis[a.size()] = tmp;
	/*for (int i = 0; i < basis.size(); i++) {
		basis[i].print();
	}*/

	LLL_cast(basis);
	//std::cout << std::endl;
	//for (int i = 0; i < basis.size(); i++) {
	//	basis[i].print();
	//}

	int n = basis.size();
	std::vector<int> x{ n, 0}, x2{ n, 0};

	for (int i = 0; i < n; i++) {
		bool success = true;
		x = std::vector<int>(n, 0);
		x2 = std::vector<int>(n, 0);
		if (std::abs(basis[i].vec[n-1]) <= eps) {
			for (int j = 0; j < basis[i].vec.size(); j++) {
				if (std::abs(basis[i].vec[j]) - 0.5 <= eps) {
					x[j] = static_cast<int>(std::round(basis[i].vec[j] + 0.5));
					x2[j] = static_cast<int>(std::round(-basis[i].vec[j] + 0.5));
				}
				else {
					success = false;
					break;
				}
			}
			if (success) {
				unsigned long long s1 = 0, s2 = 0;
				for (int i = 0; i < a.size(); i++) {
					s1 += a[i] * x[i];
					s2 += a[i] * x2[i];
				}
				//std::cout << s1 << " " << s2 << " " << s << std::endl;
				if (s1 == s) {
					return x;
				}
				if (s2 == s) {
					return x2;
				}
			}
		}
	}

	return std::vector<int>();
}

int main() {
	int ver = 3;
	if (ver == 1) {
		std::ifstream fin("C:\\Users\\alust\\source\\repos\\LLL-basis\\LLL-basis\\basis.txt");

		uint16_t rows, columns;
		fin >> rows;
		fin >> columns;
		std::vector<math_vector<int>> basis(rows, columns);
		for (uint16_t i = 0; i < rows; i++) {
			for (uint16_t j = 0; j < columns; j++) {
				fin >> basis[i].vec[j];
			}
		}
		fin.close();
		std::cout << "input matrix: " << std::endl;
		for (int i = 0; i < basis.size(); i++) {
			basis[i].print();
		}

		LLL_cast(basis);


		std::cout << std::endl;
		std::cout << "LLL cast: " << std::endl;
		for (int i = 0; i < basis.size(); i++) {
			basis[i].print();
		}
		std::cout << std::endl;
		std::cout << "is LLL basis: " << LLL_test(basis) << std::endl;
		std::cout << std::endl;
	}
	else if(ver == 2) {
		std::vector<unsigned long long> a = { 1513, 5118, 917, 3825, 140, 2812, 1441, 429, 2489, 1847 };
		int s = 10429;
		for (int i = 0; i < a.size(); i++) {
			std::cout <<a[i]<<" ";
		}
		std::cout<<std::endl;
		std::cout << "s = " << s << std::endl;
		std::vector<int> res = solve(a, s);
		if (res.size() != 0) {
			for (int i = 0; i < res.size(); i++) {
				std::cout << res[i] << " ";
			}
			std::cout << std::endl;
			int ss = 0;
			std::cout << a[0] * res[0];
			for (int i = 1; i < a.size(); i++) {
				std::cout << "+" << a[i] * res[i];
				ss += a[i] * res[i];
			}
			std::cout << "=" << ss << std::endl;
		}
		else {
			std::cout << "Not find solve\n";
		}
	}
	else {
		PRNG generator;
		initGenerator(generator);

		int n = 20;
		unsigned long long rangeMin = static_cast<unsigned long long>(pow(2,10)),
			rangeMax = static_cast<unsigned long long>(pow(2, 11)),
			a;
		int count_success = 0;
		std::vector<unsigned long long> public_key(n);
		std::set<unsigned long long> tmp;
		a = random(generator, rangeMin, rangeMax);
		if (a > rangeMax) {
			std::cout << "WTF\n";
		}
		if(a < rangeMin) {
			std::cout << "WTF\n";
		}
		std::cout << n << "   " <<log2(a)<<"   " << n/log2(a)<< std::endl;
		for (int w = 0; w < 25; w++) {
			int count = 0;
			tmp.insert(a);
			while (tmp.size() != n) {
				tmp.insert(random(generator, 0, a));
			}
			public_key = std::vector<unsigned long long>(tmp.begin(), tmp.end());
			tmp.clear();
			unsigned long long s = 0;
			for (int i = 0; i < public_key.size(); i++) {
				if (random(generator, 0, a) & 1) {
					s += (public_key[i]);
				}
			}
			std::vector<int> res = solve(public_key, s);

			if (res.size() == 0) {
				std::cout << "not solve\n";
			}
			else {
				std::cout << "s = " << s;
				unsigned long long qwe = 0;
				for (int i = 0; i < public_key.size(); i++) {
					if (res[i]) {
						qwe += (public_key[i]);
					}
				}
				std::cout << "; s = " << qwe << std::endl;
			}
		}
		/*std::vector<math_vector<int>> basis(30, 30);

		for (uint16_t i = 0; i < 30; i++) {
			for (uint16_t j = 0; j < 30; j++) {
				basis[i].vec[j] = random(generator, rangeMin, a);
			}
		}
		LLL_cast(basis);

		for (int i = 0; i < 30; i++) {
			basis[i].print();
		}
		std::cout << LLL_test(basis) << std::endl;*/

	}
		

	return 0;
}
