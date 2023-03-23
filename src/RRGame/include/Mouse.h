#pragma once 
#include "Vector2.h"
 
class Mouse { 
public:
	enum class Button {
		left,
		middle,
		right
	};

	enum class State {
		Pressed,
		Released
	};

	// вызвать перед изменением состояния LeftButton и RightButton
	void SavePrevious() {
		PrevLeftButton = LeftButton;
		PrevRightButton = RightButton;
		PrevPosition = Position;
	}

	bool Click(Button button) const {
		if (button == Button::left) {
			return LeftButton == State::Pressed && PrevLeftButton == State::Released;
		}
		else if (button == Button::right) {
			return RightButton == State::Pressed && PrevRightButton == State::Released;
		}
		return false;
	}

	Vector2 Position{};
	Vector2 PrevPosition{};
	State LeftButton{ State::Released };
	State RightButton{ State::Released };
	State PrevLeftButton{ State::Released };
	State PrevRightButton{ State::Released };
};

