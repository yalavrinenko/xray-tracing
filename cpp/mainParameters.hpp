/*
 * mainParameters.hpp
 *
 *  Created on: 14.04.2014
 *      Author: cheshire
 */

#ifndef MAINPARAMETERS_HPP_
#define MAINPARAMETERS_HPP_

#include <fstream>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <omp.h>
#include <string.h>
#include <string>
#include <vector>
#include "general.hpp"

using namespace std;

#define MIRROR_SPHERE  0
#define MIRROR_CYLINDER 1

class infoOut;

class tParameters{

private:
	void readWaveLenghts();
	void readReflectionFunction();

public:
	//GENERAL
	int rayCount = 1000000; //par
	bool isRad=true;
	int rayByIter=1e+6;

	//SRC
	double sourceDistance; //par
	double sourceSize;//par
	double sourceSize_W;
	double sourceSize_H;
	double aperture; //par
	int src_type; //SphereLight::cylindricType; //par
	double H; //par
	int orientation; //par

	int waveLenghtCount;
	vector<waveInput> waveLenghts;

	//MIRROR
	double mirrorR;//par
	double mirrorTheta = 90; //par
	double mirrorPsi = 90; //par
	double dmTh, dmPsi; //p
	double breggAngle; //60 //par
	double programAngle;
	double dprogramAngle;//0.0175; //par
	string mirrorDumpFileName;
	double refValue;
	vector<double> reflectionFunction;
	string reflectionFileName;
	double reflStep;
	int reflSize;
	double crystal2d;
	int mirrorType;


	//OUTPUT
	string dumpPlaneName; //par
	double startPoint = 0;//43.86;
	double planeCount = 0;
	double planeStep = 0;

	double planeSize = 0;
	double planeSizeW = 0;
	double planeSizeH = 0;

	string logFileName;
	string dumpDirection;

	//OBJECT_PLANE
	double objStartPoint;
	double objPlaneCount;
	double objPlaneStep;
	double objPlaneSize;
	double objPlaneSizeH;
	double objPlaneSizeW;

	//OBJECT
	double gridPos;
	double gridSize;
	string gridType;
	double whiteConst;
	double blackConst;

	tParameters(){
	}
	tParameters(char* initFileName){
		this->init(initFileName);
	}
	void init(char* initFileName);
	double distr(double phi, double lambda);
	void logVariable(infoOut &logger);
};

bool TestDir(string dir);

#endif /* MAINPARAMETERS_HPP_ */
