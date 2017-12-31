#include "Texture/TestSampler.hpp"

#include "Render/TestRenderSystem.hpp"

using namespace castor3d;
using namespace castor;

namespace TestRender
{
	TestSampler::TestSampler( TestRenderSystem * p_renderSystem, castor::String const & p_name )
		: Sampler( *p_renderSystem->getEngine(), p_name )
	{
	}

	TestSampler::~TestSampler()
	{
	}

	bool TestSampler::initialise()
	{
		return true;
	}

	void TestSampler::cleanup()
	{
	}

	void TestSampler::bind( uint32_t p_index )const
	{
	}

	void TestSampler::unbind( uint32_t p_index )const
	{
	}
}
