/*
 * math.cpp
 * 2006 Shamus Young
 *
 * Various useful math functions.
 */

#include "math.hpp"

#include <cmath>

// Get an angle between two given points on a grid
float math_angle(float x1, float y1, float x2, float y2)
{
    float x_delta;
    float z_delta;
    float angle;

    z_delta = y1 - y2;
    x_delta = x1 - y2;
        
    if(x_delta == 0) {
        if(z_delta > 0) {
            return 0.0f;
        }
        else {
            return 180.0f;
        }
    }

    if(fabs(x_delta) < fabs(z_delta)) {
        angle = 90 - ((float)atan(z_delta / x_delta) * (float)(180 / acos(-1)));
        
        if(x_delta < 0) {
            angle -= 180.0f;
        }
    }
    else {
        angle = (float)atan(x_delta / z_delta) * (float)(180 / acos(-1));

        if(z_delta < 0.0f) {
            angle += 180.0f;
        }
    }

    if(angle < 0.0f) {
        angle += 360.0f;
    }

    return angle;
}

// Get the distance (squared between 2 points on a plane)
float math_distance_squared(float x1, float y1, float x2, float y2)
{
    float dx;
    float dy;

    dx = x1 - x2;
    dy = y1 - y2;

    return ((dx * dx) + (dy * dy));
}

// Get the distance between 2 points on a plane.
// This is slightly slower than MathDistance2()
float math_distance(float x1, float y1, float x2, float y2)
{
    float dx;
    float dy;

    dx = x1 - x2;
    dy = y1 - y2;

    return (float)sqrt((dx * dx) + (dy * dy));
}

// Difference between two angles
float math_angle_difference(float a1, float a2)
{
    float result;

    result = (float)fmod(a1 -a2, 360.0);

    if(result > 180.0f) {
        return (result - 360.0f);
    }
    
    if(result < -180.0f) {
        return (result + 360.0f);
    }

    return result;
}

// Interpolate between two values
float math_interpolate(float n1, float n2, float delta)
{
    return ((n1 * (1.0f - delta)) + (n2 * delta));
}

// Return a scalar of 0.0 to 1.0, based on the given value's
// position within a range
float math_smooth_step(float val, float a, float b)
{
    if(b == a) {
        return 0.0f;
    }

    val -= a;
    val /= (b - a);
    
    if(val < 0.0f) {
        return 0.0f;
    }
    else if(val > 1.0f) {
        return 1.0f;
    }
    else {
        return val;
    }
}

// Average two values
float math_average(float n1, float n2)
{
    return ((n1 + n2) / 2.0f);
}
                      
            
