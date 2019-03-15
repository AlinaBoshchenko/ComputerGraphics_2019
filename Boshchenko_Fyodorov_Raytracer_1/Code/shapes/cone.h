#ifndef RAY_CONE_H
#define RAY_CONE_H

#include "../object.h"

class Cone : public Object {
public:
    Cone(Point const &C, Vector const &V, double theta);
    Hit getHit(const Ray &ray, double t) const;
    virtual Hit intersect(Ray const &ray);

    Point C;
    Vector V;
    double theta;
};


#endif //RAY_CONE_H
