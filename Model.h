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

static int window_width=600;
static int window_height=600;

static vector<double> debug;

static TriMesh* mesh;
static GLUI* glui;
static GLUI_StaticText *selection_info;
static double boundingSphere[4];
static double handleBoundingSphere[4];
static vector<point> vertices;
static vector<vec> vertex_normals;
static vector<vec> face_normals;
static vector<TriMesh::Face> faces;
static vector<vector<int>> neighbors;
static vector<vector<int>> adjacent_faces;

static GLuint mesh_init_list;
static GLuint mesh_display_list;
static GLuint wireframe_display_list;
static vector<int> updatable_faces;

static double x_pos=0.0;
static double y_pos=0.0;
static double z_pos=0.0;
static double zoom_factor=0.5;

static int selection_is_enabled=0;
static int face_selection_is_on=0;
static int face_handle_is_on=0;
static int handle_selection_is_on=0;

static int view_in_perspective_mode=0;
static int show_mesh=1;
static int show_wireframe=0;
static int selection_tool_ID=0;
static int boolean_selection_ID=0;
static int handle_tool_ID=0;
static int boolean_handle_ID=0;
static int show_handle_tool=0;
static int handle_manipulation_enabled=0;

static GLfloat x_rotation_matrix[16];
static GLfloat y_rotation_matrix[16];
static GLfloat z_rotation_matrix[16];
static GLfloat x_translation[1];
static GLfloat y_translation[1];
static GLfloat z_translation[1];

static GLfloat handle_x_rotation_matrix[16];
static GLfloat handle_y_rotation_matrix[16];
static GLfloat handle_z_rotation_matrix[16];
static GLfloat handle_x_translation[1];
static GLfloat handle_y_translation[1];
static GLfloat handle_z_translation[1];
static double handle_scale_factor;

static class Status{
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
static int cur_status=Status::IDLE;

// coordinates of selection polygons. 2n-1 are x-coordinates, 2n are y-coordinates. ith point coordinate is (2i, 2i+1)
static vector<int> selection_coordinates;
static hash_set<int> selected_vertices;
static hash_set<int> selected_vertices_boundary;
static hash_set<int> selected_faces;
static hash_set<int> selected_faces_boundary;
static hash_set<int> selected_handle_vertices;
static hash_set<int> selected_handle_vertices_boundary;
static hash_set<int> selected_handle_faces;
static hash_set<int> selected_handle_faces_boundary;

static vector<int> active_vertices;
static vector<int> active_faces;

static Sphere handle_sphere(24, 24);
*/