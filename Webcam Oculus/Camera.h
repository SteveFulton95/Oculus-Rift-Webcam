// Steve Fulton
#pragma once
#ifndef CAMERA_H
#define CAMERA_H


// OpenCV
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\opencv.hpp>

#include <iostream>

using namespace cv;

class Camera{

private:

	VideoCapture capture;	// how we capture an image
	Mat frame;				// the frame to be captured
	bool isCapturing;		// whether or not the webcam is open

public:

	Camera(){
		isCapturing = false;
	}

	// specify the device number.
	// If using 2 cameras make the device number for the first camera 0
	// and the second camera 1.
	bool startCapturing(int device){
		capture.open(device);
		capture.set(CV_CAP_PROP_FRAME_WIDTH, 1024);
		capture.set(CV_CAP_PROP_FRAME_HEIGHT, 720);

		if (!capture.isOpened())
			std::cerr << "Camera did not open" << std::endl;

		if (!capture.read(frame)){
			std::cerr << "Camera could not read a new frame." << std::endl;
			//return false;
		}

		isCapturing = true;

		return true;
	}

	// shut down the camera
	void stopCapturing(){
		isCapturing = false;
		if (capture.isOpened()){
			capture.release();
		}
	}

	// width and height for resizing
	Mat getImage(int width, int height){
		// read a new frame
		capture.read(frame);

		// resize the frame based off of the width and height passed in
		resize(frame, frame, cv::Size(width, height), 0, 0, 1);

		return frame;
	}
};

#endif	// CAMERA_H