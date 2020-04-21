#include <cstdlib>
#include <array>
#include <cmath>
#include "collider.hpp"

bool Collider::collides(const Collider& other) const {
    float distance = 0;

    distance += (x - other.x) * (x - other.x);
    distance += (y - other.y) * (y - other.y);
    distance += (z - other.z) * (z - other.z);
    distance = sqrtf(distance);

    float r_sum = r + other.r;

    return distance <= r_sum;
}
