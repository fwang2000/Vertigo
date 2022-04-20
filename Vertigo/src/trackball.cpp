#include "trackball.hpp"

Trackball::Trackball() : radius(0), screenWidth(0), screenHeight(0),
                         halfScreenWidth(0), halfScreenHeight(0), mode(Trackball::ARC)
{
}
Trackball::Trackball(float radius, int width, int height) : radius(radius),
                                                            screenWidth(width),
                                                            screenHeight(height),
                                                            mode(Trackball::ARC)
{
    halfScreenWidth = width * 0.5f;
    halfScreenHeight = height * 0.5f;
}

void Trackball::set(float r, int w, int h)
{
    radius = r;
    screenWidth = w;
    screenHeight = h;
    halfScreenWidth = w * 0.5f;
    halfScreenHeight = h * 0.5f;
}

void Trackball::setScreenSize(int w, int h)
{
    screenWidth = w;
    screenHeight = h;
    halfScreenWidth = w * 0.5f;
    halfScreenHeight = h * 0.5f;
}

// clamp x and y
// the params of getVector() must be inside the window: -half < (x,y) < +half
float Trackball::clampX(float x) const
{
    if(x <= -halfScreenWidth)
        x = -halfScreenWidth + 1;
    else if(x >= halfScreenWidth)
        x = halfScreenWidth - 1;
    return x;
}

float Trackball::clampY(float y) const
{
    if(y <= -halfScreenHeight)
        y = -halfScreenHeight + 1;
    else if(y >= halfScreenHeight)
        y = halfScreenHeight - 1;
    return y;
}

// return the point coords on the sphere
vec3 Trackball::getVector(float x, float y) const
{
    if(radius == 0 || screenWidth == 0 || screenHeight == 0)
        return vec3(0,0,0);

    // compute mouse position from the centre of screen (-half ~ +half)
    float mx = x - halfScreenWidth;
    float my = halfScreenHeight - y;    // OpenGL uses bottom to up orientation
    // float mx = clampX(x - halfScreenWidth);
    // float my = clampY(halfScreenHeight - y);    // OpenGL uses bottom to up orientation

    if(mode == Trackball::PROJECT)
        return getVectorWithProject(mx, my);
    else
        return getVectorWithArc(mx, my); // default mode
}

// return the point on the sphere as a unit vector
vec3 Trackball::getUnitVector(float x, float y) const
{
    vec3 vec = getVector(x, y);
    return normalize(vec);
}

// use the mouse distance from the centre of screen as arc length on the sphere
// x = R * sin(a) * cos(b)
// y = R * sin(a) * sin(b)
// z = R * cos(a)
// where a = angle on x-z plane, b = angle on x-y plane
//
// NOTE: the calculation of arc length is an estimation using linear distance
// from screen center (0,0) to the cursor position
vec3 Trackball::getVectorWithArc(float x, float y) const
{
    float arc = sqrtf(x*x + y*y);   // legnth between cursor and screen center
    float a = arc / radius;         // arc = r * a
    float b = atan2f(y, x);         // angle on x-y plane

    // float a2 = a + M_PI/8.f;
    // float b2 = b + M_PI/16.f;
    // std::cout << "normal " << a << " " << b << "\n";

    float x2 = radius * sinf(a);    // x rotated by "a" on x-z plane

    vec3 vec;
    vec.x = x2 * cosf(b);
    vec.y = x2 * sinf(b);
    vec.z = radius * cosf(a);
    // std::cout << vec << "\n";

    // float x3 = radius * sinf(a2);
    // // std::cout << "translated " << a2 << " " << b2 << "\n";

    // vec3 vec2;
    // vec2.x = x3 * cosf(b2);
    // vec2.y = x3 * sinf(b2);
    // vec2.z = radius * cosf(a2);
    // // std::cout << vec2 << "\n";

    return vec;
}

// project the mouse coords to the sphere to find the point coord
// return the point on the sphere using hyperbola where x^2 + y^2 > r^2/2
vec3 Trackball::getVectorWithProject(float x, float y) const
{
    vec3 vec = vec3(x, y, 0);
    float d = x*x + y*y;
    float rr = radius * radius;

    // use sphere if d<=0.5*r^2:  z = sqrt(r^2 - (x^2 + y^2))
    if(d <= (0.5f * rr))
    {
        vec.z = sqrtf(rr - d);
    }
    // use hyperbolic sheet if d>0.5*r^2:  z = (r^2 / 2) / sqrt(x^2 + y^2)
    // referenced from trackball.c by Gavin Bell at SGI
    else
    {
        // compute z first using hyperbola
        vec.z = 0.5f * rr / sqrtf(d);

        // scale x and y down, so the vector can be on the sphere
        // y = ax => x^2 + (ax)^2 + z^2 = r^2 => (1 + a^2)*x^2 = r^2 - z^2
        // => x = sqrt((r^2 - z^2) / (1 - a^2)
        float x2, y2, a;
        if(x == 0.0f) // avoid dividing by 0
        {
            x2 = 0.0f;
            y2 = sqrtf(rr - vec.z*vec.z);
            if(y < 0)       // correct sign
                y2 = -y2;
        }
        else
        {
            a = y / x;
            x2 = sqrtf((rr - vec.z*vec.z) / (1 + a*a));
            if(x < 0)   // correct sign
                x2 = -x2;
            y2 = a * x2;
        }

        vec.x = x2;
        vec.y = y2;
    }

    return vec;
}
