// Steve Fulton

#ifndef OCULUSRIFTAPP_H
#define OCULUSRIFTAPP_H

// Oculus Rift
#include <OVR_CAPI_GL.h>

#include <iostream>

#include "AbstractOVR.h"

using namespace std;
using namespace OVR;

class OVRApp : public AbstractOVR{
	
private:
	bool debug;
	int xOffset;
	int hmdCaps;
	int width, height;
	int distortionCaps;				// for sdk rendering
	ovrGLConfig cfg;				// configuring ovr with our gl paramaters
	ovrSizei eyeRes;	// resolution for each eye

public:
	ovrEyeRenderDesc eyeRDesc[2];	// for sdk rendering
	ovrTexture ovrTex[2];			// ovr textures that is the render target texture
	ovrHmd hmd;						// the head mounted display

	OVRApp(){
		xOffset = 0;
		debug = false;
	}

	// Project Specific configurations
	void setup(){
		hmd = ovrHmd_Create(0);
		if (hmd == nullptr){
			hmd = ovrHmd_CreateDebug(ovrHmd_DK2);
			debug = true;
		}

		// Configure Rift Tracking
		ovrHmd_ConfigureTracking(hmd, ovrTrackingCap_Orientation |
			ovrTrackingCap_MagYawCorrection |
			ovrTrackingCap_Position, 0);

		// finding the oculus rift monitor
		// set the width and height
		int count = 0;
		int yPos;
		GLFWmonitor** monitors = glfwGetMonitors(&count);
		for (int i = 0; i < count; i++){
			if (strcmp(glfwGetWin32Monitor(monitors[i]), hmd->DisplayDeviceName) == 0){
				glfwGetMonitorPos(monitors[i], &xOffset, &yPos);
				const GLFWvidmode* mode = glfwGetVideoMode(monitors[i]);
				width = mode->width;
				height = mode->height;
			}
			else{
				GLFWmonitor* monitor = glfwGetPrimaryMonitor();
				const GLFWvidmode* mode = glfwGetVideoMode(monitor);
				width = mode->width;
				height = mode->height;
			}
		}

		for (int i = 0; i < 2; i++){
			eyeRes = ovrHmd_GetFovTextureSize(hmd, (ovrEyeType)i, hmd->DefaultEyeFov[i], 1.0f);
		}
	}

	void initTextures(GLuint &left, GLuint& right){
		// fill in the ovrGLTexture structures that describe our render target texture
		for (int i = 0; i < 2; i++) {
			ovrTex[i].Header.API = ovrRenderAPI_OpenGL;
			ovrTex[i].Header.TextureSize = eyeRes;
			ovrTex[i].Header.RenderViewport = Recti(Vector2i(0, 0), eyeRes);
		}
		((ovrGLTexture&)(ovrTex[0])).OGL.TexId = left;
		((ovrGLTexture&)(ovrTex[1])).OGL.TexId = right;
	}

	void initTextures(GLuint &texture){
		// fill in the ovrGLTexture structures that describe our render target texture
		for (int i = 0; i < 2; i++) {
			ovrTex[i].Header.API = ovrRenderAPI_OpenGL;
			ovrTex[i].Header.TextureSize = eyeRes;
			ovrTex[i].Header.RenderViewport = Recti(Vector2i(0, 0), eyeRes);
			((ovrGLTexture&)(ovrTex[i])).OGL.TexId = texture;
		}	
	}

	void initRendering(){

		// setting up the configurations
		memset(&cfg, 0, sizeof cfg);
		cfg.OGL.Header.API = ovrRenderAPI_OpenGL;
		cfg.OGL.Header.BackBufferSize.w = width;
		cfg.OGL.Header.BackBufferSize.h = height;
		cfg.OGL.Header.Multisample = 1;
		cfg.OGL.Window = GetActiveWindow();
		cfg.OGL.DC = wglGetCurrentDC();

		distortionCaps = ovrDistortionCap_TimeWarp | ovrDistortionCap_Overdrive;
		ovrHmd_ConfigureRendering(hmd, &cfg.Config, distortionCaps, hmd->DefaultEyeFov, eyeRDesc);

		ovrHmd_AttachToWindow(hmd, cfg.OGL.Window, 0, 0);

		// low persistent display and dynamic prediction for latency
		hmdCaps = ovrHmdCap_LowPersistence | ovrHmdCap_DynamicPrediction;
		ovrHmd_SetEnabledCaps(hmd, hmdCaps);
	}

	int getXOffset(){
		return xOffset;
	}

	ovrSizei getEyeRes(){
		return eyeRes;
	}

	int getWidth(){
		return width;
	}

	int getHeight(){
		return height;
	}

	bool isDebug(){
		return debug;
	}
};

#endif  OCULUSRIFTAPP_H