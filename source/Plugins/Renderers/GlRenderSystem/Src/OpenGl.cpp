#include "OpenGl.hpp"

#include "GlRenderSystem.hpp"

#include <Rectangle.hpp>
#include <Logger.hpp>
#include <Utils.hpp>

using namespace GlRender;
using namespace Castor3D;
using namespace Castor;

#define DEF_USE_DIRECT_STATE_ACCESS 0


#define GL_GET_FUNC( func, ext )\
	if( !gl_api::GetFunction( cuT( "gl" ) + string::string_cast< xchar >( #func ), m_pfn##func ) )\
	{\
		if( !gl_api::GetFunction( cuT( "gl" ) + string::string_cast< xchar >( #func ) + string::string_cast< xchar >( #ext ), m_pfn##func ) )\
		{\
			Logger::LogWarning( cuT( "Unable to retrieve function gl" ) + string::string_cast< xchar >( #func ) );\
		}\
	}

//*************************************************************************************************

TexFunctionsBase::TexFunctionsBase( OpenGl & p_gl )
	: Holder( p_gl )
{
}

//*************************************************************************************************

TexFunctions::TexFunctions( OpenGl & p_gl )
	: TexFunctionsBase( p_gl )
{
	m_pfnBindTexture = &glBindTexture;
	m_pfnTexSubImage1D = &glTexSubImage1D;
	m_pfnTexSubImage2D = &glTexSubImage2D;
	m_pfnTexImage1D = &glTexImage1D;
	m_pfnTexImage2D = &glTexImage2D;
	m_pfnTexParameteri = &glTexParameteri;
	m_pfnTexParameterf = &glTexParameterf;
	m_pfnTexParameteriv = &glTexParameteriv;
	m_pfnTexParameterfv = &glTexParameterfv;
	m_pfnGetTexImage = &glGetTexImage;
	GL_GET_FUNC( TexImage3D, EXT );
	GL_GET_FUNC( TexSubImage3D, EXT );
	GL_GET_FUNC( GenerateMipmap, EXT );
}

bool TexFunctions::GenerateMipmap( eGL_TEXDIM mode )
{
	if ( m_pfnGenerateMipmap )
	{
		m_pfnGenerateMipmap( mode );
	}

	return glCheckError( GetOpenGl(), cuT( "glGenerateMipmap" ) );
}

bool TexFunctions::BindTexture( eGL_TEXDIM mode, uint32_t texture )
{
	m_pfnBindTexture( mode, texture );
	return glCheckError( GetOpenGl(), cuT( "glBindTexture" ) );
}

bool TexFunctions::TexSubImage1D( eGL_TEXDIM mode, int level, int xoffset, int width, eGL_FORMAT format, eGL_TYPE type, void const * data )
{
	m_pfnTexSubImage1D( mode, level, xoffset, width, format, type, data );
	return glCheckError( GetOpenGl(), cuT( "glTexSubImage1D" ) );
}

bool TexFunctions::TexSubImage2D( eGL_TEXDIM mode, int level, int xoffset, int yoffset, int width, int height, eGL_FORMAT format, eGL_TYPE type, void const * data )
{
	m_pfnTexSubImage2D( mode, level, xoffset, yoffset, width, height, format, type, data );
	return glCheckError( GetOpenGl(), cuT( "glTexSubImage2D" ) );
}

bool TexFunctions::TexSubImage2D( eGL_TEXDIM mode, int level, Position const & p_position, Size const & p_size, eGL_FORMAT format, eGL_TYPE type, void const * data )
{
	m_pfnTexSubImage2D( mode, level, p_position.x(), p_position.y(), p_size.width(), p_size.height(), format, type, data );
	return glCheckError( GetOpenGl(), cuT( "glTexSubImage2D" ) );
}

bool TexFunctions::TexSubImage2D( eGL_TEXDIM mode, int level, Castor::Rectangle const & p_rect, eGL_FORMAT format, eGL_TYPE type, void const * data )
{
	m_pfnTexSubImage2D( mode, level, p_rect.left(), p_rect.top(), p_rect.width(), p_rect.height(), format, type, data );
	return glCheckError( GetOpenGl(), cuT( "glTexSubImage2D" ) );
}

bool TexFunctions::TexSubImage3D( eGL_TEXDIM mode, int level, int xoffset, int yoffset, int zoffset, int width, int height, int depth, eGL_FORMAT format, eGL_TYPE type, void const * data )
{
	m_pfnTexSubImage3D( mode, level, xoffset, yoffset, zoffset, width, height, depth, format, type, data );
	return glCheckError( GetOpenGl(), cuT( "glTexSubImage3D" ) );
}

bool TexFunctions::TexImage1D( eGL_TEXDIM mode, int level, eGL_INTERNAL internalFormat, int width, int border, eGL_FORMAT format, uint32_t type, void const * data )
{
	try
	{
		m_pfnTexImage1D( mode, level, internalFormat, width, border, format, type, data );
		return glCheckError( GetOpenGl(), cuT( "glTexImage1D" ) );
	}
	catch ( ... )
	{
		return false;
	}
}

bool TexFunctions::TexImage2D( eGL_TEXDIM mode, int level, eGL_INTERNAL internalFormat, int width, int height, int border, eGL_FORMAT format, uint32_t type, void const * data )
{
	try
	{
		m_pfnTexImage2D( mode, level, internalFormat, width, height, border, format, type, data );
		return glCheckError( GetOpenGl(), cuT( "glTexImage2D" ) );
	}
	catch ( ... )
	{
		return false;
	}
}

bool TexFunctions::TexImage2D( eGL_TEXDIM mode, int level, eGL_INTERNAL internalFormat, Size const & p_size, int border, eGL_FORMAT format, uint32_t type, void const * data )
{
	try
	{
		m_pfnTexImage2D( mode, level, internalFormat, p_size.width(), p_size.height(), border, format, type, data );
		return glCheckError( GetOpenGl(), cuT( "glTexImage2D" ) );
	}
	catch ( ... )
	{
		return false;
	}
}

bool TexFunctions::TexImage3D( eGL_TEXDIM mode, int level, eGL_INTERNAL internalFormat, int width, int height, int depth, int border, eGL_FORMAT format, uint32_t type, void const * data )
{
	try
	{
		m_pfnTexImage3D( mode, level, internalFormat, width, height, depth, border, format, type, data );
		return glCheckError( GetOpenGl(), cuT( "glTexImage3D" ) );
	}
	catch ( ... )
	{
		return false;
	}
}

bool TexFunctions::TexParameter( eGL_TEXDIM mode, eGL_TEXTURE_PARAMETER pname, int param )
{
	m_pfnTexParameteri( mode, pname, param );
	return glCheckError( GetOpenGl(), cuT( "glTexParameteri" ) );
}

bool TexFunctions::TexParameter( eGL_TEXDIM mode, eGL_TEXTURE_PARAMETER pname, float param )
{
	m_pfnTexParameterf( mode, pname, param );
	return glCheckError( GetOpenGl(), cuT( "glTexParameterf" ) );
}

bool TexFunctions::TexParameter( eGL_TEXDIM mode, eGL_TEXTURE_PARAMETER pname, const int * params )
{
	m_pfnTexParameteriv( mode, pname, params );
	return glCheckError( GetOpenGl(), cuT( "glTexParameteriv" ) );
}

bool TexFunctions::TexParameter( eGL_TEXDIM mode, eGL_TEXTURE_PARAMETER pname, const float * params )
{
	m_pfnTexParameterfv( mode, pname, params );
	return glCheckError( GetOpenGl(), cuT( "glTexParameterfv" ) );
}

bool TexFunctions::GetTexImage( eGL_TEXDIM mode, int level, eGL_FORMAT format, eGL_TYPE type, void * img )
{
	m_pfnGetTexImage( mode, level, format, type, img );
	return glCheckError( GetOpenGl(), cuT( "glGetTexImage" ) );
}

//*************************************************************************************************

TexFunctionsDSA::TexFunctionsDSA( OpenGl & p_gl )
	: TexFunctionsBase( p_gl )
	, m_uiTexture( 0 )
	, m_pfnTextureSubImage1D()
	, m_pfnTextureSubImage2D()
	, m_pfnTextureSubImage3D()
	, m_pfnTextureImage1D()
	, m_pfnTextureImage2D()
	, m_pfnTextureImage3D()
{
	GL_GET_FUNC( TextureSubImage1D, EXT );
	GL_GET_FUNC( TextureSubImage2D, EXT );
	GL_GET_FUNC( TextureSubImage3D, EXT );
	GL_GET_FUNC( TextureImage1D, EXT );
	GL_GET_FUNC( TextureImage2D, EXT );
	GL_GET_FUNC( TextureImage3D, EXT );
	GL_GET_FUNC( TextureParameteri, EXT );
	GL_GET_FUNC( TextureParameterf, EXT );
	GL_GET_FUNC( TextureParameteriv, EXT );
	GL_GET_FUNC( TextureParameterfv, EXT );
	GL_GET_FUNC( GetTextureImage, EXT );
	GL_GET_FUNC( GenerateTextureMipmap, EXT );
}

bool TexFunctionsDSA::GenerateMipmap( eGL_TEXDIM mode )
{
	if ( m_pfnGenerateTextureMipmap )
	{
		m_pfnGenerateTextureMipmap( m_uiTexture, mode );
	}

	return glCheckError( GetOpenGl(), cuT( "glGenerateTextureMipmap" ) );
}

bool TexFunctionsDSA::TexSubImage1D( eGL_TEXDIM mode, int level, int xoffset, int width, eGL_FORMAT format, eGL_TYPE type, void const * data )
{
	m_pfnTextureSubImage1D( m_uiTexture, mode, level, xoffset, width, format, type, data );
	return glCheckError( GetOpenGl(), cuT( "glTextureSubImage1D" ) );
}

bool TexFunctionsDSA::TexSubImage2D( eGL_TEXDIM mode, int level, int xoffset, int yoffset, int width, int height, eGL_FORMAT format, eGL_TYPE type, void const * data )
{
	m_pfnTextureSubImage2D( m_uiTexture, mode, level, xoffset, yoffset, width, height, format, type, data );
	return glCheckError( GetOpenGl(), cuT( "glTextureSubImage2D" ) );
}

bool TexFunctionsDSA::TexSubImage2D( eGL_TEXDIM mode, int level, Position const & p_position, Size const & p_size, eGL_FORMAT format, eGL_TYPE type, void const * data )
{
	m_pfnTextureSubImage2D( m_uiTexture, mode, level, p_position.x(), p_position.y(), p_size.width(), p_size.height(), format, type, data );
	return glCheckError( GetOpenGl(), cuT( "glTextureSubImage2D" ) );
}

bool TexFunctionsDSA::TexSubImage2D( eGL_TEXDIM mode, int level, Castor::Rectangle const & p_rect, eGL_FORMAT format, eGL_TYPE type, void const * data )
{
	m_pfnTextureSubImage2D( m_uiTexture, mode, level, p_rect.left(), p_rect.top(), p_rect.width(), p_rect.height(), format, type, data );
	return glCheckError( GetOpenGl(), cuT( "glTextureSubImage2D" ) );
}

bool TexFunctionsDSA::TexSubImage3D( eGL_TEXDIM mode, int level, int xoffset, int yoffset, int zoffset, int width, int height, int depth, eGL_FORMAT format, eGL_TYPE type, void const * data )
{
	m_pfnTextureSubImage3D( m_uiTexture, mode, level, xoffset, yoffset, zoffset, width, height, depth, format, type, data );
	return glCheckError( GetOpenGl(), cuT( "glTextureSubImage3D" ) );
}

bool TexFunctionsDSA::TexImage1D( eGL_TEXDIM mode, int level, eGL_INTERNAL internalFormat, int width, int border, eGL_FORMAT format, uint32_t type, void const * data )
{
	try
	{
		m_pfnTextureImage1D( m_uiTexture, mode, level, internalFormat, width, border, format, type, data );
		return glCheckError( GetOpenGl(), cuT( "glTextureImage1D" ) );
	}
	catch ( ... )
	{
		return false;
	}
}

bool TexFunctionsDSA::TexImage2D( eGL_TEXDIM mode, int level, eGL_INTERNAL internalFormat, int width, int height, int border, eGL_FORMAT format, uint32_t type, void const * data )
{
	try
	{
		m_pfnTextureImage2D( m_uiTexture, mode, level, internalFormat, width, height, border, format, type, data );
		return glCheckError( GetOpenGl(), cuT( "glTextureImage2D" ) );
	}
	catch ( ... )
	{
		return false;
	}
}

bool TexFunctionsDSA::TexImage2D( eGL_TEXDIM mode, int level, eGL_INTERNAL internalFormat, Size const & p_size, int border, eGL_FORMAT format, uint32_t type, void const * data )
{
	try
	{
		m_pfnTextureImage2D( m_uiTexture, mode, level, internalFormat, p_size.width(), p_size.height(), border, format, type, data );
		return glCheckError( GetOpenGl(), cuT( "glTextureImage2D" ) );
	}
	catch ( ... )
	{
		return false;
	}
}

bool TexFunctionsDSA::TexImage3D( eGL_TEXDIM mode, int level, eGL_INTERNAL internalFormat, int width, int height, int depth, int border, eGL_FORMAT format, uint32_t type, void const * data )
{
	try
	{
		m_pfnTextureImage3D( m_uiTexture, mode, level, internalFormat, width, height, depth, border, format, type, data );
		return glCheckError( GetOpenGl(), cuT( "glTextureImage3D" ) );
	}
	catch ( ... )
	{
		return false;
	}
}

bool TexFunctionsDSA::TexParameter( eGL_TEXDIM mode, eGL_TEXTURE_PARAMETER pname, int param )
{
	m_pfnTextureParameteri( m_uiTexture, mode, pname, param );
	return glCheckError( GetOpenGl(), cuT( "glTextureParameteri" ) );
}

bool TexFunctionsDSA::TexParameter( eGL_TEXDIM mode, eGL_TEXTURE_PARAMETER pname, float param )
{
	m_pfnTextureParameterf( m_uiTexture, mode, pname, param );
	return glCheckError( GetOpenGl(), cuT( "glTextureParameterf" ) );
}

bool TexFunctionsDSA::TexParameter( eGL_TEXDIM mode, eGL_TEXTURE_PARAMETER pname, const int * params )
{
	m_pfnTextureParameteriv( m_uiTexture, mode, pname, params );
	return glCheckError( GetOpenGl(), cuT( "glTextureParameteriv" ) );
}

bool TexFunctionsDSA::TexParameter( eGL_TEXDIM mode, eGL_TEXTURE_PARAMETER pname, const float * params )
{
	m_pfnTextureParameterfv( m_uiTexture, mode, pname, params );
	return glCheckError( GetOpenGl(), cuT( "glTextureParameterfv" ) );
}

bool TexFunctionsDSA::GetTexImage( eGL_TEXDIM mode, int level, eGL_FORMAT format, eGL_TYPE type, void * img )
{
	m_pfnGetTextureImage( m_uiTexture, mode, level, format, type, img );
	return glCheckError( GetOpenGl(), cuT( "glGetTextureImage" ) );
}

//*************************************************************************************************

BufFunctionsBase::BufFunctionsBase( OpenGl & p_gl )
	: Holder( p_gl )
{
	if ( p_gl.HasExtension( NV_shader_buffer_load ) )
	{
		GL_GET_FUNC( MakeBufferResident, NV );
		GL_GET_FUNC( MakeBufferNonResident, NV );
		GL_GET_FUNC( IsBufferResident, NV );
		GL_GET_FUNC( MakeNamedBufferResident, NV );
		GL_GET_FUNC( MakeNamedBufferNonResident, NV );
		GL_GET_FUNC( IsNamedBufferResident, NV );
		GL_GET_FUNC( GetBufferParameterui64v, NV );
		GL_GET_FUNC( GetNamedBufferParameterui64v, NV );
	}

	if ( p_gl.HasExtension( NV_vertex_buffer_unified_memory ) )
	{
		GL_GET_FUNC( BufferAddressRange, NV );
		GL_GET_FUNC( VertexFormat, NV );
		GL_GET_FUNC( NormalFormat, NV );
		GL_GET_FUNC( ColorFormat, NV );
		GL_GET_FUNC( IndexFormat, NV );
		GL_GET_FUNC( TexCoordFormat, NV );
		GL_GET_FUNC( EdgeFlagFormat, NV );
		GL_GET_FUNC( SecondaryColorFormat, NV );
		GL_GET_FUNC( FogCoordFormat, NV );
		GL_GET_FUNC( VertexAttribFormat, NV );
		GL_GET_FUNC( VertexAttribIFormat, NV );
	}

}

bool BufFunctionsBase::EnableClientState( eGL_BUFFER_USAGE p_eArray )
{
	m_pfnEnableClientState( p_eArray );
	return glCheckError( GetOpenGl(), cuT( "glEnableClientState" ) );
}

bool BufFunctionsBase::DisableClientState( eGL_BUFFER_USAGE p_eArray )
{
	m_pfnDisableClientState( p_eArray );
	return glCheckError( GetOpenGl(), cuT( "glDisableClientState" ) );
}

bool BufFunctionsBase::BufferAddressRange( eGL_ADDRESS pname, uint32_t index, uint64_t address, size_t length )
{
	m_pfnBufferAddressRange( pname, index, address, length );
	return glCheckError( GetOpenGl(), cuT( "glBufferAddressRange" ) );
}

bool BufFunctionsBase::VertexFormat( int size, eGL_TYPE type, int stride )
{
	m_pfnVertexFormat( size, type, stride );
	return glCheckError( GetOpenGl(), cuT( "glVertexFormat" ) );
}

bool BufFunctionsBase::NormalFormat( eGL_TYPE type, int stride )
{
	m_pfnNormalFormat( type, stride );
	return glCheckError( GetOpenGl(), cuT( "glNormalFormat" ) );
}

bool BufFunctionsBase::ColorFormat( int size, eGL_TYPE type, int stride )
{
	m_pfnColorFormat( size, type, stride );
	return glCheckError( GetOpenGl(), cuT( "glColorFormat" ) );
}

bool BufFunctionsBase::IndexFormat( eGL_TYPE type, int stride )
{
	m_pfnIndexFormat( type, stride );
	return glCheckError( GetOpenGl(), cuT( "glIndexFormat" ) );
}

bool BufFunctionsBase::TexCoordFormat( int size, eGL_TYPE type, int stride )
{
	m_pfnTexCoordFormat( size, type, stride );
	return glCheckError( GetOpenGl(), cuT( "glTexCoordFormat" ) );
}

bool BufFunctionsBase::EdgeFlagFormat( int stride )
{
	m_pfnEdgeFlagFormat( stride );
	return glCheckError( GetOpenGl(), cuT( "glEdgeFlagFormat" ) );
}

bool BufFunctionsBase::SecondaryColorFormat( int size, eGL_TYPE type, int stride )
{
	m_pfnSecondaryColorFormat( size, type, stride );
	return glCheckError( GetOpenGl(), cuT( "glSecondaryColorFormat" ) );
}

bool BufFunctionsBase::FogCoordFormat( uint32_t type, int stride )
{
	m_pfnFogCoordFormat( type, stride );
	return glCheckError( GetOpenGl(), cuT( "glFogCoordFormat" ) );
}

bool BufFunctionsBase::VertexAttribFormat( uint32_t index, int size, eGL_TYPE type, bool normalized, int stride )
{
	m_pfnVertexAttribFormat( index, size, type, normalized, stride );
	return glCheckError( GetOpenGl(), cuT( "glVertexAttribFormat" ) );
}

bool BufFunctionsBase::VertexAttribIFormat( uint32_t index, int size, eGL_TYPE type, int stride )
{
	m_pfnVertexAttribIFormat( index, size, type, stride );
	return glCheckError( GetOpenGl(), cuT( "glVertexAttribIFormat" ) );
}

bool BufFunctionsBase::MakeBufferResident( eGL_BUFFER_TARGET target, eGL_RESIDENT_BUFFER_ACCESS access )
{
	m_pfnMakeBufferResident( target, access );
	return glCheckError( GetOpenGl(), cuT( "glMakeBufferResident" ) );
}

bool BufFunctionsBase::MakeBufferNonResident( eGL_BUFFER_TARGET target )
{
	m_pfnMakeBufferNonResident( target );
	return glCheckError( GetOpenGl(), cuT( "glMakeBufferNonResident" ) );
}

bool BufFunctionsBase::IsBufferResident( eGL_BUFFER_TARGET target )
{
	return m_pfnIsBufferResident( target ) && glCheckError( GetOpenGl(), cuT( "glIsBufferResident" ) );
}

bool BufFunctionsBase::MakeNamedBufferResident( uint32_t buffer, eGL_RESIDENT_BUFFER_ACCESS access )
{
	m_pfnMakeNamedBufferResident( buffer, access );
	return glCheckError( GetOpenGl(), cuT( "glMakeNamedBufferResident" ) );
}

bool BufFunctionsBase::MakeNamedBufferNonResident( uint32_t buffer )
{
	m_pfnMakeNamedBufferNonResident( buffer );
	return glCheckError( GetOpenGl(), cuT( "glMakeNamedBufferNonResident" ) );
}

bool BufFunctionsBase::IsNamedBufferResident( uint32_t buffer )
{
	return m_pfnIsNamedBufferResident( buffer ) && glCheckError( GetOpenGl(), cuT( "glIsNamedBufferResident" ) );
}

bool BufFunctionsBase::GetBufferParameter( eGL_BUFFER_TARGET target, eGL_BUFFER_PARAMETER pname, uint64_t * params )
{
	m_pfnGetBufferParameterui64v( target, pname, params );
	return glCheckError( GetOpenGl(), cuT( "glGetBufferParameterui64v" ) );
}

bool BufFunctionsBase::GetNamedBufferParameter( uint32_t buffer, eGL_BUFFER_PARAMETER pname, uint64_t * params )
{
	m_pfnGetNamedBufferParameterui64v( buffer, pname, params );
	return glCheckError( GetOpenGl(), cuT( "glGetNamedBufferParameterui64v" ) );
}

//*************************************************************************************************

BufFunctions::BufFunctions( OpenGl & p_gl )
	: BufFunctionsBase( p_gl )
{
	GL_GET_FUNC( BindBuffer, ARB );
	GL_GET_FUNC( BufferData, ARB );
	GL_GET_FUNC( BufferSubData, ARB );
	GL_GET_FUNC( GetBufferParameteriv, ARB );
	GL_GET_FUNC( MapBuffer, ARB );
	GL_GET_FUNC( UnmapBuffer, ARB );
	GL_GET_FUNC( MapBufferRange, );
	GL_GET_FUNC( FlushMappedBufferRange, );
}

bool BufFunctions::BindBuffer( eGL_BUFFER_TARGET p_target, uint32_t buffer )
{
	m_pfnBindBuffer( p_target, buffer );
	return glCheckError( GetOpenGl(), cuT( "glBindBuffer" ) );
}

bool BufFunctions::BufferData( eGL_BUFFER_TARGET p_target, ptrdiff_t size, void const * data, eGL_BUFFER_MODE usage )
{
	m_pfnBufferData( p_target, size, data, usage );
	return glCheckError( GetOpenGl(), cuT( "glBufferData" ) );
}

bool BufFunctions::BufferSubData( eGL_BUFFER_TARGET p_target, ptrdiff_t offset, ptrdiff_t size, void const * data )
{
	m_pfnBufferSubData( p_target, offset, size, data );
	return glCheckError( GetOpenGl(), cuT( "glBufferSubData" ) );
}

bool BufFunctions::GetBufferParameter( eGL_BUFFER_TARGET p_target, eGL_BUFFER_PARAMETER pname, int * params )
{
	m_pfnGetBufferParameteriv( p_target, pname, params );
	return glCheckError( GetOpenGl(), cuT( "glGetBufferParameteriv" ) );
}

void * BufFunctions::MapBuffer( eGL_BUFFER_TARGET p_target, eGL_LOCK access )
{
	void * l_return = m_pfnMapBuffer( p_target, access );

	if ( !glCheckError( GetOpenGl(), cuT( "glMapBuffer" ) ) )
	{
		l_return = NULL;
	}

	return l_return;
}

bool BufFunctions::UnmapBuffer( eGL_BUFFER_TARGET p_target )
{
	m_pfnUnmapBuffer( p_target );
	return glCheckError( GetOpenGl(), cuT( "glUnmapBuffer" ) );
}

void * BufFunctions::MapBufferRange( eGL_BUFFER_TARGET p_target, ptrdiff_t offset, ptrdiff_t length, uint32_t access )
{
	void * l_return = NULL;
//	if( length + offset < GL_BUFFER_SIZE )
	{
		l_return = m_pfnMapBufferRange( p_target, offset, length, access );

		if ( !glCheckError( GetOpenGl(), cuT( "glMapBufferRange" ) ) )
		{
			l_return = NULL;
		}
	}
	return l_return;
}

bool BufFunctions::FlushMappedBufferRange( eGL_BUFFER_TARGET p_target, ptrdiff_t offset, ptrdiff_t length )
{
	m_pfnFlushMappedBufferRange( p_target, offset, length );
	return glCheckError( GetOpenGl(), cuT( "glFlushMappedBufferRange" ) );
}

//*************************************************************************************************

BufFunctionsDSA::BufFunctionsDSA( OpenGl & p_gl )
	: BufFunctionsBase( p_gl )
	, m_uiBuffer( 0 )
{
	GL_GET_FUNC( NamedBufferData, EXT );
	GL_GET_FUNC( NamedBufferSubData, EXT );
	GL_GET_FUNC( GetNamedBufferParameteriv, EXT );
	GL_GET_FUNC( MapNamedBuffer, EXT );
	GL_GET_FUNC( UnmapNamedBuffer, EXT );
	GL_GET_FUNC( MapNamedBufferRange, EXT );
	GL_GET_FUNC( FlushMappedNamedBufferRange, EXT );
}

bool BufFunctionsDSA::BufferData( eGL_BUFFER_TARGET p_target, ptrdiff_t size, void const * data, eGL_BUFFER_MODE usage )
{
	m_pfnNamedBufferData( m_uiBuffer, size, data, usage );
	return glCheckError( GetOpenGl(), cuT( "glBufferData" ) );
}

bool BufFunctionsDSA::BufferSubData( eGL_BUFFER_TARGET p_target, ptrdiff_t offset, ptrdiff_t size, void const * data )
{
	m_pfnNamedBufferSubData( m_uiBuffer, offset, size, data );
	return glCheckError( GetOpenGl(), cuT( "glBufferSubData" ) );
}

bool BufFunctionsDSA::GetBufferParameter( eGL_BUFFER_TARGET p_target, eGL_BUFFER_PARAMETER pname, int * params )
{
	m_pfnGetNamedBufferParameteriv( m_uiBuffer, pname, params );
	return glCheckError( GetOpenGl(), cuT( "glGetBufferParameteriv" ) );
}

void * BufFunctionsDSA::MapBuffer( eGL_BUFFER_TARGET p_target, eGL_LOCK access )
{
	void * l_return = m_pfnMapNamedBuffer( m_uiBuffer, access );

	if ( !glCheckError( GetOpenGl(), cuT( "glMapBuffer" ) ) )
	{
		l_return = NULL;
	}

	return l_return;
}

bool BufFunctionsDSA::UnmapBuffer( eGL_BUFFER_TARGET p_target )
{
	m_pfnUnmapNamedBuffer( m_uiBuffer );
	return glCheckError( GetOpenGl(), cuT( "glUnmapBuffer" ) );
}

void * BufFunctionsDSA::MapBufferRange( eGL_BUFFER_TARGET p_target, ptrdiff_t offset, ptrdiff_t length, uint32_t access )
{
	void * l_return = NULL;
//	if( length + offset < GL_BUFFER_SIZE )
	{
		l_return = m_pfnMapNamedBufferRange( m_uiBuffer, offset, length, access );

		if ( !glCheckError( GetOpenGl(), cuT( "glMapBufferRange" ) ) )
		{
			l_return = NULL;
		}
	}
	return l_return;
}

bool BufFunctionsDSA::FlushMappedBufferRange( eGL_BUFFER_TARGET p_target, ptrdiff_t offset, ptrdiff_t length )
{
	m_pfnFlushMappedNamedBufferRange( m_uiBuffer, offset, length );
	return glCheckError( GetOpenGl(), cuT( "glFlushMappedBufferRange" ) );
}

//*************************************************************************************************

#define CASTOR_DBG_WIN32 0

OpenGl::OpenGl( GlRenderSystem & p_renderSystem )
	: m_bHasAnisotropic( false )
	, m_bHasInstancedDraw( false )
	, m_bHasInstancedArrays( false )
	, m_pTexFunctions( NULL )
	, m_pBufFunctions( NULL )
	, m_pfnReadPixels()
	, m_pfnBlitFramebuffer()
	, m_pfnTexImage2DMultisample()
	, m_pfnGetActiveUniform()
	, m_pfnVertexAttribPointer()
	, m_bBindVboToGpuAddress( false )
	, m_renderSystem( p_renderSystem )
{
	uint32_t l_index = 0;
	GlslStrings[l_index++] = cuT( "[e00] GLSL is not available!" );
	GlslStrings[l_index++] = cuT( "[e01] Not a valid program object!" );
	GlslStrings[l_index++] = cuT( "[e02] Not a valid object!" );
	GlslStrings[l_index++] = cuT( "[e03] Out of memory!" );
	GlslStrings[l_index++] = cuT( "[e04] Unknown compiler error!" );
	GlslStrings[l_index++] = cuT( "[e05] Linker log is not available!" );
	GlslStrings[l_index++] = cuT( "[e06] Compiler log is not available!" );
	GlslStrings[l_index] = cuT( "[Empty]" );

	l_index = 0;
	GlslErrors[l_index++] = cuT( "[500] Invalid Enum !" );
	GlslErrors[l_index++] = cuT( "[501] Invalid Value !" );
	GlslErrors[l_index++] = cuT( "[502] Invalid Operation !" );
	GlslErrors[l_index++] = cuT( "[503] Stack Overflow !" );
	GlslErrors[l_index++] = cuT( "[504] Stack Underflow !" );
	GlslErrors[l_index++] = cuT( "[505] Out of memory !" );
	GlslErrors[l_index++] = cuT( "[506] Invalid frame buffer operation" );
	GlslErrors[l_index] = cuT( "[Empty] Unknown Error" );

	PrimitiveTypes[eTOPOLOGY_POINTS] = eGL_PRIMITIVE_POINTS;
	PrimitiveTypes[eTOPOLOGY_LINES] = eGL_PRIMITIVE_LINES;
	PrimitiveTypes[eTOPOLOGY_LINE_LOOP] = eGL_PRIMITIVE_LINE_LOOP;
	PrimitiveTypes[eTOPOLOGY_LINE_STRIP] = eGL_PRIMITIVE_LINE_STRIP;
	PrimitiveTypes[eTOPOLOGY_TRIANGLES] = eGL_PRIMITIVE_TRIANGLES;
	PrimitiveTypes[eTOPOLOGY_TRIANGLE_STRIPS] = eGL_PRIMITIVE_TRIANGLE_STRIP;
	PrimitiveTypes[eTOPOLOGY_TRIANGLE_FAN] = eGL_PRIMITIVE_TRIANGLE_FAN;
	PrimitiveTypes[eTOPOLOGY_QUADS] = eGL_PRIMITIVE_QUADS;
	PrimitiveTypes[eTOPOLOGY_QUAD_STRIPS] = eGL_PRIMITIVE_QUAD_STRIP;
	PrimitiveTypes[eTOPOLOGY_POLYGON] = eGL_PRIMITIVE_POLYGON;

	TextureDimensions[eTEXTURE_TYPE_BUFFER] = eGL_TEXDIM_BUFFER;
	TextureDimensions[eTEXTURE_TYPE_1D] = eGL_TEXDIM_1D;
	TextureDimensions[eTEXTURE_TYPE_1DARRAY] = eGL_TEXDIM_1D_ARRAY;
	TextureDimensions[eTEXTURE_TYPE_2D] = eGL_TEXDIM_2D;
	TextureDimensions[eTEXTURE_TYPE_2DARRAY] = eGL_TEXDIM_2D_ARRAY;
	TextureDimensions[eTEXTURE_TYPE_2DMS] = eGL_TEXDIM_2DMS;
	TextureDimensions[eTEXTURE_TYPE_2DMSARRAY] = eGL_TEXDIM_2DMS_ARRAY;
	TextureDimensions[eTEXTURE_TYPE_3D] = eGL_TEXDIM_3D;
	TextureDimensions[eTEXTURE_TYPE_CUBE] = eGL_TEXDIM_CUBE;
	TextureDimensions[eTEXTURE_TYPE_CUBEARRAY] = eGL_TEXDIM_CUBE_ARRAY;

	AlphaFuncs[eALPHA_FUNC_ALWAYS] = eGL_FUNC_ALWAYS;
	AlphaFuncs[eALPHA_FUNC_LESS] = eGL_FUNC_LESS;
	AlphaFuncs[eALPHA_FUNC_LESS_OR_EQUAL] = eGL_FUNC_LEQUAL;
	AlphaFuncs[eALPHA_FUNC_EQUAL] = eGL_FUNC_EQUAL;
	AlphaFuncs[eALPHA_FUNC_NOT_EQUAL] = eGL_FUNC_NOTEQUAL;
	AlphaFuncs[eALPHA_FUNC_GREATER_OR_EQUAL] = eGL_FUNC_GEQUAL;
	AlphaFuncs[eALPHA_FUNC_GREATER] = eGL_FUNC_GREATER;
	AlphaFuncs[eALPHA_FUNC_NEVER] = eGL_FUNC_NEVER;

	TextureWrapMode[eWRAP_MODE_REPEAT] = eGL_WRAP_MODE_REPEAT;
	TextureWrapMode[eWRAP_MODE_MIRRORED_REPEAT] = eGL_WRAP_MODE_MIRRORED_REPEAT;
	TextureWrapMode[eWRAP_MODE_CLAMP_TO_BORDER] = eGL_WRAP_MODE_CLAMP_TO_BORDER;
	TextureWrapMode[eWRAP_MODE_CLAMP_TO_EDGE] = eGL_WRAP_MODE_CLAMP_TO_EDGE;

	TextureInterpolation[eINTERPOLATION_MODE_UNDEFINED] = eGL_INTERPOLATION_MODE_NEAREST;
	TextureInterpolation[eINTERPOLATION_MODE_NEAREST] = eGL_INTERPOLATION_MODE_NEAREST;
	TextureInterpolation[eINTERPOLATION_MODE_LINEAR] = eGL_INTERPOLATION_MODE_LINEAR;
	TextureInterpolation[eINTERPOLATION_MODE_ANISOTROPIC] = eGL_INTERPOLATION_MODE_LINEAR;

	BlendFactors[eBLEND_ZERO] = eGL_BLEND_FACTOR_ZERO;
	BlendFactors[eBLEND_ONE] = eGL_BLEND_FACTOR_ONE;
	BlendFactors[eBLEND_SRC_COLOUR] = eGL_BLEND_FACTOR_SRC_COLOR;
	BlendFactors[eBLEND_INV_SRC_COLOUR] = eGL_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
	BlendFactors[eBLEND_DST_COLOUR] = eGL_BLEND_FACTOR_DST_COLOR;
	BlendFactors[eBLEND_INV_DST_COLOUR] = eGL_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
	BlendFactors[eBLEND_SRC_ALPHA] = eGL_BLEND_FACTOR_SRC_ALPHA;
	BlendFactors[eBLEND_INV_SRC_ALPHA] = eGL_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	BlendFactors[eBLEND_DST_ALPHA] = eGL_BLEND_FACTOR_DST_ALPHA;
	BlendFactors[eBLEND_INV_DST_ALPHA] = eGL_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
	BlendFactors[eBLEND_CONSTANT] = eGL_BLEND_FACTOR_CONSTANT;
	BlendFactors[eBLEND_INV_CONSTANT] = eGL_BLEND_FACTOR_ONE_MINUS_CONSTANT;
	BlendFactors[eBLEND_SRC_ALPHA_SATURATE] = eGL_BLEND_FACTOR_SRC_ALPHA_SATURATE;
	BlendFactors[eBLEND_SRC1_COLOUR] = eGL_BLEND_FACTOR_SRC1_COLOR;
	BlendFactors[eBLEND_INV_SRC1_COLOUR] = eGL_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
	BlendFactors[eBLEND_SRC1_ALPHA] = eGL_BLEND_FACTOR_SRC1_ALPHA;
	BlendFactors[eBLEND_INV_SRC1_ALPHA] = eGL_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;

	Usages[eELEMENT_USAGE_POSITION] = eGL_BUFFER_USAGE_VERTEX_ARRAY;
	Usages[eELEMENT_USAGE_NORMAL] = eGL_BUFFER_USAGE_NORMAL_ARRAY;
	Usages[eELEMENT_USAGE_TANGENT] = eGL_BUFFER_USAGE_TANGENT_ARRAY;
	Usages[eELEMENT_USAGE_BITANGENT] = eGL_BUFFER_USAGE_BINORMAL_ARRAY;
	Usages[eELEMENT_USAGE_DIFFUSE] = eGL_BUFFER_USAGE_COLOR_ARRAY;
	Usages[eELEMENT_USAGE_TEXCOORDS0] = eGL_BUFFER_USAGE_TEXTURE_COORD_ARRAY;
	Usages[eELEMENT_USAGE_TEXCOORDS1] = eGL_BUFFER_USAGE_TEXTURE_COORD_ARRAY;
	Usages[eELEMENT_USAGE_TEXCOORDS2] = eGL_BUFFER_USAGE_TEXTURE_COORD_ARRAY;
	Usages[eELEMENT_USAGE_TEXCOORDS3] = eGL_BUFFER_USAGE_TEXTURE_COORD_ARRAY;

	TextureArguments[eBLEND_SOURCE_TEXTURE] = eGL_BLEND_SOURCE_TEXTURE;
	TextureArguments[eBLEND_SOURCE_TEXTURE0] = eGL_BLEND_SOURCE_TEXTURE0;
	TextureArguments[eBLEND_SOURCE_TEXTURE1] = eGL_BLEND_SOURCE_TEXTURE1;
	TextureArguments[eBLEND_SOURCE_TEXTURE2] = eGL_BLEND_SOURCE_TEXTURE2;
	TextureArguments[eBLEND_SOURCE_TEXTURE3] = eGL_BLEND_SOURCE_TEXTURE3;
	TextureArguments[eBLEND_SOURCE_CONSTANT] = eGL_BLEND_SOURCE_CONSTANT;
	TextureArguments[eBLEND_SOURCE_DIFFUSE] = eGL_BLEND_SOURCE_PRIMARY_COLOR;
	TextureArguments[eBLEND_SOURCE_PREVIOUS] = eGL_BLEND_SOURCE_PREVIOUS;

	RgbBlendFuncs[eRGB_BLEND_FUNC_NONE] = eGL_BLEND_FUNC_MODULATE;
	RgbBlendFuncs[eRGB_BLEND_FUNC_FIRST_ARG] = eGL_BLEND_FUNC_REPLACE;
	RgbBlendFuncs[eRGB_BLEND_FUNC_ADD] = eGL_BLEND_FUNC_ADD;
	RgbBlendFuncs[eRGB_BLEND_FUNC_ADD_SIGNED] = eGL_BLEND_FUNC_ADD_SIGNED;
	RgbBlendFuncs[eRGB_BLEND_FUNC_MODULATE] = eGL_BLEND_FUNC_MODULATE;
	RgbBlendFuncs[eRGB_BLEND_FUNC_INTERPOLATE] = eGL_BLEND_FUNC_INTERPOLATE;
	RgbBlendFuncs[eRGB_BLEND_FUNC_SUBTRACT] = eGL_BLEND_FUNC_SUBTRACT;
	RgbBlendFuncs[eRGB_BLEND_FUNC_DOT3_RGB] = eGL_BLEND_FUNC_DOT3_RGB;
	RgbBlendFuncs[eRGB_BLEND_FUNC_DOT3_RGBA] = eGL_BLEND_FUNC_DOT3_RGBA;

	AlphaBlendFuncs[eALPHA_BLEND_FUNC_NONE] = eGL_BLEND_FUNC_MODULATE;
	AlphaBlendFuncs[eALPHA_BLEND_FUNC_FIRST_ARG] = eGL_BLEND_FUNC_REPLACE;
	AlphaBlendFuncs[eALPHA_BLEND_FUNC_ADD] = eGL_BLEND_FUNC_ADD;
	AlphaBlendFuncs[eALPHA_BLEND_FUNC_ADD_SIGNED] = eGL_BLEND_FUNC_ADD_SIGNED;
	AlphaBlendFuncs[eALPHA_BLEND_FUNC_MODULATE] = eGL_BLEND_FUNC_MODULATE;
	AlphaBlendFuncs[eALPHA_BLEND_FUNC_INTERPOLATE] = eGL_BLEND_FUNC_INTERPOLATE;
	AlphaBlendFuncs[eALPHA_BLEND_FUNC_SUBSTRACT] = eGL_BLEND_FUNC_SUBTRACT;

	BlendOps[eBLEND_OP_ADD] = eGL_BLEND_OP_ADD;
	BlendOps[eBLEND_OP_SUBSTRACT] = eGL_BLEND_OP_SUBTRACT;
	BlendOps[eBLEND_OP_REV_SUBSTRACT] = eGL_BLEND_OP_REV_SUBTRACT;
	BlendOps[eBLEND_OP_MIN] = eGL_BLEND_OP_MIN;
	BlendOps[eBLEND_OP_MAX] = eGL_BLEND_OP_MAX;

	PixelFormats[ePIXEL_FORMAT_L8] = PixelFmt( eGL_FORMAT_RED, eGL_INTERNAL_R8, eGL_TYPE_UNSIGNED_BYTE );
	PixelFormats[ePIXEL_FORMAT_L16F32F] = PixelFmt( eGL_FORMAT_RED, eGL_INTERNAL_R16F, eGL_TYPE_FLOAT );
	PixelFormats[ePIXEL_FORMAT_L32F] = PixelFmt( eGL_FORMAT_RED, eGL_INTERNAL_R32F, eGL_TYPE_FLOAT );
	PixelFormats[ePIXEL_FORMAT_A8L8] = PixelFmt( eGL_FORMAT_RG, eGL_INTERNAL_RG16, eGL_TYPE_UNSIGNED_BYTE );
	PixelFormats[ePIXEL_FORMAT_AL16F32F] = PixelFmt( eGL_FORMAT_RG, eGL_INTERNAL_RG16F, eGL_TYPE_FLOAT );
	PixelFormats[ePIXEL_FORMAT_AL32F] = PixelFmt( eGL_FORMAT_RG, eGL_INTERNAL_RG32F, eGL_TYPE_FLOAT );
	PixelFormats[ePIXEL_FORMAT_A1R5G5B5] = PixelFmt( eGL_FORMAT_BGRA, eGL_INTERNAL_RGB5_A1, eGL_TYPE_UNSIGNED_SHORT_1_5_5_5_REV );
	PixelFormats[ePIXEL_FORMAT_A4R4G4B4] = PixelFmt( eGL_FORMAT_BGRA, eGL_INTERNAL_RGBA4, eGL_TYPE_UNSIGNED_SHORT_4_4_4_4_REV );
	PixelFormats[ePIXEL_FORMAT_R5G6B5] = PixelFmt( eGL_FORMAT_BGR, eGL_INTERNAL_RGB16, eGL_TYPE_UNSIGNED_SHORT_5_6_5_REV );
	PixelFormats[ePIXEL_FORMAT_R8G8B8] = PixelFmt( eGL_FORMAT_BGR, eGL_INTERNAL_RGB8, eGL_TYPE_UNSIGNED_BYTE );
	PixelFormats[ePIXEL_FORMAT_B8G8R8] = PixelFmt( eGL_FORMAT_RGB, eGL_INTERNAL_RGB8, eGL_TYPE_UNSIGNED_BYTE );
	PixelFormats[ePIXEL_FORMAT_A8R8G8B8] = PixelFmt( eGL_FORMAT_BGRA, eGL_INTERNAL_RGBA8, eGL_TYPE_UNSIGNED_BYTE );
	PixelFormats[ePIXEL_FORMAT_A8B8G8R8] = PixelFmt( eGL_FORMAT_RGBA, eGL_INTERNAL_RGBA8, eGL_TYPE_UNSIGNED_BYTE );
	PixelFormats[ePIXEL_FORMAT_RGB16F32F] = PixelFmt( eGL_FORMAT_BGR, eGL_INTERNAL_RGB16F, eGL_TYPE_FLOAT );
	PixelFormats[ePIXEL_FORMAT_ARGB16F32F] = PixelFmt( eGL_FORMAT_BGRA, eGL_INTERNAL_RGBA16F, eGL_TYPE_FLOAT );
	PixelFormats[ePIXEL_FORMAT_RGB32F] = PixelFmt( eGL_FORMAT_BGR, eGL_INTERNAL_RGB32F, eGL_TYPE_FLOAT );
	PixelFormats[ePIXEL_FORMAT_ARGB32F] = PixelFmt( eGL_FORMAT_BGRA, eGL_INTERNAL_RGBA32F, eGL_TYPE_FLOAT );
	PixelFormats[ePIXEL_FORMAT_DXTC1] = PixelFmt( eGL_FORMAT_BGR, eGL_INTERNAL_COMPRESSED_RGBA_S3TC_DXT1, eGL_TYPE_DEFAULT );
	PixelFormats[ePIXEL_FORMAT_DXTC3] = PixelFmt( eGL_FORMAT_BGRA, eGL_INTERNAL_COMPRESSED_RGBA_S3TC_DXT3, eGL_TYPE_DEFAULT );
	PixelFormats[ePIXEL_FORMAT_DXTC5] = PixelFmt( eGL_FORMAT_RGBA, eGL_INTERNAL_COMPRESSED_RGBA_S3TC_DXT5, eGL_TYPE_DEFAULT );
	PixelFormats[ePIXEL_FORMAT_YUY2] = PixelFmt( eGL_FORMAT( 0 ), eGL_INTERNAL( 0 ), eGL_TYPE_DEFAULT );
	PixelFormats[ePIXEL_FORMAT_DEPTH16] = PixelFmt( eGL_FORMAT_DEPTH, eGL_INTERNAL_DEPTH_COMPONENT16, eGL_TYPE_UNSIGNED_BYTE );
	PixelFormats[ePIXEL_FORMAT_DEPTH24] = PixelFmt( eGL_FORMAT_DEPTH, eGL_INTERNAL_DEPTH_COMPONENT24, eGL_TYPE_UNSIGNED_BYTE );
	PixelFormats[ePIXEL_FORMAT_DEPTH24S8] = PixelFmt( eGL_FORMAT_DEPTH_STENCIL, eGL_INTERNAL_DEPTH24_STENCIL8, eGL_TYPE_UNSIGNED_INT_24_8 );
	PixelFormats[ePIXEL_FORMAT_DEPTH32] = PixelFmt( eGL_FORMAT_DEPTH, eGL_INTERNAL_DEPTH_COMPONENT32, eGL_TYPE_UNSIGNED_BYTE );
	PixelFormats[ePIXEL_FORMAT_STENCIL1] = PixelFmt( eGL_FORMAT_STENCIL, eGL_INTERNAL_STENCIL_INDEX1, eGL_TYPE_UNSIGNED_BYTE );
	PixelFormats[ePIXEL_FORMAT_STENCIL8] = PixelFmt( eGL_FORMAT_STENCIL, eGL_INTERNAL_STENCIL_INDEX8, eGL_TYPE_UNSIGNED_BYTE );

	ShaderTypes[eSHADER_TYPE_VERTEX] = eGL_SHADER_TYPE_VERTEX;
	ShaderTypes[eSHADER_TYPE_HULL] = eGL_SHADER_TYPE_TESS_CONTROL;
	ShaderTypes[eSHADER_TYPE_DOMAIN] = eGL_SHADER_TYPE_TESS_EVALUATION;
	ShaderTypes[eSHADER_TYPE_GEOMETRY] = eGL_SHADER_TYPE_GEOMETRY;
	ShaderTypes[eSHADER_TYPE_PIXEL] = eGL_SHADER_TYPE_FRAGMENT;
	ShaderTypes[eSHADER_TYPE_COMPUTE] = eGL_SHADER_TYPE_COMPUTE;

	Internals[ePIXEL_FORMAT_L8] = eGL_INTERNAL_FORMAT_R16;
	Internals[ePIXEL_FORMAT_L16F32F] = eGL_INTERNAL_FORMAT_R16F;
	Internals[ePIXEL_FORMAT_L32F] = eGL_INTERNAL_FORMAT_R32F;
	Internals[ePIXEL_FORMAT_A8L8] = eGL_INTERNAL_FORMAT_RG16;
	Internals[ePIXEL_FORMAT_AL16F32F] = eGL_INTERNAL_FORMAT_RG16F;
	Internals[ePIXEL_FORMAT_AL32F] = eGL_INTERNAL_FORMAT_RG32F;
	Internals[ePIXEL_FORMAT_A1R5G5B5] = eGL_INTERNAL_FORMAT_RGBA16UI;
	Internals[ePIXEL_FORMAT_A4R4G4B4] = eGL_INTERNAL_FORMAT_RGBA16UI;
	Internals[ePIXEL_FORMAT_R5G6B5] = eGL_INTERNAL_FORMAT_RGB16UI;
	Internals[ePIXEL_FORMAT_R8G8B8] = eGL_INTERNAL_FORMAT_RGBA32UI;
	Internals[ePIXEL_FORMAT_B8G8R8] = eGL_INTERNAL_FORMAT_RGBA32UI;
	Internals[ePIXEL_FORMAT_A8R8G8B8] = eGL_INTERNAL_FORMAT_RGBA32UI;
	Internals[ePIXEL_FORMAT_A8B8G8R8] = eGL_INTERNAL_FORMAT_RGBA32UI;
	Internals[ePIXEL_FORMAT_RGB16F32F] = eGL_INTERNAL_FORMAT_RGBA16F;
	Internals[ePIXEL_FORMAT_ARGB16F32F] = eGL_INTERNAL_FORMAT_RGBA16F;
	Internals[ePIXEL_FORMAT_RGB32F] = eGL_INTERNAL_FORMAT_RGBA32F;
	Internals[ePIXEL_FORMAT_ARGB32F] = eGL_INTERNAL_FORMAT_RGBA32F;

	Attachments[eATTACHMENT_POINT_NONE] = eGL_TEXTURE_ATTACHMENT_NONE;
	Attachments[eATTACHMENT_POINT_COLOUR] = eGL_TEXTURE_ATTACHMENT_COLOR0;
	Attachments[eATTACHMENT_POINT_DEPTH] = eGL_TEXTURE_ATTACHMENT_DEPTH;
	Attachments[eATTACHMENT_POINT_STENCIL] = eGL_TEXTURE_ATTACHMENT_STENCIL;

	FramebufferModes[eFRAMEBUFFER_TARGET_DRAW] = eGL_FRAMEBUFFER_MODE_DRAW;
	FramebufferModes[eFRAMEBUFFER_TARGET_READ] = eGL_FRAMEBUFFER_MODE_READ;
	FramebufferModes[eFRAMEBUFFER_TARGET_BOTH] = eGL_FRAMEBUFFER_MODE_DEFAULT;

	RboAttachments[eATTACHMENT_POINT_NONE] = eGL_RENDERBUFFER_ATTACHMENT_NONE;
	RboAttachments[eATTACHMENT_POINT_COLOUR] = eGL_RENDERBUFFER_ATTACHMENT_COLOR0;
	RboAttachments[eATTACHMENT_POINT_DEPTH] = eGL_RENDERBUFFER_ATTACHMENT_DEPTH;
	RboAttachments[eATTACHMENT_POINT_STENCIL] = eGL_RENDERBUFFER_ATTACHMENT_STENCIL;

	RboStorages[ePIXEL_FORMAT_L8] = eGL_RENDERBUFFER_STORAGE_L8;
	RboStorages[ePIXEL_FORMAT_L16F32F] = eGL_RENDERBUFFER_STORAGE_L16F;
	RboStorages[ePIXEL_FORMAT_L32F] = eGL_RENDERBUFFER_STORAGE_L32F;
	RboStorages[ePIXEL_FORMAT_A8L8] = eGL_RENDERBUFFER_STORAGE_A8L8;
	RboStorages[ePIXEL_FORMAT_AL16F32F] = eGL_RENDERBUFFER_STORAGE_AL16F;
	RboStorages[ePIXEL_FORMAT_AL32F] = eGL_RENDERBUFFER_STORAGE_AL32F;
	RboStorages[ePIXEL_FORMAT_A1R5G5B5] = eGL_RENDERBUFFER_STORAGE_RGB5_A1;
	RboStorages[ePIXEL_FORMAT_A4R4G4B4] = eGL_RENDERBUFFER_STORAGE_RGBA4;
	RboStorages[ePIXEL_FORMAT_R5G6B5] = eGL_RENDERBUFFER_STORAGE_RGB565;
	RboStorages[ePIXEL_FORMAT_R8G8B8] = eGL_RENDERBUFFER_STORAGE_RGB8;
	RboStorages[ePIXEL_FORMAT_B8G8R8] = eGL_RENDERBUFFER_STORAGE_RGB8;
	RboStorages[ePIXEL_FORMAT_A8R8G8B8] = eGL_RENDERBUFFER_STORAGE_RGBA8;
	RboStorages[ePIXEL_FORMAT_A8B8G8R8] = eGL_RENDERBUFFER_STORAGE_RGBA8;
	RboStorages[ePIXEL_FORMAT_RGB16F32F] = eGL_RENDERBUFFER_STORAGE_RGB16F;
	RboStorages[ePIXEL_FORMAT_ARGB16F32F] = eGL_RENDERBUFFER_STORAGE_RGBA16F;
	RboStorages[ePIXEL_FORMAT_RGB32F] = eGL_RENDERBUFFER_STORAGE_RGB32F;
	RboStorages[ePIXEL_FORMAT_ARGB32F] = eGL_RENDERBUFFER_STORAGE_RGBA32F;
	RboStorages[ePIXEL_FORMAT_DXTC1] = eGL_RENDERBUFFER_STORAGE_DXTC1;
	RboStorages[ePIXEL_FORMAT_DXTC3] = eGL_RENDERBUFFER_STORAGE_DXTC3;
	RboStorages[ePIXEL_FORMAT_DXTC5] = eGL_RENDERBUFFER_STORAGE_DXTC5;
	RboStorages[ePIXEL_FORMAT_YUY2] = eGL_RENDERBUFFER_STORAGE( 0 );
	RboStorages[ePIXEL_FORMAT_DEPTH16] = eGL_RENDERBUFFER_STORAGE_DEPTH16;
	RboStorages[ePIXEL_FORMAT_DEPTH24] = eGL_RENDERBUFFER_STORAGE_DEPTH24;
	RboStorages[ePIXEL_FORMAT_DEPTH24S8] = eGL_RENDERBUFFER_STORAGE_DEPTH24;
	RboStorages[ePIXEL_FORMAT_DEPTH32] = eGL_RENDERBUFFER_STORAGE_DEPTH32;
	RboStorages[ePIXEL_FORMAT_STENCIL1] = eGL_RENDERBUFFER_STORAGE_STENCIL1;
	RboStorages[ePIXEL_FORMAT_STENCIL8] = eGL_RENDERBUFFER_STORAGE_STENCIL8;

	Tweaks[eTWEAK_DEPTH_TEST] = eGL_TWEAK_DEPTH_TEST;
	Tweaks[eTWEAK_STENCIL_TEST] = eGL_TWEAK_STENCIL_TEST;
	Tweaks[eTWEAK_BLEND] = eGL_TWEAK_BLEND;
	Tweaks[eTWEAK_LIGHTING] = eGL_TWEAK_LIGHTING;
	Tweaks[eTWEAK_ALPHA_TEST] = eGL_TWEAK_ALPHA_TEST;
	Tweaks[eTWEAK_CULLING] = eGL_TWEAK_CULL_FACE;
	Tweaks[eTWEAK_DITHERING] = eGL_TWEAK_DITHER;
	Tweaks[eTWEAK_FOG] = eGL_TWEAK_FOG;
	Tweaks[eTWEAK_DEPTH_WRITE] = eGL_TWEAK( -1 );
	Tweaks[eTWEAK_ALPHA_TO_COVERAGE] = eGL_TWEAK_ALPHA_TO_COVERAGE;

	Buffers[eBUFFER_NONE] = eGL_BUFFER_NONE;
	Buffers[eBUFFER_FRONT_LEFT] = eGL_BUFFER_FRONT_LEFT;
	Buffers[eBUFFER_FRONT_RIGHT] = eGL_BUFFER_FRONT_RIGHT;
	Buffers[eBUFFER_BACK_LEFT] = eGL_BUFFER_BACK_LEFT;
	Buffers[eBUFFER_BACK_RIGHT] = eGL_BUFFER_BACK_RIGHT;
	Buffers[eBUFFER_FRONT] = eGL_BUFFER_FRONT;
	Buffers[eBUFFER_BACK] = eGL_BUFFER_BACK;
	Buffers[eBUFFER_LEFT] = eGL_BUFFER_LEFT;
	Buffers[eBUFFER_RIGHT] = eGL_BUFFER_RIGHT;
	Buffers[eBUFFER_FRONT_AND_BACK] = eGL_BUFFER_FRONT_AND_BACK;

	Faces[eFACE_NONE] = eGL_FACE( 0 );
	Faces[eFACE_FRONT] = eGL_FACE_FRONT;
	Faces[eFACE_BACK] = eGL_FACE_BACK;
	Faces[eFACE_FRONT_AND_BACK] = eGL_FACE_FRONT_AND_BACK;

	FillModes[eFILL_MODE_POINT] = eGL_FILL_MODE_POINT;
	FillModes[eFILL_MODE_LINE] = eGL_FILL_MODE_LINE;
	FillModes[eFILL_MODE_SOLID] = eGL_FILL_MODE_FILL;

	DepthFuncs[eDEPTH_FUNC_NEVER] = eGL_FUNC_NEVER;
	DepthFuncs[eDEPTH_FUNC_LESS] = eGL_FUNC_LESS;
	DepthFuncs[eDEPTH_FUNC_EQUAL] = eGL_FUNC_LEQUAL;
	DepthFuncs[eDEPTH_FUNC_LEQUAL] = eGL_FUNC_EQUAL;
	DepthFuncs[eDEPTH_FUNC_GREATER] = eGL_FUNC_NOTEQUAL;
	DepthFuncs[eDEPTH_FUNC_NOTEQUAL] = eGL_FUNC_GEQUAL;
	DepthFuncs[eDEPTH_FUNC_GEQUAL] = eGL_FUNC_GREATER;
	DepthFuncs[eDEPTH_FUNC_ALWAYS] = eGL_FUNC_ALWAYS;

	WriteMasks[eWRITING_MASK_ZERO] = false;
	WriteMasks[eWRITING_MASK_ALL] = true;

	StencilFuncs[eSTENCIL_FUNC_NEVER] = eGL_FUNC_NEVER;
	StencilFuncs[eSTENCIL_FUNC_LESS] = eGL_FUNC_LESS;
	StencilFuncs[eSTENCIL_FUNC_EQUAL] = eGL_FUNC_LEQUAL;
	StencilFuncs[eSTENCIL_FUNC_LEQUAL] = eGL_FUNC_EQUAL;
	StencilFuncs[eSTENCIL_FUNC_GREATER] = eGL_FUNC_NOTEQUAL;
	StencilFuncs[eSTENCIL_FUNC_NOTEQUAL] = eGL_FUNC_GEQUAL;
	StencilFuncs[eSTENCIL_FUNC_GEQUAL] = eGL_FUNC_GREATER;
	StencilFuncs[eSTENCIL_FUNC_ALWAYS] = eGL_FUNC_ALWAYS;

	StencilOps[eSTENCIL_OP_KEEP] = eGL_STENCIL_OP_KEEP;
	StencilOps[eSTENCIL_OP_ZERO] = eGL_STENCIL_OP_ZERO;
	StencilOps[eSTENCIL_OP_REPLACE] = eGL_STENCIL_OP_REPLACE;
	StencilOps[eSTENCIL_OP_INCR] = eGL_STENCIL_OP_INCR;
	StencilOps[eSTENCIL_OP_INCR_WRAP] = eGL_STENCIL_OP_INCR_WRAP;
	StencilOps[eSTENCIL_OP_DECR] = eGL_STENCIL_OP_DECR;
	StencilOps[eSTENCIL_OP_DECR_WRAP] = eGL_STENCIL_OP_DECR_WRAP;
	StencilOps[eSTENCIL_OP_INVERT] = eGL_STENCIL_OP_INVERT;

	BuffersTA[eGL_TEXTURE_ATTACHMENT_NONE] = eGL_BUFFER_NONE;
	BuffersTA[eGL_TEXTURE_ATTACHMENT_COLOR0] = eGL_BUFFER_COLOR0;

	BuffersRBA[eGL_RENDERBUFFER_ATTACHMENT_NONE] = eGL_BUFFER_NONE;
	BuffersRBA[eGL_RENDERBUFFER_ATTACHMENT_COLOR0] = eGL_BUFFER_COLOR0;

	Cleanup();
}

OpenGl::~OpenGl()
{
}

bool OpenGl::PreInitialise( String const & p_strExtensions )
{
	char const * l_extensions = ( char const * )glGetString( GL_EXTENSIONS );
	m_extensions = string::string_cast< xchar >( l_extensions ) + p_strExtensions;
	m_vendor = string::string_cast< xchar >( ( char const * )glGetString( GL_VENDOR ) );
	m_renderer = string::string_cast< xchar >( ( char const * )glGetString( GL_RENDERER ) );
	m_version = string::string_cast< xchar >( ( char const * )glGetString( GL_VERSION ) );
	String l_strExtensions = m_extensions;
	double l_version;
	StringStream l_stream( m_version );
	l_stream >> l_version;
	m_iVersion = int( l_version * 10 );

	if ( m_iVersion >= 43 )
	{
		m_iGlslVersion = 430;
	}
	else if ( m_iVersion >= 42 )
	{
		m_iGlslVersion = 420;
	}
	else if ( m_iVersion >= 41 )
	{
		m_iGlslVersion = 410;
	}
	else if ( m_iVersion >= 40 )
	{
		m_iGlslVersion = 400;
	}
	else if ( m_iVersion >= 33 )
	{
		m_iGlslVersion = 330;
	}
	else if ( m_iVersion >= 32 )
	{
		m_iGlslVersion = 150;
	}
	else if ( m_iVersion >= 31 )
	{
		m_iGlslVersion = 140;
	}
	else if ( m_iVersion >= 30 )
	{
		m_iGlslVersion = 130;
	}
	else if ( m_iVersion >= 21 )
	{
		m_iGlslVersion = 120;
	}
	else if ( m_iVersion >= 20 )
	{
		m_iGlslVersion = 110;
	}

#if defined( _WIN32 )
	m_pfnMakeCurrent = &wglMakeCurrent;
	m_pfnCreateContext = &wglCreateContext;
	m_pfnDeleteContext = &wglDeleteContext;

	if ( HasExtension( ARB_create_context ) )
	{
		gl_api::GetFunction( cuT( "wglCreateContextAttribsARB" ), m_pfnCreateContextAttribs );
	}

#elif defined( __linux__ )
	m_pfnMakeCurrent = &glXMakeCurrent;
	m_pfnCreateContext = &glXCreateContext;
	m_pfnDeleteContext = &glXDestroyContext;

	if ( HasExtension( ARB_create_context ) )
	{
		gl_api::GetFunction( cuT( "glXCreateContextAttribsARB" ), m_pfnCreateContextAttribs );
	}
	else
	{
		gl_api::GetFunction( cuT( "glXCreateContextAttribsARB" ), m_pfnCreateContextAttribs );
	}

#else
#	error "Yet unsupported OS"
#endif
	return true;
}

bool OpenGl::Initialise()
{
	m_pfnGetError = &glGetError;
	m_pfnClearColor = &glClearColor;
	m_pfnShadeModel = &glShadeModel;
	m_pfnViewport = &glViewport;
	m_pfnClear = &glClear;
	m_pfnDrawArrays = &glDrawArrays;
	m_pfnDrawElements = &glDrawElements;
	m_pfnEnable = &glEnable;
	m_pfnDisable = &glDisable;
	m_pfnGenTextures = &glGenTextures;
	m_pfnDeleteTextures = &glDeleteTextures;
	m_pfnVertexPointer = &glVertexPointer;
	m_pfnColorPointer = &glColorPointer;
	m_pfnNormalPointer = &glNormalPointer;
	m_pfnTexCoordPointer = &glTexCoordPointer;
	m_pfnTexEnvi = &glTexEnvi;
	m_pfnTexEnviv = &glTexEnviv;
	m_pfnTexEnvf = &glTexEnvf;
	m_pfnTexEnvfv = &glTexEnvfv;
	m_pfnBlendFunc = &glBlendFunc;
	m_pfnAlphaFunc = &glAlphaFunc;
	m_pfnIsTexture = &glIsTexture;
	m_pfnTexGeni = &glTexGeni;
	m_pfnCullFace = &glCullFace;
	m_pfnFrontFace = &glFrontFace;
	m_pfnMaterialf = &glMaterialf;
	m_pfnMaterialfv = &glMaterialfv;
	m_pfnSelectBuffer = &glSelectBuffer;
	m_pfnGetIntegerv = &glGetIntegerv;
	m_pfnRenderMode = &glRenderMode;
	m_pfnReadBuffer = &glReadBuffer;
	m_pfnReadPixels = &glReadPixels;
	m_pfnDrawBuffer = &glDrawBuffer;
	m_pfnDrawPixels = &glDrawPixels;
	m_pfnPixelStorei = &glPixelStorei;
	m_pfnPixelStoref = &glPixelStoref;
	m_pfnDepthFunc = &glDepthFunc;
	m_pfnDepthMask = &glDepthMask;
	m_pfnColorMask = &glColorMask;
	m_pfnPolygonMode = &glPolygonMode;
	m_pfnHint = &glHint;
	m_pfnPolygonOffset = &glPolygonOffset;
	m_pfnStencilOp = &glStencilOp;
	m_pfnStencilFunc = &glStencilFunc;
	m_pfnStencilMask = &glStencilMask;
	GL_GET_FUNC( BlendEquation, EXT );
	GL_GET_FUNC( BlendFuncSeparate, EXT );
	GL_GET_FUNC( BlendColor, EXT );
	GL_GET_FUNC( StencilOpSeparate, ATI );
	GL_GET_FUNC( StencilFuncSeparate, ATI );
	GL_GET_FUNC( StencilMaskSeparate, );
	GL_GET_FUNC( ActiveTexture, ARB );
	GL_GET_FUNC( ClientActiveTexture, ARB );
#if defined( _WIN32 )
	m_pfnMakeCurrent = &wglMakeCurrent;
	m_pfnSwapBuffers = &::SwapBuffers;
	m_pfnCreateContext = &wglCreateContext;
	m_pfnDeleteContext = &wglDeleteContext;

	if ( HasExtension( ARB_create_context ) )
	{
		gl_api::GetFunction( cuT( "wglCreateContextAttribsARB" ), m_pfnCreateContextAttribs );
	}

	if ( HasExtension( EXT_swap_control ) )
	{
		gl_api::GetFunction( cuT( "wglSwapIntervalEXT" ), m_pfnSwapInterval );
	}

#elif defined( __linux__ )
	m_pfnMakeCurrent = &glXMakeCurrent;
	m_pfnSwapBuffers = &glXSwapBuffers;
	m_pfnCreateContext = &glXCreateContext;
	m_pfnDeleteContext = &glXDestroyContext;

	if ( HasExtension( ARB_create_context ) )
	{
		gl_api::GetFunction( cuT( "glXCreateContextAttribsARB" ), m_pfnCreateContextAttribs );
	}
	else
	{
		gl_api::GetFunction( cuT( "glXCreateContextAttribsARB" ), m_pfnCreateContextAttribs );
	}

	if ( HasExtension( EXT_swap_control ) )
	{
		gl_api::GetFunction( cuT( "glXSwapIntervalEXT" ), m_pfnSwapInterval );
	}
	else
	{
		gl_api::GetFunction( cuT( "glXSwapIntervalEXT" ), m_pfnSwapInterval );
	}

#else
#	error "Yet unsupported OS"
#endif
#if DEF_USE_DIRECT_STATE_ACCESS

	if ( HasExtension( EXT_direct_state_access ) )
	{
		m_pTexFunctions = new TexFunctionsDSA( *this );
		m_pBufFunctions = new BufFunctionsDSA( *this );
	}
	else
#endif
	{
		m_pTexFunctions = new TexFunctions( *this );
		m_pBufFunctions = new BufFunctions( *this );
	}

	if ( HasExtension( NV_shader_buffer_load ) )
	{
		GL_GET_FUNC( GetIntegerui64v, NV );
	}

	m_bBindVboToGpuAddress = HasExtension( NV_shader_buffer_load ) && HasExtension( NV_vertex_buffer_unified_memory );
	m_pBufFunctions->m_pfnEnableClientState = &glEnableClientState;
	m_pBufFunctions->m_pfnDisableClientState = &glDisableClientState;

	if ( m_iVersion >= 40 || HasExtension( ARB_draw_buffers_blend ) )
	{
		GL_GET_FUNC( BlendEquationi, ARB );
		GL_GET_FUNC( BlendFuncSeparatei, ARB );
	}
	else if ( HasExtension( AMD_draw_buffers_blend ) )
	{
		gl_api::GetFunction( cuT( "glBlendEquationIndexedAMD" ), m_pfnBlendEquationi );
		gl_api::GetFunction( cuT( "glBlendFuncIndexedAMD" ), m_pfnBlendFuncSeparatei );
	}

	m_bHasDepthClipping = HasExtension( EXT_clip_volume_hint );
	m_bHasNonPowerOfTwoTextures = HasExtension( ARB_texture_non_power_of_two );

	if ( HasExtension( ARB_texture_multisample ) )
	{
		GL_GET_FUNC( TexImage2DMultisample, );
		GL_GET_FUNC( SampleCoverage, ARB );
	}

	if ( HasExtension( ARB_imaging ) )
	{
	}

	if ( HasExtension( ARB_debug_output ) )
	{
		GL_GET_FUNC( DebugMessageCallback, ARB );
	}
	else if ( HasExtension( AMDX_debug_output ) )
	{
		GL_GET_FUNC( DebugMessageCallbackAMD, );
	}

	if ( HasExtension( ARB_vertex_buffer_object ) )
	{
		m_bHasVbo = true;
		GL_GET_FUNC( GenBuffers, ARB );
		GL_GET_FUNC( DeleteBuffers, ARB );
		GL_GET_FUNC( IsBuffer, ARB );

		if ( HasExtension( EXT_coordinate_frame ) )
		{
			GL_GET_FUNC( TangentPointer, EXT );
			GL_GET_FUNC( BinormalPointer, EXT );
		}

		if ( HasExtension( ARB_pixel_buffer_object ) )
		{
			m_bHasPbo = true;
		}
	}

	if ( HasExtension( ARB_vertex_array_object ) )
	{
		m_bHasVao = true;
		GL_GET_FUNC( GenVertexArrays, );
		GL_GET_FUNC( BindVertexArray, );
		GL_GET_FUNC( IsVertexArray, );
		GL_GET_FUNC( DeleteVertexArrays, );
	}

	if ( HasExtension( ARB_texture_buffer_object ) )
	{
		m_bHasTbo = true;
		GL_GET_FUNC( TexBuffer, EXT );
	}

	if ( HasExtension( ARB_framebuffer_object ) )
	{
		m_bHasFbo = true;
		GL_GET_FUNC( DrawBuffers, ARB );
		GL_GET_FUNC( BlitFramebuffer, ARB );
		GL_GET_FUNC( GenRenderbuffers, ARB );
		GL_GET_FUNC( DeleteRenderbuffers, ARB );
		GL_GET_FUNC( IsRenderbuffer, ARB );
		GL_GET_FUNC( BindRenderbuffer, ARB );
		GL_GET_FUNC( RenderbufferStorage, ARB );
		GL_GET_FUNC( RenderbufferStorageMultisample, ARB );
		GL_GET_FUNC( GetRenderbufferParameteriv, ARB );
		GL_GET_FUNC( FramebufferRenderbuffer, ARB );
		GL_GET_FUNC( GenFramebuffers, ARB );
		GL_GET_FUNC( DeleteFramebuffers, ARB );
		GL_GET_FUNC( IsFramebuffer, ARB );
		GL_GET_FUNC( BindFramebuffer, ARB );
		GL_GET_FUNC( FramebufferTexture, ARB );
		GL_GET_FUNC( FramebufferTexture1D, ARB );
		GL_GET_FUNC( FramebufferTexture2D, ARB );
		GL_GET_FUNC( FramebufferTexture3D, ARB );
		GL_GET_FUNC( FramebufferTextureLayer, ARB );
		GL_GET_FUNC( CheckFramebufferStatus, ARB );
	}
	else if ( HasExtension( EXT_framebuffer_object ) )
	{
		m_bHasFbo = true;
		GL_GET_FUNC( DrawBuffers, EXT );
		GL_GET_FUNC( BlitFramebuffer, EXT );
		GL_GET_FUNC( GenRenderbuffers, EXT );
		GL_GET_FUNC( DeleteRenderbuffers, EXT );
		GL_GET_FUNC( IsRenderbuffer, EXT );
		GL_GET_FUNC( BindRenderbuffer, EXT );
		GL_GET_FUNC( RenderbufferStorage, EXT );
		GL_GET_FUNC( RenderbufferStorageMultisample, EXT );
		GL_GET_FUNC( GetRenderbufferParameteriv, EXT );
		GL_GET_FUNC( FramebufferRenderbuffer, EXT );
		GL_GET_FUNC( GenFramebuffers, EXT );
		GL_GET_FUNC( DeleteFramebuffers, EXT );
		GL_GET_FUNC( IsFramebuffer, EXT );
		GL_GET_FUNC( BindFramebuffer, EXT );
		GL_GET_FUNC( FramebufferTexture, EXT );
		GL_GET_FUNC( FramebufferTexture1D, EXT );
		GL_GET_FUNC( FramebufferTexture2D, EXT );
		GL_GET_FUNC( FramebufferTexture3D, EXT );
		GL_GET_FUNC( FramebufferTextureLayer, EXT );
		GL_GET_FUNC( CheckFramebufferStatus, EXT );
	}

	if ( HasExtension( ARB_sampler_objects ) )
	{
		m_bHasSpl = true;
		GL_GET_FUNC( GenSamplers, );
		GL_GET_FUNC( DeleteSamplers, );
		GL_GET_FUNC( IsSampler, );
		GL_GET_FUNC( BindSampler, );
		GL_GET_FUNC( GetSamplerParameteruiv, );
		GL_GET_FUNC( GetSamplerParameterfv, );
		GL_GET_FUNC( GetSamplerParameteriv, );
		GL_GET_FUNC( SamplerParameteruiv, );
		GL_GET_FUNC( SamplerParameterf, );
		GL_GET_FUNC( SamplerParameterfv, );
		GL_GET_FUNC( SamplerParameteri, );
		GL_GET_FUNC( SamplerParameteriv, );

		if ( HasExtension( EXT_texture_filter_anisotropic ) )
		{
			m_bHasAnisotropic = true;
		}
	}

	if ( HasExtension( ARB_draw_instanced ) )
	{
		m_bHasInstancedDraw = true;
		GL_GET_FUNC( DrawArraysInstanced, ARB );
		GL_GET_FUNC( DrawElementsInstanced, ARB );

		if ( HasExtension( ARB_instanced_arrays ) )
		{
			m_bHasInstancedArrays = true;
			GL_GET_FUNC( VertexAttribDivisor, ARB );
		}
	}
	else if ( HasExtension( EXT_draw_instanced ) )
	{
		m_bHasInstancedDraw = true;
		GL_GET_FUNC( DrawArraysInstanced, EXT );
		GL_GET_FUNC( DrawElementsInstanced, EXT );

		if ( HasExtension( EXT_instanced_arrays ) )
		{
			m_bHasInstancedArrays = true;
			GL_GET_FUNC( VertexAttribDivisor, EXT );
		}
	}

	if ( HasExtension( ARB_vertex_program ) )
	{
		m_bHasVSh = true;
		GL_GET_FUNC( CreateShader, );
		GL_GET_FUNC( DeleteShader, );
		GL_GET_FUNC( IsShader, ARB );
		GL_GET_FUNC( AttachShader, );
		GL_GET_FUNC( DetachShader, );
		GL_GET_FUNC( CompileShader, ARB );
		GL_GET_FUNC( GetShaderiv, );
		GL_GET_FUNC( GetShaderInfoLog, );
		GL_GET_FUNC( ShaderSource, ARB );
		GL_GET_FUNC( CreateProgram, );
		GL_GET_FUNC( DeleteProgram, );
		GL_GET_FUNC( LinkProgram, ARB );
		GL_GET_FUNC( UseProgram, );
		GL_GET_FUNC( GetProgramiv, ARB );
		GL_GET_FUNC( GetProgramInfoLog, );
		GL_GET_FUNC( GetAttribLocation, ARB );
		GL_GET_FUNC( IsProgram, ARB );
		GL_GET_FUNC( EnableVertexAttribArray, ARB );
		GL_GET_FUNC( VertexAttribPointer, ARB );
		GL_GET_FUNC( VertexAttribIPointer, ARB );
		GL_GET_FUNC( DisableVertexAttribArray, ARB );
		GL_GET_FUNC( ProgramParameteri, ARB );
		GL_GET_FUNC( GetUniformLocation, ARB );

		if ( HasExtension( ARB_fragment_program ) )
		{
			m_bHasPSh = true;
			GL_GET_FUNC( Uniform1i, ARB );
			GL_GET_FUNC( Uniform2i, ARB );
			GL_GET_FUNC( Uniform3i, ARB );
			GL_GET_FUNC( Uniform4i, ARB );
			GL_GET_FUNC( Uniform1iv, ARB );
			GL_GET_FUNC( Uniform2iv, ARB );
			GL_GET_FUNC( Uniform3iv, ARB );
			GL_GET_FUNC( Uniform4iv, ARB );
			GL_GET_FUNC( Uniform1ui, EXT );
			GL_GET_FUNC( Uniform2ui, EXT );
			GL_GET_FUNC( Uniform3ui, EXT );
			GL_GET_FUNC( Uniform4ui, EXT );
			GL_GET_FUNC( Uniform1uiv, EXT );
			GL_GET_FUNC( Uniform2uiv, EXT );
			GL_GET_FUNC( Uniform3uiv, EXT );
			GL_GET_FUNC( Uniform4uiv, EXT );
			GL_GET_FUNC( Uniform1f, ARB );
			GL_GET_FUNC( Uniform2f, ARB );
			GL_GET_FUNC( Uniform3f, ARB );
			GL_GET_FUNC( Uniform4f, ARB );
			GL_GET_FUNC( Uniform1fv, ARB );
			GL_GET_FUNC( Uniform2fv, ARB );
			GL_GET_FUNC( Uniform3fv, ARB );
			GL_GET_FUNC( Uniform4fv, ARB );
			GL_GET_FUNC( Uniform1d, );
			GL_GET_FUNC( Uniform2d, );
			GL_GET_FUNC( Uniform3d, );
			GL_GET_FUNC( Uniform4d, );
			GL_GET_FUNC( Uniform1dv, );
			GL_GET_FUNC( Uniform2dv, );
			GL_GET_FUNC( Uniform3dv, );
			GL_GET_FUNC( Uniform4dv, );
			GL_GET_FUNC( UniformMatrix2fv, ARB );
			GL_GET_FUNC( UniformMatrix2x3fv, );
			GL_GET_FUNC( UniformMatrix2x4fv, );
			GL_GET_FUNC( UniformMatrix3fv, ARB );
			GL_GET_FUNC( UniformMatrix3x2fv, );
			GL_GET_FUNC( UniformMatrix3x4fv, );
			GL_GET_FUNC( UniformMatrix4fv, ARB );
			GL_GET_FUNC( UniformMatrix4x2fv, );
			GL_GET_FUNC( UniformMatrix4x3fv, );
			GL_GET_FUNC( UniformMatrix2dv, );
			GL_GET_FUNC( UniformMatrix2x3dv, );
			GL_GET_FUNC( UniformMatrix2x4dv, );
			GL_GET_FUNC( UniformMatrix3dv, );
			GL_GET_FUNC( UniformMatrix3x2dv, );
			GL_GET_FUNC( UniformMatrix3x4dv, );
			GL_GET_FUNC( UniformMatrix4dv, );
			GL_GET_FUNC( UniformMatrix4x2dv, );
			GL_GET_FUNC( UniformMatrix4x3dv, );

			if ( HasExtension( ARB_uniform_buffer_object ) )
			{
				m_bHasUbo = m_iGlslVersion >= 140;
				GL_GET_FUNC( GetUniformBlockIndex, );
				GL_GET_FUNC( BindBufferBase, EXT );
				GL_GET_FUNC( UniformBlockBinding, );
				GL_GET_FUNC( GetUniformIndices, );
				GL_GET_FUNC( GetActiveUniformsiv, );
				GL_GET_FUNC( GetActiveUniformBlockiv, );
			}

			if ( HasExtension( ARB_geometry_shader4 ) || HasExtension( EXT_geometry_shader4 ) )
			{
				m_bHasGSh = true;

				if ( HasExtension( ARB_tessellation_shader ) )
				{
					m_bHasTSh = true;
					GL_GET_FUNC( PatchParameteri, );

					if ( HasExtension( ARB_compute_shader ) )
					{
						m_bHasCSh = true;
					}
				}
			}
		}
	}

	HasExtension( ARB_timer_query );

	GL_GET_FUNC( GenQueries, ARB );
	GL_GET_FUNC( DeleteQueries, ARB );
	GL_GET_FUNC( IsQuery, ARB );
	GL_GET_FUNC( BeginQuery, ARB );
	GL_GET_FUNC( EndQuery, ARB );
	GL_GET_FUNC( QueryCounter, ARB );
	GL_GET_FUNC( GetQueryObjectiv, ARB );
	GL_GET_FUNC( GetQueryObjectuiv, ARB );
	GL_GET_FUNC( GetQueryObjecti64v, ARB );
	GL_GET_FUNC( GetQueryObjectui64v, ARB );

	return true;
}

void OpenGl::Cleanup()
{
	delete m_pTexFunctions;
	m_pTexFunctions = NULL;
	delete m_pBufFunctions;
	m_pBufFunctions = NULL;
	m_bHasVao = false;
	m_bHasUbo = false;
	m_bHasPbo = false;
	m_bHasTbo = false;
	m_bHasFbo = false;
	m_bHasVbo = false;
	m_bHasVSh = false;
	m_bHasPSh = false;
	m_bHasGSh = false;
	m_bHasTSh = false;
	m_bHasCSh = false;
	m_bHasSpl = false;
	m_bHasDepthClipping = false;
	m_bHasNonPowerOfTwoTextures = false;
	m_bBindVboToGpuAddress = false;
	m_iGlslVersion = 0;
	m_iVersion = 0;
}

uint32_t OpenGl::GetError()const
{
	return m_pfnGetError();
}

bool OpenGl::ClearColor( float red, float green, float blue, float alpha )const
{
	m_pfnClearColor( red, green, blue, alpha );
	return glCheckError( *this, "glClearColor" );
}

bool OpenGl::ClearColor( Castor::Colour const & p_colour )const
{
	m_pfnClearColor( p_colour.red().value(), p_colour.green().value(), p_colour.blue().value(), p_colour.alpha().value() );
	return glCheckError( *this, "glClearColor" );
}

bool OpenGl::ShadeModel( eGL_SHADE_MODEL mode )const
{
	m_pfnShadeModel( mode );
	return glCheckError( *this, "glShadeModel" );
}

bool OpenGl::Clear( uint32_t mask )const
{
	m_pfnClear( mask );
	return glCheckError( *this, "glClear" );
}

bool OpenGl::DrawArrays( eGL_PRIMITIVE mode, int first, int count )const
{
	m_pfnDrawArrays( mode, first, count );
	return glCheckError( *this, "glDrawArrays" );
}

bool OpenGl::DrawElements( eGL_PRIMITIVE mode, int count, eGL_TYPE type, void const * indices )const
{
	m_pfnDrawElements( mode, count, type, indices );
	return glCheckError( *this, "glDrawElements" );
}

bool OpenGl::Enable( eGL_TWEAK mode )const
{
	m_pfnEnable( mode );
	return glCheckError( *this, "glEnable" );
}

bool OpenGl::Disable( eGL_TWEAK mode )const
{
	m_pfnDisable( mode );
	return glCheckError( *this, "glDisable" );
}

bool OpenGl::Enable( eGL_TEXDIM texture )const
{
	m_pfnEnable( texture );
	return glCheckError( *this, "glEnable" );
}

bool OpenGl::Disable( eGL_TEXDIM texture )const
{
	m_pfnDisable( texture );
	return glCheckError( *this, "glDisable" );
}

bool OpenGl::SelectBuffer( int size, uint32_t * buffer )const
{
	m_pfnSelectBuffer( size, buffer );
	return glCheckError( *this, "glSelectBuffer" );
}

bool OpenGl::GetIntegerv( uint32_t pname, int * params )const
{
	m_pfnGetIntegerv( pname, params );
	return glCheckError( *this, "glGetIntegerv" );
}

bool OpenGl::GetIntegerv( uint32_t pname, uint64_t * params )const
{
	m_pfnGetIntegerui64v( pname, params );
	return glCheckError( *this, "glGetIntegerui64v" );
}

int OpenGl::RenderMode( eGL_RENDER_MODE mode )const
{
	int l_iReturn = m_pfnRenderMode( mode );

	if ( ! glCheckError( *this, "glRenderMode" ) )
	{
		l_iReturn = eGL_INVALID_INDEX;
	}

	return l_iReturn;
}

#if defined( _WIN32 )

bool OpenGl::MakeCurrent( HDC hdc, HGLRC hglrc )const
{
	wglMakeCurrent( hdc, hglrc );
	return true;
}

bool OpenGl::SwapBuffers( HDC hdc )const
{
	::SwapBuffers( hdc );
	return true;
}

bool OpenGl::SwapInterval( int interval )const
{
	bool l_return = false;

	if ( m_pfnSwapInterval )
	{
		m_pfnSwapInterval( interval );
		l_return = glCheckError( *this, "glSwapInterval" );
	}

	return l_return;
}

HGLRC OpenGl::CreateContext( HDC hdc )const
{
	return wglCreateContext( hdc );
}

bool OpenGl::HasCreateContextAttribs()const
{
	return m_pfnCreateContextAttribs != NULL;
}

HGLRC OpenGl::CreateContextAttribs( HDC hDC, HGLRC hShareContext, int const * attribList )const
{
	return m_pfnCreateContextAttribs( hDC, hShareContext, attribList );
}

bool OpenGl::DeleteContext( HGLRC hContext )const
{
	return m_pfnDeleteContext( hContext ) == TRUE;
}

#elif defined( __linux__ )

bool OpenGl::MakeCurrent( Display * pDisplay, GLXDrawable drawable, GLXContext context )const
{
	m_pfnMakeCurrent( pDisplay, drawable, context );
	return true;
}

bool OpenGl::SwapBuffers( Display * pDisplay, GLXDrawable drawable )const
{
	m_pfnSwapBuffers( pDisplay, drawable );
	return true;
}

bool OpenGl::SwapInterval( Display * pDisplay, GLXDrawable drawable, int interval )const
{
	bool l_return = false;

	if ( m_pfnSwapInterval )
	{
		m_pfnSwapInterval( pDisplay, drawable, interval );
		l_return = glCheckError( *this, "glSwapInterval" );
	}

	return l_return;
}

GLXContext OpenGl::CreateContext( Display * pDisplay, XVisualInfo * pVisualInfo, GLXContext shareList, Bool direct )const
{
	return m_pfnCreateContext( pDisplay, pVisualInfo, shareList, direct );
}

bool OpenGl::HasCreateContextAttribs()const
{
	return m_pfnCreateContextAttribs != NULL;
}

GLXContext OpenGl::CreateContextAttribs( Display * pDisplay, GLXFBConfig fbconfig, GLXContext shareList, Bool direct, int const * attribList )const
{
	return m_pfnCreateContextAttribs( pDisplay, fbconfig, shareList, direct, attribList );
}

bool OpenGl::DeleteContext( Display * pDisplay, GLXContext context )const
{
	m_pfnDeleteContext( pDisplay, context );
	return true;
}

#else

#	error "Yet unsupported OS"

#endif

bool OpenGl::Viewport( int x, int y, int width, int height )const
{
	m_pfnViewport( x, y, width, height );
	return glCheckError( *this, "glViewport" );
}

bool OpenGl::CullFace( eGL_FACE face )const
{
	m_pfnCullFace( face );
	return glCheckError( *this, "glCullFace" );
}

bool OpenGl::FrontFace( eGL_FRONT_FACE_DIRECTION face )const
{
	m_pfnFrontFace( face );
	return glCheckError( *this, "glFrontFace" );
}

bool OpenGl::Material( eGL_DRAW_BUFFER_MODE face, eGL_MATERIAL_COMPONENT pname, float param )const
{
	m_pfnMaterialf( face, pname, param );
	return glCheckError( *this, "glMaterialf" );
}

bool OpenGl::Material( eGL_DRAW_BUFFER_MODE face, eGL_MATERIAL_COMPONENT pname, float const * param )const
{
	m_pfnMaterialfv( face, pname, param );
	return glCheckError( *this, "glMaterialfv" );
}

bool OpenGl::GenTextures( int n, uint32_t * textures )const
{
	m_pfnGenTextures( n, textures );
	return glCheckError( *this, "glGenTextures" );
}

bool OpenGl::DeleteTextures( int n, uint32_t const * textures )const
{
	m_pfnDeleteTextures( n, textures );
	return glCheckError( *this, "glDeleteTextures" );
}

bool OpenGl::IsTexture( uint32_t texture )const
{
	return m_pfnIsTexture( texture ) && glCheckError( *this, "glIsTexture" );
}

bool OpenGl::BindTexture( eGL_TEXDIM mode, uint32_t texture )const
{
	return m_pTexFunctions->BindTexture( mode, texture );
}

bool OpenGl::GenerateMipmap( eGL_TEXDIM mode )const
{
	return m_pTexFunctions->GenerateMipmap( mode );
}

bool OpenGl::ActiveTexture( eGL_TEXTURE_INDEX target )const
{
	m_pfnActiveTexture( target );
	return glCheckError( *this, "glActiveTexture" );
}

bool OpenGl::ClientActiveTexture( eGL_TEXTURE_INDEX target )const
{
	m_pfnClientActiveTexture( target );
	return glCheckError( *this, "glClientActiveTexture" );
}

bool OpenGl::TexSubImage1D( eGL_TEXDIM mode, int level, int xoffset, int width, eGL_FORMAT format, eGL_TYPE type, void const * data )const
{
	return m_pTexFunctions->TexSubImage1D( mode, level, xoffset, width, format, type, data );
}

bool OpenGl::TexSubImage2D( eGL_TEXDIM mode, int level, int xoffset, int yoffset, int width, int height, eGL_FORMAT format, eGL_TYPE type, void const * data )const
{
	return m_pTexFunctions->TexSubImage2D( mode, level, xoffset, yoffset, width, height, format, type, data );
}

bool OpenGl::TexSubImage2D( eGL_TEXDIM mode, int level, Position const & p_position, Size const & p_size, eGL_FORMAT format, eGL_TYPE type, void const * data )const
{
	return m_pTexFunctions->TexSubImage2D( mode, level, p_position.x(), p_position.y(), p_size.width(), p_size.height(), format, type, data );
}

bool OpenGl::TexSubImage2D( eGL_TEXDIM mode, int level, Castor::Rectangle const & p_rect, eGL_FORMAT format, eGL_TYPE type, void const * data )const
{
	return m_pTexFunctions->TexSubImage2D( mode, level, p_rect.left(), p_rect.top(), p_rect.width(), p_rect.height(), format, type, data );
}

bool OpenGl::TexSubImage3D( eGL_TEXDIM mode, int level, int xoffset, int yoffset, int zoffset, int width, int height, int depth, eGL_FORMAT format, eGL_TYPE type, void const * data )const
{
	return m_pTexFunctions->TexSubImage3D( mode, level, xoffset, yoffset, zoffset, width, height, depth, format, type, data );
}

bool OpenGl::TexImage1D( eGL_TEXDIM mode, int level, eGL_INTERNAL internalFormat, int width, int border, eGL_FORMAT format, uint32_t type, void const * data )const
{
	return m_pTexFunctions->TexImage1D( mode, level, internalFormat, width, border, format, type, data );
}

bool OpenGl::TexImage2D( eGL_TEXDIM mode, int level, eGL_INTERNAL internalFormat, int width, int height, int border, eGL_FORMAT format, uint32_t type, void const * data )const
{
	return m_pTexFunctions->TexImage2D( mode, level, internalFormat, width, height, border, format, type, data );
}

bool OpenGl::TexImage2D( eGL_TEXDIM mode, int level, eGL_INTERNAL internalFormat, Size const & p_size, int border, eGL_FORMAT format, uint32_t type, void const * data )const
{
	return m_pTexFunctions->TexImage2D( mode, level, internalFormat, p_size.width(), p_size.height(), border, format, type, data );
}

bool OpenGl::TexImage3D( eGL_TEXDIM mode, int level, eGL_INTERNAL internalFormat, int width, int height, int depth, int border, eGL_FORMAT format, uint32_t type, void const * data )const
{
	return m_pTexFunctions->TexImage3D( mode, level, internalFormat, width, height, depth, border, format, type, data );
}

bool OpenGl::TexParameter( eGL_TEXDIM mode, eGL_TEXTURE_PARAMETER pname, int param )const
{
	return m_pTexFunctions->TexParameter( mode, pname, param );
}

bool OpenGl::TexParameter( eGL_TEXDIM mode, eGL_TEXTURE_PARAMETER pname, float param )const
{
	return m_pTexFunctions->TexParameter( mode, pname, param );
}

bool OpenGl::TexParameter( eGL_TEXDIM mode, eGL_TEXTURE_PARAMETER pname, const int * params )const
{
	return m_pTexFunctions->TexParameter( mode, pname, params );
}

bool OpenGl::TexParameter( eGL_TEXDIM mode, eGL_TEXTURE_PARAMETER pname, const float * params )const
{
	return m_pTexFunctions->TexParameter( mode, pname, params );
}

bool OpenGl::GetTexImage( eGL_TEXDIM mode, int level, eGL_FORMAT format, eGL_TYPE type, void * img )const
{
	return m_pTexFunctions->GetTexImage( mode, level, format, type, img );
}

bool OpenGl::TexEnvi( eGL_TEXENV_TARGET target, eGL_TEXENV_ARGNAME pname, int param )const
{
	m_pfnTexEnvi( target, pname, param );
	return glCheckError( *this, "glTexEnvi" );
}

bool OpenGl::TexEnviv( eGL_TEXENV_TARGET target, eGL_TEXENV_ARGNAME pname, int const * param )const
{
	m_pfnTexEnviv( target, pname, param );
	return glCheckError( *this, "glTexEnviv" );
}

bool OpenGl::TexEnvf( eGL_TEXENV_TARGET target, eGL_TEXENV_ARGNAME pname, float param )const
{
	m_pfnTexEnvf( target, pname, param );
	return glCheckError( *this, "glTexEnvf" );
}

bool OpenGl::TexEnvfv( eGL_TEXENV_TARGET target, eGL_TEXENV_ARGNAME pname, float const * param )const
{
	m_pfnTexEnvfv( target, pname, param );
	return glCheckError( *this, "glTexEnvfv" );
}

bool OpenGl::TexGeni( eGL_TEXGEN_COORD coord, eGL_TEXGEN_PARAM param, eGL_TEXGEN_MODE mode )const
{
	m_pfnTexGeni( coord, param, mode );
	return glCheckError( *this, "glTexGeni" );
}

bool OpenGl::BlendFunc( eGL_BLEND_FACTOR sfactor, eGL_BLEND_FACTOR dfactor )const
{
	m_pfnBlendFunc( sfactor, dfactor );
	return glCheckError( *this, "glBlendFunc" );
}

bool OpenGl::BlendFunc( eGL_BLEND_FACTOR p_eRgbSrc, eGL_BLEND_FACTOR p_eRgbDst, eGL_BLEND_FACTOR p_eAlphaSrc, eGL_BLEND_FACTOR p_eAlphaDst )const
{
	if ( m_pfnBlendFuncSeparate )
	{
		m_pfnBlendFuncSeparate( p_eRgbSrc, p_eRgbDst, p_eAlphaSrc, p_eAlphaDst );
		return glCheckError( *this, "glBlendFuncSeparate" );
	}
	else
	{
		return BlendFunc( p_eAlphaSrc, p_eAlphaDst );
	}
}

bool OpenGl::BlendFunc( uint32_t p_uiBuffer, eGL_BLEND_FACTOR p_eRgbSrc, eGL_BLEND_FACTOR p_eRgbDst, eGL_BLEND_FACTOR p_eAlphaSrc, eGL_BLEND_FACTOR p_eAlphaDst )const
{
	if ( m_pfnBlendFuncSeparatei )
	{
		m_pfnBlendFuncSeparatei( p_uiBuffer, p_eRgbSrc, p_eRgbDst, p_eAlphaSrc, p_eAlphaDst );
		return glCheckError( *this, "glBlendFuncSeparatei" );
	}
	else
	{
		return BlendFunc( p_eRgbSrc, p_eRgbDst, p_eAlphaSrc, p_eAlphaDst );
	}
}

bool OpenGl::BlendEquation( eGL_BLEND_OP p_eOp )const
{
	if ( m_pfnBlendEquation )
	{
		m_pfnBlendEquation( p_eOp );
	}

	return glCheckError( *this, "glBlendEquation" );
}

bool OpenGl::BlendEquation( uint32_t p_uiBuffer, eGL_BLEND_OP p_eOp )const
{
	if ( m_pfnBlendEquationi )
	{
		m_pfnBlendEquationi( p_uiBuffer, p_eOp );
		return glCheckError( *this, "glBlendEquationi" );
	}
	else
	{
		return BlendEquation( p_eOp );
	}
}

bool OpenGl::AlphaFunc( eGL_FUNC func, float ref )const
{
	m_pfnAlphaFunc( func, ref );
	return glCheckError( *this, "glAlphaFunc" );
}

bool OpenGl::DepthFunc( eGL_FUNC p_func )const
{
	m_pfnDepthFunc( p_func );
	return glCheckError( *this, "glDepthFunc" );
}

bool OpenGl::DepthMask( bool p_bMask )const
{
	m_pfnDepthMask( p_bMask );
	return glCheckError( *this, "glDepthMask" );
}

bool OpenGl::ColorMask( bool p_r, bool p_g, bool p_b, bool p_a )const
{
	m_pfnColorMask( p_r, p_g, p_b, p_a );
	return glCheckError( *this, "glColorMask" );
}

bool OpenGl::DebugMessageCallback( PFNGLDEBUGPROC pfnProc, void * p_pThis )const
{
	if ( m_pfnDebugMessageCallback )
	{
		m_pfnDebugMessageCallback( pfnProc, p_pThis );
	}

	return true;
}

bool OpenGl::DebugMessageCallback( PFNGLDEBUGAMDPROC pfnProc, void * p_pThis )const
{
	if ( m_pfnDebugMessageCallbackAMD )
	{
		m_pfnDebugMessageCallbackAMD( pfnProc, p_pThis );
	}

	return true;
}

bool OpenGl::PolygonMode( eGL_FACE p_eFacing, eGL_FILL_MODE p_mode )const
{
	m_pfnPolygonMode( p_eFacing, p_mode );
	return glCheckError( *this, "glPolygonMode" );
}

bool OpenGl::StencilOp( eGL_STENCIL_OP p_eStencilFail, eGL_STENCIL_OP p_eDepthFail, eGL_STENCIL_OP p_eStencilPass )const
{
	m_pfnStencilOp( p_eStencilFail, p_eDepthFail, p_eStencilPass );
	return glCheckError( *this, "glStencilOp" );
}

bool OpenGl::StencilFunc( eGL_FUNC p_func, int p_iRef, uint32_t p_uiMask )const
{
	m_pfnStencilFunc( p_func, p_iRef, p_uiMask );
	return glCheckError( *this, "glStencilFunc" );
}

bool OpenGl::StencilMask( uint32_t p_uiMask )const
{
	m_pfnStencilMask( p_uiMask );
	return glCheckError( *this, "glStencilMask" );
}

bool OpenGl::StencilOpSeparate( eGL_FACE p_eFacing, eGL_STENCIL_OP p_eStencilFail, eGL_STENCIL_OP p_eDepthFail, eGL_STENCIL_OP p_eStencilPass )const
{
	if ( m_pfnStencilOpSeparate )
	{
		m_pfnStencilOpSeparate( p_eFacing, p_eStencilFail, p_eDepthFail, p_eStencilPass );
		return glCheckError( *this, "glStencilOpSeparate" );
	}
	else
	{
		return StencilOp( p_eStencilFail, p_eDepthFail, p_eStencilPass );
	}
}

bool OpenGl::StencilFuncSeparate( eGL_FACE p_eFacing, eGL_FUNC p_func, int p_iRef, uint32_t p_uiMask )const
{
	if ( m_pfnStencilFuncSeparate )
	{
		m_pfnStencilFuncSeparate( p_eFacing, p_func, p_iRef, p_uiMask );
		return glCheckError( *this, "glStencilFuncSeparate" );
	}
	else
	{
		return StencilFunc( p_func, p_iRef, p_uiMask );
	}
}

bool OpenGl::StencilMaskSeparate( eGL_FACE p_eFacing, uint32_t p_uiMask )const
{
	if ( m_pfnStencilMaskSeparate )
	{
		m_pfnStencilMaskSeparate( p_eFacing, p_uiMask );
		return glCheckError( *this, "glStencilMaskSeparate" );
	}
	else
	{
		return StencilMask( p_uiMask );
	}
}

bool OpenGl::Hint( eGL_HINT p_eHint, eGL_HINT_VALUE p_eValue )const
{
	m_pfnHint( p_eHint, p_eValue );
	return glCheckError( *this, "glHint" );
}

bool OpenGl::PolygonOffset( float p_fFactor, float p_fUnits )const
{
	m_pfnPolygonOffset( p_fFactor, p_fUnits );
	return glCheckError( *this, "glPolygonOffset" );
}

bool OpenGl::BlendColor( Colour const & p_colour )const
{
	if ( m_pfnBlendColor )
	{
		m_pfnBlendColor( p_colour.red().value(), p_colour.green().value(), p_colour.blue().value(), p_colour.alpha().value() );
	}

	return glCheckError( *this, "glBlendColor" );
}

bool OpenGl::SampleCoverage( float fValue, bool invert )const
{
	if ( m_pfnSampleCoverage )
	{
		m_pfnSampleCoverage( fValue, invert );
	}

	return glCheckError( *this, "glSampleCoverage" );
}

bool OpenGl::DrawArraysInstanced( eGL_PRIMITIVE mode, int first, int count, int primcount )const
{
	if ( m_bHasInstancedDraw )
	{
		m_pfnDrawArraysInstanced( mode, first, count, primcount );
	}

	return glCheckError( *this, "glDrawArraysInstanced" );
}

bool OpenGl::DrawElementsInstanced( eGL_PRIMITIVE mode, int count, eGL_TYPE type, const void * indices, int primcount )const
{
	if ( m_bHasInstancedDraw )
	{
		m_pfnDrawElementsInstanced( mode, count, type, indices, primcount );
	}

	return glCheckError( *this, "glDrawElementsInstanced" );
}

bool OpenGl::VertexAttribDivisor( uint32_t index, uint32_t divisor )const
{
	if ( m_bHasInstancedArrays )
	{
		m_pfnVertexAttribDivisor( index, divisor );
	}

	return glCheckError( *this, "glVertexAttribDivisor" );
}

bool OpenGl::ReadBuffer( eGL_BUFFER p_buffer )const
{
	m_pfnReadBuffer( p_buffer );
	return glCheckError( *this, "glReadBuffer" );
}

bool OpenGl::ReadPixels( int x, int y, int width, int height, eGL_FORMAT format, eGL_TYPE type, void * pixels )const
{
	m_pfnReadPixels( x, y, width, height, format, type, pixels );
	return glCheckError( *this, "glReadPixels" );
}

bool OpenGl::ReadPixels( Position const & p_position, Size const & p_size, eGL_FORMAT format, eGL_TYPE type, void * pixels )const
{
	m_pfnReadPixels( p_position.x(), p_position.y(), p_size.width(), p_size.height(), format, type, pixels );
	return glCheckError( *this, "glReadPixels" );
}

bool OpenGl::ReadPixels( Castor::Rectangle const & p_rect, eGL_FORMAT format, eGL_TYPE type, void * pixels )const
{
	m_pfnReadPixels( p_rect.left(), p_rect.top(), p_rect.width(), p_rect.height(), format, type, pixels );
	return glCheckError( *this, "glReadPixels" );
}

bool OpenGl::DrawBuffer( eGL_BUFFER p_buffer )const
{
	m_pfnDrawBuffer( p_buffer );
	return glCheckError( *this, "glDrawBuffer" );
}

bool OpenGl::DrawPixels( int width, int height, eGL_FORMAT format, eGL_TYPE type, void const * data )const
{
	m_pfnDrawPixels( width, height, format, type, data );
	return glCheckError( *this, "glDrawPixels" );
}

bool OpenGl::PixelStore( eGL_STORAGE_MODE p_mode, int p_iParam )const
{
	m_pfnPixelStorei( p_mode, p_iParam );
	return glCheckError( *this, "glPixelStorei" );
}

bool OpenGl::PixelStore( eGL_STORAGE_MODE p_mode, float p_fParam )const
{
	m_pfnPixelStoref( p_mode, p_fParam );
	return glCheckError( *this, "glPixelStoref" );
}

bool OpenGl::GenSamplers( int count, uint32_t * samplers )const
{
	m_pfnGenSamplers( count, samplers );
	return glCheckError( *this, "glGenSamplers" );
}

bool OpenGl::DeleteSamplers( int count, const uint32_t * samplers )const
{
	m_pfnDeleteSamplers( count, samplers );
	return glCheckError( *this, "glDeleteSamplers" );
}

bool OpenGl::IsSampler( uint32_t sampler )const
{
	uint8_t l_uiRet = m_pfnIsSampler( sampler );
	return l_uiRet && glCheckError( *this, "glGetIsSampler" );
}

bool OpenGl::BindSampler( uint32_t unit, uint32_t sampler )const
{
	m_pfnBindSampler( unit, sampler );
	return glCheckError( *this, "glBindSampler" );
}

bool OpenGl::GetSamplerParameter( uint32_t sampler, eGL_SAMPLER_PARAMETER pname, uint32_t * params )const
{
	m_pfnGetSamplerParameteruiv( sampler, pname, params );
	return glCheckError( *this, "glGetSamplerParameterIuiv" );
}

bool OpenGl::GetSamplerParameter( uint32_t sampler, eGL_SAMPLER_PARAMETER pname, float * params )const
{
	m_pfnGetSamplerParameterfv( sampler, pname, params );
	return glCheckError( *this, "glGetSamplerParameterfv" );
}

bool OpenGl::GetSamplerParameter( uint32_t sampler, eGL_SAMPLER_PARAMETER pname, int * params )const
{
	m_pfnGetSamplerParameteriv( sampler, pname, params );
	return glCheckError( *this, "glGetSamplerParameteriv" );
}

bool OpenGl::SetSamplerParameter( uint32_t sampler, eGL_SAMPLER_PARAMETER pname, const uint32_t * params )const
{
	if ( pname != eGL_SAMPLER_PARAMETER_MAXANISOTROPY || m_bHasAnisotropic )
	{
		m_pfnSamplerParameteruiv( sampler, pname, params );
		return glCheckError( *this, "glSamplerParameterIuiv" );
	}
	else
	{
		return true;
	}
}

bool OpenGl::SetSamplerParameter( uint32_t sampler, eGL_SAMPLER_PARAMETER pname, float param )const
{
	if ( pname != eGL_SAMPLER_PARAMETER_MAXANISOTROPY || m_bHasAnisotropic )
	{
		m_pfnSamplerParameterf( sampler, pname, param );
		return glCheckError( *this, "glSamplerParameterf" );
	}
	else
	{
		return true;
	}
}

bool OpenGl::SetSamplerParameter( uint32_t sampler, eGL_SAMPLER_PARAMETER pname, const float * params )const
{
	if ( pname != eGL_SAMPLER_PARAMETER_MAXANISOTROPY || m_bHasAnisotropic )
	{
		m_pfnSamplerParameterfv( sampler, pname, params );
		return glCheckError( *this, "glSamplerParameterfv" );
	}
	else
	{
		return true;
	}
}

bool OpenGl::SetSamplerParameter( uint32_t sampler, eGL_SAMPLER_PARAMETER pname, int param )const
{
	if ( pname != eGL_SAMPLER_PARAMETER_MAXANISOTROPY || m_bHasAnisotropic )
	{
		m_pfnSamplerParameteri( sampler, pname, param );
		return glCheckError( *this, "glSamplerParameteri" );
	}
	else
	{
		return true;
	}
}

bool OpenGl::SetSamplerParameter( uint32_t sampler, eGL_SAMPLER_PARAMETER pname, const int * params )const
{
	if ( pname != eGL_SAMPLER_PARAMETER_MAXANISOTROPY || m_bHasAnisotropic )
	{
		m_pfnSamplerParameteriv( sampler, pname, params );
		return glCheckError( *this, "glSamplerParameteriv" );
	}
	else
	{
		return true;
	}
}

bool OpenGl::TexBuffer( eGL_TEXDIM p_target, eGL_INTERNAL_FORMAT p_eInternalFormat, uint32_t buffer )const
{
	m_pfnTexBuffer( p_target, p_eInternalFormat, buffer );
	return glCheckError( *this, "glTexBuffer" );
}

bool OpenGl::EnableClientState( eGL_BUFFER_USAGE p_eArray )const
{
	return m_pBufFunctions->EnableClientState( p_eArray );
}

bool OpenGl::DisableClientState( eGL_BUFFER_USAGE p_eArray )const
{
	return m_pBufFunctions->DisableClientState( p_eArray );
}

bool OpenGl::GenBuffers( int n, uint32_t * buffers )const
{
	m_pfnGenBuffers( n, buffers );
	return glCheckError( *this, "glGenBuffers" );
}

bool OpenGl::DeleteBuffers( int n, uint32_t const * buffers )const
{
	m_pfnDeleteBuffers( n, buffers );
	return glCheckError( *this, "glDeleteBuffers" );
}

bool OpenGl::IsBuffer( uint32_t buffer )const
{
	return m_pfnIsBuffer( buffer ) && glCheckError( *this, "glIsBuffer" );
}

bool OpenGl::BindBuffer( eGL_BUFFER_TARGET p_target, uint32_t buffer )const
{
	return m_pBufFunctions->BindBuffer( p_target, buffer );
}

bool OpenGl::BufferData( eGL_BUFFER_TARGET p_target, ptrdiff_t size, void const * data, eGL_BUFFER_MODE usage )const
{
	return m_pBufFunctions->BufferData( p_target, size, data, usage );
}

bool OpenGl::BufferSubData( eGL_BUFFER_TARGET p_target, ptrdiff_t offset, ptrdiff_t size, void const * data )const
{
	return m_pBufFunctions->BufferSubData( p_target, offset, size, data );
}

bool OpenGl::GetBufferParameter( eGL_BUFFER_TARGET p_target, eGL_BUFFER_PARAMETER pname, int * params )const
{
	return m_pBufFunctions->GetBufferParameter( p_target, pname, params );
}

bool OpenGl::GetBufferParameter( eGL_BUFFER_TARGET p_target, eGL_BUFFER_PARAMETER pname, uint64_t * params )const
{
	return m_pBufFunctions->GetBufferParameter( p_target, pname, params );
}

void * OpenGl::MapBuffer( eGL_BUFFER_TARGET p_target, eGL_LOCK access )const
{
	return m_pBufFunctions->MapBuffer( p_target, access );
}

bool OpenGl::UnmapBuffer( eGL_BUFFER_TARGET p_target )const
{
	return m_pBufFunctions->UnmapBuffer( p_target );
}

void * OpenGl::MapBufferRange( eGL_BUFFER_TARGET p_target, ptrdiff_t offset, ptrdiff_t length, uint32_t access )const
{
	return m_pBufFunctions->MapBufferRange( p_target, offset, length, access );
}

bool OpenGl::FlushMappedBufferRange( eGL_BUFFER_TARGET p_target, ptrdiff_t offset, ptrdiff_t length )const
{
	return m_pBufFunctions->FlushMappedBufferRange( p_target, offset, length );
}

bool OpenGl::VertexPointer( int size, uint32_t type, uint32_t stride, void const * pointer )const
{
	m_pfnVertexPointer( size, type, stride, pointer );
	return glCheckError( *this, "glVertexPointer" );
}

bool OpenGl::NormalPointer( uint32_t type, uint32_t stride, void const * pointer )const
{
	m_pfnNormalPointer( type, stride, pointer );
	return glCheckError( *this, "glNormalPointer" );
}

bool OpenGl::TangentPointer( uint32_t type, uint32_t stride, void const * pointer )const
{
	if ( m_pfnTangentPointer )
	{
		m_pfnTangentPointer( type, stride, const_cast< void * >( pointer ) );
	}

	return glCheckError( *this, "glTangentPointer" );
}

bool OpenGl::BinormalPointer( uint32_t type, uint32_t stride, void const * pointer )const
{
	if ( m_pfnBinormalPointer )
	{
		m_pfnBinormalPointer( type, stride, const_cast< void * >( pointer ) );
	}

	return glCheckError( *this, "glBinormalPointer" );
}

bool OpenGl::HasTangentPointer()const
{
	return m_pfnTangentPointer != NULL;
}

bool OpenGl::HasBinormalPointer()const
{
	return m_pfnBinormalPointer != NULL;
}

bool OpenGl::ColorPointer( int size, uint32_t type, uint32_t stride, void const * pointer )const
{
	m_pfnColorPointer( size, type, stride, pointer );
	return glCheckError( *this, "glColorPointer" );
}

bool OpenGl::TexCoordPointer( int size, uint32_t type, uint32_t stride, void const * pointer )const
{
	m_pfnTexCoordPointer( size, type, stride, pointer );
	return glCheckError( *this, "glTexCoordPointer" );
}

bool OpenGl::BufferAddressRange( eGL_ADDRESS pname, uint32_t index, uint64_t address, size_t length )const
{
	return m_pBufFunctions->BufferAddressRange( pname, index, address, length );
}

bool OpenGl::VertexFormat( int size, eGL_TYPE type, int stride )const
{
	return m_pBufFunctions->VertexFormat( size, type, stride );
}

bool OpenGl::NormalFormat( eGL_TYPE type, int stride )const
{
	return m_pBufFunctions->NormalFormat( type, stride );
}

bool OpenGl::ColorFormat( int size, eGL_TYPE type, int stride )const
{
	return m_pBufFunctions->ColorFormat( size, type, stride );
}

bool OpenGl::IndexFormat( eGL_TYPE type, int stride )const
{
	return m_pBufFunctions->IndexFormat( type, stride );
}

bool OpenGl::TexCoordFormat( int size, eGL_TYPE type, int stride )const
{
	return m_pBufFunctions->TexCoordFormat( size, type, stride );
}

bool OpenGl::EdgeFlagFormat( int stride )const
{
	return m_pBufFunctions->EdgeFlagFormat( stride );
}

bool OpenGl::SecondaryColorFormat( int size, eGL_TYPE type, int stride )const
{
	return m_pBufFunctions->SecondaryColorFormat( size, type, stride );
}

bool OpenGl::FogCoordFormat( uint32_t type, int stride )const
{
	return m_pBufFunctions->FogCoordFormat( type, stride );
}

bool OpenGl::VertexAttribFormat( uint32_t index, int size, eGL_TYPE type, bool normalized, int stride )const
{
	return m_pBufFunctions->VertexAttribFormat( index, size, type, normalized, stride );
}

bool OpenGl::VertexAttribIFormat( uint32_t index, int size, eGL_TYPE type, int stride )const
{
	return m_pBufFunctions->VertexAttribIFormat( index, size, type, stride );
}

bool OpenGl::MakeBufferResident( eGL_BUFFER_TARGET target, eGL_RESIDENT_BUFFER_ACCESS access )const
{
	return m_pBufFunctions->MakeBufferResident( target, access );
}

bool OpenGl::MakeBufferNonResident( eGL_BUFFER_TARGET target )const
{
	return m_pBufFunctions->MakeBufferNonResident( target );
}

bool OpenGl::IsBufferResident( eGL_BUFFER_TARGET target )const
{
	return m_pBufFunctions->IsBufferResident( target );
}

bool OpenGl::MakeNamedBufferResident( uint32_t buffer, eGL_RESIDENT_BUFFER_ACCESS access )const
{
	return m_pBufFunctions->MakeNamedBufferResident( buffer, access );
}

bool OpenGl::MakeNamedBufferNonResident( uint32_t buffer )const
{
	return m_pBufFunctions->MakeNamedBufferNonResident( buffer );
}

bool OpenGl::IsNamedBufferResident( uint32_t buffer )const
{
	return m_pBufFunctions->IsNamedBufferResident( buffer );
}

bool OpenGl::GetNamedBufferParameter( uint32_t buffer, eGL_BUFFER_PARAMETER pname, uint64_t * params )const
{
	return m_pBufFunctions->GetNamedBufferParameter( buffer, pname, params );
}

bool OpenGl::GenFramebuffers( int n, uint32_t * framebuffers )const
{
	m_pfnGenFramebuffers( n, framebuffers );
	return glCheckError( *this, "glGenFramebuffers" );
}

bool OpenGl::DeleteFramebuffers( int n, uint32_t const * framebuffers )const
{
	m_pfnDeleteFramebuffers( n, framebuffers );
	return glCheckError( *this, "glDeleteFramebuffers" );
}

bool OpenGl::IsFramebuffer( uint32_t framebuffer )const
{
	m_pfnIsFramebuffer( framebuffer );
	return glCheckError( *this, "glIsFramebuffer" );
}

bool OpenGl::BindFramebuffer( eGL_FRAMEBUFFER_MODE p_eBindingMode, uint32_t framebuffer )const
{
	m_pfnBindFramebuffer( p_eBindingMode, framebuffer );
	return glCheckError( *this, "glBindFramebuffer" );
}

bool OpenGl::FramebufferTexture( eGL_FRAMEBUFFER_MODE p_target, eGL_TEXTURE_ATTACHMENT p_eAttachment, uint32_t texture, int level )const
{
	m_pfnFramebufferTexture( p_target, p_eAttachment, texture, level );
	return glCheckError( *this, "glFramebufferTexture" );
}

bool OpenGl::FramebufferTexture1D( eGL_FRAMEBUFFER_MODE p_target, eGL_TEXTURE_ATTACHMENT p_eAttachment, eGL_TEXDIM p_eTextarget, uint32_t texture, int level )const
{
	m_pfnFramebufferTexture1D( p_target, p_eAttachment, p_eTextarget, texture, level );
	return glCheckError( *this, "glFramebufferTexture1D" );
}

bool OpenGl::FramebufferTexture2D( eGL_FRAMEBUFFER_MODE p_target, eGL_TEXTURE_ATTACHMENT p_eAttachment, eGL_TEXDIM p_eTextarget, uint32_t texture, int level )const
{
	m_pfnFramebufferTexture2D( p_target, p_eAttachment, p_eTextarget, texture, level );
	return glCheckError( *this, "glFramebufferTexture2D" );
}

bool OpenGl::FramebufferTexture3D( eGL_FRAMEBUFFER_MODE p_target, eGL_TEXTURE_ATTACHMENT p_eAttachment, eGL_TEXDIM p_eTextarget, uint32_t texture, int level, int layer )const
{
	m_pfnFramebufferTexture3D( p_target, p_eAttachment, p_eTextarget, texture, level, layer );
	return glCheckError( *this, "glFramebufferTexture3D" );
}

bool OpenGl::FramebufferTextureLayer( eGL_FRAMEBUFFER_MODE p_target, eGL_TEXTURE_ATTACHMENT p_eAttachment, uint32_t texture, int level, int layer )const
{
	m_pfnFramebufferTextureLayer( p_target, p_eAttachment, texture, level, layer );
	return glCheckError( *this, "glFramebufferTextureLayer" );
}

bool OpenGl::FramebufferRenderbuffer( eGL_FRAMEBUFFER_MODE p_target, eGL_RENDERBUFFER_ATTACHMENT p_eAttachmentPoint, eGL_RENDERBUFFER_MODE p_eRenderbufferTarget, uint32_t renderbufferId )const
{
	m_pfnFramebufferRenderbuffer( p_target, p_eAttachmentPoint, p_eRenderbufferTarget, renderbufferId );
	return glCheckError( *this, "glFramebufferRenderbuffer" );
}

bool OpenGl::GenRenderbuffers( int n, uint32_t * ids )const
{
	m_pfnGenRenderbuffers( n, ids );
	return glCheckError( *this, "glGenRenderbuffers" );
}

bool OpenGl::DeleteRenderbuffers( int n, uint32_t const * ids )const
{
	m_pfnDeleteRenderbuffers( n, ids );
	return glCheckError( *this, "glDeleteRenderbuffers" );
}

bool OpenGl::IsRenderbuffer( uint32_t framebuffer )const
{
	m_pfnIsRenderbuffer( framebuffer );
	return glCheckError( *this, "glIsRenderbuffer" );
}

bool OpenGl::BindRenderbuffer( eGL_RENDERBUFFER_MODE p_eBindingMode, uint32_t id )const
{
	m_pfnBindRenderbuffer( p_eBindingMode, id );
	return glCheckError( *this, "glBindRenderbuffer" );
}

bool OpenGl::RenderbufferStorage( eGL_RENDERBUFFER_MODE p_eBindingMode, eGL_RENDERBUFFER_STORAGE p_eFormat, int width, int height )const
{
	m_pfnRenderbufferStorage( p_eBindingMode, p_eFormat, width, height );
	return glCheckError( *this, "glRenderbufferStorage" );
}

bool OpenGl::RenderbufferStorageMultisample( eGL_RENDERBUFFER_MODE p_eBindingMode, int p_iSamples, eGL_RENDERBUFFER_STORAGE p_eFormat, int width, int height )const
{
	bool l_return = true;
	int l_iMaxSamples;
	int l_iMaxSize;
	OpenGl::GetIntegerv( eGL_MAX_SAMPLES, &l_iMaxSamples );
	OpenGl::GetIntegerv( eGL_MAX_RENDERBUFFER_SIZE, &l_iMaxSize );

	if ( p_iSamples <= l_iMaxSamples && width <= l_iMaxSize && height < l_iMaxSize )
	{
		m_pfnRenderbufferStorageMultisample( p_eBindingMode, p_iSamples, p_eFormat, width, height );
		l_return = glCheckError( *this, "glRenderbufferStorageMultisample" );
	}
	else if ( p_iSamples > l_iMaxSamples )
	{
		Logger::LogWarning( String( cuT( "glRenderbufferStorageMultisample - Asked for more samples than available, setting it to max available" ) ) );
		m_pfnRenderbufferStorageMultisample( p_eBindingMode, l_iMaxSamples, p_eFormat, width, height );
		l_return = glCheckError( *this, "glRenderbufferStorageMultisample" );
	}
	else if ( width > l_iMaxSize )
	{
		Logger::LogError( String( cuT( "glRenderbufferStorageMultisample - Asked for greater width than available" ) ) );
		l_return = false;
	}
	else if ( height > l_iMaxSize )
	{
		Logger::LogError( String( cuT( "glRenderbufferStorageMultisample - Asked for greater height than available" ) ) );
		l_return = false;
	}

	return l_return;
}

bool OpenGl::RenderbufferStorage( eGL_RENDERBUFFER_MODE p_eBindingMode, eGL_RENDERBUFFER_STORAGE p_eFormat, Castor::Size const & size )const
{
	m_pfnRenderbufferStorage( p_eBindingMode, p_eFormat, size.width(), size.height() );
	return glCheckError( *this, "glRenderbufferStorage" );
}

bool OpenGl::RenderbufferStorageMultisample( eGL_RENDERBUFFER_MODE p_eBindingMode, int p_iSamples, eGL_RENDERBUFFER_STORAGE p_eFormat, Castor::Size const & size )const
{
	bool l_return = true;
	int l_iMaxSamples;
	int l_iMaxSize;
	OpenGl::GetIntegerv( eGL_MAX_SAMPLES, &l_iMaxSamples );
	OpenGl::GetIntegerv( eGL_MAX_RENDERBUFFER_SIZE, &l_iMaxSize );

	if ( p_iSamples <= l_iMaxSamples && int( size.width() ) <= l_iMaxSize && int( size.height() ) < l_iMaxSize )
	{
		m_pfnRenderbufferStorageMultisample( p_eBindingMode, p_iSamples, p_eFormat, size.width(), size.height() );
		l_return = glCheckError( *this, "glRenderbufferStorageMultisample" );
	}
	else if ( p_iSamples > l_iMaxSamples )
	{
		Logger::LogWarning( String( cuT( "glRenderbufferStorageMultisample - Asked for more samples than available, setting it to max available" ) ) );
		m_pfnRenderbufferStorageMultisample( p_eBindingMode, l_iMaxSamples, p_eFormat, size.width(), size.height() );
		l_return = glCheckError( *this, "glRenderbufferStorageMultisample" );
	}
	else if ( int( size.width() ) > l_iMaxSize )
	{
		Logger::LogError( String( cuT( "glRenderbufferStorageMultisample - Asked for greater width than available" ) ) );
		l_return = false;
	}
	else if ( int( size.height() ) > l_iMaxSize )
	{
		Logger::LogError( String( cuT( "glRenderbufferStorageMultisample - Asked for greater height than available" ) ) );
		l_return = false;
	}

	return l_return;
}

bool OpenGl::GetRenderbufferParameteriv( eGL_RENDERBUFFER_MODE p_eBindingMode, eGL_RENDERBUFFER_PARAMETER p_eParam, int * values )const
{
	m_pfnGetRenderbufferParameteriv( p_eBindingMode, p_eParam, values );
	return glCheckError( *this, "glGetRenderbufferParameteriv" );
}

bool OpenGl::BlitFramebuffer( int srcX0, int srcY0, int srcX1, int srcY1, int dstX0, int dstY0, int dstX1, int dstY1, uint32_t mask, uint32_t filter )const
{
	m_pfnBlitFramebuffer( srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter );
	return glCheckError( *this, "glBlitFramebuffer" );
}

bool OpenGl::BlitFramebuffer( Castor::Rectangle const & rcSrc, Castor::Rectangle const & rcDst, uint32_t mask, uint32_t filter )const
{
	m_pfnBlitFramebuffer( rcSrc.left(), rcSrc.top(), rcSrc.right(), rcSrc.bottom(), rcDst.left(), rcDst.top(), rcDst.right(), rcDst.bottom(), mask, filter );
	return glCheckError( *this, "glBlitFramebuffer" );
}

bool OpenGl::DrawBuffers( int n, const uint32_t * bufs )const
{
	m_pfnDrawBuffers( n, bufs );
	return glCheckError( *this, "glDrawBuffers" );
}

eGL_FRAMEBUFFER_STATUS OpenGl::CheckFramebufferStatus( eGL_FRAMEBUFFER_MODE p_target )const
{
	eGL_FRAMEBUFFER_STATUS l_return = eGL_FRAMEBUFFER_STATUS( m_pfnCheckFramebufferStatus( p_target ) );

	if ( !glCheckError( *this, "glCheckFramebufferStatus" ) )
	{
		l_return = eGL_FRAMEBUFFER_INVALID;
	}

	return l_return;
}

bool OpenGl::TexImage2DMultisample( eGL_TEXDIM p_target, int p_iSamples, eGL_INTERNAL p_eInternalFormat, int p_iWidth, int p_iHeight, bool p_bFixedSampleLocations )const
{
	try
	{
		m_pfnTexImage2DMultisample( p_target, p_iSamples, p_eInternalFormat, p_iWidth, p_iHeight, p_bFixedSampleLocations );
		return glCheckError( *this, "glTexImage2DMultisample" );
	}
	catch ( ... )
	{
		return false;
	}
}

bool OpenGl::TexImage2DMultisample( eGL_TEXDIM p_target, int p_iSamples, eGL_INTERNAL p_eInternalFormat, Size const & p_size, bool p_bFixedSampleLocations )const
{
	try
	{
		m_pfnTexImage2DMultisample( p_target, p_iSamples, p_eInternalFormat, p_size.width(), p_size.height(), p_bFixedSampleLocations );
		return glCheckError( *this, "glTexImage2DMultisample" );
	}
	catch ( ... )
	{
		return false;
	}
}

int OpenGl::GetUniformLocation( uint32_t program, char const * name )const
{
	int l_iReturn = m_pfnGetUniformLocation( program, name );

	if ( ! glCheckError( *this, "glGetUniformLocation" ) )
	{
		l_iReturn = eGL_INVALID_INDEX;
	}

	return l_iReturn;
}

bool OpenGl::SetUniform( int location, int v0 )const
{
	m_pfnUniform1i( location, v0 );
	return glCheckError( *this, "glUniform1i" );
}

bool OpenGl::SetUniform( int location, int v0, int v1 )const
{
	m_pfnUniform2i( location, v0, v1 );
	return glCheckError( *this, "glUniform2i" );
}

bool OpenGl::SetUniform( int location, int v0, int v1, int v2 )const
{
	m_pfnUniform3i( location, v0, v1, v2 );
	return glCheckError( *this, "glUniform3i" );
}

bool OpenGl::SetUniform( int location, int v0, int v1, int v2, int v3 )const
{
	m_pfnUniform4i( location, v0, v1, v2, v3 );
	return glCheckError( *this, "glUniform4i" );
}

bool OpenGl::SetUniform1v( int location, int count, int const * params )const
{
	m_pfnUniform1iv( location, count, params );
	return glCheckError( *this, "glUniform1iv" );
}

bool OpenGl::SetUniform2v( int location, int count, int const * params )const
{
	m_pfnUniform2iv( location, count, params );
	return glCheckError( *this, "glUniform2iv" );
}

bool OpenGl::SetUniform3v( int location, int count, int const * params )const
{
	m_pfnUniform3iv( location, count, params );
	return glCheckError( *this, "glUniform3iv" );
}

bool OpenGl::SetUniform4v( int location, int count, int const * params )const
{
	m_pfnUniform4iv( location, count, params );
	return glCheckError( *this, "glUniform4iv" );
}

bool OpenGl::SetUniform( int location, uint32_t v0 )const
{
	m_pfnUniform1ui( location, v0 );
	return glCheckError( *this, "glUniform1ui" );
}

bool OpenGl::SetUniform( int location, uint32_t v0, uint32_t v1 )const
{
	m_pfnUniform2ui( location, v0, v1 );
	return glCheckError( *this, "glUniform2ui" );
}

bool OpenGl::SetUniform( int location, uint32_t v0, uint32_t v1, uint32_t v2 )const
{
	m_pfnUniform3ui( location, v0, v1, v2 );
	return glCheckError( *this, "glUniformu3i" );
}

bool OpenGl::SetUniform( int location, uint32_t v0, uint32_t v1, uint32_t v2, uint32_t v3 )const
{
	m_pfnUniform4ui( location, v0, v1, v2, v3 );
	return glCheckError( *this, "glUniform4ui" );
}

bool OpenGl::SetUniform1v( int location, int count, uint32_t const * params )const
{
	m_pfnUniform1uiv( location, count, params );
	return glCheckError( *this, "glUniform1uiv" );
}

bool OpenGl::SetUniform2v( int location, int count, uint32_t const * params )const
{
	m_pfnUniform2uiv( location, count, params );
	return glCheckError( *this, "glUniform2uiv" );
}

bool OpenGl::SetUniform3v( int location, int count, uint32_t const * params )const
{
	m_pfnUniform3uiv( location, count, params );
	return glCheckError( *this, "glUniform3uiv" );
}

bool OpenGl::SetUniform4v( int location, int count, uint32_t const * params )const
{
	m_pfnUniform4uiv( location, count, params );
	return glCheckError( *this, "glUniform4uiv" );
}

bool OpenGl::SetUniform( int location, float v0 )const
{
	m_pfnUniform1f( location, v0 );
	return glCheckError( *this, "glUniform1f" );
}

bool OpenGl::SetUniform( int location, float v0, float v1 )const
{
	m_pfnUniform2f( location, v0, v1 );
	return glCheckError( *this, "glUniform2f" );
}

bool OpenGl::SetUniform( int location, float v0, float v1, float v2 )const
{
	m_pfnUniform3f( location, v0, v1, v2 );
	return glCheckError( *this, "glUniform3f" );
}

bool OpenGl::SetUniform( int location, float v0, float v1, float v2, float v3 )const
{
	m_pfnUniform4f( location, v0, v1, v2, v3 );
	return glCheckError( *this, "glUniform4f" );
}

bool OpenGl::SetUniform1v( int location, int count, float const * params )const
{
	m_pfnUniform1fv( location, count, params );
	return glCheckError( *this, "glUniform1fv" );
}

bool OpenGl::SetUniform2v( int location, int count, float const * params )const
{
	m_pfnUniform2fv( location, count, params );
	return glCheckError( *this, "glUniform2fv" );
}

bool OpenGl::SetUniform3v( int location, int count, float const * params )const
{
	m_pfnUniform3fv( location, count, params );
	return glCheckError( *this, "glUniform3fv" );
}

bool OpenGl::SetUniform4v( int location, int count, float const * params )const
{
	m_pfnUniform4fv( location, count, params );
	return glCheckError( *this, "glUniform4fv" );
}

bool OpenGl::SetUniform( int location, double v0 )const
{
	m_pfnUniform1d( location, v0 );
	return glCheckError( *this, "glUniform1d" );
}

bool OpenGl::SetUniform( int location, double v0, double v1 )const
{
	if ( m_pfnUniform2d )
	{
		m_pfnUniform2d( location, v0, v1 );
	}

	return glCheckError( *this, "glUniform2d" );
}

bool OpenGl::SetUniform( int location, double v0, double v1, double v2 )const
{
	if ( m_pfnUniform3d )
	{
		m_pfnUniform3d( location, v0, v1, v2 );
	}

	return glCheckError( *this, "glUniform3d" );
}

bool OpenGl::SetUniform( int location, double v0, double v1, double v2, double v3 )const
{
	if ( m_pfnUniform4d )
	{
		m_pfnUniform4d( location, v0, v1, v2, v3 );
	}

	return glCheckError( *this, "glUniform4d" );
}

bool OpenGl::SetUniform1v( int location, int count, double const * params )const
{
	if ( m_pfnUniform1dv )
	{
		m_pfnUniform1dv( location, count, params );
	}

	return glCheckError( *this, "glUniform1dv" );
}

bool OpenGl::SetUniform2v( int location, int count, double const * params )const
{
	if ( m_pfnUniform2dv )
	{
		m_pfnUniform2dv( location, count, params );
	}

	return glCheckError( *this, "glUniform2dv" );
}

bool OpenGl::SetUniform3v( int location, int count, double const * params )const
{
	if ( m_pfnUniform3dv )
	{
		m_pfnUniform3dv( location, count, params );
	}

	return glCheckError( *this, "glUniform3dv" );
}

bool OpenGl::SetUniform4v( int location, int count, double const * params )const
{
	if ( m_pfnUniform4dv )
	{
		m_pfnUniform4dv( location, count, params );
	}

	return glCheckError( *this, "glUniform4dv" );
}

bool OpenGl::SetUniformMatrix2x2v( int location, int count, bool transpose, float const * value )const
{
	m_pfnUniformMatrix2fv( location, count, transpose, value );
	return glCheckError( *this, "glUniformMatrix2fv" );
}

bool OpenGl::SetUniformMatrix2x3v( int location, int count, bool transpose, float const * value )const
{
	m_pfnUniformMatrix2x3fv( location, count, transpose, value );
	return glCheckError( *this, "glUniformMatrix2x3fv" );
}

bool OpenGl::SetUniformMatrix2x4v( int location, int count, bool transpose, float const * value )const
{
	m_pfnUniformMatrix2x4fv( location, count, transpose, value );
	return glCheckError( *this, "glUniformMatrix2x4fv" );
}

bool OpenGl::SetUniformMatrix3x2v( int location, int count, bool transpose, float const * value )const
{
	m_pfnUniformMatrix3x2fv( location, count, transpose, value );
	return glCheckError( *this, "glUniformMatrix3x2fv" );
}

bool OpenGl::SetUniformMatrix3x3v( int location, int count, bool transpose, float const * value )const
{
	m_pfnUniformMatrix3fv( location, count, transpose, value );
	return glCheckError( *this, "glUniformMatrix3fv" );
}

bool OpenGl::SetUniformMatrix3x4v( int location, int count, bool transpose, float const * value )const
{
	m_pfnUniformMatrix3x4fv( location, count, transpose, value );
	return glCheckError( *this, "glUniformMatrix3x4fv" );
}

bool OpenGl::SetUniformMatrix4x2v( int location, int count, bool transpose, float const * value )const
{
	m_pfnUniformMatrix4x2fv( location, count, transpose, value );
	return glCheckError( *this, "glUniformMatrix4x2fv" );
}

bool OpenGl::SetUniformMatrix4x3v( int location, int count, bool transpose, float const * value )const
{
	m_pfnUniformMatrix4x3fv( location, count, transpose, value );
	return glCheckError( *this, "glUniformMatrix4x3fv" );
}

bool OpenGl::SetUniformMatrix4x4v( int location, int count, bool transpose, float const * value )const
{
	m_pfnUniformMatrix4fv( location, count, transpose, value );
	return glCheckError( *this, "glUniformMatrix4fv" );
}

bool OpenGl::SetUniformMatrix2x2v( int location, int count, bool transpose, double const * value )const
{
	if ( m_pfnUniformMatrix2dv )
	{
		m_pfnUniformMatrix2dv( location, count, transpose, value );
	}

	return glCheckError( *this, "glUniformMatrix2dv" );
}

bool OpenGl::SetUniformMatrix2x3v( int location, int count, bool transpose, double const * value )const
{
	if ( m_pfnUniformMatrix2x3dv )
	{
		m_pfnUniformMatrix2x3dv( location, count, transpose, value );
	}

	return glCheckError( *this, "glUniformMatrix2x3dv" );
}

bool OpenGl::SetUniformMatrix2x4v( int location, int count, bool transpose, double const * value )const
{
	if ( m_pfnUniformMatrix2x4dv )
	{
		m_pfnUniformMatrix2x4dv( location, count, transpose, value );
	}

	return glCheckError( *this, "glUniformMatrix2x4dv" );
}

bool OpenGl::SetUniformMatrix3x2v( int location, int count, bool transpose, double const * value )const
{
	if ( m_pfnUniformMatrix3x2dv )
	{
		m_pfnUniformMatrix3x2dv( location, count, transpose, value );
	}

	return glCheckError( *this, "glUniformMatrix3x2dv" );
}

bool OpenGl::SetUniformMatrix3x3v( int location, int count, bool transpose, double const * value )const
{
	if ( m_pfnUniformMatrix3dv )
	{
		m_pfnUniformMatrix3dv( location, count, transpose, value );
	}

	return glCheckError( *this, "glUniformMatrix3dv" );
}

bool OpenGl::SetUniformMatrix3x4v( int location, int count, bool transpose, double const * value )const
{
	if ( m_pfnUniformMatrix3x4dv )
	{
		m_pfnUniformMatrix3x4dv( location, count, transpose, value );
	}

	return glCheckError( *this, "glUniformMatrix3x4dv" );
}

bool OpenGl::SetUniformMatrix4x2v( int location, int count, bool transpose, double const * value )const
{
	if ( m_pfnUniformMatrix4x2dv )
	{
		m_pfnUniformMatrix4x2dv( location, count, transpose, value );
	}

	return glCheckError( *this, "glUniformMatrix4x2dv" );
}

bool OpenGl::SetUniformMatrix4x3v( int location, int count, bool transpose, double const * value )const
{
	if ( m_pfnUniformMatrix4x3dv )
	{
		m_pfnUniformMatrix4x3dv( location, count, transpose, value );
	}

	return glCheckError( *this, "glUniformMatrix4x3dv" );
}

bool OpenGl::SetUniformMatrix4x4v( int location, int count, bool transpose, double const * value )const
{
	if ( m_pfnUniformMatrix4dv )
	{
		m_pfnUniformMatrix4dv( location, count, transpose, value );
	}

	return glCheckError( *this, "glUniformMatrix4dv" );
}

uint32_t OpenGl::GetUniformBlockIndex( uint32_t program, char const * uniformBlockName )const
{
	uint32_t l_uiReturn = m_pfnGetUniformBlockIndex( program, uniformBlockName );

	if ( !glCheckError( *this, "glGetUniformBlockIndex" ) )
	{
		l_uiReturn = eGL_INVALID_INDEX;
	}

	return l_uiReturn;
}

bool OpenGl::BindBufferBase( uint32_t target, uint32_t index, uint32_t buffer )const
{
	m_pfnBindBufferBase( target, index, buffer );
	return glCheckError( *this, "glBindBufferBase" );
}

bool OpenGl::UniformBlockBinding( uint32_t program, uint32_t uniformBlockIndex, uint32_t uniformBlockBinding )const
{
	m_pfnUniformBlockBinding( program, uniformBlockIndex, uniformBlockBinding );
	return glCheckError( *this, "glUniformBlockBinding" );
}

bool OpenGl::GetUniformIndices( uint32_t program, int uniformCount, char const ** uniformNames, uint32_t * uniformIndices )const
{
	m_pfnGetUniformIndices( program, uniformCount, uniformNames, uniformIndices );
	return glCheckError( *this, "glGetUniformIndices" );
}

bool OpenGl::GetActiveUniformsiv( uint32_t program, int uniformCount, uint32_t const * uniformIndices, eGL_UNIFORM_NAME pname, int * params )const
{
	m_pfnGetActiveUniformsiv( program, uniformCount, uniformIndices, pname, params );
	return glCheckError( *this, "glGetActiveUniformsiv" );
}

bool OpenGl::GetActiveUniformBlockiv( uint32_t program, uint32_t uniformBlockIndex, eGL_UNIFORM_NAME pname, int * params )const
{
	m_pfnGetActiveUniformBlockiv( program, uniformBlockIndex, pname, params );
	return glCheckError( *this, "glGetActiveUniformBlockiv" );
}

uint32_t OpenGl::CreateShader( eGL_SHADER_TYPE type )const
{
	uint32_t l_uiReturn = m_pfnCreateShader( type );

	if ( ! glCheckError( *this, "glCreateShader" ) )
	{
		l_uiReturn = eGL_INVALID_INDEX;
	}

	return l_uiReturn;
}

bool OpenGl::DeleteShader( uint32_t shader )const
{
	m_pfnDeleteShader( shader );
	return glCheckError( *this, "glDeleteShader" );
}

bool OpenGl::IsShader( uint32_t shader )const
{
	uint8_t l_return = m_pfnIsShader( shader );
	return glCheckError( *this, "glIsShader" ) && l_return == eGL_TRUE;
}

bool OpenGl::AttachShader( uint32_t program, uint32_t shader )const
{
	m_pfnAttachShader( program, shader );
	return glCheckError( *this, "glAttachShader" );
}

bool OpenGl::DetachShader( uint32_t program, uint32_t shader )const
{
	m_pfnDetachShader( program, shader );
	return glCheckError( *this, "glDetachShader" );
}

bool OpenGl::CompileShader( uint32_t shader )const
{
	m_pfnCompileShader( shader );
	return glCheckError( *this, "glCompileShader" );
}

bool OpenGl::GetShaderiv( uint32_t shader, uint32_t pname, int * param )const
{
	m_pfnGetShaderiv( shader, pname, param );
	return glCheckError( *this, "glGetShaderiv" );
}

bool OpenGl::GetShaderInfoLog( uint32_t shader, int bufSize, int * length, char * infoLog )const
{
	m_pfnGetShaderInfoLog( shader, bufSize, length, infoLog );
	return glCheckError( *this, "glGetShaderInfoLog" );
}

bool OpenGl::ShaderSource( uint32_t shader, int count, char const ** strings, int const * lengths )const
{
	m_pfnShaderSource( shader, count, strings, lengths );
	return glCheckError( *this, "glShaderSource" );
}

uint32_t OpenGl::CreateProgram()const
{
	uint32_t l_uiReturn = m_pfnCreateProgram();

	if ( ! glCheckError( *this, "glCreateProgram" ) )
	{
		l_uiReturn = eGL_INVALID_INDEX;
	}

	return l_uiReturn;
}

bool OpenGl::DeleteProgram( uint32_t program )const
{
	m_pfnDeleteProgram( program );
	return glCheckError( *this, "glDeleteProgram" );
}

bool OpenGl::IsProgram( uint32_t program )const
{
	uint8_t l_return = m_pfnIsProgram( program );
	return glCheckError( *this, "glIsProgram" ) && l_return == eGL_TRUE;
}

bool OpenGl::LinkProgram( uint32_t program )const
{
	m_pfnLinkProgram( program );
	return glCheckError( *this, "glLinkProgram" );
}

bool OpenGl::UseProgram( uint32_t program )const
{
	m_pfnUseProgram( program );
	return glCheckError( *this, "glUseProgram" );
}

bool OpenGl::GetProgramiv( uint32_t program, uint32_t pname, int * param )const
{
	m_pfnGetProgramiv( program, pname, param );
	return glCheckError( *this, "glGetProgramiv" );
}

bool OpenGl::GetProgramInfoLog( uint32_t program, int bufSize, int * length, char * infoLog )const
{
	m_pfnGetProgramInfoLog( program, bufSize, length, infoLog );
	return glCheckError( *this, "glGetProgramInfoLog" );
}

int OpenGl::GetAttribLocation( uint32_t program, char const * name )const
{
	int l_iReturn = m_pfnGetAttribLocation( program, name );

	if ( ! glCheckError( *this, "glGetAttribLocation" ) )
	{
		l_iReturn = eGL_INVALID_INDEX;
	}

	return l_iReturn;
}

bool OpenGl::ProgramParameteri( uint32_t program, uint32_t pname, int value )const
{
	m_pfnProgramParameteri( program, pname, value );
	return glCheckError( *this, "glProgramParameteri" );
}

bool OpenGl::EnableVertexAttribArray( uint32_t index )const
{
	m_pfnEnableVertexAttribArray( index );
	return glCheckError( *this, "glEnableVertexAttribArray" );
}

bool OpenGl::VertexAttribPointer( uint32_t index, int size, eGL_TYPE type, bool normalized, int stride, void const * pointer )const
{
	m_pfnVertexAttribPointer( index, size, type, normalized, stride, pointer );
	return glCheckError( *this, "glVertexAttribPointer" );
}

bool OpenGl::VertexAttribPointer( uint32_t index, int size, eGL_TYPE type, int stride, void const * pointer )const
{
	m_pfnVertexAttribIPointer( index, size, type, stride, pointer );
	return glCheckError( *this, "glVertexAttribIPointer" );
}

bool OpenGl::DisableVertexAttribArray( uint32_t index )const
{
	m_pfnDisableVertexAttribArray( index );
	return glCheckError( *this, "glDisableVertexAttribArray" );
}

bool OpenGl::GenVertexArrays( int n, uint32_t * arrays )const
{
	m_pfnGenVertexArrays( n, arrays );
	return glCheckError( *this, "glGenVertexArrays" );
}

bool OpenGl::IsVertexArray( uint32_t array )const
{
	uint8_t l_return = m_pfnIsVertexArray( array );
	return glCheckError( *this, "glIsVertexArray" ) && l_return == eGL_TRUE;
}

bool OpenGl::BindVertexArray( uint32_t array )const
{
	m_pfnBindVertexArray( array );
	return glCheckError( *this, "glBindVertexArray" );
}

bool OpenGl::DeleteVertexArrays( int n, uint32_t const * arrays )const
{
	m_pfnDeleteVertexArrays( n, arrays );
	return glCheckError( *this, "glDeleteVertexArrays" );
}

bool OpenGl::PatchParameter( eGL_PATCH_PARAMETER p_eParam, int p_iValue )const
{
	bool l_return = false;

	if ( m_pfnPatchParameteri )
	{
		m_pfnPatchParameteri( p_eParam, p_iValue );
		l_return = glCheckError( *this, "glPatchParameteri" );
	}

	return l_return;
}

bool OpenGl::GenQueries( int p_n, uint32_t * p_queries )const
{
	m_pfnGenQueries( p_n, p_queries );
	return glCheckError( *this, "glGenQueries" );
}

bool OpenGl::DeleteQueries( int p_n, uint32_t const * p_queries )const
{
	m_pfnDeleteQueries( p_n, p_queries );
	return glCheckError( *this, "glDeleteQueries" );
}

bool OpenGl::IsQuery( uint32_t p_query )const
{
	uint8_t l_return = m_pfnIsQuery( p_query );
	return glCheckError( *this, "glIsQuery" ) && l_return == eGL_TRUE;
}

bool OpenGl::BeginQuery( eGL_QUERY p_target, uint32_t p_query )const
{
	m_pfnBeginQuery( p_target, p_query );
	return glCheckError( *this, "glBeginQuery" );
}

bool OpenGl::EndQuery( eGL_QUERY p_target )const
{
	m_pfnEndQuery( p_target );
	return glCheckError( *this, "glEndQuery" );
}

bool OpenGl::QueryCounter( uint32_t p_id, eGL_QUERY p_target )const
{
	m_pfnQueryCounter( p_id, p_target );
	return glCheckError( *this, "glQueryCounter" );
}

bool OpenGl::GetQueryObjectInfos( uint32_t p_id, eGL_QUERY_INFO p_name, int32_t * p_params )const
{
	m_pfnGetQueryObjectiv( p_id, p_name, p_params );
	return glCheckError( *this, "glGetQueryObjectiv" );
}

bool OpenGl::GetQueryObjectInfos( uint32_t p_id, eGL_QUERY_INFO p_name, uint32_t * p_params )const
{
	m_pfnGetQueryObjectuiv( p_id, p_name, p_params );
	return glCheckError( *this, "glGetQueryObjectuiv" );
}

bool OpenGl::GetQueryObjectInfos( uint32_t p_id, eGL_QUERY_INFO p_name, int64_t * p_params )const
{
	m_pfnGetQueryObjecti64v( p_id, p_name, p_params );
	return glCheckError( *this, "glGetQueryObjecti64v" );
}

bool OpenGl::GetQueryObjectInfos( uint32_t p_id, eGL_QUERY_INFO p_name, uint64_t * p_params )const
{
	m_pfnGetQueryObjectui64v( p_id, p_name, p_params );
	return glCheckError( *this, "glGetQueryObjectui64v" );
}

#if !defined( NDEBUG )

void OpenGl::Track( void * p_object, std::string const & p_name, std::string const & p_file, int p_line )const
{
	m_renderSystem.Track( p_object, p_name, p_file, p_line );
}

void OpenGl::UnTrack( void * p_object )const
{
	m_renderSystem.Untrack( p_object );
}

#endif

bool OpenGl::GlCheckError( std::string const & p_text )const
{
	return DoGlCheckError( string::string_cast< xchar >( p_text ) );
}

bool OpenGl::GlCheckError( std::wstring const & p_text )const
{
	return DoGlCheckError( string::string_cast< xchar >( p_text ) );
}

bool OpenGl::DoGlCheckError( String const & p_text )const
{
	bool l_return = true;
	uint32_t l_errorCode = GetError();

	if ( l_errorCode != GL_NO_ERROR )
	{
		String l_strSysError = System::GetLastErrorText();
		l_errorCode -= GL_INVALID_ENUM;
		String l_strError = p_text + cuT( " - " ) + GlslErrors[l_errorCode];

		if ( !l_strSysError.empty() )
		{
			l_strError += cuT( " - " ) + l_strSysError;
		}

		Logger::LogError( l_strError );
		l_return = false;
	}

	return l_return;
}

eGL_LOCK OpenGl::GetLockFlags( uint32_t p_flags )const
{
	eGL_LOCK l_eLockFlags = eGL_LOCK_READ_WRITE;

	if ( p_flags & eACCESS_TYPE_READ )
	{
		if ( p_flags & eACCESS_TYPE_WRITE )
		{
			l_eLockFlags = eGL_LOCK_READ_WRITE;
		}
		else
		{
			l_eLockFlags = eGL_LOCK_READ_ONLY;
		}
	}
	else if ( p_flags & eACCESS_TYPE_WRITE )
	{
		l_eLockFlags = eGL_LOCK_WRITE_ONLY;
	}

	return l_eLockFlags;
}

uint32_t OpenGl::GetBitfieldFlags( uint32_t p_flags )const
{
	uint32_t l_uiFlags = 0;

	if ( p_flags & eACCESS_TYPE_READ )
	{
		l_uiFlags = eGL_BUFFER_MAPPING_BIT_READ;

		if ( p_flags & eACCESS_TYPE_WRITE )
		{
			l_uiFlags |= eGL_BUFFER_MAPPING_BIT_WRITE;
		}
	}
	else if ( p_flags & eACCESS_TYPE_WRITE )
	{
		l_uiFlags = ( eGL_BUFFER_MAPPING_BIT_WRITE | eGL_BUFFER_MAPPING_BIT_INVALIDATE_RANGE );
	}

	return l_uiFlags;
}

void OpenGl::StDebugLog( eGL_DEBUG_SOURCE source, eGL_DEBUG_TYPE type, uint32_t id, eGL_DEBUG_SEVERITY severity, int length, const char * message, void * userParam )
{
	reinterpret_cast< OpenGl * >( userParam )->DebugLog( source, type, id, severity, length, message );
}

void OpenGl::StDebugLogAMD( uint32_t id, eGL_DEBUG_CATEGORY category, eGL_DEBUG_SEVERITY severity, int length, const char * message, void * userParam )
{
	reinterpret_cast< OpenGl * >( userParam )->DebugLogAMD( id, category, severity, length, message );
}

void OpenGl::DebugLog( eGL_DEBUG_SOURCE source, eGL_DEBUG_TYPE type, uint32_t id, eGL_DEBUG_SEVERITY severity, int CU_PARAM_UNUSED( length ), const char * message )const
{
	if ( id != 131185 )
	{
		String l_strToLog = cuT( "OpenGl Debug - " );

		switch ( source )
		{
		case eGL_DEBUG_SOURCE_API:
			l_strToLog += cuT( "Source:OpenGL\t" );
			break;

		case eGL_DEBUG_SOURCE_WINDOW_SYSTEM:
			l_strToLog += cuT( "Source:Windows\t" );
			break;

		case eGL_DEBUG_SOURCE_SHADER_COMPILER:
			l_strToLog += cuT( "Source:Shader compiler\t" );
			break;

		case eGL_DEBUG_SOURCE_THIRD_PARTY:
			l_strToLog += cuT( "Source:Third party\t" );
			break;

		case eGL_DEBUG_SOURCE_APPLICATION:
			l_strToLog += cuT( "Source:Application\t" );
			break;

		case eGL_DEBUG_SOURCE_OTHER:
			l_strToLog += cuT( "Source:Other\t" );
			break;
		}

		switch ( type )
		{
		case eGL_DEBUG_TYPE_ERROR:
			l_strToLog += cuT( "Type:Error\t" );
			break;

		case eGL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			l_strToLog += cuT( "Type:Deprecated behavior\t" );
			break;

		case eGL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			l_strToLog += cuT( "Type:Undefined behavior\t" );
			break;

		case eGL_DEBUG_TYPE_PORTABILITY:
			l_strToLog += cuT( "Type:Portability\t" );
			break;

		case eGL_DEBUG_TYPE_PERFORMANCE:
			l_strToLog += cuT( "Type:Performance\t" );
			break;

		case eGL_DEBUG_TYPE_OTHER:
			l_strToLog += cuT( "Type:Other\t" );
			break;
		}

		l_strToLog += cuT( "ID:" ) + string::to_string( id ) + cuT( "\t" );

		switch ( severity )
		{
		case eGL_DEBUG_SEVERITY_HIGH:
			l_strToLog += cuT( "Severity:High\t" );
			break;

		case eGL_DEBUG_SEVERITY_MEDIUM:
			l_strToLog += cuT( "Severity:Medium\t" );
			break;

		case eGL_DEBUG_SEVERITY_LOW:
			l_strToLog += cuT( "Severity:Low\t" );
			break;
		}

		Logger::LogWarning(	l_strToLog + cuT( "Message:" ) + string::string_cast< xchar >( message ) );
	}
}

void OpenGl::DebugLogAMD( uint32_t id, eGL_DEBUG_CATEGORY category, eGL_DEBUG_SEVERITY severity, int CU_PARAM_UNUSED( length ), const char * message )const
{
	String l_strToLog = cuT( "OpenGl Debug - " );

	switch ( category )
	{
	case eGL_DEBUG_CATEGORY_API_ERROR:
		l_strToLog += cuT( "Category:OpenGL\t" );
		break;

	case eGL_DEBUG_CATEGORY_WINDOW_SYSTEM:
		l_strToLog += cuT( "Category:Windows\t" );
		break;

	case eGL_DEBUG_CATEGORY_DEPRECATION:
		l_strToLog += cuT( "Category:Deprecated behavior\t" );
		break;

	case eGL_DEBUG_CATEGORY_UNDEFINED_BEHAVIOR:
		l_strToLog += cuT( "Category:Undefined behavior\t" );
		break;

	case eGL_DEBUG_CATEGORY_PERFORMANCE:
		l_strToLog += cuT( "Category:Performance\t" );
		break;

	case eGL_DEBUG_CATEGORY_SHADER_COMPILER:
		l_strToLog += cuT( "Category:Shader compiler\t" );
		break;

	case eGL_DEBUG_CATEGORY_APPLICATION:
		l_strToLog += cuT( "Category:Application\t" );
		break;

	case eGL_DEBUG_CATEGORY_OTHER:
		l_strToLog += cuT( "Category:Other\t" );
		break;
	}

	l_strToLog += cuT( "ID:" ) + string::to_string( id ) + cuT( "\t" );

	switch ( severity )
	{
	case eGL_DEBUG_SEVERITY_HIGH:
		l_strToLog += cuT( "Severity:High\t" );
		break;

	case eGL_DEBUG_SEVERITY_MEDIUM:
		l_strToLog += cuT( "Severity:Medium\t" );
		break;

	case eGL_DEBUG_SEVERITY_LOW:
		l_strToLog += cuT( "Severity:Low\t" );
		break;
	}

	Logger::LogWarning(	l_strToLog + cuT( "Message:" ) + string::string_cast< xchar >( message ) );
}

bool OpenGl::HasExtension( Castor::String const & p_strExtName )const
{
	bool l_return = m_extensions.find( p_strExtName ) != Castor::String::npos;

	if ( l_return )
	{
		Logger::LogDebug( cuT( "Extension [" ) + p_strExtName + cuT( "] available" ) );
	}
	else
	{
		Logger::LogWarning( cuT( "Extension [" ) + p_strExtName + cuT( "] unavailable" ) );
	}

	return l_return;
}

bool OpenGl::HasMultiTexturing()const
{
	return m_pfnActiveTexture && m_pfnClientActiveTexture;
}

//*************************************************************************************************
