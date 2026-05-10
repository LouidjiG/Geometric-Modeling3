#include "myFace.h"
#include "myvector3d.h"
#include "myHalfedge.h"
#include "myVertex.h"
#include <GL/glew.h>
#include <cmath>

myFace::myFace(void)
{
	adjacent_halfedge = NULL;
	normal = new myVector3D(1.0, 1.0, 1.0);
}

myFace::~myFace(void)
{
	if (normal) delete normal;
}

void myFace::computeNormal()
{
	//logique calcul normal
	// prendre 2 vecteurs de la face 
	// calculer produit vectoriel
	// normalisation

    myPoint3D* p1 = adjacent_halfedge->source->point;
    myPoint3D* p2 = adjacent_halfedge->next->source->point;
    myPoint3D* p3 = adjacent_halfedge->next->next->source->point;

    double v1x = p2->X - p1->X;
    double v1y = p2->Y - p1->Y;
    double v1z = p2->Z - p1->Z;
    double v2x = p3->X - p2->X;
    double v2y = p3->Y - p2->Y;
    double v2z = p3->Z - p2->Z;

    double nx = v1y * v2z - v1z * v2y;
    double ny = v1z * v2x - v1x * v2z;
    double nz = v1x * v2y - v1y * v2x;

    double length = sqrt(nx * nx + ny * ny + nz * nz);

    this->normal->dX = nx / length;
    this->normal->dY = ny / length;
    this->normal->dZ = nz / length;
}
