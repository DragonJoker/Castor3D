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
		, MipmapMode mipFilter )
		: m_wrapS{ wrapS }
		, m_wrapT{ wrapT }
		, m_wrapR{ wrapR }
		, m_minFilter{ minFilter }
		, m_magFilter{ magFilter }
		, m_mipFilter{ mipFilter }
	{
	}
}
