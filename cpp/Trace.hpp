/*
 * Trace.hpp
 *
 *  Created on: 16.02.2014
 *      Author: cheshire
 */

#ifndef TRACE_HPP_
#define TRACE_HPP_
#include "tRay.hpp"
#include "tPlane.hpp"
#include "tSphere.hpp"
#include "tCylinder.hpp"
#include <vector>

int rayTrace(tRay* ray,int ray_count, tObject* object,int obj_count);
tRay crossWithPlane(tRay ray, tObject obj, double &t);
tRay crossWithSphere(tRay ray, tObject obj, double &t);
tRay crossWithCylinder(tRay ray, tObject obj, double &t);
tRay crossWithGrid(tRay ray, tObject obj, double &t);
tRay crossWithDumpPlane(tRay ray, tObject obj, double &t);
std::vector<Vec3d> rayTracePlane(tRay* ray,int ray_count, tObject* object,int obj_count,int plane_id);
void rayTrace(tRay* ray,int ray_count, std::vector<tObject> &object,int obj_count);

double crossPointWithPlane(tRay ray, tObject obj);
double crossPointWithGrid(tRay ray, tObject obj);
double crossPointWithSphere(tRay ray, tObject obj);
double crossPointWithCylinder(tRay ray, tObject obj);
double crossPointWithDumpPlane(tRay ray, tObject obj);


#endif /* TRACE_HPP_ */
