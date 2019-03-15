#ifndef OBJECT_H_
#define OBJECT_H_

#include "material.h"

// not really needed here, but deriving classes may need them
#include "hit.h"
#include "ray.h"
#include "triple.h"

#include <memory>

class Object;

typedef std::shared_ptr<Object> ObjectPtr;

class Object {
public:
    Material material;

    static double constexpr EPSILON = 0.000001;
    static double constexpr DEFAULT_SHININESS = 0.2;

    virtual ~Object() = default;

    virtual Hit intersect(Ray const &ray) = 0;  // must be implemented
    // in derived class

    virtual std::pair<double, double> mapTextureCoord(Point &surface_point) {
        return std::make_pair(0, 0);
    };

};

#endif
