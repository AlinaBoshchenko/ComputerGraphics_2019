#ifndef RAY_MESH_H
#define RAY_MESH_H

#include "shapes/triangle.h"
#include "vertex.h"
#include <fstream>
#include <iostream>
#include <string>

class Mesh {
public:
    Mesh(std::string filename);
    const std::vector<Triangle> &getTriangles() const;
private:
    std::vector<Triangle> triangles;
};

#endif //RAY_MESH_H
