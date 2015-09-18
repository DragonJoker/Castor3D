#include "Face.hpp"

using namespace Castor;

namespace Castor3D
{
	Face::TextLoader::TextLoader( File::eENCODING_MODE p_eEncodingMode )
		:	Loader< Face, eFILE_TYPE_TEXT, TextFile >( File::eOPEN_MODE_DUMMY, p_eEncodingMode )
	{
	}

	bool Face::TextLoader::operator()( Castor3D::Face const & p_face, TextFile & p_file )
	{
		bool l_return = p_file.Print( 1024, cuT( "\t\t\t\tface %i %i %i\n" ), p_face.GetVertexIndex( 0 ), p_face.GetVertexIndex( 1 ), p_face.GetVertexIndex( 2 ) ) > 0;
		return l_return;
	}

	//*************************************************************************************************

	Face::BinaryParser::BinaryParser( Path const & p_path )
		:	Castor3D::BinaryParser< Face >( p_path )
	{
	}

	bool Face::BinaryParser::Fill( Face const & p_obj, BinaryChunk & p_chunk )const
	{
		return DoFillChunk( p_obj.m_pIndex, 3, eCHUNK_TYPE_SUBMESH_FACE, p_chunk );
	}

	bool Face::BinaryParser::Parse( Face & p_obj, BinaryChunk & p_chunk )const
	{
		return DoParseChunk( p_obj.m_pIndex, 3, p_chunk );
	}

	//*************************************************************************************************

	Face::Face( uint32_t a, uint32_t b, uint32_t c )
	{
		m_pIndex[0] = a;
		m_pIndex[1] = b;
		m_pIndex[2] = c;
	}

	Face::Face( Face const & p_object )
	{
		m_pIndex[0] = p_object.m_pIndex[0];
		m_pIndex[1] = p_object.m_pIndex[1];
		m_pIndex[2] = p_object.m_pIndex[2];
	}

	Face::Face( Face && p_object )
	{
		m_pIndex[0] = std::move( p_object.m_pIndex[0] );
		m_pIndex[1] = std::move( p_object.m_pIndex[1] );
		m_pIndex[2] = std::move( p_object.m_pIndex[2] );
		p_object.m_pIndex[0] = 0;
		p_object.m_pIndex[1] = 0;
		p_object.m_pIndex[2] = 0;
	}

	Face & Face::operator =( Face const & p_object )
	{
		Face l_face( p_object );
		std::swap( *this, l_face );
		return *this;
	}

	Face & Face::operator =( Face && p_object )
	{
		if ( this != &p_object )
		{
			m_pIndex[0] = std::move( p_object.m_pIndex[0] );
			m_pIndex[1] = std::move( p_object.m_pIndex[1] );
			m_pIndex[2] = std::move( p_object.m_pIndex[2] );
			p_object.m_pIndex[0] = 0;
			p_object.m_pIndex[1] = 0;
			p_object.m_pIndex[2] = 0;
		}

		return *this;
	}

	Face::~Face()
	{
	}
}
