# Gondola Spline Simulation

Welcome to the **Gondola Spline Simulation** project! This is a 2D graphics application built using OpenGL that lets you create a spline curve by adding control points and then watch a gondola move along it, influenced by gravity and centrifugal forces. It's designed as an educational tool for computer science students to explore the basics of computer graphics and physics simulation.

## Introduction

This project demonstrates how to:
- Use **OpenGL** to render 2D graphics, like curves and shapes.
- Simulate basic **physics** to animate a gondola moving along a custom spline path.
- Handle user interaction to shape the simulation.

You'll draw a spline (a smooth curve defined by control points) by clicking on the screen, then press the spacebar to start the gondola's journey along the curve. The gondola's motion respects physical principles, making this a fun way to learn graphics and simulation concepts!

## Background Knowledge

### OpenGL Basics

**OpenGL** (Open Graphics Library) is a powerful tool for drawing 2D and 3D graphics on your computer. Think of it as a set of instructions that talks to your graphics card (GPU) to render images.

- **Vertices**: These are points in space (like dots on a graph). In 2D, a vertex has an x and y coordinate. Everything we draw—lines, shapes—is made of vertices.


- **Geometry**: Connecting vertices forms shapes. For example, two vertices make a line, and a bunch of connected vertices can form a curve or a circle.


- **Rendering Pipeline**: This is the process OpenGL uses to turn your code into a picture:
    1. You define vertices in your program (on the CPU).
    2. These vertices are sent to the GPU.
    3. The GPU transforms them (e.g., moves or scales them) and decides what colors to fill in.
    4. The final image appears on your screen.
  

- **Shaders**: These are mini-programs that run on the GPU. We use two types:
    - **Vertex Shader**: Adjusts the position of each vertex (e.g., moving it to the right spot on the screen).
    - **Fragment Shader**: Colors each pixel (a tiny dot on the screen) based on the geometry.


- **Uniforms**: These are values (like colors or transformation matrices) we send from our C++ code to the shaders. They stay the same for all vertices or pixels during one draw call.


- **Matrices**: These are grids of numbers used to transform vertices. We use three kinds:
    - **Model Matrix**: Moves or rotates an object (like the gondola) in the world.
    - **View Matrix**: Shifts the world based on where the "camera" is looking.
    - **Projection Matrix**: Scales everything to fit on your screen properly.

In this project, OpenGL draws the spline curve, its control points, and the gondola, using shaders to handle positions and colors.

### Physics Concepts

The gondola's movement isn't random—it follows physics! Here's what's at play:

- **Gravity**: A downward force pulling the gondola. It affects how fast the gondola moves based on its height.
- **Centrifugal Force**: When the gondola curves along the spline, it feels an outward push. This can make it "fall off" if the curve is too sharp or it's moving too fast.
- **Energy Conservation**: The gondola's speed comes from converting potential energy (from height) into kinetic energy (motion). If it starts high, it speeds up as it drops.

Here are the key formulas we use:

1. **Velocity** (how fast the gondola moves):

   $$v = \sqrt{2 \cdot g \cdot (h_0 - h)}$$

    - $v$: Velocity (speed).
    - $g$: Gravity (set to 40 in our simulation).
    - $h_0$: Starting height of the gondola.
    - $h$: Current height.

   This formula shows that as the gondola drops ($h_0 - h$ increases), its speed grows.


2. **Curvature** (how sharp the spline bends):

   $$\kappa = \frac{| \mathbf{T}'(t) \times \mathbf{T}(t) |}{|\mathbf{T}(t)|^3}$$

    - $\kappa$: Curvature (bigger means a tighter curve).
    - $\mathbf{T}(t)$: Tangent vector (direction the spline is heading).
    - $\mathbf{T}'(t)$: How the tangent changes (from the second derivative).

   We approximate this using finite differences in the code.


3. **Total Force** (keeps the gondola on the path):

   $$F_{\text{total}} = \kappa \cdot v^2 + g \cdot \mathbf{N}_y$$

    - $F_{\text{total}}$: Combined force (must be positive to stay on the spline).
    - $\kappa \cdot v^2$: Centrifugal force (depends on speed and curve sharpness).
    - $g \cdot \mathbf{N}_y$: Gravity's effect along the normal direction ($\mathbf{N}_y$ is the y-part of the normal vector).

   If $F_{\text{total}}$ becomes negative, the gondola falls off!

These formulas are coded into the `Gondola` class to make the simulation realistic.

## Project Structure

The project is written in C++ and uses several classes to organize the code. Here's what each one does:

### Classes and Their Functions

1. **Spline**
    - **Purpose**: Creates and draws a smooth curve (a Catmull-Rom spline) based on control points you add.
    - **Why It's Needed**: The spline is the path the gondola follows. Without it, there'd be no track!
    - **How It Works**:
        - **Adding Points**: Click to add control points (`addControlPoint`). Each point gets a parameter $t$ (like a timestamp).
        - **Curve Calculation**: Uses Hermite interpolation (a math trick) to connect points smoothly (`evaluate`).
        - **Rendering**: Updates vertex data (`update`) and draws the curve in yellow and points in red (`draw`).
    - **Key Insight**: It's like drawing a curvy road by placing markers and letting math fill in the gaps.


2. **Camera**
    - **Purpose**: Controls what part of the 2D world you see and converts mouse clicks into world coordinates.
    - **Why It's Needed**: Without a camera, everything would be stuck in screen coordinates, and we couldn't zoom or pan.
    - **How It Works**:
        - **View Matrix**: Shifts the world based on the camera's center (`viewMatrix`).
        - **Projection Matrix**: Scales the world to fit the screen (`projectionMatrix`).
        - **Pixel to World**: Turns your mouse clicks into spline control points (`pixelToWorld`).
    - **Key Insight**: Think of it as a window into the simulation world, adjustable to show what you need.


3. **Gondola**
    - **Purpose**: Represents the moving object that follows the spline.
    - **Why It's Needed**: It's the star of the show! It brings the physics to life.
    - **How It Works**:
        - **Starting**: Sets initial position and energy when you press space (`start`).
        - **Animating**: Updates position, speed, and rotation using physics (`animate`).
        - **Drawing**: Renders a blue circle with white spokes, rotating as it moves (`draw`).
    - **Key Insight**: It's like a rollercoaster car, reacting to the track's shape and gravity.


4. **MyApp**
    - **Purpose**: Ties everything together—handles setup, rendering, and user input.
    - **Why It's Needed**: It's the "main" program that runs the simulation.
    - **How It Works**:
        - **Setup**: Creates the camera, spline, gondola, and shader (`onInitialization`).
        - **Rendering**: Draws everything each frame (`onDisplay`).
        - **Input**: Adds control points on clicks (`onMousePressed`) and starts the gondola on spacebar (`onKeyboard`).
        - **Animation**: Updates the gondola over time (`onTimeElapsed`).
    - **Key Insight**: It's the director, telling all the actors (classes) what to do.

### Shaders

Shaders are critical for telling the GPU how to draw our scene. We use two simple ones:

- **Vertex Shader**:
  ```glsl
  #version 330
  layout(location = 0) in vec2 cP;
  uniform mat4 MVP;
  void main() {
      gl_Position = MVP * vec4(cP, 0.0, 1.0);
  }
  ```
    - **Purpose**: Moves each vertex from the world to the screen.
    - **How It Works**:
        - Takes a 2D vertex position (`cP`).
        - Multiplies it by the **MVP** matrix (Model-View-Projection) to place it correctly.
        - Outputs `gl_Position`, which OpenGL uses to draw.
    - **Why It's Simple**: We're just positioning points, no fancy effects.


- **Fragment Shader**:
  ```glsl
  #version 330
  uniform vec3 color;
  out vec4 outColor;
  void main() {
      outColor = vec4(color, 1.0);
  }
  ```
    - **Purpose**: Colors each pixel.
    - **How It Works**:
        - Takes a color (`color`) from the C++ code.
        - Sets `outColor` with that color and full opacity (1.0).
    - **Why It's Simple**: We only need solid colors (yellow for the spline, red for points, blue/white for the gondola).

These shaders are basic but perfect for a 2D project like this. They let us focus on the simulation rather than complex visuals.

## Usage

### Prerequisites
- **C++ Compiler**: Like GCC or Visual Studio.
- **Libraries**: You'll need:
    - **GLFW**: For creating the window and handling input.
    - **GLAD**: For loading OpenGL functions.
    - **GLM**: For math (vectors and matrices).

### How to Run
1. **Clone the Repository**:
   ```bash
   git clone https://github.com/torozsom/GFX_Lab2.git
   ```
   

2. **Compile**:
    - Use a command like:
      ```bash
      g++ -o simulation *.cpp -lglfw -lGL -lGLAD
      ```
    - Adjust based on your setup (e.g., include paths for libraries).
   

3. **Run**:
   ```bash
   ./simulation
   ```

### Interacting with the Simulation
- **Add Control Points**: Left-click anywhere on the screen to place a red dot. These define the spline (yellow curve).
- **Start the Gondola**: Press the **spacebar** to launch the gondola from the start of the spline.
- **Watch**: See the gondola move, speeding up downhill and possibly falling off sharp curves!

## Conclusion

This project is a hands-on introduction to computer graphics and physics simulation. By playing with it, you'll learn:
- How OpenGL renders 2D scenes with vertices and shaders.
- How to use matrices to transform objects and views.
- How physics (gravity, centrifugal force) can animate objects realistically.
- How to structure a C++ program with classes for a graphics application.

Feel free to tweak the code—change the gravity, adjust the spline, or add new features—and see what happens! Happy coding!
