// Steve Fulton

#ifndef ABSTRACTOVR_H
#define ABSTRACTOVR_H

// Oculus Rift
#include <Kernel\OVR_System.h>
#include <Extras\OVR_Math.h>
#include "AppInterface.h"

using namespace OVR;

class AbstractOVR : public AppInterface{

public:

	bool initialize(){
		bool success = true;

		if (!ovr_Initialize()){
			success = false;
		}

		return success;
	}

	void shutdown(){
		ovr_Shutdown();
	}

	virtual void setup() = 0;
};

#endif ABSTRACTOVR_H