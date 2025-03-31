#ifndef CAMERA_H
#define CAMERA_H


#include "framework.h"


/**
 * @class Camera
 * @brief Represents a 2D camera that provides transformations between world coordinates and normalized device coordinates (NDC).
 *
 * The Camera2D class manages a 2D viewing region in a world space and provides utilities for translation, scaling,
 * and conversion between coordinate systems. It includes methods for generating view and projection matrices,
 * as well as their inverses, to facilitate transformations and rendering in a 2D environment.
 */
class Camera {
    vec2 wCenter;
    vec2 wSize;

public:
    Camera(const vec2 center, const vec2 size);

    mat4 viewMatrix() const;

    mat4 projectionMatrix() const;

    mat4 viewMatrixInverse() const;

    mat4 projectionMatrixInverse() const;

    mat4 viewProjectionMatrix() const;

    vec2 pixelToWorld(const vec2 pixelPos, const vec2 windowSize) const;
};


#endif //CAMERA_H
