#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h> 
#include <string.h>
#include "TriMesh.h"
#include <string>
#include <math.h>
using namespace std;

class Mesh{
	private:
		TriMesh* mesh;
		double boundingSphere[4];
		vector<point> vertices;
		vector<TriMesh::Face> faces;
		vector<vec> normals;
		vector< vector<int> > neighbors;
	public:
		void read_file(const char*);
		void get_face(int, TriMesh::Face&);
		void get_vertex(int, point&);
		void get_neighbors(int, vector<int>&);
		void get_mesh_center_of_mass(double*);
		double get_mesh_radius();
		void computeBoundingSphere();
};