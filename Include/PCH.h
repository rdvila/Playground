#pragma once

// C++
#include <locale>
#include <codecvt>
#include <string>
#include <filesystem>
#include <vector>
#include <stdexcept>
#include <variant>

// SDL
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

//DirectX 12
#include <directx/d3d12.h>
#include <directx/d3dx12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXColors.h>

//WIL
#include <wil/com.h>
#include <wil/result.h>

// assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>