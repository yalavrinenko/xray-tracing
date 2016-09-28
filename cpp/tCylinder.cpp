/*
 * tCylinder.cpp
 *
 *  Created on: 21.02.2014
 *      Author: cheshire
 */

#include "tCylinder.hpp"
#include <math.h>
#include <iostream>

using namespace std;

tCylinder::~tCylinder(){
	for (int i = 0; i < rayIter; i++)
		fprintf(fout, "%0.10f\t%0.10f\t%0.10f\t%0.10f\t%0.10f\n",
				outArr[i].point.x, outArr[i].point.y, outArr[i].point.z,
				outArr[i].l, outArr[i].I);
	fclose(fout);
}

tCylinder::tCylinder() {
	r0 = Vec3d();
	R = dZ = dPhi = 0;

	mirrorDumpFileName="";

	if (mirrorDumpFileName != "")
		createDumpFile((char*)mirrorDumpFileName.c_str());
}

tCylinder::tCylinder(Vec3d _r0, Vec3d _RadThetaPhi, Vec3d _delta, std::string mdpName) {
	r0 = _r0;
	R = _RadThetaPhi.x;
	dZ = _delta.y;
	dPhi = _delta.z;

	mirrorDumpFileName = mdpName;

	if (mirrorDumpFileName != "")
		createDumpFile((char*)mirrorDumpFileName.c_str());
}

double tCylinder::cross(tRay ray) {
/*
 * 	double cx = ray.b.x - r0.x;
	double cy = ray.b.y - r0.y;

	double A = ray.k.x * ray.k.x + ray.k.y * ray.k.y;
	double B = 2 * ray.k.x * cx + 2 * ray.k.y * cy;
	double C = cx * cx + cy * cy - R * R;

	double D = B * B - 4 * A * C;
	if (D < 0)
		return -1;

	double t1 = (-B + sqrt(D)) / (2 * A);
	double t2 = (-B - sqrt(D)) / (2 * A);

	double t=-1;

	if (t1>0 && checkPoint(t1, ray))
		t=t1;

	if (t2>0 && checkPoint(t2, ray)){
		if (t==-1 || dot(ray.trace(t1)) > dot(ray.trace(t2)))
			t=t2;
	}

	return t;
 */

	double cz = ray.b.z - r0.z;
	double cy = ray.b.y - r0.y;

	double A = ray.k.z * ray.k.z + ray.k.y * ray.k.y;
	double B = 2 * ray.k.z * cz + 2 * ray.k.y * cy;
	double C = cz * cz + cy * cy - R * R;

	double D = B * B - 4 * A * C;
	if (D < 0)
		return -1;

	double t1 = (-B + sqrt(D)) / (2 * A);
	double t2 = (-B - sqrt(D)) / (2 * A);

	double t=-1;

	if (t1>0 && checkPoint(t1, ray))
		t=t1;

	if (t2>0 && checkPoint(t2, ray)){
		if (t==-1 || dot(ray.trace(t1)) > dot(ray.trace(t2)))
			t=t2;
	}

	return t;
}

bool tCylinder::checkPoint(double t, tRay r) {
/*
 * 	Vec3d point = r.trace(t);

	if (fabs(point.z) > dZ)
		return false;

	double phi = asin(point.x / R);

	Vec3d hpoint = point - this->r0;

	if (hpoint.y < 0)
		return false;

	if (RadToGrad( fabs(phi) ) > dPhi)
		return false;

	return true;
 */

	Vec3d point = r.trace(t);

	if (fabs(point.x) > dZ)
		return false;

	double phi = asin(point.z / R);

	Vec3d hpoint = point - this->r0;

	if (hpoint.y < 0)
		return false;

	if (RadToGrad( fabs(phi) ) > dPhi)
		return false;

	return true;
}

tRay tCylinder::crossAndGen(tRay ray, double &t) {
	t = this->cross(ray);
	if (t <= 0) {
		t = -1;
		return tRay(Vec3d(0, 0, 0), Vec3d(0, 0, 0), -1);
	}

	rayCatch++;

	Vec3d point = ray.trace(t);

	Vec3d h_point = r0 - point;
	Vec3d ppoint = ray.k / sqrt(dot(ray.k));
	Vec3d N = h_point;
	N.x=0;
	N = N / sqrt(dot(N));

	double angle =M_PI - acos(dot2(N, ppoint) / (sqrt(dot(N)) * sqrt(dot(ppoint))));

	//--------------------------------------------------
	double Intence = 2;

	Intence = distrf(angle, ray.lambda);

	if (Intence != 0 && mirrorDumpFileName != "")
		addDumpData(point, Intence, ray.lambda);

	if (Intence==2) //reflected ray
		rayReflected++;

	//--------------------------------------------------

	Vec3d direction = ray.k / sqrt(dot(ray.k));

	Vec3d dirNormal = N * dot2(direction, N);
	Vec3d dirTangential = direction - dirNormal;

	/*cout << "(Dx,Dy,Dz):(" << direction.x << "," << direction.y << ","
			<< direction.z << ")" << endl;
	cout << "(Dnx,Dny,Dnz):(" << dirNormal.x << "," << dirNormal.y << ","
			<< dirNormal.z << ")" << endl;
	cout << "(Dtx,Dty,Dtz):(" << dirTangential.x << "," << dirTangential.y
			<< "," << dirTangential.z << ")" << endl;*/

	Vec3d newDirection = dirTangential - dirNormal;

	tRay toRet(point, newDirection, ray.lambda);
	toRet.I=Intence;
	return toRet;
}

void tCylinder::setDistrFunction(double (*_distrf)(double Theta,double lambda)){
	p_distrf=_distrf;
	parameters = NULL;
}

void tCylinder::setDistrFunction(tParameters *p){
	p_distrf=NULL;
	parameters = p;
}

void tCylinder::initRayCounter(){
	rayCatch=rayReflected=0;
}

long long int tCylinder::getReflRayCount(){
	return this->rayReflected;
}

long long int tCylinder::getCatchRayCount(){
	return this->rayCatch;
}

void tCylinder::createDumpFile(char* fileName) {
	fout = fopen(fileName, "w+");

	if (fout == NULL)
		printf("Error fopen\n");

	outArr.resize(rayLimit);
	rayIter = 0;

	fprintf(fout, "#[MIRROR Cylinder]\n");

	fprintf(fout, "#[R]\t=\t%0.10f\n", R);

	fprintf(fout, "#[X,Y,Z]\t=\t[%0.10f, %0.10f, %0.10f];\n", r0.x, r0.y,
			r0.z);

	fprintf(fout, "#[Phi,Theta]\t=\t[%0.10f, %0.10f];\n", dZ,
			dPhi);

	fprintf(fout, "#[dPhi,dTheta]\t=\t[%0.10f, %0.10f];\n",
			dZ, dPhi);
	fprintf(fout,
			"#------------------------------------------------------------\n");
}
void tCylinder::addDumpData(Vec3d crossPoint, double I, double l) {
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

double tCylinder::distrf(double Theta, double lambda) {
	if (parameters != NULL)
		return parameters->distr(Theta, lambda);
	if (p_distrf != NULL)
		return p_distrf(Theta, lambda);
}

