#ifndef GAME_FIREBALL_CONTROLLER_H
#define GAME_FIREBALL_CONTROLLER_H

#include <GL/glew.h>
#include <vector>
#include <stdio.h>

#include "global_variables.hpp"
#include "shader.hpp"
#include "controls.hpp"
#include "texture.hpp"
#include "collider.hpp"

struct FireballController {

    glm::vec3 _vertices[N_PSI][N_PHI];
    bool _is_spawned = false;
    glm::vec3 _start_position, _velocity;
    float _time_from_last_spawn;
    glm::mat4 _MVP;
    Collider _collider;

    std::vector<GLfloat> _vertices_buffer_data;
    std::vector<GLfloat> _uv_buffer_data;

    GLuint _program_ID;
    GLuint _MVP_matrix_ID;
    GLuint _vertices_buffer;
    GLuint _uv_buffer;
    GLuint _texture;
    GLuint _texture_ID;


    FireballController();
    ~FireballController();

    void _send_vertices_data_to_buffer();

    void spawn_fireball(bool);

    void updateMVP(glm::mat4);

    void draw();
};

#endif //GAME_FIREBALL_CONTROLLER_H