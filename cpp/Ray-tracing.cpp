/*
 * main.cpp
 *
 *  Created on: 08.02.2014
 *      Author: cheshire
 */

#include "tSphere.hpp"
#include "tCylinder.hpp"
#include "mainParameters.hpp"
#include "LightSorce.hpp"
#include "Trace.hpp"
#include "InputOutput.hpp"
#include "tMirror.hpp"
#include "Ray-tracing.hpp"

#define DEBUGMSG(A) cout << A << endl

#ifdef NO_OMP_SUPP
#include <time.h>
double omp_get_wtime(){
	return time(0);
}
#endif

using namespace std;

string linkedLibraryOutput;
const char* plinkedLibraryOutput;
long linkedLibraryMinorOutput;
long linkedLibraryTotalOutput;
bool isTerminated;

void terminate(){
	isTerminated = true;
}

template <class mirrorClass>
tMirror *initScene(vector<tObject> &obj, SphereLight **slight, tParameters *p) {
	double mirrorYpos = p->sourceDistance * cos(p->programAngle) - p->mirrorR;
	double srcXpos = p->sourceDistance * sin(p->programAngle);

	Vec3d spos(-srcXpos, 0, 0);
	Vec3d sdir(srcXpos, mirrorYpos + p->mirrorR, 0);

	*slight = new SphereLight(spos, sdir, p->aperture, p->sourceSize_W, p->sourceSize_H, p->breggAngle);

	if (p->src_type == SphereLight::cylindricType) {
		(*slight)->setCylinricType(p->H, p->orientation);
	}

	Vec3d mpos(0, mirrorYpos, 0);
	Vec3d mprop(p->mirrorR, p->mirrorTheta, p->mirrorPsi);
	Vec3d mdprop(0, p->dmTh, p->dmPsi);

	//select mirror type
	mirrorClass *mirror = new mirrorClass(mpos, mprop, mdprop, p->mirrorDumpFileName);
	mirror->setDistrFunction(p);
	tObject toAdd;
	toAdd.type = p->mirrorType + 1;
	toAdd.obj = mirror;

	obj.push_back(toAdd);

	tMirror* toRet=new tMirror(p->mirrorType,mirror);
	//
	return toRet;
}

void addDumpPlanesSrcLined(vector<tObject> &obj, double startPoint, Vec3d dir,
		Vec3d srcDir, int count, double step, double sizeW,double sizeH, double angl,
		double crossPattern, tParameters *p) {
	double h = startPoint;
	for (int i = 0; i < count; i++) {
		double sx = h * sin(angl);
		double sy = h * cos(angl);
		sy = dir.y - sy;

		Vec3d r0(sx, sy, 0);
		Vec3d N = srcDir - r0;
		N = N / sqrt(dot(N));

		double nx = N.x * cos(-M_PI / 2) - N.y * sin(-M_PI / 2);
		double ny = N.x * sin(-M_PI / 2) + N.y * cos(-M_PI / 2);
		N.x = nx;
		N.y = ny;

		string name = p->dumpPlaneName + doubleToStr(h) + ".dmp";

		tDumpPlane *dp = new tDumpPlane(N, r0, sizeW,sizeH, (char*) name.c_str());
		dp->setCrossPattern(crossPattern);
		tObject tAdd;
		tAdd.obj = dp;
		tAdd.type = DUMP_PLANE;
		obj.push_back(tAdd);
		h += step;
	}
}

void addDumpPlanes(vector<tObject> &obj, double startPoint, Vec3d dir,
		int count, double step, double sizeW,double sizeH, double angl, double crossPattern, tParameters *p) {
	double h = startPoint;
	for (int i = 0; i < count; i++) {
		double sx = h * sin(angl);
		double sy = h * cos(angl);
		sy = dir.y - sy;

		Vec3d r0(sx, sy, 0);
		Vec3d N(sx, sy, 0);
		N = dir - N;
		N = N / sqrt(dot(N));

		string name = p->dumpPlaneName + doubleToStr(h) + ".dmp";

		tDumpPlane *dp = new tDumpPlane(N, r0, sizeW,sizeH, (char*) name.c_str());
		dp->setCrossPattern(crossPattern);
		tObject tAdd;
		tAdd.obj = dp;
		tAdd.type = DUMP_PLANE;
		obj.push_back(tAdd);
		h += step;
	}
}

void addGrid(vector<tObject> &obj, Vec3d mirrorPos, double gridPosition,
		double size, double angl, tParameters *p) {
	double x, y;
	x = -gridPosition * sin(angl);
	y = (mirrorPos.y + p->mirrorR) - gridPosition * cos(angl);

	Vec3d posGrid(x, y, 0);
	mirrorPos.y += p->mirrorR;
	Vec3d direction = mirrorPos - posGrid;

	direction = direction / sqrt(dot(direction));

	tGrid *g = new tGrid(direction, posGrid, size, regularMesh);

	tObject og;
	og.type = GRID;
	og.obj = g;

	obj.push_back(og);
}

void addCollimator(vector<tObject> &obj, Vec3d mirrorPos, double gridPosition,
		double size, double angl, tParameters *p) {
	double x, y;
	x = -gridPosition * sin(angl);
	y = (mirrorPos.y + p->mirrorR) - gridPosition * cos(angl);

	Vec3d posGrid(x, y, 0);
	mirrorPos.y += p->mirrorR;
	Vec3d direction = mirrorPos - posGrid;

	direction = direction / sqrt(dot(direction));

	tGrid *g = new tGrid(direction, posGrid, size, collimator);

	tObject og;
	og.type = GRID;
	og.obj = g;

	obj.push_back(og);
}

int RayTracing(int argc, char* argv) {
	
	#ifdef WIN32
		std::setlocale(LC_ALL, "C");
	#endif

	isTerminated = false;
	
	vector<tObject> obj;
	SphereLight *light;
	tParameters *p;

	double startTime=omp_get_wtime();

	if (argc < 1) {
		printf("No input file!\n");
		return 1;
	} else
		p=new tParameters(argv);

	tMirror* mirror;
	if (p->mirrorType == MIRROR_SPHERE)
		mirror = initScene<tSphere>(obj,&light, p);
	if (p->mirrorType == MIRROR_CYLINDER)
		mirror = initScene<tCylinder>(obj,&light, p);


	Vec3d dumpPlaneStart = mirror->getR0();

	dumpPlaneStart.y += p->mirrorR;

	if (p->planeCount > 0) {
		if (p->dumpDirection == "Mirror")
			addDumpPlanes(obj, p->startPoint, dumpPlaneStart, p->planeCount,
					p->planeStep, p->planeSizeW,p->planeSizeH, p->programAngle, tDumpPlane::IMAGE, p);
		else
			addDumpPlanesSrcLined(obj, p->startPoint, dumpPlaneStart,
					light->position, p->planeCount, p->planeStep, p->planeSizeW, p->planeSizeH,
					p->programAngle, tDumpPlane::IMAGE, p);
	}

	if (p->objPlaneCount > 0)
		addDumpPlanes(obj, p->objStartPoint, dumpPlaneStart, p->objPlaneCount,
				p->objPlaneStep, p->objPlaneSizeW,p->objPlaneSizeH, -p->programAngle, tDumpPlane::OBJECT, p);

	if (p->gridPos > 0){
		if (p->gridType == "mesh")
			addGrid(obj, mirror->getR0(), p->gridPos, p->gridSize, p->programAngle, p);
		if (p->gridType == "slit")
			addCollimator(obj, mirror->getR0(), p->gridPos, p->gridSize, p->programAngle, p);
	}

	infoOut log((char*) p->logFileName.c_str());

	log.logText("Input File Name = "+string(argv)+"\n");
	p->logVariable(log);
	log.logText("Rmirror\t=\t" + doubleToStr(p->mirrorR));

	if (p->mirrorType == MIRROR_SPHERE)
		log.logScene(mirror->getSphere(), light);
	else if (p->mirrorType == MIRROR_CYLINDER)
		log.logScene(mirror->getCylinder(), light);

	log.logText("Size\t=\t" + doubleToStr(p->sourceSize));
	log.logText("Dist\t=\t" + doubleToStr(p->sourceDistance));

	if (p->waveLenghtCount <= 0) {
		log.logText("No wave!!!!");
		cout << "No wave!!!" << endl;
		return 1;
	}

	//

    linkedLibraryOutput = "["+to_string(0)+"/"+to_string(p->waveLenghtCount)+"]:\t";

	for (int w = 0; w < p->waveLenghtCount && !isTerminated; w++) {

		waveInput currentWaveLenght = p->waveLenghts[w];
		if (currentWaveLenght.waveLenght <= 0)
			continue;

		log.logText(
				"Set wave lenght\t=\t"
						+ doubleToStr(currentWaveLenght.waveLenght));
		log.logText(
				"Set wave dlenght\t=\t"
						+ doubleToStr(currentWaveLenght.dwaveLenght));
		log.logText(
				"Set wave intensity\t=\t"
						+ doubleToStr(currentWaveLenght.intensity));

		cout << "Wave [lenght/width/intensity]\t=\t[" << currentWaveLenght.waveLenght << "/";
		cout << currentWaveLenght.dwaveLenght << "/";
		cout << currentWaveLenght.intensity << "]"<< endl;

		int generatedRay = 0;

		mirror->initRayCounter();

		int rayByIter = p->rayByIter;
        linkedLibraryMinorOutput = 0;
        linkedLibraryTotalOutput = p->rayCount * currentWaveLenght.intensity;

		while (generatedRay < p->rayCount * currentWaveLenght.intensity && rayByIter > 0 && !isTerminated) {

			if (rayByIter > (p->rayCount * currentWaveLenght.intensity - generatedRay))
				rayByIter = p->rayCount * currentWaveLenght.intensity - generatedRay;

			tRay *ray = light->GenerateRays(currentWaveLenght.waveLenght,
					currentWaveLenght.dwaveLenght, rayByIter);
			
			if (isTerminated){
				delete[] ray;
				break;
			}

			rayTrace(ray, rayByIter, obj, obj.size());

			delete[] ray;
			generatedRay += rayByIter;
			linkedLibraryMinorOutput = generatedRay;
		}

		double rCast = generatedRay;
		double rCatch = mirror->getCatchRayCount();
		double rRefl = mirror->getReflRayCount();
		double rI = rRefl / rCatch;

		printf("===>\t[%d/%d]\t%10.6f\t%10.6f\t%10.6f\t%10.6f\t%10.6f\n",w+1,p->waveLenghtCount,currentWaveLenght.waveLenght,
				rCast,rCatch,rRefl,rI);

		linkedLibraryOutput = "[lambda gen catch relf I]=\t"
						+ doubleToStr(currentWaveLenght.waveLenght) + "\t"
						+ doubleToStr(rCast) + "\t" + doubleToStr(rCatch) + "\t"
						+ doubleToStr(rRefl) + "\t" + doubleToStr(rI);
		log.logText( linkedLibraryOutput);

		linkedLibraryOutput = "["+to_string(w+1)+"/"+to_string(p->waveLenghtCount)+"]:\t" + linkedLibraryOutput;

		plinkedLibraryOutput = linkedLibraryOutput.c_str();
		
		cout << endl;
	}
	//

	cout<<"Done!\nCleaning memory..."<<endl;
	log.logText("Done!\n Cleaning memory...");
	try{
		for (int i = 0; i < obj.size(); i++)
			if (obj[i].type == DUMP_PLANE) {
				tDumpPlane *tmp = (tDumpPlane*) obj[i].obj;
				tmp->~tDumpPlane();
			} else if (obj[i].type == SPHERE) {
				tSphere* tmp = (tSphere*) obj[i].obj;
				tmp->~tSphere();
			} else if (obj[i].type == CYLINDER) {
				tCylinder* tmp = (tCylinder*) obj[i].obj;
				tmp->~tCylinder();
			}
	}catch(...){
		cout<<"Catch something strange!"<<endl;
		log.logText("Catch something strange!");
	}

	cout<<"Clean!"<<endl;
	log.logText("Clean!");

	double finishTime= omp_get_wtime();
	cout << "Working time:"<<"\t"<<finishTime - startTime<<" sec.";
	log.logText("Working time:\t"+doubleToStr(finishTime - startTime)+" sec.");
	cout << endl << "End" << endl;
	return 0;
}

