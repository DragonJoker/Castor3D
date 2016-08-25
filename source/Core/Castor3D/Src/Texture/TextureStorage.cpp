#include "TextureStorage.hpp"

#include "Engine.hpp"

#include "Render/RenderSystem.hpp"

using namespace Castor;

namespace Castor3D
{
	TextureStorage::TextureStorage( TextureStorageType p_type, TextureImage & p_image, AccessType p_cpuAccess, AccessType p_gpuAccess )
		: OwnedBy< TextureImage >{ p_image }
		, m_cpuAccess{ p_cpuAccess }
		, m_gpuAccess{ p_gpuAccess }
		, m_type{ p_type }
	{
	}

	TextureStorage::~TextureStorage()
	{
	}
}
