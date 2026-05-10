#include "myVertex.h"
#include "myvector3d.h"
#include "myHalfedge.h"
#include "myFace.h"

myVertex::myVertex(void)
{
	point = NULL;
	originof = NULL;
	normal = new myVector3D(1.0,1.0,1.0);
}

myVertex::~myVertex(void)
{
	if (normal) delete normal;
}

void myVertex::computeNormal()
{
	this->normal->dX = 0;
	this->normal->dY = 0;
	this->normal->dZ = 0;

	myHalfedge *current = originof;
	myHalfedge *start = originof;

	while (true) {
		if (current->adjacent_face) {
			this->normal->dX += current->adjacent_face->normal->dX;
			this->normal->dY += current->adjacent_face->normal->dY;
			this->normal->dZ += current->adjacent_face->normal->dZ;
		}

		if (current->twin == NULL) {
			break;
		}

		current = current->twin->next;


		if (current == start) {
			break;
		}

	}
	this->normal->normalize();
}
