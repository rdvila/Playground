#pragma once

#include "PCH.h"
#include "RendererD3D12.h"
#include "RendererVulkan.h"

namespace Playground {

	enum class DisplayMode {
		WINDOWED,
		WINDOWED_FULLSCREEN,
		FULLSCREEN
	};

	enum class RendererType {
		D3D12,
		VULKAN
	};

	class Application {
	public:

		Application(const char* name, Uint32 width, Uint32 height, DisplayMode mode, RendererType type);

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
		DisplayMode   mMode;
		SDL_Window*   mWindow{nullptr};
		RendererType  mType;
		std::variant<RendererD3D12, RendererVulkan> mRenderer;
	};
}