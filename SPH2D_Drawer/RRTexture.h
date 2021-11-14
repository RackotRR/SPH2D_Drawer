#pragma once
#include <SDL.h>
#include <assert.h>
#include "Vector2.h" 
#include "Rectangle.h"
/*
���������� ��������
*/
class RRTexture
{
public:
	RRTexture() : 
		RRTexture{ nullptr } {}
	RRTexture(SDL_Texture* texture) : 
		texture{ texture } {
		// ���������� �������� � � ���������� 
		SDL_QueryTexture(texture, nullptr, nullptr, &size.X, &size.Y);
	}

	// ������������ ������ ��������
	void Destroy() {
		assert(texture != nullptr);

		SDL_DestroyTexture(texture);
		texture = nullptr;
	} 

	// ������ �������� X - Width, Y - Height
	Vector2 GetSize() const { 
		assert(size != Vector2::WrongVector);

		return size;
	}
	 
	// �������� ������ �������� ��� ���������
	SDL_Texture* Get() const { 
		assert(texture != nullptr);

		return texture; 
	};
private:
	Vector2 size{ Vector2::WrongVector };
	SDL_Texture* texture;
};

