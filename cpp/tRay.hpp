/*
 * tRay.hpp
 *
 *  Created on: 08.02.2014
 *      Author: cheshire
 */

#ifndef TRAY_HPP_
#define TRAY_HPP_

#include "general.hpp"
#include <math.h>

struct tRay{
	Vec3d b;
	Vec3d k;

	double lambda;
	double I;

	tRay(Vec3d _b,Vec3d _k, double l):
		b(_b), k(_k), lambda(l){
		k=k/sqrt(dot(k));
	}
	tRay():b(),k(),lambda(0){
	}

	Vec3d trace(double t){
		return k*t+b;
	}
};


#endif /* TRAY_HPP_ */
