/*
 * mainParameters.cpp
 *
 *  Created on: 31 мая 2016 г.
 *      Author: cheshire
 */
#include "mainParameters.hpp"
#include <math.h>
#include "general.hpp"
#include "param.h"
#include "LightSorce.hpp"
#include "InputOutput.hpp"
#include "InputOutput.hpp"

bool TestDir(string dir) {
	DIR *d = opendir(dir.c_str());
	if (d == NULL)
		return false;
	else
		return true;
}

void tParameters::logVariable(infoOut &logger){
	logger.logText("[VAR SECTION]------------------------------------\n");

	logger.logText("rayCount = " + to_string(rayCount) );
	logger.logText("isRad = " + to_string(isRad) );
	logger.logText("rayByIter = " + to_string(rayByIter) );
	logger.logText("sourceDistance = " + to_string(sourceDistance) );
	logger.logText("sourceSize = " + to_string(sourceSize) );
	logger.logText("sourceSize_W = " + to_string(sourceSize_W) );
	logger.logText("sourceSize_H = " + to_string(sourceSize_H) );
	logger.logText("aperture = " + to_string(aperture) );
	logger.logText("src_type = " + to_string(src_type) );
	logger.logText("H = " + to_string(H) );
	logger.logText("orientation = " + to_string(orientation) );
	logger.logText("waveLenghtCount = " + to_string(waveLenghtCount) );

	logger.logText("WaveLenghts:");

	for (auto i : waveLenghts){
		logger.logText("\t\t"+to_string(i.waveLenght)+"+-"+to_string(i.dwaveLenght)+".\tI = "+to_string(i.intensity));
	}

	logger.logText("mirrorR = " + to_string(mirrorR) );
	logger.logText("mirrorTheta = " + to_string(mirrorTheta) );
	logger.logText("mirrorPsi = " + to_string(mirrorPsi) );
	logger.logText("dmTh = " + to_string(dmTh) );
	logger.logText("dmPsi = " + to_string(dmPsi) );
	logger.logText("breggAngle = " + to_string(breggAngle) );
	logger.logText("programAngle = " + to_string(programAngle) );
	logger.logText("dprogramAngle = " + to_string(dprogramAngle) );
	logger.logText("mirrorDumpFileName = " + mirrorDumpFileName );
	logger.logText("refValue = " + to_string(refValue) );

	logger.logText("reflectionFileName = " + reflectionFileName );
	logger.logText("reflStep = " + to_string(reflStep) );
	logger.logText("reflSize = " + to_string(reflSize) );
	logger.logText("crystal2d = " + to_string(crystal2d) );
	logger.logText("mirrorType = " + to_string(mirrorType) );
	logger.logText("dumpPlaneName = " + dumpPlaneName );
	logger.logText("startPoint = " + to_string(startPoint) );
	logger.logText("planeCount = " + to_string(planeCount) );
	logger.logText("planeStep = " + to_string(planeStep) );
	logger.logText("planeSize = " + to_string(planeSize) );
	logger.logText("planeSizeW = " + to_string(planeSizeW) );
	logger.logText("planeSizeH = " + to_string(planeSizeH) );
	logger.logText("logFileName = " + logFileName );
	logger.logText("dumpDirection = " + dumpDirection );
	logger.logText("objStartPoint = " + to_string(objStartPoint) );
	logger.logText("objPlaneCount = " + to_string(objPlaneCount) );
	logger.logText("objPlaneStep = " + to_string(objPlaneStep) );
	logger.logText("objPlaneSize = " + to_string(objPlaneSize) );
	logger.logText("objPlaneSizeH = " + to_string(objPlaneSizeH) );
	logger.logText("objPlaneSizeW = " + to_string(objPlaneSizeW) );
	logger.logText("gridPos = " + to_string(gridPos) );
	logger.logText("gridSize = " + to_string(gridSize) );
	logger.logText("gridType = " + gridType );
	logger.logText("whiteConst = " + to_string(whiteConst) );
	logger.logText("blackConst = " + to_string(blackConst) );

	logger.logText("[END VAR SECTION]------------------------------------\n\n");
}

void tParameters::readReflectionFunction() {
	reflectionFunction.resize(reflSize);
	ifstream reflStream(reflectionFileName.c_str());
	for (int i = 0; i < reflSize; i++) {
		double shr;
		reflStream>>shr>>reflectionFunction[i];
	}
}

double tParameters::distr(double phi, double lambda) {
	double dPhi;
	int k;
	int k1;
	double currentRefValue;
	double randomNumber;

	if (reflectionFileName == "") {
		double newProgramAngle = M_PI / 2 - asin(lambda / crystal2d);
		if (isnan(newProgramAngle))
			newProgramAngle=40*M_PI;


		if (fabs(phi - newProgramAngle) <= dprogramAngle) {
			randomNumber = (double) (rand() % 100) / 100.0;
			if (randomNumber <= refValue)
				return 2;
			else
				return 0;
		} else
			return 0;

	} else {

		double newProgramAngle = M_PI / 2 - asin(lambda / crystal2d);

		if (isnan(newProgramAngle))
			newProgramAngle=40*M_PI;

		if (fabs(phi - newProgramAngle) <= dprogramAngle) {
			dPhi = (phi - newProgramAngle) + dprogramAngle;

			k = dPhi / reflStep;
			k1 = k + 1;

			if (k1>=reflSize) k1=k;
			currentRefValue = reflectionFunction[k]
					+ (reflectionFunction[k1] - reflectionFunction[k])
							/ reflStep * (dPhi - k * reflStep);

			if (currentRefValue>1 || currentRefValue<0.0)
				cout<<"Reflection interpolation error!"<<endl;

			randomNumber = (double) (rand() % 100) / 100.0;

			if (randomNumber <= currentRefValue)
				return 2;
			else
				return 0;
		} else
			return 0;

	}
}

void tParameters::readWaveLenghts() {
	string wavePattern;
	string dwavePattern;
	string iwavePattern;

	int waveId = 1;

	double dwave = -1;

	waveLenghts.resize(waveLenghtCount);
	for (int i = 0; i < waveLenghtCount; i++) {
		wavePattern = "SRC.wave" + intToStr(i + 1);
		dwavePattern = "SRC.dwave" + intToStr(i + 1);
		iwavePattern = "SRC.iwave" + intToStr(i + 1);

		waveLenghts[i].waveLenght = GetDblPar((char*) wavePattern.c_str());
		waveLenghts[i].intensity = (ExistsPar((char*) iwavePattern.c_str())) ?
							GetDblPar((char*) iwavePattern.c_str()) : 1.0;

		if (dwave == -1) {
			dwave = GetDblPar((char*) dwavePattern.c_str());
			waveLenghts[i].dwaveLenght = dwave;
		} else
			waveLenghts[i].dwaveLenght =
					(ExistsPar((char*) dwavePattern.c_str())) ?
							GetDblPar((char*) dwavePattern.c_str()) : dwave;
	}
	//Remove zero lines
	int realIdx = 0;
	for (int i=0; i<waveLenghtCount; i++){
		if (waveLenghts[i].waveLenght != 0){
			waveLenghts[realIdx] = waveLenghts[i]; 
			realIdx ++;
		}
	}
	waveLenghtCount = realIdx;
}

void tParameters::init(char* initFileName) {
	ParseFile(initFileName);

	char* tmp = new char[1024];

	//GENERAL
	rayCount = (ExistsPar(".rayCount")) ? GetIntPar(".rayCount") : 1000;

	rayByIter = (ExistsPar(".rayByIter")) ? GetIntPar(".rayByIter") : 1000000;

	if (ExistsPar(".angleType")) {
		GetStrPar(".angleType", tmp);
		if (!strcmp(tmp, "dec"))
			isRad = false;
		else if (!strcmp(tmp, "rad"))
			isRad = true;
	}

	//SRC
	sourceDistance =
			(ExistsPar("SRC.Distance")) ? GetDblPar("SRC.Distance") : 10.0;

	sourceSize = (ExistsPar("SRC.Size")) ? GetDblPar("SRC.Size") : 1.0;
	sourceSize_W =(ExistsPar("SRC.Size_W")) ? GetDblPar("SRC.Size_W") : sourceSize;
	sourceSize_H =(ExistsPar("SRC.Size_H")) ? GetDblPar("SRC.Size_H") : sourceSize;

	aperture = (ExistsPar("SRC.Aperture")) ? GetDblPar("SRC.Aperture") : 1.0;

	if (isRad == false)
		aperture *= M_PI / 180.0;

	src_type = SphereLight::sphereType;
	if (ExistsPar("SRC.Type")) {
		GetStrPar("SRC.Type", tmp);
		if (!strcmp(tmp, "Cylinder"))
			src_type = SphereLight::cylindricType;
		else if (!strcmp(tmp, "Sphere"))
			src_type = SphereLight::sphereType;
	}

	if (src_type == SphereLight::cylindricType) {
		H = (ExistsPar("SRC.H")) ? GetDblPar("SRC.H") : 1.0;

		orientation = SphereLight::xOz;

		if (ExistsPar("SRC.Orientation")) {
			GetStrPar("SRC.Orientation", tmp);
			if (!strcmp(tmp, "xOz"))
				orientation = SphereLight::xOz;
			else if (!strcmp(tmp, "xOy"))
				orientation = SphereLight::xOy;
			else if (!strcmp(tmp, "yOz"))
				orientation = SphereLight::yOz;
		}
	} else {
		H = -1;
		orientation = -1;
	}

	waveLenghtCount =
			(ExistsPar("SRC.waveLenghtCount")) ?
					GetIntPar("SRC.waveLenghtCount") : 0;
	readWaveLenghts();

	//MIRROR

	mirrorType = MIRROR_SPHERE;

	if (ExistsPar("MIRROR.MirrorType")) {
		GetStrPar("MIRROR.MirrorType", tmp);
		if (!strcmp(tmp, "Cylinder"))
			mirrorType = MIRROR_CYLINDER;
		else if (!strcmp(tmp, "Sphere"))
			mirrorType = MIRROR_SPHERE;
	}

	mirrorR = (ExistsPar("MIRROR.R")) ? GetDblPar("MIRROR.R") : 20.0;

	mirrorTheta =
			(ExistsPar("MIRROR.Theta")) ?
					GetDblPar("MIRROR.Theta") : mirrorTheta;
	mirrorPsi = (ExistsPar("MIRROR.Psi")) ? GetDblPar("MIRROR.Psi") : mirrorPsi;
	double hA =
			(ExistsPar("MIRROR.hAperture")) ?
					GetDblPar("MIRROR.hAperture") : mirrorR / 3.0;
	double wA =
			(ExistsPar("MIRROR.wAperture")) ?
					GetDblPar("MIRROR.wAperture") : mirrorR / 3.0;


	if (mirrorType == MIRROR_SPHERE){
		dmPsi = RadToGrad(asin(wA/(2.0*mirrorR)));
		dmTh = RadToGrad(asin(hA/(2.0*mirrorR)));
	} else if (mirrorType == MIRROR_CYLINDER){
		dmPsi = RadToGrad(asin(hA/(2.0*mirrorR)));
		dmTh = wA/2.0;
	}

	breggAngle =
			(ExistsPar("MIRROR.ScatteringAngle")) ?
					GetDblPar("MIRROR.ScatteringAngle") : 20.0;
	if (!isRad)
		breggAngle *= M_PI / 180.0;

	programAngle = M_PI / 2.0 - breggAngle;
	dprogramAngle =
			(ExistsPar("MIRROR.distributionWidth")) ?
					GetDblPar("MIRROR.distributionWidth") : 0.1;
	if (!isRad)
		dprogramAngle *= M_PI / 180.0;

	if (ExistsPar("MIRROR.dumpFile")) {
		GetStrPar("MIRROR.dumpFile", tmp);
		mirrorDumpFileName = string(tmp);
	} else
		mirrorDumpFileName = "";

	refValue =
			(ExistsPar("MIRROR.refValue")) ? GetDblPar("MIRROR.refValue") : 1.0;

	if (ExistsPar("MIRROR.refFile")) {
		GetStrPar("MIRROR.refFile", tmp);
		reflectionFileName = string(tmp);
		reflSize =
				(ExistsPar("MIRROR.refSize")) ?
						GetDblPar("MIRROR.refSize") : -1;
		reflStep =
				(ExistsPar("MIRROR.refStep")) ?
						GetDblPar("MIRROR.refStep") : -1;

		if (!isRad)
			reflStep *= M_PI/180.0;


		if (reflSize != -1 && reflStep!= -1)
			readReflectionFunction();
	} else {
		reflectionFileName = "";
		reflSize = -1;
		reflStep = -1;
	}

	crystal2d = (ExistsPar("MIRROR.Crystal2D")) ? GetDblPar("MIRROR.Crystal2D") : 1;

	//OUTPUT
	if (ExistsPar("OUTPUT.dumpPlaneNamePattern")) {
		GetStrPar("OUTPUT.dumpPlaneNamePattern", tmp);
		dumpPlaneName = string(tmp);
	} else
		dumpPlaneName = "";

	if (!TestDir(dumpPlaneName)) {
		string commang;
#ifdef WIN32
		commang = "md " + dumpPlaneName;
		cout << commang << endl;
		for (int i=0; i<commang.length(); i++)
			if (commang[i] == '/')
				commang[i] = '\\';
#else
		commang = "mkdir -v -p " + dumpPlaneName;
		for (int i=0; i<commang.length(); i++)
			if (commang[i] == '\\')
				commang[i] = '/';
#endif
		system(commang.c_str());
	}

	startPoint =
			(ExistsPar("OUTPUT.startPoint")) ?
					GetDblPar("OUTPUT.startPoint") : -1;
	planeCount = (ExistsPar("OUTPUT.Count")) ? GetDblPar("OUTPUT.Count") : -1;
	planeStep = (ExistsPar("OUTPUT.Step")) ? GetDblPar("OUTPUT.Step") : -1;
	planeSize = (ExistsPar("OUTPUT.Size")) ? GetDblPar("OUTPUT.Size") : -1;

	planeSizeH = (ExistsPar("OUTPUT.SizeH")) ? GetDblPar("OUTPUT.SizeH") : planeSize;
	planeSizeW = (ExistsPar("OUTPUT.SizeW")) ? GetDblPar("OUTPUT.SizeW") : planeSize;

	if (ExistsPar("OUTPUT.logFileName")) {
		GetStrPar("OUTPUT.logFileName", tmp);
		logFileName = string(tmp);
	} else
		logFileName = "log.txt";

	if (ExistsPar("OUTPUT.Direction")) {
		GetStrPar("OUTPUT.Direction", tmp);
		dumpDirection = string(tmp);
	} else
		dumpDirection = "Mirror";

	//OBJECT_PLANE
	objStartPoint =
			(ExistsPar("OBJECT_PLANE.startPoint")) ?
					GetDblPar("OBJECT_PLANE.startPoint") : -1;
	objPlaneCount =
			(ExistsPar("OBJECT_PLANE.Count")) ?
					GetDblPar("OBJECT_PLANE.Count") : -1;
	objPlaneStep =
			(ExistsPar("OBJECT_PLANE.Step")) ?
					GetDblPar("OBJECT_PLANE.Step") : -1;
	objPlaneSize =
			(ExistsPar("OBJECT_PLANE.Size")) ?
					GetDblPar("OBJECT_PLANE.Size") : -1;
	objPlaneSizeH =
				(ExistsPar("OBJECT_PLANE.SizeH")) ?
						GetDblPar("OBJECT_PLANE.SizeH") : objPlaneSize;

	objPlaneSizeW =
					(ExistsPar("OBJECT_PLANE.SizeW")) ?
							GetDblPar("OBJECT_PLANE.SizeW") : objPlaneSize;

	//OBJECT
	gridPos =
			(ExistsPar("OBJECT.Position")) ? GetDblPar("OBJECT.Position") : -1;
	gridSize = (ExistsPar("OBJECT.Size")) ? GetDblPar("OBJECT.Size") : -1;
	whiteConst =
			(ExistsPar("OBJECT.Prop1")) ?
					GetDblPar("OBJECT.Prop1") : -1;
	blackConst =
			(ExistsPar("OBJECT.Prop2")) ?
					GetDblPar("OBJECT.Prop2") : -1;

	if (ExistsPar("OBJECT.Type")) {
		GetStrPar("OBJECT.Type", tmp);
		gridType = string(tmp);
	} else
		gridType = "slit";


}


