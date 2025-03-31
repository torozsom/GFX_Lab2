#include "Spline.h"


/**
 * Computes a Hermite spline interpolation for a given parameter t.
 *
 * @param p0 The starting position of the Hermite curve segment.
 * @param v0 The starting tangent vector at p0.
 * @param t0 The parameter value corresponding to the starting point p0.
 * @param p1 The ending position of the Hermite curve segment.
 * @param v1 The ending tangent vector at p1.
 * @param t1 The parameter value corresponding to the ending point p1.
 * @param t The parameter value to evaluate the Hermite spline at.
 *
 * @return The interpolated position on the Hermite spline at parameter t.
 */
vec2 Hermite(const vec2 &p0, const vec2 &v0, float t0, const vec2 &p1, const vec2 &v1, float t1, float t) {
    float s = (t - t0) / (t1 - t0);
    vec2 a0 = p0;
    vec2 a1 = v0;
    vec2 a2 = (p1 - p0) * 3.0f / (t1 - t0) / (t1 - t0) - (v1 + 2.0f * v0) / (t1 - t0);
    vec2 a3 = (p0 - p1) * 2.0f / powf(t1 - t0, 3.0f) + (v1 + v0) / powf(t1 - t0, 2.0f);
    return ((a3 * s + a2) * s + a1) * s + a0;
}


/**
 * Adds a new control point to the spline.
 * The control point will be added to the list of control points and a corresponding
 * parameter value will be generated based on the number of existing points.
 * After adding the point, the spline is updated.
 *
 * @param cp The new control point to be added, represented as a 2D vector.
 */
void Spline::addControlPoint(const vec2 cp) {
    float t = cps_.empty() ? 0.0f : ts_.back() + 1.0f;
    cps_.push_back(cp);
    ts_.push_back(t);
    update();
}


/**
 * Evaluates the spline at the specified parameter t and returns the corresponding point on the curve.
 *
 * This function computes the interpolation using a Hermite spline between control points
 * that bound the parameter t. If the parameter t is outside the range of the spline,
 * it returns either the first or the last control point.
 *
 * @param t The parameter value at which to evaluate the spline. It is expected to be within
 *          the range defined by the time values of the spline control points.
 *
 * @return A vec2 representing the point on the spline at the specified parameter t.
 *         If the spline contains fewer than two control points, returns vec2(0, 0).
 */
vec2 Spline::evaluate(float t) const {
    if (cps_.size() < 2) return vec2(0, 0);

    for (int i = 0; i < int(cps_.size()) - 1; i++) {
        if (ts_[i] <= t && t <= ts_[i + 1]) {
            vec2 v0(0, 0), v1(0, 0);
            if (i > 0) v0 = (cps_[i + 1] - cps_[i - 1]) / (ts_[i + 1] - ts_[i - 1]);
            if (i < int(cps_.size()) - 2) v1 = (cps_[i + 2] - cps_[i]) / (ts_[i + 2] - ts_[i]);
            return Hermite(cps_[i], v0, ts_[i], cps_[i + 1], v1, ts_[i + 1], t);
        }
    }

    return cps_.back();
}


/**
 * @brief Updates the spline's geometry data and evaluates the curve at multiple sample points.
 *
 * This method performs the following steps:
 * - Copies the control points to the geometry used for rendering the control polygon.
 * - Updates the GPU for the control polygon geometry.
 * - Clears the current curve geometry vertices.
 * - If there are less than two control points, the method terminates after updating the GPU with an empty set of curve vertices.
 * - Otherwise, it evaluates the spline at evenly spaced sample points between the minimum and maximum parameter values.
 * - Adds the evaluated points to the curve geometry vertices.
 * - Finally, updates the GPU with the new set of curve vertices.
 */
void Spline::update() {
    controlGeometry_.Vtx() = cps_;
    controlGeometry_.updateGPU();
    curveGeometry_.Vtx().clear();

    if (cps_.size() < 2) {
        curveGeometry_.updateGPU();
        return;
    }

    int samples = 100;
    float tMin = ts_.front();
    float tMax = ts_.back();

    for (int i = 0; i <= samples; i++) {
        float t = tMin + (tMax - tMin) * i / samples;
        curveGeometry_.Vtx().push_back(evaluate(t));
    }

    curveGeometry_.updateGPU();
}


/**
 * Renders the spline curve and its control points using the GPU program and model-view-projection matrix.
 *
 * This method first sets the "MVP" uniform in the given GPU program with the provided matrix.
 * If the spline contains at least two control points, it draws the spline curve as a line strip
 * in yellow color with a line width of 3. All control points are then rendered as red points
 * with a size of 10.
 *
 * @param gpu A pointer to the GPUProgram used to set uniforms and render the spline.
 * @param MVP A constant reference to the 4x4 model-view-projection matrix used for rendering.
 */
void Spline::draw(GPUProgram *gpu, const mat4 &MVP) {
    gpu->setUniform(MVP, "MVP");

    if (cps_.size() >= 2) {
        glLineWidth(3.0f);
        curveGeometry_.Draw(gpu, GL_LINE_STRIP, vec3(1, 1, 0));
    }

    glPointSize(10);
    controlGeometry_.Draw(gpu, GL_POINTS, vec3(1, 0, 0));
}


/**
 * Retrieves the knots of the spline.
 *
 * This method returns a constant reference to the internal vector containing the knots
 * of the spline. The knots represent the parameter values that define the structure of the spline.
 *
 * @return A constant reference to the vector of knots.
 */
const std::vector<float> &Spline::getKnots() const { return ts_; }
