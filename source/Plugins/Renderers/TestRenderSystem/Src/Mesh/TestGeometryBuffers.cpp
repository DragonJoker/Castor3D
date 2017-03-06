#include "Mesh/TestGeometryBuffers.hpp"

using namespace Castor3D;
using namespace Castor;

namespace TestRender
{
	TestGeometryBuffers::TestGeometryBuffers( Topology p_topology, ShaderProgram const & p_program )
		: GeometryBuffers( p_topology, p_program )
	{
	}

	TestGeometryBuffers::~TestGeometryBuffers()
	{
	}

	bool TestGeometryBuffers::Draw( uint32_t p_size, uint32_t p_index )const
	{
		return true;
	}

	bool TestGeometryBuffers::DrawInstanced( uint32_t p_size, uint32_t p_index, uint32_t p_count )const
	{
		return true;
	}

	bool TestGeometryBuffers::DoInitialise()
	{
		return true;
	}

	void TestGeometryBuffers::DoCleanup()
	{
	}
}
