/*
 * tPlane.cpp
 *
 *  Created on: 21.02.2014
 *      Author: cheshire
 */

#include "tPlane.hpp"

#include <iostream>
#include <math.h>
using namespace std;

tPlane::tPlane() {
	r0 = Vec3d();
	N = Vec3d();
	limit_area = 0;
}

tPlane::tPlane(Vec3d _N, Vec3d _r0) {
	this->N = _N;
	N = N / sqrt(dot(N));

	this->r0 = _r0;
	limit_area = 0;
}

tPlane::tPlane(Vec3d _N, Vec3d _r0, double _lim_area) {
	this->N = _N;
	N = N / sqrt(dot(N));

	this->r0 = _r0;
	limit_area = _lim_area;
}

double tPlane::cross(tRay ray) {
	double p2 = dot2(N, ray.k);
	if (p2 < 0) {
		N.x = -N.x;
		N.y = -N.y;
		N.z = -N.z;
		p2 = dot2(N, ray.k);
	}

	double p1 = dot2(N, ray.b - r0);
	double t = -(p1 / p2);

	if (ray.I != tDumpPlane::OBJECT)
		return -1;

	return t;
}

tRay tPlane::crossAndGen(tRay ray, double &t) {
	t = this->cross(ray);
	if (t <= 0) {
		t = -1;
		return tRay(Vec3d(0, 0, 0), Vec3d(0, 0, 0), -1);
	}

	Vec3d point = ray.trace(t);

	if ((limit_area != 0 && dot(point - r0) > limit_area * limit_area) || ray.I != tDumpPlane::OBJECT) {
		t = -1;
		return tRay(Vec3d(0, 0, 0), Vec3d(0, 0, 0), -1);
	}

	Vec3d direction = ray.k / sqrt(dot(ray.k));

	double P = RadToGrad( acos(dot2(direction,N)/(sqrt(dot(direction))*sqrt(dot(N)))) );

	Vec3d dirNormal = N * dot2(direction, N);
	Vec3d dirTangential = direction - dirNormal;

	/*cout << "(Dx,Dy,Dz):(" << direction.x << "," << direction.y << ","
	 << direction.z << ")" << endl;
	 cout << "(Dnx,Dny,Dnz):(" << dirNormal.x << "," << dirNormal.y << ","
	 << dirNormal.z << ")" << endl;
	 cout << "(Dtx,Dty,Dtz):(" << dirTangential.x << "," << dirTangential.y
	 << "," << dirTangential.z << ")" << endl;*/

	Vec3d newDirection = dirTangential - dirNormal;

	P = 	RadToGrad( acos(dot2(newDirection,N)/(sqrt(dot(newDirection))*sqrt(dot(N)))) );

	tRay toRet(point, newDirection, ray.lambda);
	toRet.I = ray.I;
	return toRet;
}

Vec3d tPlane::crossPoint(tRay ray) {
	double t = this->cross(ray);
	if (t <= 0)
		return Vec3d(VERY_BIG_NUM, VERY_BIG_NUM, VERY_BIG_NUM);
	return ray.trace(t);
}

tGrid::tGrid() {
	plane = tPlane();
	r0 = Vec3d();
	transp = &defaultFunction;
}

tGrid::tGrid(Vec3d _N, Vec3d _r0, double _lim_area,
		double (*_transp)(Vec3d, tPlane)) {
	plane = tPlane(_N, _r0, _lim_area);

	r0 = _r0;
	transp = _transp;
}

tRay tGrid::crossAndGen(tRay ray, double &t) {
	t = plane.cross(ray);
	Vec3d p = plane.crossPoint(ray);
	if (p.x == VERY_BIG_NUM)
		return tRay(Vec3d(0, 0, 0), Vec3d(0, 0, 0), -1);
	//
	double v = transp(p, plane);

	ray.I = v;
	ray.b = p;
	return ray;
}

bool tDumpPlane::check(Vec3d p){
	Vec3d zDirection(0, 1, 0);
	double phi = acos(
			dot2(zDirection, N) / (sqrt(dot(zDirection)) * sqrt(dot(N))));

	Vec3d op=p - this->r0;
	double px=op.x;
	double py=op.y;
	double pz=op.z;

	op.x=px*cos(phi) - py*sin(phi);
	op.y=px*sin(phi) - py*cos(phi);

	if (fabs(op.x) > limit_area_w || fabs(op.z) > limit_area_h) return false;

	return true;
}

tDumpPlane::tDumpPlane() {
	r0 = Vec3d();
	N = Vec3d();
	limit_area = 0;
	//out.open("out.dmp");
	out = fopen("out.dmp", "w+");
	dumpHead();

	tmpVectorPosition = 0;
	tmpVector.resize(tmpVectorSize);
	dumpPattern=IMAGE;
}

tDumpPlane::tDumpPlane(Vec3d _N, Vec3d _r0) {
	this->N = _N;
	N = N / sqrt(dot(N));

	this->r0 = _r0;
	limit_area = 0;
	limit_area_h=limit_area_w=limit_area;
	out = fopen("out.dmp", "w+");
	dumpHead();

	tmpVectorPosition = 0;
	tmpVector.resize(tmpVectorSize);
	dumpPattern=IMAGE;
}

tDumpPlane::tDumpPlane(Vec3d _N, Vec3d _r0, double _lim_area, char* dumpName) {
	this->N = _N;
	N = N / sqrt(dot(N));

	this->r0 = _r0;
	limit_area = _lim_area;
	limit_area_h=limit_area_w=limit_area;

	out = fopen(dumpName, "w+");
	dumpHead();

	tmpVectorPosition = 0;
	tmpVector.resize(tmpVectorSize);
	dumpPattern=IMAGE;
}

tDumpPlane::tDumpPlane(Vec3d _N, Vec3d _r0, double _lim_area_w,double _lim_area_h, char* dumpName) {
	this->N = _N;
	N = N / sqrt(dot(N));

	this->r0 = _r0;
	limit_area = _lim_area_w;

	limit_area_h=_lim_area_h;
	limit_area_w=_lim_area_w;

	out = fopen(dumpName, "w+");
	dumpHead();

	tmpVectorPosition = 0;
	tmpVector.resize(tmpVectorSize);
	dumpPattern=IMAGE;
}

void tDumpPlane::dumpHead() {
	extern double breggAngle;

	//new Angle
	Vec3d refDirection(-1,0,0);
	double angle=acos(dot2(refDirection,this->N));

	angle*=180.0/M_PI;

	fprintf(out, "#[PLANE]\n");

	fprintf(out, "#[ScatteringAngle]\t=\t%0.10f\n", angle);

	fprintf(out, "#[X,Y,Z]\t=\t[%0.10f, %0.10f, %0.10f];\n", r0.x, r0.y,
			r0.z);

	fprintf(out, "#[Nx,Ny,Nz]\t=\t[%0.10f, %0.10f, %0.10f];\n", N.x, N.y,
			N.z);
			
	fprintf(out, "#Size:\t=\t%0.10f\t%0.10f;\n", limit_area_w,limit_area_h);
	fprintf(out,
			"#------------------------------------------------------------\n");
}

double tDumpPlane::cross(tRay ray) {
	double p2 = dot2(N, ray.k);
	if (p2 < 0) {
		N.x = -N.x;
		N.y = -N.y;
		N.z = -N.z;
		p2 = dot2(N, ray.k);
	}

	double p1 = dot2(N, ray.b - r0);
	double t = -(p1 / p2);

	Vec3d point = ray.trace(t);

	if ((limit_area != 0 && check(point) == false) || ray.I != dumpPattern)
		return -1;

	return t;
}

void tDumpPlane::setCrossPattern(int pattern){
	dumpPattern=pattern;
}

tDumpPlane::~tDumpPlane() {
	for (int i = 0; i < tmpVectorPosition; i++)
		fprintf(out, "%0.10f\t%0.10f\t%0.10f\t%0.10f\t%0.10f\n",
				tmpVector[i].point.x, tmpVector[i].point.y,
				tmpVector[i].point.z, tmpVector[i].lambda, tmpVector[i].I);
	fflush(out);
	fclose(out);
}

tRay tDumpPlane::crossAndGen(tRay ray, double &t) {
	t = this->cross(ray);
	if (t <= 0) {
		t = -1;
		return tRay(Vec3d(0, 0, 0), Vec3d(0, 0, 0), -1);
	}

	Vec3d point = ray.trace(t);

	if (limit_area != 0 && check(point) == false) {
		t = -1;
		return tRay(Vec3d(0, 0, 0), Vec3d(0, 0, 0), -1);
	}

	if (tmpVectorPosition >= tmpVectorSize) {
		tmpVectorPosition = 0;
		for (int i = 0; i < tmpVector.size(); i++)
			fprintf(out, "%0.10f\t%0.10f\t%0.10f\t%0.10f\t%0.10f\n",
					tmpVector[i].point.x, tmpVector[i].point.y,
					tmpVector[i].point.z, tmpVector[i].lambda, tmpVector[i].I);
	} else {
		tmpVector[tmpVectorPosition].point = point;
		tmpVector[tmpVectorPosition].lambda = ray.lambda;
		tmpVector[tmpVectorPosition].I = ray.I;
		tmpVectorPosition++;
	}

	tRay toRet(point, ray.k, ray.lambda);
	toRet.I = ray.I;
	return toRet;
}

Vec3d tDumpPlane::crossPoint(tRay ray) {
	double t = this->cross(ray);
	if (t <= 0)
		return Vec3d(VERY_BIG_NUM, VERY_BIG_NUM, VERY_BIG_NUM);
	return ray.trace(t);
}
