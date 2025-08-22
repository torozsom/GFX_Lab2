#include "Gondola.h"


/**
 * @brief Constructs a Gondola object and initializes its geometry and state.
 *
 * The Gondola is initialized with a specific spline for navigation and sets up
 * its circular geometry (body) along with spokes for visual representation.
 * It also initializes internal variables such as progress along the spline,
 * velocity, energy, position, and rotation angle.
 *
 * @param spline Pointer to a Spline object used by the Gondola for its path.
 * @return*/
Gondola::Gondola(Spline* spline)
    : spline_(spline), progressAlongSpline_(0), velocity_(0), energy_(0),
      position_(vec2(0, 0)), rotationAngle_(0), state_(Waiting) {
    constexpr int N = 32;
    body_.Vtx().push_back(vec2(0, 0));

    for (int i = 0; i <= N; i++) {
        const float theta = i * 2.0f * M_PI / N;
        body_.Vtx().push_back(
            vec2(gondolaRadius_ * cos(theta), gondolaRadius_ * sin(theta)));
    }

    body_.updateGPU();

    spokes_.Vtx() = {vec2(-1, 0), vec2(1, 0), vec2(0, -1), vec2(0, 1)};
    spokes_.updateGPU();
}


/**
 * Calculates the first derivative of the spline at a given parameter value.
 *
 * Uses a finite difference method with small step `h` to approximate the
 * derivative.
 *
 * @param t The parameter value at which the derivative is calculated.
 * @return The first derivative of the spline as a 2D vector.
 */
vec2 Gondola::derivative(const float t) const {
    constexpr float h = 0.001f;
    return (spline_->evaluate(t + h) - spline_->evaluate(t - h)) / (2.0f * h);
}


/**
 * Calculates the second derivative of the spline function at a given parameter
 * value.
 *
 * @param t The parameter value at which to evaluate the second derivative.
 * @return The second derivative of the spline at the specified parameter value
 * as a vec2.
 */
vec2 Gondola::secondDerivative(const float t) const {
    constexpr float h = 0.001f;
    return (spline_->evaluate(t + h) - 2.0f * spline_->evaluate(t) +
            spline_->evaluate(t - h)) /
           (h * h);
}


/**
 * @brief Starts the gondola movement if it is in the "Waiting" state.
 *
 * This method initializes several properties of the gondola including its
 * position, velocity, rotation angle, and energy, and transitions its state to
 * "Started". The position is calculated using a spline and its derivative to
 * define a tangential vector and a normal vector. Progress along the spline is
 * also initialized.
 *
 * The state of the gondola will not change or initiate if it is not already in
 * the "Waiting" state.
 */
void Gondola::start() {
    if (state_ == Waiting) {
        progressAlongSpline_ = 0.01f;
        velocity_ = 0;

        const vec2 r = spline_->evaluate(progressAlongSpline_);
        const vec2 T = normalize(derivative(progressAlongSpline_));
        const vec2 N = vec2(-T.y, T.x);

        position_ = r + N * gondolaRadius_;
        rotationAngle_ = 0.0f;
        energy_ = 40.0f * r.y + 0.5f;
        state_ = Started;
    }
}


/**
 * @brief Animates the gondola's state by updating its position, rotation, and
 * progress along the spline.
 *
 * This method computes the gondola's movement along a spline considering
 * physical forces such as gravity and centrifugal force. It evaluates the
 * spline geometry, calculates forces acting on the gondola, and updates its
 * velocity, position, and rotation angle. If the gondola exceeds the spline
 * bounds or experiences an unrealistic total force, its state is set to Fallen.
 *
 * @param dt The elapsed time since the last animation update, used for
 * calculating changes in position and velocity.
 */
void Gondola::animate(const float dt) {
    if (state_ != Started)
        return;

    constexpr float GRAVITY = 40.0f;  // Introduced constant
    constexpr float EPSILON = 0.001f; // Small value for stability checks

    // Evaluate spline and derivatives
    const vec2 position = spline_->evaluate(progressAlongSpline_);
    const vec2 tangent = derivative(progressAlongSpline_);
    const vec2 secondTangent = secondDerivative(progressAlongSpline_);
    const float tangentLength = length(tangent);
    if (tangentLength < EPSILON)
        return; // Prevent division by zero

    // Compute tangent & normal
    const vec2 normalizedTangent = tangent / tangentLength;
    const vec2 normal = vec2(-normalizedTangent.y, normalizedTangent.x);

    // Calculate forces
    const float currentHeight = (position + normal * gondolaRadius_).y;
    const float initialHeight =
        (spline_->evaluate(0.0f) + normal * gondolaRadius_).y;

    velocity_ = sqrt((2 * GRAVITY * (initialHeight - currentHeight)) / 2);

    const float curvature =
        (tangent.x * secondTangent.y - tangent.y * secondTangent.x) /
        pow(tangentLength, 3.0f);

    const float totalForce =
        curvature * velocity_ * velocity_ + GRAVITY * normal.y;

    if (totalForce < 0) {
        state_ = Fallen;
        return;
    }

    // Update position, rotation, and progress
    progressAlongSpline_ += (velocity_ * dt) / tangentLength;
    position_ = position + normal * gondolaRadius_;
    rotationAngle_ -= (velocity_ / gondolaRadius_) * dt;

    // Check if gondola exceeds spline bounds
    if (progressAlongSpline_ > spline_->getKnots().back())
        state_ = Fallen;
}


/**
 * @brief Draws the gondola using the specified shader program and
 * transformation matrix.
 *
 * This method does not perform any drawing if the gondola is in the Waiting
 * state. It applies the transformation matrix to the gondola's position and
 * rotation, sets the uniform values for the shader, and draws the gondola's
 * components (body and spokes). The body is drawn in two passes: once as a
 * filled shape and once as a wireframe*/
void Gondola::draw(GPUProgram* shader, const mat4& MVP) {
    if (state_ == Waiting)
        return;

    const mat4 M = translate(vec3(position_.x, position_.y, 0)) *
                   rotate(rotationAngle_, vec3(0, 0, 1));
    shader->setUniform(MVP * M, "MVP");
    body_.Draw(shader, GL_TRIANGLE_FAN, vec3(0.2f, 0.4f, 1.0f));
    body_.Draw(shader, GL_LINE_LOOP, vec3(1, 1, 1));
    spokes_.Draw(shader, GL_LINES, vec3(1, 1, 1));
}
