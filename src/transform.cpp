#include "transform.h"
#include <cmath>

ObjectTransform::ObjectTransform(Vector2 position, float rotation, float scale)
        : position(position), rotation(rotation), scale(scale) 
    {}

float ObjectTransform::getRotationRad() const
{
    float degrees = fmod(rotation, 360.0f);
    if (degrees < 0.0f) degrees += 360.0f;

    return degrees * (PI / 180.0f);
}

void ObjectTransform::setRotationRad(float newRadians)
{
    float degrees = newRadians * (180.0f / PI);
    degrees = fmod(degrees, 360.0f);
    if (degrees < 0.0f) degrees += 360.0f;

    rotation = degrees;
}

void ObjectTransform::setScale(float newScale)
{
    if (newScale > 0.0f && newScale < 20.0f)
    {
        scale = newScale;
    }
}