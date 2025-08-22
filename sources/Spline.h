#ifndef SPLINE_H
#define SPLINE_H

#include "Camera.h"


vec2 Hermite(const vec2& p0, const vec2& v0, float t0, const vec2& p1,
             const vec2& v1, float t1, float t);


/**
 * @class Spline
 * @brief Represents a 2D Catmull-Rom spline curve.
 *
 * The Spline class allows the creation, evaluation, rendering, and modification
 * of a Catmull-Rom spline curve based on user-specified control points.
 * It supports dynamic updates when control points are added and provides
 * methods to evaluate the curve and its derivative at given parameter values.
 */
class Spline {

    std::vector<vec2> cps_;
    std::vector<float> ts_;
    Geometry<vec2> controlGeometry_;
    Geometry<vec2> curveGeometry_;

  public:
    void addControlPoint(vec2 cp);

    vec2 evaluate(float t) const;

    void update();

    void draw(GPUProgram* gpu, const mat4& MVP);

    const std::vector<float>& getKnots() const;
};


#endif // SPLINE_H
