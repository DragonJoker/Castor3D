#include "GlVertexBufferObject.hpp"
#include "GlRenderSystem.hpp"
#include "GlShaderProgram.hpp"
#include "GlAttribute.hpp"
#include "OpenGl.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlVertexBufferObject::GlVertexBufferObject( GlRenderSystem & p_renderSystem, OpenGl & p_gl, BufferDeclaration const & p_declaration, HardwareBufferPtr p_pBuffer )
		: GlBuffer< uint8_t >( p_renderSystem, p_gl, eGL_BUFFER_TARGET_ARRAY, p_pBuffer )
		, m_bufferDeclaration( p_declaration )
	{
		for ( auto l_element : m_bufferDeclaration )
		{
			String l_name;

			switch ( l_element.m_eUsage )
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

			GlAttributeBaseSPtr l_attribute;

			switch ( l_element.m_eDataType )
			{
			case eELEMENT_TYPE_1FLOAT:
				l_attribute = std::make_shared< GlAttribute1r >( p_gl, &p_renderSystem, l_name );
				break;

			case eELEMENT_TYPE_2FLOATS:
				l_attribute = std::make_shared< GlAttribute2r >( p_gl, &p_renderSystem, l_name );
				break;

			case eELEMENT_TYPE_3FLOATS:
				l_attribute = std::make_shared< GlAttribute3r >( p_gl, &p_renderSystem, l_name );
				break;

			case eELEMENT_TYPE_4FLOATS:
				l_attribute = std::make_shared< GlAttribute4r >( p_gl, &p_renderSystem, l_name );
				break;

			case eELEMENT_TYPE_COLOUR:
				l_attribute = std::make_shared< GlAttribute1ui >( p_gl, &p_renderSystem, l_name );
				break;

			case eELEMENT_TYPE_1INT:
				l_attribute = std::make_shared< GlAttribute1i >( p_gl, &p_renderSystem, l_name );
				break;

			case eELEMENT_TYPE_2INTS:
				l_attribute = std::make_shared< GlAttribute2i >( p_gl, &p_renderSystem, l_name );
				break;

			case eELEMENT_TYPE_3INTS:
				l_attribute = std::make_shared< GlAttribute3i >( p_gl, &p_renderSystem, l_name );
				break;

			case eELEMENT_TYPE_4INTS:
				l_attribute = std::make_shared< GlAttribute4i >( p_gl, &p_renderSystem, l_name );
				break;
			}

			l_attribute->SetOffset( l_element.m_uiOffset );
			l_attribute->SetStride( m_bufferDeclaration.GetStride() );
			m_arrayAttributes.push_back( l_attribute );
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

	bool GlVertexBufferObject::Initialise( eBUFFER_ACCESS_TYPE p_type, eBUFFER_ACCESS_NATURE p_eNature, ShaderProgramBaseSPtr p_pProgram )
	{
		bool l_return = true;
		GlShaderProgramSPtr l_pNewProgram = std::static_pointer_cast< GlShaderProgram >( p_pProgram );
		GlShaderProgramSPtr l_pOldProgram = m_pProgram.lock();

		if ( l_pOldProgram != l_pNewProgram )
		{
			for ( auto l_attribute : m_arrayAttributes )
			{
				l_attribute->SetShader( l_pNewProgram );
			}

			m_pProgram = l_pNewProgram;

			if ( l_pNewProgram )
			{
				l_return = DoAttributesInitialise();
			}

			if ( l_return )
			{
				l_return = GlBuffer< uint8_t >::DoInitialise( p_type, p_eNature );
			}
		}
		else if ( !l_pOldProgram )
		{
			l_return = GlBuffer< uint8_t >::DoInitialise( p_type, p_eNature );
		}

		return l_return;
	}

	void GlVertexBufferObject::Cleanup()
	{
		DoAttributesCleanup();
		GlBuffer< uint8_t >::DoCleanup();
	}

	bool GlVertexBufferObject::Bind()
	{
		HardwareBufferPtr l_pBuffer = GetCpuBuffer();
		bool l_return = l_pBuffer && l_pBuffer->IsAssigned();

		if ( l_return )
		{
			l_return = GlBuffer< uint8_t >::DoBind();

			if ( !GetOwner()->UseShaders() || m_pProgram.expired() )
			{
				static const uint32_t s_arraySize[] = { 1, 2, 3, 4, 4, 1, 2, 3, 4 };
				static const uint32_t s_arrayType[] = { eGL_TYPE_REAL, eGL_TYPE_REAL, eGL_TYPE_REAL, eGL_TYPE_REAL, eGL_TYPE_UNSIGNED_BYTE, eGL_TYPE_UNSIGNED_INT, eGL_TYPE_UNSIGNED_INT, eGL_TYPE_UNSIGNED_INT, eGL_TYPE_UNSIGNED_INT };

				for ( BufferDeclaration::BufferElementDeclarationArrayConstIt l_it = m_bufferDeclaration.begin(); l_return && l_it != m_bufferDeclaration.end(); ++l_it )
				{
					switch ( l_it->m_eUsage )
					{
					case eELEMENT_USAGE_POSITION:
						l_return &= m_gl.EnableClientState( eGL_BUFFER_USAGE_VERTEX_ARRAY );
						l_return &= m_gl.VertexPointer( s_arraySize[l_it->m_eDataType], s_arrayType[l_it->m_eDataType], m_bufferDeclaration.GetStride(), BUFFER_OFFSET( l_it->m_uiOffset ) );
						break;

					case eELEMENT_USAGE_NORMAL:
						l_return &= m_gl.EnableClientState( eGL_BUFFER_USAGE_NORMAL_ARRAY );
						l_return &= m_gl.NormalPointer( s_arrayType[l_it->m_eDataType], m_bufferDeclaration.GetStride(), BUFFER_OFFSET( l_it->m_uiOffset ) );
						break;

					case eELEMENT_USAGE_TANGENT:

						if ( m_gl.HasTangentPointer() )
						{
							l_return &= m_gl.EnableClientState( eGL_BUFFER_USAGE_TANGENT_ARRAY );
							l_return &= m_gl.TangentPointer( s_arrayType[l_it->m_eDataType], m_bufferDeclaration.GetStride(), BUFFER_OFFSET( l_it->m_uiOffset ) );
						}

						break;

					case eELEMENT_USAGE_BITANGENT:

						if ( m_gl.HasBinormalPointer() )
						{
							l_return &= m_gl.EnableClientState( eGL_BUFFER_USAGE_BINORMAL_ARRAY );
							l_return &= m_gl.BinormalPointer( s_arrayType[l_it->m_eDataType], m_bufferDeclaration.GetStride(), BUFFER_OFFSET( l_it->m_uiOffset ) );
						}

						break;

					case eELEMENT_USAGE_DIFFUSE:
						l_return &= m_gl.EnableClientState( eGL_BUFFER_USAGE_COLOR_ARRAY );
						l_return &= m_gl.ColorPointer( s_arraySize[l_it->m_eDataType], s_arrayType[l_it->m_eDataType], m_bufferDeclaration.GetStride(), BUFFER_OFFSET( l_it->m_uiOffset ) );
						break;

					case eELEMENT_USAGE_TEXCOORDS0:
						l_return &= m_gl.ClientActiveTexture( eGL_TEXTURE_INDEX_0 );
						l_return &= m_gl.EnableClientState( eGL_BUFFER_USAGE_TEXTURE_COORD_ARRAY );
						l_return &= m_gl.TexCoordPointer( s_arraySize[l_it->m_eDataType], s_arrayType[l_it->m_eDataType], m_bufferDeclaration.GetStride(), BUFFER_OFFSET( l_it->m_uiOffset ) );
						break;

					case eELEMENT_USAGE_TEXCOORDS1:
						l_return &= m_gl.ClientActiveTexture( eGL_TEXTURE_INDEX_1 );
						l_return &= m_gl.EnableClientState( eGL_BUFFER_USAGE_TEXTURE_COORD_ARRAY );
						l_return &= m_gl.TexCoordPointer( s_arraySize[l_it->m_eDataType], s_arrayType[l_it->m_eDataType], m_bufferDeclaration.GetStride(), BUFFER_OFFSET( l_it->m_uiOffset ) );
						break;

					case eELEMENT_USAGE_TEXCOORDS2:
						l_return &= m_gl.ClientActiveTexture( eGL_TEXTURE_INDEX_2 );
						l_return &= m_gl.EnableClientState( eGL_BUFFER_USAGE_TEXTURE_COORD_ARRAY );
						l_return &= m_gl.TexCoordPointer( s_arraySize[l_it->m_eDataType], s_arrayType[l_it->m_eDataType], m_bufferDeclaration.GetStride(), BUFFER_OFFSET( l_it->m_uiOffset ) );
						break;

					case eELEMENT_USAGE_TEXCOORDS3:
						l_return &= m_gl.ClientActiveTexture( eGL_TEXTURE_INDEX_3 );
						l_return &= m_gl.EnableClientState( eGL_BUFFER_USAGE_TEXTURE_COORD_ARRAY );
						l_return &= m_gl.TexCoordPointer( s_arraySize[l_it->m_eDataType], s_arrayType[l_it->m_eDataType], m_bufferDeclaration.GetStride(), BUFFER_OFFSET( l_it->m_uiOffset ) );
						break;
					}
				}
			}
			else
			{
				DoAttributesBind();
			}
		}

		return l_return;
	}

	void GlVertexBufferObject::Unbind()
	{
		HardwareBufferPtr l_pBuffer = GetCpuBuffer();

		if ( l_pBuffer && l_pBuffer->IsAssigned() )
		{
			if ( GetOwner()->UseShaders() && ! m_pProgram.expired() )
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
		for ( auto && l_attribute : m_arrayAttributes )
		{
			l_attribute->Cleanup();
		}
	}

	bool GlVertexBufferObject::DoAttributesInitialise()
	{
		m_uiValid = 0;

		for ( auto && l_attribute : m_arrayAttributes )
		{
			m_uiValid += ( l_attribute->Initialise() ? 1 : 0 );
		}

		return m_uiValid > 0;
	}

	bool GlVertexBufferObject::DoAttributesBind()
	{
		bool l_return = true;

		for ( auto && l_it = m_arrayAttributes.begin(); l_it != m_arrayAttributes.end() && l_return; ++l_it )
		{
			if ( ( *l_it )->GetLocation() != eGL_INVALID_INDEX )
			{
				l_return = ( *l_it )->Bind( false );
			}
		}

		return l_return;
	}

	void GlVertexBufferObject::DoAttributesUnbind()
	{
		for ( auto && l_attribute : m_arrayAttributes )
		{
			if ( l_attribute->GetLocation() != eGL_INVALID_INDEX )
			{
				l_attribute->Unbind();
			}
		}
	}
}
