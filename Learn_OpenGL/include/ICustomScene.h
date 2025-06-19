#pragma once

#include <Camera.h>

class ICustomScene
{
public:
	virtual void Setup() = 0;
	virtual void Draw(const Camera& camera) = 0;
};