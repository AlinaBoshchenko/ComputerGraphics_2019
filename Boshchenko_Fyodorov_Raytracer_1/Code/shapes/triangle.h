#ifndef RAY_TRIANGLE_H
#define RAY_TRIANGLE_H

#include "../object.h"

class Triangle : public Object {
public:
    Triangle(Point const &a, Point const &b, Point const &c);
    Triangle(Triangle const &another);

    virtual Hit intersect(Ray const &ray);

    Point const a, b, c;
};


#endif //RAY_TRIANGLE_H
