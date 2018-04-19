#pragma once

#include "Scene/Character/Camera/Camera.h"

class Character : private Camera {
private:
	Character();

public:
	~Character();

	Character(Camera const&) = delete;
	Character& operator= (Character const&) = delete;

	static Character& Instance() {
		static Character s;
		return s;
	}

	template <typename T>
	T& GetComponent() {
		return dynamic_cast<T&>(*this);
	}
};


