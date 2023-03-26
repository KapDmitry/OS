#include <iostream>
#include <math.h>


extern "C"  float sinintegral(float a, float b, float e)
{
	float h = (b - a) / e;
	float x = a + h / 2;
	float integral;
	for (int i = 0; i < e; i++) {
		integral += sin(x) * h;
		x += h;
	}
	return integral;
}
extern "C" float sinintegral1(float a, float b, float e)
{
	float h = (b - a) / e;
	float integral = 0;
	integral = h * (sin(a) + sin(b)) / 2.;
	for (int i = 0; i < e - 1; i++) {
		integral += h * sin(a + h * i);
	}
	return integral;
}
	                                                                              

//g++ main.cpp -L/mnt/c/Users/K-PC/source/repos/OS_5/ libd.so -o a