#ifndef GAME_COLLIDER_H
#define GAME_COLLIDER_H

class Collider {
private:
    float x, y, z, r;

public:
    Collider() : x(0), y(0), z(0), r(1) {}
    Collider(float x, float y, float z, float r) : x(x), y(y), z(z), r(r) {}
    ~Collider() = default;

    bool collides(const Collider& other) const;
};

#endif //GAME_COLLIDER_H