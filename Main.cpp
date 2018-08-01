#include <string.h>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include "Sphere.h"
#include <math.h>
#include "newmat.h"
#include "newmatap.h"
#include <hash_set>
#include <hash_map>
using namespace std;
using namespace stdext;

namespace global_variables{
int window_width=700;
int window_height=700;

int mesh_ID=0;

GLUI* glui;
GLUI_StaticText *selection_info;
double boundingSphere[4];
double handleBoundingSphere[4];
vector<point> vertices;
vector<vec> vertex_normals;
vector<vec> face_normals;
vector<TriMesh::Face> faces;
vector<vector<int>> neighbors;
vector<vector<int>> adjacent_faces;

GLuint mesh_init_list;
GLuint mesh_display_list;
GLuint wireframe_display_list;
vector<int> updatable_faces;

float red_color;
float green_color;
float blue_color;

bool select_new_vertex_flag=false;

int light_rotation_enabled=0;
int draw_selected_vertices_as_faces=0;
int handle_manipulation_enabled=0;
int selection_is_enabled=0;
int face_selection_is_on=0;
int face_handle_is_on=0;
int handle_selection_is_on=0;
int zooming_enabled=1;
int handle_scaling_enabled=0;

int show_mesh=1;
int show_wireframe=0;
int selection_tool_ID=2;
int boolean_selection_ID=0;
int handle_tool_ID=0;
int boolean_handle_ID=0;
int show_handle_tool=0;
int deformation_update_options=1;

GLfloat x_rotation_matrix[16];
GLfloat y_rotation_matrix[16];
GLfloat z_rotation_matrix[16];
GLfloat mesh_x_rotation_matrix[16];
GLfloat mesh_y_rotation_matrix[16];
GLfloat mesh_z_rotation_matrix[16];
GLfloat light_x_rotation_matrix[16];
GLfloat light_y_rotation_matrix[16];
GLfloat light_z_rotation_matrix[16];
GLfloat handle_x_rotation_matrix[16];
GLfloat handle_y_rotation_matrix[16];
GLfloat handle_z_rotation_matrix[16];

GLfloat x_translation[1];
GLfloat y_translation[1];
GLfloat z_translation[1];
GLfloat handle_x_translation[1];
GLfloat handle_y_translation[1];
GLfloat handle_z_translation[1];
GLfloat mesh_x_translation[1];
GLfloat mesh_y_translation[1];
GLfloat mesh_z_translation[1];
double translation_offsets[3];
double handle_translation_offsets[3];
double zoom_factor;
double handle_scale_factor;

class Status{
	public:
		const static int IDLE=1;
		const static int ZOOMING=2;
		const static int SINGLE_CLICK_SELECTION_ACTIVE=3;
		const static int RECTANGLE_SELECTION_ACTIVE=4;
		const static int LASSO_SELECTION_ACTIVE=5;
		const static int HANDLE_SINGLE_CLICK_SELECTION_ACTIVE=6;
		const static int HANDLE_RECTANGLE_SELECTION_ACTIVE=7;
		const static int HANDLE_LASSO_SELECTION_ACTIVE=8;
		const static int HANDLE_MANIPULATION=9;
};
int cur_status=Status::IDLE;

// coordinates of selection polygons. 2n-1 are x-coordinates, 2n are y-coordinates. ith point coordinate is (2i, 2i+1)
vector<int> selection_coordinates;
hash_set<int> selected_vertices;
hash_set<int> selected_vertices_boundary;
hash_set<int> converted_selected_faces;
hash_set<int> converted_selected_faces_boundary;
hash_set<int> converted_selected_handle_faces;

hash_set<int> selected_faces;
hash_set<int> selected_faces_boundary;
hash_set<int> selected_handle_vertices;
hash_set<int> selected_handle_faces;
hash_set<int> selected_handle_vertices_boundary;
hash_set<int> selected_handle_faces_boundary;

vector<int> active_vertices;
vector<int> active_faces;

Sphere handle_sphere(24, 24);

int multi_resolution_modeling_k=2;

float selection_point_size=4.0;

}

namespace utilities{
using namespace global_variables;

bool contains(hash_set<int> &set, int key){
	if(set.find(key)==set.end()){
		return false;
	}
	return true;
}

double vectorLength(double x, double y, double z){
	return sqrt(sqr(x)+sqr(y)+sqr(z));
}

void recompute_face_normal(int face_idx){
	TriMesh::Face face=faces[face_idx];
	double v1[3];
	v1[0]=-vertices[face[0]][0]+vertices[face[1]][0];
	v1[1]=-vertices[face[0]][1]+vertices[face[1]][1];
	v1[2]=-vertices[face[0]][2]+vertices[face[1]][2];
	double v2[3];
	v2[0]=-vertices[face[0]][0]+vertices[face[2]][0];
	v2[1]=-vertices[face[0]][1]+vertices[face[2]][1];
	v2[2]=-vertices[face[0]][2]+vertices[face[2]][2];
	vec v3;
	v3[0]=v1[1]*v2[2]-v1[2]*v2[1];
	v3[1]=v1[2]*v2[0]-v1[0]*v2[2];
	v3[2]=v1[0]*v2[1]-v1[1]*v2[0];
	double l=vectorLength(v3[0], v3[1], v3[2]);
	v3[0]=v3[0]/l;v3[1]=v3[1]/l;v3[2]=v3[2]/l;
	face_normals[face_idx]=v3;
}

void recompute_vertex_normal(int v_idx){
	vec new_norm(0, 0, 0);
	for(int i=0;i<adjacent_faces[v_idx].size();i++){
		int face_idx=adjacent_faces[v_idx][i];
		new_norm[0]=new_norm[0]+face_normals[face_idx][0];
		new_norm[1]=new_norm[1]+face_normals[face_idx][1];
		new_norm[2]=new_norm[2]+face_normals[face_idx][2];
	}
	double l=vectorLength(new_norm[0], new_norm[1], new_norm[2]);
	new_norm[0]=new_norm[0]/l;new_norm[1]=new_norm[1]/l;new_norm[2]=new_norm[2]/l;
	vertex_normals[v_idx]=new_norm;
}

void computeBoundingSphere(){
	int size=vertices.size();
	boundingSphere[0]=0.0;boundingSphere[1]=0.0;boundingSphere[2]=0.0;
	for(int i=0;i<size;i++){
		boundingSphere[0]=boundingSphere[0]+vertices[i][0];
		boundingSphere[1]=boundingSphere[1]+vertices[i][1];
		boundingSphere[2]=boundingSphere[2]+vertices[i][2];
	}
	boundingSphere[0]=boundingSphere[0]/size;
	boundingSphere[1]=boundingSphere[1]/size;
	boundingSphere[2]=boundingSphere[2]/size;
	
	double max=-10000.0;
	for(int i=0;i<size;i++){
		double dist=pow(vertices[i][0]-boundingSphere[0], 2.0)+pow(vertices[i][1]-boundingSphere[1], 2.0)+
			pow(vertices[i][2]-boundingSphere[2], 2.0);
		if(dist>max){
			max=dist;
		}
	}
	boundingSphere[3]=sqrt(max)*2;
}

// v1 is current, v2 is neighbor, v3 is where the angle originates, aka the source vertex
double compute_angle(double x1, double y1, double z1, double x2, double y2, double z2, double x3, double y3, double z3){
	double v1[3];v1[0]=x1-x3;v1[1]=y1-y3;v1[2]=z1-z3;
	double v2[3];v2[0]=x2-x3;v2[1]=y2-y3;v2[2]=z2-z3;
	double l1=vectorLength(v1[0], v1[1], v1[2]);
	double l2=vectorLength(v2[0], v2[1], v2[2]);
	double dot_product=v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2];
	double cos_angle=dot_product/(l1*l2);
	return acos(cos_angle);
}

double compute_dist(double* p1, point p2){
	return sqrt(sqr(p1[0]-p2[0])+sqr(p1[1]-p2[1])+sqr(p1[2]-p2[2]));
}

void get_pixel_coordinates(double x, double y, double z, double &pixel_x, double &pixel_y, double &pixel_z){
	GLdouble model_view[16];
	glGetDoublev(GL_MODELVIEW_MATRIX, model_view);
	GLdouble projection[16];
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	GLdouble gl_pixel_x;GLdouble gl_pixel_y;GLdouble gl_pixel_z;
	gluProject(x, y, z, model_view, projection, viewport, &gl_pixel_x, &gl_pixel_y, &gl_pixel_z);
	pixel_x=(double)gl_pixel_x;pixel_y=glutGet(GLUT_WINDOW_HEIGHT)-(double)gl_pixel_y;pixel_z=gl_pixel_z;
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

void update_color_list(int waste){
	glDeleteLists(mesh_init_list, 1);
	mesh_init_list=glGenLists(1);

	glNewList(mesh_init_list, GL_COMPILE);
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);		
		GLfloat light_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
		glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
		GLfloat mat_diffuse[] = {red_color, green_color, blue_color, 1.0};
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
		GLfloat mat_ambient[] = {red_color/4, green_color/4, blue_color/4, 1.0};
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
		GLfloat mat_specular[] = {red_color/2, green_color/2, blue_color/2, 1.0};
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
		GLfloat mat_shininess[]={10.0};
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	
	glEndList();
}

void update_display_lists(){
	glDeleteLists(mesh_display_list, 1);
	glDeleteLists(wireframe_display_list, 1);

	mesh_display_list=glGenLists(1);
	wireframe_display_list=glGenLists(1);
		
	vector<int> non_updatable_faces;
	updatable_faces.clear();
	if(selected_vertices.size()>0){
		for(int i=0;i<faces.size();i++){
			TriMesh::Face face_temp=faces[i];
			if((!contains(selected_vertices, face_temp[0]))&&(!contains(selected_vertices, face_temp[1]))&&(!contains(selected_vertices, face_temp[2]))){
				non_updatable_faces.push_back(i);
			}else{
				updatable_faces.push_back(i);
			}
		}
	}else{
		for(int i=0;i<faces.size();i++){
			non_updatable_faces.push_back(i);
		}
	}

	glNewList(mesh_display_list, GL_COMPILE);
		glBegin(GL_TRIANGLES);
			for(int i=0;i<non_updatable_faces.size();i++){
				TriMesh::Face face_temp=faces[non_updatable_faces[i]];
				glNormal3f(vertex_normals[face_temp[0]][0], vertex_normals[face_temp[0]][1], vertex_normals[face_temp[0]][2]);
				glVertex3f(vertices[face_temp[0]][0], vertices[face_temp[0]][1], vertices[face_temp[0]][2]);
				glNormal3f(vertex_normals[face_temp[1]][0], vertex_normals[face_temp[1]][1], vertex_normals[face_temp[1]][2]);
				glVertex3f(vertices[face_temp[1]][0], vertices[face_temp[1]][1], vertices[face_temp[1]][2]);
				glNormal3f(vertex_normals[face_temp[2]][0], vertex_normals[face_temp[2]][1], vertex_normals[face_temp[2]][2]);
				glVertex3f(vertices[face_temp[2]][0], vertices[face_temp[2]][1], vertices[face_temp[2]][2]);
			}
		glEnd();
	glEndList();

	glNewList(wireframe_display_list, GL_COMPILE);	
		glEnable(GL_POLYGON_OFFSET_LINE);
		glPolygonOffset(-1.0, 1.0);
		glColor3f(1.0, 0.0, 1.0);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glBegin(GL_TRIANGLES);
			for(int i=0;i<faces.size();i++){
				TriMesh::Face face_temp=faces[i];
				glVertex3f(vertices[face_temp[0]][0], vertices[face_temp[0]][1], vertices[face_temp[0]][2]);
				glVertex3f(vertices[face_temp[1]][0], vertices[face_temp[1]][1], vertices[face_temp[1]][2]);
				glVertex3f(vertices[face_temp[2]][0], vertices[face_temp[2]][1], vertices[face_temp[2]][2]);
			}
		glEnd();
		glDisable(GL_POLYGON_OFFSET_LINE);
	glEndList();
}

}

namespace draw_graphics{

using namespace utilities;
using namespace global_variables;

void redraw(int waste){
	glutPostRedisplay();
}

void update_gl_matrices(){
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();	
	glOrtho((boundingSphere[0]-boundingSphere[3])*zoom_factor, (boundingSphere[0]+boundingSphere[3])*zoom_factor, (boundingSphere[1]-boundingSphere[3])*zoom_factor, 
		(boundingSphere[1]+boundingSphere[3])*zoom_factor, 0.0, boundingSphere[3]*10);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, boundingSphere[3], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	glTranslatef((mesh_x_translation[0]+translation_offsets[0])*boundingSphere[3]/300, (mesh_y_translation[0]+translation_offsets[1])*boundingSphere[3]/300, 
		(mesh_z_translation[0]+translation_offsets[2])*boundingSphere[3]/300);
	glMultMatrixf(mesh_x_rotation_matrix);
	glMultMatrixf(mesh_y_rotation_matrix);
	glMultMatrixf(mesh_z_rotation_matrix);	
	glTranslatef(-boundingSphere[0], -boundingSphere[1], -boundingSphere[2]);
}

void draw_selected_vertices(){
	glBegin(GL_POINTS);
		glColor3f(0.2, 0.5, 1.0);
		for(hash_set<int>::iterator it=selected_vertices.begin();it!=selected_vertices.end();it++){
			int index=*it;
			if(!contains(selected_vertices_boundary, index)){
				glVertex3f(vertices[index][0], vertices[index][1], vertices[index][2]);
			}
		}
		glColor3f(0.5, 0.8, 1.0);
		for(hash_set<int>::iterator it=selected_vertices_boundary.begin();it!=selected_vertices_boundary.end();it++){
			int index=*it;
			glVertex3f(vertices[index][0], vertices[index][1], vertices[index][2]);
		}
	glEnd();
}

void draw_selected_handle_vertices(){
	glBegin(GL_POINTS);
		glColor3f(0.6, 0.8, 0.2);
		for(hash_set<int>::iterator it=selected_handle_vertices.begin();it!=selected_handle_vertices.end();it++){
			int index=*it;
			glVertex3f(vertices[index][0], vertices[index][1], vertices[index][2]);
		}
	glEnd();
}

void draw_selected_faces(){
	glColor4f(0.2, 0.5, 1.0, 0.75);	
	glPolygonOffset(-2.0, 1.0);
	glBegin(GL_TRIANGLES);
		for(hash_set<int>::iterator it=converted_selected_faces.begin();it!=converted_selected_faces.end();it++){
			int index=*it;
			if(!contains(converted_selected_handle_faces, index)){
				int v1_idx=faces[index][0];
				int v2_idx=faces[index][1];
				int v3_idx=faces[index][2];
				glVertex3f(vertices[v1_idx][0], vertices[v1_idx][1], vertices[v1_idx][2]);
				glVertex3f(vertices[v2_idx][0], vertices[v2_idx][1], vertices[v2_idx][2]);
				glVertex3f(vertices[v3_idx][0], vertices[v3_idx][1], vertices[v3_idx][2]);	
			}
		}
		glColor4f(0.4, 0.8, 1.0, 0.75);
		for(hash_set<int>::iterator it=converted_selected_faces_boundary.begin();it!=converted_selected_faces_boundary.end();it++){
			int index=*it;
			int v1_idx=faces[index][0];
			int v2_idx=faces[index][1];
			int v3_idx=faces[index][2];
			glVertex3f(vertices[v1_idx][0], vertices[v1_idx][1], vertices[v1_idx][2]);
			glVertex3f(vertices[v2_idx][0], vertices[v2_idx][1], vertices[v2_idx][2]);
			glVertex3f(vertices[v3_idx][0], vertices[v3_idx][1], vertices[v3_idx][2]);
		}
	glEnd();
}

void draw_selected_handle_faces(){
	glColor4f(0.8, 1.0, 0.2, 0.75);	
	glPolygonOffset(-3.0, 1.0);
	glBegin(GL_TRIANGLES);
		for(hash_set<int>::iterator it=converted_selected_handle_faces.begin();it!=converted_selected_handle_faces.end();it++){
			int index=*it;
			int v1_idx=faces[index][0];
			int v2_idx=faces[index][1];
			int v3_idx=faces[index][2];
			glVertex3f(vertices[v1_idx][0], vertices[v1_idx][1], vertices[v1_idx][2]);
			glVertex3f(vertices[v2_idx][0], vertices[v2_idx][1], vertices[v2_idx][2]);
			glVertex3f(vertices[v3_idx][0], vertices[v3_idx][1], vertices[v3_idx][2]);
		}
	glEnd();
}

void draw_selection_tool(){
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	glOrtho (0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 0, 0, 1);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINES);
		for(int i=0;i<selection_coordinates.size()/2-1;i++){
			glVertex3f(selection_coordinates[2*i], selection_coordinates[2*i+1], 0.0);
			glVertex3f(selection_coordinates[2*i+2], selection_coordinates[2*i+3], 0.0);
		}
	glEnd();
}

void drawGraphics(){
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glCallList(mesh_init_list);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMultMatrixf(light_x_rotation_matrix);
	glMultMatrixf(light_y_rotation_matrix);
	glMultMatrixf(light_z_rotation_matrix);
	GLfloat light_direction[]={ 0.0, 0.0, boundingSphere[3], 0.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_direction);
	update_gl_matrices();

	if(show_mesh){
		glCallList(mesh_display_list);
		glBegin(GL_TRIANGLES);
			for(int i=0;i<updatable_faces.size();i++){
				TriMesh::Face face_temp=faces[updatable_faces[i]];
				glNormal3f(vertex_normals[face_temp[0]][0], vertex_normals[face_temp[0]][1], vertex_normals[face_temp[0]][2]);
				glVertex3f(vertices[face_temp[0]][0], vertices[face_temp[0]][1], vertices[face_temp[0]][2]);
				glNormal3f(vertex_normals[face_temp[1]][0], vertex_normals[face_temp[1]][1], vertex_normals[face_temp[1]][2]);
				glVertex3f(vertices[face_temp[1]][0], vertices[face_temp[1]][1], vertices[face_temp[1]][2]);
				glNormal3f(vertex_normals[face_temp[2]][0], vertex_normals[face_temp[2]][1], vertex_normals[face_temp[2]][2]);
				glVertex3f(vertices[face_temp[2]][0], vertices[face_temp[2]][1], vertices[face_temp[2]][2]);
			}
		glEnd();
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
	}

	if(show_wireframe){
		glEnable(GL_POLYGON_OFFSET_LINE);
		glPolygonOffset(-1.0, 1.0);
		glColor3f(1.0, 0.0, 1.0);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glBegin(GL_TRIANGLES);
			for(int i=0;i<faces.size();i++){
				TriMesh::Face face_temp=faces[i];
				glVertex3f(vertices[face_temp[0]][0], vertices[face_temp[0]][1], vertices[face_temp[0]][2]);
				glVertex3f(vertices[face_temp[1]][0], vertices[face_temp[1]][1], vertices[face_temp[1]][2]);
				glVertex3f(vertices[face_temp[2]][0], vertices[face_temp[2]][1], vertices[face_temp[2]][2]);
			}
		glEnd();
		glDisable(GL_POLYGON_OFFSET_LINE);
	}
	
	if(!draw_selected_vertices_as_faces){
		glPointSize(selection_point_size);
		draw_selected_handle_vertices();
		draw_selected_vertices();	
	}else{
		glEnable(GL_BLEND);
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		draw_selected_faces();	
		draw_selected_handle_faces();
		glDisable(GL_POLYGON_OFFSET_FILL);
		glDisable(GL_BLEND);
	}

	if((cur_status==Status::RECTANGLE_SELECTION_ACTIVE)||(cur_status==Status::LASSO_SELECTION_ACTIVE)
		||(cur_status==Status::HANDLE_RECTANGLE_SELECTION_ACTIVE)||(cur_status==Status::HANDLE_LASSO_SELECTION_ACTIVE))
	{
		draw_selection_tool();
	}
	
	glFlush();
}

}


namespace select_elements{
using namespace global_variables;
using namespace utilities;
using namespace draw_graphics;

void convert_to_selected_faces(){
	converted_selected_faces.clear();
	converted_selected_faces_boundary.clear();	
	for(hash_set<int>::iterator it=selected_vertices.begin();it!=selected_vertices.end();it++){
		for(int i=0;i<adjacent_faces[*it].size();i++){
			int face_idx=adjacent_faces[*it][i];
			if(!contains(converted_selected_faces, face_idx)){
				converted_selected_faces.insert(face_idx);
			}
		}
	}

	for(hash_set<int>::iterator it=selected_vertices_boundary.begin();it!=selected_vertices_boundary.end();it++){
		for(int i=0;i<adjacent_faces[*it].size();i++){
			int face_idx=adjacent_faces[*it][i];
			if((!contains(converted_selected_faces, face_idx))&&(!contains(converted_selected_faces_boundary, face_idx))){
				converted_selected_faces_boundary.insert(face_idx);
			}
		}
	}
}

void convert_to_selected_handle_faces(){
	converted_selected_handle_faces.clear();
	for(hash_set<int>::iterator it=selected_handle_vertices.begin();it!=selected_handle_vertices.end();it++){
		for(int i=0;i<adjacent_faces[*it].size();i++){
			int face_idx=adjacent_faces[*it][i];
			TriMesh::Face face=faces[face_idx];
			if((contains(selected_handle_vertices, face[0]))&&(contains(selected_handle_vertices, face[1]))&&(contains(selected_handle_vertices, face[2]))
				&&(!contains(converted_selected_handle_faces, face_idx))){
				converted_selected_handle_faces.insert(face_idx);
			}
		}
	}
}

void convert_vertices_to_faces(int waste){
	if((draw_selected_vertices_as_faces)&&(select_new_vertex_flag)){
		if(selected_vertices.size()!=0){
			convert_to_selected_faces();
		}
		if(selected_handle_vertices.size()!=0){
			convert_to_selected_handle_faces();
		}
		select_new_vertex_flag=false;
	}
}

void update_selected_elements(hash_set<int> &selected_elements, hash_set<int> &onetime_selected_elements){
	if(boolean_selection_ID==0){
		selected_elements.clear();
		for(hash_set<int>::iterator it=onetime_selected_elements.begin();it!=onetime_selected_elements.end();it++){
			selected_elements.insert(*it);
		}
	}else if(boolean_selection_ID==1){
		for(hash_set<int>::iterator it=selected_elements.begin();it!=selected_elements.end();it++){
			hash_set<int>::iterator it2=onetime_selected_elements.find(*it);
			if(it2!=onetime_selected_elements.end()){
				onetime_selected_elements.erase(it2);
			}
		}
		for(hash_set<int>::iterator it=onetime_selected_elements.begin();it!=onetime_selected_elements.end();it++){
			selected_elements.insert(*it);
		}
	}else if(boolean_selection_ID==2){
		hash_set<int> old_selected_elements;
		for(hash_set<int>::iterator it=selected_elements.begin();it!=selected_elements.end();it++){
			old_selected_elements.insert(*it);
		}
		for(hash_set<int>::iterator it=old_selected_elements.begin();it!=old_selected_elements.end();it++){
			hash_set<int>::iterator it2=onetime_selected_elements.find(*it);
			if(it2!=onetime_selected_elements.end()){
				selected_elements.erase(*it);
			}
		}
	}
}

void compute_k_neighbor_vertices(int source_idx, hash_set<int> &boundary, int k){
	if(k<=0){
		return;
	}
	for(int i=0;i<neighbors[source_idx].size();i++){
		int n_idx=neighbors[source_idx][i];
		if((!contains(boundary, n_idx))&&(!contains(selected_vertices, n_idx))){
			boundary.insert(n_idx);
		}
		compute_k_neighbor_vertices(n_idx, boundary, k-1);
	}
}

void compute_boundary_vertices(hash_set<int> &selected_vertices, hash_set<int> &boundary, int k){
	for(hash_set<int>::iterator it=selected_vertices.begin();it!=selected_vertices.end();it++){
		int v_idx=*it;
		for(int i=0;i<neighbors[v_idx].size();i++){
			int n_idx=neighbors[v_idx][i];
			if(!contains(selected_vertices, n_idx)){
				boundary.insert(n_idx);
			}
		}
	}
	hash_set<int> temp_boundary;
	for(hash_set<int>::iterator it=boundary.begin();it!=boundary.end();it++){
		compute_k_neighbor_vertices(*it, temp_boundary, k-1);
	}
	boundary.insert(temp_boundary.begin(), temp_boundary.end());
}

void dialate_vertices(hash_set<int> &selected_vertices){
	hash_set<int> add_list;
	for(hash_set<int>::iterator it=selected_vertices.begin();it!=selected_vertices.end();it++){
		for(int i=0;i<neighbors[*it].size();i++){
			int nb_idx=neighbors[*it][i];
			if((!contains(selected_vertices, nb_idx))&&(!contains(add_list, nb_idx))){
				add_list.insert(nb_idx);
			}
		}
	}
	for(hash_set<int>::iterator it=add_list.begin();it!=add_list.end();it++){
		selected_vertices.insert(*it);
	}	
	select_new_vertex_flag=true;
}

void dialate_selections(int waste){
	if(handle_selection_is_on){		
		dialate_vertices(selected_handle_vertices);	
	}else{	
		dialate_vertices(selected_vertices);
		compute_boundary_vertices(selected_vertices, selected_vertices_boundary, multi_resolution_modeling_k);
	}
	update_display_lists();
	glutPostRedisplay();
}

void erode_vertices(hash_set<int> &selected_vertices){
	vector<int> remove_list;
	for(hash_set<int>::iterator it=selected_vertices.begin();it!=selected_vertices.end();it++){
		for(int i=0;i<neighbors[*it].size();i++){
			if(!contains(selected_vertices, neighbors[*it][i])){
				remove_list.push_back(*it);
				break;
			}
		}
	}
	for(int i=0;i<remove_list.size();i++){
		selected_vertices.erase(remove_list[i]);
	}
}

void erode_selections(int waste){
	if(handle_selection_is_on){		
		erode_vertices(selected_handle_vertices);
	}else{		
		erode_vertices(selected_vertices);
		compute_boundary_vertices(selected_vertices, selected_vertices_boundary, multi_resolution_modeling_k);
	}
	update_display_lists();
	glutPostRedisplay();
	select_new_vertex_flag=true;
}

void select_and_update_vertices(){
	selected_faces.clear();
	selected_handle_vertices.clear();
	selected_handle_faces.clear();
	hash_set<int> onetime_selected_vertices;
	converted_selected_faces.clear();
	converted_selected_faces_boundary.clear();
	converted_selected_handle_faces.clear();
	update_gl_matrices();
	double min_dist=999999;
	int nearest_vertex_index=-1;
	for(int i=0;i<vertices.size();i++){
		double pixel_x;double pixel_y;double pixel_z;
		get_pixel_coordinates(vertices[i][0], vertices[i][1], vertices[i][2], pixel_x, pixel_y, pixel_z);
		if(selection_coordinates.size()==2){
			double dist=sqrt(sqr(pixel_x-selection_coordinates[0])+sqr(pixel_y-selection_coordinates[1]));
			if(dist<min_dist){
				min_dist=dist;
				nearest_vertex_index=i;
			}
		}else{
			bool is_inside=is_inside_polygon(pixel_x, pixel_y);
			if(is_inside){
				onetime_selected_vertices.insert(i);
			}
		}
	}
	
	update_selected_elements(selected_vertices, onetime_selected_vertices);
	selected_vertices_boundary.clear();
	selected_faces_boundary.clear();
	compute_boundary_vertices(selected_vertices, selected_vertices_boundary, multi_resolution_modeling_k);
	update_display_lists();
	if(selection_coordinates.size()==2){
		onetime_selected_vertices.insert(nearest_vertex_index);
		char buf[256];sprintf(buf, "%s%d", "Selected Vertex Index: ", nearest_vertex_index);		
		selection_info->set_text(buf);
	}else{
		char buf[256];sprintf(buf, "%s%d", "Selected Vertices Count: ", selected_vertices.size());
		selection_info->set_text(buf);
	}
	select_new_vertex_flag=true;
}

void select_and_update_handle_vertices(){
	selected_handle_faces.clear();
	converted_selected_handle_faces.clear();
	hash_set<int> onetime_selected_vertices;
	update_gl_matrices();
	double min_dist=999999;
	int nearest_vertex_index=-1;
	for(hash_set<int>::iterator it=selected_vertices.begin();it!=selected_vertices.end();it++){
		int index=*it;
		double pixel_x;double pixel_y;double pixel_z;
		get_pixel_coordinates(vertices[index][0], vertices[index][1], vertices[index][2], pixel_x, pixel_y, pixel_z);
		if(selection_coordinates.size()==2){
			double dist=sqrt(sqr(pixel_x-selection_coordinates[0])+sqr(pixel_y-selection_coordinates[1]));
			if(dist<min_dist){
				min_dist=dist;
				nearest_vertex_index=index;
			}
		}else{
			bool is_inside=is_inside_polygon(pixel_x, pixel_y);
			if(is_inside){
				onetime_selected_vertices.insert(index);
			}
		}
	}
	
	update_selected_elements(selected_handle_vertices, onetime_selected_vertices);
	if(selection_coordinates.size()==2){
		onetime_selected_vertices.insert(nearest_vertex_index);
		char buf[256];sprintf(buf, "%s%d", "Selected Handle Vertex Index: ", nearest_vertex_index);		
		selection_info->set_text(buf);
	}else{
		char buf[256];sprintf(buf, "%s%d", "Selected ROI Vertices Count: ", selected_vertices.size()-selected_handle_vertices.size());
		selection_info->set_text(buf);
	}
	select_new_vertex_flag=true;
}

void compute_handle_bounding_sphere(hash_set<int> &handle_vertices){
	int size=handle_vertices.size();
	handleBoundingSphere[0]=0.0;handleBoundingSphere[1]=0.0;handleBoundingSphere[2]=0.0;
	for(hash_set<int>::iterator it=handle_vertices.begin();it!=handle_vertices.end();it++){
		int idx=*it;
		handleBoundingSphere[0]=handleBoundingSphere[0]+vertices[idx][0];
		handleBoundingSphere[1]=handleBoundingSphere[1]+vertices[idx][1];
		handleBoundingSphere[2]=handleBoundingSphere[2]+vertices[idx][2];
	}
	handleBoundingSphere[0]=handleBoundingSphere[0]/size;
	handleBoundingSphere[1]=handleBoundingSphere[1]/size;
	handleBoundingSphere[2]=handleBoundingSphere[2]/size;
	
	double max=-10000.0;
	for(hash_set<int>::iterator it=handle_vertices.begin();it!=handle_vertices.end();it++){
		int idx=*it;
		double dist=pow(vertices[idx][0]-handleBoundingSphere[0], 2.0)+pow(vertices[idx][1]-handleBoundingSphere[1], 2.0)+
			pow(vertices[idx][2]-handleBoundingSphere[2], 2.0);
		if(dist>max){
			max=dist;
		}
	}
	handleBoundingSphere[3]=sqrt(max)*2;
}

}
namespace deformation{
using namespace global_variables;
using namespace utilities;
using namespace draw_graphics;
using namespace select_elements;
hash_set<int> free_vertices;
hash_set<int> constraint_vertices;
hash_set<int> handle_vertices;
hash_map<int, int> vertex_index_map;
// used to efficiently compute face normals
hash_set<int> free_faces;
hash_set<int> free_vertices_boundary;
Matrix L;

void init_vertices_sets(){
	free_vertices.clear();
	constraint_vertices.clear();
	handle_vertices.clear();
	for(int i=0;i<vertices.size();i++){
		if(contains(selected_handle_vertices, i)){
			handle_vertices.insert(i);
		}else if(contains(selected_vertices, i)){
			free_vertices.insert(i);
		}
	}
	for(hash_set<int>::iterator it=selected_vertices_boundary.begin();it!=selected_vertices_boundary.end();it++){
		constraint_vertices.insert(*it);
	}
}

double compute_face_area(double x1, double y1, double z1, double x2, double y2, double z2, double x3, double y3, double z3){
	double a=vectorLength(x1-x2, y1-y2, z1-z2);
	double b=vectorLength(x1-x3, y1-y3, z1-z3);	
	double c=vectorLength(x2-x3, y2-y3, z2-z3);
	double s=(a+b+c)/2.0;
	double area=sqrt(s*(s-a)*(s-b)*(s-c));
	return area;
}

double compute_voronoi_area(double source_x, double source_y, double source_z, vector<int> neighbors, bool possibly_eliminating){
	double vector_area=0.0;
	for(int i=0;i<neighbors.size();i++){
		int next_idx=i+1;
		if(i==neighbors.size()-1){
			next_idx=0;
		}
		point cur_vertex=vertices[neighbors[i]];
		point next_vertex=vertices[neighbors[next_idx]];
		bool do_not_add=false;
		if(possibly_eliminating){
			if(((!contains(constraint_vertices, neighbors[i]))&&(!contains(free_vertices, neighbors[i])))||
				((!contains(constraint_vertices, neighbors[next_idx]))&&(!contains(free_vertices, neighbors[next_idx])))){		
				do_not_add=true;
			}
		}
		if(!do_not_add){
			double area=compute_face_area(source_x, source_y, source_z, cur_vertex[0], cur_vertex[1], cur_vertex[2], next_vertex[0], next_vertex[1], next_vertex[2]);
			vector_area=vector_area+area/3;
		}
	}
	return vector_area;
}

void fill_lapalce_matrix_entry(int v_idx, int L_row, bool eliminating){
	point source_vertex=vertices[v_idx];
	double voronoi_area=compute_voronoi_area(source_vertex[0], source_vertex[1], source_vertex[2], neighbors[v_idx], eliminating);
	if(abs(voronoi_area)<1.0e-7){
		return;
	}
	double diagonal_sum=0.0;
	for(int i=0;i<neighbors[v_idx].size();i++){
		int next_idx=i+1;
		int prev_idx=i-1;
		if(i==0){
			prev_idx=neighbors[v_idx].size()-1;
		}
		if(i==neighbors[v_idx].size()-1){
			next_idx=0;
		}
		point prev_vertex=vertices[neighbors[v_idx][prev_idx]];
		point cur_vertex=vertices[neighbors[v_idx][i]];
		point next_vertex=vertices[neighbors[v_idx][next_idx]];
		double alpha=compute_angle(source_vertex[0], source_vertex[1], source_vertex[2], cur_vertex[0], cur_vertex[1], cur_vertex[2], prev_vertex[0], prev_vertex[1], prev_vertex[2]);
		double beta=compute_angle(source_vertex[0], source_vertex[1], source_vertex[2], cur_vertex[0], cur_vertex[1], cur_vertex[2], next_vertex[0], next_vertex[1], next_vertex[2]);
		double weight=(cos(alpha)/sin(alpha)+cos(beta)/sin(beta));
		int L_col=0;
		if(vertex_index_map.find(neighbors[v_idx][i])!=vertex_index_map.end()){
			L_col=vertex_index_map[neighbors[v_idx][i]];
		}
		if(L_col!=0){		
			L(L_row, L_col)=-weight/(2*voronoi_area);
			diagonal_sum=diagonal_sum+weight/(2*voronoi_area);
		}
	}
	L(L_row, L_row)=diagonal_sum;
}

void init_laplace_matrix(){
	init_vertices_sets();
	//initIJV();
	int L_size=free_vertices.size()+constraint_vertices.size()+handle_vertices.size();
//	L=new SparseSolver(L_size, L_size);
//	L=taucs_ccs_create(vertices.size(), vertices.size(), constraint_vertices.size()+handle_vertices.size()+free_vertices.size()*20, TAUCS_DOUBLE);
	L.ReSize(L_size, L_size);
	L=0;
	int index_itr=1;
	
	for(hash_set<int>::iterator it=free_vertices.begin();it!=free_vertices.end();it++){
		vertex_index_map[*it]=index_itr;
		index_itr++;
	}
	for(hash_set<int>::iterator it=constraint_vertices.begin();it!=constraint_vertices.end();it++){
		vertex_index_map[*it]=index_itr;
		index_itr++;
	}
	for(hash_set<int>::iterator it=handle_vertices.begin();it!=handle_vertices.end();it++){
		vertex_index_map[*it]=index_itr;
		index_itr++;
	}
	// Set the Laplace operator, elements are 1-based
	int L_row=1;
	for(hash_set<int>::iterator it=free_vertices.begin();it!=free_vertices.end();it++){
		fill_lapalce_matrix_entry(*it, L_row, false);
		L_row++;
	}
	for(hash_set<int>::iterator it=constraint_vertices.begin();it!=constraint_vertices.end();it++){
		fill_lapalce_matrix_entry(*it, L_row, true);
		L_row++;
	}
	for(hash_set<int>::iterator it=handle_vertices.begin();it!=handle_vertices.end();it++){
		fill_lapalce_matrix_entry(*it, L_row, false);
		L_row++;
	}
}

void normalize(double* &v){
	double l=vectorLength(v[0], v[1], v[2]);
	v[0]=v[0]/l;v[1]=v[1]/l;v[2]=v[2]/l;
}
// Normalize the vector (x, y, z). Normalized (x, y, z) will be one of the three orthonormal vectors.
// The second vector will be (-z, 0, x), unnormalized
// The third will be (xy/(x^2+z^2), -1, zy/(x^2+z^2)), unnormalized
// If x, z are zero, then second vector is (1/sqrt(2), 0, 1/sqrt(2)), third is (1/sqrt(2), 0, -1/sqrt(2))
// Store these three vectors in the matrix coor
void compute_local_coordinate_system(double x, double y, double z, Matrix &coor){
	if((x==0.0)&&(z==0.0)){
		coor(1, 1)=0;coor(1, 2)=1;coor(1, 3)=0;
		coor(2, 1)=1/sqrt(2.0);coor(2, 2)=0;coor(2, 3)=1/sqrt(2.0);
		coor(3, 1)=1/sqrt(2.0);coor(3, 2)=0;coor(3, 3)=-1/sqrt(2.0);
		return;
	}
	double* e1=new double[3];e1[0]=x;e1[1]=y;e1[2]=z;
	normalize(e1);
	double* e2=new double[3];e2[0]=-z;e2[1]=0;e2[2]=x;
	normalize(e2);
	double temp=1/(sqr(x)+sqr(z));
	double* e3=new double[3];e3[0]=temp*x*y;e3[1]=-1;e3[2]=temp*z*y;
	normalize(e3);
	coor(1, 1)=e1[0];coor(2, 1)=e1[1];coor(3, 1)=e1[2];
	coor(1, 2)=e2[0];coor(2, 2)=e2[1];coor(3, 2)=e2[2];
	coor(1, 3)=e3[0];coor(2, 3)=e3[1];coor(3, 3)=e3[2];
}


void find_neighbor_coordinates(int source_idx, Matrix &p, double* &result){
	int nb_idx=neighbors[source_idx][0];
	int row_num=vertex_index_map[nb_idx];
	if(row_num<=free_vertices.size()){
		result[0]=p(row_num, 1);
		result[1]=p(row_num, 2);
		result[2]=p(row_num, 3);
	}else{
		result[0]=vertices[nb_idx][0];
		result[1]=vertices[nb_idx][1];
		result[2]=vertices[nb_idx][2];
	}
}
vector<vector<double>> local_coordinates;
void compute_local_coordinates(Matrix &p){
	local_coordinates.clear();
	int i=1;
	for(hash_set<int>::iterator it=free_vertices.begin();it!=free_vertices.end();it++){
		Matrix E(3, 3);
		double* nb_coor=new double[3];
		find_neighbor_coordinates(*it, p, nb_coor);
		compute_local_coordinate_system(nb_coor[0]-p(i, 1), nb_coor[1]-p(i, 2), nb_coor[2]-p(i, 3), E);
		Matrix disp_vector(3, 1);
		disp_vector(1, 1)=vertices[*it][0]-p(i, 1);
		disp_vector(2, 1)=vertices[*it][1]-p(i, 2);
		disp_vector(3, 1)=vertices[*it][2]-p(i, 3);
		Matrix result=E*disp_vector;
		vector<double> temp(3);temp[0]=result(1, 1);temp[1]=result(2, 1);temp[2]=result(3, 1);
		local_coordinates.push_back(temp);
		i++;
	}
}

void add_details_back(Matrix &p){
	int i=1;
	for(hash_set<int>::iterator it=free_vertices.begin();it!=free_vertices.end();it++){
		Matrix E(3, 3);
		double* nb_coor=new double[3];
		find_neighbor_coordinates(*it, p, nb_coor);
		compute_local_coordinate_system(nb_coor[0]-p(i, 1), nb_coor[1]-p(i, 2), nb_coor[2]-p(i, 3), E);
		double new_disp_x=E(1, 1)*local_coordinates[i-1][0]+E(2, 1)*local_coordinates[i-1][1]+E(3, 1)*local_coordinates[i-1][2];
		double new_disp_y=E(1, 2)*local_coordinates[i-1][0]+E(2, 2)*local_coordinates[i-1][1]+E(3, 2)*local_coordinates[i-1][2];
		double new_disp_z=E(1, 3)*local_coordinates[i-1][0]+E(2, 3)*local_coordinates[i-1][1]+E(3, 3)*local_coordinates[i-1][2];
		vertices[*it][0]=vertices[*it][0]+new_disp_x;
		vertices[*it][1]=vertices[*it][1]+new_disp_y;
		vertices[*it][2]=vertices[*it][2]+new_disp_z;
		i++;
	}
}

Matrix Lf;
Matrix subLf;
Matrix subL;
Matrix h;
vector<vector<double>> handle_offsets;
vector<vector<double>> new_handle_offsets;
double handle_CoM[3];
void compute_free_vertices_boundary(hash_set<int> &free_vertices_boundary){
	for(hash_set<int>::iterator it=handle_vertices.begin();it!=handle_vertices.end();it++){
		for(int i=0;i<neighbors[*it].size();i++){
			int v_idx=neighbors[*it][i];
			if((!contains(free_vertices, v_idx))&&(!contains(free_vertices_boundary, v_idx))){
				free_vertices_boundary.insert(v_idx);
			}
		}
	}
}
void init_handle_offsets(){
	handle_offsets.clear();
	new_handle_offsets.clear();
	handle_CoM[0]=0;handle_CoM[1]=0;handle_CoM[2]=0;
	for(hash_set<int>::iterator it=handle_vertices.begin();it!=handle_vertices.end();it++){
		handle_CoM[0]=handle_CoM[0]+vertices[*it][0];
		handle_CoM[1]=handle_CoM[1]+vertices[*it][1];
		handle_CoM[2]=handle_CoM[2]+vertices[*it][2];
	}
	handle_CoM[0]=handle_CoM[0]/handle_vertices.size();
	handle_CoM[1]=handle_CoM[1]/handle_vertices.size();
	handle_CoM[2]=handle_CoM[2]/handle_vertices.size();
	for(hash_set<int>::iterator it=handle_vertices.begin();it!=handle_vertices.end();it++){
		vector<double> temp(3);
		temp[0]=-handle_CoM[0]+vertices[*it][0];
		temp[1]=-handle_CoM[1]+vertices[*it][1];
		temp[2]=-handle_CoM[2]+vertices[*it][2];
		handle_offsets.push_back(temp);
		new_handle_offsets.push_back(temp);
	}
	free_faces.clear();
	free_vertices_boundary.clear();
	for(hash_set<int>::iterator it=free_vertices.begin();it!=free_vertices.end();it++){
		for(int i=0;i<adjacent_faces[*it].size();i++){
			int face_idx=adjacent_faces[*it][i];
			if(!contains(free_faces, face_idx)){
				free_faces.insert(face_idx);
			}
		}
	}
	compute_free_vertices_boundary(free_vertices_boundary);
	for(hash_set<int>::iterator it=free_vertices_boundary.begin();it!=free_vertices_boundary.end();it++){
		for(int i=0;i<adjacent_faces[*it].size();i++){
			int face_idx=adjacent_faces[*it][i];
			if(!contains(free_faces, face_idx)){
				free_faces.insert(face_idx);
			}
		}
	}
}
void init_deformation(int waste){
	selection_is_enabled=0;
	handle_selection_is_on=0;
	handle_manipulation_enabled=1;
	light_rotation_enabled=0;
	for(int i=0;i<16;i++){
		x_rotation_matrix[i]=handle_x_rotation_matrix[i];
		y_rotation_matrix[i]=handle_y_rotation_matrix[i];
		z_rotation_matrix[i]=handle_z_rotation_matrix[i];
	}
	handle_translation_offsets[0]=handle_x_translation[0];
	handle_translation_offsets[1]=handle_y_translation[0];
	handle_translation_offsets[2]=handle_z_translation[0];
	handle_x_translation[0]=0.0;x_translation[0]=0.0;
	handle_y_translation[0]=0.0;y_translation[0]=0.0;
	handle_z_translation[0]=0.0;z_translation[0]=0.0;
	glui->sync_live();
	
	vertex_index_map.clear();
	init_laplace_matrix();
	init_handle_offsets();
	int L_size=free_vertices.size()+constraint_vertices.size()+handle_vertices.size();
	Matrix tempL=L;
	for(int i=1;i<multi_resolution_modeling_k;i++){
		L=L*tempL;
	}
	for(int i=free_vertices.size()+1;i<=L_size;i++){
		L(i, i)=1;
		for(int j=1;j<=L_size;j++){
			if(i==j){
				L(i, j)=1;
			}else{
				L(i, j)=0;
			}
		}
	}
/*	ofstream debug;debug.open("debug.txt");
	for(int i=1;i<=L_size;i++){
		for(int j=1;j<=L_size;j++){
			debug<<L(i, j)<<" ";
		}
		debug<<"\n";
	}
	debug<<"\n\n\n";*/
	Matrix L_topleft=L.SubMatrix(1, free_vertices.size(), 1, free_vertices.size());
	Matrix L_topright=-L.SubMatrix(1, free_vertices.size(), free_vertices.size()+1, L_size);
	L_topleft=L_topleft.i();
	L_topright=L_topleft*L_topright;
	for(int i=1;i<=free_vertices.size();i++){
		for(int j=1;j<=free_vertices.size();j++){
			L(i, j)=L_topleft(i, j);
		}
		for(int j=free_vertices.size()+1;j<=L_size;j++){
			L(i, j)=L_topright(i, j-free_vertices.size());
		}
	}
/*	for(int i=1;i<=L_size;i++){
		for(int j=1;j<=L_size;j++){
			debug<<L(i, j)<<" ";
		}
		debug<<"\n";
	}
	debug.close();*/
//	L=L.i();
//	CroutMatrix Lcrout=L;
	Matrix f(L_size, 3);
	f=0;
	int i=free_vertices.size()+1;
	for(hash_set<int>::iterator it=constraint_vertices.begin();it!=constraint_vertices.end();it++){
		f(i, 1)=vertices[*it][0];f(i, 2)=vertices[*it][1];f(i, 3)=vertices[*it][2];
		i++;
	}
	Lf=L*f;
	subLf=Lf.SubMatrix(1, free_vertices.size(), 1, 3);
	subL=L.SubMatrix(1, free_vertices.size(), L_size-handle_vertices.size()+1, L_size);
	h.ReSize(handle_vertices.size(), 3);h=0;
	i=1;
	for(hash_set<int>::iterator it=handle_vertices.begin();it!=handle_vertices.end();it++){
		h(i, 1)=vertices[*it][0];h(i, 2)=vertices[*it][1];h(i, 3)=vertices[*it][2];
		i++;
	}
	Matrix p=subLf+subL*h;
	compute_local_coordinates(p);
}

void recompute_free_vertices_normals(){
	for(hash_set<int>::iterator it=free_faces.begin();it!=free_faces.end();it++){
		recompute_face_normal(*it);
	}
	for(hash_set<int>::iterator it=free_vertices.begin();it!=free_vertices.end();it++){
		recompute_vertex_normal(*it);
	}
}

void update_offsets_and_handle(){
	double value_holder;
	for(int i=0;i<handle_offsets.size();i++){
		// scale
		new_handle_offsets[i][0]=handle_scale_factor*handle_offsets[i][0];
		new_handle_offsets[i][1]=handle_scale_factor*handle_offsets[i][1];
		new_handle_offsets[i][2]=handle_scale_factor*handle_offsets[i][2];

		// x rotate
		value_holder=new_handle_offsets[i][1];
		new_handle_offsets[i][1]=new_handle_offsets[i][1]*handle_x_rotation_matrix[5]+new_handle_offsets[i][2]*handle_x_rotation_matrix[6];
		new_handle_offsets[i][2]=value_holder*handle_x_rotation_matrix[9]+new_handle_offsets[i][2]*handle_x_rotation_matrix[10];

		// y rotate
		value_holder=new_handle_offsets[i][0];
		new_handle_offsets[i][0]=new_handle_offsets[i][0]*handle_y_rotation_matrix[0]+new_handle_offsets[i][2]*handle_y_rotation_matrix[2];
		new_handle_offsets[i][2]=value_holder*handle_y_rotation_matrix[8]+new_handle_offsets[i][2]*handle_y_rotation_matrix[10];

		// z rotate
		value_holder=new_handle_offsets[i][0];
		new_handle_offsets[i][0]=new_handle_offsets[i][0]*handle_z_rotation_matrix[0]+handle_offsets[i][1]*handle_z_rotation_matrix[1];
		new_handle_offsets[i][1]=value_holder*handle_z_rotation_matrix[4]+new_handle_offsets[i][1]*handle_z_rotation_matrix[5];
	}
	int i=0;
	for(hash_set<int>::iterator it=handle_vertices.begin();it!=handle_vertices.end();it++){
		int v_idx=*it;
		vertices[v_idx][0]=handle_CoM[0]+(handle_x_translation[0]+handle_translation_offsets[0])*boundingSphere[3]/300+new_handle_offsets[i][0];
		vertices[v_idx][1]=handle_CoM[1]+(handle_y_translation[0]+handle_translation_offsets[1])*boundingSphere[3]/300+new_handle_offsets[i][1];
		vertices[v_idx][2]=handle_CoM[2]+(handle_z_translation[0]+handle_translation_offsets[2])*boundingSphere[3]/300+new_handle_offsets[i][2];
		h(i+1, 1)=vertices[v_idx][0];h(i+1, 2)=vertices[v_idx][1];h(i+1, 3)=vertices[v_idx][2];
		i++;
	}
}

void update_deformation(int waste){
	update_offsets_and_handle();
	if(deformation_update_options==3){
		return;
	}	
	Matrix new_p=subLf+subL*h;
	int i=1;
	for(hash_set<int>::iterator it=free_vertices.begin();it!=free_vertices.end();it++){
		vertices[*it][0]=new_p(i, 1);
		vertices[*it][1]=new_p(i, 2);
		vertices[*it][2]=new_p(i, 3);
		i++;
	}
	add_details_back(new_p);
	if(deformation_update_options==1){
		recompute_free_vertices_normals();
	}
	
	glutPostRedisplay();
}

}

namespace GUI{

using namespace global_variables;
using namespace utilities;
using namespace draw_graphics;
using namespace select_elements;
using namespace deformation;

int old_mouse_x;
int old_mouse_y;

void glutMouseClickFunction(int button, int button_status, int x, int y){
	old_mouse_x=x;
	old_mouse_y=y;
	if(button==GLUT_LEFT_BUTTON){
		if(button_status==GLUT_DOWN){
			if(cur_status==Status::IDLE){
				if(selection_is_enabled){
					if(selection_tool_ID==0){
						if(handle_selection_is_on){
							cur_status=Status::HANDLE_SINGLE_CLICK_SELECTION_ACTIVE;
						}else{
							cur_status=Status::SINGLE_CLICK_SELECTION_ACTIVE;
						}
						selection_coordinates.clear();
						selection_coordinates.push_back(x);
						selection_coordinates.push_back(y);
					}else if(selection_tool_ID==1){
						if(handle_selection_is_on){
							cur_status=Status::HANDLE_LASSO_SELECTION_ACTIVE;
						}else{
							cur_status=Status::LASSO_SELECTION_ACTIVE;
						}
						selection_coordinates.clear();
						selection_coordinates.push_back(x);
						selection_coordinates.push_back(y);
						selection_coordinates.push_back(x);
						selection_coordinates.push_back(y);
					}else if(selection_tool_ID==2){
						if(handle_selection_is_on){
							cur_status=Status::HANDLE_RECTANGLE_SELECTION_ACTIVE;
						}else{
							cur_status=Status::RECTANGLE_SELECTION_ACTIVE;
						}
						selection_coordinates.clear();
						selection_coordinates.push_back(x);
						selection_coordinates.push_back(y);
						selection_coordinates.push_back(x);
						selection_coordinates.push_back(y);
						selection_coordinates.push_back(x);
						selection_coordinates.push_back(y);
						selection_coordinates.push_back(x);
						selection_coordinates.push_back(y);
						selection_coordinates.push_back(x);
						selection_coordinates.push_back(y);
					}				
				}else if(handle_manipulation_enabled){
					cur_status=Status::HANDLE_MANIPULATION;
				}
			}else if((cur_status==Status::LASSO_SELECTION_ACTIVE)||(cur_status==Status::HANDLE_LASSO_SELECTION_ACTIVE)){
				int size=selection_coordinates.size();
		//		selection_coordinates[size-2]=x;
		//		selection_coordinates[size-1]=y;
				selection_coordinates.push_back(x);
				selection_coordinates.push_back(y);
			}
		}else if(button_status==GLUT_UP){
			if((cur_status==Status::RECTANGLE_SELECTION_ACTIVE)||(cur_status==Status::SINGLE_CLICK_SELECTION_ACTIVE)){			
				select_and_update_vertices();			
				cur_status=Status::IDLE;
			}else if((cur_status==Status::HANDLE_RECTANGLE_SELECTION_ACTIVE)||(cur_status==Status::HANDLE_SINGLE_CLICK_SELECTION_ACTIVE)){			
				select_and_update_handle_vertices();
				compute_handle_bounding_sphere(selected_handle_vertices);			
				cur_status=Status::IDLE;
			}else if(cur_status==Status::HANDLE_MANIPULATION){
				cur_status=Status::IDLE;
			}
		}
	}else if(button==GLUT_RIGHT_BUTTON){
		if(cur_status==Status::LASSO_SELECTION_ACTIVE){
			cur_status=Status::IDLE;
			selection_coordinates.pop_back();
			selection_coordinates.pop_back();
			selection_coordinates.push_back(selection_coordinates[0]);
			selection_coordinates.push_back(selection_coordinates[1]);		
			select_and_update_vertices();
			compute_handle_bounding_sphere(selected_handle_vertices);
		}else if(cur_status==Status::HANDLE_LASSO_SELECTION_ACTIVE){
			cur_status=Status::IDLE;
			selection_coordinates.pop_back();
			selection_coordinates.pop_back();
			selection_coordinates.push_back(selection_coordinates[0]);
			selection_coordinates.push_back(selection_coordinates[1]);
			select_and_update_handle_vertices();
			compute_handle_bounding_sphere(selected_handle_vertices);	
		}
	}
	if(button_status==GLUT_UP){
		if(button==GLUT_WHEEL_UP){
			if(zooming_enabled){
				zoom_factor=zoom_factor-0.03;
			}
			if(handle_scaling_enabled){
				handle_scale_factor=handle_scale_factor-0.03;
				update_deformation(0);
			}
		}else if(button==GLUT_WHEEL_DOWN){
			if(zooming_enabled){
				zoom_factor=zoom_factor+0.03;
			}
			if(handle_scaling_enabled){
				handle_scale_factor=handle_scale_factor+0.03;
				update_deformation(0);
			}
		}
	}
	glutPostRedisplay();
}

void update_modelview_matrices(int waste){
	if(light_rotation_enabled){
		for(int i=0;i<16;i++){
			light_x_rotation_matrix[i]=x_rotation_matrix[i];
			light_y_rotation_matrix[i]=y_rotation_matrix[i];
			light_z_rotation_matrix[i]=z_rotation_matrix[i];
		}
	}else if(handle_manipulation_enabled){
		for(int i=0;i<16;i++){
			handle_x_rotation_matrix[i]=x_rotation_matrix[i];
			handle_y_rotation_matrix[i]=y_rotation_matrix[i];
			handle_z_rotation_matrix[i]=z_rotation_matrix[i];
		}
		handle_x_translation[0]=x_translation[0];
		handle_y_translation[0]=y_translation[0];
		handle_z_translation[0]=z_translation[0];
		update_deformation(0);
	}else{
		for(int i=0;i<16;i++){
			mesh_x_rotation_matrix[i]=x_rotation_matrix[i];
			mesh_y_rotation_matrix[i]=y_rotation_matrix[i];
			mesh_z_rotation_matrix[i]=z_rotation_matrix[i];
		}
		mesh_x_translation[0]=x_translation[0];
		mesh_y_translation[0]=y_translation[0];
		mesh_z_translation[0]=z_translation[0];
	}
}


void glutMouseMotionFunction(int x, int y){
	if((cur_status==Status::RECTANGLE_SELECTION_ACTIVE)||(cur_status==Status::HANDLE_RECTANGLE_SELECTION_ACTIVE)){
		selection_coordinates[3]=y;
		selection_coordinates[4]=x;
		selection_coordinates[5]=y;
		selection_coordinates[6]=x;
	}else if((cur_status==Status::LASSO_SELECTION_ACTIVE)||(cur_status==Status::HANDLE_LASSO_SELECTION_ACTIVE)){
		int size=selection_coordinates.size();
		selection_coordinates[size-2]=x;
		selection_coordinates[size-1]=y;
	}else if(cur_status==Status::HANDLE_MANIPULATION){
	/*	double x_dif=(x-old_mouse_x)*boundingSphere[3]/30.0;
		double y_dif=(y-old_mouse_y)*boundingSphere[3]/30.0;
		GLdouble model_view[16];
		glGetDoublev(GL_MODELVIEW_MATRIX, model_view);
		GLdouble projection[16];
		glGetDoublev(GL_PROJECTION_MATRIX, projection);
		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		GLdouble x_pos;GLdouble y_pos;GLdouble z_pos;
		gluUnProject(x, glutGet(GLUT_WINDOW_HEIGHT)-y, 0, model_view, projection, viewport, &x_pos, &y_pos, &z_pos);
		update_deformation(x_pos, y_pos, z_pos);*/
	//	update_deformation(x, y);
	}
	old_mouse_x=x;
	old_mouse_y=y;
	glutPostRedisplay();
}

void update_UI_options(int signal){
	if((signal==1)||(signal==2)){
		boolean_selection_ID=0;
	}else if(signal==3){
		selection_is_enabled=0;
	}else if(signal==4){
		handle_manipulation_enabled=0;
	}else if(signal==5){
		handle_scaling_enabled=0;
	}else if(signal==6){
		zooming_enabled=0;
	}else if(signal==7){
		selection_is_enabled=0;
		handle_selection_is_on=0;
	}else if(signal==8){
		handle_manipulation_enabled=0;		
		if(!light_rotation_enabled){
			for(int i=0;i<16;i++){
				x_rotation_matrix[i]=mesh_x_rotation_matrix[i];
				y_rotation_matrix[i]=mesh_y_rotation_matrix[i];
				z_rotation_matrix[i]=mesh_z_rotation_matrix[i];
			}
			translation_offsets[0]=mesh_x_translation[0];
			translation_offsets[1]=mesh_y_translation[0];
			translation_offsets[2]=mesh_z_translation[0];
			mesh_x_translation[0]=0.0;x_translation[0]=0.0;
			mesh_y_translation[0]=0.0;y_translation[0]=0.0;
			mesh_z_translation[0]=0.0;z_translation[0]=0.0;
		}else{
			for(int i=0;i<16;i++){
				x_rotation_matrix[i]=light_x_rotation_matrix[i];
				y_rotation_matrix[i]=light_y_rotation_matrix[i];
				z_rotation_matrix[i]=light_z_rotation_matrix[i];
			}
		}
	}else if(signal==9){
		light_rotation_enabled=0;
		if(!handle_manipulation_enabled){
			for(int i=0;i<16;i++){
				x_rotation_matrix[i]=mesh_x_rotation_matrix[i];
				y_rotation_matrix[i]=mesh_y_rotation_matrix[i];
				z_rotation_matrix[i]=mesh_z_rotation_matrix[i];
			}
			translation_offsets[0]=mesh_x_translation[0];
			translation_offsets[1]=mesh_y_translation[0];
			translation_offsets[2]=mesh_z_translation[0];
			mesh_x_translation[0]=0.0;x_translation[0]=0.0;
			mesh_y_translation[0]=0.0;y_translation[0]=0.0;
			mesh_z_translation[0]=0.0;z_translation[0]=0.0;
		}else{
			for(int i=0;i<16;i++){
				x_rotation_matrix[i]=handle_x_rotation_matrix[i];
				y_rotation_matrix[i]=handle_y_rotation_matrix[i];
				z_rotation_matrix[i]=handle_z_rotation_matrix[i];
			}
			handle_translation_offsets[0]=handle_x_translation[0];
			handle_translation_offsets[1]=handle_y_translation[0];
			handle_translation_offsets[2]=handle_z_translation[0];
			handle_x_translation[0]=0.0;x_translation[0]=0.0;
			handle_y_translation[0]=0.0;y_translation[0]=0.0;
			handle_z_translation[0]=0.0;z_translation[0]=0.0;
		}
	}
	glui->sync_live();
}

void init(int waste){
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);	
	red_color=0.0;
    green_color=1.0;
    blue_color=0.6;
	updatable_faces.clear();vertices.clear();faces.clear();neighbors.clear();adjacent_faces.clear();face_normals.clear();vertex_normals.clear();
	for(int i=0;i<16;i++){
		x_rotation_matrix[i]=0.0;y_rotation_matrix[i]=0.0;z_rotation_matrix[i]=0.0;
		mesh_x_rotation_matrix[i]=0.0;mesh_y_rotation_matrix[i]=0.0;mesh_z_rotation_matrix[i]=0.0;
		handle_x_rotation_matrix[i]=0.0;handle_y_rotation_matrix[i]=0.0;handle_z_rotation_matrix[i]=0.0;
		light_x_rotation_matrix[i]=0.0;light_y_rotation_matrix[i]=0.0;light_z_rotation_matrix[i]=0.0;
	}
	x_rotation_matrix[0]=1.0;x_rotation_matrix[5]=1.0;x_rotation_matrix[10]=1.0;x_rotation_matrix[15]=1.0;
	y_rotation_matrix[0]=1.0;y_rotation_matrix[5]=1.0;y_rotation_matrix[10]=1.0;y_rotation_matrix[15]=1.0;
	z_rotation_matrix[0]=1.0;z_rotation_matrix[5]=1.0;z_rotation_matrix[10]=1.0;z_rotation_matrix[15]=1.0;
	mesh_x_rotation_matrix[0]=1.0;mesh_x_rotation_matrix[5]=1.0;mesh_x_rotation_matrix[10]=1.0;mesh_x_rotation_matrix[15]=1.0;
	mesh_y_rotation_matrix[0]=1.0;mesh_y_rotation_matrix[5]=1.0;mesh_y_rotation_matrix[10]=1.0;mesh_y_rotation_matrix[15]=1.0;
	mesh_z_rotation_matrix[0]=1.0;mesh_z_rotation_matrix[5]=1.0;mesh_z_rotation_matrix[10]=1.0;mesh_z_rotation_matrix[15]=1.0;
	light_x_rotation_matrix[0]=1.0;light_x_rotation_matrix[5]=1.0;light_x_rotation_matrix[10]=1.0;light_x_rotation_matrix[15]=1.0;
	light_y_rotation_matrix[0]=1.0;light_y_rotation_matrix[5]=1.0;light_y_rotation_matrix[10]=1.0;light_y_rotation_matrix[15]=1.0;
	light_z_rotation_matrix[0]=1.0;light_z_rotation_matrix[5]=1.0;light_z_rotation_matrix[10]=1.0;light_z_rotation_matrix[15]=1.0;
	handle_x_rotation_matrix[0]=1.0;handle_x_rotation_matrix[5]=1.0;handle_x_rotation_matrix[10]=1.0;handle_x_rotation_matrix[15]=1.0;
	handle_y_rotation_matrix[0]=1.0;handle_y_rotation_matrix[5]=1.0;handle_y_rotation_matrix[10]=1.0;handle_y_rotation_matrix[15]=1.0;
	handle_z_rotation_matrix[0]=1.0;handle_z_rotation_matrix[5]=1.0;handle_z_rotation_matrix[10]=1.0;handle_z_rotation_matrix[15]=1.0;
	handle_x_translation[0]=0.0;handle_y_translation[0]=0.0;handle_z_translation[0]=0.0;
	mesh_x_translation[0]=0.0;mesh_y_translation[0]=0.0;mesh_z_translation[0]=0.0;	
	x_translation[0]=0.0;y_translation[0]=0.0;z_translation[0]=0.0;
	translation_offsets[0]=0.0;translation_offsets[1]=0.0;translation_offsets[2]=0.0;
	handle_translation_offsets[0]=0.0;handle_translation_offsets[1]=0.0;handle_translation_offsets[2]=0.0;
	select_new_vertex_flag=false;
	zoom_factor=0.6;
	handle_scale_factor=1.0;
	light_rotation_enabled=0;
	draw_selected_vertices_as_faces=0;
	handle_manipulation_enabled=0;
	selection_is_enabled=0;
	face_selection_is_on=0;
	face_handle_is_on=0;
	handle_selection_is_on=0;
	zooming_enabled=1;
	handle_scaling_enabled=0;
	show_mesh=1;
	show_wireframe=0;
	selection_tool_ID=2;
	boolean_selection_ID=0;
	handle_tool_ID=0;
	boolean_handle_ID=0;
	show_handle_tool=0;
	deformation_update_options=1;
	handle_scale_factor=1.0;
	cur_status=Status::IDLE;
	selection_coordinates.clear();
	selected_vertices.clear();
    selected_vertices_boundary.clear();
	converted_selected_faces.clear();
	converted_selected_faces_boundary.clear();
	converted_selected_handle_faces.clear();
	selected_faces.clear();
	selected_faces_boundary.clear();
	selected_handle_vertices.clear();
	selected_handle_faces.clear();
	selected_handle_vertices_boundary.clear();
	selected_handle_faces_boundary.clear();
	active_vertices.clear();
	active_faces.clear();
	multi_resolution_modeling_k=2;
	selection_point_size=4.0;
	glui->sync_live();

	const char* filename;
	switch(mesh_ID){
		case 0:filename="models//irr4-cyl2.off";break;
		case 1:filename="models//blade-nice-flip-cyl2.off";break;
		case 2:filename="models//camel_head.off";break;
		case 3:filename="models//squirrel.off";break;
		case 4:filename="models//torus.off";break;
		case 5:filename="models//horse.off";break;
		case 6:filename="models//gargoyle.off";break;
		case 7:filename="models//bunny.off";break;
		case 8:filename="models//beetle-ck3-tri.off";break;
		case 9:filename="models//casting.off";break;
		case 10:filename="models//fine_sphere.off";break;
		case 11:filename="models//joint.off";break;
		case 12:filename="models//octa-flower.off";break;
		case 13:filename="models//pinion.off";break;
		case 14:filename="models//rocker_arm.off";break;
		case 15:filename="models//screwdriver.off";break;
		case 16:filename="models//smooth_feature.off";break;
		case 17:filename="models//turbine.off";break;
		case 18:filename="models//venusRemesh.off";break;
	}
	TriMesh* mesh=TriMesh::read(filename);
	mesh->need_faces();
	mesh->need_neighbors();
	mesh->need_adjacentfaces();
	vertices=mesh->vertices;
	faces=mesh->faces;
	neighbors=mesh->neighbors;
	adjacent_faces=mesh->adjacentfaces;
	face_normals.resize(faces.size());
	vertex_normals.resize(vertices.size());
	for(int i=0;i<faces.size();i++){
		recompute_face_normal(i);
	}
	for(int i=0;i<vertices.size();i++){
		recompute_vertex_normal(i);
	}
	computeBoundingSphere();
	handle_sphere.set_color(1.0, 1.0, 0.0, 0.5);
	update_display_lists();	
	update_color_list(0);
}

void init_UI(int argc, char *argv[]){
	glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize (600, 600); 
    glutInitWindowPosition (0, 0);
	int window_id=glutCreateWindow ("Mesh_Editor");
	glui=GLUI_Master.create_glui("Control Panel");
	GLUI_Master.set_glutIdleFunc(NULL);
	glui->set_main_gfx_window(window_id);
	GLUI_Listbox *mesh_loader=glui->add_listbox("Select Mesh: ", &mesh_ID, 0, init);
	mesh_loader->add_item(0, "irr4-cyl2.off");
	mesh_loader->add_item(1, "blade-nice-flip-cyl2.off");
	mesh_loader->add_item(2, "camel_head.off");
	mesh_loader->add_item(3, "squirrel.off");
	mesh_loader->add_item(4, "torus.off");
	mesh_loader->add_item(5, "horse.off");
	mesh_loader->add_item(6, "gargoyle.off");
	mesh_loader->add_item(7, "bunny.off");
	mesh_loader->add_item(8, "beetle-ck3-tri.off");
	mesh_loader->add_item(9, "casting.off");
	mesh_loader->add_item(10, "fine_sphere.off");
	mesh_loader->add_item(11, "joint.off");
	mesh_loader->add_item(12, "octa-flower.off");
	mesh_loader->add_item(13, "pinion.off");
	mesh_loader->add_item(14, "rocker_arm.off");
	mesh_loader->add_item(15, "screwdriver.off");
	mesh_loader->add_item(16, "smooth_feature.off");
	mesh_loader->add_item(17, "turbine.off");
	mesh_loader->add_item(18, "venusRemesh.off");

	GLUI_Panel *display_panel=glui->add_panel("Display & Control Options");
	GLUI_Panel *hidden_disp_panel=glui->add_panel_to_panel(display_panel, "x", GLUI_PANEL_NONE);
	glui->add_checkbox_to_panel(hidden_disp_panel, "Display Mesh? ", &show_mesh);
	glui->add_checkbox_to_panel(hidden_disp_panel, "Display Wireframe? ", &show_wireframe);
	glui->add_column_to_panel(hidden_disp_panel, false);
	GLUI_Panel *color_panel=glui->add_panel_to_panel(hidden_disp_panel, "Color Options");
	glui->add_edittext_to_panel(color_panel, "Red:", GLUI_EDITTEXT_FLOAT, &red_color, 0, update_color_list);
	glui->add_edittext_to_panel(color_panel, "Green:", GLUI_EDITTEXT_FLOAT, &green_color, 0, update_color_list);
	glui->add_edittext_to_panel(color_panel, "Blue:", GLUI_EDITTEXT_FLOAT, &blue_color, 0, update_color_list);

	GLUI_Panel *rotation_panel=glui->add_panel_to_panel(display_panel, "Modeling Controls");
	glui->add_checkbox_to_panel(rotation_panel, "Enable Zooming? ", &zooming_enabled, 5, update_UI_options);
	glui->add_checkbox_to_panel(rotation_panel, "Enable Handle Scaling? ", &handle_scaling_enabled, 6, update_UI_options);
	glui->add_checkbox_to_panel(rotation_panel, "Rotate Light Instead of Mesh? ", &light_rotation_enabled, 8, update_UI_options);
	glui->add_checkbox_to_panel(rotation_panel, "Rotate & Traslate Handle Instead of Mesh?    ", &handle_manipulation_enabled, 9, update_UI_options);
	GLUI_Panel *hidden_rotation_panel=glui->add_panel_to_panel(rotation_panel, "NULL", GLUI_PANEL_NONE);
	glui->add_rotation_to_panel(hidden_rotation_panel, "X Rotation", x_rotation_matrix, 0, update_modelview_matrices);
	glui->add_column_to_panel(hidden_rotation_panel, false);
	glui->add_column_to_panel(hidden_rotation_panel, false);
	glui->add_rotation_to_panel(hidden_rotation_panel, "Y Rotation", y_rotation_matrix, 0, update_modelview_matrices);
	glui->add_column_to_panel(hidden_rotation_panel, false);
	glui->add_column_to_panel(hidden_rotation_panel, false);
	glui->add_rotation_to_panel(hidden_rotation_panel, "Z Rotation", z_rotation_matrix, 0, update_modelview_matrices);
	GLUI_Panel *hidden_translation_panel=glui->add_panel_to_panel(rotation_panel, "NULL", GLUI_PANEL_NONE);
	glui->add_translation_to_panel(hidden_translation_panel, "X Translation", GLUI_TRANSLATION_X, x_translation, 0, update_modelview_matrices);
	glui->add_column_to_panel(hidden_translation_panel, false);
	glui->add_translation_to_panel(hidden_translation_panel, "Y Translation", GLUI_TRANSLATION_Y, y_translation, 0, update_modelview_matrices);
	glui->add_column_to_panel(hidden_translation_panel, false);
	glui->add_translation_to_panel(hidden_translation_panel, "Z Translation", GLUI_TRANSLATION_Z, z_translation, 0, update_modelview_matrices);
	
	GLUI_Panel *selection_panel=glui->add_panel("ROI & Handle Options");
	glui->add_spinner_to_panel(selection_panel, "Set Energy Function Degree: ", GLUI_SPINNER_INT, &multi_resolution_modeling_k);
	glui->add_checkbox_to_panel(selection_panel, "Enable Selection?", &selection_is_enabled, 4, update_UI_options);
	glui->add_checkbox_to_panel(selection_panel, "Select Handle instead of ROI? ", &handle_selection_is_on, 1, update_UI_options);
	GLUI_Spinner *pt_spinner=glui->add_spinner_to_panel(selection_panel, "Set Selection Points Size:    ", GLUI_SPINNER_FLOAT, &selection_point_size, 0, redraw);
	pt_spinner->set_speed(0.5);
	glui->add_checkbox_to_panel(selection_panel, "Draw Selected Vertices as Faces? ", &draw_selected_vertices_as_faces, 0, convert_vertices_to_faces);
	GLUI_Panel *hidden_selection_panel=glui->add_panel_to_panel(selection_panel, "hidden", GLUI_PANEL_NONE);
	GLUI_Panel *tools_panel=glui->add_panel_to_panel(hidden_selection_panel, "Selection Tools");
	GLUI_RadioGroup *selection_tools=glui->add_radiogroup_to_panel(tools_panel, &selection_tool_ID);
	glui->add_radiobutton_to_group(selection_tools, "Single Click");
	glui->add_radiobutton_to_group(selection_tools, "Lasso Tool");
	glui->add_radiobutton_to_group(selection_tools, "Rectangle Box");
	glui->add_button_to_panel(hidden_selection_panel, "Dilation", 0, dialate_selections);
	glui->add_column_to_panel(hidden_selection_panel, false);
	glui->add_column_to_panel(hidden_selection_panel, false);
	glui->add_column_to_panel(hidden_selection_panel, false);
	GLUI_Panel *boolean_selection_panel=glui->add_panel_to_panel(hidden_selection_panel, "Boolean Operations");
	GLUI_RadioGroup *boolean_selections=glui->add_radiogroup_to_panel(boolean_selection_panel, &boolean_selection_ID);
	glui->add_radiobutton_to_group(boolean_selections, "Selection");
	glui->add_radiobutton_to_group(boolean_selections, "Union");
	glui->add_radiobutton_to_group(boolean_selections, "Subtraction");
	glui->add_button_to_panel(hidden_selection_panel, "Erosion", 0, erode_selections);		
	glui->add_button_to_panel(selection_panel, "Initialize Handle Manipulation", 0, init_deformation);
	glui->add_separator_to_panel(selection_panel);
	selection_info=glui->add_statictext_to_panel(selection_panel, "Selected Single Vertex or Face Index: None");

	GLUI_Master.set_glutMouseFunc(glutMouseClickFunction);
	glutDisplayFunc(drawGraphics); 
	glutMotionFunc(glutMouseMotionFunction);	
	init(0);
}

}

int main(int argc, char *argv[]){	
	GUI::init_UI(argc, argv);
	glutMainLoop();
	return 0;
}