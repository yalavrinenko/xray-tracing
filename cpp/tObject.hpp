#include "tPlane.hpp"

/*
 * class tPlane{

public:
	Vec3d r0;
	Vec3d N;

	double limit_area;

public:
	tPlane();
	tPlane(Vec3d _N,Vec3d _r0);
	tPlane(Vec3d _N,Vec3d _r0,double _lim_area);

	virtual double cross(tRay ray);

	virtual tRay crossAndGen(tRay ray,double &t);
};
 */

class tObjectPlane:public tPlane{
private:
    std::vector<std::vector<double> > object_map;
    int width;
    int height;
    double pixel_size;
    void read_object_map(std::string objectFile);
public:
    tObjectPlane(Vec3d _N, Vec3d _r0, std::string objectFile, double pixel_size):tPlane(_N,_r0){
        this->pixel_size = pixel_size;
        this->read_object_map(objectFile);
    }
    double cross(tRay ray);


};
