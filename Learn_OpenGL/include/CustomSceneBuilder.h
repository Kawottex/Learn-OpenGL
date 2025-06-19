#pragma once

#include <ICustomScene.h>
#include <LightScene.h>
#include <ModelScene.h>
#include <StencilScene.h>
#include <memory>

enum CustomSceneType
{
	NONE,
	LIGHT_SCENE,
	MODEL_SCENE,
	STENCIL_SCENE
};

class CustomSceneBuilder
{
public:
	static std::shared_ptr<ICustomScene> BuildCustomScene(CustomSceneType sceneType)
	{
		switch (sceneType)
		{
			case CustomSceneType::LIGHT_SCENE:
				return std::shared_ptr<LightScene>(new LightScene);
			case CustomSceneType::MODEL_SCENE:
				return std::shared_ptr<ModelScene>(new ModelScene);
			case CustomSceneType::STENCIL_SCENE:
				return std::shared_ptr<StencilScene>(new StencilScene);
			default:
				return std::shared_ptr<ICustomScene>();
		}
	}
};