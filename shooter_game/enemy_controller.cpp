#include "enemy_controller.hpp"
#include "shader.hpp"
#include "global_variables.hpp"
//#include "collider.h"

EnemyController::EnemyController() {
    _program_ID = LoadShaders(
        "enemy.vertexshader",
        "enemy.fragmentshader");

    _MVP_matrix_ID = glGetUniformLocation(_program_ID, "MVP");

    glGenBuffers(1, &_vertices_buffer);
    glGenBuffers(1, &_colors_buffer);
}

void EnemyController::_send_vertices_data_to_buffer() {

    glBindBuffer(GL_ARRAY_BUFFER, _vertices_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * _vertices_buffer_data.size(), _vertices_buffer_data.data(), GL_DYNAMIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, _colors_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * _colors_buffer_data.size(), _colors_buffer_data.data(), GL_DYNAMIC_DRAW);

}

uint32_t EnemyController::spawn_new_enemy(float x_shift, float y_shift, float z_shift) {
    static const std::vector<GLfloat> enemy_surfaces = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.5f, 0.0f,
        0.0f, 0.0f, 1.0f,

        1.0f, 0.0f, 0.0f,
        0.0f, -1.5f, 0.0f,
        0.0f, 0.0f, 1.0f,

        -1.0f, 0.0f, 0.0f,
        0.0f, 1.5f, 0.0f,
        0.0f, 0.0f, 1.0f,

        -1.0f, 0.0f, 0.0f,
        0.0f, -1.5f, 0.0f,
        0.0f, 0.0f, 1.0f,

        1.0f, 0.0f, 0.0f,
        0.0f, 1.5f, 0.0f,
        0.0f, 0.0f, -1.0f,

        1.0f, 0.0f, 0.0f,
        0.0f, -1.5f, 0.0f,
        0.0f, 0.0f, -1.0f,

        -1.0f, 0.0f, 0.0f,
        0.0f, 1.5f, 0.0f,
        0.0f, 0.0f, -1.0f,

        -1.0f, 0.0f, 0.0f,
        0.0f, -1.5f, 0.0f,
        0.0f, 0.0f, -1.0f,
    };

    srand(time(NULL));

    glm::vec3 color = glm::vec3((rand() % 1000) / 1000., (rand() % 1000) / 1000., (rand() % 1000) / 1000.);

    for (size_t surface_begin_ix = 0; surface_begin_ix < enemy_surfaces.size(); surface_begin_ix += 3) {
        _vertices_buffer_data.push_back(enemy_surfaces[surface_begin_ix + 0] + x_shift);
        _vertices_buffer_data.push_back(enemy_surfaces[surface_begin_ix + 1] + y_shift);
        _vertices_buffer_data.push_back(enemy_surfaces[surface_begin_ix + 2] + z_shift);

        _colors_buffer_data.push_back(color.x);
        _colors_buffer_data.push_back(color.y);
        _colors_buffer_data.push_back(color.z);
    }

    _send_vertices_data_to_buffer();

    _enemies_ixes.push_back(last_used_ix++);
    _colliders.emplace_back(0.0 + x_shift, 0.0 + y_shift, 0.0 + z_shift, 1.414);
    return _enemies_ixes.back();
}

size_t EnemyController::detect_collision(const Collider& collider, bool kill_in_case_of_collision) {
    size_t enemy_ix = -1;

    for (size_t ix = 0; ix < _colliders.size(); ++ix) {
        if (collider.collides(_colliders[ix])) {
            enemy_ix = _enemies_ixes[ix];
            break;
        }
    }

    if (enemy_ix != -1 && kill_in_case_of_collision) {
        kill_enemy(enemy_ix);
    }

    return enemy_ix;
}

void EnemyController::kill_enemy(uint32_t enemy_ix) {
    size_t ix_in_vector = -1;

    for (size_t ix = 0; ix < _enemies_ixes.size(); ++ix) {
        if (_enemies_ixes[ix] == enemy_ix) {
            ix_in_vector = ix;
        }
    }

    assert(ix_in_vector != -1);

    auto enemy_begin_iterator = _vertices_buffer_data.begin() + ix_in_vector * _BUFFER_DATA_POINTS_PER_ENEMY;
    auto enemy_end_iterator = enemy_begin_iterator + _BUFFER_DATA_POINTS_PER_ENEMY;

    auto color_begin_iterator = _colors_buffer_data.begin() + ix_in_vector * _BUFFER_DATA_POINTS_PER_ENEMY;
    auto color_end_iterator = color_begin_iterator + _BUFFER_DATA_POINTS_PER_ENEMY;

    _vertices_buffer_data.erase(enemy_begin_iterator, enemy_end_iterator);
    _colors_buffer_data.erase(color_begin_iterator, color_end_iterator);
    _enemies_ixes.erase(_enemies_ixes.begin() + ix_in_vector);
    _colliders.erase(_colliders.begin() + ix_in_vector);

    _send_vertices_data_to_buffer();
}

void EnemyController::updateMVP(glm::mat4 new_MVP) {
    _MVP = new_MVP;
}

void EnemyController::draw() {
    glUseProgram(_program_ID);
    glUniformMatrix4fv(_MVP_matrix_ID, 1, GL_FALSE, &_MVP[0][0]);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, _vertices_buffer);
    glVertexAttribPointer(
        0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, _vertices_buffer);
    glVertexAttribPointer(
        1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );


    glDrawArrays(GL_TRIANGLES, 0, _vertices_buffer_data.size());
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    //glDeleteBuffers(1, &_vertices_buffer);
}

EnemyController::~EnemyController() {
    glDeleteBuffers(1, &_vertices_buffer);
    glDeleteBuffers(1, &_colors_buffer);
    glDeleteProgram(_program_ID);
}
