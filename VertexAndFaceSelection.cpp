#include "Include.h"

void get_pixel_coordinates(double x, double y, double z, double &pixel_x, double &pixel_y){
	GLdouble model_view[16];
	glGetDoublev(GL_MODELVIEW_MATRIX, model_view);
	GLdouble projection[16];
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	GLdouble gl_pixel_x;GLdouble gl_pixel_y;GLdouble gl_pixel_z;
	gluProject(x, y, z, model_view, projection, viewport, &gl_pixel_x, &gl_pixel_y, &gl_pixel_z);
	pixel_x=(double)gl_pixel_x;pixel_y=(double)gl_pixel_y;
}

// Use ray casting algorithm to test whether a given point (x0, y0) is inside the polygon selection_vertices.
// The ray starts at (x0, y0) and shoots to (+inf, y0)
bool is_inside_polygon(double x0, double y0){
	int intersection_count=0;
	for(int i=0;i<selection_coordinates.size()/2-1;i++){
		// check if (x, y) intersects with edge (x1, y1)~(x2, y2)
		double x1=selection_coordinates[i*2];
		double y1=selection_coordinates[i*2+1];
		double x2=selection_coordinates[i*2+2];
		double y2=selection_coordinates[i*2+3];
		// parametrize the line (x1, y1)~(x2, y2): (x1+t(x2-x1), y1+t(y2-y1)). Solve for t and x, given y1+t(y2-y1)=y0. 
		// check if 0<=t<=1, and if x>=x0. If so, intersects with this edge
		// WARNING: Not sure what to do when t=0 or t=1, as this is likely to be count twice and ignore some vertex that should have been put in selected_vertices
		
		double t=(y0-y1)/(y2-y1);
		// If y2-y1 is zero, then no y intersection point. Impossible to intersect
		if(y2-y1==0.0){
			t=-1.0;
		}
		double x=x1+t*(x2-x1);
		if((t>=0)&&(t<=1)&&(x>=x0)){
			intersection_count++;
		}
	}
	if(intersection_count%2==0){
		return false;
	}
	return true;
}

void update_selected_vertices(){
	hash_set<int> onetime_selected_vertices;
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();	
	glOrtho((boundingSphere[0]-boundingSphere[3])*zoom_factor, (boundingSphere[0]+boundingSphere[3])*zoom_factor, (boundingSphere[1]-boundingSphere[3])*zoom_factor, 
		(boundingSphere[1]+boundingSphere[3])*zoom_factor, 0.0, boundingSphere[3]*10);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-boundingSphere[0], -boundingSphere[1], -boundingSphere[2]);
	gluLookAt(0.0, 0.0, 10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	glMultMatrixf(x_rotation_matrix);
	glMultMatrixf(y_rotation_matrix);
	glMultMatrixf(z_rotation_matrix);
	double min_dist=999999;
	int nearest_vertex_index=-1;

	debug.clear();

	for(int i=0;i<vertices.size();i++){
		double pixel_x;double pixel_y;
		get_pixel_coordinates(vertices[i][0], vertices[i][1], vertices[i][2], pixel_x, pixel_y);
		debug.push_back(pixel_x);
		debug.push_back(glutGet(GLUT_WINDOW_HEIGHT)-pixel_y);
		if(selection_coordinates.size()==2){
			double dist=sqrt(sqr(pixel_x-selection_coordinates[0])+sqr(pixel_y-selection_coordinates[1]));
			if(dist<min_dist){
				min_dist=dist;
				nearest_vertex_index=i;
			}
		}else{
			bool is_inside=is_inside_polygon(pixel_x, glutGet(GLUT_WINDOW_HEIGHT)-pixel_y);
			if(is_inside){
				onetime_selected_vertices.insert(i);
			}
		}
	}

	if(selection_coordinates.size()==2){
		onetime_selected_vertices.insert(nearest_vertex_index);
	}

	if(boolean_selection_ID==0){
		selected_vertices.clear();
		for(hash_set<int>::iterator it=onetime_selected_vertices.begin();it!=onetime_selected_vertices.end();it++){
			selected_vertices.push_back(*it);
		}
	}else if(boolean_selection_ID==1){
		for(int i=0;i<selected_vertices.size();i++){
			hash_set<int>::iterator it=onetime_selected_vertices.find(selected_vertices[i]);
			if(it!=onetime_selected_vertices.end()){
				onetime_selected_vertices.erase(it);
			}
		}
		for(hash_set<int>::iterator it=onetime_selected_vertices.begin();it!=onetime_selected_vertices.end();it++){
			selected_vertices.push_back(*it);
		}
	}else if(boolean_selection_ID==2){
		for(int i=0;i<selected_vertices.size();i++){
			hash_set<int>::iterator it=onetime_selected_vertices.find(selected_vertices[i]);
			if(it!=onetime_selected_vertices.end()){
				selected_vertices.erase(selected_vertices.begin()+i);
				i--;
			}
		}
	}
}