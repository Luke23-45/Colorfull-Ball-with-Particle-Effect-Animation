#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <deque>
#include <cstdlib>
#include <cmath>

//----------------------------
// Utility: HSV to RGB conversion
// h: 0-360, s: 0-1, v: 0-1
//----------------------------
SDL_Color HSVtoRGB(float h, float s, float v) {
    int i = int(h / 60.0f) % 6;
    float f = (h / 60.0f) - i;
    float p = v * (1 - s);
    float q = v * (1 - f * s);
    float t = v * (1 - (1 - f) * s);
    float r, g, b;
    if(i == 0)      { r = v; g = t; b = p; }
    else if(i == 1) { r = q; g = v; b = p; }
    else if(i == 2) { r = p; g = v; b = t; }
    else if(i == 3) { r = p; g = q; b = v; }
    else if(i == 4) { r = t; g = p; b = v; }
    else            { r = v; g = p; b = q; }
    return SDL_Color{ (Uint8)(r * 255), (Uint8)(g * 255), (Uint8)(b * 255), 255 };
}

//----------------------------
// Utility: Draw a filled circle
//----------------------------
void SDL_RenderFillCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius) {
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w;
            int dy = radius - h;
            if ((dx * dx + dy * dy) <= (radius * radius)) {
                SDL_RenderDrawPoint(renderer, centerX + dx, centerY + dy);
            }
        }
    }
}

//----------------------------
// Structures for Trail and Particle effects
//----------------------------
struct TrailPoint {
    int x, y;
};

struct Particle {
    float x, y;
    float vx, vy;
    int life;      // current remaining life (frames)
    int maxLife;   // maximum life
    float baseHue; // initial hue (0-360) for dynamic color shifting
};

int main() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << "\n";
        return 1;
    }
    
    // Create window
    SDL_Window* window = SDL_CreateWindow("Bouncing Ball with Dynamic Colors", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
        640, 480, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << "\n";
        SDL_Quit();
        return 1;
    }
    
    // Create renderer with VSYNC and enable blending
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    
    // Ball variables
    int ballX = 100, ballY = 100;
    int ballRadius = 20;
    int ballSpeedX = 5, ballSpeedY = 5;
    
    // Trail settings
    const int MAX_TRAIL = 30;  // maximum number of trail points
    std::deque<TrailPoint> trail;
    
    // Particle settings
    std::vector<Particle> particles;
    const int PARTICLES_PER_FRAME = 30;
    
    // Seed randomness for particle effects
    std::srand((unsigned)SDL_GetTicks());
    
    // Main game loop
    bool quit = false;
    SDL_Event e;
    Uint32 frameStart, frameTime;
    
    while (!quit) {
        frameStart = SDL_GetTicks();
        
        // Handle events
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE)
                    quit = true;
            }
        }
        
        // Compute dynamic hue offset (cycles over time)
        float hueOffset = fmod(SDL_GetTicks() / 10.0f, 360.0f);
        
        // Move the ball
        ballX += ballSpeedX;
        ballY += ballSpeedY;
        
        // Bounce off walls
        if (ballX - ballRadius <= 0 || ballX + ballRadius >= 640)
            ballSpeedX = -ballSpeedX;
        if (ballY - ballRadius <= 0 || ballY + ballRadius >= 480)
            ballSpeedY = -ballSpeedY;
        
        // Add current ball position to the trail
        trail.push_front({ballX, ballY});
        if (trail.size() > MAX_TRAIL)
            trail.pop_back();
        
        // Spawn particles around the ball for extra effect
        for (int i = 0; i < PARTICLES_PER_FRAME; ++i) {
            Particle p;
            // Spawn near the ball with a small random offset
            p.x = ballX + (std::rand() % (ballRadius)) - ballRadius/2;
            p.y = ballY + (std::rand() % (ballRadius)) - ballRadius/2;
            // Random velocity between -2 and 2
            p.vx = ((std::rand() % 100) / 50.0f - 1.0f) * 4.0f;
            p.vy = ((std::rand() % 100) / 50.0f - 1.0f) * 4.0f;
            p.maxLife = p.life = 30 + std::rand() % 20;  // 30-50 frames lifetime
            // Set a random base hue for this particle
            p.baseHue = std::rand() % 360;
            particles.push_back(p);
        }
        
        // Update particles
        for (size_t i = 0; i < particles.size(); ) {
            Particle &p = particles[i];
            p.x += p.vx;
            p.y += p.vy;
            p.life--;
            if (p.life <= 0) {
                particles.erase(particles.begin() + i);
            } else {
                ++i;
            }
        }
        
        // Clear screen with black background
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        
        // Draw the trail behind the ball.
        // The older the trail point, the smaller and more transparent it becomes.
        int index = 0;
        for (const auto &tp : trail) {
            float factor = 1.0f - (float)index / MAX_TRAIL; // 1.0 at the ball, 0.0 at tail
            int trailRadius = (int)(ballRadius * factor);
            if (trailRadius < 1) trailRadius = 1;
            
            // Compute a dynamic rainbow color based on trail index and global hue offset.
            float hue = fmod(index * (360.0f / MAX_TRAIL) + hueOffset, 360.0f);
            SDL_Color trailColor = HSVtoRGB(hue, 1.0f, 1.0f);
            // Fade out alpha based on factor
            trailColor.a = (Uint8)(factor * 255);
            SDL_SetRenderDrawColor(renderer, trailColor.r, trailColor.g, trailColor.b, trailColor.a);
            SDL_RenderFillCircle(renderer, tp.x, tp.y, trailRadius);
            ++index;
        }
        
        // Draw particles around the ball with dynamic color
        for (const auto &p : particles) {
            float lifeRatio = (float)p.life / p.maxLife;
            // Each particle's dynamic hue is its base hue plus the global offset.
            float particleHue = fmod(p.baseHue + hueOffset, 360.0f);
            SDL_Color particleColor = HSVtoRGB(particleHue, 1.0f, 1.0f);
            particleColor.a = (Uint8)(lifeRatio * 255);
            SDL_SetRenderDrawColor(renderer, particleColor.r, particleColor.g, particleColor.b, particleColor.a);
            SDL_RenderFillCircle(renderer, (int)p.x, (int)p.y, 3);
        }
        
        // Draw the ball with a dynamic color based on the hue offset
        SDL_Color ballColor = HSVtoRGB(hueOffset, 1.0f, 1.0f);
        SDL_SetRenderDrawColor(renderer, ballColor.r, ballColor.g, ballColor.b, ballColor.a);
        SDL_RenderFillCircle(renderer, ballX, ballY, ballRadius);
        
        // Present the renderer
        SDL_RenderPresent(renderer);
        
        // Frame rate control (~60 FPS)
        frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < 16)
            SDL_Delay(16 - frameTime);
    }
    
    // Clean up
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
