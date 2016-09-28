/*
 * gridFunction.cpp
 *
 *  Created on: 31.03.2014
 *      Author: cheshire
 */

#include "tPlane.hpp"

double whiteConst=1;
double blackConst=1;

double defaultFunction(Vec3d p, tPlane pl) {
	Vec3d zDirection(0, 0, 1);
	Vec3d N = pl.N;
	double phi = acos(
			dot2(zDirection, N) / (sqrt(dot(zDirection)) * sqrt(dot(N))));

	double constW = 120e-4; //white
	double constB = 14e-4; //black

	double y = p.y;
	double z = p.z;

	double ys = y * cos(phi) - z * sin(phi);
	double zs = y * sin(phi) + z * cos(phi);

	double yd = fabs(ys - pl.r0.y) - constB / 2.0;
	double xd = fabs(p.x - pl.r0.x) - constB / 2.0;

	int ny = yd / (constB + constW);
	int nx = xd / (constB + constW);

	double bx = nx * (constB + constW) + constW;

	if (xd >= bx || xd <= 0)
		return 0;

	double by = ny * (constB + constW) + constW;
	if (yd >= by || yd <= 0)
		return 0;

	return 1;
}

double regularMesh(Vec3d p, tPlane pl) {
	Vec3d zDirection(0, 1, 0);
	Vec3d N = pl.N;
	double phi = acos(
			dot2(zDirection, N) / (sqrt(dot(zDirection)) * sqrt(dot(N))));

	Vec3d op=p - pl.r0;
	double px=op.x;
	double py=op.y;
	double pz=op.z;

	op.x=px*cos(phi) - py*sin(phi);
	op.y=px*sin(phi) - py*cos(phi);

	px=op.x;
	py=op.y;

	px=fabs(px);
	pz=fabs(pz);

	if (px<whiteConst/2.0 && pz<whiteConst/2.0) return 1;

	px-=whiteConst/2.0;
	pz-=whiteConst/2.0;

	long Nx=px/(whiteConst+blackConst);
	long Ny=pz/(whiteConst+blackConst);

	double npx=Nx*(whiteConst+blackConst)+blackConst;
	double npz=Ny*(whiteConst+blackConst)+blackConst;

	if (px>npx && pz>npz)
		return 1;

	if ((px<0 && pz>npz) || (pz<0 && px>npx))
		return 1;

	return 0;
}

double circleMesh(Vec3d p, tPlane pl) {

	double dist=sqrt(dot(p-pl.r0));

	if (dist<15e-4)
		return 1;

	return 0;

}

double collimator(Vec3d p, tPlane pl) {
	Vec3d zDirection(0, 1, 0);
	Vec3d N = pl.N;
	double phi = acos(
			dot2(zDirection, N) / (sqrt(dot(zDirection)) * sqrt(dot(N))));

	Vec3d op=p - pl.r0;
	double px=op.x;
	double py=op.y;
	double pz=op.z;

	op.x=px*cos(-phi) - py*sin(-phi);
	op.y=px*sin(-phi) - py*cos(-phi);

	px=op.x;
	py=op.y;

	extern double whiteConst;
	extern double blackConst;
	double wight=whiteConst;
	double height=blackConst;
	px=fabs(px);
	pz=fabs(pz);

	if (px<wight/2.0 && pz<height/2.0)
		return 1;

	return 0;
}
