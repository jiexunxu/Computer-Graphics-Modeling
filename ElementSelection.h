/*#include "global.h"

// coordinates of selection polygons. 2n-1 are x-coordinates, 2n are y-coordinates. ith point coordinate is (2i, 2i+1)
vector<int> selection_coordinates;
hash_set<int> selected_vertices;
hash_set<int> selected_vertices_boundary;
hash_set<int> selected_faces;
hash_set<int> selected_faces_boundary;
hash_set<int> selected_handle_vertices;
hash_set<int> selected_handle_vertices_boundary;
hash_set<int> selected_handle_faces;
hash_set<int> selected_handle_faces_boundary;


bool contains(hash_set<int> &set, int key);
void update_gl_matrices();
void get_pixel_coordinates(double x, double y, double z, double &pixel_x, double &pixel_y, double &pixel_z);
// Use ray casting algorithm to test whether a given point (x0, y0) is inside the polygon selection_vertices.
// The ray starts at (x0, y0) and shoots to (+inf, y0)
bool is_inside_polygon(double x0, double y0);
void update_selected_elements(hash_set<int> &selected_elements, hash_set<int> &onetime_selected_elements);
void updateDisplayList();
void compute_vertices_from_faces(hash_set<int> &input_face_indices, hash_set<int> &returned_vertex_indices);
void compute_boundary_vertices(hash_set<int> &selected_vertices, hash_set<int> &boundary);
void compute_boundary_faces(hash_set<int> &selected_faces, hash_set<int> &boundary);
void dialate_vertices(hash_set<int> &selected_vertices, hash_set<int> &boundary);
void dialate_faces(hash_set<int> &selected_faces, hash_set<int> &boundary);
void dialate_selections(int waste);
void erode_vertices(hash_set<int> &selected_vertices, hash_set<int> &boundary);
void erode_faces(hash_set<int> &selected_faces, hash_set<int> &boundary);
void erode_selections(int waste);
void select_and_update_vertices();
void select_and_update_handle_vertices();
void select_and_update_faces();
void select_and_update_handle_faces();*/