#include "triangle.h"

Hit Triangle::intersect(Ray const &ray) {
    Vector ab = b - a, ac = c - a;
    Vector pvec = ray.D.cross(ac);
    double determinant = ab.dot(pvec);
    if (determinant > -EPSILON && determinant < EPSILON) {
        return Hit::NO_HIT();
    }

    double invDeterminant = 1.0 / determinant;
    Vector tvec = ray.O - a;
    double u = tvec.dot(pvec) * invDeterminant;
    if (u < 0.0 || u > 1.0) {
        return Hit::NO_HIT();
    }

    Vector qvec = tvec.cross(ab);
    double v = invDeterminant * ray.D.dot(qvec);
    if (v < 0.0 || u + v > 1.0) {
        return Hit::NO_HIT();
    }

    double t = invDeterminant * ac.dot(qvec);
    if (t > EPSILON) {
        Point hit = ray.O + t * ray.D;
        Vector N = hit.normalized();
        return Hit(t, N);
    }
    return Hit::NO_HIT();
}

Triangle::Triangle(Point const &a, Point const &b, Point const &c) : a(a), b(b), c(c) {}

Triangle::Triangle(Triangle const &another) : a(another.a), b(another.b), c(another.c) {}