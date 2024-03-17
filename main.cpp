#include "PCH.h"
#include "Application.h"

using namespace Playground;

struct MyApplication final : public Application {
public:

	MyApplication(const char* name, Uint32 width, Uint32 height, DisplayMode type)
		: Application(name, width, height, type)
	{
	}

	~MyApplication() override
	{
	}

	void OnParseCommandLine([[maybe_unused]] HINSTANCE hInstance, [[maybe_unused]] LPSTR lpCmdLine, [[maybe_unused]] int nCmdShow) override
	{
	}

	void OnInitilize() override
	{
	}

	void OnShutdown() override
	{
	}

	void OnEvent([[maybe_unused]] const SDL_Event* event) override
	{
	}

	void OnUpdate([[maybe_unused]] float deltaTime) override
	{
	}

	void OnRender([[maybe_unused]] float deltaTime) override
	{
	}
};

int WINAPI WinMain(HINSTANCE hInstance, [[maybe_unused]] HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MyApplication app("My App", 1280, 720, DisplayMode::WINDOWED);
	return Application::RunApplication(hInstance, lpCmdLine, nCmdShow, &app);
}