/*
 * tMirror.hpp
 *
 *  Created on: 24.05.2015
 *      Author: cheshire
 */

#ifndef TMIRROR_HPP_
#define TMIRROR_HPP_

#define MIRROR_SPHERE  0
#define MIRROR_CYLINDER 1

#include "tSphere.hpp"
#include "tCylinder.hpp"

class tMirror{
private:
	tSphere *sm;
	tCylinder *cm;
	int type;
public:
	tMirror(){
		type=0;
	}
	tMirror(int t):type(t){
	}

	tMirror(int t, void* m){
		type=t;
		if (type==MIRROR_SPHERE)
			sm=(tSphere*)m;
		if (type == MIRROR_CYLINDER)
			cm=(tCylinder*)m;
	}

	void setSphereMirror(tSphere* mirror){
		sm=mirror;
	}

	void setCylinderMirror(tCylinder* mirror){
		cm=mirror;
	}

	Vec3d getR0(){
		if (type==MIRROR_SPHERE)
			return sm->r0;
		if (type == MIRROR_CYLINDER)
			return cm->r0;
	}

	tSphere* getSphere(){
		return sm;
	}

	tCylinder* getCylinder(){
		return cm;
	}

	void initRayCounter(){
		if (type==MIRROR_SPHERE)
			return sm->initRayCounter();
		if (type == MIRROR_CYLINDER)
			return cm->initRayCounter();
	}

	long long int getCatchRayCount(){
		if (type==MIRROR_SPHERE)
			return sm->getCatchRayCount();
		if (type == MIRROR_CYLINDER)
			return cm->getCatchRayCount();
	}

	long long int getReflRayCount(){
		if (type==MIRROR_SPHERE)
			return sm->getReflRayCount();
		if (type == MIRROR_CYLINDER)
			return cm->getReflRayCount();
	}
};



#endif /* TMIRROR_HPP_ */
