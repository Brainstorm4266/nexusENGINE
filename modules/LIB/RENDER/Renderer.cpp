#include "Renderer.hpp"
#include <ctime>
#include <chrono>
#include <thread>

void Renderer::render() {
    // TODO: Implement render args and stuffs
    // Start a clock for FPS calculation
    std::clock_t start = std::clock();
    // Render stuffs



    // End the clock
    std::clock_t end = std::clock();
    // Calculate FPS
    this->FPS = (float)CLOCKS_PER_SEC / (float)(end - start);
    // If render time is too low, wait for the MAX_FPS
    if (this->FPS < this->FPS_MAX) {
        std::this_thread::sleep_for(std::chrono::milliseconds(
            (int)((this->FPS_MAX - this->FPS) * 1000)
        ));
    }
}