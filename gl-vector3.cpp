/*
 * gl-vector3.cpp
 * 2006 Shamus Young
 *
 * Functions for dealing with 3D vectors.
 */

#include "gl-vector3.hpp"

#include <cmath>
#include "math.hpp"

GLvector3 glVectorReflect(GLvector3 ray, GLvector3 normal)
{
    float dot;

    dot = glVectorDotProduct(ray, normal);
    
    return glVectorSubtract(ray, glVectorScale(normal, 2.0f * dot));
}

GLvector3 glVector(float x, float y, float z)
{
    GLvector3 result;

    result.x = x;
    result.y = y;
    result.z = z;

    return result;
}

GLvector3 glVectorInterpolate(GLvector3 v1, GLvector3 v2, float scalar)
{
    GLvector3 result;

    result.x = MathInterpolate(v1.x, v2.x, scalar);
    result.y = MathInterpolate(v1.y, v2.y, scalar);
    result.z = MathInterpolate(v1.z, v2.z, scalar);

    return result;
}

GLvector3 glVectorSubtract(GLvector3 v1, GLvector3 v2)
{
    GLvector3 result;

    result.x = v1.x - v2.x;
    result.y = v1.y - v2.y;
    result.z = v1.z - v2.z;

    return result;
}

GLvector3 glVectorAdd(GLvector3 v1, GLvector3 v2)
{
    GLvector3 result;

    result.x = v1.x + v2.x;
    result.y = v1.y + v2.y;
    result.z = v1.z + v2.z;

    return result;
}

float glVectorLength(GLvector3 v)
{
    return (float)sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

float glVectorDotProduct(GLvector3 v1, GLvector3 v2)
{
    return ((v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z));
}

GLvector3 glVectorCrossProduct(GLvector3 v1, GLvector3 v2)
{
    GLvector3 result;

    result.x = (v1.y * v2.z) - (v2.y * v1.z);
    result.y = (v1.z * v2.x) - (v2.z * v1.x);
    result.z = (v1.x * v2.y) - (v2.x * v1.y);

    return result;
}

GLvector3 glVectorInvert(GLvector3 v)
{
    v.x *= -v.x;
    v.y *= -v.y;
    v.z *= -v.z;

    return v;
}

GLvector3 glVectorScale(GLvector3 v, float scale)
{
    v.x *= scale;
    v.y *= scale;
    v.z *= scale;

    return v;
}

GLvector3 glVectorNormalize(GLvector3 v)
{
    float length;
    
    length = glVectorLength(v);

    if(length < 0.000001f) {
        return v;
    }

    return glVectorScale(v, 1.0f / length);
}
