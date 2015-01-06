#include "GlVertexBufferObject.hpp"
#include "GlRenderSystem.hpp"
#include "GlShaderProgram.hpp"
#include "GlAttribute.hpp"
#include "OpenGl.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlVertexBufferObject::GlVertexBufferObject( OpenGl & p_gl, BufferDeclaration const & p_declaration, HardwareBufferPtr p_pBuffer )
		:	GlBuffer< uint8_t >( p_gl, eGL_BUFFER_TARGET_ARRAY, p_pBuffer )
		,	m_bufferDeclaration( p_declaration )
	{
		GlAttributeBaseSPtr l_pAttribute;
		GlRenderSystem * l_pRenderSystem = static_cast< GlRenderSystem * >( p_pBuffer->GetRenderSystem() );

		for ( BufferDeclaration::BufferElementDeclarationArrayConstIt l_it = m_bufferDeclaration.Begin(); l_it != m_bufferDeclaration.End(); ++l_it )
		{
			String l_name;

			switch ( l_it->m_eUsage )
			{
			case eELEMENT_USAGE_POSITION:
				l_name = cuT( "vertex" );
				break;

			case eELEMENT_USAGE_NORMAL:
				l_name = cuT( "normal" );
				break;

			case eELEMENT_USAGE_TANGENT:
				l_name = cuT( "tangent" );
				break;

			case eELEMENT_USAGE_BITANGENT:
				l_name = cuT( "bitangent" );
				break;

			case eELEMENT_USAGE_DIFFUSE:
				l_name = cuT( "colour" );
				break;

			case eELEMENT_USAGE_TEXCOORDS0:
			case eELEMENT_USAGE_TEXCOORDS1:
			case eELEMENT_USAGE_TEXCOORDS2:
			case eELEMENT_USAGE_TEXCOORDS3:
				l_name = cuT( "texture" );
				break;

			case eELEMENT_USAGE_BONE_IDS:
				l_name = cuT( "bone_ids" );
				break;

			case eELEMENT_USAGE_BONE_WEIGHTS:
				l_name = cuT( "weights" );
				break;
			}

			switch ( l_it->m_eDataType )
			{
			case eELEMENT_TYPE_1FLOAT:
				l_pAttribute = std::make_shared< GlAttribute1r >( p_gl, l_pRenderSystem, l_name );
				break;

			case eELEMENT_TYPE_2FLOATS:
				l_pAttribute = std::make_shared< GlAttribute2r >( p_gl, l_pRenderSystem, l_name );
				break;

			case eELEMENT_TYPE_3FLOATS:
				l_pAttribute = std::make_shared< GlAttribute3r >( p_gl, l_pRenderSystem, l_name );
				break;

			case eELEMENT_TYPE_4FLOATS:
				l_pAttribute = std::make_shared< GlAttribute4r >( p_gl, l_pRenderSystem, l_name );
				break;

			case eELEMENT_TYPE_COLOUR:
				l_pAttribute = std::make_shared< GlAttribute1ui >( p_gl, l_pRenderSystem, l_name );
				break;

			case eELEMENT_TYPE_1INT:
				l_pAttribute = std::make_shared< GlAttribute1i >( p_gl, l_pRenderSystem, l_name );
				break;

			case eELEMENT_TYPE_2INTS:
				l_pAttribute = std::make_shared< GlAttribute2i >( p_gl, l_pRenderSystem, l_name );
				break;

			case eELEMENT_TYPE_3INTS:
				l_pAttribute = std::make_shared< GlAttribute3i >( p_gl, l_pRenderSystem, l_name );
				break;

			case eELEMENT_TYPE_4INTS:
				l_pAttribute = std::make_shared< GlAttribute4i >( p_gl, l_pRenderSystem, l_name );
				break;
			}

			l_pAttribute->SetOffset( l_it->m_uiOffset );
			l_pAttribute->SetStride( m_bufferDeclaration.GetStride() );
			m_arrayAttributes.push_back( l_pAttribute );
		}
	}

	GlVertexBufferObject::~GlVertexBufferObject()
	{
	}

	bool GlVertexBufferObject::Create()
	{
		return GlBuffer< uint8_t >::DoCreate();
	}

	void GlVertexBufferObject::Destroy()
	{
		GlBuffer< uint8_t >::DoDestroy();
	}

	bool GlVertexBufferObject::Initialise( eBUFFER_ACCESS_TYPE p_eType, eBUFFER_ACCESS_NATURE p_eNature, ShaderProgramBaseSPtr p_pProgram )
	{
		bool l_bReturn = true;
		GlShaderProgramSPtr l_pNewProgram = std::static_pointer_cast< GlShaderProgram >( p_pProgram );
		GlShaderProgramSPtr l_pOldProgram = m_pProgram.lock();

		if ( l_pOldProgram != l_pNewProgram )
		{
			std::for_each( m_arrayAttributes.begin(), m_arrayAttributes.end(), [&]( GlAttributeBaseSPtr p_pAttribute )
			{
				p_pAttribute->SetShader( l_pNewProgram );
			} );
			m_pProgram = l_pNewProgram;

			if ( l_pNewProgram )
			{
				l_bReturn = DoAttributesInitialise();
			}

			if ( l_bReturn )
			{
				l_bReturn = GlBuffer< uint8_t >::DoInitialise( p_eType, p_eNature );
			}
		}
		else if ( !l_pOldProgram )
		{
			l_bReturn = GlBuffer< uint8_t >::DoInitialise( p_eType, p_eNature );
		}

		return l_bReturn;
	}

	void GlVertexBufferObject::Cleanup()
	{
		DoAttributesCleanup();
		GlBuffer< uint8_t >::DoCleanup();
	}

	bool GlVertexBufferObject::Bind()
	{
		HardwareBufferPtr l_pBuffer = GetCpuBuffer();
		bool l_bReturn = l_pBuffer && l_pBuffer->IsAssigned();

		if ( l_bReturn )
		{
			l_bReturn = GlBuffer< uint8_t >::DoBind();

			if ( !l_pBuffer->GetRenderSystem()->UseShaders() || m_pProgram.expired() )
			{
				static const uint32_t s_arraySize[] = { 1, 2, 3, 4, 4, 1, 2, 3, 4 };
				static const uint32_t s_arrayType[] = { eGL_TYPE_REAL, eGL_TYPE_REAL, eGL_TYPE_REAL, eGL_TYPE_REAL, eGL_TYPE_UNSIGNED_BYTE, eGL_TYPE_UNSIGNED_INT, eGL_TYPE_UNSIGNED_INT, eGL_TYPE_UNSIGNED_INT, eGL_TYPE_UNSIGNED_INT };

				for ( BufferDeclaration::BufferElementDeclarationArrayConstIt l_it = m_bufferDeclaration.Begin(); l_bReturn && l_it != m_bufferDeclaration.End(); ++l_it )
				{
					switch ( l_it->m_eUsage )
					{
					case eELEMENT_USAGE_POSITION:
						l_bReturn &= m_gl.EnableClientState( eGL_BUFFER_USAGE_VERTEX_ARRAY );
						l_bReturn &= m_gl.VertexPointer( s_arraySize[l_it->m_eDataType], s_arrayType[l_it->m_eDataType], m_bufferDeclaration.GetStride(), BUFFER_OFFSET( l_it->m_uiOffset ) );
						break;

					case eELEMENT_USAGE_NORMAL:
						l_bReturn &= m_gl.EnableClientState( eGL_BUFFER_USAGE_NORMAL_ARRAY );
						l_bReturn &= m_gl.NormalPointer( s_arrayType[l_it->m_eDataType], m_bufferDeclaration.GetStride(), BUFFER_OFFSET( l_it->m_uiOffset ) );
						break;

					case eELEMENT_USAGE_TANGENT:

						if ( m_gl.HasTangentPointer() )
						{
							l_bReturn &= m_gl.EnableClientState( eGL_BUFFER_USAGE_TANGENT_ARRAY );
							l_bReturn &= m_gl.TangentPointer( s_arrayType[l_it->m_eDataType], m_bufferDeclaration.GetStride(), BUFFER_OFFSET( l_it->m_uiOffset ) );
						}

						break;

					case eELEMENT_USAGE_BITANGENT:

						if ( m_gl.HasBinormalPointer() )
						{
							l_bReturn &= m_gl.EnableClientState( eGL_BUFFER_USAGE_BINORMAL_ARRAY );
							l_bReturn &= m_gl.BinormalPointer( s_arrayType[l_it->m_eDataType], m_bufferDeclaration.GetStride(), BUFFER_OFFSET( l_it->m_uiOffset ) );
						}

						break;

					case eELEMENT_USAGE_DIFFUSE:
						l_bReturn &= m_gl.EnableClientState( eGL_BUFFER_USAGE_COLOR_ARRAY );
						l_bReturn &= m_gl.ColorPointer( s_arraySize[l_it->m_eDataType], s_arrayType[l_it->m_eDataType], m_bufferDeclaration.GetStride(), BUFFER_OFFSET( l_it->m_uiOffset ) );
						break;

					case eELEMENT_USAGE_TEXCOORDS0:
						l_bReturn &= m_gl.ClientActiveTexture( eGL_TEXTURE_INDEX_0 );
						l_bReturn &= m_gl.EnableClientState( eGL_BUFFER_USAGE_TEXTURE_COORD_ARRAY );
						l_bReturn &= m_gl.TexCoordPointer( s_arraySize[l_it->m_eDataType], s_arrayType[l_it->m_eDataType], m_bufferDeclaration.GetStride(), BUFFER_OFFSET( l_it->m_uiOffset ) );
						break;

					case eELEMENT_USAGE_TEXCOORDS1:
						l_bReturn &= m_gl.ClientActiveTexture( eGL_TEXTURE_INDEX_1 );
						l_bReturn &= m_gl.EnableClientState( eGL_BUFFER_USAGE_TEXTURE_COORD_ARRAY );
						l_bReturn &= m_gl.TexCoordPointer( s_arraySize[l_it->m_eDataType], s_arrayType[l_it->m_eDataType], m_bufferDeclaration.GetStride(), BUFFER_OFFSET( l_it->m_uiOffset ) );
						break;

					case eELEMENT_USAGE_TEXCOORDS2:
						l_bReturn &= m_gl.ClientActiveTexture( eGL_TEXTURE_INDEX_2 );
						l_bReturn &= m_gl.EnableClientState( eGL_BUFFER_USAGE_TEXTURE_COORD_ARRAY );
						l_bReturn &= m_gl.TexCoordPointer( s_arraySize[l_it->m_eDataType], s_arrayType[l_it->m_eDataType], m_bufferDeclaration.GetStride(), BUFFER_OFFSET( l_it->m_uiOffset ) );
						break;

					case eELEMENT_USAGE_TEXCOORDS3:
						l_bReturn &= m_gl.ClientActiveTexture( eGL_TEXTURE_INDEX_3 );
						l_bReturn &= m_gl.EnableClientState( eGL_BUFFER_USAGE_TEXTURE_COORD_ARRAY );
						l_bReturn &= m_gl.TexCoordPointer( s_arraySize[l_it->m_eDataType], s_arrayType[l_it->m_eDataType], m_bufferDeclaration.GetStride(), BUFFER_OFFSET( l_it->m_uiOffset ) );
						break;
					}
				}
			}
			else
			{
				DoAttributesBind();
			}
		}

		return l_bReturn;
	}

	void GlVertexBufferObject::Unbind()
	{
		HardwareBufferPtr l_pBuffer = GetCpuBuffer();

		if ( l_pBuffer && l_pBuffer->IsAssigned() )
		{
			if ( l_pBuffer->GetRenderSystem()->UseShaders() && ! m_pProgram.expired() )
			{
				DoAttributesUnbind();
			}
			else
			{
				m_gl.DisableClientState( eGL_BUFFER_USAGE_VERTEX_ARRAY );
				m_gl.DisableClientState( eGL_BUFFER_USAGE_NORMAL_ARRAY );
				m_gl.DisableClientState( eGL_BUFFER_USAGE_COLOR_ARRAY );

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

			GlBuffer< uint8_t >::DoUnbind();
		}
	}

	uint8_t * GlVertexBufferObject::Lock( uint32_t p_uiOffset, uint32_t p_uiCount, uint32_t p_uiFlags )
	{
		uint8_t * l_pReturn = NULL;
		HardwareBufferPtr l_pBuffer = GetCpuBuffer();

		if ( l_pBuffer && l_pBuffer->IsAssigned() )
		{
			l_pReturn = GlBuffer< uint8_t >::DoLock( p_uiOffset, p_uiCount, p_uiFlags );
		}

		return l_pReturn;
	}

	void GlVertexBufferObject::Unlock()
	{
		HardwareBufferPtr l_pBuffer = GetCpuBuffer();

		if ( l_pBuffer && l_pBuffer->IsAssigned() )
		{
			GlBuffer< uint8_t >::DoUnlock();
		}
	}

	void GlVertexBufferObject::DoAttributesCleanup()
	{
		for ( GlAttributePtrArrayIt l_it = m_arrayAttributes.begin(); l_it != m_arrayAttributes.end(); ++l_it )
		{
			( *l_it )->Cleanup();
		}
	}

	bool GlVertexBufferObject::DoAttributesInitialise()
	{
		m_uiValid = 0;

		for ( GlAttributePtrArrayIt l_it = m_arrayAttributes.begin(); l_it != m_arrayAttributes.end(); ++l_it )
		{
			m_uiValid += ( ( *l_it )->Initialise() ? 1 : 0 );
		}

		return m_uiValid > 0;
	}

	bool GlVertexBufferObject::DoAttributesBind()
	{
		bool l_bReturn = true;

		for ( GlAttributePtrArrayIt l_it = m_arrayAttributes.begin(); l_it != m_arrayAttributes.end() && l_bReturn; ++l_it )
		{
			if ( ( *l_it )->GetLocation() != eGL_INVALID_INDEX )
			{
				l_bReturn = ( *l_it )->Bind( false );
			}
		}

		return l_bReturn;
	}

	void GlVertexBufferObject::DoAttributesUnbind()
	{
		for ( GlAttributePtrArrayIt l_it = m_arrayAttributes.begin(); l_it != m_arrayAttributes.end(); ++l_it )
		{
			if ( ( *l_it )->GetLocation() != eGL_INVALID_INDEX )
			{
				( *l_it )->Unbind();
			}
		}
	}
}
