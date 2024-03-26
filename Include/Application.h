#pragma once

#include "PCH.h"

namespace Playground {

	enum class DisplayMode {
		WINDOWED,
		WINDOWED_FULLSCREEN,
		FULLSCREEN
	};

	class Application {
	public:

		Application(const char* name, Uint32 width, Uint32 height, DisplayMode mode);

		virtual ~Application();

		// Deleted methods
		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;
		Application& operator=(const Application&&) = delete;

		// Application interface
		virtual void Initilize();
		virtual void Shutdown();

		//Getters
		SDL_Window* GetWindow() { return mWindow; }
	private:
		const char*   mName;
		Uint32        mWidth;
		Uint32        mHeight;
		DisplayMode   mMode;
		SDL_Window*   mWindow{nullptr};
	};
}