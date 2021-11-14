#pragma once
#include <unordered_map>
#include <string>
#include "RRTexture.h"
#include "TextureType.h"
 
/*
����� ��� ��������, ��������� � �������� �������
*/
class RRTextureManager
{
public:
	static RRTextureManager& Instance() {
		static RRTextureManager instance;
		return instance;
	}
	// ������� ��� ������ ������
	void Initialize();

	// ������� ��� ���������� ������
	void Shutdown();

	// �������� �������� �� ����� 
	const RRTexture& GetTexture(TextureType type) const;
private:
	RRTextureManager() {}
	RRTextureManager(const RRTextureManager&) = delete;
	RRTextureManager operator=(const RRTextureManager&) = delete;
	  
	std::unordered_map<TextureType, RRTexture> textures;
};

