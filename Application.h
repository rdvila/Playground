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

	template<RendererType renderType>
	class Application {
	public:

		using Renderer = typename std::conditional<renderType == RendererType::D3D12, RendererD3D12, RendererVulkan>::type;

		Application(const char* name, Uint32 width, Uint32 height, DisplayMode mode)
			: mName(name), mWidth(width), mHeight(height), mMode(mode)
		{
		}

		virtual ~Application()
		{
		}

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
		void RunLoop()
		{
			while (true) {
				SDL_Event event;
				while (SDL_PollEvent(&event)) {

					if (event.type == SDL_QUIT) {
						return;
					}

					OnEvent(&event);
				}

				mRenderer.Render();

			}
		}

		// Application startup function
		static int RunApplication(HINSTANCE hInstance, LPSTR lpCmdLine, int nCmdShow, Application* application)
		{
			if (SDL_Init(SDL_INIT_VIDEO) != 0)
			{
				return EXIT_FAILURE;
			}

			application->BootstrapComponents();

			application->OnInitilize();

			application->OnParseCommandLine(hInstance, lpCmdLine, nCmdShow);

			application->RunLoop();

			application->OnShutdown();

			application->DestroyComponents();

			SDL_Quit();
			return 0;
		}

	protected:
		void BootstrapComponents()
		{
			Uint32 flags = 0;

			switch (mMode)
			{
			case Playground::DisplayMode::WINDOWED:
				flags |= (SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
				break;
			case Playground::DisplayMode::WINDOWED_FULLSCREEN:
				flags |= (SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP);
				break;
			case Playground::DisplayMode::FULLSCREEN:
				flags |= (SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN);
				break;
			default:
				break;
			}

			mWindow = SDL_CreateWindow(
				mName,
				SDL_WINDOWPOS_CENTERED,
				SDL_WINDOWPOS_CENTERED,
				mWidth,
				mHeight,
				flags
			);

			mRenderer.OnInitializeComponents(mWindow);
		}

		void DestroyComponents()
		{
			mRenderer.OnDestroyComponents();
		}

	private:
		const char*   mName;
		Uint32        mWidth;
		Uint32        mHeight;
		DisplayMode   mMode;
		SDL_Window*   mWindow{nullptr};
		Renderer      mRenderer;
	};
}