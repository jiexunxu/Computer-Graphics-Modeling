#include "Mesh.h"

// constructor
void Mesh::read_file(const char* filename){
	mesh=TriMesh::read(filename);
	mesh->need_faces();
	mesh->need_neighbors();
}

void Mesh::computeBoundingSphere(){
	int size=mesh->vertices.size();
	boundingSphere[0]=0.0;boundingSphere[1]=0.0;boundingSphere[2]=0.0;
	for(int i=0;i<size;i++){
		boundingSphere[0]=boundingSphere[0]+mesh->vertices[i][0];
		boundingSphere[1]=boundingSphere[1]+mesh->vertices[i][1];
		boundingSphere[2]=boundingSphere[2]+mesh->vertices[i][2];
	}
	boundingSphere[0]=boundingSphere[0]/size;
	boundingSphere[1]=boundingSphere[1]/size;
	boundingSphere[2]=boundingSphere[2]/size;
	
	double max=-9999999;
	for(int i=0;i<size;i++){
		double dist=pow(mesh->vertices[i][0]-boundingSphere[0], 2.0)+pow(mesh->vertices[i][1]-boundingSphere[1], 2.0)+
			pow(mesh->vertices[i][2]-boundingSphere[2], 2.0);
		if(dist>max){
			max=dist;
		}
	}
	boundingSphere[3]=sqrt(max);
}
// accessor functions
void Mesh::get_face(int index, TriMesh::Face& face){
	face=mesh->faces[index];
}

void Mesh::get_vertex(int index, point& p){
	p=mesh->vertices[index];
}

void Mesh::get_neighbors(int index, vector<int> &neighbors){
	neighbors=mesh->neighbors[index];
}

void Mesh::get_mesh_center_of_mass(double* CM){
	CM[0]=boundingSphere[0];
	CM[1]=boundingSphere[1];
	CM[2]=boundingSphere[2];
}

double Mesh::get_mesh_radius(){
	return boundingSphere[3];
}