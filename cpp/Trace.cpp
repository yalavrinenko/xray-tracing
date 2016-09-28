/*
 * Trace.cpp
 *
 *  Created on: 16.02.2014
 *      Author: cheshire
 */

#include "Trace.hpp"
#include "general.hpp"
#include "InputOutput.hpp"

#include <vector>
#include <math.h>

using namespace std;
bool writeRoad=false;

void rayTrace(tRay* ray, int ray_count, std::vector<tObject> &object,
		int obj_count) {
	bool flag = true;

	long proccessed=0;

	vector<vector<Vec3d> > road;
	if (writeRoad)
		road.resize(ray_count);

	vector<Vec3d> toRet;

	while (flag) {
		flag = false;

		for (int i = 0; i < ray_count; i++) {
			tRay r = ray[i];

			if (r.lambda <= 0 || fabs(r.I) <= c_eps)
				continue;

			if (writeRoad)
				road[i].push_back(r.b);

			double t;

			double nearestObj = VERY_BIG_NUM;
			int nearestObjId = VERY_BIG_NUM;

			tRay r_out;

			for (int j = 0; j < obj_count; j++) {
				if (object[j].type == SPHERE)
					t = crossPointWithSphere(r, object[j]);

				if (object[j].type == PLANE)
					t = crossPointWithPlane(r, object[j]);

				if (object[j].type == CYLINDER)
					t = crossPointWithCylinder(r, object[j]);

				if (object[j].type == GRID)
					t = crossPointWithGrid(r, object[j]);

				if (object[j].type == DUMP_PLANE)
					t = crossPointWithDumpPlane(r, object[j]);

				if (t < nearestObj && t > 0 && fabs(t) >= c_eps) {
					nearestObj = t;
					nearestObjId = j;
				}
			}

			if (nearestObj != VERY_BIG_NUM) {
				tObject obj=object[nearestObjId];
				switch (obj.type){
				case SPHERE:
					ray[i]=crossWithSphere(r,obj,t);
					break;
				case PLANE:
					ray[i]=crossWithPlane(r,obj,t);
					break;
				case CYLINDER:
					ray[i]=crossWithCylinder(r,obj,t);
					break;
				case GRID:
					ray[i]=crossWithGrid(r,obj,t);
					break;
				case DUMP_PLANE:
					ray[i]=crossWithDumpPlane(r,obj,t);
					break;
				};

				if (ray[i].I==0)
					proccessed++;
				flag = true;
			} else {

				if (writeRoad)
					road[i].push_back(ray[i].k * 10 + ray[i].b);

				ray[i] = tRay();
				proccessed++;
			}
		}
		//printf("\r%lf",((double)proccessed/(double)ray_count)*100.0);
	}

	if (writeRoad)
		dumpRoadNOSPH("Road.m", "Road", road);
}

double crossPointWithDumpPlane(tRay ray, tObject obj) {
	tDumpPlane *tpl = (tDumpPlane*) obj.obj;
	return tpl->cross(ray);
}

double crossPointWithPlane(tRay ray, tObject obj) {
	tPlane *tpl = (tPlane*) obj.obj;
	return tpl->cross(ray);
}

double crossPointWithGrid(tRay ray, tObject obj) {
	tGrid *tgl = (tGrid*) obj.obj;
	return tgl->plane.cross(ray);
}

double crossPointWithSphere(tRay ray, tObject obj) {
	tSphere *sph = (tSphere*) obj.obj;
	return sph->cross(ray);
}

double crossPointWithCylinder(tRay ray, tObject obj) {
	tCylinder *s = (tCylinder*) obj.obj;
	return s->cross(ray);
}

tRay crossWithDumpPlane(tRay ray, tObject obj, double &t) {
	tDumpPlane *tpl = (tDumpPlane*) obj.obj;
	ray = tpl->crossAndGen(ray, t);
	return ray;
}

tRay crossWithPlane(tRay ray, tObject obj, double &t) {
	tPlane *tpl = (tPlane*) obj.obj;
	ray = tpl->crossAndGen(ray, t);
	return ray;
}

tRay crossWithGrid(tRay ray, tObject obj, double &t) {
	tGrid *tgl = (tGrid*) obj.obj;
	ray = tgl->crossAndGen(ray, t);
	return ray;
}

tRay crossWithSphere(tRay ray, tObject obj, double &t) {
	tSphere *sph = (tSphere*) obj.obj;
	ray = sph->crossAndGen(ray, t);
	return ray;
}

tRay crossWithCylinder(tRay ray, tObject obj, double &t) {
	tCylinder *s = (tCylinder*) obj.obj;
	ray = s->crossAndGen(ray, t);
	return ray;
}
