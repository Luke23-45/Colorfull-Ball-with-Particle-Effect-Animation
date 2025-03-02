# Colourful Ball with Particle Effect Animation

A simple C++ project using SDL2 to render a bouncing ball with dynamic colors, particle effects, and a trailing rainbow effect.

## Getting Started

### Prerequisites
Ensure you have the following installed:
- C++ Compiler (e.g., g++)
- [SDL2](https://www.libsdl.org/)

## Project Structure

* `colorfull_ball.cc`: Main C++ source file.
* `Makefile`: Build the project.
* `src/`: Source code directory.
    * `include/`: Header files 
    * `lib/`: SDL related libraries 

### Installation
1. Clone the repository:
    ```bash
    git clone git@github.com:Luke23-45/Colorfull-Ball-with-Particle-Effect-Animation.git
    ```
## Building

This project uses `make` for building. To build the project, run the following command in your terminal:

1. Navigate to the project directory:
    ```bash
    cd Colorfull-Ball-with-Particle-Effect-Animation
    ```
3. Compile the code:
    ```bash
    make
    ```
4. Run the executable (Linux/macOS):
    ```bash
    ./Colorfull-Ball-with-Particle-Effect-Animation
    ```
5. Run the executable (Windows):
    ```bash
    main.exe
    ```
6. To clean up the build artifacts:
    ```bash
    make clean
    ```

## Features
- **Bouncing Ball Animation:**  A ball continuously bounces within the window boundaries.
- **Dynamic Ball Color:** The ball color changes dynamically, cycling through the hues of the rainbow.
- **Particle Effect:**  Particles are emitted from the ball as it moves, creating a visually appealing effect. The particles also change color dynamically.
- **Rainbow Trail:** The ball leaves a colorful trail behind it, with colors shifting along the trail, further enhancing the visual dynamics.

## Key Controls

| Action          | Key       |
| --------------- | --------- |
| Exit simulation | `ESC` key |

## Code Structure

The project is structured as follows:

- **`colorfull_ball.cc`**: This is the main source file containing all the C++ code.
    - **`main()` function**: Initializes SDL, sets up the window and renderer, contains the main game loop, handles events, updates ball and particle positions, and renders all elements.
    - **`HSVtoRGB(float h, float s, float v)` function**:  A utility function to convert HSV (Hue, Saturation, Value) color values to SDL_Color (RGB) format. This is used to generate dynamic rainbow colors.
    - **`SDL_RenderFillCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius)` function**: A utility function to draw filled circles using SDL_RenderDrawPoint.
    - **`TrailPoint` struct**:  Defines a structure to hold the x and y coordinates of a point in the ball's trail.
    - **`Particle` struct**: Defines a structure for particles, including position (`x`, `y`), velocity (`vx`, `vy`), life, maximum life, and base hue for color variation.

- **`Makefile`**:  Contains the build configurations for compiling the `colorfull_ball.cc` file into an executable. It should handle compiling with SDL2 and creating the executable.


## Demo Video
Check out the project demo video on YouTube: [https://www.youtube.com/watch?v=McOcbGHyAWA](https://www.youtube.com/watch?v=McOcbGHyAWA)

## License

This project is licensed under the MIT License. Feel free to use, modify, and distribute the code.

## Acknowledgements

- SDL2 for graphics rendering.
