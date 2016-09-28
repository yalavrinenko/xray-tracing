/*
 * tShere.cpp
 *
 *  Created on: 16.02.2014
 *      Author: cheshire
 */

#include "tSphere.hpp"
#include <math.h>
#include <iostream>
#include "InputOutput.hpp"
#include <string>

using namespace std;

double tSphere::cross(tRay ray) {
	double A = dot(ray.k);
	double B = 2 * (ray.b.x - this->r0.x) * ray.k.x
			+ 2 * (ray.b.y - this->r0.y) * ray.k.y
			+ 2 * (ray.b.z - this->r0.z) * ray.k.z;
	double C = dot(ray.b - this->r0)
			- this->RadThetaPhi.x * this->RadThetaPhi.x;
	//At*t+B*t+C=0

	double D = B * B - 4 * A * C;
	if (D < 0)
		return -1;
	double t1 = (-B + sqrt(D)) / (2 * A);
	double t2 = (-B - sqrt(D)) / (2 * A);

	if (checkPosition(ray.trace(t1)))
		return t1;

	if (checkPosition(ray.trace(t2)))
		return t2;

	return -1;
}

void tSphere::createDumpFile(char* fileName) {
	fout = fopen(fileName, "w+");

	if (fout == NULL)
		printf("Error fopen\n");

	outArr.resize(rayLimit);
	rayIter = 0;

	fprintf(fout, "#[MIRROR Sphere]\n");

	fprintf(fout, "#[R]\t=\t%0.10f\n", RadThetaPhi.x);

	fprintf(fout, "#[X,Y,Z]\t=\t[%0.10f, %0.10f, %0.10f];\n", r0.x, r0.y,
			r0.z);

	fprintf(fout, "#[Phi,Theta]\t=\t[%0.10f, %0.10f];\n", RadThetaPhi.z,
			RadThetaPhi.y);

	fprintf(fout, "#[dPhi,dTheta]\t=\t[%0.10f, %0.10f];\n",
			deltaRadThetaPhi.z, deltaRadThetaPhi.y);
	fprintf(fout,
			"#------------------------------------------------------------\n");
}
void tSphere::addDumpData(Vec3d crossPoint, double I, double l) {
	outArr[rayIter].point = crossPoint;
	outArr[rayIter].I = I;
	outArr[rayIter].l = l;
	rayIter++;
	if (rayIter >= rayLimit) {

		for (int i = 0; i < rayIter; i++) {
			fprintf(fout, "%0.10f\t%0.10f\t%0.10f\t%0.10f\t%0.10f\n",
					outArr[i].point.x, outArr[i].point.y, outArr[i].point.z,
					outArr[i].l, outArr[i].I);
		}
		rayIter = 0;
	}
}

void tSphere::initRayCounter(){
	rayCatch=rayReflected=0;
}

long long int tSphere::getReflRayCount(){
	return this->rayReflected;
}

long long int tSphere::getCatchRayCount(){
	return this->rayCatch;
}

tRay tSphere::crossAndGen(tRay ray, double &t) {
	double crossPoint = this->cross(ray);
	t = crossPoint;
	if (crossPoint == -1)
		return tRay(Vec3d(0, 0, 0), Vec3d(0, 0, 0), -1);

	//Ray catch

	rayCatch++;

	Vec3d point = ray.trace(crossPoint);

	Vec3d N = (point - this->r0) / this->RadThetaPhi.x;

	Vec3d direction = ray.k / sqrt(dot(ray.k));

	double cosA = dot2(N, direction) / (sqrt(dot(N)) * sqrt(dot(direction)));

	if (cosA > 1)
		cosA = 1;

	double angle = acos(cosA);

	//--------------------------------------------------
	double Intence = 2;

	Intence = distrf(angle, ray.lambda);

	if (Intence != 0 && mirrorDumpFileName != "")
		addDumpData(point, Intence, ray.lambda);

	if (Intence==2) //reflected ray
		rayReflected++;
	//--------------------------------------------------

	Vec3d dirNormal = N * dot2(direction, N);
	Vec3d dirTangential = direction - dirNormal;

	Vec3d newDirection = dirTangential - dirNormal;

	tRay toRet(point, newDirection, ray.lambda);
	toRet.I = Intence;
	return toRet;
}

tSphere::~tSphere() {
	for (int i = 0; i < rayIter; i++)
		fprintf(fout, "%0.10f\t%0.10f\t%0.10f\t%0.10f\t%0.10f\n",
				outArr[i].point.x, outArr[i].point.y, outArr[i].point.z,
				outArr[i].l, outArr[i].I);
	fclose(fout);
}

bool tSphere::checkPosition(Vec3d point) {
	Vec3d R = point - this->r0;
	double r = sqrt(dot(R));
	double eps = 1e-10;
	if (r - eps >= this->RadThetaPhi.x && this->RadThetaPhi.x >= r + eps)
		return false;
	double Theta = acos(R.z / r);
	double Phi = acos(R.x / (r * sin(Theta)));

	if (isnan(Phi))
		Phi = M_PI;

	if (R.y < 0)
		Phi = M_PI + Phi;

	Theta = RadToGrad(Theta);
	Phi = RadToGrad(Phi);

	if (fabs(RadThetaPhi.y - Theta) <= deltaRadThetaPhi.y
			&& fabs(RadThetaPhi.z - Phi) <= deltaRadThetaPhi.z) {
		return true;
	} else {
		return false;
	}

}

void tSphere::setDistrFunction(double (*_distrf)(double Theta,double lambda)){
	p_distrf=_distrf;
	parameters = NULL;
}

void tSphere::setDistrFunction(tParameters *p){
	p_distrf=NULL;
	parameters = p;
}

double tSphere::distrf(double Theta, double lambda) {
	if (parameters != NULL)
		return parameters->distr(Theta, lambda);
	if (p_distrf != NULL)
		return p_distrf(Theta, lambda);
}
