// Steve Fulton

#ifndef OPENGLAPP_H
#define OPENGLAPP_H

#include <iostream>
#include "AbstractGL.h"

using namespace std;

class OGLApp : public AbstractGL{

private:
	GLuint width;
	GLuint height;
	
public:
	GLFWwindow* window;

	// project specific configurations
	 bool createContext(int width, int height, int xOffset, GLFWkeyfun key_callback){
		 window = glfwCreateWindow(width, height, "OpenCV - OpenGL - Oculus Rift", nullptr, nullptr);
		 if (window == nullptr){
			 cout << "Could not create window." << endl;

			 return false;
		 }
		 else{
			 glfwSetWindowPos(window, xOffset, 0);
			 glfwMakeContextCurrent(window);
			 glfwSetKeyCallback(window, key_callback);
			 glewExperimental = true; // Needed in core profile
			 glewInit();

			 this->height = height;
			 this->width = width;

			 return true;
		 }
	 }

	 // 2 seperate textures
	 void initTextures(GLuint& texture){
		 // left eye
		 glGenTextures(1, &texture);
		 glBindTexture(GL_TEXTURE_2D, texture);
		 glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
		 glBindTexture(GL_TEXTURE_2D, 0); // unbind texture
	 }

	 // 2 seperate textures
	 void initTextures(GLuint& left, GLuint& right){
		 // left eye
		 glGenTextures(1, &left);
		 glBindTexture(GL_TEXTURE_2D, left);
		 glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
		 glBindTexture(GL_TEXTURE_2D, 0); // unbind texture

		 // right eye
		 glGenTextures(1, &right);
		 glBindTexture(GL_TEXTURE_2D, right);
		 glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
		 glBindTexture(GL_TEXTURE_2D, 0); // unbind texture
	 }

	 int getWidth(){
		 return width;
	 }

	 int getHeight(){
		 return height;
	 }

	 void setWidth(int width){
		 this->width = width;
	 }

	 void setHeight(int height){
		 this->height = height;
	 }

};

#endif OPENGLAPP_H

