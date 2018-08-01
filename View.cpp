/*#include "View.h"


void update_gl_matrices(){
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();	
	if(view_in_perspective_mode){
		gluPerspective(90.0*zoom_factor, 1.0, 0.0, boundingSphere[3]*10);
	}else{
		glOrtho((boundingSphere[0]-boundingSphere[3])*zoom_factor, (boundingSphere[0]+boundingSphere[3])*zoom_factor, (boundingSphere[1]-boundingSphere[3])*zoom_factor, 
			(boundingSphere[1]+boundingSphere[3])*zoom_factor, 0.0, boundingSphere[3]*10);
	}
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, boundingSphere[3], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	glTranslatef(x_translation[0]/(300*boundingSphere[3]), y_translation[0]/(300*boundingSphere[3]), z_translation[0]/(300*boundingSphere[3]));
	glMultMatrixf(x_rotation_matrix);
	glMultMatrixf(y_rotation_matrix);
	glMultMatrixf(z_rotation_matrix);	
	glTranslatef(-boundingSphere[0], -boundingSphere[1], -boundingSphere[2]);
}

bool contains(hash_set<int> &set, int key){
	if(set.find(key)==set.end()){
		return false;
	}
	return true;
}

void draw_small_diamond(double v1, double v2, double v3, double radius){
	double offset=radius/sqrt(2.0);
	glVertex3f(v1+offset, v2, v3+offset);
	glVertex3f(v1+offset, v2, v3-offset);
	glVertex3f(v1-offset, v2, v3-offset);
	glVertex3f(v1-offset, v2, v3+offset);

	glVertex3f(v1+offset, v2, v3+offset);
	glVertex3f(v1, v2+offset, v3);
	glVertex3f(v1+offset, v2, v3-offset);
	glVertex3f(v1, v2-offset, v3);

	glVertex3f(v1-offset, v2, v3+offset);
	glVertex3f(v1, v2+offset, v3);
	glVertex3f(v1-offset, v2, v3-offset);
	glVertex3f(v1, v2-offset, v3);
}

void draw_selected_vertices(){
//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//	glEnable(GL_POLYGON_OFFSET_LINE);	
	glBegin(GL_POINTS);
	//	glPolygonOffset(-2.0, 1.0);
		glColor3f(0.2, 0.5, 1.0);
		for(hash_set<int>::iterator it=selected_vertices.begin();it!=selected_vertices.end();it++){
			int index=*it;
			if(!contains(selected_vertices_boundary, index)){
				glVertex3f(vertices[index][0], vertices[index][1], vertices[index][2]);
			//	draw_small_diamond( boundingSphere[3]/100.0);
			}
		}
		glColor3f(0.5, 0.8, 1.0);
		for(hash_set<int>::iterator it=selected_vertices_boundary.begin();it!=selected_vertices_boundary.end();it++){
			int index=*it;
			glVertex3f(vertices[index][0], vertices[index][1], vertices[index][2]);
			//draw_small_diamond(vertices[index][0], vertices[index][1], vertices[index][2], boundingSphere[3]/100.0);
		}
	glEnd();
//	glDisable(GL_POLYGON_OFFSET_LINE);
}

void draw_selected_handle_vertices(){
//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//	glEnable(GL_POLYGON_OFFSET_FILL);
//	glPolygonOffset(-4.0, 1.0);
	glBegin(GL_POINTS);
		glColor3f(0.6, 0.8, 0.2);
		for(hash_set<int>::iterator it=selected_handle_vertices.begin();it!=selected_handle_vertices.end();it++){
			int index=*it;
			if(!contains(selected_handle_vertices_boundary, index)){
				glVertex3f(vertices[index][0], vertices[index][1], vertices[index][2]);
				//draw_small_diamond(vertices[index][0], vertices[index][1], vertices[index][2], boundingSphere[3]/100.0);
			}
		}
		glColor3f(0.8, 1.0, 0.4);
		for(hash_set<int>::iterator it=selected_handle_vertices_boundary.begin();it!=selected_handle_vertices_boundary.end();it++){
			int index=*it;
			glVertex3f(vertices[index][0], vertices[index][1], vertices[index][2]);
			//draw_small_diamond(vertices[index][0], vertices[index][1], vertices[index][2], boundingSphere[3]/100.0);
		}
	glEnd();
//	glDisable(GL_POLYGON_OFFSET_FILL);
}
void draw_selected_faces(){
	glColor4f(0.2, 0.5, 1.0, 0.75);	
	glPolygonOffset(-2.0, 1.0);
	glBegin(GL_TRIANGLES);
		for(hash_set<int>::iterator it=selected_faces.begin();it!=selected_faces.end();it++){
			int index=*it;
			if(!contains(selected_faces_boundary, index)){
				int v1_idx=faces[index][0];
				int v2_idx=faces[index][1];
				int v3_idx=faces[index][2];
				glVertex3f(vertices[v1_idx][0], vertices[v1_idx][1], vertices[v1_idx][2]);
				glVertex3f(vertices[v2_idx][0], vertices[v2_idx][1], vertices[v2_idx][2]);
				glVertex3f(vertices[v3_idx][0], vertices[v3_idx][1], vertices[v3_idx][2]);
			}
		}
		glColor4f(0.4, 0.8, 1.0, 0.75);
		for(hash_set<int>::iterator it=selected_faces_boundary.begin();it!=selected_faces_boundary.end();it++){
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
	glColor4f(0.8, 1.0, 0.2, 0.9);	
	glPolygonOffset(-3.0, 1.0);
	glBegin(GL_TRIANGLES);
		for(hash_set<int>::iterator it=selected_handle_faces.begin();it!=selected_handle_faces.end();it++){
			int index=*it;
			if(!contains(selected_handle_faces_boundary, index)){
				int v1_idx=faces[index][0];
				int v2_idx=faces[index][1];
				int v3_idx=faces[index][2];
				glVertex3f(vertices[v1_idx][0], vertices[v1_idx][1], vertices[v1_idx][2]);
				glVertex3f(vertices[v2_idx][0], vertices[v2_idx][1], vertices[v2_idx][2]);
				glVertex3f(vertices[v3_idx][0], vertices[v3_idx][1], vertices[v3_idx][2]);
			}
		}
		glColor4f(1.0, 1.0, 0.5, 0.9);
		for(hash_set<int>::iterator it=selected_handle_faces_boundary.begin();it!=selected_handle_faces_boundary.end();it++){
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

void draw_handle_transformation_tool(){
	handle_sphere.update_transformation(handleBoundingSphere[0], handleBoundingSphere[1], handleBoundingSphere[2], handleBoundingSphere[3]/2);
	handle_sphere.draw();
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
	update_gl_matrices();

	if(show_mesh){
		glCallList(mesh_init_list);
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
	
	if(selected_vertices.size()!=0){
		glPointSize(5.0);
		draw_selected_handle_vertices();
		draw_selected_vertices();	
		if(show_handle_tool){
			draw_handle_transformation_tool();
		}
	}

	if(selected_faces.size()!=0){
		glEnable(GL_BLEND);
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		draw_selected_faces();	
		if(selected_handle_faces.size()!=0){
			draw_selected_handle_faces();
			if(show_handle_tool){
				draw_handle_transformation_tool();
			}
		}
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

*/