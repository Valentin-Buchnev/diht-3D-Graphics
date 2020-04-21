// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <random>
#include <ctime>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "shader.hpp"
#include "texture.hpp"
#include "controls.hpp"
#include "fireball_controller.hpp"
#include "enemy_controller.hpp"
//#include "global_variables.hpp"

int initialize_window() {
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(1024, 768, "Shooter game", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Hide the mouse and enable unlimited mouvement
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set the mouse at the center of the screen
	glfwPollEvents();
	glfwSetCursorPos(window, 1024 / 2, 768 / 2);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);
}

int main(void)
{
	srand(time(NULL));

	if (initialize_window() == -1) {
		return -1;
	}

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	size_t iter = 0;
	size_t last_enemy_ix = 0;


	FireballController fireballController = FireballController();
	EnemyController enemyController = EnemyController();

	time_t last_time_enemy_spawned = time(NULL);

	do {

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Вычислить MVP-матрицу в зависимости от положения мыши и нажатых клавиш
		computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();
		glm::mat4 ModelMatrix = glm::mat4(1.0);
		fireballController.updateMVP(ProjectionMatrix * ViewMatrix * ModelMatrix);
		enemyController.updateMVP(ProjectionMatrix * ViewMatrix * ModelMatrix);

		fireballController.spawn_fireball((glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS));

		if (time(NULL) - last_time_enemy_spawned > 5) {
			glm::vec3 center = getPosition() + getDirection() * (float)7. + glm::vec3((rand() % 5) / 5., (rand() % 5) / 5., (rand() % 5) / 5.);
			last_enemy_ix = enemyController.spawn_new_enemy(center.x, center.y, center.z);
			if (last_enemy_ix >= 10) {
				enemyController.kill_enemy(last_enemy_ix - 10);
			}
			printf("enemy spawned at %.2f %.2f %.2f ", center.x, center.y, center.z);
			glm::vec3 color;
			size_t sz = enemyController._colors_buffer_data.size();
			color.x = enemyController._colors_buffer_data[sz - 3];
			color.y = enemyController._colors_buffer_data[sz - 2];
			color.z = enemyController._colors_buffer_data[sz - 1];
			printf("with color %.2f %.2f %.2f\n", color.x, color.y, color.z);
			last_time_enemy_spawned = time(NULL);
		}

		enemyController.detect_collision(fireballController._collider, true);

		enemyController.draw();
		fireballController.draw();

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}