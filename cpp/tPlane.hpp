/*
 * tPlane.hpp
 *
 *  Created on: 08.02.2014
 *      Author: cheshire
 */

#ifndef TPLANE_HPP_
#define TPLANE_HPP_

#include "general.hpp"
#include "tRay.hpp"
#include <fstream>
#include <vector>

class tPlane{

public:
	Vec3d r0;
	Vec3d N;

	double limit_area;

public:
	tPlane();
	tPlane(Vec3d _N,Vec3d _r0);
	tPlane(Vec3d _N,Vec3d _r0,double _lim_area);

	double cross(tRay ray);

	tRay crossAndGen(tRay ray,double &t);

	Vec3d crossPoint(tRay);
};

class tDumpPlane{

	struct outVec{
		Vec3d point;
		double I;
		double lambda;
	};

public:
	Vec3d r0;
	Vec3d N;

	FILE* out;
	double limit_area;
	double limit_area_w;
	double limit_area_h;

	std::vector<outVec> tmpVector;
	int tmpVectorPosition;
	static const int tmpVectorSize=1000;

	static const int OBJECT = 1;
	static const int IMAGE = 2;

	int dumpPattern;

	void dumpHead();

public:
	tDumpPlane();
	tDumpPlane(Vec3d _N,Vec3d _r0);
	tDumpPlane(Vec3d _N,Vec3d _r0,double _lim_area,char* dumpName);
	tDumpPlane(Vec3d _N,Vec3d _r0,double _lim_area_w,double _lim_area_h,char* dumpName);

	double cross(tRay ray);

	void setCrossPattern(int pattern);

	tRay crossAndGen(tRay ray,double &t);

	bool check(Vec3d p);

	Vec3d crossPoint(tRay);

	~tDumpPlane();
};

double defaultFunction(Vec3d p,tPlane pl);
double regularMesh(Vec3d p, tPlane pl);
double collimator(Vec3d p, tPlane pl);
double circleMesh(Vec3d p, tPlane pl);

class tGrid{
private:
	double (*transp)(Vec3d,tPlane);
	Vec3d r0;

public:
	tPlane plane;
	tGrid();
	tGrid(Vec3d _N,Vec3d _r0,double _lim_area,double (*_transp)(Vec3d,tPlane));

	tRay crossAndGen(tRay ray, double &t);
};

#endif /* TPLANE_HPP_ */
