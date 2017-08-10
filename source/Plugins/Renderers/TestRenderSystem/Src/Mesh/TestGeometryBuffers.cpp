#include "Mesh/TestGeometryBuffers.hpp"

using namespace castor3d;
using namespace castor;

namespace TestRender
{
	TestGeometryBuffers::TestGeometryBuffers( Topology p_topology, ShaderProgram const & p_program )
		: GeometryBuffers( p_topology, p_program )
	{
	}

	TestGeometryBuffers::~TestGeometryBuffers()
	{
	}

	bool TestGeometryBuffers::draw( uint32_t p_size, uint32_t p_index )const
	{
		return true;
	}

	bool TestGeometryBuffers::drawInstanced( uint32_t p_size, uint32_t p_index, uint32_t p_count )const
	{
		return true;
	}

	bool TestGeometryBuffers::doInitialise()
	{
		return true;
	}

	void TestGeometryBuffers::doCleanup()
	{
	}
}
