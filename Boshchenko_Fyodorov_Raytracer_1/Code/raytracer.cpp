#include "raytracer.h"

#include "image.h"

// =============================================================================
// -- Include all your shapes here ---------------------------------------------
// =============================================================================

#include "shapes/sphere.h"
#include "shapes/triangle.h"
#include "shapes/cone.h"
#include "shapes/cylinder.h"
#include "mesh.h"

// =============================================================================
// -- End of shape includes ----------------------------------------------------
// =============================================================================

#include "json/json.h"

#include <fstream>
#include <iostream>

using namespace std;        // no std:: required
using json = nlohmann::json;

bool Raytracer::parseObjectNode(json const &node) {
    ObjectPtr obj = nullptr;

// =============================================================================
// -- Determine type and parse object parameters ------------------------------
// =============================================================================

    if (node["type"] == "sphere") {
        Point pos(node["position"]);
        double radius = node["radius"];
        obj = ObjectPtr(new Sphere(pos, radius));
    } else if (node["type"] == "triangle") {
        Point a(node["a"]),
                b(node["b"]),
                c(node["c"]);
        obj = ObjectPtr(new Triangle(a, b, c));
    } else if (node["type"] == "cone") {
        Point C(node["C"]);
        Vector V(node["V"]);
        double theta(node["theta"]);
        obj = ObjectPtr(new Cone(C, V, theta));
    } else if (node["type"] == "cylinder") {
        Point center(node["center"]);
        double radius(node["radius"]),
                height(node["height"]);
        obj = ObjectPtr(new Cylinder(center, radius, height));
    } else if (node["type"] == "mesh") {
        string filepath = node["filepath"];
        Mesh mesh(filepath);
        for (Triangle const &triangle : mesh.getTriangles()) {
            obj = ObjectPtr(new Triangle(triangle));
            Color color((random() % 255) / 1000.0, (random() % 255) / 1000.0, (random() % 255) / 1000.0);
            Material material(color, 0.5, 0.6, 0.9, 64);
            obj->material = material;
            scene.addObject(obj);
        }
        return true;
    } else {
        cerr << "Unknown object type: " << node["type"] << ".\n";
    }

// =============================================================================
// -- End of object reading ----------------------------------------------------
// =============================================================================

    if (!obj)
        return false;

    // Parse material and add object to the scene
    obj->material = parseMaterialNode(node["material"]);
    scene.addObject(obj);
    return true;
}

Light Raytracer::parseLightNode(json const &node) const {
    Point pos(node["position"]);
    Color col(node["color"]);
    return Light(pos, col);
}

Material Raytracer::parseMaterialNode(json const &node) const {
    Color color(node["color"]);
    double ka = node["ka"];
    double kd = node["kd"];
    double ks = node["ks"];
    double n = node["n"];
    return Material(color, ka, kd, ks, n);
}

bool Raytracer::readScene(string const &ifname)
try {
    // Read and parse input json file
    ifstream infile(ifname);
    if (!infile) throw runtime_error("Could not open input file for reading.");
    json jsonscene;
    infile >> jsonscene;

// =============================================================================
// -- Read your scene data in this section -------------------------------------
// =============================================================================

    Point eye(jsonscene["Eye"]);
    scene.setEye(eye);

    // TODO: add your other configuration settings here

    for (auto const &lightNode : jsonscene["Lights"])
        scene.addLight(parseLightNode(lightNode));

    unsigned objCount = 0;
    for (auto const &objectNode : jsonscene["Objects"])
        if (parseObjectNode(objectNode))
            ++objCount;

    cout << "Parsed " << objCount << " objects.\n";

// =============================================================================
// -- End of scene data reading ------------------------------------------------
// =============================================================================

    return true;
}
catch (exception const &ex) {
    cerr << ex.what() << '\n';
    return false;
}

void Raytracer::renderToFile(string const &ofname) {
    // TODO: the size may be a settings in your file
    Image img(400, 400);
    cout << "Tracing...\n";
    scene.render(img);
    cout << "Writing image to " << ofname << "...\n";
    img.write_png(ofname);
    cout << "Done.\n";
}
