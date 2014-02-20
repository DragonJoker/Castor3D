#include "GlRenderSystem/PrecompiledHeader.hpp"

#include "GlRenderSystem/GlBuffer.hpp"
#include "GlRenderSystem/OpenGl.hpp"
#include "GlRenderSystem/GlRenderSystem.hpp"

using namespace GlRender;
using namespace Castor3D;
using namespace Castor;

//******************************************************************************

GlAttributeBase :: GlAttributeBase( OpenGl & p_gl, GlRenderSystem * p_pRenderSystem, String const & p_strAttribName, eGL_TYPE p_eGlType, int p_iCount )
	:	m_strAttribName		( p_strAttribName				)
	,	m_uiAttribLocation	( uint32_t( eGL_INVALID_INDEX )	)
	,	m_iCount			( p_iCount						)
	,	m_uiOffset			( 0								)
	,	m_iStride			( 0								)
	,	m_eGlType			( p_eGlType						)
	,	m_pRenderSystem		( p_pRenderSystem				)
	,	m_gl				( p_gl							)
{
}

GlAttributeBase :: ~GlAttributeBase()
{
}

void GlAttributeBase :: SetShader( ShaderProgramBaseSPtr p_pProgram )
{
	m_pProgram = p_pProgram;
}

bool GlAttributeBase :: Initialise()
{
	bool l_bReturn = !m_pRenderSystem->UseShaders();

	if ( !m_pProgram.expired() && m_pProgram.lock()->GetStatus() == ePROGRAM_STATUS_LINKED && !l_bReturn )
	{
		m_uiAttribLocation = m_pProgram.lock()->GetAttributeLocation( m_strAttribName );

		if( m_uiAttribLocation != eGL_INVALID_INDEX )
		{
			l_bReturn = true;
		}
	}

	return l_bReturn;
}

void GlAttributeBase :: Cleanup()
{
	m_uiAttribLocation = uint32_t( eGL_INVALID_INDEX );
	m_pProgram.reset();
}

bool GlAttributeBase :: Bind( bool p_bNormalised )
{
	bool l_bReturn = m_gl.EnableVertexAttribArray( m_uiAttribLocation );

	if( m_eGlType == eGL_TYPE_INT )
	{
		l_bReturn &= m_gl.VertexAttribPointer( m_uiAttribLocation, m_iCount, m_eGlType, m_iStride, BUFFER_OFFSET( m_uiOffset ) );
	}
	else
	{
		l_bReturn &= m_gl.VertexAttribPointer( m_uiAttribLocation, m_iCount, m_eGlType, p_bNormalised, m_iStride, BUFFER_OFFSET( m_uiOffset ) );
	}

	return l_bReturn;
}

void GlAttributeBase :: Unbind()
{
	m_gl.DisableVertexAttribArray( m_uiAttribLocation );
}

//******************************************************************************

GlIndexArray :: GlIndexArray( OpenGl & p_gl, HardwareBufferSPtr p_pBuffer )
	:	GpuBuffer	(			)
	,	m_wpBuffer	( p_pBuffer	)
	,	m_gl		( p_gl		)
{
}

GlIndexArray :: ~GlIndexArray()
{
}

bool GlIndexArray :: Create()
{
	return true;
}

void GlIndexArray :: Destroy()
{
}

bool GlIndexArray :: Initialise( eBUFFER_ACCESS_TYPE CU_PARAM_UNUSED( p_eType ), eBUFFER_ACCESS_NATURE CU_PARAM_UNUSED( p_eNature ), Castor3D::ShaderProgramBaseSPtr CU_PARAM_UNUSED( p_pProgram ) )
{
	return true;
}

void GlIndexArray :: Cleanup()
{
}

bool GlIndexArray :: Bind()
{
	return true;
}

void GlIndexArray :: Unbind()
{
}

//******************************************************************************

GlVertexArray :: GlVertexArray( OpenGl & p_gl, BufferDeclaration const & p_declaration, HardwareBufferSPtr p_pBuffer )
	:	GpuBuffer< uint8_t >	(				)
	,	m_bufferDeclaration		( p_declaration	)
	,	m_wpBuffer				( p_pBuffer		)
	,	m_gl					( p_gl			)
{
}

GlVertexArray :: ~GlVertexArray()
{
}

bool GlVertexArray :: Create()
{
	return true;
}

void GlVertexArray :: Destroy()
{
}

bool GlVertexArray :: Initialise( eBUFFER_ACCESS_TYPE CU_PARAM_UNUSED( p_eType ), eBUFFER_ACCESS_NATURE CU_PARAM_UNUSED( p_eNature ), Castor3D::ShaderProgramBaseSPtr CU_PARAM_UNUSED( p_pProgram ) )
{
	return true;
}

void GlVertexArray :: Cleanup()
{
}

bool GlVertexArray :: Bind()
{
	HardwareBufferSPtr l_pBuffer = GetCpuBuffer();
	bool l_bReturn = true;
	static const uint32_t s_arraySize[] = { 1, 2, 3, 4, 4, 1, 2, 3, 4 };
	static const uint32_t s_arrayType[] = { eGL_TYPE_REAL, eGL_TYPE_REAL, eGL_TYPE_REAL, eGL_TYPE_REAL, eGL_TYPE_UNSIGNED_BYTE, eGL_TYPE_UNSIGNED_INT, eGL_TYPE_UNSIGNED_INT, eGL_TYPE_UNSIGNED_INT, eGL_TYPE_UNSIGNED_INT };
	uint8_t const * l_pData = &l_pBuffer->data()[0];

	for( BufferDeclaration::BufferElementDeclarationArrayConstIt l_it = m_bufferDeclaration.Begin(); l_it != m_bufferDeclaration.End() && l_bReturn; ++l_it )
	{
		switch( l_it->m_eUsage )
		{
		case eELEMENT_USAGE_POSITION:
			l_bReturn &= m_gl.EnableClientState( eGL_BUFFER_USAGE_VERTEX_ARRAY );
			l_bReturn &= m_gl.VertexPointer( s_arraySize[l_it->m_eDataType], s_arrayType[l_it->m_eDataType], m_bufferDeclaration.GetStride(), &l_pData[l_it->m_uiOffset] );
			break;

		case eELEMENT_USAGE_NORMAL:
			l_bReturn &= m_gl.EnableClientState( eGL_BUFFER_USAGE_NORMAL_ARRAY );
			l_bReturn &= m_gl.NormalPointer( s_arrayType[l_it->m_eDataType], m_bufferDeclaration.GetStride(), &l_pData[l_it->m_uiOffset] );
			break;

		case eELEMENT_USAGE_TANGENT:
			if( m_gl.HasTangentPointer() )
			{
				l_bReturn &= m_gl.EnableClientState( eGL_BUFFER_USAGE_TANGENT_ARRAY );
				l_bReturn &= m_gl.TangentPointer( s_arrayType[l_it->m_eDataType], m_bufferDeclaration.GetStride(), &l_pData[l_it->m_uiOffset] );
			}
			break;

		case eELEMENT_USAGE_BITANGENT:
			if( m_gl.HasBinormalPointer() )
			{
				l_bReturn &= m_gl.EnableClientState( eGL_BUFFER_USAGE_BINORMAL_ARRAY );
				l_bReturn &= m_gl.BinormalPointer( s_arrayType[l_it->m_eDataType], m_bufferDeclaration.GetStride(), &l_pData[l_it->m_uiOffset] );
			}
			break;

		case eELEMENT_USAGE_DIFFUSE:
			l_bReturn &= m_gl.EnableClientState( eGL_BUFFER_USAGE_COLOR_ARRAY );
			l_bReturn &= m_gl.ColorPointer( s_arraySize[l_it->m_eDataType], s_arrayType[l_it->m_eDataType], m_bufferDeclaration.GetStride(), &l_pData[l_it->m_uiOffset] );
			break;

		case eELEMENT_USAGE_TEXCOORDS0:
			l_bReturn &= m_gl.ClientActiveTexture( eGL_TEXTURE_INDEX_0 );
			l_bReturn &= m_gl.EnableClientState( eGL_BUFFER_USAGE_TEXTURE_COORD_ARRAY );
			l_bReturn &= m_gl.TexCoordPointer( s_arraySize[l_it->m_eDataType], s_arrayType[l_it->m_eDataType], m_bufferDeclaration.GetStride(), &l_pData[l_it->m_uiOffset] );
			break;

		case eELEMENT_USAGE_TEXCOORDS1:
			l_bReturn &= m_gl.ClientActiveTexture( eGL_TEXTURE_INDEX_1 );
			l_bReturn &= m_gl.EnableClientState( eGL_BUFFER_USAGE_TEXTURE_COORD_ARRAY );
			l_bReturn &= m_gl.TexCoordPointer( s_arraySize[l_it->m_eDataType], s_arrayType[l_it->m_eDataType], m_bufferDeclaration.GetStride(), &l_pData[l_it->m_uiOffset] );
			break;

		case eELEMENT_USAGE_TEXCOORDS2:
			l_bReturn &= m_gl.ClientActiveTexture( eGL_TEXTURE_INDEX_2 );
			l_bReturn &= m_gl.EnableClientState( eGL_BUFFER_USAGE_TEXTURE_COORD_ARRAY );
			l_bReturn &= m_gl.TexCoordPointer( s_arraySize[l_it->m_eDataType], s_arrayType[l_it->m_eDataType], m_bufferDeclaration.GetStride(), &l_pData[l_it->m_uiOffset] );
			break;

		case eELEMENT_USAGE_TEXCOORDS3:
			l_bReturn &= m_gl.ClientActiveTexture( eGL_TEXTURE_INDEX_3 );
			l_bReturn &= m_gl.EnableClientState( eGL_BUFFER_USAGE_TEXTURE_COORD_ARRAY );
			l_bReturn &= m_gl.TexCoordPointer( s_arraySize[l_it->m_eDataType], s_arrayType[l_it->m_eDataType], m_bufferDeclaration.GetStride(), &l_pData[l_it->m_uiOffset] );
			break;
		}
	}

	return l_bReturn;
}

void GlVertexArray :: Unbind()
{
	m_gl.DisableClientState(		eGL_BUFFER_USAGE_VERTEX_ARRAY			);
	m_gl.DisableClientState(		eGL_BUFFER_USAGE_NORMAL_ARRAY			);
	m_gl.DisableClientState(		eGL_BUFFER_USAGE_COLOR_ARRAY			);
	m_gl.DisableClientState(		eGL_BUFFER_USAGE_TANGENT_ARRAY			);

	if( m_gl.HasBinormalPointer() )
	{
		m_gl.DisableClientState( eGL_BUFFER_USAGE_BINORMAL_ARRAY			);
	}

	if( m_gl.HasTangentPointer() )
	{
		m_gl.DisableClientState( eGL_BUFFER_USAGE_TANGENT_ARRAY			);
	}

	for( int i = 0; i < 4; ++i )
	{
		m_gl.ClientActiveTexture( eGL_TEXTURE_INDEX( eGL_TEXTURE_INDEX_0 + i ) );
		m_gl.DisableClientState(	eGL_BUFFER_USAGE_TEXTURE_COORD_ARRAY	);
	}
}

//******************************************************************************

GlIndexBufferObject :: GlIndexBufferObject( OpenGl & p_gl, HardwareBufferSPtr p_pBuffer )
	:	GlBuffer< uint32_t >( p_gl, eGL_BUFFER_TARGET_ELEMENT_ARRAY, p_pBuffer )
{
}

GlIndexBufferObject :: ~GlIndexBufferObject()
{
}

bool GlIndexBufferObject :: Create()
{
	return GlBuffer< uint32_t >::DoCreate();
}

void GlIndexBufferObject :: Destroy()
{
	GlBuffer< uint32_t >::DoDestroy();
}

bool GlIndexBufferObject :: Initialise( eBUFFER_ACCESS_TYPE p_eType, eBUFFER_ACCESS_NATURE p_eNature, Castor3D::ShaderProgramBaseSPtr p_pProgram )
{
	bool l_bReturn = GlBuffer< uint32_t >::DoInitialise( p_eType, p_eNature );

// 	if( l_bReturn )
// 	{
// 		m_pBuffer->Clear();
// 	}

	return l_bReturn;
}

void GlIndexBufferObject :: Cleanup()
{
	GlBuffer< uint32_t >::DoCleanup();
}

bool GlIndexBufferObject :: Bind()
{
	HardwareBufferSPtr l_pBuffer = GetCpuBuffer();
	bool l_bReturn = l_pBuffer && l_pBuffer->IsAssigned();

	if( l_bReturn )
	{
		l_bReturn = GlBuffer< uint32_t >::DoBind();
	}

	return l_bReturn;
}

void GlIndexBufferObject :: Unbind()
{
	GlBuffer< uint32_t >::DoUnbind();
}

uint32_t * GlIndexBufferObject :: Lock( uint32_t p_uiOffset, uint32_t p_uiCount, uint32_t p_uiFlags )
{
	uint32_t * l_pReturn = NULL;
	HardwareBufferSPtr l_pBuffer = GetCpuBuffer();

	if( l_pBuffer && l_pBuffer->IsAssigned() )
	{
		l_pReturn = GlBuffer< uint32_t >::DoLock( p_uiOffset, p_uiCount, p_uiFlags );
	}

	return l_pReturn;
}

void GlIndexBufferObject :: Unlock()
{
	HardwareBufferSPtr l_pBuffer = GetCpuBuffer();

	if( l_pBuffer && l_pBuffer->IsAssigned() )
	{
		GlBuffer< uint32_t >::DoUnlock();
	}
}

//******************************************************************************

GlVertexBufferObject :: GlVertexBufferObject( OpenGl & p_gl, BufferDeclaration const & p_declaration, HardwareBufferSPtr p_pBuffer )
	:	GlBuffer< uint8_t >	( p_gl, eGL_BUFFER_TARGET_ARRAY, p_pBuffer	)
	,	m_bufferDeclaration	( p_declaration								)
{
	GlAttributeBaseSPtr l_pAttribute;
	GlRenderSystem * l_pRenderSystem = static_cast< GlRenderSystem * >( p_pBuffer->GetRenderSystem() );

	for( BufferDeclaration::BufferElementDeclarationArrayConstIt l_it = m_bufferDeclaration.Begin(); l_it != m_bufferDeclaration.End(); ++l_it )
	{
		switch( l_it->m_eUsage )
		{
		case eELEMENT_USAGE_POSITION:
			l_pAttribute = std::make_shared< GlAttribute3r >( p_gl, l_pRenderSystem, cuT( "vertex" ) );
			break;

		case eELEMENT_USAGE_NORMAL:
			l_pAttribute = std::make_shared< GlAttribute3r >( p_gl, l_pRenderSystem, cuT( "normal" ) );
			break;

		case eELEMENT_USAGE_TANGENT:
			l_pAttribute = std::make_shared< GlAttribute3r >( p_gl, l_pRenderSystem, cuT( "tangent" ) );
			break;

		case eELEMENT_USAGE_BITANGENT:
			l_pAttribute = std::make_shared< GlAttribute3r >( p_gl, l_pRenderSystem, cuT( "bitangent" ) );
			break;

		case eELEMENT_USAGE_DIFFUSE:
			l_pAttribute = std::make_shared< GlAttribute4i >( p_gl, l_pRenderSystem, cuT( "colour" ) );
			break;

		case eELEMENT_USAGE_TEXCOORDS0:
		case eELEMENT_USAGE_TEXCOORDS1:
		case eELEMENT_USAGE_TEXCOORDS2:
		case eELEMENT_USAGE_TEXCOORDS3:
			l_pAttribute = std::make_shared< GlAttribute2r >( p_gl, l_pRenderSystem, cuT( "texture" ) );
			break;

		case eELEMENT_USAGE_BONE_IDS:
			l_pAttribute = std::make_shared< GlAttribute4i >( p_gl, l_pRenderSystem, cuT( "bone_ids" ) );
			break;

		case eELEMENT_USAGE_BONE_WEIGHTS:
			l_pAttribute = std::make_shared< GlAttribute4r >( p_gl, l_pRenderSystem, cuT( "weights" ) );
			break;
		}

		l_pAttribute->SetOffset( l_it->m_uiOffset );
		l_pAttribute->SetStride( m_bufferDeclaration.GetStride() );
		m_arrayAttributes.push_back( l_pAttribute );
	}
}

GlVertexBufferObject :: ~GlVertexBufferObject()
{
}

bool GlVertexBufferObject :: Create()
{
	return GlBuffer< uint8_t >::DoCreate();
}

void GlVertexBufferObject :: Destroy()
{
	GlBuffer< uint8_t >::DoDestroy();
}

bool GlVertexBufferObject :: Initialise( eBUFFER_ACCESS_TYPE p_eType, eBUFFER_ACCESS_NATURE p_eNature, ShaderProgramBaseSPtr p_pProgram )
{
	bool l_bReturn = true;
	GlShaderProgramSPtr l_pNewProgram = std::static_pointer_cast< GlShaderProgram >( p_pProgram );
	GlShaderProgramSPtr l_pOldProgram = m_pProgram.lock();

	if( l_pOldProgram != l_pNewProgram )
	{
		std::for_each( m_arrayAttributes.begin(), m_arrayAttributes.end(), [&]( GlAttributeBaseSPtr p_pAttribute )
		{
			p_pAttribute->SetShader( l_pNewProgram );
		} );

		m_pProgram = l_pNewProgram;

		if( l_pNewProgram )
		{
			l_bReturn = DoAttributesInitialise();
		}

		if( l_bReturn )
		{
			l_bReturn = GlBuffer< uint8_t >::DoInitialise( p_eType, p_eNature );
		}
	}
	else if( !l_pOldProgram )
	{
		l_bReturn = GlBuffer< uint8_t >::DoInitialise( p_eType, p_eNature );
	}

	return l_bReturn;
}

void GlVertexBufferObject :: Cleanup()
{
	DoAttributesCleanup();
	GlBuffer< uint8_t >::DoCleanup();
}

bool GlVertexBufferObject :: Bind()
{
	HardwareBufferSPtr l_pBuffer = GetCpuBuffer();
	bool l_bReturn = l_pBuffer && l_pBuffer->IsAssigned();

	if( l_bReturn )
	{
		l_bReturn = GlBuffer< uint8_t >::DoBind();

		if( !l_pBuffer->GetRenderSystem()->UseShaders() || m_pProgram.expired() )
		{
			static const uint32_t s_arraySize[] = { 1, 2, 3, 4, 4, 1, 2, 3, 4 };
			static const uint32_t s_arrayType[] = { eGL_TYPE_REAL, eGL_TYPE_REAL, eGL_TYPE_REAL, eGL_TYPE_REAL, eGL_TYPE_UNSIGNED_BYTE, eGL_TYPE_UNSIGNED_INT, eGL_TYPE_UNSIGNED_INT, eGL_TYPE_UNSIGNED_INT, eGL_TYPE_UNSIGNED_INT };

			for( BufferDeclaration::BufferElementDeclarationArrayConstIt l_it = m_bufferDeclaration.Begin(); l_bReturn && l_it != m_bufferDeclaration.End(); ++l_it )
			{
				switch( l_it->m_eUsage)
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
					if( m_gl.HasTangentPointer() )
					{
						l_bReturn &= m_gl.EnableClientState( eGL_BUFFER_USAGE_TANGENT_ARRAY );
						l_bReturn &= m_gl.TangentPointer( s_arrayType[l_it->m_eDataType], m_bufferDeclaration.GetStride(), BUFFER_OFFSET( l_it->m_uiOffset ) );
					}
					break;

				case eELEMENT_USAGE_BITANGENT:
					if( m_gl.HasBinormalPointer() )
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

void GlVertexBufferObject :: Unbind()
{
	HardwareBufferSPtr l_pBuffer = GetCpuBuffer();

	if( l_pBuffer && l_pBuffer->IsAssigned() )
	{
		if( l_pBuffer->GetRenderSystem()->UseShaders() && ! m_pProgram.expired() )
		{
			DoAttributesUnbind();
		}
		else
		{
			m_gl.DisableClientState(		eGL_BUFFER_USAGE_VERTEX_ARRAY			);
			m_gl.DisableClientState(		eGL_BUFFER_USAGE_NORMAL_ARRAY			);
			m_gl.DisableClientState(		eGL_BUFFER_USAGE_COLOR_ARRAY			);

			if( m_gl.HasBinormalPointer() )
			{
				m_gl.DisableClientState(	eGL_BUFFER_USAGE_BINORMAL_ARRAY			);
			}

			if( m_gl.HasTangentPointer() )
			{
				m_gl.DisableClientState(	eGL_BUFFER_USAGE_TANGENT_ARRAY			);
			}

			for( int i = 0; i < 4; ++i )
			{
				m_gl.ClientActiveTexture( eGL_TEXTURE_INDEX( eGL_TEXTURE_INDEX_0 + i ) );
				m_gl.DisableClientState(	eGL_BUFFER_USAGE_TEXTURE_COORD_ARRAY	);
			}
		}

		GlBuffer< uint8_t >::DoUnbind();
	}
}

uint8_t * GlVertexBufferObject :: Lock( uint32_t p_uiOffset, uint32_t p_uiCount, uint32_t p_uiFlags )
{
	uint8_t * l_pReturn = NULL;
	HardwareBufferSPtr l_pBuffer = GetCpuBuffer();

	if( l_pBuffer && l_pBuffer->IsAssigned() )
	{
		l_pReturn = GlBuffer< uint8_t >::DoLock( p_uiOffset, p_uiCount, p_uiFlags );
	}

	return l_pReturn;
}

void GlVertexBufferObject :: Unlock()
{
	HardwareBufferSPtr l_pBuffer = GetCpuBuffer();

	if( l_pBuffer && l_pBuffer->IsAssigned() )
	{
		GlBuffer< uint8_t >::DoUnlock();
	}
}

void GlVertexBufferObject :: DoAttributesCleanup()
{
	for( GlAttributePtrArrayIt l_it = m_arrayAttributes.begin(); l_it != m_arrayAttributes.end(); ++l_it )
	{
		(*l_it)->Cleanup();
	}
}

bool GlVertexBufferObject :: DoAttributesInitialise()
{
	m_uiValid = 0;

	for( GlAttributePtrArrayIt l_it = m_arrayAttributes.begin(); l_it != m_arrayAttributes.end(); ++l_it )
	{
		m_uiValid += ((*l_it)->Initialise() ? 1 : 0);
	}

	return m_uiValid > 0;
}

bool GlVertexBufferObject :: DoAttributesBind()
{
	bool l_bReturn = true;

	for( GlAttributePtrArrayIt l_it = m_arrayAttributes.begin(); l_it != m_arrayAttributes.end() && l_bReturn; ++l_it )
	{
		if( (*l_it)->GetLocation() != eGL_INVALID_INDEX )
		{
			l_bReturn = (*l_it)->Bind( false );
		}
	}

	return l_bReturn;
}

void GlVertexBufferObject :: DoAttributesUnbind()
{
	for( GlAttributePtrArrayIt l_it = m_arrayAttributes.begin(); l_it != m_arrayAttributes.end(); ++l_it )
	{
		if( (*l_it)->GetLocation() != eGL_INVALID_INDEX )
		{
			(*l_it)->Unbind();
		}
	}
}

//******************************************************************************

Gl3VertexBufferObject :: Gl3VertexBufferObject( OpenGl & p_gl, BufferDeclaration const & p_declaration, HardwareBufferSPtr p_pBuffer )
	:	GlVertexBufferObject( p_gl, p_declaration, p_pBuffer )
{
}

Gl3VertexBufferObject :: ~Gl3VertexBufferObject()
{
}

bool Gl3VertexBufferObject :: Bind()
{
	HardwareBufferSPtr l_pBuffer = GetCpuBuffer();
	bool l_bReturn = l_pBuffer && l_pBuffer->IsAssigned();

	if( l_bReturn )
	{
		l_bReturn = DoBind();
	}

	if( l_bReturn )
	{
		l_bReturn = DoAttributesBind();
	}

	return l_bReturn;
}

void Gl3VertexBufferObject :: Unbind()
{
	HardwareBufferSPtr l_pBuffer = GetCpuBuffer();

	if( l_pBuffer && l_pBuffer->IsAssigned() )
	{
		DoAttributesUnbind();
	}
}

//******************************************************************************

GlMatrixBufferObject :: GlMatrixBufferObject( OpenGl & p_gl, HardwareBufferSPtr p_pBuffer )
	:	GlBuffer< real >( p_gl, eGL_BUFFER_TARGET_ARRAY, p_pBuffer )
{
	GlRenderSystem * l_pRenderSystem = static_cast< GlRenderSystem * >( p_pBuffer->GetRenderSystem() );
	m_pAttribute = std::make_shared< GlAttribute< real, 16 > >( p_gl, l_pRenderSystem, cuT( "transform" ) );
}

GlMatrixBufferObject :: ~GlMatrixBufferObject()
{
}

bool GlMatrixBufferObject :: Create()
{
	return GlBuffer< real >::DoCreate();
}

void GlMatrixBufferObject :: Destroy()
{
	GlBuffer< real >::DoDestroy();
}

bool GlMatrixBufferObject :: Initialise( eBUFFER_ACCESS_TYPE p_eType, eBUFFER_ACCESS_NATURE p_eNature, Castor3D::ShaderProgramBaseSPtr p_pProgram )
{
	bool l_bReturn = true;
	GlShaderProgramSPtr l_pNewProgram = std::static_pointer_cast< GlShaderProgram >( p_pProgram );
	GlShaderProgramSPtr l_pOldProgram = m_pProgram.lock();

	if( l_pOldProgram != l_pNewProgram )
	{
		m_pAttribute->SetShader( l_pNewProgram );
		m_pProgram = l_pNewProgram;

		if( l_pNewProgram )
		{
			l_bReturn = DoAttributeInitialise();
		}

		if( l_bReturn )
		{
			l_bReturn = GlBuffer< real >::DoInitialise( p_eType, p_eNature );
		}
	}
	else if( !l_pOldProgram )
	{
		l_bReturn = GlBuffer< real >::DoInitialise( p_eType, p_eNature );
	}

	return l_bReturn;
}

void GlMatrixBufferObject :: Cleanup()
{
	DoAttributeCleanup();
	GlBuffer< real >::DoCleanup();
}

bool GlMatrixBufferObject :: Bind( uint32_t p_uiCount )
{
	HardwareBufferSPtr l_pBuffer = GetCpuBuffer();
	bool l_bReturn = l_pBuffer && l_pBuffer->IsAssigned();

	if( l_bReturn )
	{
		l_bReturn = GlBuffer< real >::DoBind();
	}

	if( l_bReturn )
	{
		l_bReturn = DoAttributeBind( p_uiCount > 1 );
	}

	return l_bReturn;
}

void GlMatrixBufferObject :: Unbind()
{
	HardwareBufferSPtr l_pBuffer = GetCpuBuffer();

	if( l_pBuffer && l_pBuffer->IsAssigned() )
	{
		DoAttributeUnbind();
		GlBuffer< real >::DoUnbind();
	}
}

real * GlMatrixBufferObject :: Lock( uint32_t p_uiOffset, uint32_t p_uiCount, uint32_t p_uiFlags )
{
	real * l_pReturn = NULL;
	HardwareBufferSPtr l_pBuffer = GetCpuBuffer();

	if( l_pBuffer && l_pBuffer->IsAssigned() )
	{
		l_pReturn = GlBuffer< real >::DoLock( p_uiOffset, p_uiCount, p_uiFlags );
	}

	return l_pReturn;
}

void GlMatrixBufferObject :: Unlock()
{
	HardwareBufferSPtr l_pBuffer = GetCpuBuffer();

	if( l_pBuffer && l_pBuffer->IsAssigned() )
	{
		GlBuffer< real >::DoUnlock();
	}
}

void GlMatrixBufferObject :: DoAttributeCleanup()
{
	m_pAttribute->Cleanup();
}

bool GlMatrixBufferObject :: DoAttributeInitialise()
{
	bool l_bReturn = m_pAttribute->Initialise();
	return l_bReturn;
}

bool GlMatrixBufferObject :: DoAttributeBind( bool p_bInstanced )
{
	bool l_bReturn = true;
	uint32_t l_uiLocation = m_pAttribute->GetLocation();

	if( l_uiLocation != eGL_INVALID_INDEX )
	{
		for( int i = 0; i < 4 && l_bReturn; ++i )
		{
			l_bReturn = m_gl.EnableVertexAttribArray( l_uiLocation + i );
			l_bReturn &= m_gl.VertexAttribPointer( l_uiLocation + i, 4, eGL_TYPE_REAL, false, 16 * sizeof( real ), BUFFER_OFFSET( i * 4 * sizeof( real ) ) );
			l_bReturn &= m_gl.VertexAttribDivisor( l_uiLocation + i, p_bInstanced ? 1 : 0 );
		}
	}

	return l_bReturn;
}

void GlMatrixBufferObject :: DoAttributeUnbind()
{
	uint32_t l_uiLocation = m_pAttribute->GetLocation();

	if( l_uiLocation != eGL_INVALID_INDEX )
	{
		for( int i = 0; i < 4; ++i )
		{
			m_gl.DisableVertexAttribArray( l_uiLocation + i );
		}
	}
}

//******************************************************************************

GlGeometryBuffers :: GlGeometryBuffers( OpenGl & p_gl, VertexBufferSPtr p_pVertexBuffer, IndexBufferSPtr p_pIndexBuffer, MatrixBufferSPtr p_pMatrixBuffer )
	:	GeometryBuffers	( p_pVertexBuffer, p_pIndexBuffer, p_pMatrixBuffer	)
	,	m_uiIndex		( eGL_INVALID_INDEX									)
	,	m_gl			( p_gl												)
{
}

GlGeometryBuffers :: ~GlGeometryBuffers()
{
}

bool GlGeometryBuffers :: Draw( eTOPOLOGY p_ePrimitiveType, ShaderProgramBaseSPtr p_pProgram, uint32_t p_uiSize, uint32_t p_uiIndex )
{
	eGL_PRIMITIVE l_eMode = m_gl.Get( p_ePrimitiveType );

	if( p_pProgram->HasProgram( eSHADER_TYPE_HULL ) )
	{
		l_eMode = eGL_PRIMITIVE_PATCHES;
		m_gl.PatchParameter( eGL_PATCH_PARAMETER_VERTICES, 3 );
	}

	if( Bind() )
	{
		if( m_pIndexBuffer )
		{
			m_gl.DrawElements( l_eMode, int( p_uiSize ), eGL_TYPE_UNSIGNED_INT, BUFFER_OFFSET( p_uiIndex ) );
		}
		else
		{
			m_gl.DrawArrays( l_eMode, int( p_uiIndex ), int( p_uiSize ) );
		}

		Unbind();
	}

	return true;
}

bool GlGeometryBuffers :: DrawInstanced( eTOPOLOGY p_eTopology, ShaderProgramBaseSPtr p_pProgram, uint32_t p_uiSize, uint32_t p_uiIndex, uint32_t p_uiCount )
{
	eGL_PRIMITIVE l_eMode = m_gl.Get( p_eTopology );

	if( p_pProgram->HasProgram( eSHADER_TYPE_HULL ) )
	{
		l_eMode = eGL_PRIMITIVE_PATCHES;
		m_gl.PatchParameter( eGL_PATCH_PARAMETER_VERTICES, 3 );
	}

	if( m_pMatrixBuffer )
	{
		m_gl.BindBuffer( eGL_BUFFER_TARGET_ARRAY, std::static_pointer_cast< GlMatrixBufferObject >( m_pMatrixBuffer->GetGpuBuffer() )->GetGlIndex() );
		m_pMatrixBuffer->GetGpuBuffer()->Fill( m_pMatrixBuffer->data(), m_pMatrixBuffer->GetSize(), eBUFFER_ACCESS_TYPE_DYNAMIC, eBUFFER_ACCESS_NATURE_DRAW );
	}

	if( Bind() )
	{
		if( m_pIndexBuffer )
		{
			m_gl.DrawElementsInstanced( l_eMode, int( p_uiSize ), eGL_TYPE_UNSIGNED_INT, BUFFER_OFFSET( p_uiIndex ), int( p_uiCount ) );
		}
		else
		{
			m_gl.DrawArraysInstanced( l_eMode, int( p_uiIndex ), int( p_uiSize ), int( p_uiCount ) );
		}

		Unbind();
	}

	return true;
}

bool GlGeometryBuffers :: Initialise()
{
	bool l_bReturn = false;
	
#if !C3DGL_LIMIT_TO_2_1
	if( m_gl.HasVao() )
	{
		m_pfnBind = PFnBind( [&]()
		{
			bool l_bReturn = m_uiIndex != eGL_INVALID_INDEX;

			if( l_bReturn )
			{
				l_bReturn = m_gl.BindVertexArray( m_uiIndex );
			}

			return l_bReturn;
		} );

		m_pfnUnbind = PFnUnbind( [&]()
		{
			m_gl.BindVertexArray( 0 );
		} );

		if( m_uiIndex == eGL_INVALID_INDEX )
		{
			m_gl.GenVertexArrays( 1, &m_uiIndex );
		}

		if( m_uiIndex != eGL_INVALID_INDEX )
		{
			if( m_pVertexBuffer )
			{
				l_bReturn = m_gl.BindVertexArray( m_uiIndex );

				if( l_bReturn )
				{
					m_pVertexBuffer->Bind();

					if( m_pIndexBuffer )
					{
						m_pIndexBuffer->Bind();
					}

					if( m_pMatrixBuffer )
					{
						m_pMatrixBuffer->Bind( 2 );
					}

					m_gl.BindVertexArray( 0 );
				}
			}
		}
	}
	else
#endif
	{
		m_pfnBind = PFnBind( [&]()
		{
			return GeometryBuffers::Bind();
		} );

		m_pfnUnbind = PFnUnbind( [&]()
		{
			GeometryBuffers::Unbind();
		} );

		m_uiIndex = 0;
		l_bReturn = true;
	}

	return l_bReturn;
}

void GlGeometryBuffers :: Cleanup()
{
#if !C3DGL_LIMIT_TO_2_1
	if( m_gl.HasVao() )
	{
		if( m_uiIndex != eGL_INVALID_INDEX )
		{
			m_gl.DeleteVertexArrays( 1, &m_uiIndex );
		}
	}
#endif
	m_uiIndex = eGL_INVALID_INDEX;
}

bool GlGeometryBuffers :: Bind()
{
	return m_pfnBind();
}

void GlGeometryBuffers :: Unbind()
{
	m_pfnUnbind();
}

//******************************************************************************

GlTextureBufferObject :: GlTextureBufferObject( OpenGl & p_gl, HardwareBufferSPtr p_pBuffer )
	:	GlBuffer< uint8_t >		( p_gl, eGL_BUFFER_TARGET_TEXTURE, p_pBuffer	)
{
}

GlTextureBufferObject :: ~GlTextureBufferObject()
{
}

bool GlTextureBufferObject :: Create()
{
	return GlBuffer< uint8_t >::DoCreate();
}

void GlTextureBufferObject :: Destroy()
{
	return GlBuffer< uint8_t >::DoDestroy();
}

bool GlTextureBufferObject :: Initialise( Castor3D::eBUFFER_ACCESS_TYPE p_eType, eBUFFER_ACCESS_NATURE p_eNature, ShaderProgramBaseSPtr p_pProgram )
{
	bool l_bReturn = GlBuffer< uint8_t >::DoInitialise( p_eType, p_eNature );

// 	if( l_bReturn )
// 	{
// 		m_pBuffer->Clear();
// 	}

	return l_bReturn;
}

void GlTextureBufferObject :: Cleanup()
{
	GlBuffer< uint8_t >::DoCleanup();
}

bool GlTextureBufferObject :: Bind()
{
	HardwareBufferSPtr l_pBuffer = GetCpuBuffer();
	bool l_bReturn = l_pBuffer && l_pBuffer->IsAssigned();

	if( l_bReturn )
	{
		l_bReturn = GlBuffer< uint8_t >::DoBind();
	}

	return l_bReturn;
}

void GlTextureBufferObject :: Unbind()
{
	GlBuffer< uint8_t >::DoUnbind();
}

uint8_t * GlTextureBufferObject :: Lock( uint32_t p_uiOffset, uint32_t p_uiCount, uint32_t p_uiFlags )
{
	uint8_t * l_pReturn = NULL;
	HardwareBufferSPtr l_pBuffer = GetCpuBuffer();

	if( l_pBuffer && l_pBuffer->IsAssigned() )
	{
		l_pReturn = GlBuffer< uint8_t >::DoLock( p_uiOffset, p_uiCount, p_uiFlags );
	}

	return l_pReturn;
}

void GlTextureBufferObject :: Unlock()
{
	HardwareBufferSPtr l_pBuffer = GetCpuBuffer();

	if( l_pBuffer && l_pBuffer->IsAssigned() )
	{
		GlBuffer< uint8_t >::DoUnlock();
	}
}

//******************************************************************************
