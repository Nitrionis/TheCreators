#pragma once

#include "Vulkan/Buffer/Buffer.h"
#include <memory>
#include "Vulkan/Other/VectorTypes.h"

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
}