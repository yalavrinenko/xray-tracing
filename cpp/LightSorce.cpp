/*
 * LightSorce.cpp
 *
 *  Created on: 11.02.2014
 *      Author: cheshire
 */

#include "LightSorce.hpp"
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <time.h>

using namespace std;

#define RAND_CONST 16348

inline double grand(int A) {
	return rand() % A;
}

LightSorce::LightSorce() {
	this->position = Vec3d(0, 0, 0);
	this->direction = Vec3d(1, 1, 1);
	this->H = this->L = 1;
}

LightSorce::LightSorce(Vec3d _position, Vec3d _direction, double _l,
		double _h) {
	position = _position;
	direction = _direction;
	L = _l;
	H = _h;
}

tRay* LightSorce::GenerateRays(double lambda, double dlambda, int count,
		double R) {
	tRay* res;
	res = new tRay[count];

	srand(time(0));

	int index = 0;

	double stddevLambda = dlambda / (sqrt(2.0 * log(2.0)));

	for (int i = 0; i < count; i++) {
		double __lambda = lambda + normal(0.0, stddevLambda);
		Vec3d b = position;
		b.x += grand(RAND_CONST) * L / RAND_CONST - L / 2;
		b.y += grand(RAND_CONST) * H / RAND_CONST - H / 2;

		Vec3d k = direction;
		double kx = grand(RAND_CONST) * R / RAND_CONST - R / 2;
		double ky = grand(RAND_CONST) * R / RAND_CONST - R / 2;
		k.x += kx;
		k.y += ky;

		k = k - b;

		if (kx * kx + ky * ky <= R * R) {
			res[index] = tRay(b, k, __lambda);
			res[index].I = 1;
			index++;
		} else
			i--;
	}
	cout << "Gen " << count << " rays" << endl;
	return res;
}

circleLightSorce::circleLightSorce() {
	this->position = Vec3d(0, 0, 0);
	this->direction = Vec3d(1, 1, 1);
	this->R = 1.0;
}
circleLightSorce::circleLightSorce(Vec3d _position, Vec3d _direction,
		double _r) {
	position = _position;
	direction = _direction;
	R = _r;
}

tRay* circleLightSorce::GenerateRays(double lambda, double dlambda, int count,
		double _R) {
	tRay* res;
	res = new tRay[count];

	srand(time(0));

	int index = 0;

	double stddevLambda = dlambda / (sqrt(2.0 * log(2.0)));

	for (int i = 0; i < count; i++) {
		double __lambda = lambda + normal(0.0, stddevLambda);
		Vec3d b = position;
		double bx = grand(RAND_CONST) * R / RAND_CONST - R / 2;
		double by = grand(RAND_CONST) * R / RAND_CONST - R / 2;

		b.x += bx;
		b.y += by;

		Vec3d k = direction;
		double kx = grand(RAND_CONST) * _R / RAND_CONST - _R / 2;
		double ky = grand(RAND_CONST) * _R / RAND_CONST - _R / 2;

		k.x += kx;
		k.y += ky;

		k = k - b;

		if (kx * kx + ky * ky <= _R * _R && bx * bx + by * by <= R * R) {
			res[index] = tRay(b, k, __lambda);
			res[index].I = 1;
			index++;
		} else
			i--;
	}
	cout << "Gen " << count << " rays" << endl;
	return res;
}

SphereLight::SphereLight() {
	this->position = Vec3d(0, 0, 0);
	this->direction = Vec3d(1, 1, 1);
	dx.x = dy.x = dz.x = 0;
	dx.y = dy.y = dz.y = 1;
	R = 1;
	Rw = R;
	Rh = R;
	apperture = 0.1;
	type = sphereType;
	breggAngle = 0;

	srand(time(0));
}

SphereLight::SphereLight(Vec3d _position, Vec3d _direction, double _app,
		double _R, double _bA) {
	breggAngle = _bA;
	position = _position;
	direction = _direction;
	apperture = _app;
	R = _R;
	Rw = R;
	Rh = R;
	r = direction - position;
	r = r / sqrt(dot(r));

	dx.x = (r.x < 0) ? -1 : 0;
	dy.x = (r.y < 0) ? -1 : 0;
	dz.x = (r.z < 0) ? -1 : 0;

	dx.y = (r.x < 0) ? 0 : 1;
	dy.y = (r.y < 0) ? 0 : 1;
	dz.y = (r.z < 0) ? 0 : 1;

	type = sphereType;

	srand(time(0));
}

SphereLight::SphereLight(Vec3d _position, Vec3d _direction, double _app,
		double _Rw, double _Rh, double _bA) {
	breggAngle = _bA;
	position = _position;
	direction = _direction;
	apperture = _app;
	R = _Rw;
	Rw = _Rw;
	Rh = _Rh;

	r = direction - position;
	r = r / sqrt(dot(r));

	dx.x = (r.x < 0) ? -1 : 0;
	dy.x = (r.y < 0) ? -1 : 0;
	dz.x = (r.z < 0) ? -1 : 0;

	dx.y = (r.x < 0) ? 0 : 1;
	dy.y = (r.y < 0) ? 0 : 1;
	dz.y = (r.z < 0) ? 0 : 1;

	type = sphereType;

	srand(time(0));
}

tRay* SphereLight::GenerateRays(double lambda, double dlambda, int count) {

	if (type == cylindricType)
		return generateCylindricRays(lambda, dlambda, count);

	tRay* res;
	res = new tRay[count];

	int index = 0;
	double stddevLambda = dlambda / (sqrt(2.0 * log(2.0)));
	for (int i = 0; i < count; i++) {
		double __lambda = lambda + normal(0.0, stddevLambda);
		Vec3d b = position;
		b.x += (double) grand(RAND_CONST) * 2 * Rw / (double) RAND_CONST - Rw;
		b.y += (double) grand(RAND_CONST) * 2 * Rw / (double) RAND_CONST - Rw;
		b.z += (double) grand(RAND_CONST) * 2 * Rh / (double) RAND_CONST - Rh;

		Vec3d a = direction;
		a = a / sqrt(dot(a));

		Vec3d p;
		double r = tan(this->apperture);
		p.x = (double) grand(RAND_CONST) * (2 * r) / (double) RAND_CONST - (r);
		p.y = (double) grand(RAND_CONST) * (2 * r) / (double) RAND_CONST - (r);
		p.z = (double) grand(RAND_CONST) * (2 * r) / (double) RAND_CONST - (r);
		p = p + a;

		if (dot(p - a) > r * r) {
			double minAppert = (p.x - a.x) * (p.x - a.x)
					+ (p.z - a.z) * (p.z - a.z);

			if (minAppert >= apperture) {
				i--;
				continue;
			}

			double newAppert = minAppert
					+ (double) grand(RAND_CONST) * (apperture - minAppert)
							/ (double) RAND_CONST;
			r = tan(newAppert);

			double C = r * r - (p.x - a.x) * (p.x - a.x)
					- (p.z - a.z) * (p.z - a.z);
			p.y = sqrt(C) + a.y;
		}

		p = p / sqrt(dot(p));

		if (isnan(p.x) || isnan(p.y) || isnan(p.z)) {
			i--;
			continue;
			cout << "NaN" << endl;
		}

		double appAngle = acos(dot2(p, a));

		Vec3d tr=b - position;
		tr.x=tr.x / Rw;
		tr.z=tr.z / Rh;
		tr.y=tr.y / R;

		if (dot(tr) <= 1.0){
			res[index] = tRay(b, p, __lambda);
			res[index].I = 1;
			index++;
		} else
			i--;
	}
	//cout << "Gen " << count << " rays" << endl;
	return res;
}

void SphereLight::setCylinricType(double _h, int _orientation) {
	type = cylindricType;
	H = _h;
	orientation = _orientation;
}
void SphereLight::setSphereType() {
	type = sphereType;
}

tRay* SphereLight::generateCylindricRays(double lambda, double dlambda,
		int count) {
	tRay* res;
	res = new tRay[count];

	int index = 0;
	double stddevLambda = dlambda / (sqrt(2.0 * log(2.0)));
	for (int i = 0; i < count; i++) {
		double __lambda = lambda + normal(0.0, stddevLambda);
		Vec3d b = position;

		double dx, dy, dz;

		double rDx;
		double rDy;

		switch (orientation) {
		case xOz:
			dx = (double) grand(RAND_CONST) * 2 * R / (double) RAND_CONST - R;
			dy = (double) grand(RAND_CONST) * 2 * R / (double) RAND_CONST - R;
			dz = (double) grand(RAND_CONST) * H / (double) RAND_CONST - H / 2.0;

			if (dx * dx + dy * dy > R * R) {
				i--;
				continue;
			}
			break;
		case xOy:
			dz = (double) grand(RAND_CONST) * 2 * R / (double) RAND_CONST - R;
			dy = (double) grand(RAND_CONST) * 2 * R / (double) RAND_CONST - R;
			dx = (double) grand(RAND_CONST) * H / (double) RAND_CONST - H / 2.0;

			if (dz * dz + dy * dy > R * R) {
				i--;
				continue;
			}

			rDx = dx * cos(breggAngle) - dy * sin(breggAngle);
			rDy = dx * sin(breggAngle) + dy * cos(breggAngle);
			dx = rDx;
			dy = rDy;

			break;
		case yOz:
			dx = (double) grand(RAND_CONST) * 2 * R / (double) RAND_CONST - R;
			dz = (double) grand(RAND_CONST) * 2 * R / (double) RAND_CONST - R;
			dy = (double) grand(RAND_CONST) * H / (double) RAND_CONST - H / 2.0;

			if (dx * dx + dz * dz > R * R) {
				i--;
				continue;
			}

			rDx = dx * cos(breggAngle) - dy * sin(breggAngle);
			rDy = dx * sin(breggAngle) + dy * cos(breggAngle);
			dx = rDx;
			dy = rDy;

			break;
		};

		b.x += dx;
		b.y += dy;
		b.z += dz;

		Vec3d a = direction;
		a = a / sqrt(dot(a));

		Vec3d p;
		double r = tan(this->apperture);
		p.x = (double) grand(RAND_CONST) * (2 * r) / (double) RAND_CONST - (r);
		p.y = (double) grand(RAND_CONST) * (2 * r) / (double) RAND_CONST - (r);
		p.z = (double) grand(RAND_CONST) * (2 * r) / (double) RAND_CONST - (r);

		p = p + a;

		if (dot(p - a) > r * r) {
			double minAppert = (p.x - a.x) * (p.x - a.x)
					+ (p.z - a.z) * (p.z - a.z);

			if (minAppert >= apperture) {
				i--;
				continue;
			}

			double newAppert = minAppert
					+ (double) grand(RAND_CONST) * (apperture - minAppert)
							/ (double) RAND_CONST;
			r = tan(newAppert);

			double C = r * r - (p.x - a.x) * (p.x - a.x)
					- (p.z - a.z) * (p.z - a.z);
			p.y = sqrt(C) + a.y;
		}

		p = p / sqrt(dot(p));

		if (isnan(p.x) || isnan(p.y) || isnan(p.z)) {
			i--;
			continue;
			cout << "NaN" << endl;
		}

		double appAngle = acos(dot2(p, a));

		if (appAngle > this->apperture)
			cout << "!" << endl;

		res[index] = tRay(b, p, __lambda);
		res[index].I = 1;
		index++;
	}
	cout << "Gen " << count << " rays" << endl;
	return res;
}
