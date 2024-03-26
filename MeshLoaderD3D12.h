#include "PCH.h"
#include "MeshLoader.h"

namespace Playground::Graphics {

	class MeshLoaderD3D12 final : public MeshLoader {

	public:
		MeshLoaderD3D12(const std::filesystem::path& path);
		void Load() override;
		void Unload() override;
	};
}