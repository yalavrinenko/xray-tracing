/*
 * tCylinder.hpp
 *
 *  Created on: 21.02.2014
 *      Author: cheshire
 */

#ifndef TCYLINDER_HPP_
#define TCYLINDER_HPP_

#include "tRay.hpp"
#include "general.hpp"
#include "tPlane.hpp"
#include "mainParameters.hpp"

class tCylinder{
private:
	struct outVec{
		Vec3d point;
		double I,l;
	};

	double R;
	double dZ; //center At y = y0
	double dPhi;  //center At Phi = 90;

	bool checkPoint(double t, tRay r);

	double (*p_distrf)(double Theta,double lambda);
	tParameters *parameters;

	long long int rayCatch;
	long long int rayReflected;
	static const int rayLimit=1000;

	std::string mirrorDumpFileName;
	std::vector<outVec> outArr;
	int rayIter;

	double distrf(double Theta, double lambda);

public:

	Vec3d r0;

	FILE *fout;
	void createDumpFile(char* fileName);
	void addDumpData(Vec3d crossPoint,double I,double l);

	tCylinder();
	tCylinder(Vec3d _r0, Vec3d _RadThetaPhi, Vec3d _delta, std::string mdfName);

	double cross(tRay ray);
	tRay crossAndGen(tRay ray,double &t);
	void setDistrFunction(double (*_distrf)(double Theta,double lambda));

	void setDistrFunction(tParameters *p);

	void initRayCounter();
	long long int getReflRayCount();
	long long int getCatchRayCount();

	double setMirrorDumpFileName(std::string name){
		this->mirrorDumpFileName=name;
	}

	~tCylinder();
};


#endif /* TCYLINDER_HPP_ */
