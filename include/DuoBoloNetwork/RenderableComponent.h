#pragma once

#include <raylib.h>
#include <string>

struct RenderableComponent
{
	std::string model = "cube";
	Color tint = WHITE;
};
