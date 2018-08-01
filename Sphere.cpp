#include "Sphere.h"

Sphere::Sphere(){
	Sphere::Sphere(32, 32);
}

Sphere::Sphere(int hr, int vr, double R, double G, double B, double alpha){
	Sphere::Sphere(hr, vr);
	set_color(R, G, B, alpha);
}

Sphere::Sphere(int hr, int vr){	
	horizontal_resolution=hr;
	vertical_resolution=vr;	
	double horizontal_increment=2*PI/horizontal_resolution;
	double vertical_increment=PI/vertical_resolution;

	for(int i=1;i<vertical_resolution;i++){
		double radius=sin(vertical_increment*i);
		double y=cos(vertical_increment*i);
		for(int j=0;j<horizontal_resolution;j++){
			double angle=horizontal_increment*j;
			double x=radius*cos(angle);
			double z=radius*sin(angle);
			points.push_back(point(x, y, z));
		}
	}
}

void Sphere::update_transformation(double x_trans, double y_trans, double z_trans, double r){
	points.clear();
	double horizontal_increment=2*PI/horizontal_resolution;
	double vertical_increment=PI/vertical_resolution;
	for(int i=1;i<vertical_resolution;i++){
		double radius=sin(vertical_increment*i)*r;
		double y=cos(vertical_increment*i)*r+y_trans;
		for(int j=0;j<horizontal_resolution;j++){
			double angle=horizontal_increment*j;
			double x=radius*cos(angle)+x_trans;
			double z=radius*sin(angle)+z_trans;
			points.push_back(point(x, y, z));
		}
	}

}

void Sphere::set_color(double R, double G, double B, double alpha){
	color[0]=R;color[1]=G;color[2]=B;color[3]=alpha;
}

int Sphere::get_horizontal_resolution(){
	return horizontal_resolution;
}

int Sphere::get_vertical_resolution(){
	return vertical_resolution;
}

void Sphere::draw(){
	point point1, point2, point3, point4;
	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glColor4f(color[0], color[1], color[2], color[3]);
	glBegin(GL_POLYGON);
		for(int i=0;i<horizontal_resolution;i++){
			glVertex3d(points[i][0], points[i][1], points[i][2]);	
		}
	glEnd();

	glBegin(GL_QUADS);
		for(int i=0;i<vertical_resolution-2;i++){		
			for(int j=0;j<horizontal_resolution-1;j++){
				point1=points[i*horizontal_resolution+j];
				point2=points[i*horizontal_resolution+j+1];
				point3=points[(i+1)*horizontal_resolution+j+1];
				point4=points[(i+1)*horizontal_resolution+j];

				glVertex3d(point1[0], point1[1], point1[2]);
				glVertex3d(point2[0], point2[1], point2[2]);
				glVertex3d(point3[0], point3[1], point3[2]);
				glVertex3d(point4[0], point4[1], point4[2]);
			}
			
			point1=points[i*horizontal_resolution+horizontal_resolution-1];
			point2=points[i*horizontal_resolution];		
			point3=points[(i+1)*horizontal_resolution];
			point4=points[(i+1)*horizontal_resolution+horizontal_resolution-1];

			glVertex3d(point1[0], point1[1], point1[2]);
			glVertex3d(point2[0], point2[1], point2[2]);
			glVertex3d(point3[0], point3[1], point3[2]);
			glVertex3d(point4[0], point4[1], point4[2]);
		}
	glEnd();

	glBegin(GL_POLYGON);
		int offset=(vertical_resolution-2)*horizontal_resolution;
		for(int i=0;i<horizontal_resolution;i++){		
			glVertex3d(points[offset+i][0], points[offset+i][1], points[offset+i][2]);	
		}
	glEnd();
	glDisable(GL_BLEND);
}