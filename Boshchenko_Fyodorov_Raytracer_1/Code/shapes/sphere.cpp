#include "sphere.h"

#include <cmath>

using namespace std;

Hit Sphere::intersect(Ray const &ray) {
    Vector L = ray.O - position; // Direction from the sphere  center towards the origin of the ray
    double a = ray.D.dot(ray.D),
            b = 2 * ray.D.dot(L),
            c = (L).dot(L) - r * r;
    double delta = b * b - 4 * a * c;
    if (delta < 0) {
        return Hit::NO_HIT();
    }
    double t1 = (-b - sqrt(delta)) / (2.0 * a),
            t2 = (-b + sqrt(delta)) / (2.0 * a);
    if (t1 < 0 && t2 < 0) {
        return Hit::NO_HIT();
    }
    double t;
    if (t1 < 0) {
        t = t2;
    } else if (t2 < 0) {
        t = t1;
    } else {
        t = min(t1, t2);
    }
    Point hit = ray.O + t * ray.D;
    Vector N = (hit - position).normalized();
    return Hit(t, N);
}

Sphere::Sphere(Point const &pos, double radius)
        :
        position(pos),
        r(radius) {}
