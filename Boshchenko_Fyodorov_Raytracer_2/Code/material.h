#ifndef MATERIAL_H_
#define MATERIAL_H_

#include "triple.h"
#include "image.h"
#include <iostream>
class Material {
public:
    Color color;        // base color
    double ka;          // ambient intensity
    double kd;          // makeDiffuse intensity
    double ks;          // specular intensity
    double n;           // exponent for specular highlight size
    bool has_texture = false;
    Image texture;

    Material() = default;

    void setTexture(std::string const &png_file) {
        has_texture = true;
        texture = Image("../Scenes/" + png_file);
    }

    Material(Color const &color, double ka, double kd, double ks, double n)
            :
            color(color),
            ka(ka),
            kd(kd),
            ks(ks),
            n(n) {}
};

#endif
