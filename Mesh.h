#pragma once

#include "PCH.h"

namespace Playground::Graphics {

	class Mesh final {

	public:
		Mesh(const std::filesystem::path& path);
		void Initialize();
		void Load();
		void Unload();
		void Destroy();


	private:
		std::filesystem::path mPath;
		uint32_t mVextexCount;
		uint32_t mIndexCount;
		std::vector<float> mVertices;
		std::vector<uint32_t> mIndices;
	};
}