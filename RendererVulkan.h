#pragma once

#include "PCH.h"

namespace Playground {
	class RendererVulkan final {

	public:
		RendererVulkan();

		void OnInitializeComponents(SDL_Window* window);
		void OnResize(Uint32 width, Uint32 height);
		void OnDestroyComponents();
		void Render();
	};
}