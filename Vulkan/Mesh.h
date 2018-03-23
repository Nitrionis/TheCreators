#pragma once

#include "Buffer.h"
#include <memory>
#include "VectorTypes.h"

namespace vk
{
	template <typename T>
	class shared_array : public std::shared_ptr<T> {
	public:
		shared_array(T* ptr) : std::shared_ptr<T>::shared_ptr(ptr, std::default_delete<T[]>()) {}

		T& operator [] (uint32_t index) {
			return std::shared_ptr<T>::get()[index];
		}
	};

	/*class Mesh {
	private:
		struct {
			vk::Buffer indices;
			vk::Buffer vertices;
			vk::Buffer normals;
			vk::Buffer uv[3];
		}buffer;

		vk::shared_array<uint8_t> vertices = nullptr;
		vk::shared_array<uint8_t> indices = nullptr;
		vk::shared_array<uint8_t> normals = nullptr;
		vk::shared_array<uint8_t> uv[3];
	public:
		Mesh();
		~Mesh();

		template <typename T>
		void SetVertices(vk::shared_array<T> arr) {
			vertices = *reinterpret_cast<vk::shared_array<uint8_t>*>(&arr);
		}
		template <typename T>
		void SetIndices(vk::shared_array<T> arr) {
			indices = *reinterpret_cast<vk::shared_array<uint8_t>*>(&arr);
		}
		template <typename T>
		void SetNormals(vk::shared_array<T> arr) {
			normals = *reinterpret_cast<vk::shared_array<uint8_t>*>(&arr);
		}
		template <typename T>
		void SetUV(vk::shared_array<T> arr, uint8_t channelIndex) {
			uv[channelIndex] = *reinterpret_cast<vk::shared_array<uint8_t>*>(&arr);
		}
	};*/
}