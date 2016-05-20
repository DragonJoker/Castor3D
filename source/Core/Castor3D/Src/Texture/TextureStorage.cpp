#include "TextureStorage.hpp"

#include "Engine.hpp"

#include "Render/RenderSystem.hpp"

using namespace Castor;

namespace Castor3D
{
	TextureStorage::TextureStorage( eTEXTURE_TYPE p_type, TextureImage & p_image, uint8_t p_cpuAccess, uint8_t p_gpuAccess )
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
