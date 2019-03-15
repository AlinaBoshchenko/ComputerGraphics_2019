#ifndef RAY_CYLINDER_H
#define RAY_CYLINDER_H

#include "../object.h"

/**
 * Defines a simple Sphere located at 'center'
 * with the specified radius
 */
class Cylinder : public Object {
public:
    Cylinder(Point const &center, double radius, double height);
    Hit intersect(Ray const &ray);

    Point center;
    double radius, height;
};


#endif //RAY_CYLINDER_H
