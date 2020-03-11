// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <cmath>

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

const float PI = atan2(0., -1.);

const int N_PHI = 50;
const int N_PSI = 50;

GLfloat g_vertex_buffer_data[2 * N_PSI * N_PHI * 9];
GLfloat g_color_buffer_data[2 * N_PSI * N_PHI * 9];

int main( void )
{
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 1024, 768, "Tutorial 04 - Colored Cube", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders( "TransformVertexShader.vertexshader", "ColorFragmentShader.fragmentshader" );

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	// Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 Model      = glm::mat4(1.0f);
	
	glm::vec3 vertices[N_PSI][N_PHI];

	for (int j = 1; j < N_PSI; ++j) {
		for (int i = 0; i < N_PHI; ++i) {
			float phi = i * 2 * PI / N_PHI;
			float psi = j * PI / N_PSI;
			vertices[j][i] = glm::vec3(sin(psi) * cos(phi), sin(psi) * sin(phi), cos(psi));
		}
	}

	glm::vec3 north_pole = vec3(0., 0., 1.);
	glm::vec3 south_pole = vec3(0., 0., -1.);

	int cur_idx = 0;

	//add triangles for south pole
	for (int i = 0; i < N_PHI; ++i) {
		g_color_buffer_data[cur_idx] = g_vertex_buffer_data[cur_idx++] = north_pole.x;
		g_color_buffer_data[cur_idx] = g_vertex_buffer_data[cur_idx++] = north_pole.y;
		g_color_buffer_data[cur_idx] = g_vertex_buffer_data[cur_idx++] = north_pole.z;

		g_color_buffer_data[cur_idx] = g_vertex_buffer_data[cur_idx++] = vertices[1][i].x;
		g_color_buffer_data[cur_idx] = g_vertex_buffer_data[cur_idx++] = vertices[1][i].y;
		g_color_buffer_data[cur_idx] = g_vertex_buffer_data[cur_idx++] = vertices[1][i].z;

		g_color_buffer_data[cur_idx] = g_vertex_buffer_data[cur_idx++] = vertices[1][(i + 1) % N_PHI].x;
		g_color_buffer_data[cur_idx] = g_vertex_buffer_data[cur_idx++] = vertices[1][(i + 1) % N_PHI].y;
		g_color_buffer_data[cur_idx] = g_vertex_buffer_data[cur_idx++] = vertices[1][(i + 1) % N_PHI].z;
	}

	//add triangles for north pole
	for (int i = 0; i < N_PHI; ++i) {
		g_color_buffer_data[cur_idx] = g_vertex_buffer_data[cur_idx++] = south_pole.x;
		g_color_buffer_data[cur_idx] = g_vertex_buffer_data[cur_idx++] = south_pole.y;
		g_color_buffer_data[cur_idx] = g_vertex_buffer_data[cur_idx++] = south_pole.z;

		g_color_buffer_data[cur_idx] = g_vertex_buffer_data[cur_idx++] = vertices[N_PSI - 1][i].x;
		g_color_buffer_data[cur_idx] = g_vertex_buffer_data[cur_idx++] = vertices[N_PSI - 1][i].y;
		g_color_buffer_data[cur_idx] = g_vertex_buffer_data[cur_idx++] = vertices[N_PSI - 1][i].z;

		g_color_buffer_data[cur_idx] = g_vertex_buffer_data[cur_idx++] = vertices[N_PSI - 1][(i + 1) % N_PHI].x;
		g_color_buffer_data[cur_idx] = g_vertex_buffer_data[cur_idx++] = vertices[N_PSI - 1][(i + 1) % N_PHI].y;
		g_color_buffer_data[cur_idx] = g_vertex_buffer_data[cur_idx++] = vertices[N_PSI - 1][(i + 1) % N_PHI].z;
	}

	//add all sphere triangles
	for (int j = 1; j < N_PSI - 1; ++j) {
		for (int i = 0; i < N_PHI; ++i) {
			g_color_buffer_data[cur_idx] = g_vertex_buffer_data[cur_idx++] = vertices[j][i].x;
			g_color_buffer_data[cur_idx] = g_vertex_buffer_data[cur_idx++] = vertices[j][i].y;
			g_color_buffer_data[cur_idx] = g_vertex_buffer_data[cur_idx++] = vertices[j][i].z;

			g_color_buffer_data[cur_idx] = g_vertex_buffer_data[cur_idx++] = vertices[j + 1][i].x;
			g_color_buffer_data[cur_idx] = g_vertex_buffer_data[cur_idx++] = vertices[j + 1][i].y;
			g_color_buffer_data[cur_idx] = g_vertex_buffer_data[cur_idx++] = vertices[j + 1][i].z;

			g_color_buffer_data[cur_idx] = g_vertex_buffer_data[cur_idx++] = vertices[j + 1][(i + 1) % N_PHI].x;
			g_color_buffer_data[cur_idx] = g_vertex_buffer_data[cur_idx++] = vertices[j + 1][(i + 1) % N_PHI].y;
			g_color_buffer_data[cur_idx] = g_vertex_buffer_data[cur_idx++] = vertices[j + 1][(i + 1) % N_PHI].z;
		
			//////
			
			g_color_buffer_data[cur_idx] = g_vertex_buffer_data[cur_idx++] = vertices[j][i].x;
			g_color_buffer_data[cur_idx] = g_vertex_buffer_data[cur_idx++] = vertices[j][i].y;
			g_color_buffer_data[cur_idx] = g_vertex_buffer_data[cur_idx++] = vertices[j][i].z;

			g_color_buffer_data[cur_idx] = g_vertex_buffer_data[cur_idx++] = vertices[j + 1][(i + 1) % N_PHI].x;
			g_color_buffer_data[cur_idx] = g_vertex_buffer_data[cur_idx++] = vertices[j + 1][(i + 1) % N_PHI].y;
			g_color_buffer_data[cur_idx] = g_vertex_buffer_data[cur_idx++] = vertices[j + 1][(i + 1) % N_PHI].z;
		
			g_color_buffer_data[cur_idx] = g_vertex_buffer_data[cur_idx++] = vertices[j][(i + 1) % N_PHI].x;
			g_color_buffer_data[cur_idx] = g_vertex_buffer_data[cur_idx++] = vertices[j][(i + 1) % N_PHI].y;
			g_color_buffer_data[cur_idx] = g_vertex_buffer_data[cur_idx++] = vertices[j][(i + 1) % N_PHI].z;
			
		}
	}

	//normalize colors
	for (int i = 0; i < cur_idx; ++i) {
		g_color_buffer_data[i] += 1;
		g_color_buffer_data[i] /= 2.;
	}


	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	GLuint colorbuffer;
	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);

	float iter_cnt = 0.;

	do{

		// Camera matrix
		glm::mat4 View = glm::lookAt(
			glm::vec3(3 * sin(iter_cnt), 0, 3 * cos(iter_cnt)), // Camera is at (4,3,-3), in World Space
			glm::vec3(0, 0, 0), // and looks at the origin
			glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
		);
		// Our ModelViewProjection : multiplication of our 3 matrices
		glm::mat4 MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around

		iter_cnt += 0.001;

		if (iter_cnt >= 360)
			iter_cnt -= 360;


		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

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

		// 2nd attribute buffer : colors
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, (2 * N_PSI - 2) * N_PHI * 9);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &colorbuffer);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

