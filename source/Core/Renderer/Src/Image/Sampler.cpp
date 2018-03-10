/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#include "Image/Sampler.hpp"

namespace renderer
{
	Sampler::Sampler( Device const & device
		, SamplerCreateInfo const & createInfo )
		: m_info{ createInfo }
	{
	}
}
