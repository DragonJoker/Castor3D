#include "Face.hpp"

using namespace Castor;

namespace Castor3D
{
	Face::BinaryParser::BinaryParser( Path const & p_path )
		: Castor3D::BinaryParser< Face >{ p_path }
	{
	}

	bool Face::BinaryParser::Fill( Face const & p_obj, BinaryChunk & p_chunk )const
	{
		return DoFillChunk( p_obj.m_index, 3, eCHUNK_TYPE_SUBMESH_FACE, p_chunk );
	}

	bool Face::BinaryParser::Parse( Face & p_obj, BinaryChunk & p_chunk )const
	{
		return DoParseChunk( p_obj.m_index, 3, p_chunk );
	}

	//*************************************************************************************************

	Face::Face( uint32_t a, uint32_t b, uint32_t c )
		: m_index{ a, b, c }
	{
	}
}
