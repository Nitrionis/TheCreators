#pragma once


class ImageLoader {
public:
	ImageLoader(const char* fileName = nullptr);

	~ImageLoader();

	void Reload(const char* fileName);

	void* pixels = nullptr;

	int
		width,
		height,
		channels,
		size;
};