#include <cstdio>
#include <cmath>
#include <iostream>
#include <cstring>
#include <algorithm>
extern "C" float sinintegral(float a, float b, float e);
/*float sinintegral(float a, float b, float e)
{
	float h = (b - a) / e;
	float x = a + h / 2;
	float integral=0;
	for (int i = 0; i < e; i++) {
		integral += sin(x) * h;
		x += h;
	}
	return integral;
}
float sinintegral1(float a, float b, float e)
{
	float h = (b - a) / e;
	float integral = 0;
	integral = h * (sin(a) + sin(b)) / 2.;
	for (int i = 0; i < e - 1; i++) {
		integral += h * sin(a + h * i);
	}
	return integral;
}
std::string  translation(long long x) {
	std::string buffer;
	while (x > 0) {
		if (x % 2 == 1) {
			buffer += '1';
		}
		if (x % 2 == 0) {
			buffer += '0';
		}
		x = x / 2;
	}
	std::reverse(buffer.begin(), buffer.end());
	return buffer;
}*/
std::string  translation1(long long x) {
	std::string buffer;
	while (x > 0) {
		if (x % 3 == 1) {
			buffer += '1';
		}
		if (x % 3 == 0) {
			buffer += '0';
		}
		if (x % 3 == 2) {
			buffer += '2';
		}
		x = x / 3;
	}
	std::reverse(buffer.begin(), buffer.end());
	return buffer;
}
int main()
{
	float res = sinintegral(0, 3.14, 1000);
	std::cout << res;
}
/ mnt / c / Users / K - PC / source / repos / OS_5