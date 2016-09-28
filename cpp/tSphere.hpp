/*
 * tSphere.hpp
 *
 *  Created on: 16.02.2014
 *      Author: cheshire
 */

#ifndef TSPHERE_HPP_
#define TSPHERE_HPP_

#include "tRay.hpp"
#include <vector>
#include <string>
#include "mainParameters.hpp"

class tSphere {
private:

	struct outVec{
		Vec3d point;
		double I,l;
	};

	Vec3d RadThetaPhi;
	Vec3d deltaRadThetaPhi;
	//x - Radius
	//y - Theta
	//z - Phi

	long long int rayCatch;
	long long int rayReflected;

	bool checkPosition(Vec3d point);
	std::string mirrorDumpFileName;

	double (*p_distrf)(double Theta,double lambde);
	tParameters *parameters;

	std::vector<outVec> outArr;

	static const int rayLimit=1000;
	int rayIter;

	double distrf(double Theta, double lambda);

public:
	Vec3d r0;
	FILE *fout;
	void createDumpFile(char* fileName);
	void addDumpData(Vec3d crossPoint,double I,double l);
	void setMirrorDumpFileName(std::string mdfName){
		mirrorDumpFileName = mdfName;
	}

public:
	tSphere() :
			r0(), RadThetaPhi() {
		mirrorDumpFileName = "";

		if (mirrorDumpFileName != "")
			createDumpFile((char*)mirrorDumpFileName.c_str());
	}
	tSphere(Vec3d _r0, Vec3d _RadThetaPhi, Vec3d _delta, std::string mdfName) :
			r0(_r0), RadThetaPhi(_RadThetaPhi), deltaRadThetaPhi(_delta), mirrorDumpFileName(mdfName) {

		if (mirrorDumpFileName != "")
			createDumpFile((char*)mirrorDumpFileName.c_str());
	}

	void initRayCounter();
	long long int getReflRayCount();
	long long int getCatchRayCount();

	double cross(tRay ray);
	tRay crossAndGen(tRay ray,double &t);

	void setDistrFunction(double (*_distrf)(double Theta,double lambda));
	void setDistrFunction(tParameters *p);

	~tSphere();

};

#endif /* TSPHERE_HPP_ */
