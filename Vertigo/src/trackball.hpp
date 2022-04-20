#pragma once
#include "common.hpp"

// credit goes to http://www.songho.ca/opengl/gl_camera.html#lookat for trackball code
class Trackball
{
public:
    enum Mode
    {
        ARC = 0,
        PROJECT = 1
    };

    Trackball();
    Trackball(float radius, int screenWidth, int screenHeight);

    // setters
    void set(float r, int w, int h);
    void setScreenSize(int w, int h);
    void setRadius(float r)             { radius = r; }
    void setMode(Trackball::Mode m)  { this->mode = m; }

    // getters
    int getScreenWidth() const          { return screenWidth; }
    int getScreenHeight() const         { return screenHeight; }
    float getRadius() const             { return radius; }
    Trackball::Mode getMode() const     { return mode; }
    vec3 getVector(float x, float y) const;      // return a point on sphere for given mouse (x,y)
    vec3 getUnitVector(float x, float y) const;  // return normalized point for mouse (x, y)

protected:

private:
    // member functions
    vec3 getVectorWithArc(float x, float y) const;
    vec3 getVectorWithProject(float x, float y) const;
    float clampX(float x) const;                // -halfScreenWidth < x < halfScreenWidth
    float clampY(float y) const;                // -halfScreenHeight < y < halfScreenHeight

    // member variables
    float radius;
    int   screenWidth;
    int   screenHeight;
    float halfScreenWidth;
    float halfScreenHeight;
    Trackball::Mode mode;
};