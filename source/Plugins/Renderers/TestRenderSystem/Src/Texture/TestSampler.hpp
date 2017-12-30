/* See LICENSE file in root folder */
#ifndef ___TRS_SAMPLER_H___
#define ___TRS_SAMPLER_H___

#include "TestRenderSystemPrerequisites.hpp"

#include <Texture/Sampler.hpp>

namespace TestRender
{
	class TestSampler
		: public castor3d::Sampler
	{
	public:
		TestSampler( TestRenderSystem * p_renderSystem, castor::String const & p_name );
		virtual ~TestSampler();

		virtual bool initialise();
		virtual void cleanup();
		virtual void bind( uint32_t p_index )const;
		virtual void unbind( uint32_t p_index )const;
	};
}

#endif
