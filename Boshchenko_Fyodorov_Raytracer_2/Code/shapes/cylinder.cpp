#include "cylinder.h"
#include <math.h>

Hit Cylinder::intersect(Ray const &ray) {
    double a = (ray.D.x * ray.D.x) + (ray.D.z * ray.D.z);
    double b = 2 * (ray.D.x * (ray.O.x - center.x) + ray.D.z * (ray.O.z - center.z));
    double c = (ray.O.x - center.x) * (ray.O.x - center.x) + (ray.O.z - center.z) * (ray.O.z - center.z) -
               (radius * radius);

    double delta = b * b - 4 * (a * c);
    if (fabs(delta) < 0.001) return Hit::NO_HIT();
    if (delta < 0.0) return Hit::NO_HIT();

    double t1 = (-b - sqrt(delta)) / (2 * a);
    double t2 = (-b + sqrt(delta)) / (2 * a);
    double t;

    if (t1 > t2) t = t2;
    else t = t1;

    double r = ray.O.y + t * ray.D.y;
    Point hit = (ray.O + t * ray.D).normalized();

    if (r >= center.y and r <= center.y + height) {
        return Hit(t, hit);
    }
    return Hit::NO_HIT();
}

Cylinder::Cylinder(Point const &center, double radius, double height) : center(center), radius(radius),
                                                                        height(height) {}