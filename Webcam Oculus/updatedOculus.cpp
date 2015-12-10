// Steve Fulton

#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#define OVR_OS_WIN32

#include "OpenGLApp.h"
#include "OculusRiftApp.h"
#include "Camera.h"

#include <iostream>

using namespace std;

// Tools for using OpenGL and OculusRift
OGLApp oglA;
OVRApp ovrA;

// glfwPollEvents() uses this for input
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode){
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

// Once program is done executing, cleanup
void done(){
	ovrA.destroy();
	oglA.destroy();
}

int main(){

	// Our camera
	Camera cam;
	int width, height;
	bool soFarSoGood;

	// initialize our Tools for OpenGL and OculusRift
	cout << "Initializing OpenGL" << endl;
	oglA.initGL();
	cout << "Initializing OculusRift" << endl;
	ovrA.initOVR();

	// getting the screen resolution based off of which monitor the Oculus Rift is on
	width = ovrA.getWidth();
	height = ovrA.getHeight();

	// will offset the window position if hmd is connected
	if (!oglA.createContext(width, height, ovrA.getXOffset(), key_callback))
		done(); // We can't do anything without this being successful

	// one texture for the entire rift
	GLuint texture = 0;

	cout << "Initializing OpenGL textures" << endl;
	oglA.initTextures(texture);
	cout << "Initializing OculusRift textures" << endl;
	ovrA.initTextures(texture);
	cout << "Initializing Rendering for the OculusRift" << endl;
	ovrA.initRendering();

	// The first camera device
	cout << "Start Camera Capturing" << endl;
	if (cam.startCapturing(0)){

		while (!glfwWindowShouldClose(oglA.window)){
			Mat frame;

			glfwPollEvents();

			ovrHmd_BeginFrame(ovrA.hmd, 0);

			// get a new frame if there is one
			if (cam.isNewFrame()){
				glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT);

				frame = cam.getImage(width, height);
				flip(frame.clone(), frame, 0);
			}

			//Get eye poses, feeding in correct IPD offset
			ovrVector3f ViewOffset[2] = { ovrA.eyeRDesc[0].HmdToEyeViewOffset, ovrA.eyeRDesc[1].HmdToEyeViewOffset };
			ovrPosef EyeRenderPose[2];
			ovrHmd_GetEyePoses(ovrA.hmd, 0, ViewOffset, EyeRenderPose, NULL);

			// bind the texture we are working with
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, frame.cols, frame.rows, GL_BGR, GL_UNSIGNED_BYTE, frame.data);
			glBindTexture(GL_TEXTURE_2D, 0);
			// unbind the texture once we are done updating it

			ovrHmd_EndFrame(ovrA.hmd, EyeRenderPose, ovrA.ovrTex);
		}
		// release the webcam
		cam.stopCapturing();
	}

	// cleanup
	done();

	return 0;
}