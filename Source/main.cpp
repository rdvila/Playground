#include "PCH.h"
#include "Application.h"
#include "Graphics/DX12/DX12Renderer.h"

template<typename R>
struct MyApplication final : public Playground::Application {
public:

	MyApplication(const char* name, Uint32 width, Uint32 height, Playground::DisplayMode mode)
		: Application(name, width, height, mode)
	{
		mRenderer = std::make_unique<R>();
	}

	~MyApplication() override
	{
	}

	void OnParseCommandLine([[maybe_unused]] HINSTANCE hInstance, [[maybe_unused]] LPSTR lpCmdLine, [[maybe_unused]] int nCmdShow)
	{
	}

	void Initilize() override
	{
		Application::Initilize();

		mRenderer->OnInitializeComponents(
			this->GetWindow()
		);
	}

	void Shutdown() override
	{
		mRenderer->OnDestroyComponents();

		Application::Shutdown();
	}

	void OnEvent([[maybe_unused]] const SDL_Event* event)
	{
	}

	void OnUpdate([[maybe_unused]] float deltaTime)
	{
	}

	void OnRender([[maybe_unused]] float deltaTime)
	{
		mRenderer->Render();
	}

private:
	std::unique_ptr<R> mRenderer;
};

template<typename T>
int RunApplication(HINSTANCE hInstance, LPSTR lpCmdLine, int nCmdShow, T* application)
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		return EXIT_FAILURE;
	}

	application->Initilize();

	application->OnParseCommandLine(hInstance, lpCmdLine, nCmdShow);

	bool keepRunning = true;
	while (keepRunning) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {

			if (event.type == SDL_QUIT) {
				keepRunning = false;
			}

			application->OnEvent(&event);
		}

		application->OnUpdate(0.0f);

		application->OnRender(0.0f);
	}

	application->Shutdown();

	SDL_Quit();
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, [[maybe_unused]] HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MyApplication<Playground::Graphics::DX12Renderer> app("My App", 1280, 720, Playground::DisplayMode::WINDOWED);
	return RunApplication(hInstance, lpCmdLine, nCmdShow, &app);
}
