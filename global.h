/*#include <string.h>
#include <string>
#include <sstream>
#include "Sphere.h"
#include <math.h>
#include "newmat.h"
#include "newmatap.h"
#include <hash_set>
using namespace std;
using namespace stdext;

int window_width=600;
int window_height=600;

vector<double> debug;

TriMesh* mesh;
GLUI* glui;
GLUI_StaticText *selection_info;
double boundingSphere[4];
double handleBoundingSphere[4];
vector<point> vertices;
vector<vec> normals;
vector<TriMesh::Face> faces;
vector<vector<int>> neighbors;
vector<vector<int>> adjacent_faces;
GLuint display_list;
GLuint wireframe_list;

double x_pos=0.0;
double y_pos=0.0;
double z_pos=0.0;
double zoom_factor=0.5;

int selection_is_enabled=0;
int face_selection_is_on=0;
int face_handle_is_on=0;
int handle_selection_is_on=0;

int show_mesh=1;
int show_wireframe=0;
int selection_tool_ID=0;
int boolean_selection_ID=0;
int handle_tool_ID=0;
int boolean_handle_ID=0;

GLfloat x_rotation_matrix[16];
GLfloat y_rotation_matrix[16];
GLfloat z_rotation_matrix[16];

GLfloat handle_x_rotation_matrix[16];
GLfloat handle_y_rotation_matrix[16];
GLfloat handle_z_rotation_matrix[16];
GLfloat handle_x_translation[1];
GLfloat handle_y_translation[1];
GLfloat handle_z_translation[1];
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
};
int cur_status=Status::IDLE;

Sphere handle_sphere(24, 24);

/*void glutMouseClickFunction(int button, int button_status, int x, int y);
void glutMouseMotionFunction(int x, int y);
void update_gl_matrices();
bool contains(hash_set<int> &set, int key);
void draw_small_diamond(double v1, double v2, double v3, double radius);
void draw_selected_vertices();
void draw_selected_handle_vertices();
void draw_selected_faces();
void draw_selected_handle_faces();
void draw_handle_transformation_tool();
void draw_selection_tool();
void drawGraphics();
void get_pixel_coordinates(double x, double y, double z, double &pixel_x, double &pixel_y, double &pixel_z);
void update_selected_elements(hash_set<int> &selected_elements, hash_set<int> &onetime_selected_elements);
void select_and_update_vertices();
void select_and_update_handle_vertices();
void select_and_update_faces();
void select_and_update_handle_faces();
void updateDisplayList();
void compute_vertices_from_faces(hash_set<int> &input_face_indices, hash_set<int> &returned_vertex_indices);
void compute_handle_bounding_sphere(hash_set<int> &handle_vertices);
void computeBoundingSphere();
void init(int argc, char *argv[], const char* filename);
int main(int argc, char *argv[]);*/