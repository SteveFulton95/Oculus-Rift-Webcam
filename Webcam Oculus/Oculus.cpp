// Steve Fulton

// Referenced learnopengl.com
// Fragment and Vertest Shaders taken from http://learnopengl.com/#!Getting-started/Shaders

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

#include <iostream>

#include "Shader.h"
#include "Camera.h"
#include "GLUtils.h"

using namespace std;
using namespace cv;
using namespace OVR;

// function prototypes
bool initGLFW(void);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
bool initOVR(void);
void drawScene(GLuint&);

int WIDTH = 800;
int HEIGHT = 640;

static GLFWwindow* window;

int main(int argc, char* argv[]){

	if (!ovr_Initialize()){
		cerr << "Could not initialize OVR libraries. Closing application." << endl;
		return -2;
	}

	ovrHmd hmd = ovrHmd_Create(0);
	if (hmd == nullptr){
		hmd = ovrHmd_CreateDebug(ovrHmd_DK2);
	}

	ovrSizei resolution = hmd->Resolution;

	if (!initGLFW()){
		cerr << "Could not initialize glfw. Closing application." << endl;
		return -1;
	}

	// Configure Rift Tracking
	try{
		ovrHmd_ConfigureTracking(hmd, ovrTrackingCap_Orientation |
			ovrTrackingCap_MagYawCorrection |
			ovrTrackingCap_Position, 0);
	}
	catch (int exception){
		std::cout << "An exception occured. Exception: " << exception << std::endl;
	}

	// get target resolution for each eye
	ovrSizei eyeRes[2];
	eyeRes[0] = ovrHmd_GetFovTextureSize(hmd, ovrEye_Left, hmd->DefaultEyeFov[0], 1.0f);
	eyeRes[1] = ovrHmd_GetFovTextureSize(hmd, ovrEye_Right, hmd->DefaultEyeFov[0], 1.0f);

	// create a single render Texture for both eyes
	int width = eyeRes[0].w + eyeRes[1].w;
	int height = max(eyeRes[0].h, eyeRes[1].h);

	bool windowed = (hmd->HmdCaps & ovrHmdCap_ExtendDesktop) ? false : true;
	//if (windowed){
	//	window = glfwCreateWindow(WIDTH, HEIGHT, "OpenCV - OpenGL - Oculus Rift", nullptr, nullptr);
	//	width = WIDTH;
	//	height = HEIGHT;
	//}
	//else{
		window = glfwCreateWindow(resolution.w, resolution.h, "LearnOpenGL", nullptr, nullptr);
	//}
	

	if (window == NULL){
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowPos(window, 0, 0);
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);

	// Define the viewport dimensions
	glViewport(0, 0, WIDTH, HEIGHT);

	glewExperimental = true; // Needed in core profile 

	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -2;
	}

	/* calculate the next power of two in both dimensions and use that as a texture size */
	GLuint fb_tex_width = pow(2, ceil(log(width) / log(2)));
	GLuint fb_tex_height = pow(2, ceil(log(height) / log(2)));

	/*// Build and compile our shader program
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

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

	glBindVertexArray(0); // Unbind VAO*/

	GLuint texture, frameBuffer, renderBuffer;

	/*glGenFramebuffers(1, &frameBuffer);
	glGenTextures(1, &texture);
	glGenRenderbuffers(1, &renderBuffer);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	/* create and attach the texture that will be used as a color buffer 
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_BGR, fb_tex_width, fb_tex_height, 0,
		GL_BGR, GL_UNSIGNED_BYTE, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

	/* create and attach the renderbuffer that will serve as our z-buffer 
	glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, fb_tex_width, fb_tex_height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);*/

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fb_tex_width, fb_tex_height, 0, GL_BGR, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glBindTexture(GL_TEXTURE_2D, 0); // unbind texture

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindTexture(GL_TEXTURE_2D, texture);
	//glBindVertexArray(VAO);
	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	//glBindVertexArray(0);

	/* fill in the ovrGLTexture structures that describe our render target texture */
	ovrGLTexture ovrTex[2];
	for (int i = 0; i < 2; i++) {
		ovrTex[i].OGL.Header.API = ovrRenderAPI_OpenGL;
		ovrTex[i].OGL.Header.TextureSize.w = fb_tex_width;
		ovrTex[i].OGL.Header.TextureSize.h = fb_tex_height;

		/* this next field is the only one that differs between the two eyes */
		ovrTex[i].OGL.Header.RenderViewport.Pos.x = i == 0 ? 0 : width / 2.0;
		ovrTex[i].OGL.Header.RenderViewport.Pos.y = 0;
		ovrTex[i].OGL.Header.RenderViewport.Size.w = width / 2.0;
		ovrTex[i].OGL.Header.RenderViewport.Size.h = height;
		ovrTex[i].OGL.TexId = texture;	/* both eyes will use the same texture id */
	}

	ovrGLConfig cfg;
	memset(&cfg, 0, sizeof cfg);
	cfg.OGL.Header.API = ovrRenderAPI_OpenGL;
	cfg.OGL.Header.BackBufferSize.w = resolution.w;
	cfg.OGL.Header.BackBufferSize.h = resolution.h;
	cfg.OGL.Header.Multisample = 1;
	cfg.OGL.Window = GetActiveWindow();
	cfg.OGL.DC = wglGetCurrentDC();

	// Configure SDK rendering
	int distortionCaps;
	ovrEyeRenderDesc eyeRDesc[2];

	distortionCaps = ovrDistortionCap_TimeWarp | ovrDistortionCap_Overdrive;
	ovrHmd_ConfigureRendering(hmd, &cfg.Config, distortionCaps, hmd->DefaultEyeFov, eyeRDesc);

	if (hmd->HmdCaps & ovrHmdCap_ExtendDesktop){
		std::cout << "In Extended mode." << std::endl;
	}
	else{
		ovrHmd_AttachToWindow(hmd, cfg.OGL.Window, 0, 0);
	}

	// low persistent display and dynamic prediction for latency
	int hmdCaps = ovrHmdCap_LowPersistence | ovrHmdCap_DynamicPrediction;
	ovrHmd_SetEnabledCaps(hmd, hmdCaps);

	Camera cam;
	cam.startCapturing();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_NORMALIZE);
	glClearColor(0.1, 0.1, 0.1, 1);

	while (!glfwWindowShouldClose(window)){

		glfwPollEvents();

		ovrHmd_BeginFrame(hmd, 0);

		//Get eye poses, feeding in correct IPD offset
		ovrVector3f ViewOffset[2] = { eyeRDesc[0].HmdToEyeViewOffset, eyeRDesc[1].HmdToEyeViewOffset };
		ovrMatrix4f proj;
		ovrPosef pose[2];
		Mat frame;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// get a new frame if there is one
		if (cam.isNewFrame()){

			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			frame = cam.getImage(width, height);
			flip(frame, frame, 1);
			flip(frame, frame, 0);
			//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, frame.cols, frame.rows, GL_BGR, GL_UNSIGNED_BYTE, frame.data);
		}
		
		for (int i = 0; i < 2; i++){

			ovrEyeType eye = hmd->EyeRenderOrder[i];

			/* -- viewport transformation --
			* setup the viewport to draw in the left half of the framebuffer when we're
			* rendering the left eye's view (0, 0, width/2, height), and in the right half
			* of the framebuffer for the right eye's view (width/2, 0, width/2, height)
			*/
			glViewport(eye == ovrEye_Left ? 0 : width / 2, 0, width / 2, height);

			proj = ovrMatrix4f_Projection(hmd->DefaultEyeFov[eye], 0.5, 500.0, 1);
			glMatrixMode(GL_PROJECTION);
			glLoadTransposeMatrixf(proj.M[0]);

			/* -- view/camera transformation --
			* we need to construct a view matrix by combining all the information provided by the oculus
			* SDK, about the position and orientation of the user's head in the world.
			*/
			/* TODO: use ovrHmd_GetEyePoses out of the loop instead */
			pose[eye] = ovrHmd_GetHmdPosePerEye(hmd, eye);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			glTranslatef(eyeRDesc[eye].HmdToEyeViewOffset.x,
				eyeRDesc[eye].HmdToEyeViewOffset.y,
				eyeRDesc[eye].HmdToEyeViewOffset.z);
			/* retrieve the orientation quaternion and convert it to a rotation matrix */
			//quat_to_matrix(&pose[eye].Orientation.x, rot_mat);
			//glMultMatrixf(rot_mat);
			/* translate the view matrix with the positional tracking */
			glTranslatef(-pose[eye].Position.x, -pose[eye].Position.y, -pose[eye].Position.z);
			/* move the camera to the eye level of the user */
			glTranslatef(0, -ovrHmd_GetFloat(hmd, OVR_KEY_EYE_HEIGHT, 1.65), 0);
			if (i ==0)
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, frame.cols, frame.rows, GL_BGR, GL_UNSIGNED_BYTE, frame.data);
			else
				glTexSubImage2D(GL_TEXTURE_2D, 0, eyeRes[0].w, 0, frame.cols, frame.rows, GL_BGR, GL_UNSIGNED_BYTE, frame.data);
			//drawScene(texture);
		}

		//assert(glGetError() == GL_NO_ERROR);

		ovrHmd_EndFrame(hmd, pose, &ovrTex[0].Texture);

		//ourShader.Use();

		// Draw container
		//glBindVertexArray(VAO);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		//glBindVertexArray(0);

		/*// Render
		// Clear the colorbuffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		cap.read(frame);
		flip(frame, frame, 1);
		flip(frame, frame, 0);
		resize(frame, frame, Size(WIDTH, HEIGHT), 0, 0, 1);

		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, frame.cols, frame.rows, GL_BGR, GL_UNSIGNED_BYTE, frame.data);

		// Bind Texture
		glBindTexture(GL_TEXTURE_2D, texture);

		// Activate shader
		ourShader.Use();

		// Draw container
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glfwSwapBuffers(window);*/
	}

	// Properly de-allocate all resources once they've outlived their purpose
	//glDeleteVertexArrays(1, &VAO);
	//glDeleteBuffers(1, &VBO);
	//glDeleteBuffers(1, &EBO);

	cam.stopCapturing();
	glfwDestroyWindow(window);
	glfwTerminate();
	ovr_Shutdown();


	return 0;
}

bool initGLFW(void){

	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	return true;
}

void drawScene(GLuint& texture){
	float grey[] = { 0.8, 0.8, 0.8, 1 };
	float col[] = { 0, 0, 0, 1 };
	float lpos[][4] = {
		{ -8, 2, 10, 1 },
		{ 0, 15, 0, 1 }
	};
	float lcol[][4] = {
		{ 0.8, 0.8, 0.8, 1 },
		{ 0.4, 0.3, 0.3, 1 }
	};

	for (int i = 0; i<2; i++) {
		glLightfv(GL_LIGHT0 + i, GL_POSITION, lpos[i]);
		glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, lcol[i]);
	}

	glMatrixMode(GL_MODELVIEW);

	glPushMatrix();
	glTranslatef(0, 10, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, grey);
	glBindTexture(GL_TEXTURE_2D, texture);
	//glEnable(GL_TEXTURE_2D);
	//draw_box(30, 20, 30, -1.0);
	//glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	for (int i = 0; i<4; i++) {
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, grey);
		glPushMatrix();
		glTranslatef(i & 1 ? 5 : -5, 1, i & 2 ? -5 : 5);
		//draw_box(0.5, 2, 0.5, 1.0);
		glPopMatrix();

		col[0] = i & 1 ? 1.0 : 0.3;
		col[1] = i == 0 ? 1.0 : 0.3;
		col[2] = i & 2 ? 1.0 : 0.3;
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, col);

		glPushMatrix();
		if (i & 1) {
			glTranslatef(0, 0.25, i & 2 ? 2 : -2);
		}
		else {
			glTranslatef(i & 2 ? 2 : -2, 0.25, 0);
		}
		//draw_box(0.5, 0.5, 0.5, 1.0);
		glPopMatrix();
	}

	col[0] = 1;
	col[1] = 1;
	col[2] = 0.4;
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, col);
	//draw_box(0.05, 1.2, 6, 1.0);
	//draw_box(6, 1.2, 0.05, 1.0);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode){
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}