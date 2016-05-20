#include "StaticTexture.hpp"

#include "Sampler.hpp"

#include "Render/RenderSystem.hpp"

#include <Logger.hpp>
#include <Image.hpp>

using namespace Castor;

namespace Castor3D
{
	StaticTexture::StaticTexture( RenderSystem & p_renderSystem, eTEXTURE_TYPE p_type, uint8_t p_cpuAccess, uint8_t p_gpuAccess )
		: TextureLayout{ p_renderSystem, p_type, p_cpuAccess, p_gpuAccess }
	{
	}

	StaticTexture::~StaticTexture()
	{
	}
}
