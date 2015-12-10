// Steve Fulton

#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#define OVR_OS_WIN32

// GLEW
#define GLEW_STATIC
#include <GL\glew.h>	// include glew before other header files that require openGL


// Oculus Rift
#include <OVR_CAPI_GL.h>
#include <Kernel\OVR_System.h>
#include <Extras\OVR_Math.h>

// GLFW
#include <GLFW\glfw3.h>
#include <GLFW\glfw3native.h>

// OpenCV
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\core\opengl_interop.hpp>
#include <opencv2\core\cuda_devptrs.hpp>
#include <opencv2\opencv.hpp>

#include <iostream>

using namespace std;
using namespace cv;

// function prototypes
bool initGLFW();
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void draw(void);

const int width = 800;
const int height = 640;

static GLFWwindow* window;

int main(int argc, char* argv[]){

	if (!initGLFW()){
		cerr << "Could not initialize glfw. Closing application." << endl;
		return -1;
	}

	GLuint capTextureID;
	VideoCapture cap;
	cap.open(0);

	// initialze OpenGL texture		
	glEnable(GL_TEXTURE_RECTANGLE_ARB);

	glGenTextures(1, &capTextureID);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, capTextureID);

	glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	while (!glfwWindowShouldClose(window)){

		glfwPollEvents();

		Mat frame;

		bool success = cap.read(frame);
		flip(frame, frame, 1); // mirror the image
		flip(frame, frame, 0); // right side up

		if (success){

			// clear the buffers
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glDisable(GL_DEPTH_TEST);
			glDisable(GL_LIGHTING);
			glEnable(GL_TEXTURE_RECTANGLE_ARB);

			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluOrtho2D(0.0, (GLdouble)frame.cols, 0.0, (GLdouble)frame.rows);

			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			glBindTexture(GL_TEXTURE_RECTANGLE_ARB, capTextureID);

			if (frame.channels() == 3)
				glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGB, frame.cols, frame.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, frame.data);
			else if (frame.channels() == 4)
				glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, frame.cols, frame.rows, 0, GL_BGRA, GL_UNSIGNED_BYTE, frame.data);

			glBegin(GL_QUADS);
			glTexCoord2i(0, 0);
			glVertex2i(0, 0);
			glTexCoord2i(frame.cols, 0);
			glVertex2i(frame.cols, 0);
			glTexCoord2i(frame.cols, frame.rows);
			glVertex2i(frame.cols, frame.rows);
			glTexCoord2i(0, frame.rows);
			glVertex2i(0, frame.rows);
			glEnd();
		}

		glDisable(GL_TEXTURE_RECTANGLE_ARB);


		draw();

		glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();


	return 0;
}

bool initGLFW(){
	glfwInit();
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	window = glfwCreateWindow(width, height, "OpenCV OpenGL", nullptr, nullptr);

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

	return true;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode){
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

void draw(void)  {

	glDisable(GL_LIGHTING);
	glColor4f(1.0, 1.0, 1.0, 1.0);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, 200, 0, 200);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}