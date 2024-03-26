#include "PCH.h"
#include "Application.h"

using namespace Playground;

Application::Application(const char* name, Uint32 width, Uint32 height, DisplayMode mode)
	: mName(name), mWidth(width), mHeight(height), mMode(mode)
{
}

Application::~Application()
{
}

void Application::Initilize()
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
}

void Application::Shutdown()
{
	SDL_DestroyWindow(mWindow);
}