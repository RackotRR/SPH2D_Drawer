#include "RRTextureManager.h"
#include "RRGameIO.h"
#include <assert.h>
#include <vector>

void RRTextureManager::Initialize() {
	auto& gameIO{ RRGameIO::Instance() };

	// ����������� ������ ��� �������� 
	textures.reserve(static_cast<size_t>(TextureType::NumberOfTextures));

	// �������� ���������������� ���-�� ��� 
	//textures.emplace(TextureType::forest, gameIO.LoadTextureFromFile("Textures\\Forest.png"));
	 
} 

void RRTextureManager::Shutdown() {
	for (auto& iter : textures) {
		iter.second.Destroy();
	}
}

const RRTexture& RRTextureManager::GetTexture(TextureType type) const { 
	assert(type != TextureType::None);

	return textures.at(type);
}