#pragma once

#include "PCH.h"

namespace Playground::Graphics {

	class MeshLoader {

	public:
		MeshLoader(const std::filesystem::path& path);
		void Initialize();
		virtual void Load() = 0;
		virtual void Unload() = 0;
		void Destroy();


	private:
		std::filesystem::path mPath;
		uint32_t mVextexCount;
		uint32_t mIndexCount;
		std::vector<float> mVertices;
		std::vector<uint32_t> mIndices;
	};
}