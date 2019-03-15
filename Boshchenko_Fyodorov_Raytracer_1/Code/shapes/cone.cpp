#include <cmath>
#include "cone.h"

double cosd(double radians) {
    return cos(fmod((radians), 360) * M_PI / 180);
}

Hit Cone::intersect(Ray const &ray) {
    Vector CO = ray.O - C;
    double a = pow(ray.D.dot(V), 2) - pow(cosd(theta), 2),
            b = 2 * (ray.D.dot(V) * CO.dot(V) - ray.D.dot(CO) * pow(cosd(theta), 2)),
            c = pow(CO.dot(V), 2) - CO.dot(CO) * pow(cosd(theta), 2);

    double delta = b * b - 4 * a * c;
    if (delta < 0) {
        return Hit::NO_HIT();
    }
    double t;
    if (delta < EPSILON) {
        t = -b / 2 * a;
        getHit(ray, t);
    }
    double t1 = (-b - sqrt(delta)) / (2.0 * a),
            t2 = (-b + sqrt(delta)) / (2.0 * a);
    t = std::min(t1, t2);
    return getHit(ray, t);
}

Hit Cone::getHit(const Ray &ray, double t) const {
    Point P = ray.O + t * ray.D;
    if (theta < 90) {
        if ((P - C).dot(V) <= EPSILON) {
            return Hit::NO_HIT();
        }
    }
    return Hit(t, P.normalized());
}

Cone::Cone(Point const &C, Vector const &V, double theta) : C(C), V(V), theta(theta) {}
