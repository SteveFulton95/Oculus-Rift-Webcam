// Steve Fulton

// Referenced learnopengl.com
// Fragment and Vertest Shaders taken from http://learnopengl.com/#!Getting-started/Shaders

#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#define OVR_OS_WIN32

// GLEW
//#define GLEW_STATIC
//#include <GL\glew.h>	// include glew before other header files that require openGL

#include "Shader.h"
#include "Camera.h"

// GLFW
#include <GLFW\glfw3.h>
#include <GLFW\glfw3native.h>

// Oculus Rift
#include <OVR_CAPI_GL.h>
#include <Kernel\OVR_System.h>
#include <Extras\OVR_Math.h>

using namespace std;
using namespace cv;

// function prototypes
bool initGLFW();
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

const int WIDTH = 1280;
const int HEIGHT = 960;

static GLFWwindow* window;

int main(int argc, char* argv[]){

	if (!initGLFW()){
		cerr << "Could not initialize glfw. Closing application." << endl;
		return -1;
	}

	// Build and compile our shader program
	Shader ourShader("shader.vs", "shader.frag");
	
	GLfloat vertices[] = {
		// Positions          // Colors           // Texture Coords
		1.0f, 1.0, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,   // Top Right
		1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // Bottom Right
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,   // Bottom Left
		-1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f    // Top Left 
	};
	GLuint indices[] = {  // Note that we start from 0!
		0, 1, 3, // First Triangle
		1, 2, 3  // Second Triangle
	};
	GLuint VBO, VAO, EBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// TexCoord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_BGR, GL_UNSIGNED_BYTE, 0);	// a blank texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glBindTexture(GL_TEXTURE_2D, 0); // unbind texture

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindTexture(GL_TEXTURE_2D, texture);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	Camera cam;
	cam.startCapturing();

	while (!glfwWindowShouldClose(window)){

		glfwPollEvents();

		// get a new frame if there is one
		if (cam.isNewFrame()){

			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			Mat frame = cam.getImage(WIDTH, HEIGHT);
			flip(frame.clone(), frame, 0);
			
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, frame.cols, frame.rows, GL_BGR, GL_UNSIGNED_BYTE, frame.data);
		}

		// Activate shader
		ourShader.Use();

		// Draw container
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
	}

	// Properly de-allocate all resources once they've outlived their purpose
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	
	cam.stopCapturing();
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

bool initGLFW(){

	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	window = glfwCreateWindow(WIDTH, HEIGHT, "OpenCV OpenGL", nullptr, nullptr);

	if (window == NULL){
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);

	glewExperimental = true; // Needed in core profile 

	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return false;
	}

	// Define the viewport dimensions
	glViewport(0, 0, WIDTH, HEIGHT);

	return true;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode){
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}