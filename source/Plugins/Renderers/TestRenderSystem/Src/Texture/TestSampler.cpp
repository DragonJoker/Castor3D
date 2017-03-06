#include "Texture/TestSampler.hpp"

#include "Render/TestRenderSystem.hpp"

using namespace Castor3D;
using namespace Castor;

namespace TestRender
{
	TestSampler::TestSampler( TestRenderSystem * p_renderSystem, Castor::String const & p_name )
		: Sampler( *p_renderSystem->GetEngine(), p_name )
	{
	}

	TestSampler::~TestSampler()
	{
	}

	bool TestSampler::Initialise()
	{
		return true;
	}

	void TestSampler::Cleanup()
	{
	}

	void TestSampler::Bind( uint32_t p_index )const
	{
	}

	void TestSampler::Unbind( uint32_t p_index )const
	{
	}
}
