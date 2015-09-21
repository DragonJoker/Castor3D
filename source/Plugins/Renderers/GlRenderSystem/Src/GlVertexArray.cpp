#include "GlVertexArray.hpp"
#include "OpenGl.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlVertexArray::GlVertexArray( OpenGl & p_gl, BufferDeclaration const & p_declaration, HardwareBufferPtr p_pBuffer )
		:	GpuBuffer< uint8_t >()
		,	m_bufferDeclaration( p_declaration )
		,	m_pBuffer( p_pBuffer )
		,	m_gl( p_gl )
	{
	}

	GlVertexArray::~GlVertexArray()
	{
	}

	bool GlVertexArray::Create()
	{
		return true;
	}

	void GlVertexArray::Destroy()
	{
	}

	bool GlVertexArray::Initialise( eBUFFER_ACCESS_TYPE CU_PARAM_UNUSED( p_type ), eBUFFER_ACCESS_NATURE CU_PARAM_UNUSED( p_eNature ), Castor3D::ShaderProgramBaseSPtr CU_PARAM_UNUSED( p_pProgram ) )
	{
		return true;
	}

	void GlVertexArray::Cleanup()
	{
	}

	bool GlVertexArray::Bind()
	{
		HardwareBufferPtr l_pBuffer = GetCpuBuffer();
		bool l_return = true;
		static const uint32_t s_arraySize[] = { 1, 2, 3, 4, 4, 1, 2, 3, 4 };
		static const uint32_t s_arrayType[] = { eGL_TYPE_REAL, eGL_TYPE_REAL, eGL_TYPE_REAL, eGL_TYPE_REAL, eGL_TYPE_UNSIGNED_BYTE, eGL_TYPE_UNSIGNED_INT, eGL_TYPE_UNSIGNED_INT, eGL_TYPE_UNSIGNED_INT, eGL_TYPE_UNSIGNED_INT };
		uint8_t const * l_pData = &l_pBuffer->data()[0];

		for ( BufferDeclaration::BufferElementDeclarationArrayConstIt l_it = m_bufferDeclaration.Begin(); l_it != m_bufferDeclaration.End() && l_return; ++l_it )
		{
			switch ( l_it->m_eUsage )
			{
			case eELEMENT_USAGE_POSITION:
				l_return &= m_gl.EnableClientState( eGL_BUFFER_USAGE_VERTEX_ARRAY );
				l_return &= m_gl.VertexPointer( s_arraySize[l_it->m_eDataType], s_arrayType[l_it->m_eDataType], m_bufferDeclaration.GetStride(), &l_pData[l_it->m_uiOffset] );
				break;

			case eELEMENT_USAGE_NORMAL:
				l_return &= m_gl.EnableClientState( eGL_BUFFER_USAGE_NORMAL_ARRAY );
				l_return &= m_gl.NormalPointer( s_arrayType[l_it->m_eDataType], m_bufferDeclaration.GetStride(), &l_pData[l_it->m_uiOffset] );
				break;

			case eELEMENT_USAGE_TANGENT:

				if ( m_gl.HasTangentPointer() )
				{
					l_return &= m_gl.EnableClientState( eGL_BUFFER_USAGE_TANGENT_ARRAY );
					l_return &= m_gl.TangentPointer( s_arrayType[l_it->m_eDataType], m_bufferDeclaration.GetStride(), &l_pData[l_it->m_uiOffset] );
				}

				break;

			case eELEMENT_USAGE_BITANGENT:

				if ( m_gl.HasBinormalPointer() )
				{
					l_return &= m_gl.EnableClientState( eGL_BUFFER_USAGE_BINORMAL_ARRAY );
					l_return &= m_gl.BinormalPointer( s_arrayType[l_it->m_eDataType], m_bufferDeclaration.GetStride(), &l_pData[l_it->m_uiOffset] );
				}

				break;

			case eELEMENT_USAGE_DIFFUSE:
				l_return &= m_gl.EnableClientState( eGL_BUFFER_USAGE_COLOR_ARRAY );
				l_return &= m_gl.ColorPointer( s_arraySize[l_it->m_eDataType], s_arrayType[l_it->m_eDataType], m_bufferDeclaration.GetStride(), &l_pData[l_it->m_uiOffset] );
				break;

			case eELEMENT_USAGE_TEXCOORDS0:
				l_return &= m_gl.ClientActiveTexture( eGL_TEXTURE_INDEX_0 );
				l_return &= m_gl.EnableClientState( eGL_BUFFER_USAGE_TEXTURE_COORD_ARRAY );
				l_return &= m_gl.TexCoordPointer( s_arraySize[l_it->m_eDataType], s_arrayType[l_it->m_eDataType], m_bufferDeclaration.GetStride(), &l_pData[l_it->m_uiOffset] );
				break;

			case eELEMENT_USAGE_TEXCOORDS1:
				l_return &= m_gl.ClientActiveTexture( eGL_TEXTURE_INDEX_1 );
				l_return &= m_gl.EnableClientState( eGL_BUFFER_USAGE_TEXTURE_COORD_ARRAY );
				l_return &= m_gl.TexCoordPointer( s_arraySize[l_it->m_eDataType], s_arrayType[l_it->m_eDataType], m_bufferDeclaration.GetStride(), &l_pData[l_it->m_uiOffset] );
				break;

			case eELEMENT_USAGE_TEXCOORDS2:
				l_return &= m_gl.ClientActiveTexture( eGL_TEXTURE_INDEX_2 );
				l_return &= m_gl.EnableClientState( eGL_BUFFER_USAGE_TEXTURE_COORD_ARRAY );
				l_return &= m_gl.TexCoordPointer( s_arraySize[l_it->m_eDataType], s_arrayType[l_it->m_eDataType], m_bufferDeclaration.GetStride(), &l_pData[l_it->m_uiOffset] );
				break;

			case eELEMENT_USAGE_TEXCOORDS3:
				l_return &= m_gl.ClientActiveTexture( eGL_TEXTURE_INDEX_3 );
				l_return &= m_gl.EnableClientState( eGL_BUFFER_USAGE_TEXTURE_COORD_ARRAY );
				l_return &= m_gl.TexCoordPointer( s_arraySize[l_it->m_eDataType], s_arrayType[l_it->m_eDataType], m_bufferDeclaration.GetStride(), &l_pData[l_it->m_uiOffset] );
				break;
			}
		}

		return l_return;
	}

	void GlVertexArray::Unbind()
	{
		m_gl.DisableClientState( eGL_BUFFER_USAGE_VERTEX_ARRAY );
		m_gl.DisableClientState( eGL_BUFFER_USAGE_NORMAL_ARRAY );
		m_gl.DisableClientState( eGL_BUFFER_USAGE_COLOR_ARRAY );
		m_gl.DisableClientState( eGL_BUFFER_USAGE_TANGENT_ARRAY );

		if ( m_gl.HasBinormalPointer() )
		{
			m_gl.DisableClientState( eGL_BUFFER_USAGE_BINORMAL_ARRAY );
		}

		if ( m_gl.HasTangentPointer() )
		{
			m_gl.DisableClientState( eGL_BUFFER_USAGE_TANGENT_ARRAY );
		}

		for ( int i = 0; i < 4; ++i )
		{
			m_gl.ClientActiveTexture( eGL_TEXTURE_INDEX( eGL_TEXTURE_INDEX_0 + i ) );
			m_gl.DisableClientState( eGL_BUFFER_USAGE_TEXTURE_COORD_ARRAY );
		}
	}
}
