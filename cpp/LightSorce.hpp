/*
 * LightSorce.hpp
 *
 *  Created on: 09.02.2014
 *      Author: cheshire
 */

#ifndef LIGHTSORCE_HPP_
#define LIGHTSORCE_HPP_

#include "tRay.hpp"
#include "general.hpp"

class SphereLight{
private:
	double apperture;
	double R;
	double Rw,Rh;

	Vec3d r;
	Vec2d dx;
	Vec2d dy;
	Vec2d dz;

	int type;
	double H;
	int orientation;

	double breggAngle;

	tRay* generateCylindricRays(double lambda,double dlambda,int count);

public:
	Vec3d position;
	Vec3d direction;

	static const int sphereType=1;
	static const int cylindricType=2;

	static const int xOz=0;
	static const int xOy=1;
	static const int yOz=2;

public:
	SphereLight();
	SphereLight(Vec3d _position,Vec3d _direction,double _app,double _R, double bA);
	SphereLight(Vec3d _position,Vec3d _direction,double _app,double _Rw,double _Rh, double bA);

	tRay* GenerateRays(double lambda,double dlambda,int count);

	void setCylinricType(double _h,int _orientation);
	void setSphereType();
	void setBreggAngle(double bA){
		breggAngle = bA;
	}

};

class LightSorce{
private:
	double L,H;

	Vec3d position;
	Vec3d direction;

public:
	LightSorce();

	LightSorce(Vec3d _position,Vec3d _direction,double _l,double _h);

	tRay* GenerateRays(double lambda,double dlambda,int count,double R);

};

class circleLightSorce{
private:

	double R;

	Vec3d position;
	Vec3d direction;

public:
	circleLightSorce();

	circleLightSorce(Vec3d _position,Vec3d _direction,double _r);

	tRay* GenerateRays(double lambda,double dlambda,int count,double R);

};

#endif /* LIGHTSORCE_HPP_ */
