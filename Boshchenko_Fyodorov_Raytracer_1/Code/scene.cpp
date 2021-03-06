#include <memory>

#include "scene.h"

#include "image.h"

#include <cmath>

using namespace std;

void makeDiffuse(Color &color, Material material, Vector L, Vector N, LightPtr const &light) {
    double dot = L.dot(N);
    if (dot > 0) {
        color += dot * material.color * light->color * material.kd;
    }
}

void makeSpecular(Color &color, Material material, Vector R, Vector V, LightPtr const &light) {
    double dot = R.dot(V);
    if (dot > 0) {
        color += pow(dot, material.n) * light->color * material.ks;
    }
}

Color Scene::trace(Ray const &ray) {
    // Find hit object and distance
    Hit min_hit(numeric_limits<double>::infinity(), Vector());
    ObjectPtr obj = nullptr;
    for (unsigned idx = 0; idx != objects.size(); ++idx) {
        Hit hit(objects[idx]->intersect(ray));
        if (hit.t < min_hit.t) {
            min_hit = hit;
            obj = objects[idx];
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

    Color color = material.color * material.ka;              // Ambient
    for (LightPtr const &light : lights) {
        Vector L = (light->position - hit).normalized(),    // Vector from the hit location to the light position.
                R = 2 * N.dot(L) * N - L;                   // Reflected vector.
        makeDiffuse(color, material, L, N, light);
        makeSpecular(color, material, R, V, light);
    }
    return color;
}

void Scene::render(Image &img) {
    unsigned w = img.width();
    unsigned h = img.height();
    for (unsigned y = 0; y < h; ++y) {
        for (unsigned x = 0; x < w; ++x) {
            Point pixel(x + 0.5, h - 1 - y + 0.5, 0);
            Ray ray(eye, (pixel - eye).normalized());
            Color col = trace(ray);
            col.clamp();
            img(x, y) = col;
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

unsigned Scene::getNumObject() {
    return static_cast<unsigned int>(objects.size());
}

unsigned Scene::getNumLights() {
    return static_cast<unsigned int>(lights.size());
}
