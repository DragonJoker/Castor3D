#include "GeometryBuffers.hpp"

using namespace castor;

namespace castor3d
{
	GeometryBuffers::GeometryBuffers( Topology p_topology, ShaderProgram const & p_program )
		: m_topology{ p_topology }
		, m_program{ p_program }
	{
	}

	GeometryBuffers::~GeometryBuffers()
	{
	}

	bool GeometryBuffers::initialise( VertexBufferArray const & p_buffers, IndexBufferRPtr p_index )
	{
		m_buffers = p_buffers;
		m_indexBuffer = p_index;
		return doInitialise();
	}

	void GeometryBuffers::cleanup()
	{
		doCleanup();
		m_indexBuffer = nullptr;
		m_buffers.clear();
	}

	void GeometryBuffers::setTopology( Topology p_value )
	{
		m_topology = p_value;
		doSetTopology( p_value );
	}
}
