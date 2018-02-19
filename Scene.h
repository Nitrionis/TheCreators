#pragma once
#include "SceneRenderer.h"

class Scene : public BaseCode
{
public:
    Scene();
    ~Scene();

private:
    SceneRenderer sceneRenderer;
};