// Steve Fulton

// glew
#define GLEW_STATIC
#include <GL\glew.h>

// glfw
#include <GLFW\glfw3.h>
#include <GLFW\glfw3native.h>

#include "AppInterface.h"

class  AbstractGL: public AppInterface{

public:

	bool initialize(){
		// Init GLFW
		glfwInit();
		// Set all the required options for GLFW
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

		return true;
	}

	void shutdown(){
		glfwTerminate();
	}

	virtual bool createContext(int width, int height, int xOffset, GLFWkeyfun key_callback) = 0;
};