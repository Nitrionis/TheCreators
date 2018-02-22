#include "ImageLoader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <stdexcept>

ImageLoader::ImageLoader(const char* fileName) {
	Reload(fileName);
}

ImageLoader::~ImageLoader() {
	if (pixels != nullptr) {
		stbi_image_free(reinterpret_cast<stbi_uc*>(pixels));
	}
}

void ImageLoader::Reload(const char *fileName) {
	if (pixels != nullptr) {
		stbi_image_free(reinterpret_cast<stbi_uc*>(pixels));
	}
	if (fileName != nullptr) {
		pixels = stbi_load(fileName, &width, &height, &channels, STBI_rgb_alpha);
		size = width * height * 4;

		if (!pixels) {
			throw std::runtime_error("Failed to load texture textureImage!");
		}
	}
}
