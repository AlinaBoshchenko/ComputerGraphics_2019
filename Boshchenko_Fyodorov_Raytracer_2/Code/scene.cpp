#include <memory>

#include "scene.h"
#include "object.h"
#include "image.h"

#include <cmath>
#include <iostream>

using namespace std;

void makeDiffuse(Color &color, Color &material_color, Material material, Vector L, Vector N, LightPtr const &light) {
    double dot = L.dot(N);
    if (dot > 0) {
        color += dot * material_color * light->color * material.kd;
    }
}

void makeSpecular(Color &color, Material material, Vector R, Vector V, LightPtr const &light) {
    if (material.ks < Object::EPSILON) {
        return;
    }
    double dot = R.dot(V);
    if (dot > 0) {
        color += pow(dot, material.n) * light->color * material.ks;
    }
}

bool isShadowed(std::vector<ObjectPtr> &objects, Point hit, Vector L) {
    for (auto const &object : objects) {
        Ray ray(hit, L); // It is a ray from the hit point towards the light source.
        Hit temp_hit(object->intersect(ray));
        if (!isnan(temp_hit.t)) { // Check if there is an intersection.
            return true;
        }
    }
    return false;
}

void calcReflection(Color &color, std::vector<ObjectPtr> const &objects, ObjectPtr &hit_object, Ray const &ray,
                    Point &hit, Vector &N, int depth) {
    if (depth == 0 || hit_object->material.ks < Object::EPSILON) {
        return;
    }
    Vector reflected = ray.D - 2 * ray.D.dot(N) * N;
    Vector V = -ray.D;

    Hit min_hit(numeric_limits<double>::infinity(), Vector());
    ObjectPtr reflected_object = nullptr;
    Point hit_point;
    for (ObjectPtr const &object : objects) {
        Ray new_ray(hit, reflected);
        Hit new_hit(object->intersect(new_ray));
        if (new_hit.t < min_hit.t) {
            reflected_object = object;
            min_hit = new_hit;
            hit_point = new_ray.at(new_hit.t);
        }
    }
    if (!reflected_object) {
        return;
    }
    color += pow(V.dot(V), hit_object->material.n) * reflected_object->material.color * hit_object->material.ks *
             Object::DEFAULT_SHININESS;
    calcReflection(color, objects, reflected_object, Ray(hit_point, reflected), hit_point, min_hit.N, depth - 1);
}

Color Scene::trace(Ray const &ray) {
    // Find hit object and distance
    Hit min_hit(numeric_limits<double>::infinity(), Vector());
    ObjectPtr obj = nullptr;
    for (ObjectPtr const &object : objects) {
        Hit hit(object->intersect(ray));
        if (hit.t < min_hit.t) {
            min_hit = hit;
            obj = object;
        }
    }

    // No hit? Return background color.
    if (!obj) {
        return Color(0.0, 0.0, 0.0);
    }

    Material material = obj->material;             //the hit objects material
    Point hit = ray.at(min_hit.t);                 //the hit point
    Vector N = min_hit.N;                          //the normal at hit point
    Vector V = -ray.D;                             //the view vector

    /****************************************************
    * This is where you should insert the color
    * calculation (Phong model).
    *
    * Given: material, hit, N, V, lights[]
    * Sought: color
    *
    * Hints: (see triple.h)
    *        Triple.dot(Vector) dot product
    *        Vector + Vector    vector sum
    *        Vector - Vector    vector difference
    *        Point - Point      yields vector
    *        Vector.normalize() normalizes vector, returns length
    *        double * Color     scales each color component (r,g,b)
    *        Color * Color      dito
    *        pow(a,b)           a to the power of b
    ****************************************************/
    Color material_color;
    if (material.has_texture) {
        auto mapped_coord = obj->mapTextureCoord(hit);
        material_color = material.texture.colorAt((float) mapped_coord.first, (float) mapped_coord.second);
    } else {
        material_color = material.color;
    }
    Color color = material_color * material.ka;              // Ambient
    for (LightPtr const &light : lights) {
        Vector L = (light->position - hit).normalized(),    // Vector from the hit location to the light position.
                R = 2 * N.dot(L) * N - L;                   // Reflected vector.
        if (shadows && isShadowed(objects, hit, L)) {
            // No need for diffuse and specular reflections when we are in the shadow.
            continue;
        }
        makeDiffuse(color, material_color, material, L, N, light);
        makeSpecular(color, material, R, V, light);
        calcReflection(color, objects, obj, ray, hit, N, recursion_depth);
    }
    return color;
}

void Scene::render(Image &img) {
    unsigned w = img.width();
    unsigned h = img.height();
    for (unsigned y = 0; y < h; ++y) {
        for (unsigned x = 0; x < w; ++x) {
            Color color(0, 0, 0);

            // Anti-aliasing
            double step = 1.0 / ss_factor;
            double halved_step = step / 2;
            for (double i = halved_step; i < 1; i += step) {
                for (double j = halved_step; j < 1; j += step) {
                    Point pixel(x + i, h - 1 - y + j, 0);
                    Ray ray(eye, (pixel - eye).normalized());
                    color += trace(ray);
                }
            }
            color /= pow(ss_factor, 2);
            color.clamp();
            img(x, y) = color;
        }
    }
}

// --- Misc functions ----------------------------------------------------------

void Scene::addObject(ObjectPtr obj) {
    objects.push_back(obj);
}

void Scene::addLight(Light const &light) {
    lights.push_back(std::make_shared<Light>(light));
}

void Scene::setEye(Triple const &position) {
    eye = position;
}

void Scene::setShadows(bool shadows) {
    this->shadows = shadows;
}

void Scene::setSsFactor(int ss_factor) {
    this->ss_factor = ss_factor;
}

void Scene::setRecursionDepth(int recursion_depth) {
    this->recursion_depth = recursion_depth;
}

unsigned Scene::getNumObject() {
    return static_cast<unsigned int>(objects.size());
}

unsigned Scene::getNumLights() {
    return static_cast<unsigned int>(lights.size());
}
