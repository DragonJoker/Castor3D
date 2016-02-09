#include "GeometryBuffers.hpp"

#include "IndexBuffer.hpp"
#include "MatrixBuffer.hpp"
#include "ShaderProgram.hpp"
#include "Submesh.hpp"
#include "VertexBuffer.hpp"

using namespace Castor;

namespace Castor3D
{
	GeometryBuffers::GeometryBuffers( eTOPOLOGY p_topology, ShaderProgram const & p_program, VertexBuffer * p_vtx, IndexBuffer * p_idx, VertexBuffer * p_bones, MatrixBuffer * p_inst )
		: m_vertexBuffer( p_vtx )
		, m_indexBuffer( p_idx )
		, m_bonesBuffer( p_bones )
		, m_matrixBuffer( p_inst )
		, m_topology( p_topology )
		, m_layout( p_program.GetLayout() )
	{
	}

	GeometryBuffers::~GeometryBuffers()
	{
	}
}
