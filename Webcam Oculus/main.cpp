// Steve Fulton

#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#define OVR_OS_WIN32

#define TWO_CAMERAS 0;

#include "OpenGLApp.h"
#include "OculusRiftApp.h"
#include "Camera.h"

#include <iostream>

using namespace std;

// glfwPollEvents() uses this for input
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode){
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

int main(){
	OGLApp oglA;
	OVRApp ovrA;
	int width, height;

	oglA.initialize();
	ovrA.initialize();
	ovrA.setup();

	// getting the screen resolution
	width = ovrA.getWidth();
	height = ovrA.getHeight();

	oglA.createContext(width, height, ovrA.getXOffset(), key_callback); // offsets the window position

#if TWO_CAMERAS

	// one texture for each eye - left and right
	Camera left, right;
	GLuint lTex = 0;
	GLuint rTex = 0;

	oglA.initTextures(lTex, rTex);
	ovrA.initTextures(lTex, rTex);
	ovrA.initRendering();

	left.startCapturing(0);
	right.startCapturing(1);

	while (!glfwWindowShouldClose(oglA.window)){
		Mat lFrame, rFrame;

		glfwPollEvents();

		ovrFrameTiming hmdFrameTiming = ovrHmd_BeginFrameTiming(ovrA.hmd, 0);
		ovrHmd_BeginFrame(ovrA.hmd, 0);

		// get a new frame if there is one
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		lFrame = left.getImage(width, height);
		flip(lFrame.clone(), lFrame, 0);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		rFrame = right.getImage(width, height);
		flip(rFrame.clone(), rFrame, 0);

		//Get eye poses, feeding in correct IPD offset
		ovrVector3f ViewOffset[2] = { ovrA.eyeRDesc[0].HmdToEyeViewOffset, ovrA.eyeRDesc[1].HmdToEyeViewOffset };
		ovrPosef EyeRenderPose[2];
		ovrHmd_GetEyePoses(ovrA.hmd, 0, ViewOffset, EyeRenderPose, NULL);

		glBindTexture(GL_TEXTURE_2D, lTex);	// bind
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, lFrame.cols, lFrame.rows, GL_BGR, GL_UNSIGNED_BYTE, lFrame.data);
		glBindTexture(GL_TEXTURE_2D, 0);	// unbind

		glBindTexture(GL_TEXTURE_2D, rTex);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, rFrame.cols, rFrame.rows, GL_BGR, GL_UNSIGNED_BYTE, rFrame.data);
		glBindTexture(GL_TEXTURE_2D, 0);

		ovrHmd_EndFrame(ovrA.hmd, EyeRenderPose, ovrA.ovrTex);
		ovrHmd_EndFrameTiming(ovrA.hmd);
	}

	left.stopCapturing();
	right.stopCapturing();

#else

	Camera cam;
	GLuint texture = 0;

	oglA.initTextures(texture);
	ovrA.initTextures(texture);
	ovrA.initRendering();

	cam.startCapturing(0);

	while (!glfwWindowShouldClose(oglA.window)){
		Mat frame;

		glfwPollEvents();

		ovrFrameTiming hmdFrameTiming = ovrHmd_BeginFrameTiming(ovrA.hmd, 0);
		ovrHmd_BeginFrame(ovrA.hmd, 0);

		// get a new frame if there is one
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		frame = cam.getImage(width, height);
		flip(frame.clone(), frame, 0);

		//Get eye poses, feeding in correct IPD offset
		ovrVector3f ViewOffset[2] = { ovrA.eyeRDesc[0].HmdToEyeViewOffset, ovrA.eyeRDesc[1].HmdToEyeViewOffset };
		ovrPosef EyeRenderPose[2];
		ovrHmd_GetEyePoses(ovrA.hmd, 0, ViewOffset, EyeRenderPose, NULL);

		glBindTexture(GL_TEXTURE_2D, texture);	// bind
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, frame.cols, frame.rows, GL_BGR, GL_UNSIGNED_BYTE, frame.data);
		glBindTexture(GL_TEXTURE_2D, 0);	// unbind

		ovrHmd_EndFrame(ovrA.hmd, EyeRenderPose, ovrA.ovrTex);
		ovrHmd_EndFrameTiming(ovrA.hmd);
	}

	cam.stopCapturing();

#endif

	ovrA.shutdown();
	oglA.shutdown();

	return 0;
}