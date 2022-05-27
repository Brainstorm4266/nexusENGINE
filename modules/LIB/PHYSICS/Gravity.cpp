#include "Physics.hpp"

// Accepts: Mass (in kilograms), and newtons per kilogram. Returns: Gravity (in newtons).
float Gravity::getGravity(float mass, float newton_per_kg) {
    return mass * newton_per_kg;
}
// Accepts: Gravity (in newtons), and newtons per kilogram. Returns mass in kilograms.
float Gravity::getMass(float gravity, float newton_per_kg) {
    return gravity / newton_per_kg;
}
// Accepts: Gravity (in newtons), and mass (in kilograms). Returns newtons per kilogram.
float Gravity::getNewtonPerKg(float gravity, float mass) {
    return gravity / mass;
}