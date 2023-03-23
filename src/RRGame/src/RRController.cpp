#include "RRController.h"
#include <SDL.h>

void RRController::Update() {
	// ��������� ���������� ��������� ����
	mouseState.SavePrevious();

	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_MOUSEBUTTONDOWN: { // ������� ������� ������ �� �����
					if (event.button.button == SDL_BUTTON_LEFT) // ���
						mouseState.LeftButton = Mouse::State::Pressed;
					if (event.button.button == SDL_BUTTON_RIGHT) // ���
						mouseState.RightButton = Mouse::State::Pressed;
					break;
			}
			case SDL_MOUSEBUTTONUP: { // ������� ������� ������ �� �����
					if (event.button.button == SDL_BUTTON_LEFT)
						mouseState.LeftButton = Mouse::State::Released;
					if (event.button.button == SDL_BUTTON_RIGHT)
						mouseState.RightButton = Mouse::State::Released;
					break;
			}
			case SDL_MOUSEMOTION: { // ������� �������� ����
				mouseState.Position = Vector2(event.motion.x, event.motion.y);
				break;
			}
			case SDL_WINDOWEVENT:
				if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
					/// resized
					event.window.data1; // x
					event.window.data2; // y
				}
				break;
			case SDL_QUIT:
				hasQuitEvent = true;
				break;
			default:
				break;
		}
	}


	// �������� ������ ����� � ����������
	keyState.Update((unsigned char*)SDL_GetKeyboardState(NULL));
}

RRController& RRController::Instance() {
	static RRController instance;
	return instance;
}

