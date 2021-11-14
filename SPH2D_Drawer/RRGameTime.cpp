#include "RRGameTime.h"
#include <SDL.h>

RRGameTime& RRGameTime::Instance() {
	static RRGameTime instance;
	return instance;
}


void RRGameTime::Update() {
	previousFrameTicks = currentFrameTicks;
	currentFrameTicks = SDL_GetTicks();
	passedTime = currentFrameTicks - previousFrameTicks; 
}

unsigned int RRGameTime::GetPassedTime() const {
	return passedTime;
}

unsigned int RRGameTime::GetTotalTime() const {
	return currentFrameTicks;
}