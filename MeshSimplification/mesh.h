
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <queue>
#include <deque>
#include <stack>
#include <glut.h>
using namespace std;

struct HalfEdge;
struct Vertex;
struct Face;
struct Edge;
struct TexCoord;

typedef list<Vertex>::iterator  VertexIter;
typedef list<Face>::iterator    FaceIter;
typedef list<Edge>::iterator    EdgeIter;
typedef vector<TexCoord>::iterator TexCoordIter;


struct HalfEdge {
    HalfEdge *next, *prev;
    HalfEdge *mate;

    VertexIter vertex;  // halfEdge的终点
    FaceIter   face;
    EdgeIter   edge;

    HalfEdge(){
        next = prev = NULL;
        mate = NULL;
    }
};

struct Edge {
    HalfEdge *halfedge[2]; // if boundary edge, halfedge[1] == NULL
    int id;

    int ect_id; // id of corresponding "EdgeCollapseTarget"

    bool isActive;

    Edge(){}
    Edge(HalfEdge *he0, HalfEdge *he1, int n){
        halfedge[0] = he0; 
        halfedge[1] = he1;
        id = n;
        isActive = true;
    };
};


struct Vertex {
    double coord[3];
    double normal[3];
	
    int id;
    // pointer to one of the halfedges incident to this vertex
    HalfEdge *neighborHe;  //以该点为起点的halfEdge？？one of

    bool isBoundary;
    bool isActive;

    double Q[10];

    Vertex(){};
    Vertex(double *coord_in, int n){
        for(int i=0; i<3; i++) coord[i] = coord_in[i];
        id = n;
        neighborHe = NULL;
        isBoundary = false;
        isActive = true;
    }
};

struct TexCoord {
	double coord[2];
	int id;

	TexCoord(double *coord_in, int id_in) {
		for (int i = 0; i < 2; i++)
			coord[i] = coord_in[i];
		id = id_in;
	}
};


struct Face {
    //VertexIter vertex[3];
    HalfEdge   halfedge[3]; // circular list ex) halfedge[0].next == halfedge[1]

    double normal[3];
	//double texCoord[3];
	int texCoordIndex[3];
    double area;
    int id;

    bool isActive;

    Face(){}
    Face(VertexIter v0, VertexIter v1, VertexIter v2, int n){
        halfedge[0].vertex = v0;
        halfedge[1].vertex = v1;
        halfedge[2].vertex = v2;
        id = n;
        isActive = true;
    }
	Face(VertexIter v0, VertexIter v1, VertexIter v2, int n, int *texCoordIndex_in) {
		halfedge[0].vertex = v0;
		halfedge[1].vertex = v1;
		halfedge[2].vertex = v2;
		id = n;
		isActive = true;

		for (int i = 0; i < 3; i++) {
			texCoordIndex[i] = texCoordIndex_in[i];
		}
	}
};


class Mesh {
protected:

    bool ReadOFFFile(char *filename);
	bool ReadOBJFile(char *filename);
    void AddEdgeInfo();
    void MakeCircularList(FaceIter &fi);

public:
    
    list<Vertex> vertices;
    list<Face>   faces;
    list<Edge>   edges;

	vector<TexCoord> texturecoords;

    int n_vertices, n_faces, n_edges, n_texturecoords;


    Mesh(){
        n_vertices = n_faces = n_edges = 0;
    }

    bool ConstructMeshDataStructure(char *filename);
    void AssignFaceNormal(FaceIter &fi);
    void AssignVertexNormal(VertexIter &vi);
	void AssignVertexNormal(VertexIter &vi, int i);
    void Display(int mode);
	void OutputOBJ(string filename);
    //void Picking(int& x, int& y);
};

extern void GLInit();
extern void CrossProduct( double *a, double *b, double *c );
extern void Normalize(double *a);
extern double DotProduct(double *a, double *b);
extern double DotProduct4D(double *a, double *b);
extern void GetEdgeVector(double *v1, double *v2, double *edge_vector);
extern void GetArea(double *normal, double &area);
extern double GetDistance(double *a, double *b);
extern double GetLength(double *a);
extern void Swap(double &a, double &b);
extern bool SolveLinearSystem(double (*matrix)[4], double *rhs, double *solution);

#define EPSILON 1.0e-6
