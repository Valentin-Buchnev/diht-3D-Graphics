// Include standard headers
#include <stdio.h>
#include <stdlib.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/controls.hpp>

const float PI = atan2(0., -1.);

const int N_PHI = 50;
const int N_PSI = 50;

GLfloat g_vertex_buffer_data[3 * 6 * N_PSI * N_PHI];
GLfloat g_uv_buffer_data[2 * 6 * N_PSI * N_PHI];

void addFireTriangles(vec3 vertices[][N_PHI], vec3 shift, float radius=1) {
	size_t cur_idx = 0;

	for (int j = 0; j < N_PSI - 1; ++j) {
		for (int i = 0; i < N_PHI; ++i) {

			g_vertex_buffer_data[cur_idx++] = radius * vertices[j][i].x + shift.x;
			g_vertex_buffer_data[cur_idx++] = radius * vertices[j][i].y + shift.y;
			g_vertex_buffer_data[cur_idx++] = radius * vertices[j][i].z + shift.z;

			g_vertex_buffer_data[cur_idx++] = radius * vertices[j + 1][i].x + shift.x;
			g_vertex_buffer_data[cur_idx++] = radius * vertices[j + 1][i].y + shift.y;
			g_vertex_buffer_data[cur_idx++] = radius * vertices[j + 1][i].z + shift.z;

			g_vertex_buffer_data[cur_idx++] = radius * vertices[j + 1][(i + 1) % N_PHI].x + shift.x;
			g_vertex_buffer_data[cur_idx++] = radius * vertices[j + 1][(i + 1) % N_PHI].y + shift.y;
			g_vertex_buffer_data[cur_idx++] = radius * vertices[j + 1][(i + 1) % N_PHI].z + shift.z;

			//////

			g_vertex_buffer_data[cur_idx++] = radius * vertices[j][i].x + shift.x;
			g_vertex_buffer_data[cur_idx++] = radius * vertices[j][i].y + shift.y;
			g_vertex_buffer_data[cur_idx++] = radius * vertices[j][i].z + shift.z;

			g_vertex_buffer_data[cur_idx++] = radius * vertices[j + 1][(i + 1) % N_PHI].x + shift.x;
			g_vertex_buffer_data[cur_idx++] = radius * vertices[j + 1][(i + 1) % N_PHI].y + shift.y;
			g_vertex_buffer_data[cur_idx++] = radius * vertices[j + 1][(i + 1) % N_PHI].z + shift.z;

			g_vertex_buffer_data[cur_idx++] = radius * vertices[j][(i + 1) % N_PHI].x + shift.x;
			g_vertex_buffer_data[cur_idx++] = radius * vertices[j][(i + 1) % N_PHI].y + shift.y;
			g_vertex_buffer_data[cur_idx++] = radius * vertices[j][(i + 1) % N_PHI].z + shift.z;

		}
	}
}

int main( void )
{
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 1024, 768, "Tutorial 05 - Textured Cube", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
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

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders( "TransformVertexShader.vertexshader", "TextureFragmentShader.fragmentshader" );

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	// Load the texture using any two methods
	GLuint Texture = loadBMP_custom("earthmap.bmp");
	//GLuint Texture = loadDDS("uvtemplate.DDS");
	
	// Get a handle for our "myTextureSampler" uniform
	GLuint TextureID  = glGetUniformLocation(programID, "myTextureSampler");

	// Our vertices. Tree consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
	// A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices

	glm::vec3 vertices[N_PSI][N_PHI];

	for (int j = 0; j < N_PSI; ++j) {
		for (int i = 0; i < N_PHI - 1; ++i) {
			float phi = i * 2 * PI / (N_PHI - 1);
			float psi = j * PI / (N_PSI - 1);
			vertices[j][i] = glm::vec3(sin(psi) * cos(phi), sin(psi) * sin(phi), cos(psi));
		}
		vertices[j][N_PHI - 1] = vertices[j][N_PHI - 2];
	}


	//add all sphere triangles
	addFireTriangles(vertices, vec3(0., 0., 0.));

	size_t cur_idx = 0;
	// add texture  UV coordinates
	for (int j = 0; j < N_PSI - 1; ++j) {
		for (int i = 0; i < N_PHI; ++i) {
			g_uv_buffer_data[cur_idx++] = (atan2(vertices[j][i].y, vertices[j][i].x) / 2 / PI + 0.5);
			g_uv_buffer_data[cur_idx++] = asin(vertices[j][i].z) / PI + 0.5;

			g_uv_buffer_data[cur_idx++] = (atan2(vertices[j + 1][i].y, vertices[j + 1][i].x) / 2 / PI + 0.5);
			g_uv_buffer_data[cur_idx++] = asin(vertices[j + 1][i].z) / PI + 0.5;

			g_uv_buffer_data[cur_idx++] = (atan2(vertices[j + 1][(i + 1) % N_PHI].y, vertices[j + 1][(i + 1) % N_PHI].x) / 2 / PI + 0.5);
			g_uv_buffer_data[cur_idx++] = asin(vertices[j + 1][(i + 1) % N_PHI].z) / PI + 0.5;

			//////

			g_uv_buffer_data[cur_idx++] = (atan2(vertices[j][i].y, vertices[j][i].x) / 2 / PI + 0.5);
			g_uv_buffer_data[cur_idx++] = asin(vertices[j][i].z) / PI + 0.5;

			g_uv_buffer_data[cur_idx++] = (atan2(vertices[j + 1][(i + 1) % N_PHI].y, vertices[j + 1][(i + 1) % N_PHI].x) / 2 / PI + 0.5);
			g_uv_buffer_data[cur_idx++] = asin(vertices[j + 1][(i + 1) % N_PHI].z) / PI + 0.5;

			g_uv_buffer_data[cur_idx++] = (atan2(vertices[j][(i + 1) % N_PHI].y, vertices[j][(i + 1) % N_PHI].x) / 2 / PI + 0.5);
			g_uv_buffer_data[cur_idx++] = asin(vertices[j][(i + 1) % N_PHI].z) / PI + 0.5;


			if (g_uv_buffer_data[cur_idx - 2] < g_uv_buffer_data[cur_idx - 6]) {
				g_uv_buffer_data[cur_idx - 2] = 1;
				g_uv_buffer_data[cur_idx - 4] = 1;
				g_uv_buffer_data[cur_idx - 8] = 1;

			}
		}
	}

	//GLuint vertexbuffer;
	//glGenBuffers(1, &vertexbuffer);
	//glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data), g_uv_buffer_data, GL_STATIC_DRAW);

	bool fire_spawned = false;
	vec3 start_position, velocity;
	float time_from_last_spawn;

	do {

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		// Вычислить MVP-матрицу в зависимости от положения мыши и нажатых клавиш
		computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();
		glm::mat4 ModelMatrix = glm::mat4(1.0);
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		// Set our "myTextureSampler" sampler to use Texture Unit 0
		glUniform1i(TextureID, 0);

		bool F_pressed = (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS);

		if (F_pressed || fire_spawned) {

			if (!fire_spawned) {
				fire_spawned = true;
				start_position = getPosition();
				velocity = getDirection();
				time_from_last_spawn = 0;
			}

			addFireTriangles(vertices, start_position + velocity * time_from_last_spawn, 0.3);

			GLuint vertexbuffer;
			glGenBuffers(1, &vertexbuffer);
			glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

			// 1rst attribute buffer : vertices
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
			glVertexAttribPointer(
				0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
				3,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				0,                  // stride
				(void*)0            // array buffer offset
			);

			// 2nd attribute buffer : UVs
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
			glVertexAttribPointer(
				1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
				2,                                // size : U+V => 2
				GL_FLOAT,                         // type
				GL_FALSE,                         // normalized?
				0,                                // stride
				(void*)0                          // array buffer offset
			);

			// Draw the triangle !
			glDrawArrays(GL_TRIANGLES, 0, 6 * (N_PSI - 1) * N_PHI * 3);

			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);

			time_from_last_spawn += 0.01;
			if (time_from_last_spawn > 20) {
				fire_spawned = false;
			}

			glDeleteBuffers(1, &vertexbuffer);
		}

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup VBO and shader
	//glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteProgram(programID);
	glDeleteTextures(1, &Texture);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

