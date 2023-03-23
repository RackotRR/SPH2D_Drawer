#include "RRController.h"
#include <SDL.h>

void RRController::Update() {
	// сохраняем предыдущее состояние мыши
	mouseState.SavePrevious();

	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_MOUSEBUTTONDOWN: { // событие нажатия кнопки на мышке
					if (event.button.button == SDL_BUTTON_LEFT) // ЛКМ
						mouseState.LeftButton = Mouse::State::Pressed;
					if (event.button.button == SDL_BUTTON_RIGHT) // ПКМ
						mouseState.RightButton = Mouse::State::Pressed;
					break;
			}
			case SDL_MOUSEBUTTONUP: { // событие отжатия кнопки на мышке
					if (event.button.button == SDL_BUTTON_LEFT)
						mouseState.LeftButton = Mouse::State::Released;
					if (event.button.button == SDL_BUTTON_RIGHT)
						mouseState.RightButton = Mouse::State::Released;
					break;
			}
			case SDL_MOUSEMOTION: { // событие движение мыши
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


	// получаем данные ввода с клавиатуры
	keyState.Update((unsigned char*)SDL_GetKeyboardState(NULL));
}

RRController& RRController::Instance() {
	static RRController instance;
	return instance;
}

