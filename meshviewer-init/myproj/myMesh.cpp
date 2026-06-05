#include "myMesh.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <utility>
#include <cmath>
#include <GL/glew.h>
#include "myvector3d.h"

using namespace std;

bool isConvexVertex(myVertex *vp, myVertex *v, myVertex *vn, myVector3D *N)
{
	double v1x = v->point->X - vp->point->X;
	double v1y = v->point->Y - vp->point->Y;
	double v1z = v->point->Z - vp->point->Z;

	double v2x = vn->point->X - v->point->X;
	double v2y = vn->point->Y - v->point->Y;
	double v2z = vn->point->Z - v->point->Z;

	double nx = v1y*v2z - v1z*v2y;
	double ny = v1z*v2x - v1x*v2z;
	double nz = v1x*v2y - v1y*v2x;

	double d = nx*N->dX + ny*N->dY + nz*N->dZ;
	if (d > 0) {
		return true;
	}
	else {
		return false;
	}
}

bool pointInTri(myVertex *p, myVertex *a, myVertex *b, myVertex *c, myVector3D *N)
{
	double ax = b->point->X - a->point->X;
	double ay = b->point->Y - a->point->Y;
	double az = b->point->Z - a->point->Z;
	double px = p->point->X - a->point->X;
	double py = p->point->Y - a->point->Y;
	double pz = p->point->Z - a->point->Z;
	double cx = ay*pz - az*py;
	double cy = az*px - ax*pz;
	double cz = ax*py - ay*px;
	double s1 = cx*N->dX + cy*N->dY + cz*N->dZ;

	ax = c->point->X - b->point->X;
	ay = c->point->Y - b->point->Y;
	az = c->point->Z - b->point->Z;
	px = p->point->X - b->point->X;
	py = p->point->Y - b->point->Y;
	pz = p->point->Z - b->point->Z;
	cx = ay*pz - az*py;
	cy = az*px - ax*pz;
	cz = ax*py - ay*px;
	double s2 = cx*N->dX + cy*N->dY + cz*N->dZ;

	ax = a->point->X - c->point->X;
	ay = a->point->Y - c->point->Y;
	az = a->point->Z - c->point->Z;
	px = p->point->X - c->point->X;
	py = p->point->Y - c->point->Y;
	pz = p->point->Z - c->point->Z;
	cx = ay*pz - az*py;
	cy = az*px - ax*pz;
	cz = ax*py - ay*px;
	double s3 = cx*N->dX + cy*N->dY + cz*N->dZ;

	if (s1 > 0 && s2 > 0 && s3 > 0) {
		return true;
	}
	else if (s1 < 0 && s2 < 0 && s3 < 0) {
		return true;
	}
	else {
		return false;
	}
	
}

myMesh::myMesh(void)
{
	/**** TODO ****/
}


myMesh::~myMesh(void)
{
	/**** TODO ****/
}

void myMesh::clear()
{
	for (unsigned int i = 0; i < vertices.size(); i++) if (vertices[i]) delete vertices[i];
	for (unsigned int i = 0; i < halfedges.size(); i++) if (halfedges[i]) delete halfedges[i];
	for (unsigned int i = 0; i < faces.size(); i++) if (faces[i]) delete faces[i];

	vector<myVertex *> empty_vertices;    vertices.swap(empty_vertices);
	vector<myHalfedge *> empty_halfedges; halfedges.swap(empty_halfedges);
	vector<myFace *> empty_faces;         faces.swap(empty_faces);
}

void myMesh::checkMesh()
{
	vector<myHalfedge *>::iterator it;
	for (it = halfedges.begin(); it != halfedges.end(); it++)
	{
		if ((*it)->twin == NULL)
			break;
	}
	if (it != halfedges.end())
		cout << "Error! Not all edges have their twins!\n";
	else cout << "Each edge has a twin!\n";
}


bool myMesh::readFile(std::string filename)
{
	string s, t, u;
	vector<int> faceids;
	myHalfedge **hedges;

	ifstream fin(filename);
	if (!fin.is_open()) {
		cout << "Unable to open file!\n";
		return false;
	}
	name = filename;

	map<pair<int, int>, myHalfedge *> twin_map;
	map<pair<int, int>, myHalfedge *>::iterator it;

	while (getline(fin, s))
	{
		if (s.empty()) {
			continue;
		}

		stringstream myline(s);
		t = "";
		myline >> t;
		if (t == "") {
			continue;
		}
		if (t == "g") {}
		else if (t == "v")
		{
			float x, y, z;
			myline >> x >> y >> z;
			myPoint3D *p = new myPoint3D(x, y, z);
			myVertex *v = new myVertex();

			v->point = p;
			
			vertices.push_back(v);
			
			cout << "v " << x << " " << y << " " << z << endl;
		}
		else if (t == "mtllib") {}
		else if (t == "usemtl") {}
		else if (t == "s") {}
		else if (t == "f")
		{
			faceids.clear();
			cout << "f"; 
			while (myline >> u) {
				cout << " " << atoi((u.substr(0, u.find("/"))).c_str());

				int index = atoi((u.substr(0, u.find("/"))).c_str()) - 1;
				faceids.push_back(index);
			}
			myFace* f = new myFace();
			faces.push_back(f);

			int nVertices = faceids.size();
			hedges = new myHalfedge *[nVertices];

			for (int i = 0; i < nVertices; i++) {
				hedges[i] = new myHalfedge();
				halfedges.push_back(hedges[i]);
				hedges[i]->adjacent_face = f;
				hedges[i]->source = vertices[faceids[i]];

				vertices[faceids[i]]->originof = hedges[i];

				if (i == 0) {
					f->adjacent_halfedge = hedges[i];
				}
				
			}

			for (int i = 0; i < nVertices; i++) {
				hedges[i]->next = hedges[(i + 1) % nVertices];
				hedges[i]->prev = hedges[(i + nVertices - 1) % nVertices];
			}

			for (int i = 0; i < nVertices; i++) {
				int vStart = faceids[i];
				int vEnd = faceids[(i + 1) % nVertices];

				it = twin_map.find({vEnd, vStart});

				if (it != twin_map.end()) {
					hedges[i]->twin = it->second;
					it->second->twin = hedges[i];
				}
				else {
					twin_map[{vStart, vEnd}] = hedges[i];
				}
			}
			
			delete[] hedges;
			cout << endl;
		}
	}
	computeNormals();
	checkMesh();
	normalize();

	return true;
}


void myMesh::computeNormals()
{
	for (int i = 0; i < faces.size(); i++) {
		if (faces[i]) {
			faces[i]->computeNormal();
		}
	}

	for (int i = 0; i < vertices.size(); i++) {
		if (vertices[i]) {
			vertices[i]->computeNormal();
		}
	}
}

void myMesh::normalize()
{
	if (vertices.size() < 1) return;

	int tmpxmin = 0, tmpymin = 0, tmpzmin = 0, tmpxmax = 0, tmpymax = 0, tmpzmax = 0;

	for (unsigned int i = 0; i < vertices.size(); i++) {
		if (vertices[i]->point->X < vertices[tmpxmin]->point->X) tmpxmin = i;
		if (vertices[i]->point->X > vertices[tmpxmax]->point->X) tmpxmax = i;

		if (vertices[i]->point->Y < vertices[tmpymin]->point->Y) tmpymin = i;
		if (vertices[i]->point->Y > vertices[tmpymax]->point->Y) tmpymax = i;

		if (vertices[i]->point->Z < vertices[tmpzmin]->point->Z) tmpzmin = i;
		if (vertices[i]->point->Z > vertices[tmpzmax]->point->Z) tmpzmax = i;
	}

	double xmin = vertices[tmpxmin]->point->X, xmax = vertices[tmpxmax]->point->X,
		ymin = vertices[tmpymin]->point->Y, ymax = vertices[tmpymax]->point->Y,
		zmin = vertices[tmpzmin]->point->Z, zmax = vertices[tmpzmax]->point->Z;

	double scale = (xmax - xmin) > (ymax - ymin) ? (xmax - xmin) : (ymax - ymin);
	scale = scale > (zmax - zmin) ? scale : (zmax - zmin);

	for (unsigned int i = 0; i < vertices.size(); i++) {
		vertices[i]->point->X -= (xmax + xmin) / 2;
		vertices[i]->point->Y -= (ymax + ymin) / 2;
		vertices[i]->point->Z -= (zmax + zmin) / 2;

		vertices[i]->point->X /= scale;
		vertices[i]->point->Y /= scale;
		vertices[i]->point->Z /= scale;
	}
}


void myMesh::splitFaceTRIS(myFace* f, myPoint3D* p)
{
	/**** TODO ****/
}

void myMesh::splitEdge(myHalfedge *e1, myPoint3D *p)
{

	/**** TODO ****/
}

void myMesh::splitFaceQUADS(myFace *f, myPoint3D *p)
{
	/**** TODO ****/
}


void myMesh::subdivisionCatmullClark()
{
	/**** TODO ****/
}


void myMesh::SurfaceOfRevolution()
{
	int n = 24;
	double pi = 3.14159;

	vector< pair<double,double> > profile;
	profile.push_back( make_pair(0.5, -1.0));
	profile.push_back( make_pair(0.5,  1.0));
	int k = (int)profile.size();

	for (int ring = 0; ring < k; ring++)
	{
		double r = profile[ring].first;
		double y = profile[ring].second;

		for (int slice = 0; slice < n; slice++)
		{
			double angle = 2.0 * pi * (double)slice / (double)n;
			double x = r * cos(angle);
			double z = r * sin(angle);

			myPoint3D *p = new myPoint3D(x, y, z);
			myVertex *v = new myVertex();
			v->point = p;
			vertices.push_back(v);
		}
	}

	map< pair<int,int>, myHalfedge* > twin_map;
	map< pair<int,int>, myHalfedge* >::iterator it;

	for (int ring = 0; ring < k - 1; ring++)
	{
		for (int slice = 0; slice < n; slice++)
		{
			int sliceNext = (slice + 1) % n;

			int v00 = ring * n + slice;
			int v01 = ring * n + sliceNext;
			int v10 = (ring + 1) * n + slice;
			int v11 = (ring + 1) * n + sliceNext;

			int triangles[2][3] = {
				{v00, v01, v11},
				{v00, v11, v10}
			};

			for (int t = 0; t < 2; t++)
			{
				myFace *f = new myFace();
				faces.push_back(f);

				myHalfedge *hedges[3];
				for (int i = 0; i < 3; i++)
				{
					hedges[i] = new myHalfedge();
					halfedges.push_back(hedges[i]);
					hedges[i]->adjacent_face = f;
					hedges[i]->source = vertices[triangles[t][i]];
					vertices[triangles[t][i]]->originof = hedges[i];

					if (i == 0) f->adjacent_halfedge = hedges[i];
				}

				for (int i = 0; i < 3; i++)
				{
					hedges[i]->next = hedges[(i + 1) % 3];
					hedges[i]->prev = hedges[(i + 2) % 3];
				}

				for (int i = 0; i < 3; i++)
				{
					int vStart = triangles[t][i];
					int vEnd   = triangles[t][(i + 1) % 3];

					it = twin_map.find(make_pair(vEnd, vStart));
					if (it != twin_map.end())
					{
						hedges[i]->twin = it->second;
						it->second->twin = hedges[i];
					}
					else
					{
						twin_map[make_pair(vStart, vEnd)] = hedges[i];
					}
				}
			}
		}
	}

	computeNormals();
	checkMesh();
	normalize();
}


void myMesh::triangulate()
{
	int nbFacesInitial = faces.size();
	for (int i = 0; i < nbFacesInitial; i++) {
		triangulate(faces[i]);
	}
}

void myMesh::cutEar(myHalfedge *e)
{
	myHalfedge* prev = e->prev;
	myHalfedge* nxt = e->next;
	myHalfedge* before = prev->prev;

	myVertex* vprev = prev->source;
	myVertex* vnext = nxt->source;

	myFace* oldFace = e->adjacent_face;

	myFace* tri = new myFace();
	tri->index = faces.size();
	faces.push_back(tri);

	myHalfedge* d1 = new myHalfedge();
	myHalfedge* d2 = new myHalfedge();
	halfedges.push_back(d1);
	halfedges.push_back(d2);

	d1->source = vnext;
	d2->source = vprev;
	d1->twin = d2;
	d2->twin = d1;

	d1->adjacent_face = tri;
	prev->adjacent_face = tri;
	e->adjacent_face = tri;
	tri->adjacent_halfedge = prev;

	prev->next = e;
	e->prev = prev;
	e->next = d1;
	d1->prev = e;
	d1->next = prev;
	prev->prev = d1;

	d2->adjacent_face = oldFace;
	d2->prev = before;
	before->next = d2;
	d2->next = nxt;
	nxt->prev = d2;

	if (oldFace->adjacent_halfedge == prev || oldFace->adjacent_halfedge == e) {
		oldFace->adjacent_halfedge = d2;
	}
}

bool myMesh::triangulate(myFace* f)
{
	int n = 0;
	myHalfedge* start = f->adjacent_halfedge;
	myHalfedge* current = start;

	do {
		n++;
		current = current->next;
	} while (current != start);


	if (n <= 3) {
		return false;
	}


	double aire = 0;
	myHalfedge* hc = f->adjacent_halfedge;
	for (int i = 0; i < n; i++) {
		myPoint3D* p0 = hc->prev->source->point;
		myPoint3D* p1 = hc->source->point;
		myPoint3D* p2 = hc->next->source->point;

		double ax = p1->X - p0->X;
		double ay = p1->Y - p0->Y;
		double az = p1->Z - p0->Z;
		double bx = p2->X - p1->X;
		double by = p2->Y - p1->Y;
		double bz = p2->Z - p1->Z;
		double cx = ay*bz - az*by;
		double cy = az*bx - ax*bz;
		double cz = ax*by - ay*bx;

		aire += cx*f->normal->dX + cy*f->normal->dY + cz*f->normal->dZ;
		hc = hc->next;
	}

	cout << "aire = " << aire << endl;

	myVector3D nrml;
	if (aire < 0) {
		nrml.dX = - f->normal->dX;
		nrml.dY = - f->normal->dY;
		nrml.dZ = - f->normal->dZ;
	}
	else {
		nrml.dX = f->normal->dX;
		nrml.dY = f->normal->dY;
		nrml.dZ = f->normal->dZ;
	}


	while (n > 3) {
		myHalfedge* ear = NULL;
		myHalfedge* e = f->adjacent_halfedge;

		for (int k = 0; k < n; k++) {
			myVertex* vp = e->prev->source;
			myVertex* v = e->source;
			myVertex* vn = e->next->source;

			if (isConvexVertex(vp, v, vn, &nrml) == false) {
				e = e->next;
				continue;
			}

			bool ok = true;
			myHalfedge* o = e->next->next;
			while (o != e->prev) {
				if (pointInTri(o->source, vp, v, vn, &nrml)) {
					ok = false;
					break;
				}
				o = o->next;
			}

			if (ok == true) {
				ear = e;
				break;
			}
			e = e->next;
		}
		cutEar(ear);
		cout << n - 1 << " somet restant" << endl;
		n = n - 1;
	}

	cout << "fin triangulate face" << endl;
	return true;
}


void myMesh::simplification()
{
	myHalfedge* shortest = NULL;
	double mini = 99999999.0;

	for (int i = 0; i < halfedges.size(); i++) {
		myHalfedge* e = halfedges[i];
		myPoint3D* p1 = e->source->point;
		myPoint3D* p2 = e->twin->source->point;

		double dx = p2->X - p1->X;
		double dy = p2->Y - p1->Y;
		double dz = p2->Z - p1->Z;
		double len = sqrt(dx*dx + dy*dy + dz*dz);

		if (len < mini) {
			mini = len;
			shortest = e;
		}
	}

	cout << mini << endl;

	collapseEdge(shortest);
	computeNormals();
}

void myMesh::collapseEdge(myHalfedge* e)
{
	myHalfedge* et = e->twin;

	myFace* f1 = e->adjacent_face;
	myFace* f2 = et->adjacent_face;

	myHalfedge* a1 = e->next;
	myHalfedge* b1 = e->next->next;
	myHalfedge* a2 = et->next;
	myHalfedge* b2 = et->next->next;

	myVertex* v1 = e->source;
	myVertex* v2 = et->source;
	myVertex* v3 = b1->source;
	myVertex* v4 = b2->source;

	double mx = (v1->point->X + v2->point->X) / 2.0;
	double my = (v1->point->Y + v2->point->Y) / 2.0;
	double mz = (v1->point->Z + v2->point->Z) / 2.0;
	v1->point->X = mx;
	v1->point->Y = my;
	v1->point->Z = mz;

	for (int i = 0; i < halfedges.size(); i++) {
		if (halfedges[i]->source == v2) {
			halfedges[i]->source = v1;
		}
	}

	a1->twin->twin = b1->twin;
	b1->twin->twin = a1->twin;
	a2->twin->twin = b2->twin;
	b2->twin->twin = a2->twin;

	v1->originof = b1->twin;
	v3->originof = a1->twin;
	v4->originof = a2->twin;

	for (int i = halfedges.size() - 1; i >= 0; i--) {
		myHalfedge* h = halfedges[i];
		if (h == e || h == et || h == a1 || h == b1 || h == a2 || h == b2) {
			delete h;
			halfedges.erase(halfedges.begin() + i);
		}
	}

	for (int i = faces.size() - 1; i >= 0; i--) {
		if (faces[i] == f1 || faces[i] == f2) {
			delete faces[i];
			faces.erase(faces.begin() + i);
		}
	}

	for (int i = vertices.size() - 1; i >= 0; i--) {
		if (vertices[i] == v2) {
			delete vertices[i];
			vertices.erase(vertices.begin() + i);
		}
	}




}

