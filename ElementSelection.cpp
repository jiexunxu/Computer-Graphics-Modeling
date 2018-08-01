/*#include "ElementSelection.h"

bool contains(hash_set<int> &set, int key){
	if(set.find(key)==set.end()){
		return false;
	}
	return true;
}

void update_gl_matrices(){
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();	
	glOrtho((boundingSphere[0]-boundingSphere[3])*zoom_factor, (boundingSphere[0]+boundingSphere[3])*zoom_factor, (boundingSphere[1]-boundingSphere[3])*zoom_factor, 
		(boundingSphere[1]+boundingSphere[3])*zoom_factor, 0.0, boundingSphere[3]*10);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-boundingSphere[0], -boundingSphere[1], -boundingSphere[2]);
	gluLookAt(0.0, 0.0, boundingSphere[3], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	glMultMatrixf(x_rotation_matrix);
	glMultMatrixf(y_rotation_matrix);
	glMultMatrixf(z_rotation_matrix);
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

void updateDisplayList(){
	glDeleteLists(display_list, 1);
	glDeleteLists(wireframe_list, 1);
	display_list=glGenLists(1);
	wireframe_list=glGenLists(1);
	glNewList(display_list, GL_COMPILE);
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		GLfloat light_direction[]={1.0, 1.0, 1.0, 0.0};
		glLightfv(GL_LIGHT0, GL_POSITION, light_direction);
		GLfloat light_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
		glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
		GLfloat mat_diffuse[] = {0.2, 1.0, 0.6, 1.0};
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
		GLfloat mat_ambient[] = {0.2, 0.2, 0.2, 1.0};
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		
	glEndList();

	glNewList(wireframe_list, GL_COMPILE);	
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



void compute_vertices_from_faces(hash_set<int> &input_face_indices, hash_set<int> &returned_vertex_indices){
	hash_set<int> seen_indices;
	for(hash_set<int>::iterator it=input_face_indices.begin();it!=input_face_indices.end();it++){
		int index=*it;
		int idx1=faces[index][0];
		int idx2=faces[index][1];
		int idx3=faces[index][2];
		if(!contains(seen_indices, idx1)){
			seen_indices.insert(idx1);
			returned_vertex_indices.insert(idx1);
		}
		if(!contains(seen_indices, idx2)){
			seen_indices.insert(idx2);
			returned_vertex_indices.insert(idx2);
		}
		if(!contains(seen_indices, idx3)){
			seen_indices.insert(idx3);
			returned_vertex_indices.insert(idx3);
		}
	}
}

void compute_boundary_vertices(hash_set<int> &selected_vertices, hash_set<int> &boundary){
	for(hash_set<int>::iterator it=selected_vertices.begin();it!=selected_vertices.end();it++){
		int v_idx=*it;
		for(int i=0;i<neighbors[v_idx].size();i++){
			if(!contains(selected_vertices, neighbors[v_idx][i])){
				boundary.insert(v_idx);
			}
		}
	}
}
void compute_boundary_faces(hash_set<int> &selected_faces, hash_set<int> &boundary){
	hash_set<int> vertices_list;
	compute_vertices_from_faces(selected_faces, vertices_list);
	for(hash_set<int>::iterator it=selected_faces.begin();it!=selected_faces.end();it++){
		int face_idx=*it;
		int v1_idx=faces[face_idx][0];int v2_idx=faces[face_idx][1];int v3_idx=faces[face_idx][2];
		bool is_boundary=false;
		for(int i=0;i<neighbors[v1_idx].size();i++){
			if(!contains(vertices_list, neighbors[v1_idx][i])){
				boundary.insert(face_idx);
				is_boundary=true;
				break;
			}
		}
		if(is_boundary){
			continue;
		}
		for(int i=0;i<neighbors[v2_idx].size();i++){
			if(!contains(vertices_list, neighbors[v2_idx][i])){
				boundary.insert(face_idx);
				is_boundary=true;
				break;
			}
		}
		if(is_boundary){
			continue;
		}
		for(int i=0;i<neighbors[v3_idx].size();i++){
			if(!contains(vertices_list, neighbors[v3_idx][i])){
				boundary.insert(face_idx);
				break;
			}
		}
	}
}
void dialate_vertices(hash_set<int> &selected_vertices, hash_set<int> &boundary){
	hash_set<int> old_boundary;
	for(hash_set<int>::iterator it=boundary.begin();it!=boundary.end();it++){
		old_boundary.insert(*it);
	}
	boundary.clear();
	for(hash_set<int>::iterator it=old_boundary.begin();it!=old_boundary.end();it++){
		int v_idx=*it;
		for(int i=0;i<neighbors[v_idx].size();i++){
			int index=neighbors[v_idx][i];
			if(!contains(selected_vertices, index)){
				boundary.insert(index);
				selected_vertices.insert(index);
			}
		}
	}
}
void dialate_faces(hash_set<int> &selected_faces, hash_set<int> &boundary){
	hash_set<int> old_boundary;
	for(hash_set<int>::iterator it=boundary.begin();it!=boundary.end();it++){
		old_boundary.insert(*it);
	}
	boundary.clear();
	for(hash_set<int>::iterator it=old_boundary.begin();it!=old_boundary.end();it++){
		int face_idx=*it;
		int v1_idx=faces[face_idx][0];int v2_idx=faces[face_idx][1];int v3_idx=faces[face_idx][2];
		for(int i=0;i<adjacent_faces[v1_idx].size();i++){
			int index=adjacent_faces[v1_idx][i];
			if(!contains(selected_faces, index)){
				boundary.insert(index);
				selected_faces.insert(index);
			}
		}
		for(int i=0;i<adjacent_faces[v2_idx].size();i++){
			int index=adjacent_faces[v2_idx][i];
			if(!contains(selected_faces, index)){
				boundary.insert(index);
				selected_faces.insert(index);
			}
		}
		for(int i=0;i<adjacent_faces[v3_idx].size();i++){
			int index=adjacent_faces[v3_idx][i];
			if(!contains(selected_faces, index)){
				boundary.insert(index);
				selected_faces.insert(index);
			}
		}
	}
}
void dialate_selections(int waste){
	if(handle_selection_is_on){
		if(selected_handle_vertices.size()!=0){
			dialate_vertices(selected_handle_vertices, selected_handle_vertices_boundary);
		}else if(selected_handle_faces.size()!=0){
			dialate_faces(selected_handle_faces, selected_handle_faces_boundary);
		}
	}else{
		if(selected_vertices.size()!=0){
			dialate_vertices(selected_vertices, selected_vertices_boundary);
		}else if(selected_faces.size()!=0){
			dialate_faces(selected_faces, selected_faces_boundary);
		}
	}
	glutPostRedisplay();
}
void erode_vertices(hash_set<int> &selected_vertices, hash_set<int> &boundary){
	hash_set<int> old_boundary;
	for(hash_set<int>::iterator it=boundary.begin();it!=boundary.end();it++){
		old_boundary.insert(*it);
		selected_vertices.erase(*it);
	}
	boundary.clear();
	compute_boundary_vertices(selected_vertices, boundary);
}
void erode_faces(hash_set<int> &selected_faces, hash_set<int> &boundary){
	hash_set<int> old_boundary;
	for(hash_set<int>::iterator it=boundary.begin();it!=boundary.end();it++){
		old_boundary.insert(*it);
		selected_faces.erase(*it);
	}
	boundary.clear();
	compute_boundary_faces(selected_faces, boundary);
}
void erode_selections(int waste){
	if(handle_selection_is_on){
		if(selected_handle_vertices.size()!=0){
			erode_vertices(selected_handle_vertices, selected_handle_vertices_boundary);
		}else if(selected_handle_faces.size()!=0){
			erode_faces(selected_handle_faces, selected_handle_faces_boundary);
		}
	}else{
		if(selected_vertices.size()!=0){
			erode_vertices(selected_vertices, selected_vertices_boundary);
		}else if(selected_faces.size()!=0){
			erode_faces(selected_faces, selected_faces_boundary);
		}
	}
	glutPostRedisplay();
}

void select_and_update_vertices(){
	selected_faces.clear();
	selected_handle_vertices.clear();
	selected_handle_faces.clear();
	hash_set<int> onetime_selected_vertices;
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

	if(selection_coordinates.size()==2){
		onetime_selected_vertices.insert(nearest_vertex_index);
		char buf[256];sprintf(buf, "%s%d", "Selected Vertex Index: ", nearest_vertex_index);		
		selection_info->set_text(buf);
	}else{
		char buf[256];sprintf(buf, "%s", "Selected Vertex Indices: Not Shown");
		selection_info->set_text(buf);
	}
	update_selected_elements(selected_vertices, onetime_selected_vertices);
	selected_vertices_boundary.clear();
	selected_faces_boundary.clear();
	selected_handle_vertices_boundary.clear();
	selected_handle_faces_boundary.clear();
	compute_boundary_vertices(selected_vertices, selected_vertices_boundary);
}

void select_and_update_handle_vertices(){
	selected_handle_faces.clear();
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

	if(selection_coordinates.size()==2){
		onetime_selected_vertices.insert(nearest_vertex_index);
		char buf[256];sprintf(buf, "%s%d", "Selected Handle Vertex Index: ", nearest_vertex_index);		
		selection_info->set_text(buf);
	}else{
		char buf[256];sprintf(buf, "%s", "Selected Handle Vertex Indices: Not Shown");
		selection_info->set_text(buf);
	}
	update_selected_elements(selected_handle_vertices, onetime_selected_vertices);
	selected_handle_vertices_boundary.clear();
	selected_handle_faces_boundary.clear();
	compute_boundary_vertices(selected_handle_vertices, selected_handle_vertices_boundary);
}

void select_and_update_faces(){
	selected_vertices.clear();
	selected_handle_vertices.clear();
	selected_handle_faces.clear();
	hash_set<int> onetime_selected_faces;
	update_gl_matrices();
	double min_dist=999999;
	int nearest_face_index=-1;
	for(int i=0;i<faces.size();i++){
		int v1_idx=faces[i][0];int v2_idx=faces[i][1];int v3_idx=faces[i][2];
		double pixel_x1;double pixel_y1;double pixel_z1;
		double pixel_x2;double pixel_y2;double pixel_z2;
		double pixel_x3;double pixel_y3;double pixel_z3;
		get_pixel_coordinates(vertices[v1_idx][0], vertices[v1_idx][1], vertices[v1_idx][2], pixel_x1, pixel_y1, pixel_z1);
		get_pixel_coordinates(vertices[v2_idx][0], vertices[v2_idx][1], vertices[v2_idx][2], pixel_x2, pixel_y2, pixel_z2);
		get_pixel_coordinates(vertices[v3_idx][0], vertices[v3_idx][1], vertices[v3_idx][2], pixel_x3, pixel_y3, pixel_z3);
		if(selection_coordinates.size()==2){
			double dist1=sqrt(sqr(pixel_x1-selection_coordinates[0])+sqr(pixel_y1-selection_coordinates[1]));
			double dist2=sqrt(sqr(pixel_x2-selection_coordinates[0])+sqr(pixel_y2-selection_coordinates[1]));
			double dist3=sqrt(sqr(pixel_x3-selection_coordinates[0])+sqr(pixel_y3-selection_coordinates[1]));
			double dist=dist1+dist2+dist3;
			if(dist<min_dist){
				min_dist=dist;
				nearest_face_index=i;
			}
		}else{
			bool is_inside1=is_inside_polygon(pixel_x1, pixel_y1);
			bool is_inside2=is_inside_polygon(pixel_x2, pixel_y2);
			bool is_inside3=is_inside_polygon(pixel_x3, pixel_y3);
			if(is_inside1&&is_inside2&&is_inside3){
				onetime_selected_faces.insert(i);
			}
		}
	}

	if(selection_coordinates.size()==2){
		onetime_selected_faces.insert(nearest_face_index);
		char buf[256];sprintf(buf, "%s%d", "Selected Face Index: ", nearest_face_index);		
		selection_info->set_text(buf);
	}else{
		char buf[256];sprintf(buf, "%s", "Selected Face Incides: Not Shown");
		selection_info->set_text(buf);
	}
	update_selected_elements(selected_faces, onetime_selected_faces);
	selected_vertices_boundary.clear();
	selected_faces_boundary.clear();
	selected_handle_vertices_boundary.clear();
	selected_handle_faces_boundary.clear();
	compute_boundary_faces(selected_faces, selected_faces_boundary);
}


void select_and_update_handle_faces(){
	selected_handle_vertices.clear();
	hash_set<int> onetime_selected_faces;
	update_gl_matrices();
	double min_dist=999999;
	int nearest_face_index=-1;
	for(hash_set<int>::iterator it=selected_faces.begin();it!=selected_faces.end();it++){
		int index=*it;
		int v1_idx=faces[index][0];int v2_idx=faces[index][1];int v3_idx=faces[index][2];
		double pixel_x1;double pixel_y1;double pixel_z1;
		double pixel_x2;double pixel_y2;double pixel_z2;
		double pixel_x3;double pixel_y3;double pixel_z3;
		get_pixel_coordinates(vertices[v1_idx][0], vertices[v1_idx][1], vertices[v1_idx][2], pixel_x1, pixel_y1, pixel_z1);
		get_pixel_coordinates(vertices[v2_idx][0], vertices[v2_idx][1], vertices[v2_idx][2], pixel_x2, pixel_y2, pixel_z2);
		get_pixel_coordinates(vertices[v3_idx][0], vertices[v3_idx][1], vertices[v3_idx][2], pixel_x3, pixel_y3, pixel_z3);
		if(selection_coordinates.size()==2){
			double dist1=sqrt(sqr(pixel_x1-selection_coordinates[0])+sqr(pixel_y1-selection_coordinates[1]));
			double dist2=sqrt(sqr(pixel_x2-selection_coordinates[0])+sqr(pixel_y2-selection_coordinates[1]));
			double dist3=sqrt(sqr(pixel_x3-selection_coordinates[0])+sqr(pixel_y3-selection_coordinates[1]));
			double dist=dist1+dist2+dist3;
			if(dist<min_dist){
				min_dist=dist;
				nearest_face_index=index;
			}
		}else{
			bool is_inside1=is_inside_polygon(pixel_x1, pixel_y1);
			bool is_inside2=is_inside_polygon(pixel_x2, pixel_y2);
			bool is_inside3=is_inside_polygon(pixel_x3, pixel_y3);
			if(is_inside1&&is_inside2&&is_inside3){
				onetime_selected_faces.insert(index);
			}
		}
	}

	if(selection_coordinates.size()==2){
		onetime_selected_faces.insert(nearest_face_index);
		char buf[256];sprintf(buf, "%s%d", "Selected Handle Face Index: ", nearest_face_index);		
		selection_info->set_text(buf);
	}else{
		char buf[256];sprintf(buf, "%s", "Selected Handle Face Indices: Not Shown");
		selection_info->set_text(buf);
	}
	update_selected_elements(selected_handle_faces, onetime_selected_faces);
	selected_handle_vertices_boundary.clear();
	selected_handle_faces_boundary.clear();
	compute_boundary_faces(selected_handle_faces, selected_handle_faces_boundary);
}*/
