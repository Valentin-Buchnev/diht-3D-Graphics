#ifndef GAME_ENEMY_CONTROLLER_H
#define GAME_ENEMY_CONTROLLER_H

#include <GL/glew.h>
#include <vector>
#include <stdio.h>
#include <ctime>
#include <random>
#include "global_variables.hpp"
#include "collider.hpp"

struct EnemyController {
    const static size_t _BUFFER_DATA_POINTS_PER_ENEMY = 72;

    std::vector<GLfloat> _vertices_buffer_data;
    std::vector<GLfloat> _colors_buffer_data;
    std::vector<uint32_t> _enemies_ixes;
    std::vector<Collider> _colliders;

    glm::mat4 _MVP;

    GLuint _program_ID;
    GLuint _MVP_matrix_ID;
    GLuint _vertices_buffer;
    GLuint _colors_buffer;

    uint32_t last_used_ix = 0;

    EnemyController();

    ~EnemyController();

    void _send_vertices_data_to_buffer();

    uint32_t spawn_new_enemy(float x_shift, float y_shift, float z_shift);

    void updateMVP(glm::mat4);

    size_t detect_collision(const Collider& collider, bool kill_in_case_of_collision = false);

    void kill_enemy(uint32_t enemy_ix);

    void draw();
};

#endif //GAME_ENEMY_CONTROLLER_H