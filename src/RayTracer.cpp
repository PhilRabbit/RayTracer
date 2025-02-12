

/*
 * File:   RayTracer.cpp
 * Author: p_rabbat
 *
 * Created on January 25, 2023, 2:41 PM
 */

#include "RayTracer.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <iostream>
#include <string>
#include <Eigen/Core>
#include <Eigen/Dense>

#include "../external/json.hpp"
#include "../external/simpleppm.h"


using namespace std;
using namespace nlohmann;
using color = Eigen::Vector3d;
using ray = Eigen::Vector3d;
using vec3 = Eigen::Vector3d;
using namespace std;


int test_eigen();
int test_save_ppm();
int test_json(nlohmann::json& j);


color RayTracer::ray_color(const Ray& r, const Eigen::Vector4d hit)
{
    color colorEnd;
    colorEnd << 0, 0, 0;
    for (int iterate = 0; iterate < this->type.size(); iterate++)
    {
        //cout << "Count: " << iterate << endl;
        if (this->type[iterate] == "sphere" && hit[3] <= -1)
        {
            point3 iPoint;
            iPoint << hit[0], hit[1], hit[2];

            for (int i = 0; i < this->lightType.size(); i++)
            {


                //normal of the point on a sphere
                vec3 normal = iPoint - sphereCenter;
                //vector of intersection point to light
                vec3 light = lightCenter[i] - iPoint;
                //distance of the point to the light
                float distance = light.norm();
                // cout << "d: " << distance << endl;
                //distance = distance * distance;
                // normalizing normal and light
                normal.normalize();
                light.normalize();

                // ambient color
                vec3 ambient = ka[iterate] * ac[iterate];

                //cout << "ka: " << ka[iterate] << "ac: " << ac[iterate] << endl;
                // calculating diffuse color

                float NdotL = normal.dot(light);
                if (NdotL < 0)
                {
                    NdotL = 0;
                }


                vec3 diffuse = (kd[iterate] * dc[iterate] * NdotL);
                //diffuse = diffuse.cwiseMax(1);

                //cout << "kd: " << kd[iterate] << "NdotL: " << NdotL << "dc: " << dc[iterate] << endl;
                // calculating specular color
                // point to the camera
                vec3 viewDir = center - iPoint;
                viewDir.normalize();

                float spec = 0;
                float specAngle;
                if (NdotL > 0)
                {
                    vec3 R = -light - 2.0 * normal.dot(-light) * normal;
                    specAngle = R.dot(viewDir);
                    if (specAngle < 0)
                    {
                        specAngle = 0;
                    }
                    spec = pow(specAngle, pc[iterate]);
                }
                else {
                    vec3 halfDir = viewDir + light;
                    halfDir.normalize();
                    float NdotH = normal.dot(halfDir);
                    if (NdotH < 0)
                    {
                        NdotH = 0;
                    }

                    spec = pow(NdotH, pc[iterate]);
                }
                //cout << "spec: " << spec << endl;
                //cout << "NdotL: " << NdotL << "specAngle: " << specAngle << endl;
                vec3 specular = ks[iterate] * spec * sc[iterate];
                //cout << "a: " << ambient << "d: " << "s: " << specular << endl;
                colorEnd = colorEnd + (ambient.cwiseProduct(ai[0]) + diffuse + specular);
                colorEnd = colorEnd.cwiseMin(1);

                if (colorEnd.isZero(0))

                {
                    //cout << "light: " << NdotL << "normal: " << normal << endl;
                }
                //cout << colorEnd;
            }



        }
        else if (this->type[iterate] == "rectangle" && hit[3] >= 0 && hit[3] != 100)
        {
            for (int i = 0; i < this->lightType.size(); i++)
            {
                point3 Point;
                Point << hit[0], hit[1], hit[2];
                Eigen::Vector4d empty;
                empty << 0, 0, 0, 0;
                vec3 normal = (P2[iterate] - P1[iterate]).cross(P3[iterate] - P1[iterate]) / ((P2[iterate] - P1[iterate]).cross(P3[iterate] - P1[iterate]).norm());
                vec3 light = lightCenter[i] - Point;
                //distance of the point to the light
                float distance = light.norm();
                distance = distance * distance;
                Ray shadow(Point, light);
                Eigen::Vector4d shadowHit = ray_hit(shadow);
                if (shadowHit[3] >= 0 && hit[3] != 100)
                {

                    distance = sqrt(distance);
                    // normalizing normal and light
                    normal.normalize();
                    light.normalize();

                    // ambient color
                    vec3 ambient = ka[iterate] * ac[iterate];
                    //cout << "ka: " << ka[iterate] << "ac: " << ac[iterate] << endl;
                    // calculating diffuse color

                    float NdotL = normal.dot(light);
                    if (NdotL < 0)
                    {
                        NdotL = 0;
                    }

                    vec3 diffuse = (kd[iterate] * NdotL * dc[iterate]);
                    //cout << "kd: " << kd[iterate] << "NdotL: " << NdotL << "dc: " << dc[iterate] << endl;
                    // calculating specular color
                    // point to the camera
                    vec3 viewDir = center - Point;
                    viewDir.normalize();

                    float spec = 0;
                    float specAngle;
                    if (NdotL > 0)
                    {
                        vec3 R = -light - 2.0 * normal.dot(-light) * normal;
                        specAngle = R.dot(viewDir);
                        if (specAngle < 0)
                        {
                            specAngle = 0;
                        }
                        spec = pow(specAngle, pc[iterate]);
                    }
                    else {
                        vec3 halfDir = viewDir + light;
                        halfDir.normalize();
                        float NdotH = normal.dot(halfDir);
                        if (NdotH < 0)
                        {
                            NdotH = 0;
                        }
                        spec = pow(NdotH, pc[iterate]);
                    }

                    //cout << "NdotL: " << NdotL << "specAngle: " << specAngle << endl;
                    vec3 specular = ks[iterate] * spec * sc[iterate];
                    //cout << "a: " << ambient << "d: " << "s: " << specular << endl;
                    colorEnd = colorEnd + (ambient.cwiseProduct(ai[0]) + diffuse.cwiseProduct(id[i]) + specular.cwiseProduct(is[i]));
                    colorEnd = colorEnd.cwiseMin(1);
                    if (colorEnd.isZero(0))
                    {
                        //cout << "light: " << NdotL << "normal: " << normal << endl;
                    }
                    //cout << colorEnd;
                }



            }
        }
        else
        {
            if (colorEnd == bkc)
            {
                colorEnd = bkc;
            }

        }

    }

    return colorEnd;


}


Eigen::Vector4d RayTracer::ray_hit(const Ray& r)
{
    Eigen::Vector4d hit;
    hit << 100, 100, 100, 100;
    point3 iPoint;
    point3 Point;
    vec3 distance(3000, 3000, 3000);
    // calculate intersection points
    for (int iterate = 0; iterate < spheres.size(); iterate++)
    {
        Eigen::Vector3d oc = r.origin() - spheres[iterate].center;
        float a = r.direction().dot(r.direction());
        float b = 2.0 * (oc.dot(r.direction()));
        float c = oc.dot(oc) - spheres[iterate].radius * spheres[iterate].radius;
        float discriminant = ((b * b) - (4 * a * c));
        if (discriminant > 0)
        {
            float x1 = (-b + sqrt(discriminant)) / (2 * a);
            float x2 = (-b - sqrt(discriminant)) / (2 * a);

            // both points are positive means ray is facing sphere
            // x2 is the intersecting point
            if (x1 > 0 && x2 > 0)
            {
                //if (x2 > x1) std::swap(x2, x1);
                iPoint = r.origin() + x2 * r.direction();

                distance = iPoint - center;
                hit << iPoint[0], iPoint[1], iPoint[2], spheres[iterate].id;

            }
        }
    }


    for (int iterate = 0; iterate < rectangles.size(); iterate++)
    {
        if (this->type[iterate] == "rectangle")
        {
            // rectangle intersection
            // rectangles are assumed to be coplanar which means the normal of a tirangle
            // using P1, P2, P3 is the same normal as the rectangle
            vec3 normal = rectangles[iterate].normal;
            float D = -normal.dot(P2[iterate]);
            float t = -(normal.dot(r.origin()) + D) / normal.dot(r.direction());
            Point = r.origin() + t * r.direction();
            // cout << "T: " << t << endl;

            float x = ((P2[iterate] - P1[iterate]).cross(Point - P1[iterate])).dot(normal);
            float y = ((P3[iterate] - P2[iterate]).cross(Point - P2[iterate])).dot(normal);
            float z = ((P4[iterate] - P3[iterate]).cross(Point - P3[iterate])).dot(normal);
            float w = ((P1[iterate] - P4[iterate]).cross(Point - P4[iterate])).dot(normal);

            if ((x > 0 && y > 0 && z > 0 && w > 0) || (x < 0 && y < 0 && z < 0 && w < 0))
            {
                //Eigen::Vector4d hit;

                if (distance.norm() > (Point - center).norm() || hit[2] == 0)
                {
                    hit << Point[0], Point[1], Point[2], rectangles[iterate].id;
                    distance = Point - center;
                }

            }
        }

    }
    return hit;
}

Eigen::Vector4d RayTracer::ray_hit(const Ray& r, const int id, point3 hit_point)
{
    Eigen::Vector4d hit;
    hit << 100, 100, 100, 100;
    point3 iPoint;
    point3 Point;
    vec3 distance(3000, 3000, 3000);
    // calculate intersection points
    for (int iterate = 0; iterate < spheres.size(); iterate++)
    {
        if (spheres[iterate].id != id)
        {
            Eigen::Vector3d oc = r.origin() - spheres[iterate].center;
            float a = r.direction().dot(r.direction());
            float b = 2.0 * (oc.dot(r.direction()));
            float c = oc.dot(oc) - spheres[iterate].radius * spheres[iterate].radius;
            float discriminant = ((b * b) - (4 * a * c));
            if (discriminant > 0)
            {
                float x1 = (-b + sqrt(discriminant)) / (2 * a);
                float x2 = (-b - sqrt(discriminant)) / (2 * a);

                // both points are positive means ray is facing sphere
                // x2 is the intersecting point
                if (x1 > 0 && x2 > 0)
                {
                    //if (x2 > x1) std::swap(x2, x1);
                    iPoint = r.origin() + x2 * r.direction();
                    if (distance.norm() > (iPoint - hit_point).norm() || hit[2] == 0)
                    {
                        distance = iPoint - hit_point;
                        hit << iPoint[0], iPoint[1], iPoint[2], spheres[iterate].id;
                    }
                }
            }
        }
    }

    for (int iterate = 0; iterate < rectangles.size(); iterate++)
    {
        if (rectangles[iterate].id != id)
        {
            // rectangle intersection
            // rectangles are assumed to be coplanar which means the normal of a tirangle
            // using P1, P2, P3 is the same normal as the rectangle
            vec3 normal = rectangles[iterate].normal;
            float D = -normal.dot(P2[iterate]);
            float t = -(normal.dot(r.origin()) + D) / normal.dot(r.direction());
            Point = r.origin() + t * r.direction();
            // cout << "T: " << t << endl;

            float x = ((rectangles[iterate].P2 - rectangles[iterate].P1).cross(Point - rectangles[iterate].P1)).dot(normal);
            float y = ((rectangles[iterate].P3 - rectangles[iterate].P2).cross(Point - rectangles[iterate].P2)).dot(normal);
            float z = ((rectangles[iterate].P4 - rectangles[iterate].P3).cross(Point - rectangles[iterate].P3)).dot(normal);
            float w = ((rectangles[iterate].P1 - rectangles[iterate].P4).cross(Point - rectangles[iterate].P4)).dot(normal);

            if ((x > 0 && y > 0 && z > 0 && w > 0) || (x < 0 && y < 0 && z < 0 && w < 0))
            {
                //Eigen::Vector4d hit;

                if (distance.norm() > (Point - hit_point).norm() || hit[2] == 0)
                {
                    hit << Point[0], Point[1], Point[2], rectangles[iterate].id;
                    distance = Point - hit_point;
                }

            }
        }

    }
    return hit;
}

Eigen::Vector3d RayTracer::hit_normal(Eigen::Vector4d hit)
{
    vec3 normal(0, 0, 0);
    if (hit[3] <= -1)
    {
        point3 iPoint;
        iPoint << hit[0], hit[1], hit[2];
        normal = iPoint - sphereCenter;
    }
    else if (hit[3] >= 0 && hit[3] != 100)
    {
        normal = rectangles[hit[3]].normal;
    }
    return normal;
}

float randFloat() {
    return (float)rand() / RAND_MAX;
}

vec3 randHemisphere(vec3 normal) {
    vec3 v(randFloat() * 2.0f - 1.0f, randFloat() * 2.0f - 1.0f, randFloat() * 2.0f - 1.0f);
    v.normalize();
    if (v.dot(normal) < 0.0f) {
        v = -v;
    }
    return v;
}
point3 RayTracer::random_point_on_area_light()
{
    static std::default_random_engine generator;
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    double u = distribution(generator);
    double v = distribution(generator);
    // Interpolate between the corners of the rectangle
    point3 point = (1 - u) * (1 - v) * lightP1 +
        u * (1 - v) * lightP3 +
        u * v * lightP2 +
        (1 - u) * v * lightP4;

    // Add the rectangle's origin to get the point in world coordinates
    return point;

}


Eigen::Vector3d RayTracer::traceRay(Ray ray, int bounces) {
    vec3 accumulatedColor(0.0f, 0.0f, 0.0f);

    // Check if the ray intersects with any objects in the scene
    Eigen::Vector4d hit = ray_hit(ray);
    point3 hit_point(hit[0], hit[1], hit[2]);
    if (bounces > 0)
    {
        hit = ray_hit(ray, hit[3], hit_point);
    }
    if (useCenter)
    {

        vec3 surfaceNormal = hit_normal(hit);
        vec3 lightDirection = (lightCenter[0] - hit_point).normalized();
        double intensity = 1;
        double diffuseFactor = std::max(0.0, surfaceNormal.dot(lightDirection));
        // Check if the maximum number of bounces has been reached
        if (bounces >= maxBounces)
        {

            vec3 light = lightCenter[0] - hit_point;
            Ray shadow(hit_point, light);
            Eigen::Vector4d shadowHit = ray_hit(shadow, hit[3], hit_point);
            if (shadowHit[3] != 100)
            {
                // Accumulate the shadow color obtained from the shadow ray
                vec3 shadowColor = vec3(0.0, 0.0, 0.0);
                return shadowColor;
            }
            
            return dc[hit[3]];
        }
        // DOESN'T HIT
        if (hit_point == point3(100, 100, 100))
        {
            return bkc;
        }
        // HITS
        else
        {
            // Calculate the color of the point using the Lambertian shading model
            vec3 color = dc[hit[3]] * (intensity * diffuseFactor);
            accumulatedColor += color;
        }

        // Determine if the ray terminates
        if (randFloat() < probterminate)
        {

            vec3 light = lightCenter[0] - hit_point;
            Ray shadow(hit_point, light);
            Eigen::Vector4d shadowHit = ray_hit(shadow, hit[3], hit_point);
            if (shadowHit[3] != 100)
            {
                // Accumulate the shadow color obtained from the shadow ray
                vec3 shadowColor = vec3(0.0, 0.0, 0.0);
                return shadowColor;
            }
            
            return dc[hit[3]];
        }

        // Determine the color of the ray by simulating its interactions with the scene
        vec3 sampleDirection = randHemisphere(hit_normal(hit));
        Ray sampleRay(hit_point, sampleDirection);
        accumulatedColor += traceRay(sampleRay, bounces + 1);


    }
    else
    {
        for (int i = 0; i < 5; i++)
        {
            point3 random_light = random_point_on_area_light();
            vec3 surfaceNormal = hit_normal(hit);
            vec3 lightDirection = (random_light - hit_point).normalized();
            double intensity = 1;
            double diffuseFactor = std::max(0.0, surfaceNormal.dot(lightDirection));
            // Check if the maximum number of bounces has been reached
            if (bounces >= maxBounces)
            {

                for (int i = 0; i < 5; i++)
                {
                    vec3 light = random_light - hit_point;
                    Ray shadow(hit_point, light);
                    Eigen::Vector4d shadowHit = ray_hit(shadow, hit[3], hit_point);
                    if (shadowHit[3] != 100)
                    {
                        // Accumulate the shadow color obtained from the shadow ray
                        vec3 shadowColor = vec3(0.0, 0.0, 0.0);
                        return shadowColor;
                    }

                }
                return dc[hit[3]];
            }
            // DOESN'T HIT
            if (hit_point == point3(100, 100, 100))
            {
                return bkc;
            }
            // HITS
            else
            {
                // Calculate the color of the point using the Lambertian shading model
                vec3 color = dc[hit[3]] * (intensity * diffuseFactor);
                accumulatedColor += color;
            }

            // Determine if the ray terminates
            if (randFloat() < probterminate)
            {
   
                for (int i = 0; i < 5; i++)
                {

                    vec3 light = random_light - hit_point;
                    Ray shadow(hit_point, light);
                    Eigen::Vector4d shadowHit = ray_hit(shadow, hit[3], hit_point);
                    if (shadowHit[3] != 100)
                    {
                        // Accumulate the shadow color obtained from the shadow ray
                        vec3 shadowColor = vec3(0.0, 0.0, 0.0);
                        return shadowColor;
                    }

                }
                

                return dc[hit[3]];
            }

            // Determine the color of the ray by simulating its interactions with the scene
            vec3 sampleDirection = randHemisphere(hit_normal(hit));
            Ray sampleRay(hit_point, sampleDirection);
            accumulatedColor += traceRay(sampleRay, bounces + 1);

        }
    }

    return accumulatedColor;
}



void write_color(ofstream& out, color pixel_color) {
    // Write the translated [0,255] value of each color component.
    out << static_cast<int>(255.999 * pixel_color(0)) << ' '
        << static_cast<int>(255.999 * pixel_color(1)) << ' '
        << static_cast<int>(255.999 * pixel_color(2)) << '\n';
}

RayTracer::RayTracer()
{

}


// THIS IS ALL JUST TO GET THE INFO OUT OF THE JSON FILE
// THERE WAS PROBABLY ANM EASIER WAY TO DO THIS BUT I COULDN'T FIND IT
RayTracer::RayTracer(nlohmann::json j)
{
    // Objects
    json = j;
    cout << "Geometry: " << endl;
    int gc = 0;
    int recs = 0;
    int sphes = 0;
    // use iterators to read-in array types
    for (auto itr = json["geometry"].begin(); itr != json["geometry"].end(); itr++) {

        std::string type;
        point3 empty;
        if (itr->contains("type")) {
            // type = static_cast<std::string>((*itr)["type"]);
            type = (*itr)["type"].get<std::string>();
        }
        else
        {
            cout << "Fatal error: geometry should always contain a type!!!" << endl;
        }

        if (type == "sphere") {
            cout << "Sphere: " << endl;
            this->type.push_back("sphere");
            //hasSphere = true;
            int i = 0;
            for (auto itr2 = (*itr)["centre"].begin(); itr2 != (*itr)["centre"].end(); itr2++) {
                if (i < 3)
                {
                    sphereCenter[i++] = (*itr2).get<float>();
                }
                else
                {
                    cout << "Warning: Too many entries in center" << endl;
                }
            }


            sphereRadius = (*itr)["radius"].get<float>();

            cout << "Center: " << sphereCenter << endl;
            cout << "Radius: " << sphereRadius << endl;
            Sphere blank;
            spheres.push_back(blank);
            spheres[sphes].center = sphereCenter;
            spheres[sphes].radius = sphereRadius;
            spheres[sphes].id = -1 - sphes;


            sphes++;
        }
        else if (type == "rectangle")
        {

            this->type.push_back("rectangle");
            this->P1.push_back(empty);
            int i = 0;
            for (auto itr2 = (*itr)["p1"].begin(); itr2 != (*itr)["p1"].end(); itr2++)
            {
                if (i < 3)
                {
                    P1[gc][i++] = (*itr2).get<float>();
                }
                else
                {
                    cout << "Warning: Too many entries in P1" << endl;
                }
            }
            this->P2.push_back(empty);
            i = 0;
            for (auto itr2 = (*itr)["p2"].begin(); itr2 != (*itr)["p2"].end(); itr2++)
            {
                if (i < 3)
                {
                    P2[gc][i++] = (*itr2).get<float>();
                }
                else
                {
                    cout << "Warning: Too many entries in P2" << endl;
                }
            }
            this->P3.push_back(empty);
            i = 0;
            for (auto itr2 = (*itr)["p3"].begin(); itr2 != (*itr)["p3"].end(); itr2++)
            {
                if (i < 3)
                {
                    P3[gc][i++] = (*itr2).get<float>();
                }
                else
                {
                    cout << "Warning: Too many entries in P3" << endl;
                }
            }
            this->P4.push_back(empty);
            i = 0;
            for (auto itr2 = (*itr)["p4"].begin(); itr2 != (*itr)["p4"].end(); itr2++)
            {
                if (i < 3)
                {
                    P4[gc][i++] = (*itr2).get<float>();
                }
                else
                {
                    cout << "Warning: Too many entries in P4" << endl;
                }
            }
            Rectangle blank;
            rectangles.push_back(blank);
            rectangles[recs].P1 = P1[recs];
            rectangles[recs].P2 = P2[recs];
            rectangles[recs].P3 = P3[recs];
            rectangles[recs].P4 = P4[recs];
            vec3 normal = (rectangles[recs].P2 - rectangles[recs].P1).cross(rectangles[recs].P3 - rectangles[recs].P1) / ((rectangles[recs].P2 - rectangles[recs].P1).cross(rectangles[recs].P3 - rectangles[recs].P1).norm());
            rectangles[recs].normal = normal;
            rectangles[recs].id = recs;

            cout << "P1: " << P1[recs] << endl;
            cout << "P2: " << P2[recs] << endl;
            cout << "P3: " << P3[recs] << endl;
            cout << "P4: " << P4[recs] << endl;
            recs++;
        }

        int i = 0;
        ac.push_back(empty);
        for (auto itr2 = (*itr)["ac"].begin(); itr2 != (*itr)["ac"].end(); itr2++) {
            if (i < 3)
            {
                ac[gc][i++] = (*itr2).get<float>();
            }
            else
            {
                cout << "Warning: Too many entries in ac" << endl;
            }
        }
        i = 0;
        dc.push_back(empty);
        for (auto itr2 = (*itr)["dc"].begin(); itr2 != (*itr)["dc"].end(); itr2++) {
            if (i < 3)
            {
                dc[gc][i++] = (*itr2).get<float>();
            }
            else
            {
                cout << "Warning: Too many entries in dc" << endl;
            }
        }
        i = 0;
        sc.push_back(empty);
        for (auto itr2 = (*itr)["sc"].begin(); itr2 != (*itr)["sc"].end(); itr2++) {
            if (i < 3)
            {
                sc[gc][i++] = (*itr2).get<float>();
            }
            else
            {
                cout << "Warning: Too many entries in sc" << endl;
            }
        }
        ka.push_back((*itr)["ka"].get<float>());
        kd.push_back((*itr)["kd"].get<float>());
        ks.push_back((*itr)["ks"].get<float>());
        pc.push_back((*itr)["pc"].get<float>());
        cout << std::setprecision(10) << "Ac: " << ac[gc] << endl;
        cout << std::setprecision(10) << "Dc: " << dc[gc] << endl;
        cout << std::setprecision(10) << "Sc: " << sc[gc] << endl;
        cout << std::setprecision(10) << "ka: " << ka[gc] << endl;
        cout << std::setprecision(10) << "kd: " << kd[gc] << endl;
        cout << std::setprecision(10) << "ks: " << ks[gc] << endl;
        cout << std::setprecision(10) << "pc: " << pc[gc] << endl;

        ++gc;
    }

    cout << "We have: " << gc << " objects!" << endl;
    cout << "Outputs: " << endl;
    int lc = 0;

    // use iterators to read-in array types
    for (auto itr = j["output"].begin(); itr != j["output"].end(); itr++) {

        point3 empty;
        if (itr->contains("filename")) {
            //  filename = static_cast<std::string>((*itr)["filename"]);
            filename = (*itr)["filename"].get<std::string>();
        }
        else {
            cout << "Fatal error: output should always contain a filename!!!" << endl;
        }


        // Screen Ratio
        int i = 0;
        for (auto itr2 = (*itr)["size"].begin(); itr2 != (*itr)["size"].end(); itr2++) {
            if (i < 2) {
                screenSize[i++] = (*itr2).get<float>();
            }
            else {
                cout << "Warning: Too many entries in center" << endl;
            }
        }

        // Center of Camera
        i = 0;
        for (auto itr2 = (*itr)["centre"].begin(); itr2 != (*itr)["centre"].end(); itr2++) {
            if (i < 3) {
                center[i++] = (*itr2).get<float>();
            }
            else {
                cout << "Warning: Too many entries in center" << endl;
            }
        }
        // Lookat Vector      
        i = 0;
        for (auto itr2 = (*itr)["lookat"].begin(); itr2 != (*itr)["lookat"].end(); itr2++) {
            if (i < 3) {
                lookat[i++] = (*itr2).get<float>();
            }
            else {
                cout << "Warning: Too many entries in lookat" << endl;
            }
        }
        // Up Vector
        i = 0;
        for (auto itr2 = (*itr)["up"].begin(); itr2 != (*itr)["up"].end(); itr2++) {
            if (i < 3) {
                up[i++] = (*itr2).get<float>();
            }
            else {
                cout << "Warning: Too many entries in up" << endl;
            }
        }
        i = 0;
        ai.push_back(empty);
        for (auto itr2 = (*itr)["ai"].begin(); itr2 != (*itr)["ai"].end(); itr2++) {
            if (i < 3)
            {
                ai[lc][i++] = (*itr2).get<float>();
            }
            else
            {
                cout << "Warning: Too many entries in dc" << endl;
            }
        }
        i = 0;
        cout << "YES";
        for (auto itr2 = (*itr)["bkc"].begin(); itr2 != (*itr)["bkc"].end(); itr2++) {
            if (i < 3) {
                bkc[i++] = (*itr2).get<float>();
            }
            else {
                cout << "Warning: Too many entries in bkc" << endl;
            }
        }

        i = 0;
        //raysPerPixel.push_back(0);
        for (auto itr2 = (*itr)["raysperpixel"].begin(); itr2 != (*itr)["raysperpixel"].end(); itr2++) {
            if (i < 3)
            {
                raysPerPixel.push_back((*itr2).get<float>());
            }
            else
            {
                cout << "Warning: Too many entries in raysPerPixel" << endl;
            }
        }

        if (itr->contains("globalillum"))
        {
            globalillum = (*itr)["globalillum"].get<bool>();
        }
        else
        {
            globalillum = false;
        }
        if (globalillum)
        {
            maxBounces = (*itr)["maxbounces"].get<float>();
            probterminate = (*itr)["probterminate"].get<float>();

        }
        else
        {
            maxBounces = 0;
            probterminate = 1;
        }



        cout << std::setprecision(10) << "Ai: " << ai[lc] << endl;
        fov = (*itr)["fov"].get<float>();

        cout << "Filename: " << filename << endl;
        cout << "Camera centre: " << center << endl;
        cout << "Camera up: " << up << endl;
        cout << "Camera lookat: " << lookat << endl;
        cout << "Sceen size: " << screenSize[0] << " by " << screenSize[1] << endl;
        cout << "FOV: " << fov << endl;
        for (i = 0; i < raysPerPixel.size(); i++)
        {
            cout << "raysPerPixel: " << raysPerPixel[i] << ", ";
        }
        cout << " vector size: " << raysPerPixel.size() << endl;
        cout << std::setprecision(10) << "maxBounces: " << maxBounces << endl;
        cout << std::setprecision(10) << "probterminate: " << probterminate << endl;
        cout << "globalillum: " << globalillum << endl;
        cout << "bkc: " << bkc << endl;

        ++lc;
    }

    cout << "We have: " << lc << " camera objects!" << endl;

    // LIGHT
    cout << "Light: " << endl;
    int lt = 0;

    // use iterators to read-in array types
    for (auto itr = j["light"].begin(); itr != j["light"].end(); itr++) {

        std::string type;
        point3 empty;
        if (itr->contains("type")) {
            // type = static_cast<std::string>((*itr)["type"]);
            type = (*itr)["type"].get<std::string>();
        }
        else {
            cout << "Fatal error: light shoudl always contain a type!!!" << endl;

        }

        if (type == "point")
        {
            cout << "Point based light: " << endl;
            this->lightType.push_back("point");
        }
        else if (type == "area")
        {
            cout << "Area based light: " << endl;
            this->lightType.push_back("area");
        }
        //Eigen::Vector3d centre(0, 0, 0);
        int i = 0;
        lightCenter.push_back(empty);
        if (lightType[lt] == "point")
        {
            for (auto itr2 = (*itr)["centre"].begin(); itr2 != (*itr)["centre"].end(); itr2++) {
                if (i < 3) {
                    lightCenter[lt][i++] = (*itr2).get<float>();
                }
                else {
                    cout << "Warning: Too many entries in center" << endl;
                }
            }
            cout << "Centre: " << lightCenter[lt] << endl;
        }

        else
        {
            // Area light points
            i = 0;
            for (auto itr2 = (*itr)["p1"].begin(); itr2 != (*itr)["p1"].end(); itr2++) {
                if (i < 3) {
                    lightP1[i++] = (*itr2).get<float>();
                }
                else {
                    cout << "Warning: Too many entries in p1" << endl;
                }
            }

            i = 0;
            for (auto itr2 = (*itr)["p2"].begin(); itr2 != (*itr)["p2"].end(); itr2++) {
                if (i < 3) {
                    lightP2[i++] = (*itr2).get<float>();
                }
                else {
                    cout << "Warning: Too many entries in p2" << endl;
                }
            }

            i = 0;
            for (auto itr2 = (*itr)["p3"].begin(); itr2 != (*itr)["p3"].end(); itr2++) {
                if (i < 3) {
                    lightP3[i++] = (*itr2).get<float>();
                }
                else {
                    cout << "Warning: Too many entries in p3" << endl;
                }
            }

            i = 0;
            for (auto itr2 = (*itr)["p4"].begin(); itr2 != (*itr)["p4"].end(); itr2++) {
                if (i < 3) {
                    lightP4[i++] = (*itr2).get<float>();
                }
                else {
                    cout << "Warning: Too many entries in p4" << endl;
                }
            }
            if (itr->contains("usecenter"))
            {
                useCenter = (*itr)["usecenter"].get<bool>();
            }
            else{
                useCenter = false;
            }
            if (useCenter)
            {
                point3 midpoint;
                midpoint[0] = (lightP1[0] + lightP3[0]) / 2.0;
                midpoint[1] = (lightP1[1] + lightP3[1]) / 2.0;
                midpoint[2] = (lightP1[2] + lightP3[2]) / 2.0;
                lightCenter[lt] = midpoint;
            }
        }


        i = 0;
        id.push_back(empty);
        for (auto itr2 = (*itr)["id"].begin(); itr2 != (*itr)["id"].end(); itr2++) {
            if (i < 3) {
                id[lt][i++] = (*itr2).get<float>();
            }
            else {
                cout << "Warning: Too many entries in id" << endl;
            }
        }
        i = 0;
        is.push_back(empty);
        for (auto itr2 = (*itr)["is"].begin(); itr2 != (*itr)["is"].end(); itr2++) {
            if (i < 3) {
                is[lt][i++] = (*itr2).get<float>();
            }
            else {
                cout << "Warning: Too many entries in is" << endl;
            }
        }
        cout << std::setprecision(10) << "is: " << is[lt] << endl;
        cout << std::setprecision(10) << "id: " << id[lt] << endl;
        ++lt;
    }

    cout << "We have: " << lt << " light objects!" << endl;


}

RayTracer::~RayTracer()
{

}

void RayTracer::run()
{
    // Image
    const int image_width = screenSize[0];
    const int image_height = screenSize[1];



    // Camera
    // for ray tracing
    point3 origin = center;

    Eigen::Vector3d r = up.cross(lookat);
    double delta = (2 * tan((fov / 2) * M_PI / 180)) / image_height;
    cout << "DELTA: " << delta << endl;
    point3 A = center + lookat;
    point3 B = A + tan((fov / 2 * M_PI / 180)) * up;
    point3 C = B - (image_width / 2) * r * delta;
    // for path tracing
    float aspectRatio = (float)image_width / (float)image_height;
    float halfHeight = tan((fov / 2 * M_PI / 180));
    float halfWidth = aspectRatio * halfHeight;
    float imagePlaneDistance = 1.0f;
    vec3 imagePlaneOrigin = center + lookat * imagePlaneDistance - r * halfWidth + up * halfHeight;
    vec3 imagePlaneHorizontal = r * 2.0f * halfWidth;
    vec3 imagePlaneVertical = up * 2.0f * halfHeight;




    // Render

    ofstream out;
    out.open(filename);
    out << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    // Normal RayTracing
    if (!globalillum)
    {
        for (int i = 0; i < image_height; ++i)
        {
            for (int j = image_width - 1; j >= 0; --j)
            {

                Eigen::Vector3d pixel = C + ((j * delta + (delta / 2)) * r - (i * delta + (delta / 2)) * up);
                //Eigen::Vector3d pixel = (center + lookat + (((image_height - (((image_height - i) * 2 + 1) * delta)) / 2) * up) + (((image_width - ((j * 2 + 1) * delta)) / 2) * -r));
               // cout << pixel << endl;
                vec3 direc = pixel - origin;
                direc.normalize();
                Ray fromPixel(origin, direc);
                Eigen::Vector4d empty;
                empty << 100, 100, 100, 100;
                Eigen::Vector4d hit = ray_hit(fromPixel);
                color pixel_color;
                if (hit != empty) {
                    pixel_color = ray_color(fromPixel, hit);
                }
                else
                {
                    pixel_color = bkc;
                }

                write_color(out, pixel_color);
            }
        }
    }

    // Path Tracing
    else if (globalillum)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(-1.0f, 1.0f);
        cout << spheres.size() << endl;

        // Loop over all pixels
        for (int y = 0; y < image_height; y++) {
            for (int x = image_width - 1; x >= 0; --x) {
                vec3 pixelColor(0.0f, 0.0f, 0.0f);
                Eigen::Vector3d pixelPosition = C + ((x * delta + (delta / 2)) * r - (y * delta + (delta / 2)) * up);
                // Sample rays per pixel
                if (raysPerPixel.size() == 1) {
                    // Uniform random sampling
                    for (int i = 0; i < raysPerPixel[0]; i++) {
                        // Calculate the direction of the ray for the given pixel
                        float u = dis(gen) * delta;
                        float v = dis(gen) * delta;
                        vec3 samplePosition = pixelPosition + (u - delta / 2) * r + (v - delta / 2) * up;
                        vec3 rayDirection = (samplePosition - center);
                        rayDirection.normalize();
                        Ray ray(origin, rayDirection);
                        pixelColor += traceRay(ray, 0);
                    }
                    pixelColor /= raysPerPixel[0];
                    pixelColor = pixelColor.cwiseMin(vec3(1.0f, 1.0f, 1.0f));
                    write_color(out, pixelColor);
                }
                else if (raysPerPixel.size() == 2) {
                    // Stratified sampling
                    int a = raysPerPixel[0];
                    int b = raysPerPixel[1];
                    float invA = 1.0f / static_cast<float>(a);
                    //float invB = 1.0f / static_cast<float>(b);
                    for (int i = 0; i < a; i++) {
                        for (int j = 0; j < a; j++) {
                            for (int k = 0; k < b; k++) {
                                // Calculate the direction of the ray for the given pixel subregion
                                float u = (i + dis(gen)) * invA * delta;
                                float v = (j + dis(gen)) * invA * delta;
                                vec3 samplePosition = pixelPosition + (u - delta / 2) * r + (v - delta / 2) * up;
                                vec3 rayDirection = (samplePosition - center);
                                rayDirection.normalize();
                                Ray ray(origin, rayDirection);
                                pixelColor += traceRay(ray, 0);
                            }
                        }
                    }
                    pixelColor /= static_cast<float>(a * a * b);
                    pixelColor = pixelColor.cwiseMin(vec3(1.0f, 1.0f, 1.0f));
                    write_color(out, pixelColor);
                }
                else if (raysPerPixel.size() == 3) {
                    // Stratified sampling with grid
                    int a = raysPerPixel[0];
                    int b = raysPerPixel[1];
                    int c = raysPerPixel[2];
                    float invA = 1.0f / static_cast<float>(a);
                    float invB = 1.0f / static_cast<float>(b);

                    for (int i = 0; i < a; i++) {
                        for (int j = 0; j < b; j++) {
                            for (int k = 0; k < c; k++) {
                                // Calculate the direction of the ray for the given pixel subregion
                                float u = (i + dis(gen)) * invA * delta;
                                float v = (j + dis(gen)) * invB * delta;
                                vec3 samplePosition = pixelPosition + (u - delta / 2) * r + (v - delta / 2) * up;
                                vec3 rayDirection = (samplePosition - center);
                                rayDirection.normalize();
                                Ray ray(origin, rayDirection);
                                pixelColor += traceRay(ray, 0);
                            }
                        }
                    }
                    pixelColor /= static_cast<float>(a * b * c);
                    pixelColor = pixelColor.cwiseMin(vec3(1.0f, 1.0f, 1.0f));
                    write_color(out, pixelColor);
                }
            }
        }
    }
    // render
    out.close();
}