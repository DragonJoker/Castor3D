#include "OpenGl.hpp"

#include <Rectangle.hpp>
#include <Logger.hpp>
#include <Utils.hpp>

using namespace GlRender;
using namespace Castor3D;
using namespace Castor;

#define DEF_USE_DIRECT_STATE_ACCESS	0

//*************************************************************************************************

MtxFunctionsBase::MtxFunctionsBase( OpenGl & p_gl )
	:	m_gl( p_gl	)
{
}

//*************************************************************************************************

MtxFunctions::MtxFunctions( OpenGl & p_gl )
	:	MtxFunctionsBase( p_gl	)
	,	m_pfnOrtho(	)
	,	m_pfnFrustum(	)
{
}

bool MtxFunctions::MatrixMode( eGL_MATRIX_TYPE mode )
{
	m_pfnMatrixMode( mode );
	return glCheckError( m_gl, cuT( "glMatrixMode" ) );
}

bool MtxFunctions::LoadIdentity()
{
	m_pfnLoadIdentity();
	return glCheckError( m_gl, cuT( "glLoadIdentity" ) );
}

bool MtxFunctions::Ortho( double left, double right, double bottom, double top, double zNear, double zFar )
{
	m_pfnOrtho( left, right, bottom, top, zNear, zFar );
	return glCheckError( m_gl, cuT( "glOrtho" ) );
}

bool MtxFunctions::Frustum( double left, double right, double bottom, double top, double zNear, double zFar )
{
	m_pfnFrustum( left, right, bottom, top, zNear, zFar );
	return glCheckError( m_gl, cuT( "glFrustum" ) );
}

bool MtxFunctions::PushMatrix()
{
	m_pfnPushMatrix();
	return glCheckError( m_gl, cuT( "glPushMatrix" ) );
}

bool MtxFunctions::PopMatrix()
{
	m_pfnPopMatrix();
	return glCheckError( m_gl, cuT( "glPopMatrix" ) );
}

bool MtxFunctions::Translate( real x, real y, real z )
{
	m_pfnTranslate( x, y, z );
	return glCheckError( m_gl, cuT( "glTranslate" ) );
}

bool MtxFunctions::Rotate( real angle, real x, real y, real z )
{
	m_pfnRotate( angle, x, y, z );
	return glCheckError( m_gl, cuT( "glRotate" ) );
}

bool MtxFunctions::Scale( real x, real y, real z )
{
	m_pfnScale( x, y, z );
	return glCheckError( m_gl, cuT( "glScale" ) );
}

bool MtxFunctions::MultMatrix( real const * matrix )
{
	m_pfnMultMatrix( matrix );
	return glCheckError( m_gl, cuT( "glMultMatrix" ) );
}

//*************************************************************************************************

MtxFunctionsDSA::MtxFunctionsDSA( OpenGl & p_gl )
	:	MtxFunctionsBase( p_gl )
	,	m_eMatrixMode( eGL_MATRIX_TYPE_MODELVIEW )
	,	m_pfnMatrixOrtho()
	,	m_pfnMatrixFrustum()
{
}

bool MtxFunctionsDSA::LoadIdentity()
{
	m_pfnMatrixLoadIdentity( m_eMatrixMode );
	return glCheckError( m_gl, cuT( "glMatrixLoadIdentity" ) );
}

bool MtxFunctionsDSA::Ortho( double left, double right, double bottom, double top, double zNear, double zFar )
{
	m_pfnMatrixOrtho( m_eMatrixMode, left, right, bottom, top, zNear, zFar );
	return glCheckError( m_gl, cuT( "glMatrixOrtho" ) );
}

bool MtxFunctionsDSA::Frustum( double left, double right, double bottom, double top, double zNear, double zFar )
{
	m_pfnMatrixFrustum( m_eMatrixMode, left, right, bottom, top, zNear, zFar );
	return glCheckError( m_gl, cuT( "glMatrixFrustum" ) );
}

bool MtxFunctionsDSA::PushMatrix()
{
	m_pfnMatrixPush( m_eMatrixMode );
	return glCheckError( m_gl, cuT( "glMatrixPush" ) );
}

bool MtxFunctionsDSA::PopMatrix()
{
	m_pfnMatrixPop( m_eMatrixMode );
	return glCheckError( m_gl, cuT( "glMatrixPop" ) );
}

bool MtxFunctionsDSA::Translate( real x, real y, real z )
{
#if CASTOR_USE_DOUBLE
	m_pfnMatrixTranslated( m_eMatrixMode, x, y, z );
#else
	m_pfnMatrixTranslatef( m_eMatrixMode, x, y, z );
#endif
	return glCheckError( m_gl, cuT( "glMatrixTranslate" ) );
}

bool MtxFunctionsDSA::Rotate( real angle, real x, real y, real z )
{
#if CASTOR_USE_DOUBLE
	m_pfnMatrixRotated( m_eMatrixMode, angle, x, y, z );
#else
	m_pfnMatrixRotatef( m_eMatrixMode, angle, x, y, z );
#endif
	return glCheckError( m_gl, cuT( "glMatrixRotate" ) );
}

bool MtxFunctionsDSA::Scale( real x, real y, real z )
{
#if CASTOR_USE_DOUBLE
	m_pfnMatrixScaled( m_eMatrixMode, x, y, z );
#else
	m_pfnMatrixScalef( m_eMatrixMode, x, y, z );
#endif
	return glCheckError( m_gl, cuT( "glMatrixScale" ) );
}

bool MtxFunctionsDSA::MultMatrix( real const * matrix )
{
#if CASTOR_USE_DOUBLE
	m_pfnMatrixMultd( m_eMatrixMode, matrix );
#else
	m_pfnMatrixMultf( m_eMatrixMode, matrix );
#endif
	return glCheckError( m_gl, cuT( "glMatrixMult" ) );
}

//*************************************************************************************************

TexFunctionsBase::TexFunctionsBase( OpenGl & p_gl )
	:	m_gl( p_gl )
{
}

//*************************************************************************************************

TexFunctions::TexFunctions( OpenGl & p_gl )
	:	TexFunctionsBase( p_gl )
	,	m_pfnTexSubImage1D()
	,	m_pfnTexSubImage2D()
	,	m_pfnTexSubImage3D()
	,	m_pfnTexImage1D()
	,	m_pfnTexImage2D()
	,	m_pfnTexImage3D()
{
}

bool TexFunctions::GenerateMipmap( eGL_TEXDIM mode )
{
	if ( m_pfnGenerateMipmap )
	{
		m_pfnGenerateMipmap( mode );
	}

	return glCheckError( m_gl, cuT( "glGenerateMipmap" ) );
}

bool TexFunctions::BindTexture( eGL_TEXDIM mode, uint32_t texture )
{
	m_pfnBindTexture( mode, texture );
	return glCheckError( m_gl, cuT( "glBindTexture" ) );
}

bool TexFunctions::TexSubImage1D( eGL_TEXDIM mode, int level, int xoffset, int width, eGL_FORMAT format, eGL_TYPE type, void const * data )
{
	m_pfnTexSubImage1D( mode, level, xoffset, width, format, type, data );
	return glCheckError( m_gl, cuT( "glTexSubImage1D" ) );
}

bool TexFunctions::TexSubImage2D( eGL_TEXDIM mode, int level, int xoffset, int yoffset, int width, int height, eGL_FORMAT format, eGL_TYPE type, void const * data )
{
	m_pfnTexSubImage2D( mode, level, xoffset, yoffset, width, height, format, type, data );
	return glCheckError( m_gl, cuT( "glTexSubImage2D" ) );
}

bool TexFunctions::TexSubImage2D( eGL_TEXDIM mode, int level, Position const & p_position, Size const & p_size, eGL_FORMAT format, eGL_TYPE type, void const * data )
{
	m_pfnTexSubImage2D( mode, level, p_position.x(), p_position.y(), p_size.width(), p_size.height(), format, type, data );
	return glCheckError( m_gl, cuT( "glTexSubImage2D" ) );
}

bool TexFunctions::TexSubImage2D( eGL_TEXDIM mode, int level, Castor::Rectangle const & p_rect, eGL_FORMAT format, eGL_TYPE type, void const * data )
{
	m_pfnTexSubImage2D( mode, level, p_rect.left(), p_rect.top(), p_rect.width(), p_rect.height(), format, type, data );
	return glCheckError( m_gl, cuT( "glTexSubImage2D" ) );
}

bool TexFunctions::TexSubImage3D( eGL_TEXDIM mode, int level, int xoffset, int yoffset, int zoffset, int width, int height, int depth, eGL_FORMAT format, eGL_TYPE type, void const * data )
{
	m_pfnTexSubImage3D( mode, level, xoffset, yoffset, zoffset, width, height, depth, format, type, data );
	return glCheckError( m_gl, cuT( "glTexSubImage3D" ) );
}

bool TexFunctions::TexImage1D( eGL_TEXDIM mode, int level, eGL_INTERNAL internalFormat, int width, int border, eGL_FORMAT format, uint32_t type, void const * data )
{
	try
	{
		m_pfnTexImage1D( mode, level, internalFormat, width, border, format, type, data );
		return glCheckError( m_gl, cuT( "glTexImage1D" ) );
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
		return glCheckError( m_gl, cuT( "glTexImage2D" ) );
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
		return glCheckError( m_gl, cuT( "glTexImage2D" ) );
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
		return glCheckError( m_gl, cuT( "glTexImage3D" ) );
	}
	catch ( ... )
	{
		return false;
	}
}

bool TexFunctions::TexParameter( eGL_TEXDIM mode, eGL_TEXTURE_PARAMETER pname, int param )
{
	m_pfnTexParameteri( mode, pname, param );
	return glCheckError( m_gl, cuT( "glTexParameteri" ) );
}

bool TexFunctions::TexParameter( eGL_TEXDIM mode, eGL_TEXTURE_PARAMETER pname, float param )
{
	m_pfnTexParameterf( mode, pname, param );
	return glCheckError( m_gl, cuT( "glTexParameterf" ) );
}

bool TexFunctions::TexParameter( eGL_TEXDIM mode, eGL_TEXTURE_PARAMETER pname, const int * params )
{
	m_pfnTexParameteriv( mode, pname, params );
	return glCheckError( m_gl, cuT( "glTexParameteriv" ) );
}

bool TexFunctions::TexParameter( eGL_TEXDIM mode, eGL_TEXTURE_PARAMETER pname, const float * params )
{
	m_pfnTexParameterfv( mode, pname, params );
	return glCheckError( m_gl, cuT( "glTexParameterfv" ) );
}

bool TexFunctions::GetTexImage( eGL_TEXDIM mode, int level, eGL_FORMAT format, eGL_TYPE type, void * img )
{
	m_pfnGetTexImage( mode, level, format, type, img );
	return glCheckError( m_gl, cuT( "glGetTexImage" ) );
}

//*************************************************************************************************

TexFunctionsDSA::TexFunctionsDSA( OpenGl & p_gl )
	:	TexFunctionsBase( p_gl )
	,	m_uiTexture( 0 )
	,	m_pfnTextureSubImage1D()
	,	m_pfnTextureSubImage2D()
	,	m_pfnTextureSubImage3D()
	,	m_pfnTextureImage1D()
	,	m_pfnTextureImage2D()
	,	m_pfnTextureImage3D()
{
}

bool TexFunctionsDSA::GenerateMipmap( eGL_TEXDIM mode )
{
	if ( m_pfnGenerateTextureMipmap )
	{
		m_pfnGenerateTextureMipmap( m_uiTexture, mode );
	}

	return glCheckError( m_gl, cuT( "glGenerateTextureMipmap" ) );
}

bool TexFunctionsDSA::TexSubImage1D( eGL_TEXDIM mode, int level, int xoffset, int width, eGL_FORMAT format, eGL_TYPE type, void const * data )
{
	m_pfnTextureSubImage1D( m_uiTexture, mode, level, xoffset, width, format, type, data );
	return glCheckError( m_gl, cuT( "glTextureSubImage1D" ) );
}

bool TexFunctionsDSA::TexSubImage2D( eGL_TEXDIM mode, int level, int xoffset, int yoffset, int width, int height, eGL_FORMAT format, eGL_TYPE type, void const * data )
{
	m_pfnTextureSubImage2D( m_uiTexture, mode, level, xoffset, yoffset, width, height, format, type, data );
	return glCheckError( m_gl, cuT( "glTextureSubImage2D" ) );
}

bool TexFunctionsDSA::TexSubImage2D( eGL_TEXDIM mode, int level, Position const & p_position, Size const & p_size, eGL_FORMAT format, eGL_TYPE type, void const * data )
{
	m_pfnTextureSubImage2D( m_uiTexture, mode, level, p_position.x(), p_position.y(), p_size.width(), p_size.height(), format, type, data );
	return glCheckError( m_gl, cuT( "glTextureSubImage2D" ) );
}

bool TexFunctionsDSA::TexSubImage2D( eGL_TEXDIM mode, int level, Castor::Rectangle const & p_rect, eGL_FORMAT format, eGL_TYPE type, void const * data )
{
	m_pfnTextureSubImage2D( m_uiTexture, mode, level, p_rect.left(), p_rect.top(), p_rect.width(), p_rect.height(), format, type, data );
	return glCheckError( m_gl, cuT( "glTextureSubImage2D" ) );
}

bool TexFunctionsDSA::TexSubImage3D( eGL_TEXDIM mode, int level, int xoffset, int yoffset, int zoffset, int width, int height, int depth, eGL_FORMAT format, eGL_TYPE type, void const * data )
{
	m_pfnTextureSubImage3D( m_uiTexture, mode, level, xoffset, yoffset, zoffset, width, height, depth, format, type, data );
	return glCheckError( m_gl, cuT( "glTextureSubImage3D" ) );
}

bool TexFunctionsDSA::TexImage1D( eGL_TEXDIM mode, int level, eGL_INTERNAL internalFormat, int width, int border, eGL_FORMAT format, uint32_t type, void const * data )
{
	try
	{
		m_pfnTextureImage1D( m_uiTexture, mode, level, internalFormat, width, border, format, type, data );
		return glCheckError( m_gl, cuT( "glTextureImage1D" ) );
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
		return glCheckError( m_gl, cuT( "glTextureImage2D" ) );
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
		return glCheckError( m_gl, cuT( "glTextureImage2D" ) );
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
		return glCheckError( m_gl, cuT( "glTextureImage3D" ) );
	}
	catch ( ... )
	{
		return false;
	}
}

bool TexFunctionsDSA::TexParameter( eGL_TEXDIM mode, eGL_TEXTURE_PARAMETER pname, int param )
{
	m_pfnTextureParameteri( m_uiTexture, mode, pname, param );
	return glCheckError( m_gl, cuT( "glTextureParameteri" ) );
}

bool TexFunctionsDSA::TexParameter( eGL_TEXDIM mode, eGL_TEXTURE_PARAMETER pname, float param )
{
	m_pfnTextureParameterf( m_uiTexture, mode, pname, param );
	return glCheckError( m_gl, cuT( "glTextureParameterf" ) );
}

bool TexFunctionsDSA::TexParameter( eGL_TEXDIM mode, eGL_TEXTURE_PARAMETER pname, const int * params )
{
	m_pfnTextureParameteriv( m_uiTexture, mode, pname, params );
	return glCheckError( m_gl, cuT( "glTextureParameteriv" ) );
}

bool TexFunctionsDSA::TexParameter( eGL_TEXDIM mode, eGL_TEXTURE_PARAMETER pname, const float * params )
{
	m_pfnTextureParameterfv( m_uiTexture, mode, pname, params );
	return glCheckError( m_gl, cuT( "glTextureParameterfv" ) );
}

bool TexFunctionsDSA::GetTexImage( eGL_TEXDIM mode, int level, eGL_FORMAT format, eGL_TYPE type, void * img )
{
	m_pfnGetTextureImage( m_uiTexture, mode, level, format, type, img );
	return glCheckError( m_gl, cuT( "glGetTextureImage" ) );
}

//*************************************************************************************************

BufFunctionsBase::BufFunctionsBase( OpenGl & p_gl )
	:	m_gl( p_gl )
{
}

bool BufFunctionsBase::EnableClientState( eGL_BUFFER_USAGE p_eArray )
{
	m_pfnEnableClientState( p_eArray );
	return glCheckError( m_gl, cuT( "glEnableClientState" ) );
}

bool BufFunctionsBase::DisableClientState( eGL_BUFFER_USAGE p_eArray )
{
	m_pfnDisableClientState( p_eArray );
	return glCheckError( m_gl, cuT( "glDisableClientState" ) );
}

bool BufFunctionsBase::BufferAddressRange( eGL_ADDRESS pname, uint32_t index, uint64_t address, size_t length )
{
	m_pfnBufferAddressRange( pname, index, address, length );
	return glCheckError( m_gl, cuT( "glBufferAddressRange" ) );
}

bool BufFunctionsBase::VertexFormat( int size, eGL_TYPE type, int stride )
{
	m_pfnVertexFormat( size, type, stride );
	return glCheckError( m_gl, cuT( "glVertexFormat" ) );
}

bool BufFunctionsBase::NormalFormat( eGL_TYPE type, int stride )
{
	m_pfnNormalFormat( type, stride );
	return glCheckError( m_gl, cuT( "glNormalFormat" ) );
}

bool BufFunctionsBase::ColorFormat( int size, eGL_TYPE type, int stride )
{
	m_pfnColorFormat( size, type, stride );
	return glCheckError( m_gl, cuT( "glColorFormat" ) );
}

bool BufFunctionsBase::IndexFormat( eGL_TYPE type, int stride )
{
	m_pfnIndexFormat( type, stride );
	return glCheckError( m_gl, cuT( "glIndexFormat" ) );
}

bool BufFunctionsBase::TexCoordFormat( int size, eGL_TYPE type, int stride )
{
	m_pfnTexCoordFormat( size, type, stride );
	return glCheckError( m_gl, cuT( "glTexCoordFormat" ) );
}

bool BufFunctionsBase::EdgeFlagFormat( int stride )
{
	m_pfnEdgeFlagFormat( stride );
	return glCheckError( m_gl, cuT( "glEdgeFlagFormat" ) );
}

bool BufFunctionsBase::SecondaryColorFormat( int size, eGL_TYPE type, int stride )
{
	m_pfnSecondaryColorFormat( size, type, stride );
	return glCheckError( m_gl, cuT( "glSecondaryColorFormat" ) );
}

bool BufFunctionsBase::FogCoordFormat( uint32_t type, int stride )
{
	m_pfnFogCoordFormat( type, stride );
	return glCheckError( m_gl, cuT( "glFogCoordFormat" ) );
}

bool BufFunctionsBase::VertexAttribFormat( uint32_t index, int size, eGL_TYPE type, bool normalized, int stride )
{
	m_pfnVertexAttribFormat( index, size, type, normalized, stride );
	return glCheckError( m_gl, cuT( "glVertexAttribFormat" ) );
}

bool BufFunctionsBase::VertexAttribIFormat( uint32_t index, int size, eGL_TYPE type, int stride )
{
	m_pfnVertexAttribIFormat( index, size, type, stride );
	return glCheckError( m_gl, cuT( "glVertexAttribIFormat" ) );
}

bool BufFunctionsBase::MakeBufferResident( eGL_BUFFER_TARGET target, eGL_RESIDENT_BUFFER_ACCESS access )
{
	m_pfnMakeBufferResident( target, access );
	return glCheckError( m_gl, cuT( "glMakeBufferResident" ) );
}

bool BufFunctionsBase::MakeBufferNonResident( eGL_BUFFER_TARGET target )
{
	m_pfnMakeBufferNonResident( target );
	return glCheckError( m_gl, cuT( "glMakeBufferNonResident" ) );
}

bool BufFunctionsBase::IsBufferResident( eGL_BUFFER_TARGET target )
{
	return m_pfnIsBufferResident( target ) && glCheckError( m_gl, cuT( "glIsBufferResident" ) );
}

bool BufFunctionsBase::MakeNamedBufferResident( uint32_t buffer, eGL_RESIDENT_BUFFER_ACCESS access )
{
	m_pfnMakeNamedBufferResident( buffer, access );
	return glCheckError( m_gl, cuT( "glMakeNamedBufferResident" ) );
}

bool BufFunctionsBase::MakeNamedBufferNonResident( uint32_t buffer )
{
	m_pfnMakeNamedBufferNonResident( buffer );
	return glCheckError( m_gl, cuT( "glMakeNamedBufferNonResident" ) );
}

bool BufFunctionsBase::IsNamedBufferResident( uint32_t buffer )
{
	return m_pfnIsNamedBufferResident( buffer ) && glCheckError( m_gl, cuT( "glIsNamedBufferResident" ) );
}

bool BufFunctionsBase::GetBufferParameter( eGL_BUFFER_TARGET target, eGL_BUFFER_PARAMETER pname, uint64_t * params )
{
	m_pfnGetBufferParameterui64v( target, pname, params );
	return glCheckError( m_gl, cuT( "glGetBufferParameterui64v" ) );
}

bool BufFunctionsBase::GetNamedBufferParameter( uint32_t buffer, eGL_BUFFER_PARAMETER pname,  uint64_t * params )
{
	m_pfnGetNamedBufferParameterui64v( buffer, pname, params );
	return glCheckError( m_gl, cuT( "glGetNamedBufferParameterui64v" ) );
}

//*************************************************************************************************

BufFunctions::BufFunctions( OpenGl & p_gl )
	:	BufFunctionsBase( p_gl )
{
}

bool BufFunctions::BindBuffer( eGL_BUFFER_TARGET p_eTarget, uint32_t buffer )
{
	m_pfnBindBuffer( p_eTarget, buffer );
	return glCheckError( m_gl, cuT( "glBindBuffer" ) );
}

bool BufFunctions::BufferData( eGL_BUFFER_TARGET p_eTarget, ptrdiff_t size, void const * data, eGL_BUFFER_MODE usage )
{
	m_pfnBufferData( p_eTarget, size, data, usage );
	return glCheckError( m_gl, cuT( "glBufferData" ) );
}

bool BufFunctions::BufferSubData( eGL_BUFFER_TARGET p_eTarget, ptrdiff_t offset, ptrdiff_t size, void const * data )
{
	m_pfnBufferSubData( p_eTarget, offset, size, data );
	return glCheckError( m_gl, cuT( "glBufferSubData" ) );
}

bool BufFunctions::GetBufferParameter( eGL_BUFFER_TARGET p_eTarget, eGL_BUFFER_PARAMETER pname, int * params )
{
	m_pfnGetBufferParameteriv( p_eTarget, pname, params );
	return glCheckError( m_gl, cuT( "glGetBufferParameteriv" ) );
}

void * BufFunctions::MapBuffer( eGL_BUFFER_TARGET p_eTarget, eGL_LOCK access )
{
	void * l_pReturn = m_pfnMapBuffer( p_eTarget, access );

	if ( !glCheckError( m_gl, cuT( "glMapBuffer" ) ) )
	{
		l_pReturn = NULL;
	}

	return l_pReturn;
}

bool BufFunctions::UnmapBuffer( eGL_BUFFER_TARGET p_eTarget )
{
	m_pfnUnmapBuffer( p_eTarget );
	return glCheckError( m_gl, cuT( "glUnmapBuffer" ) );
}

void * BufFunctions::MapBufferRange( eGL_BUFFER_TARGET p_eTarget, ptrdiff_t offset, ptrdiff_t length, uint32_t access )
{
	void * l_pReturn = NULL;
//	if( length + offset < GL_BUFFER_SIZE )
	{
		l_pReturn = m_pfnMapBufferRange( p_eTarget, offset, length, access );

		if ( !glCheckError( m_gl, cuT( "glMapBufferRange" ) ) )
		{
			l_pReturn = NULL;
		}
	}
	return l_pReturn;
}

bool BufFunctions::FlushMappedBufferRange( eGL_BUFFER_TARGET p_eTarget, ptrdiff_t offset, ptrdiff_t length )
{
	m_pfnFlushMappedBufferRange( p_eTarget, offset, length );
	return glCheckError( m_gl, cuT( "glFlushMappedBufferRange" ) );
}

//*************************************************************************************************

BufFunctionsDSA::BufFunctionsDSA( OpenGl & p_gl )
	:	BufFunctionsBase( p_gl )
	,	m_uiBuffer( 0 )
{
}

bool BufFunctionsDSA::BufferData( eGL_BUFFER_TARGET p_eTarget, ptrdiff_t size, void const * data, eGL_BUFFER_MODE usage )
{
	m_pfnNamedBufferData( m_uiBuffer, size, data, usage );
	return glCheckError( m_gl, cuT( "glBufferData" ) );
}

bool BufFunctionsDSA::BufferSubData( eGL_BUFFER_TARGET p_eTarget, ptrdiff_t offset, ptrdiff_t size, void const * data )
{
	m_pfnNamedBufferSubData( m_uiBuffer, offset, size, data );
	return glCheckError( m_gl, cuT( "glBufferSubData" ) );
}

bool BufFunctionsDSA::GetBufferParameter( eGL_BUFFER_TARGET p_eTarget, eGL_BUFFER_PARAMETER pname, int * params )
{
	m_pfnGetNamedBufferParameteriv( m_uiBuffer, pname, params );
	return glCheckError( m_gl, cuT( "glGetBufferParameteriv" ) );
}

void * BufFunctionsDSA::MapBuffer( eGL_BUFFER_TARGET p_eTarget, eGL_LOCK access )
{
	void * l_pReturn = m_pfnMapNamedBuffer( m_uiBuffer, access );

	if ( !glCheckError( m_gl, cuT( "glMapBuffer" ) ) )
	{
		l_pReturn = NULL;
	}

	return l_pReturn;
}

bool BufFunctionsDSA::UnmapBuffer( eGL_BUFFER_TARGET p_eTarget )
{
	m_pfnUnmapNamedBuffer( m_uiBuffer );
	return glCheckError( m_gl, cuT( "glUnmapBuffer" ) );
}

void * BufFunctionsDSA::MapBufferRange( eGL_BUFFER_TARGET p_eTarget, ptrdiff_t offset, ptrdiff_t length, uint32_t access )
{
	void * l_pReturn = NULL;
//	if( length + offset < GL_BUFFER_SIZE )
	{
		l_pReturn = m_pfnMapNamedBufferRange( m_uiBuffer, offset, length, access );

		if ( !glCheckError( m_gl, cuT( "glMapBufferRange" ) ) )
		{
			l_pReturn = NULL;
		}
	}
	return l_pReturn;
}

bool BufFunctionsDSA::FlushMappedBufferRange( eGL_BUFFER_TARGET p_eTarget, ptrdiff_t offset, ptrdiff_t length )
{
	m_pfnFlushMappedNamedBufferRange( m_uiBuffer, offset, length );
	return glCheckError( m_gl, cuT( "glFlushMappedBufferRange" ) );
}

//*************************************************************************************************

#define CASTOR_DBG_WIN32 0

OpenGl::OpenGl()
	:	m_bHasAnisotropic( false )
	,	m_bHasInstancedDraw( false )
	,	m_bHasInstancedArrays( false )
	,	m_pMtxFunctions( NULL )
	,	m_pTexFunctions( NULL )
	,	m_pBufFunctions( NULL )
	,	m_pfnReadPixels()
	,	m_pfnBlitFramebuffer()
	,	m_pfnTexImage2DMultisample()
	,	m_pfnGetActiveUniform()
	,	m_pfnVertexAttribPointer()
	,	m_bBindVboToGpuAddress( false )
{
	uint32_t l_uiIndex = 0;
	GlslStrings[			l_uiIndex++							] =	cuT( "[e00] GLSL is not available!"	);
	GlslStrings[			l_uiIndex++							] =	cuT( "[e01] Not a valid program object!"	);
	GlslStrings[			l_uiIndex++							] =	cuT( "[e02] Not a valid object!"	);
	GlslStrings[			l_uiIndex++							] =	cuT( "[e03] Out of memory!"	);
	GlslStrings[			l_uiIndex++							] =	cuT( "[e04] Unknown compiler error!"	);
	GlslStrings[			l_uiIndex++							] =	cuT( "[e05] Linker log is not available!"	);
	GlslStrings[			l_uiIndex++							] =	cuT( "[e06] Compiler log is not available!"	);
	GlslStrings[			l_uiIndex							] =	cuT( "[Empty]"	);

	l_uiIndex = 0;
	GlslErrors[				l_uiIndex++							] =	cuT( "[500] Invalid Enum !"	);
	GlslErrors[				l_uiIndex++							] =	cuT( "[501] Invalid Value !"	);
	GlslErrors[				l_uiIndex++							] =	cuT( "[502] Invalid Operation !"	);
	GlslErrors[				l_uiIndex++							] =	cuT( "[503] Stack Overflow !"	);
	GlslErrors[				l_uiIndex++							] =	cuT( "[504] Stack Underflow !"	);
	GlslErrors[				l_uiIndex++							] =	cuT( "[505] Out of memory !"	);
	GlslErrors[				l_uiIndex++							] =	cuT( "[506] Invalid frame buffer operation"	);
	GlslErrors[				l_uiIndex							] =	cuT( "[Empty] Unknown Error"	);

	PrimitiveTypes[			eTOPOLOGY_POINTS					] =	eGL_PRIMITIVE_POINTS					;
	PrimitiveTypes[			eTOPOLOGY_LINES						] =	eGL_PRIMITIVE_LINES						;
	PrimitiveTypes[			eTOPOLOGY_LINE_LOOP					] =	eGL_PRIMITIVE_LINE_LOOP					;
	PrimitiveTypes[			eTOPOLOGY_LINE_STRIP				] =	eGL_PRIMITIVE_LINE_STRIP				;
	PrimitiveTypes[			eTOPOLOGY_TRIANGLES					] =	eGL_PRIMITIVE_TRIANGLES					;
	PrimitiveTypes[			eTOPOLOGY_TRIANGLE_STRIPS			] =	eGL_PRIMITIVE_TRIANGLE_STRIP			;
	PrimitiveTypes[			eTOPOLOGY_TRIANGLE_FAN				] =	eGL_PRIMITIVE_TRIANGLE_FAN				;
	PrimitiveTypes[			eTOPOLOGY_QUADS						] =	eGL_PRIMITIVE_QUADS						;
	PrimitiveTypes[			eTOPOLOGY_QUAD_STRIPS				] =	eGL_PRIMITIVE_QUAD_STRIP				;
	PrimitiveTypes[			eTOPOLOGY_POLYGON					] =	eGL_PRIMITIVE_POLYGON					;

	TextureDimensions[		eTEXTURE_DIMENSION_1D				] =	eGL_TEXDIM_1D							;
	TextureDimensions[		eTEXTURE_DIMENSION_2D				] =	eGL_TEXDIM_2D							;
	TextureDimensions[		eTEXTURE_DIMENSION_3D				] =	eGL_TEXDIM_3D							;
	TextureDimensions[		eTEXTURE_DIMENSION_2DMS				] =	eGL_TEXDIM_2D_MULTISAMPLE				;
	TextureDimensions[		eTEXTURE_DIMENSION_2DARRAY			] =	eGL_TEXDIM_2D_ARRAY						;

	AlphaFuncs[				eALPHA_FUNC_ALWAYS					] =	eGL_FUNC_ALWAYS							;
	AlphaFuncs[				eALPHA_FUNC_LESS					] =	eGL_FUNC_LESS							;
	AlphaFuncs[				eALPHA_FUNC_LESS_OR_EQUAL			] =	eGL_FUNC_LEQUAL							;
	AlphaFuncs[				eALPHA_FUNC_EQUAL					] =	eGL_FUNC_EQUAL							;
	AlphaFuncs[				eALPHA_FUNC_NOT_EQUAL				] =	eGL_FUNC_NOTEQUAL						;
	AlphaFuncs[				eALPHA_FUNC_GREATER_OR_EQUAL		] =	eGL_FUNC_GEQUAL							;
	AlphaFuncs[				eALPHA_FUNC_GREATER					] =	eGL_FUNC_GREATER						;
	AlphaFuncs[				eALPHA_FUNC_NEVER					] =	eGL_FUNC_NEVER							;

	TextureWrapMode[		eWRAP_MODE_REPEAT					] =	eGL_WRAP_MODE_REPEAT					;
	TextureWrapMode[		eWRAP_MODE_MIRRORED_REPEAT			] =	eGL_WRAP_MODE_MIRRORED_REPEAT			;
	TextureWrapMode[		eWRAP_MODE_CLAMP_TO_BORDER			] =	eGL_WRAP_MODE_CLAMP_TO_BORDER			;
	TextureWrapMode[		eWRAP_MODE_CLAMP_TO_EDGE			] =	eGL_WRAP_MODE_CLAMP_TO_EDGE				;

	TextureInterpolation[	eINTERPOLATION_MODE_UNDEFINED		] =	eGL_INTERPOLATION_MODE_NEAREST			;
	TextureInterpolation[	eINTERPOLATION_MODE_NEAREST			] =	eGL_INTERPOLATION_MODE_NEAREST			;
	TextureInterpolation[	eINTERPOLATION_MODE_LINEAR			] =	eGL_INTERPOLATION_MODE_LINEAR			;
	TextureInterpolation[	eINTERPOLATION_MODE_ANISOTROPIC		] =	eGL_INTERPOLATION_MODE_LINEAR			;

	LightIndexes[			eLIGHT_INDEXES_0					] =	eGL_LIGHT_INDEX_0						;
	LightIndexes[			eLIGHT_INDEXES_1					] =	eGL_LIGHT_INDEX_1						;
	LightIndexes[			eLIGHT_INDEXES_2					] =	eGL_LIGHT_INDEX_2						;
	LightIndexes[			eLIGHT_INDEXES_3					] =	eGL_LIGHT_INDEX_3						;
	LightIndexes[			eLIGHT_INDEXES_4					] =	eGL_LIGHT_INDEX_4						;
	LightIndexes[			eLIGHT_INDEXES_5					] =	eGL_LIGHT_INDEX_5						;
	LightIndexes[			eLIGHT_INDEXES_6					] =	eGL_LIGHT_INDEX_6						;
	LightIndexes[			eLIGHT_INDEXES_7					] =	eGL_LIGHT_INDEX_7						;

	BlendFactors[			eBLEND_ZERO							] =	eGL_BLEND_FACTOR_ZERO					;
	BlendFactors[			eBLEND_ONE							] =	eGL_BLEND_FACTOR_ONE					;
	BlendFactors[			eBLEND_SRC_COLOUR					] =	eGL_BLEND_FACTOR_SRC_COLOR				;
	BlendFactors[			eBLEND_INV_SRC_COLOUR				] =	eGL_BLEND_FACTOR_ONE_MINUS_SRC_COLOR	;
	BlendFactors[			eBLEND_DST_COLOUR					] =	eGL_BLEND_FACTOR_DST_COLOR				;
	BlendFactors[			eBLEND_INV_DST_COLOUR				] =	eGL_BLEND_FACTOR_ONE_MINUS_DST_COLOR	;
	BlendFactors[			eBLEND_SRC_ALPHA					] =	eGL_BLEND_FACTOR_SRC_ALPHA				;
	BlendFactors[			eBLEND_INV_SRC_ALPHA				] =	eGL_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA	;
	BlendFactors[			eBLEND_DST_ALPHA					] =	eGL_BLEND_FACTOR_DST_ALPHA				;
	BlendFactors[			eBLEND_INV_DST_ALPHA				] =	eGL_BLEND_FACTOR_ONE_MINUS_DST_ALPHA	;
	BlendFactors[			eBLEND_CONSTANT						] =	eGL_BLEND_FACTOR_CONSTANT				;
	BlendFactors[			eBLEND_INV_CONSTANT					] =	eGL_BLEND_FACTOR_ONE_MINUS_CONSTANT		;
	BlendFactors[			eBLEND_SRC_ALPHA_SATURATE			] =	eGL_BLEND_FACTOR_SRC_ALPHA_SATURATE		;
	BlendFactors[			eBLEND_SRC1_COLOUR					] =	eGL_BLEND_FACTOR_SRC1_COLOR				;
	BlendFactors[			eBLEND_INV_SRC1_COLOUR				] =	eGL_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR	;
	BlendFactors[			eBLEND_SRC1_ALPHA					] =	eGL_BLEND_FACTOR_SRC1_ALPHA				;
	BlendFactors[			eBLEND_INV_SRC1_ALPHA				] =	eGL_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA	;

	Usages[					eELEMENT_USAGE_POSITION				] =	eGL_BUFFER_USAGE_VERTEX_ARRAY			;
	Usages[					eELEMENT_USAGE_NORMAL				] =	eGL_BUFFER_USAGE_NORMAL_ARRAY			;
	Usages[					eELEMENT_USAGE_TANGENT				] =	eGL_BUFFER_USAGE_TANGENT_ARRAY			;
	Usages[					eELEMENT_USAGE_BITANGENT			] =	eGL_BUFFER_USAGE_BINORMAL_ARRAY			;
	Usages[					eELEMENT_USAGE_DIFFUSE				] =	eGL_BUFFER_USAGE_COLOR_ARRAY			;
	Usages[					eELEMENT_USAGE_TEXCOORDS0			] =	eGL_BUFFER_USAGE_TEXTURE_COORD_ARRAY	;
	Usages[					eELEMENT_USAGE_TEXCOORDS1			] =	eGL_BUFFER_USAGE_TEXTURE_COORD_ARRAY	;
	Usages[					eELEMENT_USAGE_TEXCOORDS2			] =	eGL_BUFFER_USAGE_TEXTURE_COORD_ARRAY	;
	Usages[					eELEMENT_USAGE_TEXCOORDS3			] =	eGL_BUFFER_USAGE_TEXTURE_COORD_ARRAY	;

	TextureArguments[		eBLEND_SOURCE_TEXTURE				] =	eGL_BLEND_SOURCE_TEXTURE				;
	TextureArguments[		eBLEND_SOURCE_TEXTURE0				] =	eGL_BLEND_SOURCE_TEXTURE0				;
	TextureArguments[		eBLEND_SOURCE_TEXTURE1				] =	eGL_BLEND_SOURCE_TEXTURE1				;
	TextureArguments[		eBLEND_SOURCE_TEXTURE2				] =	eGL_BLEND_SOURCE_TEXTURE2				;
	TextureArguments[		eBLEND_SOURCE_TEXTURE3				] =	eGL_BLEND_SOURCE_TEXTURE3				;
	TextureArguments[		eBLEND_SOURCE_CONSTANT				] =	eGL_BLEND_SOURCE_CONSTANT				;
	TextureArguments[		eBLEND_SOURCE_DIFFUSE				] =	eGL_BLEND_SOURCE_PRIMARY_COLOR			;
	TextureArguments[		eBLEND_SOURCE_PREVIOUS				] =	eGL_BLEND_SOURCE_PREVIOUS				;

	RgbBlendFuncs[			eRGB_BLEND_FUNC_NONE				] =	eGL_BLEND_FUNC_MODULATE					;
	RgbBlendFuncs[			eRGB_BLEND_FUNC_FIRST_ARG			] =	eGL_BLEND_FUNC_REPLACE					;
	RgbBlendFuncs[			eRGB_BLEND_FUNC_ADD					] =	eGL_BLEND_FUNC_ADD						;
	RgbBlendFuncs[			eRGB_BLEND_FUNC_ADD_SIGNED			] =	eGL_BLEND_FUNC_ADD_SIGNED				;
	RgbBlendFuncs[			eRGB_BLEND_FUNC_MODULATE			] =	eGL_BLEND_FUNC_MODULATE					;
	RgbBlendFuncs[			eRGB_BLEND_FUNC_INTERPOLATE			] =	eGL_BLEND_FUNC_INTERPOLATE				;
	RgbBlendFuncs[			eRGB_BLEND_FUNC_SUBTRACT			] =	eGL_BLEND_FUNC_SUBTRACT					;
	RgbBlendFuncs[			eRGB_BLEND_FUNC_DOT3_RGB			] =	eGL_BLEND_FUNC_DOT3_RGB					;
	RgbBlendFuncs[			eRGB_BLEND_FUNC_DOT3_RGBA			] =	eGL_BLEND_FUNC_DOT3_RGBA				;

	AlphaBlendFuncs[		eALPHA_BLEND_FUNC_NONE				] =	eGL_BLEND_FUNC_MODULATE					;
	AlphaBlendFuncs[		eALPHA_BLEND_FUNC_FIRST_ARG			] =	eGL_BLEND_FUNC_REPLACE					;
	AlphaBlendFuncs[		eALPHA_BLEND_FUNC_ADD				] =	eGL_BLEND_FUNC_ADD						;
	AlphaBlendFuncs[		eALPHA_BLEND_FUNC_ADD_SIGNED		] =	eGL_BLEND_FUNC_ADD_SIGNED				;
	AlphaBlendFuncs[		eALPHA_BLEND_FUNC_MODULATE			] =	eGL_BLEND_FUNC_MODULATE					;
	AlphaBlendFuncs[		eALPHA_BLEND_FUNC_INTERPOLATE		] =	eGL_BLEND_FUNC_INTERPOLATE				;
	AlphaBlendFuncs[		eALPHA_BLEND_FUNC_SUBSTRACT			] =	eGL_BLEND_FUNC_SUBTRACT					;

	BlendOps[				eBLEND_OP_ADD						] =	eGL_BLEND_OP_ADD						;
	BlendOps[				eBLEND_OP_SUBSTRACT					] =	eGL_BLEND_OP_SUBTRACT					;
	BlendOps[				eBLEND_OP_REV_SUBSTRACT				] =	eGL_BLEND_OP_REV_SUBTRACT				;
	BlendOps[				eBLEND_OP_MIN						] =	eGL_BLEND_OP_MIN						;
	BlendOps[				eBLEND_OP_MAX						] =	eGL_BLEND_OP_MAX						;

	PixelFormats[			ePIXEL_FORMAT_L8					] = PixelFmt( eGL_FORMAT_RED,				eGL_INTERNAL_R8,						eGL_TYPE_UNSIGNED_BYTE	);
	PixelFormats[			ePIXEL_FORMAT_L16F32F				] = PixelFmt( eGL_FORMAT_RED,				eGL_INTERNAL_R16F,						eGL_TYPE_FLOAT	);
	PixelFormats[			ePIXEL_FORMAT_L32F					] = PixelFmt( eGL_FORMAT_RED,				eGL_INTERNAL_R32F,						eGL_TYPE_FLOAT	);
	PixelFormats[			ePIXEL_FORMAT_A8L8					] = PixelFmt( eGL_FORMAT_RG,				eGL_INTERNAL_RG16,						eGL_TYPE_UNSIGNED_BYTE	);
	PixelFormats[			ePIXEL_FORMAT_AL16F32F				] = PixelFmt( eGL_FORMAT_RG,				eGL_INTERNAL_RG16F,						eGL_TYPE_FLOAT	);
	PixelFormats[			ePIXEL_FORMAT_AL32F					] = PixelFmt( eGL_FORMAT_RG,				eGL_INTERNAL_RG32F,						eGL_TYPE_FLOAT	);
	PixelFormats[			ePIXEL_FORMAT_A1R5G5B5				] = PixelFmt( eGL_FORMAT_BGRA,				eGL_INTERNAL_RGB5_A1,					eGL_TYPE_UNSIGNED_SHORT_1_5_5_5_REV	);
	PixelFormats[			ePIXEL_FORMAT_A4R4G4B4				] = PixelFmt( eGL_FORMAT_BGRA,				eGL_INTERNAL_RGBA4,						eGL_TYPE_UNSIGNED_SHORT_4_4_4_4_REV	);
	PixelFormats[			ePIXEL_FORMAT_R5G6B5				] = PixelFmt( eGL_FORMAT_BGR,				eGL_INTERNAL_RGB16,						eGL_TYPE_UNSIGNED_SHORT_5_6_5_REV	);
	PixelFormats[			ePIXEL_FORMAT_R8G8B8				] = PixelFmt( eGL_FORMAT_BGR,				eGL_INTERNAL_RGB8,						eGL_TYPE_UNSIGNED_BYTE	);
	PixelFormats[			ePIXEL_FORMAT_A8R8G8B8				] = PixelFmt( eGL_FORMAT_BGRA,				eGL_INTERNAL_RGBA8,						eGL_TYPE_UNSIGNED_BYTE	);
	PixelFormats[			ePIXEL_FORMAT_RGB16F32F				] = PixelFmt( eGL_FORMAT_RGB,				eGL_INTERNAL_RGB16F,					eGL_TYPE_FLOAT	);
	PixelFormats[			ePIXEL_FORMAT_ARGB16F32F			] = PixelFmt( eGL_FORMAT_RGBA,				eGL_INTERNAL_RGBA16F,					eGL_TYPE_FLOAT	);
	PixelFormats[			ePIXEL_FORMAT_RGB32F				] = PixelFmt( eGL_FORMAT_RGB,				eGL_INTERNAL_RGB32F,					eGL_TYPE_FLOAT	);
	PixelFormats[			ePIXEL_FORMAT_ARGB32F				] = PixelFmt( eGL_FORMAT_BGRA,				eGL_INTERNAL_RGBA32F,					eGL_TYPE_FLOAT	);
	PixelFormats[			ePIXEL_FORMAT_DXTC1					] = PixelFmt( eGL_FORMAT_BGR,				eGL_INTERNAL_COMPRESSED_RGBA_S3TC_DXT1,	eGL_TYPE_DEFAULT	);
	PixelFormats[			ePIXEL_FORMAT_DXTC3					] = PixelFmt( eGL_FORMAT_BGRA,				eGL_INTERNAL_COMPRESSED_RGBA_S3TC_DXT3,	eGL_TYPE_DEFAULT	);
	PixelFormats[			ePIXEL_FORMAT_DXTC5					] = PixelFmt( eGL_FORMAT_RGBA,				eGL_INTERNAL_COMPRESSED_RGBA_S3TC_DXT5,	eGL_TYPE_DEFAULT	);
	PixelFormats[			ePIXEL_FORMAT_YUY2					] = PixelFmt( eGL_FORMAT( 0 ),				eGL_INTERNAL( 0 ),						eGL_TYPE_DEFAULT	);
	PixelFormats[			ePIXEL_FORMAT_DEPTH16				] = PixelFmt( eGL_FORMAT_DEPTH,				eGL_INTERNAL_DEPTH_COMPONENT16,			eGL_TYPE_UNSIGNED_BYTE	);
	PixelFormats[			ePIXEL_FORMAT_DEPTH24				] = PixelFmt( eGL_FORMAT_DEPTH,				eGL_INTERNAL_DEPTH_COMPONENT24,			eGL_TYPE_UNSIGNED_BYTE	);
	PixelFormats[			ePIXEL_FORMAT_DEPTH24S8				] = PixelFmt( eGL_FORMAT_DEPTH_STENCIL,		eGL_INTERNAL_DEPTH24_STENCIL8,			eGL_TYPE_UNSIGNED_INT_24_8	);
	PixelFormats[			ePIXEL_FORMAT_DEPTH32				] = PixelFmt( eGL_FORMAT_DEPTH,				eGL_INTERNAL_DEPTH_COMPONENT32,			eGL_TYPE_UNSIGNED_BYTE	);
	PixelFormats[			ePIXEL_FORMAT_STENCIL1				] = PixelFmt( eGL_FORMAT_STENCIL,			eGL_INTERNAL_STENCIL_INDEX1,			eGL_TYPE_UNSIGNED_BYTE	);
	PixelFormats[			ePIXEL_FORMAT_STENCIL8				] = PixelFmt( eGL_FORMAT_STENCIL,			eGL_INTERNAL_STENCIL_INDEX8,			eGL_TYPE_UNSIGNED_BYTE	);

	ShaderTypes[			eSHADER_TYPE_VERTEX					] = eGL_SHADER_TYPE_VERTEX					;
	ShaderTypes[			eSHADER_TYPE_HULL					] = eGL_SHADER_TYPE_TESS_CONTROL			;
	ShaderTypes[			eSHADER_TYPE_DOMAIN					] = eGL_SHADER_TYPE_TESS_EVALUATION			;
	ShaderTypes[			eSHADER_TYPE_GEOMETRY				] = eGL_SHADER_TYPE_GEOMETRY				;
	ShaderTypes[			eSHADER_TYPE_PIXEL					] = eGL_SHADER_TYPE_FRAGMENT				;
	ShaderTypes[			eSHADER_TYPE_COMPUTE				] = eGL_SHADER_TYPE_COMPUTE					;

	Internals[				ePIXEL_FORMAT_L8					] =	eGL_INTERNAL_FORMAT_R16					;
	Internals[				ePIXEL_FORMAT_L16F32F				] =	eGL_INTERNAL_FORMAT_R16F				;
	Internals[				ePIXEL_FORMAT_L32F					] =	eGL_INTERNAL_FORMAT_R32F				;
	Internals[				ePIXEL_FORMAT_A8L8					] =	eGL_INTERNAL_FORMAT_RG16				;
	Internals[				ePIXEL_FORMAT_AL16F32F				] =	eGL_INTERNAL_FORMAT_RG16F				;
	Internals[				ePIXEL_FORMAT_AL32F					] =	eGL_INTERNAL_FORMAT_RG32F				;
	Internals[				ePIXEL_FORMAT_A1R5G5B5				] =	eGL_INTERNAL_FORMAT_RGBA16UI			;
	Internals[				ePIXEL_FORMAT_A4R4G4B4				] =	eGL_INTERNAL_FORMAT_RGBA16UI			;
	Internals[				ePIXEL_FORMAT_R5G6B5				] =	eGL_INTERNAL_FORMAT_RGB16UI				;
	Internals[				ePIXEL_FORMAT_R8G8B8				] =	eGL_INTERNAL_FORMAT_RGBA32UI			;
	Internals[				ePIXEL_FORMAT_A8R8G8B8				] =	eGL_INTERNAL_FORMAT_RGBA32UI			;
	Internals[				ePIXEL_FORMAT_RGB16F32F				] =	eGL_INTERNAL_FORMAT_RGBA16F				;
	Internals[				ePIXEL_FORMAT_ARGB16F32F			] =	eGL_INTERNAL_FORMAT_RGBA16F				;
	Internals[				ePIXEL_FORMAT_RGB32F				] =	eGL_INTERNAL_FORMAT_RGBA32F				;
	Internals[				ePIXEL_FORMAT_ARGB32F				] =	eGL_INTERNAL_FORMAT_RGBA32F				;

	Attachments[			eATTACHMENT_POINT_NONE				] =	eGL_TEXTURE_ATTACHMENT_NONE				;
	Attachments[			eATTACHMENT_POINT_COLOUR0			] =	eGL_TEXTURE_ATTACHMENT_COLOR0			;
	Attachments[			eATTACHMENT_POINT_COLOUR1			] =	eGL_TEXTURE_ATTACHMENT_COLOR1			;
	Attachments[			eATTACHMENT_POINT_COLOUR2			] =	eGL_TEXTURE_ATTACHMENT_COLOR2			;
	Attachments[			eATTACHMENT_POINT_COLOUR3			] =	eGL_TEXTURE_ATTACHMENT_COLOR3			;
	Attachments[			eATTACHMENT_POINT_COLOUR4			] =	eGL_TEXTURE_ATTACHMENT_COLOR4			;
	Attachments[			eATTACHMENT_POINT_COLOUR5			] =	eGL_TEXTURE_ATTACHMENT_COLOR5			;
	Attachments[			eATTACHMENT_POINT_COLOUR6			] =	eGL_TEXTURE_ATTACHMENT_COLOR6			;
	Attachments[			eATTACHMENT_POINT_COLOUR7			] =	eGL_TEXTURE_ATTACHMENT_COLOR7			;
	Attachments[			eATTACHMENT_POINT_COLOUR8			] =	eGL_TEXTURE_ATTACHMENT_COLOR8			;
	Attachments[			eATTACHMENT_POINT_COLOUR9			] =	eGL_TEXTURE_ATTACHMENT_COLOR9			;
	Attachments[			eATTACHMENT_POINT_COLOUR10			] =	eGL_TEXTURE_ATTACHMENT_COLOR10			;
	Attachments[			eATTACHMENT_POINT_COLOUR11			] =	eGL_TEXTURE_ATTACHMENT_COLOR11			;
	Attachments[			eATTACHMENT_POINT_COLOUR12			] =	eGL_TEXTURE_ATTACHMENT_COLOR12			;
	Attachments[			eATTACHMENT_POINT_COLOUR13			] =	eGL_TEXTURE_ATTACHMENT_COLOR13			;
	Attachments[			eATTACHMENT_POINT_COLOUR14			] =	eGL_TEXTURE_ATTACHMENT_COLOR14			;
	Attachments[			eATTACHMENT_POINT_COLOUR15			] =	eGL_TEXTURE_ATTACHMENT_COLOR15			;
	Attachments[			eATTACHMENT_POINT_DEPTH				] =	eGL_TEXTURE_ATTACHMENT_DEPTH			;
	Attachments[			eATTACHMENT_POINT_STENCIL			] =	eGL_TEXTURE_ATTACHMENT_STENCIL			;

	FramebufferModes[		eFRAMEBUFFER_TARGET_DRAW			] =	eGL_FRAMEBUFFER_MODE_DRAW				;
	FramebufferModes[		eFRAMEBUFFER_TARGET_READ			] =	eGL_FRAMEBUFFER_MODE_READ				;
	FramebufferModes[		eFRAMEBUFFER_TARGET_BOTH			] =	eGL_FRAMEBUFFER_MODE_DEFAULT			;

	RboAttachments[			eATTACHMENT_POINT_NONE				] = eGL_RENDERBUFFER_ATTACHMENT_NONE		;
	RboAttachments[			eATTACHMENT_POINT_COLOUR0			] = eGL_RENDERBUFFER_ATTACHMENT_COLOR0		;
	RboAttachments[			eATTACHMENT_POINT_COLOUR1			] = eGL_RENDERBUFFER_ATTACHMENT_COLOR1		;
	RboAttachments[			eATTACHMENT_POINT_COLOUR2			] = eGL_RENDERBUFFER_ATTACHMENT_COLOR2		;
	RboAttachments[			eATTACHMENT_POINT_COLOUR3			] = eGL_RENDERBUFFER_ATTACHMENT_COLOR3		;
	RboAttachments[			eATTACHMENT_POINT_COLOUR4			] = eGL_RENDERBUFFER_ATTACHMENT_COLOR4		;
	RboAttachments[			eATTACHMENT_POINT_COLOUR5			] = eGL_RENDERBUFFER_ATTACHMENT_COLOR5		;
	RboAttachments[			eATTACHMENT_POINT_COLOUR6			] = eGL_RENDERBUFFER_ATTACHMENT_COLOR6		;
	RboAttachments[			eATTACHMENT_POINT_COLOUR7			] = eGL_RENDERBUFFER_ATTACHMENT_COLOR7		;
	RboAttachments[			eATTACHMENT_POINT_COLOUR8			] = eGL_RENDERBUFFER_ATTACHMENT_COLOR8		;
	RboAttachments[			eATTACHMENT_POINT_COLOUR9			] = eGL_RENDERBUFFER_ATTACHMENT_COLOR9		;
	RboAttachments[			eATTACHMENT_POINT_COLOUR10			] = eGL_RENDERBUFFER_ATTACHMENT_COLOR10		;
	RboAttachments[			eATTACHMENT_POINT_COLOUR11			] = eGL_RENDERBUFFER_ATTACHMENT_COLOR11		;
	RboAttachments[			eATTACHMENT_POINT_COLOUR12			] = eGL_RENDERBUFFER_ATTACHMENT_COLOR12		;
	RboAttachments[			eATTACHMENT_POINT_COLOUR13			] = eGL_RENDERBUFFER_ATTACHMENT_COLOR13		;
	RboAttachments[			eATTACHMENT_POINT_COLOUR14			] = eGL_RENDERBUFFER_ATTACHMENT_COLOR14		;
	RboAttachments[			eATTACHMENT_POINT_COLOUR15			] = eGL_RENDERBUFFER_ATTACHMENT_COLOR15		;
	RboAttachments[			eATTACHMENT_POINT_DEPTH				] = eGL_RENDERBUFFER_ATTACHMENT_DEPTH		;
	RboAttachments[			eATTACHMENT_POINT_STENCIL			] = eGL_RENDERBUFFER_ATTACHMENT_STENCIL		;

	RboStorages[			ePIXEL_FORMAT_L8					] =	eGL_RENDERBUFFER_STORAGE_L8				;
	RboStorages[			ePIXEL_FORMAT_L16F32F				] =	eGL_RENDERBUFFER_STORAGE_L16F			;
	RboStorages[			ePIXEL_FORMAT_L32F					] =	eGL_RENDERBUFFER_STORAGE_L32F			;
	RboStorages[			ePIXEL_FORMAT_A8L8					] =	eGL_RENDERBUFFER_STORAGE_A8L8			;
	RboStorages[			ePIXEL_FORMAT_AL16F32F				] =	eGL_RENDERBUFFER_STORAGE_AL16F			;
	RboStorages[			ePIXEL_FORMAT_AL32F					] =	eGL_RENDERBUFFER_STORAGE_AL32F			;
	RboStorages[			ePIXEL_FORMAT_A1R5G5B5				] =	eGL_RENDERBUFFER_STORAGE_RGB5_A1		;
	RboStorages[			ePIXEL_FORMAT_A4R4G4B4				] =	eGL_RENDERBUFFER_STORAGE_RGBA4			;
	RboStorages[			ePIXEL_FORMAT_R5G6B5				] =	eGL_RENDERBUFFER_STORAGE_RGB565			;
	RboStorages[			ePIXEL_FORMAT_R8G8B8				] =	eGL_RENDERBUFFER_STORAGE_RGB8			;
	RboStorages[			ePIXEL_FORMAT_A8R8G8B8				] =	eGL_RENDERBUFFER_STORAGE_RGBA8			;
	RboStorages[			ePIXEL_FORMAT_RGB16F32F				] =	eGL_RENDERBUFFER_STORAGE_RGB16F			;
	RboStorages[			ePIXEL_FORMAT_ARGB16F32F			] =	eGL_RENDERBUFFER_STORAGE_RGBA16F		;
	RboStorages[			ePIXEL_FORMAT_RGB32F				] =	eGL_RENDERBUFFER_STORAGE_RGB32F			;
	RboStorages[			ePIXEL_FORMAT_ARGB32F				] =	eGL_RENDERBUFFER_STORAGE_RGBA32F		;
	RboStorages[			ePIXEL_FORMAT_DXTC1					] =	eGL_RENDERBUFFER_STORAGE_DXTC1			;
	RboStorages[			ePIXEL_FORMAT_DXTC3					] =	eGL_RENDERBUFFER_STORAGE_DXTC3			;
	RboStorages[			ePIXEL_FORMAT_DXTC5					] =	eGL_RENDERBUFFER_STORAGE_DXTC5			;
	RboStorages[			ePIXEL_FORMAT_YUY2					] =	eGL_RENDERBUFFER_STORAGE( 0 )			;
	RboStorages[			ePIXEL_FORMAT_DEPTH16				] =	eGL_RENDERBUFFER_STORAGE_DEPTH16		;
	RboStorages[			ePIXEL_FORMAT_DEPTH24				] =	eGL_RENDERBUFFER_STORAGE_DEPTH24		;
	RboStorages[			ePIXEL_FORMAT_DEPTH24S8				] =	eGL_RENDERBUFFER_STORAGE_DEPTH24		;
	RboStorages[			ePIXEL_FORMAT_DEPTH32				] =	eGL_RENDERBUFFER_STORAGE_DEPTH32		;
	RboStorages[			ePIXEL_FORMAT_STENCIL1				] =	eGL_RENDERBUFFER_STORAGE_STENCIL1		;
	RboStorages[			ePIXEL_FORMAT_STENCIL8				] =	eGL_RENDERBUFFER_STORAGE_STENCIL8		;

	Tweaks[					eTWEAK_DEPTH_TEST					] =	eGL_TWEAK_DEPTH_TEST					;
	Tweaks[					eTWEAK_STENCIL_TEST					] =	eGL_TWEAK_STENCIL_TEST					;
	Tweaks[					eTWEAK_BLEND						] =	eGL_TWEAK_BLEND							;
	Tweaks[					eTWEAK_LIGHTING						] =	eGL_TWEAK_LIGHTING						;
	Tweaks[					eTWEAK_ALPHA_TEST					] =	eGL_TWEAK_ALPHA_TEST					;
	Tweaks[					eTWEAK_CULLING						] =	eGL_TWEAK_CULL_FACE						;
	Tweaks[					eTWEAK_DITHERING					] =	eGL_TWEAK_DITHER						;
	Tweaks[					eTWEAK_FOG							] =	eGL_TWEAK_FOG							;
	Tweaks[					eTWEAK_DEPTH_WRITE					] =	eGL_TWEAK( -1 )							;
	Tweaks[					eTWEAK_ALPHA_TO_COVERAGE			] =	eGL_TWEAK_ALPHA_TO_COVERAGE				;

	Buffers[				eBUFFER_NONE						] =	eGL_BUFFER_NONE							;
	Buffers[				eBUFFER_FRONT_LEFT					] =	eGL_BUFFER_FRONT_LEFT					;
	Buffers[				eBUFFER_FRONT_RIGHT					] =	eGL_BUFFER_FRONT_RIGHT					;
	Buffers[				eBUFFER_BACK_LEFT					] =	eGL_BUFFER_BACK_LEFT					;
	Buffers[				eBUFFER_BACK_RIGHT					] =	eGL_BUFFER_BACK_RIGHT					;
	Buffers[				eBUFFER_FRONT						] =	eGL_BUFFER_FRONT						;
	Buffers[				eBUFFER_BACK						] =	eGL_BUFFER_BACK							;
	Buffers[				eBUFFER_LEFT						] =	eGL_BUFFER_LEFT							;
	Buffers[				eBUFFER_RIGHT						] =	eGL_BUFFER_RIGHT						;
	Buffers[				eBUFFER_FRONT_AND_BACK				] =	eGL_BUFFER_FRONT_AND_BACK				;

	Faces[					eFACE_NONE							] =	eGL_FACE( 0 )							;
	Faces[					eFACE_FRONT							] =	eGL_FACE_FRONT							;
	Faces[					eFACE_BACK							] =	eGL_FACE_BACK							;
	Faces[					eFACE_FRONT_AND_BACK				] =	eGL_FACE_FRONT_AND_BACK					;

	FillModes[				eFILL_MODE_POINT					] =	eGL_FILL_MODE_POINT						;
	FillModes[				eFILL_MODE_LINE						] =	eGL_FILL_MODE_LINE						;
	FillModes[				eFILL_MODE_SOLID					] =	eGL_FILL_MODE_FILL						;

	DepthFuncs[				eDEPTH_FUNC_NEVER					] =	eGL_FUNC_NEVER							;
	DepthFuncs[				eDEPTH_FUNC_LESS					] =	eGL_FUNC_LESS							;
	DepthFuncs[				eDEPTH_FUNC_EQUAL					] =	eGL_FUNC_LEQUAL							;
	DepthFuncs[				eDEPTH_FUNC_LEQUAL					] =	eGL_FUNC_EQUAL							;
	DepthFuncs[				eDEPTH_FUNC_GREATER					] =	eGL_FUNC_NOTEQUAL						;
	DepthFuncs[				eDEPTH_FUNC_NOTEQUAL				] =	eGL_FUNC_GEQUAL							;
	DepthFuncs[				eDEPTH_FUNC_GEQUAL					] =	eGL_FUNC_GREATER						;
	DepthFuncs[				eDEPTH_FUNC_ALWAYS					] =	eGL_FUNC_ALWAYS							;

	WriteMasks[				eWRITING_MASK_ZERO					] =	false									;
	WriteMasks[				eWRITING_MASK_ALL					] =	true									;

	StencilFuncs[			eSTENCIL_FUNC_NEVER					] =	eGL_FUNC_NEVER							;
	StencilFuncs[			eSTENCIL_FUNC_LESS					] =	eGL_FUNC_LESS							;
	StencilFuncs[			eSTENCIL_FUNC_EQUAL					] =	eGL_FUNC_LEQUAL							;
	StencilFuncs[			eSTENCIL_FUNC_LEQUAL				] =	eGL_FUNC_EQUAL							;
	StencilFuncs[			eSTENCIL_FUNC_GREATER				] =	eGL_FUNC_NOTEQUAL						;
	StencilFuncs[			eSTENCIL_FUNC_NOTEQUAL				] =	eGL_FUNC_GEQUAL							;
	StencilFuncs[			eSTENCIL_FUNC_GEQUAL				] =	eGL_FUNC_GREATER						;
	StencilFuncs[			eSTENCIL_FUNC_ALWAYS				] =	eGL_FUNC_ALWAYS							;

	StencilOps[				eSTENCIL_OP_KEEP					] =	eGL_STENCIL_OP_KEEP						;
	StencilOps[				eSTENCIL_OP_ZERO					] =	eGL_STENCIL_OP_ZERO						;
	StencilOps[				eSTENCIL_OP_REPLACE					] =	eGL_STENCIL_OP_REPLACE					;
	StencilOps[				eSTENCIL_OP_INCR					] =	eGL_STENCIL_OP_INCR						;
	StencilOps[				eSTENCIL_OP_INCR_WRAP				] =	eGL_STENCIL_OP_INCR_WRAP				;
	StencilOps[				eSTENCIL_OP_DECR					] =	eGL_STENCIL_OP_DECR						;
	StencilOps[				eSTENCIL_OP_DECR_WRAP				] =	eGL_STENCIL_OP_DECR_WRAP				;
	StencilOps[				eSTENCIL_OP_INVERT					] =	eGL_STENCIL_OP_INVERT					;

	BuffersTA[				eGL_TEXTURE_ATTACHMENT_NONE			] = eGL_BUFFER_NONE							;
	BuffersTA[				eGL_TEXTURE_ATTACHMENT_COLOR0		] = eGL_BUFFER_COLOR0						;
	BuffersTA[				eGL_TEXTURE_ATTACHMENT_COLOR1		] = eGL_BUFFER_COLOR1						;
	BuffersTA[				eGL_TEXTURE_ATTACHMENT_COLOR2		] = eGL_BUFFER_COLOR2						;
	BuffersTA[				eGL_TEXTURE_ATTACHMENT_COLOR3		] = eGL_BUFFER_COLOR3						;
	BuffersTA[				eGL_TEXTURE_ATTACHMENT_COLOR4		] = eGL_BUFFER_COLOR4						;
	BuffersTA[				eGL_TEXTURE_ATTACHMENT_COLOR5		] = eGL_BUFFER_COLOR5						;
	BuffersTA[				eGL_TEXTURE_ATTACHMENT_COLOR6		] = eGL_BUFFER_COLOR6						;
	BuffersTA[				eGL_TEXTURE_ATTACHMENT_COLOR7		] = eGL_BUFFER_COLOR7						;
	BuffersTA[				eGL_TEXTURE_ATTACHMENT_COLOR8		] = eGL_BUFFER_COLOR8						;
	BuffersTA[				eGL_TEXTURE_ATTACHMENT_COLOR9		] = eGL_BUFFER_COLOR9						;
	BuffersTA[				eGL_TEXTURE_ATTACHMENT_COLOR10		] = eGL_BUFFER_COLOR10						;
	BuffersTA[				eGL_TEXTURE_ATTACHMENT_COLOR11		] = eGL_BUFFER_COLOR11						;
	BuffersTA[				eGL_TEXTURE_ATTACHMENT_COLOR12		] = eGL_BUFFER_COLOR12						;
	BuffersTA[				eGL_TEXTURE_ATTACHMENT_COLOR13		] = eGL_BUFFER_COLOR13						;
	BuffersTA[				eGL_TEXTURE_ATTACHMENT_COLOR14		] = eGL_BUFFER_COLOR14						;
	BuffersTA[				eGL_TEXTURE_ATTACHMENT_COLOR15		] = eGL_BUFFER_COLOR15						;

	BuffersRBA[				eGL_RENDERBUFFER_ATTACHMENT_NONE	] = eGL_BUFFER_NONE							;
	BuffersRBA[				eGL_RENDERBUFFER_ATTACHMENT_COLOR0	] = eGL_BUFFER_COLOR0						;
	BuffersRBA[				eGL_RENDERBUFFER_ATTACHMENT_COLOR1	] = eGL_BUFFER_COLOR1						;
	BuffersRBA[				eGL_RENDERBUFFER_ATTACHMENT_COLOR2	] = eGL_BUFFER_COLOR2						;
	BuffersRBA[				eGL_RENDERBUFFER_ATTACHMENT_COLOR3	] = eGL_BUFFER_COLOR3						;
	BuffersRBA[				eGL_RENDERBUFFER_ATTACHMENT_COLOR4	] = eGL_BUFFER_COLOR4						;
	BuffersRBA[				eGL_RENDERBUFFER_ATTACHMENT_COLOR5	] = eGL_BUFFER_COLOR5						;
	BuffersRBA[				eGL_RENDERBUFFER_ATTACHMENT_COLOR6	] = eGL_BUFFER_COLOR6						;
	BuffersRBA[				eGL_RENDERBUFFER_ATTACHMENT_COLOR7	] = eGL_BUFFER_COLOR7						;
	BuffersRBA[				eGL_RENDERBUFFER_ATTACHMENT_COLOR8	] = eGL_BUFFER_COLOR8						;
	BuffersRBA[				eGL_RENDERBUFFER_ATTACHMENT_COLOR9	] = eGL_BUFFER_COLOR9						;
	BuffersRBA[				eGL_RENDERBUFFER_ATTACHMENT_COLOR10	] = eGL_BUFFER_COLOR10						;
	BuffersRBA[				eGL_RENDERBUFFER_ATTACHMENT_COLOR11	] = eGL_BUFFER_COLOR11						;
	BuffersRBA[				eGL_RENDERBUFFER_ATTACHMENT_COLOR12	] = eGL_BUFFER_COLOR12						;
	BuffersRBA[				eGL_RENDERBUFFER_ATTACHMENT_COLOR13	] = eGL_BUFFER_COLOR13						;
	BuffersRBA[				eGL_RENDERBUFFER_ATTACHMENT_COLOR14	] = eGL_BUFFER_COLOR14						;
	BuffersRBA[				eGL_RENDERBUFFER_ATTACHMENT_COLOR15	] = eGL_BUFFER_COLOR15						;

	Cleanup();
}

OpenGl::~OpenGl()
{
}

bool OpenGl::PreInitialise( String const & p_strExtensions )
{
	char const * l_extensions = ( char const * )glGetString( GL_EXTENSIONS );
	m_strExtensions	= str_utils::from_str( l_extensions ) + p_strExtensions;
	m_strVendor		= str_utils::from_str( ( char const * )glGetString( GL_VENDOR	) );
	m_strRenderer	= str_utils::from_str( ( char const * )glGetString( GL_RENDERER	) );
	m_strVersion	= str_utils::from_str( ( char const * )glGetString( GL_VERSION	) );
	String l_strExtensions = m_strExtensions;
	StringArray l_arrayExtensions = str_utils::split( l_strExtensions, cuT( " " ), 10000, false );
	Logger::LogDebug( cuT( "Available extensions :" ) );
	std::for_each( l_arrayExtensions.begin(), l_arrayExtensions.end(), [&]( String const & p_strExtension )
	{
		Logger::LogDebug( p_strExtension );
	} );
	double l_dVersion;
	StringStream l_stream( m_strVersion );
	l_stream >> l_dVersion;
	m_iVersion = int( l_dVersion * 10 );

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
	m_pfnMakeCurrent			= &wglMakeCurrent			;
	m_pfnCreateContext			= &wglCreateContext			;
	m_pfnDeleteContext			= &wglDeleteContext			;

	if ( HasExtension( ARB_create_context ) )
	{
		gl_api::GetFunction( cuT( "wglCreateContextAttribsARB" ), m_pfnCreateContextAttribs );
	}

#elif defined( __linux__ )
	m_pfnMakeCurrent	= &glXMakeCurrent				;
	m_pfnCreateContext	= &glXCreateContext				;
	m_pfnDeleteContext	= &glXDestroyContext			;

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
#define GL_GET_FUNC( ptr, func, ext )\
	if( !gl_api::GetFunction( cuT( "gl" ) + str_utils::from_str( #func ), ptr->m_pfn##func ) )										\
	{																																\
		if( !gl_api::GetFunction( cuT( "gl" ) + str_utils::from_str( #func ) + str_utils::from_str( #ext ), ptr->m_pfn##func ) )	\
		{																															\
			Logger::LogWarning( cuT( "Unable to retrieve function gl" ) + str_utils::from_str( #func ) );							\
		}																															\
	}
	m_pfnGetError				= &glGetError				;
	m_pfnClearColor				= &glClearColor				;
	m_pfnShadeModel				= &glShadeModel				;
	m_pfnViewport				= &glViewport				;
	m_pfnClear					= &glClear					;
	m_pfnDrawArrays				= &glDrawArrays				;
	m_pfnDrawElements			= &glDrawElements			;
	m_pfnEnable					= &glEnable					;
	m_pfnDisable				= &glDisable				;
	m_pfnGenTextures			= &glGenTextures			;
	m_pfnDeleteTextures			= &glDeleteTextures			;
	m_pfnVertexPointer			= &glVertexPointer			;
	m_pfnColorPointer			= &glColorPointer			;
	m_pfnNormalPointer			= &glNormalPointer			;
	m_pfnTexCoordPointer		= &glTexCoordPointer		;
	m_pfnTexEnvi				= &glTexEnvi				;
	m_pfnTexEnviv				= &glTexEnviv				;
	m_pfnTexEnvf				= &glTexEnvf				;
	m_pfnTexEnvfv				= &glTexEnvfv				;
	m_pfnBlendFunc				= &glBlendFunc				;
	m_pfnAlphaFunc				= &glAlphaFunc				;
	m_pfnLightModeli			= &glLightModeli			;
	m_pfnLightModeliv			= &glLightModeliv			;
	m_pfnLightModelf			= &glLightModelf			;
	m_pfnLightModelfv			= &glLightModelfv			;
	m_pfnIsTexture				= &glIsTexture				;
	m_pfnTexGeni				= &glTexGeni				;
	m_pfnCullFace				= &glCullFace				;
	m_pfnFrontFace				= &glFrontFace				;
	m_pfnMaterialf				= &glMaterialf				;
	m_pfnMaterialfv				= &glMaterialfv				;
	m_pfnLightf					= &glLightf					;
	m_pfnLightfv				= &glLightfv				;
	m_pfnSelectBuffer			= &glSelectBuffer			;
	m_pfnGetIntegerv			= &glGetIntegerv			;
	m_pfnRenderMode				= &glRenderMode				;
	m_pfnReadBuffer				= &glReadBuffer				;
	m_pfnReadPixels				= &glReadPixels				;
	m_pfnDrawBuffer				= &glDrawBuffer				;
	m_pfnDrawPixels				= &glDrawPixels				;
	m_pfnPixelStorei			= &glPixelStorei			;
	m_pfnPixelStoref			= &glPixelStoref			;
	m_pfnDepthFunc				= &glDepthFunc				;
	m_pfnDepthMask				= &glDepthMask				;
	m_pfnColorMask				= &glColorMask				;
	m_pfnPolygonMode			= &glPolygonMode			;
	m_pfnHint					= &glHint					;
	m_pfnPolygonOffset			= &glPolygonOffset			;
	m_pfnStencilOp				= &glStencilOp				;
	m_pfnStencilFunc			= &glStencilFunc			;
	m_pfnStencilMask			= &glStencilMask			;
	GL_GET_FUNC( this,	BlendEquation,			EXT	);
	GL_GET_FUNC( this,	BlendFuncSeparate,		EXT	);
	GL_GET_FUNC( this,	BlendColor,				EXT );
	GL_GET_FUNC( this,	StencilOpSeparate,		ATI	);
	GL_GET_FUNC( this,	StencilFuncSeparate,	ATI	);
	GL_GET_FUNC( this,	StencilMaskSeparate,	);
	GL_GET_FUNC( this,	ActiveTexture,			ARB	);
	GL_GET_FUNC( this,	ClientActiveTexture,	ARB	);
#if defined( _WIN32 )
	m_pfnMakeCurrent			= &wglMakeCurrent			;
	m_pfnSwapBuffers			= &::SwapBuffers			;
	m_pfnCreateContext			= &wglCreateContext			;
	m_pfnDeleteContext			= &wglDeleteContext			;

	if ( HasExtension( ARB_create_context ) )
	{
		gl_api::GetFunction( cuT( "wglCreateContextAttribsARB" ), m_pfnCreateContextAttribs );
	}

	if ( HasExtension( EXT_swap_control ) )
	{
		gl_api::GetFunction( cuT( "wglSwapIntervalEXT" ), m_pfnSwapInterval );
	}

#elif defined( __linux__ )
	m_pfnMakeCurrent	= &glXMakeCurrent				;
	m_pfnSwapBuffers	= &glXSwapBuffers				;
	m_pfnCreateContext	= &glXCreateContext				;
	m_pfnDeleteContext	= &glXDestroyContext			;

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
		MtxFunctionsDSA * l_pMtxFunctions = new MtxFunctionsDSA( *this );
#if CASTOR_USE_DOUBLE
		GL_GET_FUNC( l_pMtxFunctions,	MatrixTranslated,				EXT	);
		GL_GET_FUNC( l_pMtxFunctions,	MatrixRotated,					EXT	);
		GL_GET_FUNC( l_pMtxFunctions,	MatrixScaled,					EXT	);
		GL_GET_FUNC( l_pMtxFunctions,	MatrixMultd,					EXT	);
#else
		GL_GET_FUNC( l_pMtxFunctions,	MatrixTranslatef,				EXT	);
		GL_GET_FUNC( l_pMtxFunctions,	MatrixRotatef,					EXT	);
		GL_GET_FUNC( l_pMtxFunctions,	MatrixScalef,					EXT	);
		GL_GET_FUNC( l_pMtxFunctions,	MatrixMultf,					EXT	);
#endif
		GL_GET_FUNC( l_pMtxFunctions,	MatrixLoadIdentity,				EXT	);
		GL_GET_FUNC( l_pMtxFunctions,	MatrixOrtho,					EXT	);
		GL_GET_FUNC( l_pMtxFunctions,	MatrixPush,						EXT	);
		GL_GET_FUNC( l_pMtxFunctions,	MatrixPop,						EXT	);
		GL_GET_FUNC( l_pMtxFunctions,	MatrixFrustum,					EXT	);
		m_pMtxFunctions = l_pMtxFunctions;
		TexFunctionsDSA * l_pTexFunctions = new TexFunctionsDSA( *this );
		GL_GET_FUNC( l_pTexFunctions,	TextureSubImage1D,				EXT );
		GL_GET_FUNC( l_pTexFunctions,	TextureSubImage2D,				EXT );
		GL_GET_FUNC( l_pTexFunctions,	TextureImage1D,					EXT );
		GL_GET_FUNC( l_pTexFunctions,	TextureImage2D,					EXT );
		GL_GET_FUNC( l_pTexFunctions,	TextureParameteri,				EXT );
		GL_GET_FUNC( l_pTexFunctions,	TextureParameterf,				EXT );
		GL_GET_FUNC( l_pTexFunctions,	TextureParameteriv,				EXT );
		GL_GET_FUNC( l_pTexFunctions,	TextureParameterfv,				EXT );
		GL_GET_FUNC( l_pTexFunctions,	GetTextureImage,				EXT );
		GL_GET_FUNC( l_pTexFunctions,	TextureImage3D,					EXT	);
		GL_GET_FUNC( l_pTexFunctions,	TextureSubImage3D,				EXT	);
		GL_GET_FUNC( l_pTexFunctions,	GenerateTextureMipmap,			EXT	);
		m_pTexFunctions = l_pTexFunctions;
		BufFunctionsDSA * l_pBufFunctions = new BufFunctionsDSA( *this );
		GL_GET_FUNC( l_pBufFunctions,	NamedBufferData,				EXT );
		GL_GET_FUNC( l_pBufFunctions,	NamedBufferSubData,				EXT );
		GL_GET_FUNC( l_pBufFunctions,	GetNamedBufferParameteriv,		EXT );
		GL_GET_FUNC( l_pBufFunctions,	MapNamedBuffer,					EXT );
		GL_GET_FUNC( l_pBufFunctions,	UnmapNamedBuffer,				EXT );
		GL_GET_FUNC( l_pBufFunctions,	MapNamedBufferRange,			EXT );
		GL_GET_FUNC( l_pBufFunctions,	FlushMappedNamedBufferRange,	EXT );
		m_pBufFunctions = l_pBufFunctions;
	}
	else
#endif
	{
		MtxFunctions * l_pMtxFunctions = new MtxFunctions( *this );
#if CASTOR_USE_DOUBLE
		l_pMtxFunctions->m_pfnTranslate				= &glTranslated;
		l_pMtxFunctions->m_pfnRotate				= &glRotated;
		l_pMtxFunctions->m_pfnScale					= &glScaled;
		l_pMtxFunctions->m_pfnMultMatrix			= &glMultMatrixd;
#else
		l_pMtxFunctions->m_pfnTranslate				= &glTranslatef;
		l_pMtxFunctions->m_pfnRotate				= &glRotatef;
		l_pMtxFunctions->m_pfnScale					= &glScalef;
		l_pMtxFunctions->m_pfnMultMatrix			= &glMultMatrixf;
#endif
		l_pMtxFunctions->m_pfnMatrixMode			= &glMatrixMode;
		l_pMtxFunctions->m_pfnLoadIdentity			= &glLoadIdentity;
		l_pMtxFunctions->m_pfnOrtho					= &glOrtho;
		l_pMtxFunctions->m_pfnPushMatrix			= &glPushMatrix;
		l_pMtxFunctions->m_pfnPopMatrix				= &glPopMatrix;
		l_pMtxFunctions->m_pfnFrustum				= &glFrustum;
		m_pMtxFunctions = l_pMtxFunctions;
		TexFunctions * l_pTexFunctions = new TexFunctions( *this );
		l_pTexFunctions->m_pfnBindTexture			= &glBindTexture;
		l_pTexFunctions->m_pfnTexSubImage1D			= &glTexSubImage1D;
		l_pTexFunctions->m_pfnTexSubImage2D			= &glTexSubImage2D;
		l_pTexFunctions->m_pfnTexImage1D			= &glTexImage1D;
		l_pTexFunctions->m_pfnTexImage2D			= &glTexImage2D;
		l_pTexFunctions->m_pfnTexParameteri			= &glTexParameteri;
		l_pTexFunctions->m_pfnTexParameterf			= &glTexParameterf;
		l_pTexFunctions->m_pfnTexParameteriv		= &glTexParameteriv;
		l_pTexFunctions->m_pfnTexParameterfv		= &glTexParameterfv;
		l_pTexFunctions->m_pfnGetTexImage			= &glGetTexImage;
		GL_GET_FUNC( l_pTexFunctions,	TexImage3D,							EXT	);
		GL_GET_FUNC( l_pTexFunctions,	TexSubImage3D,						EXT	);
		GL_GET_FUNC( l_pTexFunctions,	GenerateMipmap,						EXT	);
		m_pTexFunctions = l_pTexFunctions;
		BufFunctions * l_pBufFunctions = new BufFunctions( *this );
		GL_GET_FUNC( l_pBufFunctions,	BindBuffer,							ARB	);
		GL_GET_FUNC( l_pBufFunctions,	BufferData,							ARB	);
		GL_GET_FUNC( l_pBufFunctions,	BufferSubData,						ARB	);
		GL_GET_FUNC( l_pBufFunctions,	GetBufferParameteriv,				ARB	);
		GL_GET_FUNC( l_pBufFunctions,	MapBuffer,							ARB	);
		GL_GET_FUNC( l_pBufFunctions,	UnmapBuffer,						ARB	);
		GL_GET_FUNC( l_pBufFunctions,	MapBufferRange,	);
		GL_GET_FUNC( l_pBufFunctions,	FlushMappedBufferRange,	);
		m_pBufFunctions = l_pBufFunctions;
	}

	if ( HasExtension( NV_shader_buffer_load ) )
	{
		GL_GET_FUNC( m_pBufFunctions,	MakeBufferResident,					NV	);
		GL_GET_FUNC( m_pBufFunctions,	MakeBufferNonResident,				NV	);
		GL_GET_FUNC( m_pBufFunctions,	IsBufferResident,					NV	);
		GL_GET_FUNC( m_pBufFunctions,	MakeNamedBufferResident,			NV	);
		GL_GET_FUNC( m_pBufFunctions,	MakeNamedBufferNonResident,			NV	);
		GL_GET_FUNC( m_pBufFunctions,	IsNamedBufferResident,				NV	);
		GL_GET_FUNC( m_pBufFunctions,	GetBufferParameterui64v,			NV	);
		GL_GET_FUNC( m_pBufFunctions,	GetNamedBufferParameterui64v,		NV	);
		GL_GET_FUNC( this,				GetIntegerui64v,					NV	);
	}

	if ( HasExtension( NV_vertex_buffer_unified_memory ) )
	{
		GL_GET_FUNC( m_pBufFunctions,	BufferAddressRange,				NV	);
		GL_GET_FUNC( m_pBufFunctions,	VertexFormat,					NV	);
		GL_GET_FUNC( m_pBufFunctions,	NormalFormat,					NV	);
		GL_GET_FUNC( m_pBufFunctions,	ColorFormat,					NV	);
		GL_GET_FUNC( m_pBufFunctions,	IndexFormat,					NV	);
		GL_GET_FUNC( m_pBufFunctions,	TexCoordFormat,					NV	);
		GL_GET_FUNC( m_pBufFunctions,	EdgeFlagFormat,					NV	);
		GL_GET_FUNC( m_pBufFunctions,	SecondaryColorFormat,			NV	);
		GL_GET_FUNC( m_pBufFunctions,	FogCoordFormat,					NV	);
		GL_GET_FUNC( m_pBufFunctions,	VertexAttribFormat,				NV	);
		GL_GET_FUNC( m_pBufFunctions,	VertexAttribIFormat,			NV	);
	}

	m_bBindVboToGpuAddress =  HasExtension( NV_shader_buffer_load ) && HasExtension( NV_vertex_buffer_unified_memory );
	m_pBufFunctions->m_pfnEnableClientState		= &glEnableClientState;
	m_pBufFunctions->m_pfnDisableClientState	= &glDisableClientState;

	if ( m_iVersion >= 40 || HasExtension( ARB_draw_buffers_blend ) )
	{
		GL_GET_FUNC( this,	BlendEquationi,		ARB	);
		GL_GET_FUNC( this,	BlendFuncSeparatei,	ARB	);
	}
	else if ( HasExtension( AMD_draw_buffers_blend ) )
	{
		gl_api::GetFunction( cuT( "glBlendEquationIndexedAMD"	), m_pfnBlendEquationi	);
		gl_api::GetFunction( cuT( "glBlendFuncIndexedAMD"	), m_pfnBlendFuncSeparatei	);
	}

	m_bHasDepthClipping = HasExtension( EXT_clip_volume_hint );
	m_bHasNonPowerOfTwoTextures = HasExtension( ARB_texture_non_power_of_two );

	if ( HasExtension( ARB_texture_multisample ) )
	{
		GL_GET_FUNC( this,	TexImage2DMultisample,	);
		GL_GET_FUNC( this,	SampleCoverage,					ARB	);
	}

	if ( HasExtension( ARB_imaging ) )
	{
	}

	if ( HasExtension( ARB_debug_output ) )
	{
		GL_GET_FUNC( this,	DebugMessageCallback,			ARB	);
	}
	else if ( HasExtension( AMDX_debug_output ) )
	{
		GL_GET_FUNC( this,	DebugMessageCallbackAMD,	);
	}

	if ( HasExtension( ARB_vertex_buffer_object ) )
	{
		m_bHasVbo = true;
		GL_GET_FUNC( this,	GenBuffers,						ARB	);
		GL_GET_FUNC( this,	DeleteBuffers,					ARB	);
		GL_GET_FUNC( this,	IsBuffer,						ARB	);

		if ( HasExtension( EXT_coordinate_frame ) )
		{
			GL_GET_FUNC( this,	TangentPointer,				EXT	);
			GL_GET_FUNC( this,	BinormalPointer,			EXT	);
		}

		if ( HasExtension( ARB_pixel_buffer_object ) )
		{
			m_bHasPbo = true;
		}
	}

	if ( HasExtension( ARB_vertex_array_object ) )
	{
		m_bHasVao = true;
		GL_GET_FUNC( this,	GenVertexArrays,	);
		GL_GET_FUNC( this,	BindVertexArray,	);
		GL_GET_FUNC( this,	DeleteVertexArrays,	);
	}

	if ( HasExtension( ARB_texture_buffer_object ) )
	{
		m_bHasTbo = true;
		GL_GET_FUNC( this,	TexBuffer,						EXT	);
	}

	if ( HasExtension( ARB_framebuffer_object ) )
	{
		m_bHasFbo = true;
		GL_GET_FUNC( this,	DrawBuffers,					ARB );
		GL_GET_FUNC( this,	BlitFramebuffer,				ARB );
		GL_GET_FUNC( this,	GenRenderbuffers,				ARB	);
		GL_GET_FUNC( this,	DeleteRenderbuffers,			ARB	);
		GL_GET_FUNC( this,	BindRenderbuffer,				ARB	);
		GL_GET_FUNC( this,	RenderbufferStorage,			ARB	);
		GL_GET_FUNC( this,	RenderbufferStorageMultisample,	ARB	);
		GL_GET_FUNC( this,	GetRenderbufferParameteriv,		ARB	);
		GL_GET_FUNC( this,	FramebufferRenderbuffer,		ARB	);
		GL_GET_FUNC( this,	GenFramebuffers,				ARB	);
		GL_GET_FUNC( this,	DeleteFramebuffers,				ARB	);
		GL_GET_FUNC( this,	BindFramebuffer,				ARB	);
		GL_GET_FUNC( this,	FramebufferTexture,				ARB	);
		GL_GET_FUNC( this,	FramebufferTexture1D,			ARB	);
		GL_GET_FUNC( this,	FramebufferTexture2D,			ARB	);
		GL_GET_FUNC( this,	FramebufferTexture3D,			ARB	);
		GL_GET_FUNC( this,	FramebufferTextureLayer,		ARB	);
		GL_GET_FUNC( this,	CheckFramebufferStatus,			ARB	);
	}
	else if ( HasExtension( EXT_framebuffer_object ) )
	{
		m_bHasFbo = true;
		GL_GET_FUNC( this,	DrawBuffers,					EXT );
		GL_GET_FUNC( this,	BlitFramebuffer,				EXT );
		GL_GET_FUNC( this,	GenRenderbuffers,				EXT	);
		GL_GET_FUNC( this,	DeleteRenderbuffers,			EXT	);
		GL_GET_FUNC( this,	BindRenderbuffer,				EXT	);
		GL_GET_FUNC( this,	RenderbufferStorage,			EXT	);
		GL_GET_FUNC( this,	RenderbufferStorageMultisample,	EXT	);
		GL_GET_FUNC( this,	GetRenderbufferParameteriv,		EXT	);
		GL_GET_FUNC( this,	FramebufferRenderbuffer,		EXT	);
		GL_GET_FUNC( this,	GenFramebuffers,				EXT	);
		GL_GET_FUNC( this,	DeleteFramebuffers,				EXT	);
		GL_GET_FUNC( this,	BindFramebuffer,				EXT	);
		GL_GET_FUNC( this,	FramebufferTexture,				EXT	);
		GL_GET_FUNC( this,	FramebufferTexture1D,			EXT	);
		GL_GET_FUNC( this,	FramebufferTexture2D,			EXT	);
		GL_GET_FUNC( this,	FramebufferTexture3D,			EXT	);
		GL_GET_FUNC( this,	FramebufferTextureLayer,		EXT	);
		GL_GET_FUNC( this,	CheckFramebufferStatus,			EXT	);
	}

	if ( HasExtension( ARB_sampler_objects ) )
	{
		m_bHasSpl = true;
		GL_GET_FUNC( this,	BindSampler,	);
		GL_GET_FUNC( this,	DeleteSamplers,	);
		GL_GET_FUNC( this,	GenSamplers,	);
		GL_GET_FUNC( this,	GetSamplerParameteruiv,	);
		GL_GET_FUNC( this,	GetSamplerParameterfv,	);
		GL_GET_FUNC( this,	GetSamplerParameteriv,	);
		GL_GET_FUNC( this,	IsSampler,	);
		GL_GET_FUNC( this,	SamplerParameteruiv,	);
		GL_GET_FUNC( this,	SamplerParameterf,	);
		GL_GET_FUNC( this,	SamplerParameterfv,	);
		GL_GET_FUNC( this,	SamplerParameteri,	);
		GL_GET_FUNC( this,	SamplerParameteriv,	);

		if ( HasExtension( EXT_texture_filter_anisotropic ) )
		{
			m_bHasAnisotropic = true;
		}
	}

	if ( HasExtension( ARB_draw_instanced ) )
	{
		m_bHasInstancedDraw = true;
		GL_GET_FUNC( this,	DrawArraysInstanced,			ARB	);
		GL_GET_FUNC( this,	DrawElementsInstanced,			ARB	);

		if ( HasExtension( ARB_instanced_arrays ) )
		{
			m_bHasInstancedArrays = true;
			GL_GET_FUNC( this,	VertexAttribDivisor,		ARB	);
		}
	}
	else if ( HasExtension( EXT_draw_instanced ) )
	{
		m_bHasInstancedDraw = true;
		GL_GET_FUNC( this,	DrawArraysInstanced,			EXT	);
		GL_GET_FUNC( this,	DrawElementsInstanced,			EXT	);

		if ( HasExtension( EXT_instanced_arrays ) )
		{
			m_bHasInstancedArrays = true;
			GL_GET_FUNC( this,	VertexAttribDivisor,		EXT	);
		}
	}

	if ( HasExtension( ARB_vertex_program ) )
	{
		m_bHasVSh = true;
		GL_GET_FUNC( this,	CreateShader,	);
		GL_GET_FUNC( this,	DeleteShader,	);
		GL_GET_FUNC( this,	AttachShader,	);
		GL_GET_FUNC( this,	DetachShader,	);
		GL_GET_FUNC( this,	CompileShader,					ARB	);
		GL_GET_FUNC( this,	GetShaderiv,	);
		GL_GET_FUNC( this,	GetShaderInfoLog,	);
		GL_GET_FUNC( this,	ShaderSource,					ARB	);
		GL_GET_FUNC( this,	CreateProgram,	);
		GL_GET_FUNC( this,	DeleteProgram,	);
		GL_GET_FUNC( this,	LinkProgram,					ARB	);
		GL_GET_FUNC( this,	UseProgram,	);
		GL_GET_FUNC( this,	GetProgramiv,					ARB	);
		GL_GET_FUNC( this,	GetProgramInfoLog,	);
		GL_GET_FUNC( this,	GetAttribLocation,				ARB	);
		GL_GET_FUNC( this,	IsProgram,						ARB	);
		GL_GET_FUNC( this,	EnableVertexAttribArray,		ARB	);
		GL_GET_FUNC( this,	VertexAttribPointer,			ARB	);
		GL_GET_FUNC( this,	VertexAttribIPointer,			ARB	);
		GL_GET_FUNC( this,	DisableVertexAttribArray,		ARB	);
		GL_GET_FUNC( this,	ProgramParameteri,				ARB	);
		GL_GET_FUNC( this,	GetUniformLocation,				ARB	);

		if ( HasExtension( ARB_fragment_program ) )
		{
			m_bHasPSh = true;
			GL_GET_FUNC( this,	Uniform1i,					ARB	);
			GL_GET_FUNC( this,	Uniform2i,					ARB	);
			GL_GET_FUNC( this,	Uniform3i,					ARB	);
			GL_GET_FUNC( this,	Uniform4i,					ARB	);
			GL_GET_FUNC( this,	Uniform1iv,					ARB	);
			GL_GET_FUNC( this,	Uniform2iv,					ARB	);
			GL_GET_FUNC( this,	Uniform3iv,					ARB	);
			GL_GET_FUNC( this,	Uniform4iv,					ARB	);
			GL_GET_FUNC( this,	Uniform1ui,					EXT	);
			GL_GET_FUNC( this,	Uniform2ui,					EXT	);
			GL_GET_FUNC( this,	Uniform3ui,					EXT	);
			GL_GET_FUNC( this,	Uniform4ui,					EXT	);
			GL_GET_FUNC( this,	Uniform1uiv,				EXT	);
			GL_GET_FUNC( this,	Uniform2uiv,				EXT	);
			GL_GET_FUNC( this,	Uniform3uiv,				EXT	);
			GL_GET_FUNC( this,	Uniform4uiv,				EXT	);
			GL_GET_FUNC( this,	Uniform1f,					ARB	);
			GL_GET_FUNC( this,	Uniform2f,					ARB	);
			GL_GET_FUNC( this,	Uniform3f,					ARB	);
			GL_GET_FUNC( this,	Uniform4f,					ARB	);
			GL_GET_FUNC( this,	Uniform1fv,					ARB	);
			GL_GET_FUNC( this,	Uniform2fv,					ARB	);
			GL_GET_FUNC( this,	Uniform3fv,					ARB	);
			GL_GET_FUNC( this,	Uniform4fv,					ARB	);
			GL_GET_FUNC( this,	Uniform1d,	);
			GL_GET_FUNC( this,	Uniform2d,	);
			GL_GET_FUNC( this,	Uniform3d,	);
			GL_GET_FUNC( this,	Uniform4d,	);
			GL_GET_FUNC( this,	Uniform1dv,	);
			GL_GET_FUNC( this,	Uniform2dv,	);
			GL_GET_FUNC( this,	Uniform3dv,	);
			GL_GET_FUNC( this,	Uniform4dv,	);
			GL_GET_FUNC( this,	UniformMatrix2fv,			ARB	);
			GL_GET_FUNC( this,	UniformMatrix2x3fv,	);
			GL_GET_FUNC( this,	UniformMatrix2x4fv,	);
			GL_GET_FUNC( this,	UniformMatrix3fv,			ARB	);
			GL_GET_FUNC( this,	UniformMatrix3x2fv,	);
			GL_GET_FUNC( this,	UniformMatrix3x4fv,	);
			GL_GET_FUNC( this,	UniformMatrix4fv,			ARB	);
			GL_GET_FUNC( this,	UniformMatrix4x2fv,	);
			GL_GET_FUNC( this,	UniformMatrix4x3fv,	);
			GL_GET_FUNC( this,	UniformMatrix2dv,	);
			GL_GET_FUNC( this,	UniformMatrix2x3dv,	);
			GL_GET_FUNC( this,	UniformMatrix2x4dv,	);
			GL_GET_FUNC( this,	UniformMatrix3dv,	);
			GL_GET_FUNC( this,	UniformMatrix3x2dv,	);
			GL_GET_FUNC( this,	UniformMatrix3x4dv,	);
			GL_GET_FUNC( this,	UniformMatrix4dv,	);
			GL_GET_FUNC( this,	UniformMatrix4x2dv,	);
			GL_GET_FUNC( this,	UniformMatrix4x3dv,	);

			if ( HasExtension( ARB_uniform_buffer_object ) )
			{
				m_bHasUbo = m_iGlslVersion >= 140;
				GL_GET_FUNC( this,	GetUniformBlockIndex,	);
				GL_GET_FUNC( this,	BindBufferBase,			EXT	);
				GL_GET_FUNC( this,	UniformBlockBinding,	);
				GL_GET_FUNC( this,	GetUniformIndices,	);
				GL_GET_FUNC( this,	GetActiveUniformsiv,	);
				GL_GET_FUNC( this,	GetActiveUniformBlockiv,	);
			}

			if ( HasExtension( ARB_geometry_shader4 ) || HasExtension( EXT_geometry_shader4 ) )
			{
				m_bHasGSh = true;

				if ( HasExtension( ARB_tessellation_shader ) )
				{
					m_bHasTSh = true;
					GL_GET_FUNC( this,	PatchParameteri,	);

					if ( HasExtension( ARB_compute_shader ) )
					{
						m_bHasCSh = true;
					}
				}
			}
		}
	}

#undef GL_GET_FUNC
	return true;
}

void OpenGl::Cleanup()
{
	delete m_pMtxFunctions;
	m_pMtxFunctions = NULL;
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

bool OpenGl::ClearColor( float red, float green, float blue, float alpha )
{
	m_pfnClearColor( red, green, blue, alpha );
	return glCheckError( *this, "glClearColor" );
}

bool OpenGl::ClearColor( Castor::Colour const & p_colour )
{
	m_pfnClearColor( p_colour.red().value(), p_colour.green().value(), p_colour.blue().value(), p_colour.alpha().value() );
	return glCheckError( *this, "glClearColor" );
}

bool OpenGl::ShadeModel( eGL_SHADE_MODEL mode )
{
	m_pfnShadeModel( mode );
	return glCheckError( *this, "glShadeModel" );
}

bool OpenGl::Clear( uint32_t mask )
{
	m_pfnClear( mask );
	return glCheckError( *this, "glClear" );
}

bool OpenGl::DrawArrays( eGL_PRIMITIVE mode, int first, int count )
{
	m_pfnDrawArrays( mode, first, count );
	return glCheckError( *this, "glDrawArrays" );
}

bool OpenGl::DrawElements( eGL_PRIMITIVE mode, int count, eGL_TYPE type, void const * indices )
{
	m_pfnDrawElements( mode, count, type, indices );
	return glCheckError( *this, "glDrawElements" );
}

bool OpenGl::Enable( eGL_TWEAK mode )
{
	m_pfnEnable( mode );
	return glCheckError( *this, "glEnable" );
}

bool OpenGl::Disable( eGL_TWEAK mode )
{
	m_pfnDisable( mode );
	return glCheckError( *this, "glDisable" );
}

bool OpenGl::Enable( eGL_LIGHT_INDEX light )
{
	m_pfnEnable( light );
	return glCheckError( *this, "glEnable" );
}

bool OpenGl::Disable( eGL_LIGHT_INDEX light )
{
	m_pfnDisable( light );
	return glCheckError( *this, "glDisable" );
}

bool OpenGl::Enable( eGL_TEXDIM texture )
{
	m_pfnEnable( texture );
	return glCheckError( *this, "glEnable" );
}

bool OpenGl::Disable( eGL_TEXDIM texture )
{
	m_pfnDisable( texture );
	return glCheckError( *this, "glDisable" );
}

bool OpenGl::SelectBuffer( int size, uint32_t * buffer )
{
	m_pfnSelectBuffer( size, buffer );
	return glCheckError( *this, "glSelectBuffer" );
}

bool OpenGl::GetIntegerv( uint32_t pname, int * params )
{
	m_pfnGetIntegerv( pname, params );
	return glCheckError( *this, "glGetIntegerv" );
}

bool OpenGl::GetIntegerv( uint32_t pname, uint64_t * params )
{
	m_pfnGetIntegerui64v( pname, params );
	return glCheckError( *this, "glGetIntegerui64v" );
}

int OpenGl::RenderMode( eGL_RENDER_MODE mode )
{
	int l_iReturn = m_pfnRenderMode( mode );

	if ( ! glCheckError( *this, "glRenderMode" ) )
	{
		l_iReturn = eGL_INVALID_INDEX;
	}

	return l_iReturn;
}

bool OpenGl::Light( eGL_LIGHT_INDEX light, uint32_t pname, float param )
{
	m_pfnLightf( light, pname, param );
	return glCheckError( *this, "glLightf" );
}

bool OpenGl::Light( eGL_LIGHT_INDEX light, uint32_t pname, float const * param )
{
	m_pfnLightfv( light, pname, param );
	return glCheckError( *this, "glLightfv" );
}

bool OpenGl::LightModel( uint32_t pname, int param )
{
	m_pfnLightModeli( pname, param );
	return glCheckError( *this, "glLightModeli" );
}

bool OpenGl::LightModel( uint32_t pname, int const * param )
{
	m_pfnLightModeliv( pname, param );
	return glCheckError( *this, "glLightModeliv" );
}

bool OpenGl::LightModel( uint32_t pname, float param )
{
	m_pfnLightModelf( pname, param );
	return glCheckError( *this, "glLightModelf" );
}

bool OpenGl::LightModel( uint32_t pname, float const * param )
{
	m_pfnLightModelfv( pname, param );
	return glCheckError( *this, "glLightModelfv" );
}
#if defined( _WIN32 )
bool OpenGl::MakeCurrent( HDC hdc, HGLRC hglrc )
{
	wglMakeCurrent( hdc, hglrc );
	return true;
}

bool OpenGl::SwapBuffers( HDC hdc )
{
	::SwapBuffers( hdc );
	return true;
}

bool OpenGl::SwapInterval( int interval )
{
	bool l_bReturn = false;

	if ( m_pfnSwapInterval )
	{
		m_pfnSwapInterval( interval );
		l_bReturn = glCheckError( *this, "glSwapInterval" );
	}

	return l_bReturn;
}

HGLRC OpenGl::CreateContext( HDC hdc )
{
	return wglCreateContext( hdc );
}

bool OpenGl::HasCreateContextAttribs()
{
	return m_pfnCreateContextAttribs != NULL;
}

HGLRC OpenGl::CreateContextAttribs( HDC hDC, HGLRC hShareContext, int const * attribList )
{
	return m_pfnCreateContextAttribs( hDC, hShareContext, attribList );
}

bool OpenGl::DeleteContext( HGLRC hContext )
{
	return m_pfnDeleteContext( hContext ) == TRUE;
}
#elif defined( __linux__ )
bool OpenGl::MakeCurrent( Display * pDisplay, GLXDrawable drawable, GLXContext context )
{
	m_pfnMakeCurrent( pDisplay, drawable, context );
	return true;
}

bool OpenGl::SwapBuffers( Display * pDisplay, GLXDrawable drawable )
{
	m_pfnSwapBuffers( pDisplay, drawable );
	return true;
}

bool OpenGl::SwapInterval( Display * pDisplay, GLXDrawable drawable, int interval )
{
	bool l_bReturn = false;

	if ( m_pfnSwapInterval )
	{
		m_pfnSwapInterval( pDisplay, drawable, interval );
		l_bReturn = glCheckError( *this, "glSwapInterval" );
	}

	return l_bReturn;
}

GLXContext OpenGl::CreateContext( Display * pDisplay, XVisualInfo * pVisualInfo, GLXContext shareList, Bool direct )
{
	return m_pfnCreateContext( pDisplay, pVisualInfo, shareList, direct );
}

bool OpenGl::HasCreateContextAttribs()
{
	return m_pfnCreateContextAttribs != NULL;
}

GLXContext OpenGl::CreateContextAttribs( Display * pDisplay, GLXFBConfig fbconfig, GLXContext shareList, Bool direct, int const * attribList )
{
	return m_pfnCreateContextAttribs( pDisplay, fbconfig, shareList, direct, attribList );
}

bool OpenGl::DeleteContext( Display * pDisplay, GLXContext context )
{
	m_pfnDeleteContext( pDisplay, context );
	return true;
}
#else
#	error "Yet unsupported OS"
#endif
bool OpenGl::Viewport( int x, int y, int width, int height )
{
	m_pfnViewport( x, y, width, height );
	return glCheckError( *this, "glViewport" );
}

bool OpenGl::MatrixMode( eGL_MATRIX_TYPE mode )
{
	return m_pMtxFunctions->MatrixMode( mode );
}

bool OpenGl::LoadIdentity()
{
	return m_pMtxFunctions->LoadIdentity();
}

bool OpenGl::Ortho( double left, double right, double bottom, double top, double zNear, double zFar )
{
	return m_pMtxFunctions->Ortho( left, right, bottom, top, zNear, zFar );
}

bool OpenGl::Frustum( double left, double right, double bottom, double top, double zNear, double zFar )
{
	return m_pMtxFunctions->Frustum( left, right, bottom, top, zNear, zFar );
}

bool OpenGl::PushMatrix()
{
	return m_pMtxFunctions->PushMatrix();
}

bool OpenGl::PopMatrix()
{
	return m_pMtxFunctions->PopMatrix();
}

bool OpenGl::Translate( real x, real y, real z )
{
	return m_pMtxFunctions->Translate( x, y, z );
}

bool OpenGl::Rotate( real angle, real x, real y, real z )
{
	return m_pMtxFunctions->Rotate( angle, x, y, z );
}

bool OpenGl::Scale( real x, real y, real z )
{
	return m_pMtxFunctions->Scale( x, y, z );
}

bool OpenGl::MultMatrix( real const * matrix )
{
	return m_pMtxFunctions->MultMatrix( matrix );
}

bool OpenGl::CullFace( eGL_FACE face )
{
	m_pfnCullFace( face );
	return glCheckError( *this, "glCullFace" );
}

bool OpenGl::FrontFace( eGL_FRONT_FACE_DIRECTION face )
{
	m_pfnFrontFace( face );
	return glCheckError( *this, "glFrontFace" );
}

bool OpenGl::Material( eGL_DRAW_BUFFER_MODE face, eGL_MATERIAL_COMPONENT pname, float param )
{
	m_pfnMaterialf( face, pname, param );
	return glCheckError( *this, "glMaterialf" );
}

bool OpenGl::Material( eGL_DRAW_BUFFER_MODE face, eGL_MATERIAL_COMPONENT pname, float const * param )
{
	m_pfnMaterialfv( face, pname, param );
	return glCheckError( *this, "glMaterialfv" );
}

bool OpenGl::GenTextures( int n, uint32_t * textures )
{
	m_pfnGenTextures( n, textures );
	return glCheckError( *this, "glGenTextures" );
}

bool OpenGl::DeleteTextures( int n, uint32_t const * textures )
{
	m_pfnDeleteTextures( n, textures );
	return glCheckError( *this, "glDeleteTextures" );
}

bool OpenGl::GenerateMipmap( eGL_TEXDIM mode )
{
	return m_pTexFunctions->GenerateMipmap( mode );
}

bool OpenGl::ActiveTexture( eGL_TEXTURE_INDEX target )
{
	m_pfnActiveTexture( target );
	return glCheckError( *this, "glActiveTexture" );
}

bool OpenGl::ClientActiveTexture( eGL_TEXTURE_INDEX target )
{
	m_pfnClientActiveTexture( target );
	return glCheckError( *this, "glClientActiveTexture" );
}

bool OpenGl::BindTexture( eGL_TEXDIM mode, uint32_t texture )
{
	return m_pTexFunctions->BindTexture( mode, texture );
}

bool OpenGl::TexSubImage1D( eGL_TEXDIM mode, int level, int xoffset, int width, eGL_FORMAT format, eGL_TYPE type, void const * data )
{
	return m_pTexFunctions->TexSubImage1D( mode, level, xoffset, width, format, type, data );
}

bool OpenGl::TexSubImage2D( eGL_TEXDIM mode, int level, int xoffset, int yoffset, int width, int height, eGL_FORMAT format, eGL_TYPE type, void const * data )
{
	return m_pTexFunctions->TexSubImage2D( mode, level, xoffset, yoffset, width, height, format, type, data );
}

bool OpenGl::TexSubImage2D( eGL_TEXDIM mode, int level, Position const & p_position, Size const & p_size, eGL_FORMAT format, eGL_TYPE type, void const * data )
{
	return m_pTexFunctions->TexSubImage2D( mode, level, p_position.x(), p_position.y(), p_size.width(), p_size.height(), format, type, data );
}

bool OpenGl::TexSubImage2D( eGL_TEXDIM mode, int level, Castor::Rectangle const & p_rect, eGL_FORMAT format, eGL_TYPE type, void const * data )
{
	return m_pTexFunctions->TexSubImage2D( mode, level, p_rect.left(), p_rect.top(), p_rect.width(), p_rect.height(), format, type, data );
}

bool OpenGl::TexSubImage3D( eGL_TEXDIM mode, int level, int xoffset, int yoffset, int zoffset, int width, int height, int depth, eGL_FORMAT format, eGL_TYPE type, void const * data )
{
	return m_pTexFunctions->TexSubImage3D( mode, level, xoffset, yoffset, zoffset, width, height, depth, format, type, data );
}

bool OpenGl::TexImage1D( eGL_TEXDIM mode, int level, eGL_INTERNAL internalFormat, int width, int border, eGL_FORMAT format, uint32_t type, void const * data )
{
	return m_pTexFunctions->TexImage1D( mode, level, internalFormat, width, border, format, type, data );
}

bool OpenGl::TexImage2D( eGL_TEXDIM mode, int level, eGL_INTERNAL internalFormat, int width, int height, int border, eGL_FORMAT format, uint32_t type, void const * data )
{
	return m_pTexFunctions->TexImage2D( mode, level, internalFormat, width, height, border, format, type, data );
}

bool OpenGl::TexImage2D( eGL_TEXDIM mode, int level, eGL_INTERNAL internalFormat, Size const & p_size, int border, eGL_FORMAT format, uint32_t type, void const * data )
{
	return m_pTexFunctions->TexImage2D( mode, level, internalFormat, p_size.width(), p_size.height(), border, format, type, data );
}

bool OpenGl::TexImage3D( eGL_TEXDIM mode, int level, eGL_INTERNAL internalFormat, int width, int height, int depth, int border, eGL_FORMAT format, uint32_t type, void const * data )
{
	return m_pTexFunctions->TexImage3D( mode, level, internalFormat, width, height, depth, border, format, type, data );
}

bool OpenGl::TexParameter( eGL_TEXDIM mode, eGL_TEXTURE_PARAMETER pname, int param )
{
	return m_pTexFunctions->TexParameter( mode, pname, param );
}

bool OpenGl::TexParameter( eGL_TEXDIM mode, eGL_TEXTURE_PARAMETER pname, float param )
{
	return m_pTexFunctions->TexParameter( mode, pname, param );
}

bool OpenGl::TexParameter( eGL_TEXDIM mode, eGL_TEXTURE_PARAMETER pname, const int * params )
{
	return m_pTexFunctions->TexParameter( mode, pname, params );
}

bool OpenGl::TexParameter( eGL_TEXDIM mode, eGL_TEXTURE_PARAMETER pname, const float * params )
{
	return m_pTexFunctions->TexParameter( mode, pname, params );
}

bool OpenGl::GetTexImage( eGL_TEXDIM mode, int level, eGL_FORMAT format, eGL_TYPE type, void * img )
{
	return m_pTexFunctions->GetTexImage( mode, level, format, type, img );
}

bool OpenGl::TexEnvi( eGL_TEXENV_TARGET target, eGL_TEXENV_ARGNAME pname, int param )
{
	m_pfnTexEnvi( target, pname, param );
	return glCheckError( *this, "glTexEnvi" );
}

bool OpenGl::TexEnviv( eGL_TEXENV_TARGET target, eGL_TEXENV_ARGNAME pname, int const * param )
{
	m_pfnTexEnviv( target, pname, param );
	return glCheckError( *this, "glTexEnviv" );
}

bool OpenGl::TexEnvf( eGL_TEXENV_TARGET target, eGL_TEXENV_ARGNAME pname, float param )
{
	m_pfnTexEnvf( target, pname, param );
	return glCheckError( *this, "glTexEnvf" );
}

bool OpenGl::TexEnvfv( eGL_TEXENV_TARGET target, eGL_TEXENV_ARGNAME pname, float const * param )
{
	m_pfnTexEnvfv( target, pname, param );
	return glCheckError( *this, "glTexEnvfv" );
}

bool OpenGl::BlendFunc( eGL_BLEND_FACTOR sfactor, eGL_BLEND_FACTOR dfactor )
{
	m_pfnBlendFunc( sfactor, dfactor );
	return glCheckError( *this, "glBlendFunc" );
}

bool OpenGl::BlendFunc( eGL_BLEND_FACTOR p_eRgbSrc, eGL_BLEND_FACTOR p_eRgbDst, eGL_BLEND_FACTOR p_eAlphaSrc, eGL_BLEND_FACTOR p_eAlphaDst )
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

bool OpenGl::BlendFunc( uint32_t p_uiBuffer, eGL_BLEND_FACTOR p_eRgbSrc, eGL_BLEND_FACTOR p_eRgbDst, eGL_BLEND_FACTOR p_eAlphaSrc, eGL_BLEND_FACTOR p_eAlphaDst )
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

bool OpenGl::BlendEquation( eGL_BLEND_OP p_eOp )
{
	if ( m_pfnBlendEquation )
	{
		m_pfnBlendEquation( p_eOp );
	}

	return glCheckError( *this, "glBlendEquation" );
}

bool OpenGl::BlendEquation( uint32_t p_uiBuffer, eGL_BLEND_OP p_eOp )
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

bool OpenGl::AlphaFunc( eGL_FUNC func, float ref )
{
	m_pfnAlphaFunc( func, ref );
	return glCheckError( *this, "glAlphaFunc" );
}

bool OpenGl::DepthFunc( eGL_FUNC p_eFunc )
{
	m_pfnDepthFunc( p_eFunc );
	return glCheckError( *this, "glDepthFunc" );
}

bool OpenGl::DepthMask( bool p_bMask )
{
	m_pfnDepthMask( p_bMask );
	return glCheckError( *this, "glDepthMask" );
}

bool OpenGl::ColorMask( bool p_r, bool p_g, bool p_b, bool p_a )
{
	m_pfnColorMask( p_r, p_g, p_b, p_a );
	return glCheckError( *this, "glColorMask" );
}

bool OpenGl::DebugMessageCallback( PFNGLDEBUGPROC pfnProc, void * p_pThis )
{
	if ( m_pfnDebugMessageCallback )
	{
		m_pfnDebugMessageCallback( pfnProc, p_pThis );
	}

	return true;
}

bool OpenGl::DebugMessageCallback( PFNGLDEBUGAMDPROC pfnProc, void * p_pThis )
{
	if ( m_pfnDebugMessageCallbackAMD )
	{
		m_pfnDebugMessageCallbackAMD( pfnProc, p_pThis );
	}

	return true;
}

bool OpenGl::PolygonMode( eGL_FACE p_eFacing, eGL_FILL_MODE p_eMode )
{
	m_pfnPolygonMode( p_eFacing, p_eMode );
	return glCheckError( *this, "glPolygonMode" );
}

bool OpenGl::StencilOp( eGL_STENCIL_OP p_eStencilFail, eGL_STENCIL_OP p_eDepthFail, eGL_STENCIL_OP p_eStencilPass )
{
	m_pfnStencilOp( p_eStencilFail, p_eDepthFail, p_eStencilPass );
	return glCheckError( *this, "glStencilOp" );
}

bool OpenGl::StencilFunc( eGL_FUNC p_eFunc, int p_iRef, uint32_t p_uiMask )
{
	m_pfnStencilFunc( p_eFunc, p_iRef, p_uiMask );
	return glCheckError( *this, "glStencilFunc" );
}

bool OpenGl::StencilMask( uint32_t p_uiMask )
{
	m_pfnStencilMask( p_uiMask );
	return glCheckError( *this, "glStencilMask" );
}

bool OpenGl::StencilOpSeparate( eGL_FACE p_eFacing, eGL_STENCIL_OP p_eStencilFail, eGL_STENCIL_OP p_eDepthFail, eGL_STENCIL_OP p_eStencilPass )
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

bool OpenGl::StencilFuncSeparate( eGL_FACE p_eFacing, eGL_FUNC p_eFunc, int p_iRef, uint32_t p_uiMask )
{
	if ( m_pfnStencilFuncSeparate )
	{
		m_pfnStencilFuncSeparate( p_eFacing, p_eFunc, p_iRef, p_uiMask );
		return glCheckError( *this, "glStencilFuncSeparate" );
	}
	else
	{
		return StencilFunc( p_eFunc, p_iRef, p_uiMask );
	}
}

bool OpenGl::StencilMaskSeparate( eGL_FACE p_eFacing, uint32_t p_uiMask )
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

bool OpenGl::Hint( eGL_HINT p_eHint, eGL_HINT_VALUE p_eValue )
{
	m_pfnHint( p_eHint, p_eValue );
	return glCheckError( *this, "glHint" );
}

bool OpenGl::PolygonOffset( float p_fFactor, float p_fUnits )
{
	m_pfnPolygonOffset( p_fFactor, p_fUnits );
	return glCheckError( *this, "glPolygonOffset" );
}

bool OpenGl::BlendColor( Colour const & p_colour )
{
	if ( m_pfnBlendColor )
	{
		m_pfnBlendColor( p_colour.red().value(), p_colour.green().value(), p_colour.blue().value(), p_colour.alpha().value() );
	}

	return glCheckError( *this, "glBlendColor" );
}

bool OpenGl::SampleCoverage( float fValue, bool invert )
{
	if ( m_pfnSampleCoverage )
	{
		m_pfnSampleCoverage( fValue, invert );
	}

	return glCheckError( *this, "glSampleCoverage" );
}

bool OpenGl::DrawArraysInstanced( eGL_PRIMITIVE mode, int first, int count, int primcount )
{
	if ( m_bHasInstancedDraw )
	{
		m_pfnDrawArraysInstanced( mode, first, count, primcount );
	}

	return glCheckError( *this, "glDrawArraysInstanced" );
}

bool OpenGl::DrawElementsInstanced( eGL_PRIMITIVE mode, int count, eGL_TYPE type, const void * indices, int primcount )
{
	if ( m_bHasInstancedDraw )
	{
		m_pfnDrawElementsInstanced( mode, count, type, indices, primcount );
	}

	return glCheckError( *this, "glDrawElementsInstanced" );
}

bool OpenGl::VertexAttribDivisor( uint32_t index, uint32_t divisor )
{
	if ( m_bHasInstancedArrays )
	{
		m_pfnVertexAttribDivisor( index, divisor );
	}

	return glCheckError( *this, "glVertexAttribDivisor" );
}

bool OpenGl::IsTexture( uint32_t texture )
{
	return m_pfnIsTexture( texture ) && glCheckError( *this, "glIsTexture" );
}

bool OpenGl::TexGeni( eGL_TEXGEN_COORD coord, eGL_TEXGEN_PARAM param, eGL_TEXGEN_MODE mode )
{
	m_pfnTexGeni( coord, param, mode );
	return glCheckError( *this, "glTexGeni" );
}

bool OpenGl::ReadBuffer( eGL_BUFFER p_eBuffer )
{
	m_pfnReadBuffer( p_eBuffer );
	return glCheckError( *this, "glReadBuffer" );
}

bool OpenGl::ReadPixels( int x, int y, int width, int height, eGL_FORMAT format, eGL_TYPE type, void * pixels )
{
	m_pfnReadPixels( x, y, width, height, format, type, pixels );
	return glCheckError( *this, "glReadPixels" );
}

bool OpenGl::ReadPixels( Position const & p_position, Size const & p_size, eGL_FORMAT format, eGL_TYPE type, void * pixels )
{
	m_pfnReadPixels( p_position.x(), p_position.y(), p_size.width(), p_size.height(), format, type, pixels );
	return glCheckError( *this, "glReadPixels" );
}

bool OpenGl::ReadPixels( Castor::Rectangle const & p_rect, eGL_FORMAT format, eGL_TYPE type, void * pixels )
{
	m_pfnReadPixels( p_rect.left(), p_rect.top(), p_rect.width(), p_rect.height(), format, type, pixels );
	return glCheckError( *this, "glReadPixels" );
}

bool OpenGl::DrawBuffer( eGL_BUFFER p_eBuffer )
{
	m_pfnDrawBuffer( p_eBuffer );
	return glCheckError( *this, "glDrawBuffer" );
}

bool OpenGl::DrawPixels( int width, int height, eGL_FORMAT format, eGL_TYPE type, void const * data )
{
	m_pfnDrawPixels( width, height, format, type, data );
	return glCheckError( *this, "glDrawPixels" );
}

bool OpenGl::PixelStore( eGL_STORAGE_MODE p_eMode, int p_iParam )
{
	m_pfnPixelStorei( p_eMode, p_iParam );
	return glCheckError( *this, "glPixelStorei" );
}

bool OpenGl::PixelStore( eGL_STORAGE_MODE p_eMode, float p_fParam )
{
	m_pfnPixelStoref( p_eMode, p_fParam );
	return glCheckError( *this, "glPixelStoref" );
}

bool OpenGl::BindSampler( uint32_t unit, uint32_t sampler )
{
	m_pfnBindSampler( unit, sampler );
	return glCheckError( *this, "glBindSampler" );
}

bool OpenGl::DeleteSamplers( int count, const uint32_t * samplers )
{
	m_pfnDeleteSamplers( count, samplers );
	return glCheckError( *this, "glDeleteSamplers" );
}

bool OpenGl::GenSamplers( int count, uint32_t * samplers )
{
	m_pfnGenSamplers( count, samplers );
	return glCheckError( *this, "glGenSamplers" );
}

bool OpenGl::GetSamplerParameter( uint32_t sampler, eGL_SAMPLER_PARAMETER pname, uint32_t * params )
{
	m_pfnGetSamplerParameteruiv( sampler, pname, params );
	return glCheckError( *this, "glGetSamplerParameterIuiv" );
}

bool OpenGl::GetSamplerParameter( uint32_t sampler, eGL_SAMPLER_PARAMETER pname, float * params )
{
	m_pfnGetSamplerParameterfv( sampler, pname, params );
	return glCheckError( *this, "glGetSamplerParameterfv" );
}

bool OpenGl::GetSamplerParameter( uint32_t sampler, eGL_SAMPLER_PARAMETER pname, int * params )
{
	m_pfnGetSamplerParameteriv( sampler, pname, params );
	return glCheckError( *this, "glGetSamplerParameteriv" );
}

bool OpenGl::IsSampler( uint32_t sampler )
{
	uint8_t l_uiRet = m_pfnIsSampler( sampler );
	return l_uiRet && glCheckError( *this, "glGetIsSampler" );
}

bool OpenGl::SetSamplerParameter( uint32_t sampler, eGL_SAMPLER_PARAMETER pname, const uint32_t * params )
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

bool OpenGl::SetSamplerParameter( uint32_t sampler, eGL_SAMPLER_PARAMETER pname, float param )
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

bool OpenGl::SetSamplerParameter( uint32_t sampler, eGL_SAMPLER_PARAMETER pname, const float * params )
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

bool OpenGl::SetSamplerParameter( uint32_t sampler, eGL_SAMPLER_PARAMETER pname, int param )
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

bool OpenGl::SetSamplerParameter( uint32_t sampler, eGL_SAMPLER_PARAMETER pname, const int * params )
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

bool OpenGl::TexBuffer( eGL_TEXDIM p_eTarget, eGL_INTERNAL_FORMAT p_eInternalFormat, uint32_t buffer )
{
	m_pfnTexBuffer( p_eTarget, p_eInternalFormat, buffer );
	return glCheckError( *this, "glTexBuffer" );
}

bool OpenGl::EnableClientState( eGL_BUFFER_USAGE p_eArray )
{
	return m_pBufFunctions->EnableClientState( p_eArray );
}

bool OpenGl::DisableClientState( eGL_BUFFER_USAGE p_eArray )
{
	return m_pBufFunctions->DisableClientState( p_eArray );
}

bool OpenGl::GenBuffers( int n, uint32_t * buffers )
{
	m_pfnGenBuffers( n, buffers );
	return glCheckError( *this, "glGenBuffers" );
}

bool OpenGl::DeleteBuffers( int n, uint32_t const * buffers )
{
	m_pfnDeleteBuffers( n, buffers );
	return glCheckError( *this, "glDeleteBuffers" );
}

bool OpenGl::IsBuffer( uint32_t buffer )
{
	return m_pfnIsBuffer( buffer ) && glCheckError( *this, "glIsBuffer" );
}

bool OpenGl::BindBuffer( eGL_BUFFER_TARGET p_eTarget, uint32_t buffer )
{
	return m_pBufFunctions->BindBuffer( p_eTarget, buffer );
}

bool OpenGl::BufferData( eGL_BUFFER_TARGET p_eTarget, ptrdiff_t size, void const * data, eGL_BUFFER_MODE usage )
{
	return m_pBufFunctions->BufferData( p_eTarget, size, data, usage );
}

bool OpenGl::BufferSubData( eGL_BUFFER_TARGET p_eTarget, ptrdiff_t offset, ptrdiff_t size, void const * data )
{
	return m_pBufFunctions->BufferSubData( p_eTarget, offset, size, data );
}

bool OpenGl::GetBufferParameter( eGL_BUFFER_TARGET p_eTarget, eGL_BUFFER_PARAMETER pname, int * params )
{
	return m_pBufFunctions->GetBufferParameter( p_eTarget, pname, params );
}

bool OpenGl::GetBufferParameter( eGL_BUFFER_TARGET p_eTarget, eGL_BUFFER_PARAMETER pname, uint64_t * params )
{
	return m_pBufFunctions->GetBufferParameter( p_eTarget, pname, params );
}

void * OpenGl::MapBuffer( eGL_BUFFER_TARGET p_eTarget, eGL_LOCK access )
{
	return m_pBufFunctions->MapBuffer( p_eTarget, access );
}

bool OpenGl::UnmapBuffer( eGL_BUFFER_TARGET p_eTarget )
{
	return m_pBufFunctions->UnmapBuffer( p_eTarget );
}

void * OpenGl::MapBufferRange( eGL_BUFFER_TARGET p_eTarget, ptrdiff_t offset, ptrdiff_t length, uint32_t access )
{
	return m_pBufFunctions->MapBufferRange( p_eTarget, offset, length, access );
}

bool OpenGl::FlushMappedBufferRange( eGL_BUFFER_TARGET p_eTarget, ptrdiff_t offset, ptrdiff_t length )
{
	return m_pBufFunctions->FlushMappedBufferRange( p_eTarget, offset, length );
}

bool OpenGl::VertexPointer( int size, uint32_t type, uint32_t stride, void const * pointer )
{
	m_pfnVertexPointer( size, type, stride, pointer );
	return glCheckError( *this, "glVertexPointer" );
}

bool OpenGl::NormalPointer( uint32_t type, uint32_t stride, void const * pointer )
{
	m_pfnNormalPointer( type, stride, pointer );
	return glCheckError( *this, "glNormalPointer" );
}

bool OpenGl::TangentPointer( uint32_t type, uint32_t stride, void const * pointer )
{
	if ( m_pfnTangentPointer )
	{
		m_pfnTangentPointer( type, stride, const_cast< void * >( pointer ) );
	}

	return glCheckError( *this, "glTangentPointer" );
}

bool OpenGl::BinormalPointer( uint32_t type, uint32_t stride, void const * pointer )
{
	if ( m_pfnBinormalPointer )
	{
		m_pfnBinormalPointer( type, stride, const_cast< void * >( pointer ) );
	}

	return glCheckError( *this, "glBinormalPointer" );
}

bool OpenGl::HasTangentPointer()
{
	return m_pfnTangentPointer != NULL;
}

bool OpenGl::HasBinormalPointer()
{
	return m_pfnBinormalPointer != NULL;
}

bool OpenGl::ColorPointer( int size, uint32_t type, uint32_t stride, void const * pointer )
{
	m_pfnColorPointer( size, type, stride, pointer );
	return glCheckError( *this, "glColorPointer" );
}

bool OpenGl::TexCoordPointer( int size, uint32_t type, uint32_t stride, void const * pointer )
{
	m_pfnTexCoordPointer( size, type, stride, pointer );
	return glCheckError( *this, "glTexCoordPointer" );
}

bool OpenGl::BufferAddressRange( eGL_ADDRESS pname, uint32_t index, uint64_t address, size_t length )
{
	return m_pBufFunctions->BufferAddressRange( pname, index, address, length );
}

bool OpenGl::VertexFormat( int size, eGL_TYPE type, int stride )
{
	return m_pBufFunctions->VertexFormat( size, type, stride );
}

bool OpenGl::NormalFormat( eGL_TYPE type, int stride )
{
	return m_pBufFunctions->NormalFormat( type, stride );
}

bool OpenGl::ColorFormat( int size, eGL_TYPE type, int stride )
{
	return m_pBufFunctions->ColorFormat( size, type, stride );
}

bool OpenGl::IndexFormat( eGL_TYPE type, int stride )
{
	return m_pBufFunctions->IndexFormat( type, stride );
}

bool OpenGl::TexCoordFormat( int size, eGL_TYPE type, int stride )
{
	return m_pBufFunctions->TexCoordFormat( size, type, stride );
}

bool OpenGl::EdgeFlagFormat( int stride )
{
	return m_pBufFunctions->EdgeFlagFormat( stride );
}

bool OpenGl::SecondaryColorFormat( int size, eGL_TYPE type, int stride )
{
	return m_pBufFunctions->SecondaryColorFormat( size, type, stride );
}

bool OpenGl::FogCoordFormat( uint32_t type, int stride )
{
	return m_pBufFunctions->FogCoordFormat( type, stride );
}

bool OpenGl::VertexAttribFormat( uint32_t index, int size, eGL_TYPE type, bool normalized, int stride )
{
	return m_pBufFunctions->VertexAttribFormat( index, size, type, normalized, stride );
}

bool OpenGl::VertexAttribIFormat( uint32_t index, int size, eGL_TYPE type, int stride )
{
	return m_pBufFunctions->VertexAttribIFormat( index, size, type, stride );
}

bool OpenGl::MakeBufferResident( eGL_BUFFER_TARGET target, eGL_RESIDENT_BUFFER_ACCESS access )
{
	return m_pBufFunctions->MakeBufferResident( target, access );
}

bool OpenGl::MakeBufferNonResident( eGL_BUFFER_TARGET target )
{
	return m_pBufFunctions->MakeBufferNonResident( target );
}

bool OpenGl::IsBufferResident( eGL_BUFFER_TARGET target )
{
	return m_pBufFunctions->IsBufferResident( target );
}

bool OpenGl::MakeNamedBufferResident( uint32_t buffer, eGL_RESIDENT_BUFFER_ACCESS access )
{
	return m_pBufFunctions->MakeNamedBufferResident( buffer, access );
}

bool OpenGl::MakeNamedBufferNonResident( uint32_t buffer )
{
	return m_pBufFunctions->MakeNamedBufferNonResident( buffer );
}

bool OpenGl::IsNamedBufferResident( uint32_t buffer )
{
	return m_pBufFunctions->IsNamedBufferResident( buffer );
}

bool OpenGl::GetNamedBufferParameter( uint32_t buffer, eGL_BUFFER_PARAMETER pname, uint64_t * params )
{
	return m_pBufFunctions->GetNamedBufferParameter( buffer, pname, params );
}

bool OpenGl::GenFramebuffers( int n, uint32_t * framebuffers )
{
	m_pfnGenFramebuffers( n, framebuffers );
	return glCheckError( *this, "glGenFramebuffers" );
}

bool OpenGl::DeleteFramebuffers( int n, uint32_t const * framebuffers )
{
	m_pfnDeleteFramebuffers( n, framebuffers );
	return glCheckError( *this, "glDeleteFramebuffers" );
}

bool OpenGl::BindFramebuffer( eGL_FRAMEBUFFER_MODE p_eBindingMode, uint32_t framebuffer )
{
	m_pfnBindFramebuffer( p_eBindingMode, framebuffer );
	return glCheckError( *this, "glBindFramebuffer" );
}

bool OpenGl::FramebufferTexture( eGL_FRAMEBUFFER_MODE p_eTarget, eGL_TEXTURE_ATTACHMENT p_eAttachment, uint32_t texture, int level )
{
	m_pfnFramebufferTexture( p_eTarget, p_eAttachment, texture, level );
	return glCheckError( *this, "glFramebufferTexture" );
}

bool OpenGl::FramebufferTexture1D( eGL_FRAMEBUFFER_MODE p_eTarget, eGL_TEXTURE_ATTACHMENT p_eAttachment, eGL_TEXDIM p_eTextarget, uint32_t texture, int level )
{
	m_pfnFramebufferTexture1D( p_eTarget, p_eAttachment, p_eTextarget, texture, level );
	return glCheckError( *this, "glFramebufferTexture1D" );
}

bool OpenGl::FramebufferTexture2D( eGL_FRAMEBUFFER_MODE p_eTarget, eGL_TEXTURE_ATTACHMENT p_eAttachment, eGL_TEXDIM p_eTextarget, uint32_t texture, int level )
{
	m_pfnFramebufferTexture2D( p_eTarget, p_eAttachment, p_eTextarget, texture, level );
	return glCheckError( *this, "glFramebufferTexture2D" );
}

bool OpenGl::FramebufferTexture3D( eGL_FRAMEBUFFER_MODE p_eTarget, eGL_TEXTURE_ATTACHMENT p_eAttachment, eGL_TEXDIM p_eTextarget, uint32_t texture, int level, int layer )
{
	m_pfnFramebufferTexture3D( p_eTarget, p_eAttachment, p_eTextarget, texture, level, layer );
	return glCheckError( *this, "glFramebufferTexture3D" );
}

bool OpenGl::FramebufferTextureLayer( eGL_FRAMEBUFFER_MODE p_eTarget, eGL_TEXTURE_ATTACHMENT p_eAttachment, uint32_t texture, int level, int layer )
{
	m_pfnFramebufferTextureLayer( p_eTarget, p_eAttachment, texture, level, layer );
	return glCheckError( *this, "glFramebufferTextureLayer" );
}

bool OpenGl::FramebufferRenderbuffer( eGL_FRAMEBUFFER_MODE p_eTarget, eGL_RENDERBUFFER_ATTACHMENT p_eAttachmentPoint, eGL_RENDERBUFFER_MODE p_eRenderbufferTarget, uint32_t renderbufferId )
{
	m_pfnFramebufferRenderbuffer( p_eTarget, p_eAttachmentPoint, p_eRenderbufferTarget, renderbufferId );
	return glCheckError( *this, "glFramebufferRenderbuffer" );
}

bool OpenGl::GenRenderbuffers( int n, uint32_t * ids )
{
	m_pfnGenRenderbuffers( n, ids );
	return glCheckError( *this, "glGenRenderbuffers" );
}

bool OpenGl::DeleteRenderbuffers( int n, uint32_t const * ids )
{
	m_pfnDeleteRenderbuffers( n, ids );
	return glCheckError( *this, "glDeleteRenderbuffers" );
}

bool OpenGl::BindRenderbuffer( eGL_RENDERBUFFER_MODE p_eBindingMode, uint32_t id )
{
	m_pfnBindRenderbuffer( p_eBindingMode, id );
	return glCheckError( *this, "glBindRenderbuffer" );
}

bool OpenGl::RenderbufferStorage( eGL_RENDERBUFFER_MODE p_eBindingMode, eGL_RENDERBUFFER_STORAGE p_eFormat, int width, int height )
{
	m_pfnRenderbufferStorage( p_eBindingMode, p_eFormat, width, height );
	return glCheckError( *this, "glRenderbufferStorage" );
}

bool OpenGl::RenderbufferStorageMultisample( eGL_RENDERBUFFER_MODE p_eBindingMode, int p_iSamples, eGL_RENDERBUFFER_STORAGE p_eFormat, int width, int height )
{
	bool l_bReturn = true;
	int l_iMaxSamples;
	int l_iMaxSize;
	OpenGl::GetIntegerv( eGL_MAX_SAMPLES,			&l_iMaxSamples	);
	OpenGl::GetIntegerv( eGL_MAX_RENDERBUFFER_SIZE,	&l_iMaxSize	);

	if ( p_iSamples <= l_iMaxSamples && width <= l_iMaxSize && height < l_iMaxSize )
	{
		m_pfnRenderbufferStorageMultisample( p_eBindingMode, p_iSamples, p_eFormat, width, height );
		l_bReturn = glCheckError( *this, "glRenderbufferStorageMultisample" );
	}
	else if ( p_iSamples > l_iMaxSamples )
	{
		Logger::LogWarning( String( cuT( "glRenderbufferStorageMultisample - Asked for more samples than available, setting it to max available" ) ) );
		m_pfnRenderbufferStorageMultisample( p_eBindingMode, l_iMaxSamples, p_eFormat, width, height );
		l_bReturn = glCheckError( *this, "glRenderbufferStorageMultisample" );
	}
	else if ( width > l_iMaxSize )
	{
		Logger::LogError( String( cuT( "glRenderbufferStorageMultisample - Asked for greater width than available" ) ) );
		l_bReturn = false;
	}
	else if ( height > l_iMaxSize )
	{
		Logger::LogError( String( cuT( "glRenderbufferStorageMultisample - Asked for greater height than available" ) ) );
		l_bReturn = false;
	}

	return l_bReturn;
}

bool OpenGl::RenderbufferStorage( eGL_RENDERBUFFER_MODE p_eBindingMode, eGL_RENDERBUFFER_STORAGE p_eFormat, Castor::Size const & size )
{
	m_pfnRenderbufferStorage( p_eBindingMode, p_eFormat, size.width(), size.height() );
	return glCheckError( *this, "glRenderbufferStorage" );
}

bool OpenGl::RenderbufferStorageMultisample( eGL_RENDERBUFFER_MODE p_eBindingMode, int p_iSamples, eGL_RENDERBUFFER_STORAGE p_eFormat, Castor::Size const & size )
{
	bool l_bReturn = true;
	int l_iMaxSamples;
	int l_iMaxSize;
	OpenGl::GetIntegerv( eGL_MAX_SAMPLES,				&l_iMaxSamples	);
	OpenGl::GetIntegerv( eGL_MAX_RENDERBUFFER_SIZE, 	&l_iMaxSize	);

	if ( p_iSamples <= l_iMaxSamples && int( size.width() ) <= l_iMaxSize && int( size.height() ) < l_iMaxSize )
	{
		m_pfnRenderbufferStorageMultisample( p_eBindingMode, p_iSamples, p_eFormat, size.width(), size.height() );
		l_bReturn = glCheckError( *this, "glRenderbufferStorageMultisample" );
	}
	else if ( p_iSamples > l_iMaxSamples )
	{
		Logger::LogWarning( String( cuT( "glRenderbufferStorageMultisample - Asked for more samples than available, setting it to max available" ) ) );
		m_pfnRenderbufferStorageMultisample( p_eBindingMode, l_iMaxSamples, p_eFormat, size.width(), size.height() );
		l_bReturn = glCheckError( *this, "glRenderbufferStorageMultisample" );
	}
	else if ( int( size.width() ) > l_iMaxSize )
	{
		Logger::LogError( String( cuT( "glRenderbufferStorageMultisample - Asked for greater width than available" ) ) );
		l_bReturn = false;
	}
	else if ( int( size.height() ) > l_iMaxSize )
	{
		Logger::LogError( String( cuT( "glRenderbufferStorageMultisample - Asked for greater height than available" ) ) );
		l_bReturn = false;
	}

	return l_bReturn;
}

bool OpenGl::GetRenderbufferParameteriv( eGL_RENDERBUFFER_MODE p_eBindingMode, eGL_RENDERBUFFER_PARAMETER p_eParam, int * values )
{
	m_pfnGetRenderbufferParameteriv( p_eBindingMode, p_eParam, values );
	return glCheckError( *this, "glGetRenderbufferParameteriv" );
}

bool OpenGl::BlitFramebuffer( int srcX0, int srcY0, int srcX1, int srcY1, int dstX0, int dstY0, int dstX1, int dstY1, uint32_t mask, uint32_t filter )
{
	m_pfnBlitFramebuffer( srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter );
	return glCheckError( *this, "glBlitFramebuffer" );
}

bool OpenGl::BlitFramebuffer( Castor::Rectangle const & rcSrc, Castor::Rectangle const & rcDst, uint32_t mask, uint32_t filter )
{
	m_pfnBlitFramebuffer( rcSrc.left(), rcSrc.top(), rcSrc.right(), rcSrc.bottom(), rcDst.left(), rcDst.top(), rcDst.right(), rcDst.bottom(), mask, filter );
	return glCheckError( *this, "glBlitFramebuffer" );
}

bool OpenGl::DrawBuffers( int n, const uint32_t * bufs )
{
	m_pfnDrawBuffers( n, bufs );
	return glCheckError( *this, "glDrawBuffers" );
}

uint32_t OpenGl::CheckFramebufferStatus( eGL_FRAMEBUFFER_MODE p_eTarget )
{
	uint32_t l_eReturn = m_pfnCheckFramebufferStatus( p_eTarget );

	if ( ! glCheckError( *this, "glCheckFramebufferStatus" ) )
	{
		l_eReturn = eGL_INVALID_INDEX;
	}

	return l_eReturn;
}

bool OpenGl::TexImage2DMultisample( eGL_TEXDIM p_eTarget, int p_iSamples, eGL_INTERNAL p_eInternalFormat, int p_iWidth, int p_iHeight, bool p_bFixedSampleLocations )
{
	try
	{
		m_pfnTexImage2DMultisample( p_eTarget, p_iSamples, p_eInternalFormat, p_iWidth, p_iHeight, p_bFixedSampleLocations );
		return glCheckError( *this, "glTexImage2DMultisample" );
	}
	catch ( ... )
	{
		return false;
	}
}

bool OpenGl::TexImage2DMultisample( eGL_TEXDIM p_eTarget, int p_iSamples, eGL_INTERNAL p_eInternalFormat, Size const & p_size, bool p_bFixedSampleLocations )
{
	try
	{
		m_pfnTexImage2DMultisample( p_eTarget, p_iSamples, p_eInternalFormat, p_size.width(), p_size.height(), p_bFixedSampleLocations );
		return glCheckError( *this, "glTexImage2DMultisample" );
	}
	catch ( ... )
	{
		return false;
	}
}

int OpenGl::GetUniformLocation( uint32_t program, char const * name )
{
	int l_iReturn = m_pfnGetUniformLocation( program, name );

	if ( ! glCheckError( *this, "glGetUniformLocation" ) )
	{
		l_iReturn = eGL_INVALID_INDEX;
	}

	return l_iReturn;
}

bool OpenGl::SetUniform( int location, int v0 )
{
	m_pfnUniform1i( location, v0 );
	return glCheckError( *this, "glUniform1i" );
}

bool OpenGl::SetUniform( int location, int v0, int v1 )
{
	m_pfnUniform2i( location, v0, v1 );
	return glCheckError( *this, "glUniform2i" );
}

bool OpenGl::SetUniform( int location, int v0, int v1, int v2 )
{
	m_pfnUniform3i( location, v0, v1, v2 );
	return glCheckError( *this, "glUniform3i" );
}

bool OpenGl::SetUniform( int location, int v0, int v1, int v2, int v3 )
{
	m_pfnUniform4i( location, v0, v1, v2, v3 );
	return glCheckError( *this, "glUniform4i" );
}

bool OpenGl::SetUniform1v( int location, int count, int const * params )
{
	m_pfnUniform1iv( location, count, params );
	return glCheckError( *this, "glUniform1iv" );
}

bool OpenGl::SetUniform2v( int location, int count, int const * params )
{
	m_pfnUniform2iv( location, count, params );
	return glCheckError( *this, "glUniform2iv" );
}

bool OpenGl::SetUniform3v( int location, int count, int const * params )
{
	m_pfnUniform3iv( location, count, params );
	return glCheckError( *this, "glUniform3iv" );
}

bool OpenGl::SetUniform4v( int location, int count, int const * params )
{
	m_pfnUniform4iv( location, count, params );
	return glCheckError( *this, "glUniform4iv" );
}

bool OpenGl::SetUniform( int location, uint32_t v0 )
{
	m_pfnUniform1ui( location, v0 );
	return glCheckError( *this, "glUniform1ui" );
}

bool OpenGl::SetUniform( int location, uint32_t v0, uint32_t v1 )
{
	m_pfnUniform2ui( location, v0, v1 );
	return glCheckError( *this, "glUniform2ui" );
}

bool OpenGl::SetUniform( int location, uint32_t v0, uint32_t v1, uint32_t v2 )
{
	m_pfnUniform3ui( location, v0, v1, v2 );
	return glCheckError( *this, "glUniformu3i" );
}

bool OpenGl::SetUniform( int location, uint32_t v0, uint32_t v1, uint32_t v2, uint32_t v3 )
{
	m_pfnUniform4ui( location, v0, v1, v2, v3 );
	return glCheckError( *this, "glUniform4ui" );
}

bool OpenGl::SetUniform1v( int location, int count, uint32_t const * params )
{
	m_pfnUniform1uiv( location, count, params );
	return glCheckError( *this, "glUniform1uiv" );
}

bool OpenGl::SetUniform2v( int location, int count, uint32_t const * params )
{
	m_pfnUniform2uiv( location, count, params );
	return glCheckError( *this, "glUniform2uiv" );
}

bool OpenGl::SetUniform3v( int location, int count, uint32_t const * params )
{
	m_pfnUniform3uiv( location, count, params );
	return glCheckError( *this, "glUniform3uiv" );
}

bool OpenGl::SetUniform4v( int location, int count, uint32_t const * params )
{
	m_pfnUniform4uiv( location, count, params );
	return glCheckError( *this, "glUniform4uiv" );
}

bool OpenGl::SetUniform( int location, float v0 )
{
	m_pfnUniform1f( location, v0 );
	return glCheckError( *this, "glUniform1f" );
}

bool OpenGl::SetUniform( int location, float v0, float v1 )
{
	m_pfnUniform2f( location, v0, v1 );
	return glCheckError( *this, "glUniform2f" );
}

bool OpenGl::SetUniform( int location, float v0, float v1, float v2 )
{
	m_pfnUniform3f( location, v0, v1, v2 );
	return glCheckError( *this, "glUniform3f" );
}

bool OpenGl::SetUniform( int location, float v0, float v1, float v2, float v3 )
{
	m_pfnUniform4f( location, v0, v1, v2, v3 );
	return glCheckError( *this, "glUniform4f" );
}

bool OpenGl::SetUniform1v( int location, int count, float const * params )
{
	m_pfnUniform1fv( location, count, params );
	return glCheckError( *this, "glUniform1fv" );
}

bool OpenGl::SetUniform2v( int location, int count, float const * params )
{
	m_pfnUniform2fv( location, count, params );
	return glCheckError( *this, "glUniform2fv" );
}

bool OpenGl::SetUniform3v( int location, int count, float const * params )
{
	m_pfnUniform3fv( location, count, params );
	return glCheckError( *this, "glUniform3fv" );
}

bool OpenGl::SetUniform4v( int location, int count, float const * params )
{
	m_pfnUniform4fv( location, count, params );
	return glCheckError( *this, "glUniform4fv" );
}

bool OpenGl::SetUniform( int location, double v0 )
{
	m_pfnUniform1d( location, v0 );
	return glCheckError( *this, "glUniform1d" );
}

bool OpenGl::SetUniform( int location, double v0, double v1 )
{
	if ( m_pfnUniform2d )
	{
		m_pfnUniform2d( location, v0, v1 );
	}

	return glCheckError( *this, "glUniform2d" );
}

bool OpenGl::SetUniform( int location, double v0, double v1, double v2 )
{
	if ( m_pfnUniform3d )
	{
		m_pfnUniform3d( location, v0, v1, v2 );
	}

	return glCheckError( *this, "glUniform3d" );
}

bool OpenGl::SetUniform( int location, double v0, double v1, double v2, double v3 )
{
	if ( m_pfnUniform4d )
	{
		m_pfnUniform4d( location, v0, v1, v2, v3 );
	}

	return glCheckError( *this, "glUniform4d" );
}

bool OpenGl::SetUniform1v( int location, int count, double const * params )
{
	if ( m_pfnUniform1dv )
	{
		m_pfnUniform1dv( location, count, params );
	}

	return glCheckError( *this, "glUniform1dv" );
}

bool OpenGl::SetUniform2v( int location, int count, double const * params )
{
	if ( m_pfnUniform2dv )
	{
		m_pfnUniform2dv( location, count, params );
	}

	return glCheckError( *this, "glUniform2dv" );
}

bool OpenGl::SetUniform3v( int location, int count, double const * params )
{
	if ( m_pfnUniform3dv )
	{
		m_pfnUniform3dv( location, count, params );
	}

	return glCheckError( *this, "glUniform3dv" );
}

bool OpenGl::SetUniform4v( int location, int count, double const * params )
{
	if ( m_pfnUniform4dv )
	{
		m_pfnUniform4dv( location, count, params );
	}

	return glCheckError( *this, "glUniform4dv" );
}

bool OpenGl::SetUniformMatrix2x2v( int location, int count, bool transpose, float const * value )
{
	m_pfnUniformMatrix2fv( location, count, transpose, value );
	return glCheckError( *this, "glUniformMatrix2fv" );
}

bool OpenGl::SetUniformMatrix2x3v( int location, int count, bool transpose, float const * value )
{
	m_pfnUniformMatrix2x3fv( location, count, transpose, value );
	return glCheckError( *this, "glUniformMatrix2x3fv" );
}

bool OpenGl::SetUniformMatrix2x4v( int location, int count, bool transpose, float const * value )
{
	m_pfnUniformMatrix2x4fv( location, count, transpose, value );
	return glCheckError( *this, "glUniformMatrix2x4fv" );
}

bool OpenGl::SetUniformMatrix3x2v( int location, int count, bool transpose, float const * value )
{
	m_pfnUniformMatrix3x2fv( location, count, transpose, value );
	return glCheckError( *this, "glUniformMatrix3x2fv" );
}

bool OpenGl::SetUniformMatrix3x3v( int location, int count, bool transpose, float const * value )
{
	m_pfnUniformMatrix3fv( location, count, transpose, value );
	return glCheckError( *this, "glUniformMatrix3fv" );
}

bool OpenGl::SetUniformMatrix3x4v( int location, int count, bool transpose, float const * value )
{
	m_pfnUniformMatrix3x4fv( location, count, transpose, value );
	return glCheckError( *this, "glUniformMatrix3x4fv" );
}

bool OpenGl::SetUniformMatrix4x2v( int location, int count, bool transpose, float const * value )
{
	m_pfnUniformMatrix4x2fv( location, count, transpose, value );
	return glCheckError( *this, "glUniformMatrix4x2fv" );
}

bool OpenGl::SetUniformMatrix4x3v( int location, int count, bool transpose, float const * value )
{
	m_pfnUniformMatrix4x3fv( location, count, transpose, value );
	return glCheckError( *this, "glUniformMatrix4x3fv" );
}

bool OpenGl::SetUniformMatrix4x4v( int location, int count, bool transpose, float const * value )
{
	m_pfnUniformMatrix4fv( location, count, transpose, value );
	return glCheckError( *this, "glUniformMatrix4fv" );
}

bool OpenGl::SetUniformMatrix2x2v( int location, int count, bool transpose, double const * value )
{
	if ( m_pfnUniformMatrix2dv )
	{
		m_pfnUniformMatrix2dv( location, count, transpose, value );
	}

	return glCheckError( *this, "glUniformMatrix2dv" );
}

bool OpenGl::SetUniformMatrix2x3v( int location, int count, bool transpose, double const * value )
{
	if ( m_pfnUniformMatrix2x3dv )
	{
		m_pfnUniformMatrix2x3dv( location, count, transpose, value );
	}

	return glCheckError( *this, "glUniformMatrix2x3dv" );
}

bool OpenGl::SetUniformMatrix2x4v( int location, int count, bool transpose, double const * value )
{
	if ( m_pfnUniformMatrix2x4dv )
	{
		m_pfnUniformMatrix2x4dv( location, count, transpose, value );
	}

	return glCheckError( *this, "glUniformMatrix2x4dv" );
}

bool OpenGl::SetUniformMatrix3x2v( int location, int count, bool transpose, double const * value )
{
	if ( m_pfnUniformMatrix3x2dv )
	{
		m_pfnUniformMatrix3x2dv( location, count, transpose, value );
	}

	return glCheckError( *this, "glUniformMatrix3x2dv" );
}

bool OpenGl::SetUniformMatrix3x3v( int location, int count, bool transpose, double const * value )
{
	if ( m_pfnUniformMatrix3dv )
	{
		m_pfnUniformMatrix3dv( location, count, transpose, value );
	}

	return glCheckError( *this, "glUniformMatrix3dv" );
}

bool OpenGl::SetUniformMatrix3x4v( int location, int count, bool transpose, double const * value )
{
	if ( m_pfnUniformMatrix3x4dv )
	{
		m_pfnUniformMatrix3x4dv( location, count, transpose, value );
	}

	return glCheckError( *this, "glUniformMatrix3x4dv" );
}

bool OpenGl::SetUniformMatrix4x2v( int location, int count, bool transpose, double const * value )
{
	if ( m_pfnUniformMatrix4x2dv )
	{
		m_pfnUniformMatrix4x2dv( location, count, transpose, value );
	}

	return glCheckError( *this, "glUniformMatrix4x2dv" );
}

bool OpenGl::SetUniformMatrix4x3v( int location, int count, bool transpose, double const * value )
{
	if ( m_pfnUniformMatrix4x3dv )
	{
		m_pfnUniformMatrix4x3dv( location, count, transpose, value );
	}

	return glCheckError( *this, "glUniformMatrix4x3dv" );
}

bool OpenGl::SetUniformMatrix4x4v( int location, int count, bool transpose, double const * value )
{
	if ( m_pfnUniformMatrix4dv )
	{
		m_pfnUniformMatrix4dv( location, count, transpose, value );
	}

	return glCheckError( *this, "glUniformMatrix4dv" );
}

uint32_t OpenGl::GetUniformBlockIndex( uint32_t program, char const * uniformBlockName )
{
	uint32_t l_uiReturn = m_pfnGetUniformBlockIndex( program, uniformBlockName );

	if ( !glCheckError( *this, "glGetUniformBlockIndex" ) )
	{
		l_uiReturn = eGL_INVALID_INDEX;
	}

	return l_uiReturn;
}

bool OpenGl::BindBufferBase( uint32_t target, uint32_t index, uint32_t buffer )
{
	m_pfnBindBufferBase( target, index, buffer );
	return glCheckError( *this, "glBindBufferBase" );
}

bool OpenGl::UniformBlockBinding( uint32_t program, uint32_t uniformBlockIndex, uint32_t uniformBlockBinding )
{
	m_pfnUniformBlockBinding( program, uniformBlockIndex, uniformBlockBinding );
	return glCheckError( *this, "glUniformBlockBinding" );
}

bool OpenGl::GetUniformIndices( uint32_t program, int uniformCount, char const ** uniformNames, uint32_t * uniformIndices )
{
	m_pfnGetUniformIndices( program, uniformCount, uniformNames, uniformIndices );
	return glCheckError( *this, "glGetUniformIndices" );
}

bool OpenGl::GetActiveUniformsiv( uint32_t program, int uniformCount, uint32_t const * uniformIndices, eGL_UNIFORM_NAME pname, int * params )
{
	m_pfnGetActiveUniformsiv( program, uniformCount, uniformIndices, pname, params );
	return glCheckError( *this, "glGetActiveUniformsiv" );
}

bool OpenGl::GetActiveUniformBlockiv( uint32_t program, uint32_t uniformBlockIndex, eGL_UNIFORM_NAME pname, int * params )
{
	m_pfnGetActiveUniformBlockiv( program, uniformBlockIndex, pname, params );
	return glCheckError( *this, "glGetActiveUniformBlockiv" );
}

uint32_t OpenGl::CreateShader( eGL_SHADER_TYPE type )
{
	uint32_t l_uiReturn = m_pfnCreateShader( type );

	if ( ! glCheckError( *this, "glCreateShader" ) )
	{
		l_uiReturn = eGL_INVALID_INDEX;
	}

	return l_uiReturn;
}

bool OpenGl::DeleteShader( uint32_t shader )
{
	m_pfnDeleteShader( shader );
	return glCheckError( *this, "glDeleteShader" );
}

bool OpenGl::AttachShader( uint32_t program, uint32_t shader )
{
	m_pfnAttachShader( program, shader );
	return glCheckError( *this, "glAttachShader" );
}

bool OpenGl::DetachShader( uint32_t program, uint32_t shader )
{
	m_pfnDetachShader( program, shader );
	return glCheckError( *this, "glDetachShader" );
}

bool OpenGl::CompileShader( uint32_t shader )
{
	m_pfnCompileShader( shader );
	return glCheckError( *this, "glCompileShader" );
}

bool OpenGl::GetShaderiv( uint32_t shader, uint32_t pname, int * param )
{
	m_pfnGetShaderiv( shader, pname, param );
	return glCheckError( *this, "glGetShaderiv" );
}

bool OpenGl::GetShaderInfoLog( uint32_t shader, int bufSize, int * length, char * infoLog )
{
	m_pfnGetShaderInfoLog( shader, bufSize, length, infoLog );
	return glCheckError( *this, "glGetShaderInfoLog" );
}

bool OpenGl::ShaderSource( uint32_t shader, int count, char const ** strings, int const * lengths )
{
	m_pfnShaderSource( shader, count, strings, lengths );
	return glCheckError( *this, "glShaderSource" );
}

uint32_t OpenGl::CreateProgram()
{
	uint32_t l_uiReturn = m_pfnCreateProgram();

	if ( ! glCheckError( *this, "glCreateProgram" ) )
	{
		l_uiReturn = eGL_INVALID_INDEX;
	}

	return l_uiReturn;
}

bool OpenGl::DeleteProgram( uint32_t program )
{
	m_pfnDeleteProgram( program );
	return glCheckError( *this, "glDeleteProgram" );
}

bool OpenGl::LinkProgram( uint32_t program )
{
	m_pfnLinkProgram( program );
	return glCheckError( *this, "glLinkProgram" );
}

bool OpenGl::UseProgram( uint32_t program )
{
	m_pfnUseProgram( program );
	return glCheckError( *this, "glUseProgram" );
}

bool OpenGl::GetProgramiv( uint32_t program, uint32_t pname, int * param )
{
	m_pfnGetProgramiv( program, pname, param );
	return glCheckError( *this, "glGetProgramiv" );
}

bool OpenGl::GetProgramInfoLog( uint32_t program, int bufSize, int * length, char * infoLog )
{
	m_pfnGetProgramInfoLog( program, bufSize, length, infoLog );
	return glCheckError( *this, "glGetProgramInfoLog" );
}

int OpenGl::GetAttribLocation( uint32_t program, char const * name )
{
	int l_iReturn = m_pfnGetAttribLocation( program, name );

	if ( ! glCheckError( *this, "glGetAttribLocation" ) )
	{
		l_iReturn = eGL_INVALID_INDEX;
	}

	return l_iReturn;
}

bool OpenGl::IsProgram( uint32_t program )
{
	m_pfnIsProgram( program );
	return glCheckError( *this, "glIsProgram" );
}

bool OpenGl::ProgramParameteri( uint32_t program, uint32_t pname, int value )
{
	m_pfnProgramParameteri( program, pname, value );
	return glCheckError( *this, "glProgramParameteri" );
}

bool OpenGl::EnableVertexAttribArray( uint32_t index )
{
	m_pfnEnableVertexAttribArray( index );
	return glCheckError( *this, "glEnableVertexAttribArray" );
}

bool OpenGl::VertexAttribPointer( uint32_t index, int size, eGL_TYPE type, bool normalized, int stride, void const * pointer )
{
	m_pfnVertexAttribPointer( index, size, type, normalized, stride, pointer );
	return glCheckError( *this, "glVertexAttribPointer" );
}

bool OpenGl::VertexAttribPointer( uint32_t index, int size, eGL_TYPE type, int stride, void const * pointer )
{
	m_pfnVertexAttribIPointer( index, size, type, stride, pointer );
	return glCheckError( *this, "glVertexAttribIPointer" );
}

bool OpenGl::DisableVertexAttribArray( uint32_t index )
{
	m_pfnDisableVertexAttribArray( index );
	return glCheckError( *this, "glDisableVertexAttribArray" );
}

bool OpenGl::GenVertexArrays( int n, uint32_t * arrays )
{
	m_pfnGenVertexArrays( n, arrays );
	return glCheckError( *this, "glGenVertexArrays" );
}

bool OpenGl::BindVertexArray( uint32_t array )
{
	m_pfnBindVertexArray( array );
	return glCheckError( *this, "glBindVertexArray" );
}

bool OpenGl::DeleteVertexArrays( int n, uint32_t * arrays )
{
	m_pfnDeleteVertexArrays( n, arrays );
	return glCheckError( *this, "glDeleteVertexArrays" );
}

bool OpenGl::PatchParameter( eGL_PATCH_PARAMETER p_eParam, int p_iValue )
{
	bool l_bReturn = false;

	if ( m_pfnPatchParameteri )
	{
		m_pfnPatchParameteri( p_eParam, p_iValue );
		l_bReturn = glCheckError( *this, "glPatchParameteri" );
	}

	return l_bReturn;
}

bool OpenGl::GlCheckError( std::string const & p_text )const
{
	return DoGlCheckError( str_utils::from_str( p_text ) );
}

bool OpenGl::GlCheckError( std::wstring const & p_text )const
{
	return DoGlCheckError( str_utils::from_wstr( p_text ) );
}

bool OpenGl::DoGlCheckError( String const & p_text )const
{
	bool l_bReturn = true;
	uint32_t l_errorCode = GetError();

	if ( l_errorCode != GL_NO_ERROR )
	{
		String l_strSysError = System::GetLastErrorText();
		l_errorCode -= GL_INVALID_ENUM;
		String l_strError = p_text + cuT( " - " ) + GlslErrors[l_errorCode];
		l_strError += cuT( " - " ) + l_strSysError;
		Logger::LogError( l_strError );
		l_bReturn = false;
	}

	return l_bReturn;
}

eGL_LOCK OpenGl::GetLockFlags( uint32_t p_uiFlags )const
{
	eGL_LOCK l_eLockFlags = eGL_LOCK_READ_WRITE;

	if ( p_uiFlags & eLOCK_FLAG_READ_ONLY )
	{
		if ( p_uiFlags & eLOCK_FLAG_WRITE_ONLY )
		{
			l_eLockFlags = eGL_LOCK_READ_WRITE;
		}
		else
		{
			l_eLockFlags = eGL_LOCK_READ_ONLY;
		}
	}
	else if ( p_uiFlags & eLOCK_FLAG_WRITE_ONLY )
	{
		l_eLockFlags = eGL_LOCK_WRITE_ONLY;
	}

	return l_eLockFlags;
}

uint32_t OpenGl::GetBitfieldFlags( uint32_t p_uiFlags )const
{
	uint32_t l_uiFlags = 0;

	if ( p_uiFlags & eLOCK_FLAG_READ_ONLY )
	{
		l_uiFlags = eGL_BUFFER_MAPPING_BIT_READ;

		if ( p_uiFlags & eLOCK_FLAG_WRITE_ONLY )
		{
			l_uiFlags |= eGL_BUFFER_MAPPING_BIT_WRITE;
		}
	}
	else if ( p_uiFlags & eLOCK_FLAG_WRITE_ONLY )
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

void OpenGl::DebugLog( eGL_DEBUG_SOURCE source, eGL_DEBUG_TYPE type, uint32_t id, eGL_DEBUG_SEVERITY severity, int CU_PARAM_UNUSED( length ), const char * message )
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

	l_strToLog += cuT( "ID:" ) + str_utils::to_string( id ) + cuT( "\t" );

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

	Logger::LogWarning(	l_strToLog + cuT( "Message:" ) + str_utils::from_str( message ) );
}

void OpenGl::DebugLogAMD( uint32_t id, eGL_DEBUG_CATEGORY category, eGL_DEBUG_SEVERITY severity, int CU_PARAM_UNUSED( length ), const char * message )
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

	l_strToLog += cuT( "ID:" ) + str_utils::to_string( id ) + cuT( "\t" );

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

	Logger::LogWarning(	l_strToLog + cuT( "Message:" ) + str_utils::from_str( message ) );
}

bool OpenGl::HasExtension( Castor::String const & p_strExtName )const
{
	bool l_bReturn = m_strExtensions.find( p_strExtName ) != Castor::String::npos;

	if ( l_bReturn )
	{
		Logger::LogDebug( cuT( "Extension [" ) + p_strExtName + cuT( "] available" ) );
	}
	else
	{
		Logger::LogWarning( cuT( "Extension [" ) + p_strExtName + cuT( "] unavailable" ) );
	}

	return l_bReturn;
}

bool OpenGl::HasMultiTexturing()const
{
	return m_pfnActiveTexture && m_pfnClientActiveTexture;
}

//*************************************************************************************************
