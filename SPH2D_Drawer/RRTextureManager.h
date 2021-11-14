#pragma once
#include <unordered_map>
#include <string>
#include "RRTexture.h"
#include "TextureType.h"
 
/*
класс для загрузки, получения и выгрузки текстур
*/
class RRTextureManager
{
public:
	static RRTextureManager& Instance() {
		static RRTextureManager instance;
		return instance;
	}
	// вызвать для начала работы
	void Initialize();

	// вызвать для завершения работы
	void Shutdown();

	// получить текстуру по ключу 
	const RRTexture& GetTexture(TextureType type) const;
private:
	RRTextureManager() {}
	RRTextureManager(const RRTextureManager&) = delete;
	RRTextureManager operator=(const RRTextureManager&) = delete;
	  
	std::unordered_map<TextureType, RRTexture> textures;
};

