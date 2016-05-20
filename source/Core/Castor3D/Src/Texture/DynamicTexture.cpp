#include "DynamicTexture.hpp"

#include "Sampler.hpp"

#include "Render/RenderSystem.hpp"

#include <Image.hpp>
#include <PixelBufferBase.hpp>

using namespace Castor;

namespace Castor3D
{
	DynamicTexture::DynamicTexture( RenderSystem & p_renderSystem, eTEXTURE_TYPE p_type, uint8_t p_cpuAccess, uint8_t p_gpuAccess )
		: TextureLayout{ p_renderSystem, p_type, p_cpuAccess, p_gpuAccess }
		, m_samplesCount{ 0 }
	{
	}

	DynamicTexture::~DynamicTexture()
	{
	}
}
