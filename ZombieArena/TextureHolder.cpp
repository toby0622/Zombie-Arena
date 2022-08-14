#include "TextureHolder.h"
#include <assert.h>

TextureHolder* TextureHolder::m_s_Instance = nullptr;

TextureHolder::TextureHolder() {
	assert(m_s_Instance == nullptr);
	m_s_Instance = this;
}

Texture& TextureHolder::GetTexture(string const& filename) {
	// get a reference to "m_Textures" using "m_s_Instance"
	// auto is equivalent of "map<string, Texture>"
	auto& m = m_s_Instance -> m_Textures;

	// create an iterator to hold a key-value pair and search for the required pair using the filename
	// auto is equivalent of "map<string, Texture>::iterator"
	auto keyValuePair = m.find(filename);

	// verify the pair match
	if (keyValuePair != m.end()) {
		// match, return the texture, the second part of the key-value pair
		return keyValuePair->second;
	} else {
		// file name not found, create a new key value pair using the filename
		auto& texture = m[filename];

		// load the texture from file
		texture.loadFromFile(filename);

		// return the required texture
		return texture;
	}
}