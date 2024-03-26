#pragma once

#include "PCH.h"

namespace Playground {
	class Renderer {

	public:
		Renderer() = default;
		virtual ~Renderer() = default;

		virtual void OnInitializeComponents(SDL_Window* window) = 0;
		virtual void OnResize(Uint32 width, Uint32 height) = 0;
		virtual void OnDestroyComponents() = 0;
		virtual void Render() = 0;
	};
}