/*
 * general.cpp
 *
 *  Created on: 16.02.2014
 *      Author: cheshire
 */

#include "general.hpp"
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include "mainParameters.hpp"
using namespace std;

Vec3d operator-(const Vec3d &a, const Vec3d &b) {
	return Vec3d(a.x - b.x, a.y - b.y, a.z - b.z);
}

Vec3d operator+(const Vec3d &a, const Vec3d &b) {
	return Vec3d(a.x + b.x, a.y + b.y, a.z + b.z);
}

Vec3d operator*(const Vec3d &a, double t) {
	return Vec3d(a.x * t, a.y * t, a.z * t);
}

double dot(const Vec3d &a){
	return a.x*a.x+a.y*a.y+a.z*a.z;
}

Vec3d operator/(const Vec3d &a, double t) {
	return Vec3d(a.x / t, a.y / t, a.z / t);
}

double dot2(const Vec3d &a, const Vec3d &b){
	return a.x*b.x+a.y*b.y+a.z*b.z;
}

double rdrand(int a){
	long long int r;
	asm("rdrand %0" : "=r" (r));

	return (double)(r%a)/a;
}

double ngrand(int a){
	double r=(double)(rand()%a)/a;
	return 1 - 2*r;
}

double normal(double mean, double stddev){
	double S=0;
	double NRAND=20;
	for (int i=0; i<NRAND; i++)
		S+=ngrand(100000);

	double normalStdDiv=2.0/sqrt(12.0);
	double value=S/(normalStdDiv*sqrt(NRAND));
	return value*stddev;
}
