#pragma once

#include <ICustomScene.h>
#include <LightScene.h>
#include <ModelScene.h>
#include <StencilScene.h>
#include <BlendingScene.h>
#include <FramebufferScene.h>
#include <MirrorFramebufferScene.h>
#include <CubemapScene.h>
#include <TestScene.h>
#include <memory>

enum CustomSceneType
{
	NONE,
	LIGHT_SCENE,
	MODEL_SCENE,
	STENCIL_SCENE,
	BLENDING_SCENE,
	FRAMEBUFFER_SCENE,
	MIRRORFRAMEBUFFER_SCENE,
	CUBEMAP_SCENE,
	TEST_SCENE
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
			case CustomSceneType::BLENDING_SCENE:
				return std::shared_ptr<BlendingScene>(new BlendingScene);
			case CustomSceneType::FRAMEBUFFER_SCENE:
				return std::shared_ptr<FramebufferScene>(new FramebufferScene);
			case CustomSceneType::MIRRORFRAMEBUFFER_SCENE:
				return std::shared_ptr<MirrorFramebufferScene>(new MirrorFramebufferScene);
			case CustomSceneType::CUBEMAP_SCENE:
				return std::shared_ptr<CubemapScene>(new CubemapScene);
			case CustomSceneType::TEST_SCENE:
				return std::shared_ptr<TestScene>(new TestScene);
			default:
				return std::shared_ptr<ICustomScene>();
		}
	}
};