#include "GeometryBuffers.hpp"

using namespace Castor;

namespace Castor3D
{
	GeometryBuffers::GeometryBuffers( Topology p_topology, ShaderProgram const & p_program )
		: m_topology{ p_topology }
		, m_program{ p_program }
	{
	}

	GeometryBuffers::~GeometryBuffers()
	{
	}

	bool GeometryBuffers::Initialise( VertexBufferArray const & p_buffers, IndexBufferRPtr p_index )
	{
		m_buffers = p_buffers;
		m_indexBuffer = p_index;
		return DoInitialise();
	}

	void GeometryBuffers::Cleanup()
	{
		DoCleanup();
		m_indexBuffer = nullptr;
		m_buffers.clear();
	}
}
