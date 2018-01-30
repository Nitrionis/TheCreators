#pragma once

//#include <functional>

namespace vk
{
	class Device;
	template <typename T>
	class UniqueHandle
	{
	public:
		UniqueHandle() : UniqueHandle([](T, VkAllocationCallbacks*) {}) {}

		UniqueHandle(std::function<void(T, VkAllocationCallbacks*)> deletef) {
			this->deleter = [=](T obj) { deletef(obj, nullptr); };
		}

		UniqueHandle(VkInstance *instance, std::function<void(VkInstance, T, VkAllocationCallbacks*)> deletef) {
			this->deleter = [instance, deletef](T obj) { deletef(*instance, obj, nullptr); };
		}

		UniqueHandle(VkDevice *device, std::function<void(VkDevice, T, VkAllocationCallbacks*)> deletef) {
			this->deleter = [device, deletef](T obj) { deletef(*device, obj, nullptr); };
		}

		~UniqueHandle() {
			Cleanup();
		}

		const T* operator &() const {
			return &object;
		}

		T* replace() {
			Cleanup();
			return &object;
		}

		operator T() const {
			return object;
		}

		void operator = (T rhs) {
			Cleanup();
			object = rhs;
		}

		template<typename V>
		bool operator == (V rhs) {
			return object == T(rhs);
		}

	private:
		T object{ VK_NULL_HANDLE };
		std::function<void(T)> deleter;

		void Cleanup() {
			if (object != VK_NULL_HANDLE) {
				deleter(object);
			}
			object = VK_NULL_HANDLE;
		}
	};

	using CommandQueue = VkQueue;
	using CommandBuffer = VkCommandBuffer;
	using PhysicalDevice = VkPhysicalDevice;

	using UniqueCommandPool = vk::UniqueHandle<VkCommandPool>;
	using UniqueFramebuffer = vk::UniqueHandle<VkFramebuffer>;
	using UniqueCommandBuffer = vk::UniqueHandle<VkCommandBuffer>;

}