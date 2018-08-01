#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h> 
#include <string.h>
#include "TriMesh.h"
#include "newmat.h"
#include "newmatap.h"
#include <gl/glut.h>
#include <gl/glui.h>
#include <string>
#include <math.h>
#include <hash_set>
using namespace std;
using namespace stdext;

int window_width=600;
int window_height=600;


TriMesh* mesh;

GLUI* glui;

class Status{
	public:
		const static int IDLE=1;
		const static int ZOOMING=2;
		const static int SINGLE_CLICK_SELECTION_ACTIVE=3;
		const static int RECTANGLE_SELECTION_ACTIVE=4;
		const static int LASSO_SELECTION_ACTIVE=5;
};
int cur_status=Status::IDLE;

vector<double> debug;

double boundingSphere[4];

int is_wireframe_composite=0;
GLfloat x_rotation_matrix[16];
GLfloat y_rotation_matrix[16];
GLfloat z_rotation_matrix[16];
double zoom_factor=0.5;

int selection_tool_ID=0;
int boolean_selection_ID=0;

vector<point> vertices;
vector<vec> normals;
vector<TriMesh::Face> faces;

// coordinates of selection polygons. 2n-1 are x-coordinates, 2n are y-coordinates. ith point coordinate is (2i, 2i+1)
vector<int> selection_coordinates;
vector<int> selected_vertices;

GLuint display_list;
GLuint wireframe_list;


// Functions
void drawGraphics();
void glutMouseMotionFunction(int x, int y);
void glutMouseClickFunction(int button, int button_status, int x, int y);
void gluiAndglutInit(int argc, char *argv[]);

void get_pixel_coordinates(double x, double y, double z, double &pixel_x, double &pixel_y);
bool is_inside_polygon(double x0, double y0);
void update_selected_vertices();