
#include <iostream>
#include <string>
#include <Eigen/Core>
#include <Eigen/Dense>
#include <vector>
#include <random>



#include "../external/json.hpp"
#include "../external/simpleppm.h"



#ifndef RAYTRACER_H
#define	RAYTRACER_H
using point3 = Eigen::Vector3d;

struct Rectangle
{
public:
    point3 P1;
    point3 P2;
    point3 P3;
    point3 P4;
    Eigen::Vector3d normal;
    int id;
};

struct Sphere
{
public:
    point3 center;
    double radius;
    Eigen::Vector3d normal;
    int id;
};

class Ray
{
public:
    Ray() {}
    Ray(const point3& origin, const Eigen::Vector3d direction)
        : orig(origin), dir(direction)
    {}

    point3 origin() const { return orig; }
    Eigen::Vector3d direction() const { return dir; }

    point3 at(double t) const {
        return orig + t * dir;
    }

public:
    point3 orig;
    Eigen::Vector3d dir;
};

class RayTracer {
public:
    RayTracer();
    RayTracer(nlohmann::json j);
    virtual ~RayTracer();
    void run();
    point3 random_point_on_area_light();
    Eigen::Vector4d ray_hit(const Ray& r);
    Eigen::Vector4d ray_hit(const Ray& r, const int id, point3 hit_point);
    Eigen::Vector3d ray_color(const Ray& r, const Eigen::Vector4d hit);
    Eigen::Vector3d traceRay(Ray r, int bounces);
    Eigen::Vector3d hit_normal(Eigen::Vector4d hit);

    nlohmann::json json;
    // objects
    // sphere
    std::vector<std::string> type;
    std::vector <Sphere> spheres;
    point3 sphereCenter;
    double sphereRadius;
    //rectangle
    std::vector <Rectangle> rectangles;
    std::vector <Eigen::Vector3d> P1;
    std::vector <Eigen::Vector3d> P2;
    std::vector <Eigen::Vector3d> P3;
    std::vector <Eigen::Vector3d> P4;
    //universal
    std::vector<Eigen::Vector3d> ac;
    std::vector<Eigen::Vector3d> dc;
    std::vector<Eigen::Vector3d> sc;

    std::vector<float> ka;
    std::vector<float> kd;
    std::vector<float> ks;

    std::vector<float> pc;

    // camera
    std::string filename;
    int screenSize[2];
    Eigen::Vector3d lookat;
    Eigen::Vector3d up;
    Eigen::Vector3d center;
    float fov;
    Eigen::Vector3d bkc;

    //light
    std::vector<std::string> lightType;
    std::vector<Eigen::Vector3d> lightCenter;
    Eigen::Vector3d lightP1;
    Eigen::Vector3d lightP2;
    Eigen::Vector3d lightP3;
    Eigen::Vector3d lightP4;
    bool useCenter;
    std::vector<Eigen::Vector3d> ai;
    std::vector<Eigen::Vector3d> id;
    std::vector<Eigen::Vector3d> is;

    // misc
    std::vector<int> raysPerPixel;
    float maxBounces;
    float probterminate;
    bool globalillum;




private:

};
#endif	/* RAYTRACER_H */

