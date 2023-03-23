#pragma once
#include "Mouse.h"
#include "RRKeyboardState.h"

class RRController {
public:
	static RRController& Instance();

	const Mouse& GetMouseState() const {
		return mouseState;
	}
	const RRKeyboardState& GetKeyState() const {
		return keyState;
	}

	bool HasQuitEvent() const {
		return hasQuitEvent;
	}

	void Update();
private:
	bool hasQuitEvent{ false };
	Mouse mouseState; 
	RRKeyboardState keyState;

	RRController() = default;
	RRController(const RRController&) = delete;
	void operator=(const RRController&) = delete;
};