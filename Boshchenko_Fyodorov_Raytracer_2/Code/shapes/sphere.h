#ifndef SPHERE_H_
#define SPHERE_H_

#include "../object.h"

class Sphere : public Object {
public:
    Sphere(Point const &pos, double radius);

    Sphere(Point const &pos, double radius, Vector const &rotation, int angle);

    virtual Hit intersect(Ray const &ray);

    double degToRad(int degrees);

    Point rotatedPoint(Point const &surface_point);

    virtual std::pair<double, double> mapTextureCoord(Point &surface_point);

    bool is_rotated = false;
    Point const center;
    double const radius;
    Vector rotation;
    double angle_rad;

};

#endif
