#ifndef SCENE_H_
#define SCENE_H_

#include "light.h"
#include "object.h"
#include "triple.h"

#include <vector>

// Forward declerations
class Ray;

class Image;

class Scene {
    std::vector<ObjectPtr> objects;
    std::vector<LightPtr> lights;   // no ptr needed, but kept for consistency
    Point eye;
    bool shadows = false;
    int ss_factor = 1;
    int recursion_depth = 0;

public:

    // trace a ray into the scene and return the color
    Color trace(Ray const &ray);

    // render the scene to the given image
    void render(Image &img);

    void addObject(ObjectPtr obj);

    void addLight(Light const &light);

    void setEye(Triple const &position);

    void setShadows(bool shadows);

    void setRecursionDepth(int recursion_depth);

    void setSsFactor(int ss_factor);

    unsigned getNumObject();

    unsigned getNumLights();
};

#endif
