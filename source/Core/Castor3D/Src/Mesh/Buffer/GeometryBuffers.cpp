#include "GeometryBuffers.hpp"

using namespace Castor;

namespace Castor3D
{
	GeometryBuffers::GeometryBuffers( eTOPOLOGY p_topology, ShaderProgram const & p_program )
		: m_topology{ p_topology }
		, m_program{ p_program }
	{
	}

	GeometryBuffers::~GeometryBuffers()
	{
	}

	bool GeometryBuffers::Initialise( VertexBufferSPtr p_vtx, VertexBufferSPtr p_anm, IndexBufferSPtr p_idx, VertexBufferSPtr p_bones, VertexBufferSPtr p_inst )
	{
		m_vertexBuffer = p_vtx;
		m_animationBuffer = p_anm;
		m_indexBuffer = p_idx;
		m_bonesBuffer = p_bones;
		m_matrixBuffer = p_inst;
		return DoInitialise();
	}

	void GeometryBuffers::Cleanup()
	{
		DoCleanup();
		m_vertexBuffer.reset();
		m_animationBuffer.reset();
		m_indexBuffer.reset();
		m_bonesBuffer.reset();
		m_matrixBuffer.reset();
	}
}
