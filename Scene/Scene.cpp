#include "Scene.h"

Scene::Scene()
	/*: ChunksBuilder(GetComponent<ChunksRenderer>())
	, BlurBuilder(GetComponent<BlurRenderer>())
	, InterfaceBuilder(GetComponent<InterfaceRenderer>())*/
{
	/*ChunksBuilder::Initialize();
	BlurBuilder::Initialize();
	InterfaceBuilder::Initialize();*/
}

Scene::~Scene() {

}

template<typename T>
T& Scene::GetComponent() {
	return dynamic_cast<T&>(*this);
}
