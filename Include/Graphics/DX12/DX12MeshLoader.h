#include "PCH.h"
#include "Graphics/Mesh.h"

namespace Playground::Graphics {

	class DX12MeshLoader final {

	public:
		void Load(const Mesh &mesh);
		void Unload();
	};
}