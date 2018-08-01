#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h> 
#include <gl/glui.h>
#include "TriMesh.h"
#define PI	3.141592657

class Sphere{
	private:
		int horizontal_resolution;
		int vertical_resolution;
		double color[4];
		vector<point> points;
	public:
		Sphere(int, int, double, double, double, double);
		Sphere(int, int);
		Sphere();
		void set_color(double, double, double, double);
		void draw();		
		int get_horizontal_resolution();
		int get_vertical_resolution();
		void update_transformation(double, double, double, double);
};