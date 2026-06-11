# Geometry Modeling — TP

Implementation of the methods required by the Geometry Modeling course.

## Tasks

- [x] **readFile**
- [x] **computeNormals** 
- [x] **Silhouette**
- [x] **Triangulation fan clipping**
- [x] **Surface of revolution**
- [x] **Triangulation ear clipping**
- [x] **Mesh simplification**
- [ ] **Triangulation (expert: polygons with holes)**
- [x] **Catmull-Clark subdivision**
- [ ] **Loop algorithm**

## Usage of AI

### General
- Clarifying parts of the course slides and the TDs that were not clear to me

### readFile
- How to use the syntax std::map<pair<int,int>, myHalfedge*>

### Surface of revolution
- The parametric formula x = r * cos(angle), z = r * sin(angle) with angle = 2 * pi * slice / n for placing each vertex of a ring.
- The diagonal pattern used to split each quad of the strip into two triangles ({v00, v01, v11} then {v00, v11, v10}), so the normals of both triangles end up on the same side.

### Triangulation ear clipping
- Problem with the order in which to update next, prev, twin and adjacent_face when inserting a new diagonal

### Mesh simplification
- Adding the Simplification entry in the main.cpp switch


