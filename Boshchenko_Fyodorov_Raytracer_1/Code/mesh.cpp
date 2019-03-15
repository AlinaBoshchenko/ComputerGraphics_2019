#include "shapes/triangle.h"
#include "objloader.h"
#include "mesh.h"
#include <fstream>
#include <iostream>

#define OFFSET 300
#define SCALEFAC 50

Mesh::Mesh(std::string const filename) {

    OBJLoader obj(filename);

    std::vector<Vertex> data = obj.vertex_data();

    //add a new triangle for every 3 points
    for (unsigned long i = 0; i < data.size(); i += 3) {
        Point a{
                SCALEFAC * data.at(i).x + OFFSET,
                SCALEFAC * data.at(i).y + OFFSET,
                SCALEFAC * data.at(i).z + OFFSET,
        };
        Point b{
                SCALEFAC * data.at(i + 1).x + OFFSET,
                SCALEFAC * data.at(i + 1).y + OFFSET,
                SCALEFAC * data.at(i + 1).z + OFFSET,
        };
        Point c{
                SCALEFAC * data.at(i + 2).x + OFFSET,
                SCALEFAC * data.at(i + 2).y + OFFSET,
                SCALEFAC * data.at(i + 2).z + OFFSET,
        };

        Triangle triangle(a, b, c);
        triangles.push_back(triangle);
    }
}

const std::vector<Triangle> &Mesh::getTriangles() const {
    return triangles;
}
