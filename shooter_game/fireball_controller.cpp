#include "fireball_controller.hpp"

FireballController::FireballController() {
    _program_ID = LoadShaders(
        "fireball.vertexshader",
        "fireball.fragmentshader");

    _texture = loadBMP_custom("earthmap.bmp");
    _texture_ID = glGetUniformLocation(_program_ID, "fireballTexture");

    _MVP_matrix_ID = glGetUniformLocation(_program_ID, "MVP");

    glGenBuffers(1, &_uv_buffer);

    for (int j = 0; j < N_PSI; ++j) {
        for (int i = 0; i < N_PHI - 1; ++i) {
            float phi = i * 2 * PI / (N_PHI - 1);
            float psi = j * PI / (N_PSI - 1);
            _vertices[j][i] = glm::vec3(sin(psi) * cos(phi), sin(psi) * sin(phi), cos(psi));
        }
        _vertices[j][N_PHI - 1] = _vertices[j][N_PHI - 2];
    }

    // add texture  UV coordinates
    for (int j = 0; j < N_PSI - 1; ++j) {
        for (int i = 0; i < N_PHI; ++i) {
            _uv_buffer_data.push_back((atan2(_vertices[j][i].y, _vertices[j][i].x) / 2 / PI + 0.5));
            _uv_buffer_data.push_back(asin(_vertices[j][i].z) / PI + 0.5);

            _uv_buffer_data.push_back((atan2(_vertices[j + 1][i].y, _vertices[j + 1][i].x) / 2 / PI + 0.5));
            _uv_buffer_data.push_back(asin(_vertices[j + 1][i].z) / PI + 0.5);

            _uv_buffer_data.push_back((atan2(_vertices[j + 1][(i + 1) % N_PHI].y, _vertices[j + 1][(i + 1) % N_PHI].x) / 2 / PI + 0.5));
            _uv_buffer_data.push_back(asin(_vertices[j + 1][(i + 1) % N_PHI].z) / PI + 0.5);

            //////

            _uv_buffer_data.push_back((atan2(_vertices[j][i].y, _vertices[j][i].x) / 2 / PI + 0.5));
            _uv_buffer_data.push_back(asin(_vertices[j][i].z) / PI + 0.5);

            _uv_buffer_data.push_back((atan2(_vertices[j + 1][(i + 1) % N_PHI].y, _vertices[j + 1][(i + 1) % N_PHI].x) / 2 / PI + 0.5));
            _uv_buffer_data.push_back(asin(_vertices[j + 1][(i + 1) % N_PHI].z) / PI + 0.5);

            _uv_buffer_data.push_back((atan2(_vertices[j][(i + 1) % N_PHI].y, _vertices[j][(i + 1) % N_PHI].x) / 2 / PI + 0.5));
            _uv_buffer_data.push_back(asin(_vertices[j][(i + 1) % N_PHI].z) / PI + 0.5);

            size_t cur_size = _uv_buffer_data.size();

            if (_uv_buffer_data[cur_size - 2] < _uv_buffer_data[cur_size - 6]) {
                _uv_buffer_data[cur_size - 2] = 1;
                _uv_buffer_data[cur_size - 4] = 1;
                _uv_buffer_data[cur_size - 8] = 1;
            }
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, _uv_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * _uv_buffer_data.size(), _uv_buffer_data.data(), GL_DYNAMIC_DRAW);
}

void FireballController::_send_vertices_data_to_buffer() {
    glGenBuffers(1, &_vertices_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, _vertices_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * _vertices_buffer_data.size(), _vertices_buffer_data.data(), GL_DYNAMIC_DRAW);
}

void FireballController::spawn_fireball(bool respawn = false) {

    _vertices_buffer_data.resize(0);

    if (!respawn && !_is_spawned) {
        return;
    }

    if (!_is_spawned) {
        _is_spawned = true;
        _start_position = getPosition();
        _velocity = getDirection() * 5.0f;
        _time_from_last_spawn = 0;
    }

    glm::vec3 shift = _start_position + _velocity * _time_from_last_spawn;

    _collider = Collider(shift.x, shift.y, shift.z, radius);

    for (int j = 0; j < N_PSI - 1; ++j) {
        for (int i = 0; i < N_PHI; ++i) {

            _vertices_buffer_data.push_back(radius * _vertices[j][i].x + shift.x);
            _vertices_buffer_data.push_back(radius * _vertices[j][i].y + shift.y);
            _vertices_buffer_data.push_back(radius * _vertices[j][i].z + shift.z);

            _vertices_buffer_data.push_back(radius * _vertices[j + 1][i].x + shift.x);
            _vertices_buffer_data.push_back(radius * _vertices[j + 1][i].y + shift.y);
            _vertices_buffer_data.push_back(radius * _vertices[j + 1][i].z + shift.z);

            _vertices_buffer_data.push_back(radius * _vertices[j + 1][(i + 1) % N_PHI].x + shift.x);
            _vertices_buffer_data.push_back(radius * _vertices[j + 1][(i + 1) % N_PHI].y + shift.y);
            _vertices_buffer_data.push_back(radius * _vertices[j + 1][(i + 1) % N_PHI].z + shift.z);

            //////

            _vertices_buffer_data.push_back(radius * _vertices[j][i].x + shift.x);
            _vertices_buffer_data.push_back(radius * _vertices[j][i].y + shift.y);
            _vertices_buffer_data.push_back(radius * _vertices[j][i].z + shift.z);

            _vertices_buffer_data.push_back(radius * _vertices[j + 1][(i + 1) % N_PHI].x + shift.x);
            _vertices_buffer_data.push_back(radius * _vertices[j + 1][(i + 1) % N_PHI].y + shift.y);
            _vertices_buffer_data.push_back(radius * _vertices[j + 1][(i + 1) % N_PHI].z + shift.z);

            _vertices_buffer_data.push_back(radius * _vertices[j][(i + 1) % N_PHI].x + shift.x);
            _vertices_buffer_data.push_back(radius * _vertices[j][(i + 1) % N_PHI].y + shift.y);
            _vertices_buffer_data.push_back(radius * _vertices[j][(i + 1) % N_PHI].z + shift.z);
        }
    }

    _send_vertices_data_to_buffer();

    _time_from_last_spawn += 0.01;
    if (_time_from_last_spawn > 3) {
        _is_spawned = false; 
        glDeleteBuffers(1, &_vertices_buffer);
    }

    return;
}

void FireballController::updateMVP(glm::mat4 new_MVP) {
    _MVP = new_MVP;
}

void FireballController::draw() {
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
    glBindBuffer(GL_ARRAY_BUFFER, _uv_buffer);
    glVertexAttribPointer(
        1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
        2,                                // size : U+V => 2
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );

    glDrawArrays(GL_TRIANGLES, 0, _vertices_buffer_data.size());
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    glDeleteBuffers(1, &_vertices_buffer);
}

FireballController::~FireballController() {
    glDeleteBuffers(1, &_vertices_buffer);
    glDeleteBuffers(1, &_uv_buffer);
    glDeleteProgram(_program_ID);
    glDeleteTextures(1, &_texture);
}