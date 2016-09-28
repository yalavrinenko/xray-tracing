/*
 * InputOutput.cpp
 *
 *  Created on: 17.02.2014
 *      Author: cheshire
 */
#include "InputOutput.hpp"

#include <sstream>

vector<Vec3d> data;

void dump(char* name, char* name_func, tRay* arr, int count, double *t) {
	ofstream out(name);
	out << "function [] = " << name_func << "()" << endl;
	out << "\tfigure" << endl;
	out << "\thold on" << endl;

	out << "[x,y,z]=sphere;" << endl;
	out << "surf(80*x,(80*y+3),(80*z+100));" << endl;

	for (int i = 0; i < count; i++) {
		Vec3d s = arr[i].b;
		Vec3d f = arr[i].trace(t[i]);
		out << "\tplot3([" << s.x << ";" << f.x << "],[" << s.y << ";" << f.y
				<< "],[" << s.z << ";" << f.z << "],'r');" << endl;
	}
	out << "end" << endl;
}

void dumpRef(char* name, char* name_func, tRay* arr, int count, double *t) {
	ofstream out(name);
	out << "function [] = " << name_func << "()" << endl;

	for (int i = 0; i < count; i++) {
		Vec3d s = arr[i].b;
		Vec3d f = arr[i].trace(t[i]);
		out << "\tplot3([" << s.x << ";" << f.x << "],[" << s.y << ";" << f.y
				<< "],[" << s.z << ";" << f.z << "],'b');" << endl;
	}
	out << "end" << endl;
}

void appPoint(Vec3d point) {
	data.push_back(point);
}
void dumpPoint(char* name) {
	ofstream out(name);
	for (int i = 0; i < data.size(); i++)
		out << data[i].x << "\t" << data[i].y << "\t" << data[i].z << endl;
}

string doubleToStr(double a) {
	stringstream ss;
	ss << a;
	string o;
	ss >> o;
	return o;
}

string intToStr(int a) {
	stringstream ss;
	ss << a;
	string o;
	ss >> o;
	return o;
}

void dumpPlane(char* name, vector<Vec3d> odata) {
	ofstream out(name);
	for (int i = 0; i < odata.size(); i++)
		out << odata[i].x << "\t" << odata[i].y << "\t" << odata[i].z << endl;
}

void dumpRoad(char* name, char* name_func, vector<vector<Vec3d> > road) {
	ofstream out(name);
	out << "function [] = " << name_func << "()" << endl;
	out << "\tfigure" << endl;
	out << "\thold on" << endl;

	out << "[x,y,z]=sphere;" << endl;
	out << "surf(80*x,(80*y+100),(80*z));" << endl;

	for (int i = 0; i < road.size(); i++) {
		string x = "[";
		string y = "[";
		string z = "[";
		for (int j = 0; j < road[i].size(); j++) {
			x += doubleToStr(road[i][j].x) + ";";
			y += doubleToStr(road[i][j].y) + ";";
			z += doubleToStr(road[i][j].z) + ";";
		}
		x += "]";
		y += "]";
		z += "]";
		out << "plot3(" << x << "," << y << "," << z << ");" << endl;
	}

	out << "end" << endl;
}

void dumpRoadNOSPH(char* name, char* name_func, vector<vector<Vec3d> > road) {
	ofstream out(name);
	out << "function [] = " << name_func << "()" << endl;
	out << "\tfigure" << endl;
	out << "\thold on" << endl;

	for (int i = 0; i < road.size(); i++) {
		string x = "[";
		string y = "[";
		string z = "[";
		for (int j = 0; j < road[i].size(); j++) {
			x += doubleToStr(road[i][j].x) + ";";
			y += doubleToStr(road[i][j].y) + ";";
			z += doubleToStr(road[i][j].z) + ";";
		}
		x += "]";
		y += "]";
		z += "]";
		out << "plot3(" << x << "," << y << "," << z << ");" << endl;
	}

	out << "end" << endl;
}

void dumpRays(tRay* d,char* fileName,int count, Vec3d dir){
	ofstream off(fileName);
	for (int i=0; i<count; i++){
		Vec3d direction = d[i].k / sqrt(dot(d[i].k));
		double ang=acos(dot2(dir, direction) / (sqrt(dot(dir)) * sqrt(dot(direction))));;
		off<<d[i].k.x<<"\t"<<d[i].k.y<<"\t"<<d[i].k.z<<"\t"<<ang*180/M_PI<<endl;
	}
	off.close();
}

infoOut::infoOut() {
	out.open("log.txt");
}

infoOut::infoOut(char* name) {
	out.open(name);
}

void infoOut::logScene(tSphere *mirror, SphereLight *light) {
	out << "[MIRROR]" << endl;
	out << "[X,Y,Z]" << "[" << mirror->r0.x << ", " << mirror->r0.y << ", "
			<< mirror->r0.z << "]" << endl << endl;
	out << "[LIGHT SOURCE]" << endl;
	out << "[X,Y,Z]" << "[" << light->position.x << ", " << light->position.y
			<< ", " << light->position.z << "]" << endl;
}

void infoOut::logScene(tCylinder *mirror, SphereLight *light) {
	out << "[MIRROR]" << endl;
	out << "[X,Y,Z]" << "[" << mirror->r0.x << ", " << mirror->r0.y << ", "
			<< mirror->r0.z << "]" << endl << endl;
	out << "[LIGHT SOURCE]" << endl;
	out << "[X,Y,Z]" << "[" << light->position.x << ", " << light->position.y
			<< ", " << light->position.z << "]" << endl;
}

void infoOut::logText(string text) {
	out << text << endl;
}
