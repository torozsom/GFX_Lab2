#ifndef GONDOLA_H
#define GONDOLA_H

#include "Spline.h"


enum GondolaState { Waiting, Started, Fallen };


class Gondola {

    Spline* spline_;
    float progressAlongSpline_;
    float velocity_;
    float energy_;

    vec2 position_;
    float rotationAngle_;
    const float gondolaRadius_ = 1.0f;

    GondolaState state_;
    Geometry<vec2> body_;
    Geometry<vec2> spokes_;

  public:
    explicit Gondola(Spline* spline);

    vec2 derivative(float t) const;

    vec2 secondDerivative(float t) const;

    void start();

    void animate(float dt);

    void draw(GPUProgram* shader, const mat4& MVP);
};


#endif // GONDOLA_H
