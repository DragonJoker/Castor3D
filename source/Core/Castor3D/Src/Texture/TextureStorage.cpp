#include "TextureStorage.hpp"

#include "Engine.hpp"

#include "Render/RenderSystem.hpp"

using namespace Castor;

namespace Castor3D
{
	TextureStorage::TextureStorage(
		TextureStorageType p_type,
		TextureLayout & p_layout,
		AccessTypes const & p_cpuAccess,
		AccessTypes const & p_gpuAccess )
		: OwnedBy< TextureLayout >{ p_layout }
		, m_cpuAccess{ p_cpuAccess }
		, m_gpuAccess{ p_gpuAccess }
		, m_type{ p_type }
	{
	}

	TextureStorage::~TextureStorage()
	{
	}
}
