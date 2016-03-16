#include <Logger.hpp>
#include <Position.hpp>
#include <Rectangle.hpp>
#include <Size.hpp>
#include <Utils.hpp>

namespace GlRender
{
	//*************************************************************************************************

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

	bool TexFunctions::TexSubImage2D( eGL_TEXDIM mode, int level, Castor::Position const & p_position, Castor::Size const & p_size, eGL_FORMAT format, eGL_TYPE type, void const * data )
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

	bool TexFunctions::TexImage2D( eGL_TEXDIM mode, int level, eGL_INTERNAL internalFormat, Castor::Size const & p_size, int border, eGL_FORMAT format, uint32_t type, void const * data )
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

	bool TexFunctionsDSA::TexSubImage2D( eGL_TEXDIM mode, int level, Castor::Position const & p_position, Castor::Size const & p_size, eGL_FORMAT format, eGL_TYPE type, void const * data )
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

	bool TexFunctionsDSA::TexImage2D( eGL_TEXDIM mode, int level, eGL_INTERNAL internalFormat, Castor::Size const & p_size, int border, eGL_FORMAT format, uint32_t type, void const * data )
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
			l_return = nullptr;
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
		void * l_return = m_pfnMapBufferRange( p_target, offset, length, access );

		if ( !glCheckError( GetOpenGl(), cuT( "glMapBufferRange" ) ) )
		{
			l_return = nullptr;
		}

		return l_return;
	}

	bool BufFunctions::FlushMappedBufferRange( eGL_BUFFER_TARGET p_target, ptrdiff_t offset, ptrdiff_t length )
	{
		m_pfnFlushMappedBufferRange( p_target, offset, length );
		return glCheckError( GetOpenGl(), cuT( "glFlushMappedBufferRange" ) );
	}

	//*************************************************************************************************

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
			l_return = nullptr;
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
		void * l_return = m_pfnMapNamedBufferRange( m_uiBuffer, offset, length, access );

		if ( !glCheckError( GetOpenGl(), cuT( "glMapBufferRange" ) ) )
		{
			l_return = nullptr;
		}

		return l_return;
	}

	bool BufFunctionsDSA::FlushMappedBufferRange( eGL_BUFFER_TARGET p_target, ptrdiff_t offset, ptrdiff_t length )
	{
		m_pfnFlushMappedNamedBufferRange( m_uiBuffer, offset, length );
		return glCheckError( GetOpenGl(), cuT( "glFlushMappedBufferRange" ) );
	}

	//*************************************************************************************************

	bool OpenGl::HasVao()const
	{
		return m_bHasVao;
	}

	bool OpenGl::HasUbo()const
	{
		return m_bHasUbo;
	}

	bool OpenGl::HasPbo()const
	{
		return m_bHasPbo;
	}

	bool OpenGl::HasTbo()const
	{
		return m_bHasTbo;
	}

	bool OpenGl::HasFbo()const
	{
		return m_bHasFbo;
	}

	bool OpenGl::HasVSh()const
	{
		return m_bHasVSh;
	}

	bool OpenGl::HasPSh()const
	{
		return m_bHasPSh;
	}

	bool OpenGl::HasGSh()const
	{
		return m_bHasGSh;
	}

	bool OpenGl::HasTSh()const
	{
		return m_bHasTSh;
	}

	bool OpenGl::HasCSh()const
	{
		return m_bHasCSh;
	}

	bool OpenGl::HasSpl()const
	{
		return m_bHasSpl;
	}

	bool OpenGl::HasVbo()const
	{
		return m_bHasVbo;
	}

	bool OpenGl::HasDepthClipping()const
	{
		return m_bHasDepthClipping;
	}

	bool OpenGl::HasInstancing()const
	{
		return m_bHasInstancedDraw && m_bHasInstancedArrays;
	}

	bool OpenGl::HasNonPowerOfTwoTextures()const
	{
		return m_bHasNonPowerOfTwoTextures;
	}

	bool OpenGl::CanBindVboToGpuAddress()const
	{
		return m_bBindVboToGpuAddress;
	}
	Castor3D::eELEMENT_TYPE OpenGl::Get( eGLSL_ATTRIBUTE_TYPE p_type )const
	{
		switch ( p_type )
		{
		case eGLSL_ATTRIBUTE_TYPE_FLOAT:
			return Castor3D::eELEMENT_TYPE_1FLOAT;
			break;

		case eGLSL_ATTRIBUTE_TYPE_FLOAT_VEC2:
			return Castor3D::eELEMENT_TYPE_2FLOATS;
			break;

		case eGLSL_ATTRIBUTE_TYPE_FLOAT_VEC3:
			return Castor3D::eELEMENT_TYPE_3FLOATS;
			break;

		case eGLSL_ATTRIBUTE_TYPE_FLOAT_VEC4:
			return Castor3D::eELEMENT_TYPE_4FLOATS;
			break;

		case eGLSL_ATTRIBUTE_TYPE_INT:
			return Castor3D::eELEMENT_TYPE_1INT;
			break;

		case eGLSL_ATTRIBUTE_TYPE_INT_VEC2:
			return Castor3D::eELEMENT_TYPE_2INTS;
			break;

		case eGLSL_ATTRIBUTE_TYPE_INT_VEC3:
			return Castor3D::eELEMENT_TYPE_3INTS;
			break;

		case eGLSL_ATTRIBUTE_TYPE_INT_VEC4:
			return Castor3D::eELEMENT_TYPE_4INTS;
			break;

		case eGLSL_ATTRIBUTE_TYPE_FLOAT_MAT2:
			return Castor3D::eELEMENT_TYPE_2x2FLOATS;
			break;

		case eGLSL_ATTRIBUTE_TYPE_FLOAT_MAT3:
			return Castor3D::eELEMENT_TYPE_3x3FLOATS;
			break;

		case eGLSL_ATTRIBUTE_TYPE_FLOAT_MAT4:
			return Castor3D::eELEMENT_TYPE_4x4FLOATS;
			break;

		default:
			assert( false && "Unsupported attribute type." );
			break;
		}

		return Castor3D::eELEMENT_TYPE_COUNT;
	}

	Castor::String const & OpenGl::GetGlslErrorString( int p_index )const
	{
		return GlslStrings[p_index];
	}

	inline eGL_PRIMITIVE OpenGl::Get( Castor3D::eTOPOLOGY p_index )const
	{
		return PrimitiveTypes[p_index];
	}

	inline eGL_TEXDIM OpenGl::Get( Castor3D::eTEXTURE_TYPE p_index )const
	{
		if ( p_index == Castor3D::eTEXTURE_TYPE_BUFFER )
		{
			if ( !HasTbo() )
			{
				p_index = Castor3D::eTEXTURE_TYPE_1D;
			}
		}

		return TextureDimensions[p_index];
	}

	inline eGL_FUNC OpenGl::Get( Castor3D::eALPHA_FUNC p_eAlphaFunc )const
	{
		return AlphaFuncs[p_eAlphaFunc];
	}

	inline eGL_WRAP_MODE OpenGl::Get( Castor3D::eWRAP_MODE p_eWrapMode )const
	{
		return TextureWrapMode[p_eWrapMode];
	}

	inline eGL_INTERPOLATION_MODE OpenGl::Get( Castor3D::eINTERPOLATION_MODE p_interpolation )const
	{
		return TextureInterpolation[p_interpolation];
	}

	inline int OpenGl::Get( Castor3D::eBLEND_SOURCE p_eArgument )const
	{
		return TextureArguments[p_eArgument];
	}

	inline eGL_BLEND_FUNC OpenGl::Get( Castor3D::eRGB_BLEND_FUNC p_mode )const
	{
		return RgbBlendFuncs[p_mode];
	}

	inline eGL_BLEND_FUNC OpenGl::Get( Castor3D::eALPHA_BLEND_FUNC p_mode )const
	{
		return AlphaBlendFuncs[p_mode];
	}

	inline eGL_BLEND_FACTOR OpenGl::Get( Castor3D::eBLEND p_eBlendFactor )const
	{
		return BlendFactors[p_eBlendFactor];
	}

	inline OpenGl::PixelFmt const & OpenGl::Get( Castor::ePIXEL_FORMAT p_pixelFormat )const
	{
		return PixelFormats[p_pixelFormat];
	}

	inline eGL_SHADER_TYPE OpenGl::Get( Castor3D::eSHADER_TYPE p_type )const
	{
		return ShaderTypes[p_type];
	}

	inline eGL_INTERNAL_FORMAT OpenGl::GetInternal( Castor::ePIXEL_FORMAT p_format )const
	{
		return Internals[p_format];
	}

	inline uint32_t OpenGl::GetComponents( uint32_t p_components )const
	{
		return ( p_components & Castor3D::eBUFFER_COMPONENT_COLOUR ? eGL_BUFFER_BIT_COLOR : 0 ) | ( p_components & Castor3D::eBUFFER_COMPONENT_DEPTH ? eGL_BUFFER_BIT_DEPTH : 0 ) | ( p_components & Castor3D::eBUFFER_COMPONENT_STENCIL ? eGL_BUFFER_BIT_STENCIL : 0 );
	}

	inline eGL_TEXTURE_ATTACHMENT OpenGl::Get( Castor3D::eATTACHMENT_POINT p_eAttachment )const
	{
		return Attachments[p_eAttachment];
	}

	inline eGL_FRAMEBUFFER_MODE OpenGl::Get( Castor3D::eFRAMEBUFFER_TARGET p_target )const
	{
		return FramebufferModes[p_target];
	}

	inline eGL_RENDERBUFFER_ATTACHMENT OpenGl::GetRboAttachment( Castor3D::eATTACHMENT_POINT p_eAttachment )const
	{
		return RboAttachments[p_eAttachment];
	}

	inline eGL_RENDERBUFFER_STORAGE OpenGl::GetRboStorage( Castor::ePIXEL_FORMAT p_pixelFormat )const
	{
		return RboStorages[p_pixelFormat];
	}

	inline eGL_TWEAK OpenGl::Get( Castor3D::eTWEAK p_eTweak )const
	{
		return Tweaks[p_eTweak];
	}

	inline eGL_BUFFER OpenGl::Get( Castor3D::eBUFFER p_buffer )const
	{
		return Buffers[p_buffer];
	}

	inline eGL_BUFFER OpenGl::Get( eGL_TEXTURE_ATTACHMENT p_buffer )const
	{
		return BuffersTA.find( p_buffer )->second;
	}

	inline eGL_BUFFER OpenGl::Get( eGL_RENDERBUFFER_ATTACHMENT p_buffer )const
	{
		return BuffersRBA.find( p_buffer )->second;
	}

	inline eGL_FACE OpenGl::Get( Castor3D::eFACE p_eFace )const
	{
		return Faces[p_eFace];
	}

	inline eGL_FILL_MODE OpenGl::Get( Castor3D::eFILL_MODE p_mode )const
	{
		return FillModes[p_mode];
	}

	inline eGL_FUNC OpenGl::Get( Castor3D::eSTENCIL_FUNC p_func )const
	{
		return StencilFuncs[p_func];
	}

	inline eGL_STENCIL_OP OpenGl::Get( Castor3D::eSTENCIL_OP p_eOp )const
	{
		return StencilOps[p_eOp];
	}

	inline eGL_BLEND_OP OpenGl::Get( Castor3D::eBLEND_OP p_eOp )const
	{
		return BlendOps[p_eOp];
	}

	inline eGL_FUNC OpenGl::Get( Castor3D::eDEPTH_FUNC p_func )const
	{
		return DepthFuncs[p_func];
	}

	inline eGL_QUERY OpenGl::Get( Castor3D::eQUERY_TYPE p_value )const
	{
		return Queries[p_value];
	}

	inline eGL_QUERY_INFO OpenGl::Get( Castor3D::eQUERY_INFO p_value )const
	{
		return QueryInfos[p_value];
	}

	inline bool OpenGl::Get( Castor3D::eWRITING_MASK p_eMask )const
	{
		return WriteMasks[p_eMask];
	}

	inline bool OpenGl::HasDebugOutput()const
	{
		return ( m_pfnDebugMessageCallback || m_pfnDebugMessageCallbackAMD );
	}

	inline Castor::String const & OpenGl::GetVendor()const
	{
		return m_vendor;
	}

	inline Castor::String const & OpenGl::GetRenderer()const
	{
		return m_renderer;
	}

	inline Castor::String const & OpenGl::GetStrVersion()const
	{
		return m_version;
	}

	inline int OpenGl::GetVersion()const
	{
		return m_iVersion;
	}

	inline int OpenGl::GetGlslVersion()const
	{
		return m_iGlslVersion;
	}

	inline GlRenderSystem & OpenGl::GetRenderSystem()
	{
		return m_renderSystem;
	}

	inline GlRenderSystem const & OpenGl::GetRenderSystem()const
	{
		return m_renderSystem;
	}

	inline eGL_BUFFER_MODE OpenGl::GetBufferFlags( uint32_t p_flags )const
	{
		eGL_BUFFER_MODE l_eReturn = eGL_BUFFER_MODE( 0 );

		if ( p_flags & Castor3D::eBUFFER_ACCESS_TYPE_DYNAMIC )
		{
			if ( p_flags & Castor3D::eBUFFER_ACCESS_NATURE_READ )
			{
				l_eReturn = eGL_BUFFER_MODE_DYNAMIC_READ;
			}
			else if ( p_flags & Castor3D::eBUFFER_ACCESS_NATURE_DRAW )
			{
				l_eReturn = eGL_BUFFER_MODE_DYNAMIC_DRAW;
			}
			else if ( p_flags & Castor3D::eBUFFER_ACCESS_NATURE_COPY )
			{
				l_eReturn = eGL_BUFFER_MODE_DYNAMIC_COPY;
			}
		}
		else if ( p_flags & Castor3D::eBUFFER_ACCESS_TYPE_STATIC )
		{
			if ( p_flags & Castor3D::eBUFFER_ACCESS_NATURE_READ )
			{
				l_eReturn = eGL_BUFFER_MODE_STATIC_READ;
			}
			else if ( p_flags & Castor3D::eBUFFER_ACCESS_NATURE_DRAW )
			{
				l_eReturn = eGL_BUFFER_MODE_STATIC_DRAW;
			}
			else if ( p_flags & Castor3D::eBUFFER_ACCESS_NATURE_COPY )
			{
				l_eReturn = eGL_BUFFER_MODE_STATIC_COPY;
			}
		}
		else if ( p_flags & Castor3D::eBUFFER_ACCESS_TYPE_STREAM )
		{
			if ( p_flags & Castor3D::eBUFFER_ACCESS_NATURE_READ )
			{
				l_eReturn = eGL_BUFFER_MODE_STREAM_READ;
			}
			else if ( p_flags & Castor3D::eBUFFER_ACCESS_NATURE_DRAW )
			{
				l_eReturn = eGL_BUFFER_MODE_STREAM_DRAW;
			}
			else if ( p_flags & Castor3D::eBUFFER_ACCESS_NATURE_COPY )
			{
				l_eReturn = eGL_BUFFER_MODE_STREAM_COPY;
			}
		}

		return l_eReturn;
	}

	eGL_LOCK OpenGl::GetLockFlags( uint32_t p_flags )const
	{
		eGL_LOCK l_eLockFlags = eGL_LOCK_READ_WRITE;

		if ( p_flags & Castor3D::eACCESS_TYPE_READ )
		{
			if ( p_flags & Castor3D::eACCESS_TYPE_WRITE )
			{
				l_eLockFlags = eGL_LOCK_READ_WRITE;
			}
			else
			{
				l_eLockFlags = eGL_LOCK_READ_ONLY;
			}
		}
		else if ( p_flags & Castor3D::eACCESS_TYPE_WRITE )
		{
			l_eLockFlags = eGL_LOCK_WRITE_ONLY;
		}

		return l_eLockFlags;
	}

	uint32_t OpenGl::GetBitfieldFlags( uint32_t p_flags )const
	{
		uint32_t l_uiFlags = 0;

		if ( p_flags & Castor3D::eACCESS_TYPE_READ )
		{
			l_uiFlags = eGL_BUFFER_MAPPING_BIT_READ;

			if ( p_flags & Castor3D::eACCESS_TYPE_WRITE )
			{
				l_uiFlags |= eGL_BUFFER_MAPPING_BIT_WRITE;
			}
		}
		else if ( p_flags & Castor3D::eACCESS_TYPE_WRITE )
		{
			l_uiFlags = ( eGL_BUFFER_MAPPING_BIT_WRITE | eGL_BUFFER_MAPPING_BIT_INVALIDATE_RANGE );
		}

		return l_uiFlags;
	}

	bool OpenGl::HasExtension( Castor::String const & p_strExtName, bool p_log )const
	{
		bool l_return = m_extensions.find( p_strExtName ) != Castor::String::npos;

		if ( p_log )
		{
			if ( l_return )
			{
				Castor::Logger::LogDebug( cuT( "Extension [" ) + p_strExtName + cuT( "] available" ) );
			}
			else
			{
				Castor::Logger::LogWarning( cuT( "Extension [" ) + p_strExtName + cuT( "] unavailable" ) );
			}
		}

		return l_return;
	}

	bool OpenGl::HasMultiTexturing()const
	{
		return m_pfnActiveTexture && m_pfnClientActiveTexture;
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
		return m_pfnCreateContextAttribs != nullptr;
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
		return m_pfnCreateContextAttribs != nullptr;
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

	bool OpenGl::TexSubImage2D( eGL_TEXDIM mode, int level, Castor::Position const & p_position, Castor::Size const & p_size, eGL_FORMAT format, eGL_TYPE type, void const * data )const
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

	bool OpenGl::TexImage2D( eGL_TEXDIM mode, int level, eGL_INTERNAL internalFormat, Castor::Size const & p_size, int border, eGL_FORMAT format, uint32_t type, void const * data )const
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

	bool OpenGl::BlendColor( Castor::Colour const & p_colour )const
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

	bool OpenGl::ReadPixels( Castor::Position const & p_position, Castor::Size const & p_size, eGL_FORMAT format, eGL_TYPE type, void * pixels )const
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
		return m_pfnTangentPointer != nullptr;
	}

	bool OpenGl::HasBinormalPointer()const
	{
		return m_pfnBinormalPointer != nullptr;
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

	bool OpenGl::RenderbufferStorage( eGL_RENDERBUFFER_MODE p_eBindingMode, eGL_RENDERBUFFER_STORAGE p_format, int width, int height )const
	{
		m_pfnRenderbufferStorage( p_eBindingMode, p_format, width, height );
		return glCheckError( *this, "glRenderbufferStorage" );
	}

	bool OpenGl::RenderbufferStorageMultisample( eGL_RENDERBUFFER_MODE p_eBindingMode, int p_iSamples, eGL_RENDERBUFFER_STORAGE p_format, int width, int height )const
	{
		bool l_return = true;
		int l_iMaxSamples;
		int l_iMaxSize;
		OpenGl::GetIntegerv( eGL_MAX_SAMPLES, &l_iMaxSamples );
		OpenGl::GetIntegerv( eGL_MAX_RENDERBUFFER_SIZE, &l_iMaxSize );

		if ( p_iSamples <= l_iMaxSamples && width <= l_iMaxSize && height < l_iMaxSize )
		{
			m_pfnRenderbufferStorageMultisample( p_eBindingMode, p_iSamples, p_format, width, height );
			l_return = glCheckError( *this, "glRenderbufferStorageMultisample" );
		}
		else if ( p_iSamples > l_iMaxSamples )
		{
			Castor::Logger::LogWarning( cuT( "glRenderbufferStorageMultisample - Asked for more samples than available, setting it to max available" ) );
			m_pfnRenderbufferStorageMultisample( p_eBindingMode, l_iMaxSamples, p_format, width, height );
			l_return = glCheckError( *this, "glRenderbufferStorageMultisample" );
		}
		else if ( width > l_iMaxSize )
		{
			Castor::Logger::LogError( cuT( "glRenderbufferStorageMultisample - Asked for greater width than available" ) );
			l_return = false;
		}
		else if ( height > l_iMaxSize )
		{
			Castor::Logger::LogError( cuT( "glRenderbufferStorageMultisample - Asked for greater height than available" ) );
			l_return = false;
		}

		return l_return;
	}

	bool OpenGl::RenderbufferStorage( eGL_RENDERBUFFER_MODE p_eBindingMode, eGL_RENDERBUFFER_STORAGE p_format, Castor::Size const & size )const
	{
		m_pfnRenderbufferStorage( p_eBindingMode, p_format, size.width(), size.height() );
		return glCheckError( *this, "glRenderbufferStorage" );
	}

	bool OpenGl::RenderbufferStorageMultisample( eGL_RENDERBUFFER_MODE p_eBindingMode, int p_iSamples, eGL_RENDERBUFFER_STORAGE p_format, Castor::Size const & size )const
	{
		bool l_return = true;
		int l_iMaxSamples;
		int l_iMaxSize;
		OpenGl::GetIntegerv( eGL_MAX_SAMPLES, &l_iMaxSamples );
		OpenGl::GetIntegerv( eGL_MAX_RENDERBUFFER_SIZE, &l_iMaxSize );

		if ( p_iSamples <= l_iMaxSamples && int( size.width() ) <= l_iMaxSize && int( size.height() ) < l_iMaxSize )
		{
			m_pfnRenderbufferStorageMultisample( p_eBindingMode, p_iSamples, p_format, size.width(), size.height() );
			l_return = glCheckError( *this, "glRenderbufferStorageMultisample" );
		}
		else if ( p_iSamples > l_iMaxSamples )
		{
			Castor::Logger::LogWarning( cuT( "glRenderbufferStorageMultisample - Asked for more samples than available, setting it to max available" ) );
			m_pfnRenderbufferStorageMultisample( p_eBindingMode, l_iMaxSamples, p_format, size.width(), size.height() );
			l_return = glCheckError( *this, "glRenderbufferStorageMultisample" );
		}
		else if ( int( size.width() ) > l_iMaxSize )
		{
			Castor::Logger::LogError( cuT( "glRenderbufferStorageMultisample - Asked for greater width than available" ) );
			l_return = false;
		}
		else if ( int( size.height() ) > l_iMaxSize )
		{
			Castor::Logger::LogError( cuT( "glRenderbufferStorageMultisample - Asked for greater height than available" ) );
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
		m_pfnTexImage2DMultisample( p_target, p_iSamples, p_eInternalFormat, p_iWidth, p_iHeight, p_bFixedSampleLocations );
		return glCheckError( *this, "glTexImage2DMultisample" );
	}

	bool OpenGl::TexImage2DMultisample( eGL_TEXDIM p_target, int p_iSamples, eGL_INTERNAL p_eInternalFormat, Castor::Size const & p_size, bool p_bFixedSampleLocations )const
	{
		m_pfnTexImage2DMultisample( p_target, p_iSamples, p_eInternalFormat, p_size.width(), p_size.height(), p_bFixedSampleLocations );
		return glCheckError( *this, "glTexImage2DMultisample" );
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

	bool OpenGl::GetActiveAttrib( uint32_t program, uint32_t index, int bufSize, int * length, int * size, uint32_t * type, char * name )const
	{
		m_pfnGetActiveAttrib( program, index, bufSize, length, size, type, name );
		return glCheckError( *this, "glGetActiveAttrib" );
	}

	uint32_t OpenGl::CreateProgram()const
	{
		uint32_t l_uiReturn = m_pfnCreateProgram();

		if ( !glCheckError( *this, "glCreateProgram" ) )
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

		if ( !glCheckError( *this, "glGetAttribLocation" ) )
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

	bool OpenGl::GetProgramInterfaceInfos( uint32_t program, eGLSL_INTERFACE programInterface, eGLSL_DATA_NAME name, int * params )
	{
		m_pfnGetProgramInterfaceiv( program, programInterface, name, params );
		return glCheckError( *this, "glGetProgramInterfaceiv" );
	}

	int OpenGl::GetProgramResourceIndex( uint32_t program, eGLSL_INTERFACE programInterface, char const * const name )
	{
		m_pfnGetProgramResourceIndex( program, programInterface, name );
		return glCheckError( *this, "glGetProgramResourceIndex" );
	}

	int OpenGl::GetProgramResourceLocation( uint32_t program, eGLSL_INTERFACE programInterface, char const * const name )
	{
		m_pfnGetProgramResourceLocation( program, programInterface, name );
		return glCheckError( *this, "glGetProgramResourceLocation" );
	}

	int OpenGl::GetProgramResourceLocationIndex( uint32_t program, eGLSL_INTERFACE programInterface, char const * const name )
	{
		m_pfnGetProgramResourceLocationIndex( program, programInterface, name );
		return glCheckError( *this, "glGetProgramResourceLocationIndex" );
	}

	bool OpenGl::GetProgramResourceName( uint32_t program, eGLSL_INTERFACE programInterface, uint32_t index, int bufSize, int * length, char * name )
	{
		m_pfnGetProgramResourceName( program, programInterface, index, bufSize, length, name );
		return glCheckError( *this, "glGetProgramResourceName" );
	}

	bool OpenGl::GetProgramResourceInfos( uint32_t program, eGLSL_INTERFACE programInterface, uint32_t index, int propCount, uint32_t * props, int bufSize, int * length, int * params )
	{
		m_pfnGetProgramResourceiv( program, programInterface, index, propCount, props, bufSize, length, params );
		return glCheckError( *this, "m_pfnGetProgramResourceiv" );
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

	//*************************************************************************************************
}
