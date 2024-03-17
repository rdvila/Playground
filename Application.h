#pragma once

#include "PCH.h"
#include "Renderer.h"

namespace Playground {

	enum class DisplayMode {
		WINDOWED,
		WINDOWED_FULLSCREEN,
		FULLSCREEN
	};

	class Application {
	public:
		Application(const char* name, Uint32 width, Uint32 height, DisplayMode type);
		virtual ~Application();

		// Deleted methods
		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;
		Application& operator=(const Application&&) = delete;

		// Application interface
		virtual void OnParseCommandLine(HINSTANCE hInstance, LPSTR lpCmdLine, int nCmdShow) = 0;
		virtual void OnInitilize() = 0;
		virtual void OnShutdown() = 0;
		virtual void OnEvent(const SDL_Event* event) = 0;
		virtual void OnUpdate(float deltaTime) = 0;
		virtual void OnRender(float deltaTime) = 0;

		// Application loop
		void RunLoop();

		// Application startup function
		static int RunApplication(HINSTANCE hInstance, LPSTR lpCmdLine, int nCmdShow, Application* application);

	protected:
		void BootstrapComponents();
		void DestroyComponents();

	private:
		const char*   mName;
		Uint32        mWidth;
		Uint32        mHeight;
		DisplayMode   mType;
		SDL_Window*   mWindow{nullptr};
		Renderer      mRenderer;
	};
}