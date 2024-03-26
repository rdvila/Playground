#include "PCH.h"
#include "Graphics/DX12/DX12Renderer.h"

enum class DisplayMode
{
	WINDOWED,
	WINDOWED_FULLSCREEN,
	FULLSCREEN
};

struct Window final
{
public:

	Window(const char* name, Uint32 width, Uint32 height, DisplayMode mode)
		: mName(name), mWidth(width), mHeight(height), mMode(mode)
	{

	}

	~Window()
	{
		SDL_DestroyWindow(mWindow);
	}

	// Application interface
	void Create() {
		Uint32 flags = 0;

		switch (mMode)
		{
		case DisplayMode::WINDOWED:
			flags |= (SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
			break;
		case DisplayMode::WINDOWED_FULLSCREEN:
			flags |= (SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP);
			break;
		case DisplayMode::FULLSCREEN:
			flags |= (SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN);
			break;
		default:
			break;
		}

		if (mWindow)
		{
			SDL_DestroyWindow(mWindow);
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

	//Getters
	[[nodiscard]] SDL_Window* GetWindow() { return mWindow; }

private:
	const char*   mName;
	Uint32        mWidth;
	Uint32        mHeight;
	DisplayMode   mMode;
	SDL_Window*   mWindow{ nullptr };
};

template<typename R>
struct MyApplication final {
public:

	MyApplication(const char* name, Uint32 width, Uint32 height, DisplayMode mode)
	{
		mWindow = std::make_unique<Window>(name, width, height, mode);
		mRenderer = std::make_unique<R>();
	}

	~MyApplication()
	{
	}

	void OnParseCommandLine([[maybe_unused]] HINSTANCE hInstance, [[maybe_unused]] LPSTR lpCmdLine, [[maybe_unused]] int nCmdShow)
	{
	}

	void Initilize()
	{
		mWindow->Create();
		mRenderer->OnInitializeComponents(
			mWindow->GetWindow()
		);
	}

	void Shutdown()
	{
		mRenderer->OnDestroyComponents();
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
	std::unique_ptr<Window> mWindow;
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
	MyApplication<Playground::Graphics::DX12Renderer> app("My App", 1280, 720, DisplayMode::WINDOWED);
	return RunApplication(hInstance, lpCmdLine, nCmdShow, &app);
}
