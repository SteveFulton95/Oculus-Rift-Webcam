// Steve Fulton

#ifndef APPINTERFACE_H
#define APPINTERFACE_H

class AppInterface{

public:
	virtual bool initialize() = 0;
	virtual void shutdown() = 0;
};

#endif APPINTERFACE_H