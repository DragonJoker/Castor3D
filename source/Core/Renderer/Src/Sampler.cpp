/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "Sampler.hpp"

namespace renderer
{
	Sampler::Sampler( Device const & device
		, WrapMode wrapS
		, WrapMode wrapT
		, WrapMode wrapR
		, Filter minFilter
		, Filter magFilter
		, MipmapMode mipFilter
		, float minLod
		, float maxLod
		, float lodBias
		, BorderColour borderColour
		, float maxAnisotropy
		, CompareOp compareOp )
		: m_wrapS{ wrapS }
		, m_wrapT{ wrapT }
		, m_wrapR{ wrapR }
		, m_minFilter{ minFilter }
		, m_magFilter{ magFilter }
		, m_mipFilter{ mipFilter }
		, m_minLod{ minLod }
		, m_maxLod{ maxLod }
		, m_lodBias{ lodBias }
		, m_borderColour{ borderColour }
		, m_maxAnisotropy{ maxAnisotropy }
		, m_compareOp{ compareOp }
	{
	}
}
