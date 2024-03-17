#include "PCH.h"
#include "Application.h"

using namespace Playground;

void Playground::Application::RunLoop()
{
	while (true) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {

			if (event.type == SDL_QUIT) {
				return;
			}

			OnEvent(&event);
		}
	}
}

int Application::RunApplication(HINSTANCE hInstance, LPSTR lpCmdLine, int nCmdShow, Application* application)
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

void Application::BootstrapComponents()
{
	Uint32 flags = 0;

	switch (mType)
	{
	case Playground::DisplayMode::WINDOWED:
		flags |= (SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE) ;
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
}

void Application::DestroyComponents()
{
}

Application::Application(const char* name, Uint32 width, Uint32 height, DisplayMode type)
	: mName(name), mWidth(width), mHeight(height), mType(type)
{
}

Application::~Application()
{
}
