#include "Camera.h"


/**
 * @brief Constructs a Camera object with a specified center and size in world
 * space.
 *
 * This constructor initializes the camera by setting its center position and
 * size in world space coordinates. These parameters define the viewing area of
 * the camera.
 *
 * @param center The center position of the camera in world space coordinates.
 * @param size The size of the camera's viewport in world space dimensions.
 * @return A constructed Camera object.
 */
Camera::Camera(const vec2 center, const vec2 size)
    : wCenter(center), wSize(size) {}


/**
 * @brief Computes the view matrix for the camera in world space.
 *
 * This method calculates the view matrix by translating the world coordinates
 * based on the inverse of the camera's center position. The result is used
 * to determine how the world is viewed relative to the camera's position.
 *
 * @return The view matrix as a 4x4 matrix.
 */
mat4 Camera::viewMatrix() const {
    return translate(vec3(-wCenter.x, -wCenter.y, 0));
}


/**
 * @brief Computes the projection matrix for the camera.
 *
 * The P function generates the projection transformation matrix that maps
 * world space coordinates to normalized device coordinates. It achieves this
 * by scaling the coordinates based on the world size dimensions, ensuring
 * proper aspect ratio and alignment.
 *
 * @return A 4x4 matrix representing the projection transformation.
 */
mat4 Camera::projectionMatrix() const {
    return scale(vec3(2.0f / wSize.x, 2.0f / wSize.y, 1));
}


/**
 * @brief Computes the inverse view matrix for the camera.
 *
 * The Vinv function generates the transformation matrix that maps
 * world coordinates relative to the camera's center position. This
 * effectively reverses the view transformation, shifting the world
 * space back into the camera's reference frame.
 *
 * @return A 4x4 matrix that represents the inverse view transformation of the
 * camera.
 */
mat4 Camera::viewMatrixInverse() const {
    return translate(vec3(wCenter.x, wCenter.y, 0));
}


/**
 * @brief Computes the inverse projection matrix for the camera.
 *
 * The Pinv function returns a scaling matrix that is used to map
 * normalized device coordinates (NDC) to window coordinates based
 * on the size of the viewing window. This is primarily used to reverse
 * the projection transformation.
 *
 * @return A 4x4 matrix representing the inverse projection transformation.
 */
mat4 Camera::projectionMatrixInverse() const {
    return scale(vec3(wSize.x / 2.0f, wSize.y / 2.0f, 1));
}


/**
 * @brief Computes the combined View-Projection (VP) matrix of the camera.
 *
 * This method returns the product of the Projection (P) and View (V) matrices
 * of the camera. The VP matrix is used in transforming world coordinates
 * into clip coordinates for rendering.
 *
 * @return A 4x4 matrix representing the combined View-Projection
 * transformation.
 */
mat4 Camera::viewProjectionMatrix() const {
    return projectionMatrix() * viewMatrix();
}


/**
 * @brief Converts a position in pixel space to world-space coordinates.
 *
 * This method transforms a 2D position in screen pixel coordinates into
 * corresponding 2D world-space coordinates using the inverse of the Camera's
 * View and Projection matrices. This is useful for mapping user interactions
 * (e.g., mouse input) from screen space to the world space of the application.
 *
 * @param pixelPos A vec2 representing the position in pixel space.
 *                 The x and y values are the pixel coordinates on the screen.
 * @param windowSize A vec2 representing the size of the window in pixel space.
 *                   The x and y values correspond to the width and height of
 *                   the application window, respectively.
 *
 * @return A vec2 representing the corresponding position in world space.
 */
vec2 Camera::pixelToWorld(const vec2 pixelPos, const vec2 windowSize) const {
    const float ndcX = 2.0f * pixelPos.x / windowSize.x - 1.0f;
    const float ndcY = 1.0f - 2.0f * pixelPos.y / windowSize.y;
    const vec4 clipSpace(ndcX, ndcY, 0, 1);
    const vec4 world =
        projectionMatrixInverse() * viewMatrixInverse() * clipSpace;
    return vec2(world.x, world.y);
}
