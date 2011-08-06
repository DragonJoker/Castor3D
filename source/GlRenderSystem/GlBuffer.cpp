#include "GlRenderSystem/PrecompiledHeader.hpp"

#include "GlRenderSystem/GlBuffer.hpp"
#include "GlRenderSystem/OpenGl.hpp"

using namespace Castor3D;

//*************************************************************************************************

GlBufferBase :: GlBufferBase( GLenum p_target)
	:	m_uiGlIndex	( GL_INVALID_INDEX)
	,	m_target	( p_target)
{
}

GlBufferBase :: ~GlBufferBase()
{
}

bool GlBufferBase :: Create()
{
	return OpenGl::GenBuffers( 1, & m_uiGlIndex);
}

bool GlBufferBase :: Cleanup()
{
	if (m_uiGlIndex != GL_INVALID_INDEX)
	{
		OpenGl::DeleteBuffers( 1, & m_uiGlIndex);
		m_uiGlIndex = GL_INVALID_INDEX;
	}

	return true;
}

bool GlBufferBase :: Initialise( void * p_pBuffer, GLsizeiptr p_iSize, eBUFFER_MODE p_eMode)
{
	bool l_bResult = Create();
	l_bResult &= Bind();
	l_bResult &= Fill( p_pBuffer, p_iSize, GlEnum::GetBufferFlags( p_eMode));
	l_bResult &= Unbind();
	return l_bResult;
}

bool GlBufferBase :: Bind()
{
	return OpenGl::BindBuffer( m_target, m_uiGlIndex);
}

bool GlBufferBase :: Unbind()
{
	return OpenGl::BindBuffer( m_target, 0);
}

bool GlBufferBase :: Fill( void * p_pBuffer, GLsizeiptr p_iSize, GLenum p_mode)
{
	return OpenGl::BufferData( m_target, p_iSize, p_pBuffer, p_mode);
}

void * GlBufferBase :: Lock( size_t p_uiOffset, size_t p_uiSize, size_t p_uiFlags)
{
	void * l_pReturn = NULL;

	if (m_uiGlIndex != GL_INVALID_INDEX)
	{
		unsigned char * l_pBuffer = reinterpret_cast<unsigned char *>( OpenGl::MapBuffer( m_target, GlEnum::GetLockFlags( p_uiFlags)));
		l_pReturn = (l_pBuffer + p_uiOffset);
	}

	return l_pReturn;
}

void * GlBufferBase :: Lock( GLenum p_access)
{
	void * l_pReturn = NULL;

	if (m_uiGlIndex != GL_INVALID_INDEX)
	{
		l_pReturn = OpenGl::MapBuffer( m_target, p_access);
	}

	return l_pReturn;
}

bool GlBufferBase :: Unlock()
{
	bool l_bReturn = m_uiGlIndex != GL_INVALID_INDEX;
	l_bReturn &= OpenGl::UnmapBuffer( m_target);
	return l_bReturn;
}

//******************************************************************************

GlAttributeBase :: GlAttributeBase( String const & p_strAttribName, GLenum p_glType, GLint p_iCount)
	:	m_strAttribName		( p_strAttribName)
	,	m_uiAttribLocation	( GL_INVALID_INDEX)
	,	m_iCount			( p_iCount)
	,	m_uiOffset			( 0)
	,	m_iStride			( 0)
	,	m_glType			( p_glType)
{
}

GlAttributeBase :: ~GlAttributeBase()
{
}

void GlAttributeBase :: SetShader( ShaderProgramPtr p_pProgram)
{
	m_pProgram = p_pProgram;
}

void GlAttributeBase :: Cleanup()
{
	m_uiAttribLocation = GL_INVALID_INDEX;
	m_pProgram.reset();
}

bool GlAttributeBase :: Initialise()
{
	bool l_bReturn = ! RenderSystem::UseShaders();

	if ( ! m_pProgram.expired() && m_pProgram.lock()->IsLinked() && ! l_bReturn)
	{
		m_uiAttribLocation = m_pProgram.lock()->GetAttributeLocation( m_strAttribName.char_str());

		if (m_uiAttribLocation != GL_INVALID_INDEX)
		{
			l_bReturn = true;
		}
	}

	return l_bReturn;
}

void GlAttributeBase :: Activate( GLboolean p_bNormalised)
{
	OpenGl::EnableVertexAttribArray( m_uiAttribLocation);
	OpenGl::VertexAttribPointer( m_uiAttribLocation, m_iCount, m_glType, p_bNormalised, m_iStride, BUFFER_OFFSET( m_uiOffset));
}

void GlAttributeBase :: Deactivate()
{
	OpenGl::DisableVertexAttribArray( m_uiAttribLocation);
}

//******************************************************************************

GlIndexArray :: GlIndexArray()
	:	IndexBuffer()
{
}

GlIndexArray :: ~GlIndexArray()
{
}

void GlIndexArray :: Cleanup()
{
}

void GlIndexArray :: Activate()
{
}

void GlIndexArray :: Deactivate()
{
}

//******************************************************************************

GlVertexArray :: GlVertexArray( const BufferElementDeclaration * p_pElements, size_t p_uiCount)
	:	VertexBuffer( p_pElements, p_uiCount)
	,	m_declaration	( VertexBuffer::m_bufferDeclaration)
{
}

GlVertexArray :: ~GlVertexArray()
{
}

void GlVertexArray :: Cleanup()
{
}

void GlVertexArray :: Activate()
{
	static const unsigned int s_arraySize[] = {1, 2, 3, 4, 4};
	static const unsigned int s_arrayType[] = {GL_FLOAT, GL_FLOAT, GL_FLOAT, GL_FLOAT, GL_UNSIGNED_BYTE};

	for (BufferDeclaration::const_iterator l_it = m_bufferDeclaration.Begin() ; l_it != m_bufferDeclaration.End() ; ++l_it)
	{
		switch (( * l_it).m_eUsage)
		{
		case eELEMENT_USAGE_POSITION:
			OpenGl::EnableClientState( GL_VERTEX_ARRAY);
			OpenGl::VertexPointer( s_arraySize[( * l_it).m_eDataType], s_arrayType[( * l_it).m_eDataType], m_bufferDeclaration.GetStride(), & m_buffer[( * l_it).m_uiOffset]);
			break;

		case eELEMENT_USAGE_NORMAL:
			OpenGl::EnableClientState( GL_NORMAL_ARRAY);
			OpenGl::NormalPointer( s_arrayType[( * l_it).m_eDataType], m_bufferDeclaration.GetStride(), & m_buffer[( * l_it).m_uiOffset]);
			break;

		case eELEMENT_USAGE_TANGENT:
			OpenGl::EnableClientState( GL_TANGENT_ARRAY_EXT);
			OpenGl::TangentPointer( s_arrayType[( * l_it).m_eDataType], m_bufferDeclaration.GetStride(), & m_buffer[( * l_it).m_uiOffset]);
			break;

		case eELEMENT_USAGE_DIFFUSE:
			OpenGl::EnableClientState( GL_COLOR_ARRAY);
			OpenGl::ColorPointer( s_arraySize[( * l_it).m_eDataType], s_arrayType[( * l_it).m_eDataType], m_bufferDeclaration.GetStride(), & m_buffer[( * l_it).m_uiOffset]);
			break;

		case eELEMENT_USAGE_TEXCOORDS0:
			OpenGl::ClientActiveTexture( GL_TEXTURE0);
			OpenGl::EnableClientState( GL_TEXTURE_COORD_ARRAY);
			OpenGl::TexCoordPointer( s_arraySize[( * l_it).m_eDataType], s_arrayType[( * l_it).m_eDataType], m_bufferDeclaration.GetStride(), & m_buffer[( * l_it).m_uiOffset]);
			break;

		case eELEMENT_USAGE_TEXCOORDS1:
			OpenGl::ClientActiveTexture( GL_TEXTURE1);
			OpenGl::EnableClientState( GL_TEXTURE_COORD_ARRAY);
			OpenGl::TexCoordPointer( s_arraySize[( * l_it).m_eDataType], s_arrayType[( * l_it).m_eDataType], m_bufferDeclaration.GetStride(), & m_buffer[( * l_it).m_uiOffset]);
			break;

		case eELEMENT_USAGE_TEXCOORDS2:
			OpenGl::ClientActiveTexture( GL_TEXTURE2);
			OpenGl::EnableClientState( GL_TEXTURE_COORD_ARRAY);
			OpenGl::TexCoordPointer( s_arraySize[( * l_it).m_eDataType], s_arrayType[( * l_it).m_eDataType], m_bufferDeclaration.GetStride(), & m_buffer[( * l_it).m_uiOffset]);
			break;

		case eELEMENT_USAGE_TEXCOORDS3:
			OpenGl::ClientActiveTexture( GL_TEXTURE3);
			OpenGl::EnableClientState( GL_TEXTURE_COORD_ARRAY);
			OpenGl::TexCoordPointer( s_arraySize[( * l_it).m_eDataType], s_arrayType[( * l_it).m_eDataType], m_bufferDeclaration.GetStride(), & m_buffer[( * l_it).m_uiOffset]);
			break;
		}
	}
}

void GlVertexArray :: Deactivate()
{
	OpenGl::DisableClientState( GL_VERTEX_ARRAY);
	OpenGl::DisableClientState( GL_NORMAL_ARRAY);
	OpenGl::DisableClientState( GL_COLOR_ARRAY);
	OpenGl::DisableClientState( GL_TANGENT_ARRAY_EXT);

	for (int i = 0; i < 4; ++i)
	{
		OpenGl::ClientActiveTexture( GL_TEXTURE0 + i);
		OpenGl::DisableClientState( GL_TEXTURE_COORD_ARRAY);
	}
}

//******************************************************************************

GlIndexBufferObject :: GlIndexBufferObject()
	:	GlBuffer<unsigned int>( IndexBuffer::m_filled, IndexBuffer::m_buffer, GL_ELEMENT_ARRAY_BUFFER)
{
}

GlIndexBufferObject :: ~GlIndexBufferObject()
{
}

void GlIndexBufferObject :: Cleanup()
{
	CASTOR_TRACK;
	Buffer3D<unsigned int>::clear();
	BufferObjectCleanup( m_bAssigned, this);
}

void GlIndexBufferObject :: Initialise( eBUFFER_MODE p_eMode)
{
	CASTOR_TRACK;
	m_bAssigned = BufferObjectInitialise( p_eMode, this);
}

void GlIndexBufferObject :: Activate()
{
	CASTOR_TRACK;
	if (m_bAssigned)
	{
		BufferObjectBind();
	}
}

void GlIndexBufferObject :: Deactivate()
{
	CASTOR_TRACK;
}

void * GlIndexBufferObject :: Lock( size_t p_uiOffset, size_t p_uiSize, size_t p_uiFlags)
{
	void * l_pReturn = NULL;

	if (m_bAssigned)
	{
		l_pReturn = BufferObjectLock( p_uiOffset, p_uiSize, p_uiFlags);
	}

	return l_pReturn;
}

void GlIndexBufferObject :: Unlock()
{
	if (m_bAssigned)
	{
		BufferObjectUnlock();
	}
}

void GlIndexBufferObject :: CleanupBuffer()
{
	CASTOR_TRACK;
	BufferObjectCleanup( m_bAssigned, this);
}

//******************************************************************************

GlVertexBufferObject :: GlVertexBufferObject( const BufferElementDeclaration * p_pElements, size_t p_uiNbElements)
	:	VertexBuffer( p_pElements, p_uiNbElements)
	,	GlBuffer<real>( VertexBuffer::m_filled, VertexBuffer::m_buffer, GL_ARRAY_BUFFER)
	,	m_declaration	( VertexBuffer::m_bufferDeclaration)
	,	m_uiValid		( 0)
{
	GlAttributePtr l_pAttribute;

	for (BufferDeclaration::const_iterator l_it = m_declaration.Begin() ; l_it != m_declaration.End() ; ++l_it)
	{
		switch (( * l_it).m_eUsage)
		{
		case eELEMENT_USAGE_POSITION:
			l_pAttribute.reset( new GlAttribute3r( "vertex"));
			break;

		case eELEMENT_USAGE_NORMAL:
			l_pAttribute.reset( new GlAttribute3r( "normal"));
			break;

		case eELEMENT_USAGE_TANGENT:
			l_pAttribute.reset( new GlAttribute3r( "tangent"));
			break;

		case eELEMENT_USAGE_TEXCOORDS0:
		case eELEMENT_USAGE_TEXCOORDS1:
		case eELEMENT_USAGE_TEXCOORDS2:
		case eELEMENT_USAGE_TEXCOORDS3:
			l_pAttribute.reset( new GlAttribute2r( "texture"));
			break;
		}

		l_pAttribute->SetOffset( ( * l_it).m_uiOffset);
		l_pAttribute->SetStride( m_declaration.GetStride());
		m_arrayAttributes.push_back( l_pAttribute);
	}
}

GlVertexBufferObject :: ~GlVertexBufferObject()
{
}

void GlVertexBufferObject :: Cleanup()
{
	CASTOR_TRACK;
	Buffer3D<real>::clear();
	AttributeCleanup();
	BufferObjectCleanup( m_bAssigned, this);
}

void GlVertexBufferObject :: Initialise( eBUFFER_MODE p_eMode)
{
	CASTOR_TRACK;
	if (m_pProgram.expired() || AttributeInitialise())
	{
		m_bAssigned = BufferObjectInitialise( p_eMode, this);
	}
}

void GlVertexBufferObject :: Activate()
{
	CASTOR_TRACK;
	if (m_bAssigned)
	{
		bool l_bResult = BufferObjectBind();

		if ( ! RenderSystem::UseShaders() || m_pProgram.expired())
		{
			static const unsigned int s_arraySize[] = {1, 2, 3, 4, 4};
			static const unsigned int s_arrayType[] = {GL_FLOAT, GL_FLOAT, GL_FLOAT, GL_FLOAT, GL_UNSIGNED_BYTE};

			for (BufferDeclaration::const_iterator l_it = m_declaration.Begin() ; l_bResult && l_it != m_declaration.End() ; ++l_it)
			{
				switch (( * l_it).m_eUsage)
				{
				case eELEMENT_USAGE_POSITION:
					l_bResult &= OpenGl::EnableClientState( GL_VERTEX_ARRAY);
					l_bResult &= OpenGl::VertexPointer( s_arraySize[( * l_it).m_eDataType], s_arrayType[( * l_it).m_eDataType], m_declaration.GetStride(), BUFFER_OFFSET( ( * l_it).m_uiOffset));
					break;

				case eELEMENT_USAGE_NORMAL:
					l_bResult &= OpenGl::EnableClientState( GL_NORMAL_ARRAY);
					l_bResult &= OpenGl::NormalPointer( s_arrayType[( * l_it).m_eDataType], m_declaration.GetStride(), BUFFER_OFFSET( ( * l_it).m_uiOffset));
					break;

				case eELEMENT_USAGE_TANGENT:
					if (OpenGl::HasTangentPointer())
					{
						l_bResult &= OpenGl::EnableClientState( GL_TANGENT_ARRAY_EXT);
						l_bResult &= OpenGl::TangentPointer( s_arrayType[( * l_it).m_eDataType], m_declaration.GetStride(), BUFFER_OFFSET( ( * l_it).m_uiOffset));
					}
					break;

				case eELEMENT_USAGE_DIFFUSE:
					l_bResult &= OpenGl::EnableClientState( GL_COLOR_ARRAY);
					l_bResult &= OpenGl::ColorPointer( s_arraySize[( * l_it).m_eDataType], s_arrayType[( * l_it).m_eDataType], m_declaration.GetStride(), BUFFER_OFFSET( ( * l_it).m_uiOffset));
					break;

				case eELEMENT_USAGE_TEXCOORDS0:
					l_bResult &= OpenGl::ClientActiveTexture( GL_TEXTURE0);
					l_bResult &= OpenGl::EnableClientState( GL_TEXTURE_COORD_ARRAY);
					l_bResult &= OpenGl::TexCoordPointer( s_arraySize[( * l_it).m_eDataType], s_arrayType[( * l_it).m_eDataType], m_declaration.GetStride(), BUFFER_OFFSET( ( * l_it).m_uiOffset));
					break;

				case eELEMENT_USAGE_TEXCOORDS1:
					l_bResult &= OpenGl::ClientActiveTexture( GL_TEXTURE1);
					l_bResult &= OpenGl::EnableClientState( GL_TEXTURE_COORD_ARRAY);
					l_bResult &= OpenGl::TexCoordPointer( s_arraySize[( * l_it).m_eDataType], s_arrayType[( * l_it).m_eDataType], m_declaration.GetStride(), BUFFER_OFFSET( ( * l_it).m_uiOffset));
					break;

				case eELEMENT_USAGE_TEXCOORDS2:
					l_bResult &= OpenGl::ClientActiveTexture( GL_TEXTURE2);
					l_bResult &= OpenGl::EnableClientState( GL_TEXTURE_COORD_ARRAY);
					l_bResult &= OpenGl::TexCoordPointer( s_arraySize[( * l_it).m_eDataType], s_arrayType[( * l_it).m_eDataType], m_declaration.GetStride(), BUFFER_OFFSET( ( * l_it).m_uiOffset));
					break;

				case eELEMENT_USAGE_TEXCOORDS3:
					l_bResult &= OpenGl::ClientActiveTexture( GL_TEXTURE3);
					l_bResult &= OpenGl::EnableClientState( GL_TEXTURE_COORD_ARRAY);
					l_bResult &= OpenGl::TexCoordPointer( s_arraySize[( * l_it).m_eDataType], s_arrayType[( * l_it).m_eDataType], m_declaration.GetStride(), BUFFER_OFFSET( ( * l_it).m_uiOffset));
					break;
				}
			}
		}
		else
		{
			AttributeActivate();
		}
	}
}

void GlVertexBufferObject :: Deactivate()
{
	CASTOR_TRACK;
	if (m_bAssigned)
	{
		if (RenderSystem::UseShaders() && ! m_pProgram.expired())
		{
			AttributeDeactivate();
		}
		else
		{
			OpenGl::DisableClientState( GL_VERTEX_ARRAY);
			OpenGl::DisableClientState( GL_NORMAL_ARRAY);
			OpenGl::DisableClientState( GL_COLOR_ARRAY);

			for (int i = 0; i < 4; ++i)
			{
				OpenGl::ClientActiveTexture( GL_TEXTURE0 + i);
				OpenGl::DisableClientState( GL_TEXTURE_COORD_ARRAY);
			}
		}
	}
}

void * GlVertexBufferObject :: Lock( size_t p_uiOffset, size_t p_uiSize, size_t p_uiFlags)
{
	void * l_pReturn = NULL;

	if (m_bAssigned)
	{
		BufferObjectLock( p_uiOffset, p_uiSize, p_uiFlags);
	}

	return l_pReturn;
}

void GlVertexBufferObject :: Unlock()
{
	if (m_bAssigned)
	{
		BufferObjectUnlock();
	}
}

void GlVertexBufferObject :: SetShaderProgram( ShaderProgramPtr p_pProgram)
{
	GlShaderProgramPtr l_pProgram = static_pointer_cast<GlShaderProgram>( p_pProgram);

	if ((this->m_pProgram.expired() && l_pProgram) || ( ! l_pProgram && ! this->m_pProgram.expired()) || this->m_pProgram.lock() != l_pProgram)
	{
		BufferObjectCleanup( m_bAssigned, this);
		AttributeCleanup();

		for (size_t i = 0 ; i < m_arrayAttributes.size() ; i++)
		{
			m_arrayAttributes[i]->SetShader( l_pProgram);
		}

		m_pProgram = l_pProgram;
		Initialise( eBUFFER_MODE_DYNAMIC);
	}
}

void GlVertexBufferObject :: CleanupBuffer()
{
	CASTOR_TRACK;
	BufferObjectCleanup( m_bAssigned, this);
	AttributeCleanup();
}

void GlVertexBufferObject :: AttributeCleanup()
{
	CASTOR_TRACK;
	for (size_t i = 0 ; i < m_arrayAttributes.size() ; i++)
	{
		m_arrayAttributes[i]->Cleanup();
	}
}

bool GlVertexBufferObject :: AttributeInitialise()
{
	CASTOR_TRACK;
	m_uiValid = 0;

	for (size_t i = 0 ; i < m_arrayAttributes.size() ; i++)
	{
		m_uiValid += (m_arrayAttributes[i]->Initialise() ? 1 : 0);
	}

	return m_uiValid > 0;
}

void GlVertexBufferObject :: AttributeActivate()
{
	CASTOR_TRACK;
	for (size_t i = 0 ; i < m_arrayAttributes.size() ; i++)
	{
		if (m_arrayAttributes[i]->GetLocation() != GL_INVALID_INDEX)
		{
			m_arrayAttributes[i]->Activate( false);
		}
	}
}

void GlVertexBufferObject :: AttributeDeactivate()
{
	CASTOR_TRACK;
	for (size_t i = 0 ; i < m_arrayAttributes.size() ; i++)
	{
		if (m_arrayAttributes[i]->GetLocation() != GL_INVALID_INDEX)
		{
			m_arrayAttributes[i]->Deactivate();
		}
	}
}

//******************************************************************************

Gl3VertexBufferObject :: Gl3VertexBufferObject( const BufferElementDeclaration * p_pElements, size_t p_uiNbElements)
	:	GlVertexBufferObject( p_pElements, p_uiNbElements)
{
}

Gl3VertexBufferObject :: ~Gl3VertexBufferObject()
{
}

void Gl3VertexBufferObject :: Activate()
{
	CASTOR_TRACK;
	if (m_bAssigned)
	{
		if (BufferObjectBind())
		{
			AttributeActivate();
		}
	}
}

void Gl3VertexBufferObject :: Deactivate()
{
	CASTOR_TRACK;
	if (m_bAssigned)
	{
		AttributeDeactivate();
	}
}

//*************************************************************************************************

GlPixelBuffer :: GlPixelBuffer( unsigned char * p_pPixels, size_t p_uiPixelsSize, GLenum p_packMode, GLenum p_drawMode)
	:	GlBufferBase	( p_packMode)
	,	m_uiPixelsSize	( p_uiPixelsSize)
	,	m_pPixels		( p_pPixels)
	,	m_packMode		( p_packMode)
	,	m_drawMode		( p_drawMode)
{
}

GlPixelBuffer :: ~GlPixelBuffer()
{
}

bool GlPixelBuffer :: Activate()
{
	return GlBufferBase::Bind();
}

void GlPixelBuffer :: Deactivate()
{
	GlBufferBase::Unbind();
}

bool GlPixelBuffer :: Fill( void * p_pBuffer, GLsizeiptr p_iSize)
{
	return GlBufferBase::Fill( p_pBuffer, p_iSize, m_drawMode);
}

//*************************************************************************************************

GlPackPixelBuffer :: GlPackPixelBuffer( unsigned char * p_pPixels, size_t p_uiPixelsSize)
	:	GlPixelBuffer( p_pPixels, p_uiPixelsSize, GL_PIXEL_PACK_BUFFER, GL_STREAM_READ)
{
}

GlPackPixelBuffer :: ~GlPackPixelBuffer()
{
}

void GlPackPixelBuffer :: Initialise()
{
	Create();

	if (Bind())
	{
		Fill( m_pPixels, m_uiPixelsSize);
		Unbind();
	}
}

//*************************************************************************************************

GlUnpackPixelBuffer :: GlUnpackPixelBuffer( unsigned char * p_pPixels, size_t p_uiPixelsSize)
	:	GlPixelBuffer( p_pPixels, p_uiPixelsSize, GL_PIXEL_UNPACK_BUFFER, GL_STREAM_DRAW)
{
}

GlUnpackPixelBuffer :: ~GlUnpackPixelBuffer()
{
}

void GlUnpackPixelBuffer :: Initialise()
{
	Create();

	if (Bind())
	{
		Fill( NULL, m_uiPixelsSize);
		Unbind();
	}
}

//******************************************************************************

GlUboVariableBase :: GlUboVariableBase( GlUniformBufferObject * p_pParent, String const & p_strName, size_t p_uiElemSize, size_t p_uiCount)
	:	m_uiIndex		( GL_INVALID_INDEX)
	,	m_pBuffer		( NULL)
	,	m_strName		( p_strName)
	,	m_uiCount		( p_uiCount)
	,	m_pParent		( p_pParent)
	,	m_uiElemSize	( p_uiElemSize)
{
}

GlUboVariableBase :: ~GlUboVariableBase()
{
}

void GlUboVariableBase :: Activate()
{
	OpenGl::BufferSubData( GL_UNIFORM_BUFFER_EXT, m_iOffset, GetSize(), m_pBuffer);
}

void GlUboVariableBase :: Deactivate()
{
}

//******************************************************************************

GlUniformBufferObject :: GlUniformBufferObject( String const & p_strUniformBlockName)
	:	Buffer3D<unsigned char>()
	,	m_glBuffer( GL_UNIFORM_BUFFER)
	,	m_pProgram( nullptr)
	,	m_strUniformBlockName( p_strUniformBlockName)
	,	m_iUniformBlockIndex( GL_INVALID_INDEX)
	,	m_bChanged( true)
{
}

GlUniformBufferObject :: ~GlUniformBufferObject()
{
	Cleanup();
}

void GlUniformBufferObject :: Cleanup()
{
	Buffer3D<unsigned char>::clear();
	m_glBuffer.Cleanup();
	Root::GetSingleton()->GetBufferManager()->UnassignBuffer<unsigned char>( m_glBuffer.GetGlIndex(), this);
}

void GlUniformBufferObject :: Initialise( GlShaderProgram * p_pProgram)
{
	m_pProgram = p_pProgram;

	if (m_pProgram && m_pProgram->GetProgramObject() > 0)
	{
		if (m_glBuffer.GetGlIndex() == GL_INVALID_INDEX)
		{
			m_glBuffer.Create();
		}

		if (m_iUniformBlockIndex == GL_INVALID_INDEX && m_glBuffer.GetGlIndex() != GL_INVALID_INDEX)
		{
			Root::GetSingleton()->GetBufferManager()->AssignBuffer<unsigned char>( m_glBuffer.GetGlIndex(), this);

			m_iUniformBlockIndex = OpenGl::GetUniformBlockIndex( m_pProgram->GetProgramObject(), m_strUniformBlockName.char_str());

			if (m_iUniformBlockIndex != GL_INVALID_INDEX)
			{
				OpenGl::GetActiveUniformBlockiv( m_pProgram->GetProgramObject(), m_iUniformBlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, & m_iUniformBlockSize);
				reserve( m_iUniformBlockSize, 0);
				m_glBuffer.Bind();
				m_glBuffer.Fill( NULL, m_iUniformBlockSize, GL_STREAM_DRAW);
				OpenGl::BindBufferBase( GL_UNIFORM_BUFFER, 0, m_glBuffer.GetGlIndex());
				OpenGl::UniformBlockBinding( m_pProgram->GetProgramObject(), m_iUniformBlockIndex, 0);
			}
		}
	}
}

void GlUniformBufferObject :: Activate()
{
	if (m_iUniformBlockIndex != GL_INVALID_INDEX && m_bChanged)
	{
		m_glBuffer.Bind();

		for (GlUboVariablePtrStrMap::iterator l_it = m_mapVariables.begin() ; l_it != m_mapVariables.end() ; ++l_it)
		{
			l_it->second->Activate();
		}
	}

	m_bChanged = false;
}

void GlUniformBufferObject :: Deactivate()
{
	m_glBuffer.Unbind();
}

void * GlUniformBufferObject :: Lock( size_t p_uiOffset, size_t p_uiSize, size_t p_uiFlags)
{
	return m_glBuffer.Lock( p_uiOffset, p_uiSize, p_uiFlags);
}

void GlUniformBufferObject :: Unlock()
{
	m_glBuffer.Unlock();
}

bool GlUniformBufferObject :: AddVariable( GlUboVariablePtr p_pVariable)
{
	bool l_bReturn = false;
	char const * l_pChar = p_pVariable->m_strName.char_str();
	OpenGl::GetUniformIndices( m_pProgram->GetProgramObject(), 1, & l_pChar, & (p_pVariable->m_uiIndex));

	if (p_pVariable->m_uiIndex != GL_INVALID_INDEX)
	{
		OpenGl::GetActiveUniformsiv( m_pProgram->GetProgramObject(), 1, & p_pVariable->m_uiIndex, GL_UNIFORM_OFFSET, & (p_pVariable->m_iOffset));
		OpenGl::GetActiveUniformsiv( m_pProgram->GetProgramObject(), 1, & p_pVariable->m_uiIndex, GL_UNIFORM_SIZE, & (p_pVariable->m_iSize));

		if (p_pVariable->m_iSize == 1)
		{
			p_pVariable->m_iSize = p_pVariable->GetSize();
		}

		if (p_pVariable->GetSize() + p_pVariable->m_iOffset <= m_totalSize)
		{
			m_mapVariables.insert( GlUboVariablePtrStrMap::value_type( p_pVariable->m_strName, p_pVariable));
			l_bReturn = true;
			m_bChanged = true;
		}
	}

	return l_bReturn;
}

//******************************************************************************

GlVertexArrayObjects :: GlVertexArrayObjects()
	:	m_uiIndex( GL_INVALID_INDEX)
{
}

GlVertexArrayObjects :: ~GlVertexArrayObjects()
{
	Cleanup();
}

void GlVertexArrayObjects :: Cleanup()
{
	if (m_uiIndex != GL_INVALID_INDEX)
	{
		OpenGl::DeleteVertexArrays( 1, & m_uiIndex);
		m_uiIndex = GL_INVALID_INDEX;
	}
}

void GlVertexArrayObjects :: Initialise()
{
	if (m_uiIndex == GL_INVALID_INDEX)
	{
		OpenGl::GenVertexArrays( 1, & m_uiIndex);
	}
}

void GlVertexArrayObjects :: Activate()
{
	if (m_uiIndex != GL_INVALID_INDEX)
	{
		OpenGl::BindVertexArray( m_uiIndex);
	}
}

void GlVertexArrayObjects :: Deactivate()
{
	OpenGl::BindVertexArray( 0);
}

//******************************************************************************

GlTextureBufferObject :: GlTextureBufferObject()
	:	TextureBufferObject(),
		m_uiIndex( GL_INVALID_INDEX)
{
}

GlTextureBufferObject :: ~GlTextureBufferObject()
{
	Cleanup();
}

void GlTextureBufferObject :: Cleanup()
{
	if (m_uiIndex != GL_INVALID_INDEX)// && glIsBuffer( m_uiIndex))
	{
		Root::GetSingleton()->GetBufferManager()->UnassignBuffer<unsigned char>( m_uiIndex, this);
		OpenGl::DeleteBuffers( 1, & m_uiIndex);
	}

	m_uiIndex = GL_INVALID_INDEX;
}

void GlTextureBufferObject :: Initialise( const ePIXEL_FORMAT & p_format, size_t p_uiSize, unsigned char const * p_pBytes)
{
	TextureBufferObject::Initialise( p_format, p_uiSize, p_pBytes);

	OpenGl::GenBuffers( 1, & m_uiIndex);

	if (m_uiIndex != GL_INVALID_INDEX)// && glIsBuffer( m_uiIndex))
	{
		OpenGl::BindBuffer( GL_TEXTURE_BUFFER, m_uiIndex);
		OpenGl::BufferData( GL_TEXTURE_BUFFER, m_uiSize * Castor::Resources::GetBytesPerPixel( m_pixelFormat) / 8, m_pBytes, GL_STREAM_DRAW);
		OpenGl::BindBuffer( GL_TEXTURE_BUFFER, 0);
		Root::GetSingleton()->GetBufferManager()->AssignBuffer<unsigned char>( m_uiIndex, this);
	}
	else
	{
		m_uiIndex = GL_INVALID_INDEX;
	}
}

void GlTextureBufferObject :: Activate()
{
}

void GlTextureBufferObject :: Activate( PassPtr p_pPass)
{
}

void GlTextureBufferObject :: Deactivate()
{
}

void * GlTextureBufferObject :: Lock( size_t p_uiOffset, size_t p_uiSize, size_t p_uiFlags)
{
	OpenGl::BindBuffer( GL_TEXTURE_BUFFER, m_uiIndex);
	unsigned char * l_pBuffer;
	l_pBuffer = reinterpret_cast<unsigned char *>( OpenGl::MapBuffer( GL_TEXTURE_BUFFER, GlEnum::GetLockFlags( p_uiFlags)));
	return l_pBuffer + p_uiOffset;
}

void GlTextureBufferObject :: Unlock()
{
	OpenGl::UnmapBuffer( GL_TEXTURE_BUFFER);
}

//******************************************************************************
