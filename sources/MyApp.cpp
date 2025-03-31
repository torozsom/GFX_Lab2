#include "Spline.h"
#include "Camera.h"
#include "Gondola.h"


const char *vertexSource = R"(
    #version 330
    layout(location = 0) in vec2 cP;
    uniform mat4 MVP;
    void main() {
        gl_Position = MVP * vec4(cP, 0.0, 1.0);
    }
)";


const char *fragmentSource = R"(
    #version 330
    uniform vec3 color;
    out vec4 outColor;
    void main() {
        outColor = vec4(color, 1.0);
    }
)";


/**
 * @class MyApp
 * @brief Application class for rendering and interacting with a 2D scene including a spline and gondola.
 *
 * Inherits from glApp to provide functionality for OpenGL-based rendering, event handling,
 * and animation. This class integrates a camera for world-space interaction, a spline
 * for defining a path, and a gondola for movement along the spline.
 */
class MyApp : public glApp {
    Camera *camera_;
    Spline *spline_;
    Gondola *gondola_;
    GPUProgram shader_;

public:
    MyApp() : glApp(4, 5, 600, 600, "Gondola Spline Simulation") {
    }


    /**
     * @brief Initializes resources and objects required for the application.
     *
     * This method overrides the base class onInitialization to set up the camera,
     * spline, gondola, and shader program. The camera is initialized with a specific
     * view range, the spline is created as the path for the gondola, and the gondola
     * is linked to the spline. A GPUProgram is created and initialized with vertex
     * and fragment shader source code.
     */
    void onInitialization() override {
        camera_ = new Camera(vec2(0, 0), vec2(20, 20));
        spline_ = new Spline();
        gondola_ = new Gondola(spline_);
        shader_.create(vertexSource, fragmentSource);
    }


    /**
     * @brief Renders the scene, including the spline and gondola, using the current camera view and projection.
     *
     * Clears the screen with a black background and sets up the Model-View-Projection (MVP)
     * matrix using the camera's viewProjectionMatrix. The spline and gondola are then drawn
     * using the shader and the computed MVP matrix.
     */
    void onDisplay() override {
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        mat4 MVP = camera_->viewProjectionMatrix();
        spline_->draw(&shader_, MVP);
        gondola_->draw(&shader_, MVP);
    }


    /**
     * @brief Handles mouse press events to add control points to the spline.
     *
     * Overrides glApp's `onMousePressed` method. If the left mouse button is
     * pressed, the method converts the given pixel coordinates to world space
     * using the camera and adds a new control point to the spline. The screen
     * is refreshed to reflect updates to the spline.
     *
     * @param button The mouse button that was pressed. Expected to be one of
     *               the values from the `MouseButton` enumeration.
     * @param pX The x-coordinate of the mouse in window coordinates.
     * @param pY The y-coordinate of the mouse in window coordinates.
     */
    void onMousePressed(const MouseButton button, const int pX, const int pY) override {
        if (button == MOUSE_LEFT) {
            vec2 world = camera_->pixelToWorld(vec2(pX, pY), vec2(600, 600));
            spline_->addControlPoint(world);
            refreshScreen();
        }
    }


    /**
     * @brief Handles keyboard input and triggers actions based on the key pressed.
     *
     * Overrides the base class implementation for handling keyboard events.
     * When the spacebar (' ') key is pressed, this method starts the gondola's movement
     * and refreshes the display to reflect updates.
     *
     * @param key The integer representation of the key that is pressed.
     *            For example, 32 represents the spacebar (' ').
     */
    void onKeyboard(const int key) override {
        if (key == ' ') {
            gondola_->start();
            refreshScreen();
        }
    }


    /**
     * @brief Handles time-based animation and screen updates.
     *
     * This method is called periodically to advance the gondola's animation along the spline within the specified time interval.
     * It processes the elapsed time in small increments and ensures the screen is refreshed after updates.
     *
     * @param startTime The starting point of the time interval for this update.
     * @param endTime The ending point of the time interval for this update.
     */
    void onTimeElapsed(const float startTime, const float endTime) override {
        float dt = 0.01f;
        for (float t = startTime; t < endTime; t += dt) {
            float Dt = fmin(dt, endTime - t);
            gondola_->animate(Dt);
        }
        refreshScreen();
    }
} app;
