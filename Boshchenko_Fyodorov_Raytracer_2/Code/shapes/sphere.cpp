#include "sphere.h"

#include <cmath>

using namespace std;

Hit Sphere::intersect(Ray const &ray) {
    Vector L = ray.O - center; // Direction from the sphere  center towards the origin of the ray
    double a = ray.D.dot(ray.D),
            b = 2 * ray.D.dot(L),
            c = L.dot(L) - radius * radius;
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
    if (t < EPSILON) {
        // Without such check, there will be a grainy picture because of a floating-point accuracy problem.
        return Hit::NO_HIT();
    }
    Point hit = ray.at(t);
    Vector N = (hit - center).normalized();
    return Hit(t, N);
}

double Sphere::degToRad(int degrees) {
    return degrees * M_PI / 180;
}

Point Sphere::rotatedPoint(Point const &surface_point) {
    Vector vector_to_surface = surface_point - center;
    // Rodrigues' rotation is applied.
    Vector rotated_vector = vector_to_surface * cos(angle_rad) +
                            rotation.cross(vector_to_surface) * sin(angle_rad) +
                            rotation * rotation.dot(vector_to_surface) * (1 - cos(angle_rad));
    Point rotated_point = rotated_vector + center;
    return rotated_point;
}

std::pair<double, double> Sphere::mapTextureCoord(Point &surface_point) {
    Point rotated_point = is_rotated ? rotatedPoint(surface_point) : surface_point;

    double theta = acos((rotated_point.z - center.z) / radius),
            phi = atan2(rotated_point.y - center.y, rotated_point.x - center.x);
    double u = phi / (2 * M_PI),
            v = (M_PI - theta) / M_PI;

    return std::make_pair(u, v);
}

Sphere::Sphere(Point const &pos, double radius)
        :
        center(pos),
        radius(radius) {
    rotation = Vector(0, 0, 1);
    angle_rad = 0;
}

Sphere::Sphere(Point const &pos, double radius, Vector const &rotation, int angle)
        :
        center(pos),
        radius(radius) {
    this->rotation = rotation.normalized();
    angle_rad = degToRad(angle);
    is_rotated = true;
}
