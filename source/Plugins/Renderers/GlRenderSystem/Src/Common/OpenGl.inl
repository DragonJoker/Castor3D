#include <Log/Logger.hpp>
#include <Graphics/Position.hpp>
#include <Graphics/Rectangle.hpp>
#include <Graphics/Size.hpp>
#include <Miscellaneous/Utils.hpp>

namespace GlRender
{
	template< typename FuncT, typename ... ParamsT >
	bool ExecuteFunction( OpenGl const & p_gl, FuncT p_function, char const * const p_name, ParamsT && ... p_params )
	{
		REQUIRE( p_function );
		p_function( p_params... );
		return glCheckError( p_gl, p_name );
	}

#	define EXEC_FUNCTION( Name, ... )\
	ExecuteFunction( GetOpenGl(), m_pfn##Name, "gl"#Name, __VA_ARGS__ )


	//*************************************************************************************************

	bool TexFunctions::GenerateMipmap( GlTexDim mode )
	{
		if ( m_pfnGenerateMipmap )
		{
			m_pfnGenerateMipmap( uint32_t( mode ) );
		}

		return glCheckError( GetOpenGl(), cuT( "glGenerateMipmap" ) );
	}

	bool TexFunctions::BindTexture( GlTexDim mode, uint32_t texture )
	{
		return EXEC_FUNCTION( BindTexture, uint32_t( mode ), texture );
	}

	bool TexFunctions::TexSubImage1D( eGL_TEXTURE_STORAGE mode, int level, int xoffset, int width, GlFormat format, GlType type, void const * data )
	{
		return EXEC_FUNCTION( TexSubImage1D, uint32_t( mode ), level, xoffset, width, uint32_t( format ), uint32_t( type ), data );
	}

	bool TexFunctions::TexSubImage2D( eGL_TEXTURE_STORAGE mode, int level, int xoffset, int yoffset, int width, int height, GlFormat format, GlType type, void const * data )
	{
		return EXEC_FUNCTION( TexSubImage2D, uint32_t( mode ), level, xoffset, yoffset, width, height, uint32_t( format ), uint32_t( type ), data );
	}

	bool TexFunctions::TexSubImage2D( eGL_TEXTURE_STORAGE mode, int level, Castor::Position const & p_position, Castor::Size const & p_size, GlFormat format, GlType type, void const * data )
	{
		return EXEC_FUNCTION( TexSubImage2D, uint32_t( mode ), level, p_position.x(), p_position.y(), p_size.width(), p_size.height(), uint32_t( format ), uint32_t( type ), data );
	}

	bool TexFunctions::TexSubImage2D( eGL_TEXTURE_STORAGE mode, int level, Castor::Rectangle const & p_rect, GlFormat format, GlType type, void const * data )
	{
		return EXEC_FUNCTION( TexSubImage2D, uint32_t( mode ), level, p_rect.left(), p_rect.top(), p_rect.width(), p_rect.height(), uint32_t( format ), uint32_t( type ), data );
	}

	bool TexFunctions::TexSubImage3D( eGL_TEXTURE_STORAGE mode, int level, int xoffset, int yoffset, int zoffset, int width, int height, int depth, GlFormat format, GlType type, void const * data )
	{
		return EXEC_FUNCTION( TexSubImage3D, uint32_t( mode ), level, xoffset, yoffset, zoffset, width, height, depth, uint32_t( format ), uint32_t( type ), data );
	}

	bool TexFunctions::TexImage1D( eGL_TEXTURE_STORAGE mode, int level, GlInternal internalFormat, int width, int border, GlFormat format, GlType type, void const * data )
	{
		try
		{
			return EXEC_FUNCTION( TexImage1D, uint32_t( mode ), level, uint32_t( internalFormat ), width, border, uint32_t( format ), uint32_t( type ), data );
		}
		catch ( ... )
		{
			return false;
		}
	}

	bool TexFunctions::TexImage2D( eGL_TEXTURE_STORAGE mode, int level, GlInternal internalFormat, int width, int height, int border, GlFormat format, GlType type, void const * data )
	{
		try
		{
			return EXEC_FUNCTION( TexImage2D, uint32_t( mode ), level, uint32_t( internalFormat ), width, height, border, uint32_t( format ), uint32_t( type ), data );
		}
		catch ( ... )
		{
			return false;
		}
	}

	bool TexFunctions::TexImage2D( eGL_TEXTURE_STORAGE mode, int level, GlInternal internalFormat, Castor::Size const & p_size, int border, GlFormat format, GlType type, void const * data )
	{
		try
		{
			return EXEC_FUNCTION( TexImage2D, uint32_t( mode ), level, uint32_t( internalFormat ), p_size.width(), p_size.height(), border, uint32_t( format ), uint32_t( type ), data );
		}
		catch ( ... )
		{
			return false;
		}
	}

	bool TexFunctions::TexImage3D( eGL_TEXTURE_STORAGE mode, int level, GlInternal internalFormat, int width, int height, int depth, int border, GlFormat format, GlType type, void const * data )
	{
		try
		{
			return EXEC_FUNCTION( TexImage3D, uint32_t( mode ), level, uint32_t( internalFormat ), width, height, depth, border, uint32_t( format ), uint32_t( type ), data );
		}
		catch ( ... )
		{
			return false;
		}
	}

	bool TexFunctions::GetTexImage( eGL_TEXTURE_STORAGE mode, int level, GlFormat format, GlType type, void * img )
	{
		return EXEC_FUNCTION( GetTexImage, uint32_t( mode ), level, uint32_t( format ), uint32_t( type ), img );
	}

	//*************************************************************************************************

	bool TexFunctionsDSA::GenerateMipmap( GlTexDim mode )
	{
		if ( m_pfnGenerateTextureMipmap )
		{
			m_pfnGenerateTextureMipmap( m_uiTexture, uint32_t( mode ) );
		}

		return glCheckError( GetOpenGl(), cuT( "glGenerateTextureMipmap" ) );
	}

	bool TexFunctionsDSA::TexSubImage1D( eGL_TEXTURE_STORAGE mode, int level, int xoffset, int width, GlFormat format, GlType type, void const * data )
	{
		return EXEC_FUNCTION( TextureSubImage1D, m_uiTexture, uint32_t( mode ), level, xoffset, width, uint32_t( format ), uint32_t( type ), data );
	}

	bool TexFunctionsDSA::TexSubImage2D( eGL_TEXTURE_STORAGE mode, int level, int xoffset, int yoffset, int width, int height, GlFormat format, GlType type, void const * data )
	{
		return EXEC_FUNCTION( TextureSubImage2D, m_uiTexture, uint32_t( mode ), level, xoffset, yoffset, width, height, uint32_t( format ), uint32_t( type ), data );
	}

	bool TexFunctionsDSA::TexSubImage2D( eGL_TEXTURE_STORAGE mode, int level, Castor::Position const & p_position, Castor::Size const & p_size, GlFormat format, GlType type, void const * data )
	{
		return EXEC_FUNCTION( TextureSubImage2D, m_uiTexture, uint32_t( mode ), level, p_position.x(), p_position.y(), p_size.width(), p_size.height(), uint32_t( format ), uint32_t( type ), data );
	}

	bool TexFunctionsDSA::TexSubImage2D( eGL_TEXTURE_STORAGE mode, int level, Castor::Rectangle const & p_rect, GlFormat format, GlType type, void const * data )
	{
		return EXEC_FUNCTION( TextureSubImage2D, m_uiTexture, uint32_t( mode ), level, p_rect.left(), p_rect.top(), p_rect.width(), p_rect.height(), uint32_t( format ), uint32_t( type ), data );
	}

	bool TexFunctionsDSA::TexSubImage3D( eGL_TEXTURE_STORAGE mode, int level, int xoffset, int yoffset, int zoffset, int width, int height, int depth, GlFormat format, GlType type, void const * data )
	{
		return EXEC_FUNCTION( TextureSubImage3D, m_uiTexture, uint32_t( mode ), level, xoffset, yoffset, zoffset, width, height, depth, uint32_t( format ), uint32_t( type ), data );
	}

	bool TexFunctionsDSA::TexImage1D( eGL_TEXTURE_STORAGE mode, int level, GlInternal internalFormat, int width, int border, GlFormat format, GlType type, void const * data )
	{
		try
		{
			return EXEC_FUNCTION( TextureImage1D, m_uiTexture, uint32_t( mode ), level, uint32_t( internalFormat ), width, border, uint32_t( format ), uint32_t( type ), data );
		}
		catch ( ... )
		{
			return false;
		}
	}

	bool TexFunctionsDSA::TexImage2D( eGL_TEXTURE_STORAGE mode, int level, GlInternal internalFormat, int width, int height, int border, GlFormat format, GlType type, void const * data )
	{
		try
		{
			return EXEC_FUNCTION( TextureImage2D, m_uiTexture, uint32_t( mode ), level, uint32_t( internalFormat ), width, height, border, uint32_t( format ), uint32_t( type ), data );
		}
		catch ( ... )
		{
			return false;
		}
	}

	bool TexFunctionsDSA::TexImage2D( eGL_TEXTURE_STORAGE mode, int level, GlInternal internalFormat, Castor::Size const & p_size, int border, GlFormat format, GlType type, void const * data )
	{
		try
		{
			return EXEC_FUNCTION( TextureImage2D, m_uiTexture, uint32_t( mode ), level, uint32_t( internalFormat ), p_size.width(), p_size.height(), border, uint32_t( format ), uint32_t( type ), data );
		}
		catch ( ... )
		{
			return false;
		}
	}

	bool TexFunctionsDSA::TexImage3D( eGL_TEXTURE_STORAGE mode, int level, GlInternal internalFormat, int width, int height, int depth, int border, GlFormat format, GlType type, void const * data )
	{
		try
		{
			return EXEC_FUNCTION( TextureImage3D, m_uiTexture, uint32_t( mode ), level, uint32_t( internalFormat ), width, height, depth, border, uint32_t( format ), uint32_t( type ), data );
		}
		catch ( ... )
		{
			return false;
		}
	}

	bool TexFunctionsDSA::GetTexImage( eGL_TEXTURE_STORAGE mode, int level, GlFormat format, GlType type, void * img )
	{
		return EXEC_FUNCTION( GetTextureImage, m_uiTexture, uint32_t( mode ), level, uint32_t( format ), uint32_t( type ), img );
	}

	//*************************************************************************************************

	bool BufFunctionsBase::BufferAddressRange( eGL_ADDRESS pname, uint32_t index, uint64_t address, size_t length )const
	{
		return EXEC_FUNCTION( BufferAddressRange, uint32_t( pname ), index, address, length );
	}

	bool BufFunctionsBase::VertexFormat( int size, GlType type, int stride )const
	{
		return EXEC_FUNCTION( VertexFormat, size, uint32_t( type ), stride );
	}

	bool BufFunctionsBase::NormalFormat( GlType type, int stride )const
	{
		return EXEC_FUNCTION( NormalFormat, uint32_t( type ), stride );
	}

	bool BufFunctionsBase::ColorFormat( int size, GlType type, int stride )const
	{
		return EXEC_FUNCTION( ColorFormat, size, uint32_t( type ), stride );
	}

	bool BufFunctionsBase::IndexFormat( GlType type, int stride )const
	{
		return EXEC_FUNCTION( IndexFormat, uint32_t( type ), stride );
	}

	bool BufFunctionsBase::TexCoordFormat( int size, GlType type, int stride )const
	{
		return EXEC_FUNCTION( TexCoordFormat, size, uint32_t( type ), stride );
	}

	bool BufFunctionsBase::EdgeFlagFormat( int stride )const
	{
		return EXEC_FUNCTION( EdgeFlagFormat, stride );
	}

	bool BufFunctionsBase::SecondaryColorFormat( int size, GlType type, int stride )const
	{
		return EXEC_FUNCTION( SecondaryColorFormat, size, uint32_t( type ), stride );
	}

	bool BufFunctionsBase::FogCoordFormat( uint32_t type, int stride )const
	{
		return EXEC_FUNCTION( FogCoordFormat, uint32_t( type ), stride );
	}

	bool BufFunctionsBase::VertexAttribFormat( uint32_t index, int size, GlType type, bool normalized, int stride )const
	{
		return EXEC_FUNCTION( VertexAttribFormat, index, size, uint32_t( type ), normalized, stride );
	}

	bool BufFunctionsBase::VertexAttribIFormat( uint32_t index, int size, GlType type, int stride )const
	{
		return EXEC_FUNCTION( VertexAttribIFormat, index, size, uint32_t( type ), stride );
	}

	bool BufFunctionsBase::MakeBufferResident( GlBufferTarget target, eGL_RESIDENT_BUFFER_ACCESS access )const
	{
		return EXEC_FUNCTION( MakeBufferResident, uint32_t( target ), uint32_t( access ) );
	}

	bool BufFunctionsBase::MakeBufferNonResident( GlBufferTarget target )const
	{
		return EXEC_FUNCTION( MakeBufferNonResident, uint32_t( target ) );
	}

	bool BufFunctionsBase::IsBufferResident( GlBufferTarget target )const
	{
		return m_pfnIsBufferResident( uint32_t( target ) ) && glCheckError( GetOpenGl(), cuT( "glIsBufferResident" ) );
	}

	bool BufFunctionsBase::MakeNamedBufferResident( uint32_t buffer, eGL_RESIDENT_BUFFER_ACCESS access )const
	{
		return EXEC_FUNCTION( MakeNamedBufferResident, buffer, uint32_t( access ) );
	}

	bool BufFunctionsBase::MakeNamedBufferNonResident( uint32_t buffer )const
	{
		return EXEC_FUNCTION( MakeNamedBufferNonResident, buffer );
	}

	bool BufFunctionsBase::IsNamedBufferResident( uint32_t buffer )const
	{
		return m_pfnIsNamedBufferResident( buffer ) && glCheckError( GetOpenGl(), cuT( "glIsNamedBufferResident" ) );
	}

	bool BufFunctionsBase::GetBufferParameter( GlBufferTarget target, eGL_BUFFER_PARAMETER pname, uint64_t * params )const
	{
		return EXEC_FUNCTION( GetBufferParameterui64v, uint32_t( target ), uint32_t( pname ), params );
	}

	bool BufFunctionsBase::GetNamedBufferParameter( uint32_t buffer, eGL_BUFFER_PARAMETER pname, uint64_t * params )const
	{
		return EXEC_FUNCTION( GetNamedBufferParameterui64v, buffer, uint32_t( pname ), params );
	}

	//*************************************************************************************************

	bool BufFunctions::BindBuffer( GlBufferTarget p_target, uint32_t buffer )const
	{
		return EXEC_FUNCTION( BindBuffer, uint32_t( p_target ), buffer );
	}

	bool BufFunctions::BufferData( GlBufferTarget p_target, ptrdiff_t size, void const * data, GlBufferMode usage )const
	{
		return EXEC_FUNCTION( BufferData, uint32_t( p_target ), size, data, uint32_t( usage ) );
	}

	bool BufFunctions::CopyBufferSubData( GlBufferTarget readtarget, GlBufferTarget writetarget, ptrdiff_t readoffset, ptrdiff_t writeoffset, ptrdiff_t size )const
	{
		return EXEC_FUNCTION( CopyBufferSubData, uint32_t( readtarget ), uint32_t( writetarget ), readoffset, writeoffset, size );
	}

	bool BufFunctions::BufferSubData( GlBufferTarget p_target, ptrdiff_t offset, ptrdiff_t size, void const * data )const
	{
		return EXEC_FUNCTION( BufferSubData, uint32_t( p_target ), offset, size, data );
	}

	bool BufFunctions::GetBufferParameter( GlBufferTarget p_target, eGL_BUFFER_PARAMETER pname, int * params )const
	{
		return EXEC_FUNCTION( GetBufferParameteriv, uint32_t( p_target ), uint32_t( pname ), params );
	}

	void * BufFunctions::MapBuffer( GlBufferTarget p_target, GlAccessType access )const
	{
		void * l_return = m_pfnMapBuffer( uint32_t( p_target ), uint32_t( access ) );

		if ( !glCheckError( GetOpenGl(), cuT( "glMapBuffer" ) ) )
		{
			l_return = nullptr;
		}

		return l_return;
	}

	bool BufFunctions::UnmapBuffer( GlBufferTarget p_target )const
	{
		return EXEC_FUNCTION( UnmapBuffer, uint32_t( p_target ) );
	}

	void * BufFunctions::MapBufferRange( GlBufferTarget p_target, ptrdiff_t offset, ptrdiff_t length, uint32_t access )const
	{
		void * l_return = m_pfnMapBufferRange( uint32_t( p_target ), offset, length, access );

		if ( !glCheckError( GetOpenGl(), cuT( "glMapBufferRange" ) ) )
		{
			l_return = nullptr;
		}

		return l_return;
	}

	bool BufFunctions::FlushMappedBufferRange( GlBufferTarget p_target, ptrdiff_t offset, ptrdiff_t length )const
	{
		return EXEC_FUNCTION( FlushMappedBufferRange, uint32_t( p_target ), offset, length );
	}

	//*************************************************************************************************

	bool BufFunctionsDSA::BufferData( GlBufferTarget p_target, ptrdiff_t size, void const * data, GlBufferMode usage )const
	{
		return EXEC_FUNCTION( NamedBufferData, m_uiBuffer, size, data, uint32_t( usage ) );
	}

	bool BufFunctionsDSA::BufferSubData( GlBufferTarget p_target, ptrdiff_t offset, ptrdiff_t size, void const * data )const
	{
		return EXEC_FUNCTION( NamedBufferSubData, m_uiBuffer, offset, size, data );
	}

	bool BufFunctionsDSA::CopyBufferSubData( GlBufferTarget readtarget, GlBufferTarget writetarget, ptrdiff_t readoffset, ptrdiff_t writeoffset, ptrdiff_t size )const
	{
		return EXEC_FUNCTION( CopyNamedBufferSubData, uint32_t( readtarget ), uint32_t( writetarget ), readoffset, writeoffset, size );
	}

	bool BufFunctionsDSA::GetBufferParameter( GlBufferTarget p_target, eGL_BUFFER_PARAMETER pname, int * params )const
	{
		return EXEC_FUNCTION( GetNamedBufferParameteriv, m_uiBuffer, uint32_t( pname ), params );
	}

	void * BufFunctionsDSA::MapBuffer( GlBufferTarget p_target, GlAccessType access )const
	{
		void * l_return = m_pfnMapNamedBuffer( m_uiBuffer, uint32_t( access ) );

		if ( !glCheckError( GetOpenGl(), cuT( "glMapBuffer" ) ) )
		{
			l_return = nullptr;
		}

		return l_return;
	}

	bool BufFunctionsDSA::UnmapBuffer( GlBufferTarget p_target )const
	{
		return EXEC_FUNCTION( UnmapNamedBuffer, m_uiBuffer );
	}

	void * BufFunctionsDSA::MapBufferRange( GlBufferTarget p_target, ptrdiff_t offset, ptrdiff_t length, uint32_t access )const
	{
		void * l_return = m_pfnMapNamedBufferRange( m_uiBuffer, offset, length, access );

		if ( !glCheckError( GetOpenGl(), cuT( "glMapBufferRange" ) ) )
		{
			l_return = nullptr;
		}

		return l_return;
	}

	bool BufFunctionsDSA::FlushMappedBufferRange( GlBufferTarget p_target, ptrdiff_t offset, ptrdiff_t length )const
	{
		return EXEC_FUNCTION( FlushMappedNamedBufferRange, m_uiBuffer, offset, length );
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

	Castor3D::ElementType OpenGl::Get( eGLSL_ATTRIBUTE_TYPE p_type )const
	{
		switch ( p_type )
		{
		case eGLSL_ATTRIBUTE_TYPE_FLOAT:
			return Castor3D::ElementType::Float;
			break;

		case eGLSL_ATTRIBUTE_TYPE_FLOAT_VEC2:
			return Castor3D::ElementType::Vec2;
			break;

		case eGLSL_ATTRIBUTE_TYPE_FLOAT_VEC3:
			return Castor3D::ElementType::Vec3;
			break;

		case eGLSL_ATTRIBUTE_TYPE_FLOAT_VEC4:
			return Castor3D::ElementType::Vec4;
			break;

		case eGLSL_ATTRIBUTE_TYPE_INT:
			return Castor3D::ElementType::Int;
			break;

		case eGLSL_ATTRIBUTE_TYPE_INT_VEC2:
			return Castor3D::ElementType::IVec2;
			break;

		case eGLSL_ATTRIBUTE_TYPE_INT_VEC3:
			return Castor3D::ElementType::IVec3;
			break;

		case eGLSL_ATTRIBUTE_TYPE_INT_VEC4:
			return Castor3D::ElementType::IVec4;
			break;

		case eGLSL_ATTRIBUTE_TYPE_FLOAT_MAT2:
			return Castor3D::ElementType::Mat2;
			break;

		case eGLSL_ATTRIBUTE_TYPE_FLOAT_MAT3:
			return Castor3D::ElementType::Mat3;
			break;

		case eGLSL_ATTRIBUTE_TYPE_FLOAT_MAT4:
			return Castor3D::ElementType::Mat4;
			break;

		default:
			assert( false && "Unsupported attribute type." );
			break;
		}

		return Castor3D::ElementType::Count;
	}

	Castor::String const & OpenGl::GetGlslErrorString( int p_index )const
	{
		return GlslStrings[p_index];
	}

	inline GlTopology OpenGl::Get( Castor3D::Topology p_index )const
	{
		return PrimitiveTypes[uint32_t( p_index )];
	}

	inline GlTexDim OpenGl::Get( Castor3D::TextureType p_index )const
	{
		if ( p_index == Castor3D::TextureType::Buffer )
		{
			if ( !HasTbo() )
			{
				p_index = Castor3D::TextureType::OneDimension;
			}
		}

		return TextureDimensions[uint32_t( p_index )];
	}

	inline GlComparator OpenGl::Get( Castor3D::ComparisonFunc p_eAlphaFunc )const
	{
		return AlphaFuncs[uint32_t( p_eAlphaFunc )];
	}

	inline GlWrapMode OpenGl::Get( Castor3D::WrapMode p_eWrapMode )const
	{
		return TextureWrapMode[uint32_t( p_eWrapMode )];
	}

	inline GlInterpolationMode OpenGl::Get( Castor3D::InterpolationMode p_interpolation )const
	{
		return TextureInterpolation[uint32_t( p_interpolation )];
	}

	inline GlBlendSource OpenGl::Get( Castor3D::BlendSource p_eArgument )const
	{
		return TextureArguments[uint32_t( p_eArgument )];
	}

	inline GlBlendFunc OpenGl::Get( Castor3D::ColourBlendFunc p_mode )const
	{
		return RgbBlendFuncs[uint32_t( p_mode )];
	}

	inline GlBlendFunc OpenGl::Get( Castor3D::AlphaBlendFunc p_mode )const
	{
		return AlphaBlendFuncs[uint32_t( p_mode )];
	}

	inline GlBlendFactor OpenGl::Get( Castor3D::BlendOperand p_eBlendFactor )const
	{
		return BlendFactors[uint32_t( p_eBlendFactor )];
	}

	inline OpenGl::PixelFmt const & OpenGl::Get( Castor::PixelFormat p_pixelFormat )const
	{
		return PixelFormats[uint32_t( p_pixelFormat )];
	}

	inline GlShaderType OpenGl::Get( Castor3D::ShaderType p_type )const
	{
		return ShaderTypes[uint32_t( p_type )];
	}

	inline GlInternal OpenGl::GetInternal( Castor::PixelFormat p_format )const
	{
		return Internals[uint32_t( p_format )];
	}

	inline uint32_t OpenGl::GetComponents( uint32_t p_components )const
	{
		return ( Castor::CheckFlag( p_components, uint32_t( Castor3D::BufferComponent::Colour ) ) ? uint32_t( GlBufferBit::Colour ) : 0u )
			   | ( Castor::CheckFlag( p_components, uint32_t( Castor3D::BufferComponent::Depth ) ) ? uint32_t( GlBufferBit::Depth ) : 0u )
			   | ( Castor::CheckFlag( p_components, uint32_t( Castor3D::BufferComponent::Stencil ) ) ? uint32_t( GlBufferBit::Stencil ) : 0u );
	}

	inline GlAttachmentPoint OpenGl::Get( Castor3D::AttachmentPoint p_eAttachment )const
	{
		return Attachments[uint32_t( p_eAttachment )];
	}

	inline GlInternal OpenGl::GetRboStorage( Castor::PixelFormat p_pixelFormat )const
	{
		return RboStorages[uint32_t( p_pixelFormat )];
	}

	inline GlFrameBufferMode OpenGl::Get( Castor3D::FrameBufferTarget p_target )const
	{
		return FramebufferModes[uint32_t( p_target )];
	}

	inline GlBufferBinding OpenGl::Get( Castor3D::WindowBuffer p_buffer )const
	{
		return Buffers[uint32_t( p_buffer )];
	}

	inline GlBufferBinding OpenGl::Get( GlAttachmentPoint p_buffer )const
	{
		return BuffersTA.find( p_buffer )->second;
	}

	inline GlCulledFace OpenGl::Get( Castor3D::Culling p_eFace )const
	{
		return Faces[uint32_t( p_eFace )];
	}

	inline eGL_FILL_MODE OpenGl::Get( Castor3D::FillMode p_mode )const
	{
		return FillModes[uint32_t( p_mode )];
	}

	inline GlComparator OpenGl::Get( Castor3D::StencilFunc p_func )const
	{
		return StencilFuncs[uint32_t( p_func )];
	}

	inline eGL_STENCIL_OP OpenGl::Get( Castor3D::StencilOp p_eOp )const
	{
		return StencilOps[uint32_t( p_eOp )];
	}

	inline GlBlendOp OpenGl::Get( Castor3D::BlendOperation p_eOp )const
	{
		return BlendOps[uint32_t( p_eOp )];
	}

	inline GlComparator OpenGl::Get( Castor3D::DepthFunc p_func )const
	{
		return DepthFuncs[uint32_t( p_func )];
	}

	inline eGL_QUERY OpenGl::Get( Castor3D::QueryType p_value )const
	{
		return Queries[uint32_t( p_value )];
	}

	inline eGL_QUERY_INFO OpenGl::Get( Castor3D::QueryInfo p_value )const
	{
		return QueryInfos[uint32_t( p_value )];
	}

	inline eGL_TEXTURE_STORAGE OpenGl::Get( Castor3D::TextureStorageType p_value )const
	{
		return TextureStorages[uint32_t( p_value )];
	}

	inline GlTexDim OpenGl::Get( Castor3D::CubeMapFace p_value )const
	{
		return CubeMapFaces[uint32_t( p_value )];
	}

	inline eGL_COMPARE_MODE OpenGl::Get( Castor3D::ComparisonMode p_value )const
	{
		return ComparisonModes[uint32_t( p_value )];
	}

	inline bool OpenGl::Get( Castor3D::WritingMask p_eMask )const
	{
		return WriteMasks[uint32_t( p_eMask )];
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

	inline GlBufferMode OpenGl::GetBufferFlags( uint32_t p_flags )const
	{
		GlBufferMode l_eReturn = GlBufferMode( 0 );

		if ( Castor::CheckFlag( p_flags, Castor3D::BufferAccessType::Dynamic ) )
		{
			if ( Castor::CheckFlag( p_flags, Castor3D::BufferAccessNature::Read ) )
			{
				l_eReturn = GlBufferMode::DynamicRead;
			}
			else if ( Castor::CheckFlag( p_flags, Castor3D::BufferAccessNature::Draw ) )
			{
				l_eReturn = GlBufferMode::DynamicDraw;
			}
			else if ( Castor::CheckFlag( p_flags, Castor3D::BufferAccessNature::Copy ) )
			{
				l_eReturn = GlBufferMode::DynamicCopy;
			}
		}
		else if ( Castor::CheckFlag( p_flags, Castor3D::BufferAccessType::Static ) )
		{
			if ( Castor::CheckFlag( p_flags, Castor3D::BufferAccessNature::Read ) )
			{
				l_eReturn = GlBufferMode::StaticRead;
			}
			else if ( Castor::CheckFlag( p_flags, Castor3D::BufferAccessNature::Draw ) )
			{
				l_eReturn = GlBufferMode::StaticDraw;
			}
			else if ( Castor::CheckFlag( p_flags, Castor3D::BufferAccessNature::Copy ) )
			{
				l_eReturn = GlBufferMode::StaticCopy;
			}
		}
		else if ( Castor::CheckFlag( p_flags, Castor3D::BufferAccessType::Stream ) )
		{
			if ( Castor::CheckFlag( p_flags, Castor3D::BufferAccessNature::Read ) )
			{
				l_eReturn = GlBufferMode::StreamRead;
			}
			else if ( Castor::CheckFlag( p_flags, Castor3D::BufferAccessNature::Draw ) )
			{
				l_eReturn = GlBufferMode::StreamDraw;
			}
			else if ( Castor::CheckFlag( p_flags, Castor3D::BufferAccessNature::Copy ) )
			{
				l_eReturn = GlBufferMode::StreamCopy;
			}
		}

		return l_eReturn;
	}

	GlAccessType OpenGl::GetLockFlags( Castor3D::AccessType p_flags )const
	{
		GlAccessType l_eLockFlags = GlAccessType::ReadWrite;

		if ( Castor::CheckFlag( p_flags, Castor3D::AccessType::Read ) )
		{
			if ( Castor::CheckFlag( p_flags, Castor3D::AccessType::Write ) )
			{
				l_eLockFlags = GlAccessType::ReadWrite;
			}
			else
			{
				l_eLockFlags = GlAccessType::Read;
			}
		}
		else if ( Castor::CheckFlag( p_flags, Castor3D::AccessType::Write ) )
		{
			l_eLockFlags = GlAccessType::Write;
		}

		return l_eLockFlags;
	}

	uint32_t OpenGl::GetBitfieldFlags( Castor3D::AccessType p_flags )const
	{
		uint32_t l_uiFlags = 0;

		if ( Castor::CheckFlag( p_flags, Castor3D::AccessType::Read ) )
		{
			Castor::AddFlag( l_uiFlags, GlBufferMappingBit::Read );

			if ( Castor::CheckFlag( p_flags, Castor3D::AccessType::Write ) )
			{
				Castor::AddFlag( l_uiFlags, GlBufferMappingBit::Write );
			}
		}
		else if ( Castor::CheckFlag( p_flags, Castor3D::AccessType::Write ) )
		{
			Castor::AddFlag( l_uiFlags, GlBufferMappingBit::Write );
			Castor::AddFlag( l_uiFlags, GlBufferMappingBit::InvalidateRange );
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

	bool OpenGl::ClearDepth( double value )const
	{
		m_pfnClearDepth( value );
		return glCheckError( *this, "glClearDepth" );
	}

	bool OpenGl::Clear( uint32_t mask )const
	{
		m_pfnClear( mask );
		return glCheckError( *this, "glClear" );
	}

	bool OpenGl::DrawArrays( GlTopology mode, int first, int count )const
	{
		m_pfnDrawArrays( uint32_t( mode ), first, count );
		return glCheckError( *this, "glDrawArrays" );
	}

	bool OpenGl::DrawElements( GlTopology mode, int count, GlType type, void const * indices )const
	{
		m_pfnDrawElements( uint32_t( mode ), count, uint32_t( type ), indices );
		return glCheckError( *this, "glDrawElements" );
	}

	bool OpenGl::Enable( GlTweak mode )const
	{
		m_pfnEnable( uint32_t( mode ) );
		return glCheckError( *this, "glEnable" );
	}

	bool OpenGl::Disable( GlTweak mode )const
	{
		m_pfnDisable( uint32_t( mode ) );
		return glCheckError( *this, "glDisable" );
	}

	bool OpenGl::Enable( GlTexDim texture )const
	{
		m_pfnEnable( uint32_t( texture ) );
		return glCheckError( *this, "glEnable" );
	}

	bool OpenGl::Disable( GlTexDim texture )const
	{
		m_pfnDisable( uint32_t( texture ) );
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

	bool OpenGl::GetIntegerv( GlMax pname, int * params )const
	{
		return GetIntegerv( uint32_t( pname ), params );
	}

	bool OpenGl::GetIntegerv( GlMin pname, int * params )const
	{
		return GetIntegerv( uint32_t( pname ), params );
	}

	bool OpenGl::GetIntegerv( GlGpuInfo pname, int * params )const
	{
		return GetIntegerv( uint32_t( pname ), params );
	}

	bool OpenGl::GetIntegerv( uint32_t pname, uint64_t * params )const
	{
		m_pfnGetIntegerui64v( pname, params );
		return glCheckError( *this, "glGetIntegerui64v" );
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

	bool OpenGl::CullFace( GlCulledFace face )const
	{
		m_pfnCullFace( uint32_t( face ) );
		return glCheckError( *this, "glCullFace" );
	}

	bool OpenGl::FrontFace( GlFrontFaceDirection face )const
	{
		m_pfnFrontFace( uint32_t( face ) );
		return glCheckError( *this, "glFrontFace" );
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

	bool OpenGl::BindTexture( GlTexDim mode, uint32_t texture )const
	{
		return m_pTexFunctions->BindTexture( mode, texture );
	}

	bool OpenGl::GenerateMipmap( GlTexDim mode )const
	{
		return m_pTexFunctions->GenerateMipmap( mode );
	}

	bool OpenGl::ActiveTexture( GlTextureIndex target )const
	{
		m_pfnActiveTexture( uint32_t( target ) );
		return glCheckError( *this, "glActiveTexture" );
	}

	bool OpenGl::ClientActiveTexture( GlTextureIndex target )const
	{
		m_pfnClientActiveTexture( uint32_t( target ) );
		return glCheckError( *this, "glClientActiveTexture" );
	}

	bool OpenGl::TexSubImage1D( eGL_TEXTURE_STORAGE mode, int level, int xoffset, int width, GlFormat format, GlType type, void const * data )const
	{
		return m_pTexFunctions->TexSubImage1D( mode, level, xoffset, width, format, type, data );
	}

	bool OpenGl::TexSubImage2D( eGL_TEXTURE_STORAGE mode, int level, int xoffset, int yoffset, int width, int height, GlFormat format, GlType type, void const * data )const
	{
		return m_pTexFunctions->TexSubImage2D( mode, level, xoffset, yoffset, width, height, format, type, data );
	}

	bool OpenGl::TexSubImage2D( eGL_TEXTURE_STORAGE mode, int level, Castor::Position const & p_position, Castor::Size const & p_size, GlFormat format, GlType type, void const * data )const
	{
		return m_pTexFunctions->TexSubImage2D( mode, level, p_position.x(), p_position.y(), p_size.width(), p_size.height(), format, type, data );
	}

	bool OpenGl::TexSubImage2D( eGL_TEXTURE_STORAGE mode, int level, Castor::Rectangle const & p_rect, GlFormat format, GlType type, void const * data )const
	{
		return m_pTexFunctions->TexSubImage2D( mode, level, p_rect.left(), p_rect.top(), p_rect.width(), p_rect.height(), format, type, data );
	}

	bool OpenGl::TexSubImage3D( eGL_TEXTURE_STORAGE mode, int level, int xoffset, int yoffset, int zoffset, int width, int height, int depth, GlFormat format, GlType type, void const * data )const
	{
		return m_pTexFunctions->TexSubImage3D( mode, level, xoffset, yoffset, zoffset, width, height, depth, format, type, data );
	}

	bool OpenGl::TexImage1D( eGL_TEXTURE_STORAGE mode, int level, GlInternal internalFormat, int width, int border, GlFormat format, GlType type, void const * data )const
	{
		return m_pTexFunctions->TexImage1D( mode, level, internalFormat, width, border, format, type, data );
	}

	bool OpenGl::TexImage2D( eGL_TEXTURE_STORAGE mode, int level, GlInternal internalFormat, int width, int height, int border, GlFormat format, GlType type, void const * data )const
	{
		return m_pTexFunctions->TexImage2D( mode, level, internalFormat, width, height, border, format, type, data );
	}

	bool OpenGl::TexImage2D( eGL_TEXTURE_STORAGE mode, int level, GlInternal internalFormat, Castor::Size const & p_size, int border, GlFormat format, GlType type, void const * data )const
	{
		return m_pTexFunctions->TexImage2D( mode, level, internalFormat, p_size.width(), p_size.height(), border, format, type, data );
	}

	bool OpenGl::TexImage3D( eGL_TEXTURE_STORAGE mode, int level, GlInternal internalFormat, int width, int height, int depth, int border, GlFormat format, GlType type, void const * data )const
	{
		return m_pTexFunctions->TexImage3D( mode, level, internalFormat, width, height, depth, border, format, type, data );
	}

	bool OpenGl::GetTexImage( eGL_TEXTURE_STORAGE mode, int level, GlFormat format, GlType type, void * img )const
	{
		return m_pTexFunctions->GetTexImage( mode, level, format, type, img );
	}

	bool OpenGl::BlendFunc( GlBlendFactor sfactor, GlBlendFactor dfactor )const
	{
		m_pfnBlendFunc( uint32_t( sfactor ), uint32_t( dfactor ) );
		return glCheckError( *this, "glBlendFunc" );
	}

	bool OpenGl::BlendFunc( GlBlendFactor p_eRgbSrc, GlBlendFactor p_eRgbDst, GlBlendFactor p_eAlphaSrc, GlBlendFactor p_eAlphaDst )const
	{
		if ( m_pfnBlendFuncSeparate )
		{
			m_pfnBlendFuncSeparate( uint32_t( p_eRgbSrc ), uint32_t( p_eRgbDst ), uint32_t( p_eAlphaSrc ), uint32_t( p_eAlphaDst ) );
			return glCheckError( *this, "glBlendFuncSeparate" );
		}
		else
		{
			return BlendFunc( p_eAlphaSrc, p_eAlphaDst );
		}
	}

	bool OpenGl::BlendFunc( uint32_t p_uiBuffer, GlBlendFactor p_eRgbSrc, GlBlendFactor p_eRgbDst, GlBlendFactor p_eAlphaSrc, GlBlendFactor p_eAlphaDst )const
	{
		if ( m_pfnBlendFuncSeparatei )
		{
			m_pfnBlendFuncSeparatei( p_uiBuffer, uint32_t( p_eRgbSrc ), uint32_t( p_eRgbDst ), uint32_t( p_eAlphaSrc ), uint32_t( p_eAlphaDst ) );
			return glCheckError( *this, "glBlendFuncSeparatei" );
		}
		else
		{
			return BlendFunc( p_eRgbSrc, p_eRgbDst, p_eAlphaSrc, p_eAlphaDst );
		}
	}

	bool OpenGl::BlendEquation( GlBlendOp p_eOp )const
	{
		if ( m_pfnBlendEquation )
		{
			m_pfnBlendEquation( uint32_t( p_eOp ) );
		}

		return glCheckError( *this, "glBlendEquation" );
	}

	bool OpenGl::BlendEquation( uint32_t p_uiBuffer, GlBlendOp p_eOp )const
	{
		if ( m_pfnBlendEquationi )
		{
			m_pfnBlendEquationi( p_uiBuffer, uint32_t( p_eOp ) );
			return glCheckError( *this, "glBlendEquationi" );
		}
		else
		{
			return BlendEquation( p_eOp );
		}
	}

	bool OpenGl::ComparisonFunc( GlComparator func, float ref )const
	{
		m_pfnAlphaFunc( uint32_t( func ), ref );
		return glCheckError( *this, "glAlphaFunc" );
	}

	bool OpenGl::DepthFunc( GlComparator p_func )const
	{
		m_pfnDepthFunc( uint32_t( p_func ) );
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

	bool OpenGl::PolygonMode( GlCulledFace p_eFacing, eGL_FILL_MODE p_mode )const
	{
		m_pfnPolygonMode( p_eFacing, uint32_t( p_mode ) );
		return glCheckError( *this, "glPolygonMode" );
	}

	bool OpenGl::StencilOp( eGL_STENCIL_OP p_eStencilFail, eGL_STENCIL_OP p_eDepthFail, eGL_STENCIL_OP p_eStencilPass )const
	{
		m_pfnStencilOp( uint32_t( p_eStencilFail ), uint32_t( p_eDepthFail ), uint32_t( p_eStencilPass ) );
		return glCheckError( *this, "glStencilOp" );
	}

	bool OpenGl::StencilFunc( GlComparator p_func, int p_iRef, uint32_t p_uiMask )const
	{
		m_pfnStencilFunc( uint32_t( p_func ), p_iRef, p_uiMask );
		return glCheckError( *this, "glStencilFunc" );
	}

	bool OpenGl::StencilMask( uint32_t p_uiMask )const
	{
		m_pfnStencilMask( p_uiMask );
		return glCheckError( *this, "glStencilMask" );
	}

	bool OpenGl::StencilOpSeparate( GlCulledFace p_eFacing, eGL_STENCIL_OP p_eStencilFail, eGL_STENCIL_OP p_eDepthFail, eGL_STENCIL_OP p_eStencilPass )const
	{
		if ( m_pfnStencilOpSeparate )
		{
			m_pfnStencilOpSeparate( uint32_t( p_eFacing ), uint32_t( p_eStencilFail ), uint32_t( p_eDepthFail ), uint32_t( p_eStencilPass ) );
			return glCheckError( *this, "glStencilOpSeparate" );
		}
		else
		{
			return StencilOp( p_eStencilFail, p_eDepthFail, p_eStencilPass );
		}
	}

	bool OpenGl::StencilFuncSeparate( GlCulledFace p_eFacing, GlComparator p_func, int p_iRef, uint32_t p_uiMask )const
	{
		if ( m_pfnStencilFuncSeparate )
		{
			m_pfnStencilFuncSeparate( uint32_t( p_eFacing ), uint32_t( p_func ), p_iRef, p_uiMask );
			return glCheckError( *this, "glStencilFuncSeparate" );
		}
		else
		{
			return StencilFunc( p_func, p_iRef, p_uiMask );
		}
	}

	bool OpenGl::StencilMaskSeparate( GlCulledFace p_eFacing, uint32_t p_uiMask )const
	{
		if ( m_pfnStencilMaskSeparate )
		{
			m_pfnStencilMaskSeparate( uint32_t( p_eFacing ), p_uiMask );
			return glCheckError( *this, "glStencilMaskSeparate" );
		}
		else
		{
			return StencilMask( p_uiMask );
		}
	}

	bool OpenGl::Hint( eGL_HINT p_eHint, eGL_HINT_VALUE p_eValue )const
	{
		m_pfnHint( uint32_t( p_eHint ), uint32_t( p_eValue ) );
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

	bool OpenGl::DrawArraysInstanced( GlTopology mode, int first, int count, int primcount )const
	{
		if ( m_bHasInstancedDraw )
		{
			m_pfnDrawArraysInstanced( uint32_t( mode ), first, count, primcount );
		}

		return glCheckError( *this, "glDrawArraysInstanced" );
	}

	bool OpenGl::DrawElementsInstanced( GlTopology mode, int count, GlType type, const void * indices, int primcount )const
	{
		if ( m_bHasInstancedDraw )
		{
			m_pfnDrawElementsInstanced( uint32_t( mode ), count, uint32_t( type ), indices, primcount );
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

	bool OpenGl::ReadBuffer( GlBufferBinding p_buffer )const
	{
		m_pfnReadBuffer( uint32_t( p_buffer ) );
		return glCheckError( *this, "glReadBuffer" );
	}

	bool OpenGl::ReadPixels( int x, int y, int width, int height, GlFormat format, GlType type, void * pixels )const
	{
		m_pfnReadPixels( x, y, width, height, uint32_t( format ), uint32_t( type ), pixels );
		return glCheckError( *this, "glReadPixels" );
	}

	bool OpenGl::ReadPixels( Castor::Position const & p_position, Castor::Size const & p_size, GlFormat format, GlType type, void * pixels )const
	{
		m_pfnReadPixels( p_position.x(), p_position.y(), p_size.width(), p_size.height(), uint32_t( format ), uint32_t( type ), pixels );
		return glCheckError( *this, "glReadPixels" );
	}

	bool OpenGl::ReadPixels( Castor::Rectangle const & p_rect, GlFormat format, GlType type, void * pixels )const
	{
		m_pfnReadPixels( p_rect.left(), p_rect.top(), p_rect.width(), p_rect.height(), uint32_t( format ), uint32_t( type ), pixels );
		return glCheckError( *this, "glReadPixels" );
	}

	bool OpenGl::DrawBuffer( GlBufferBinding p_buffer )const
	{
		m_pfnDrawBuffer( uint32_t( p_buffer ) );
		return glCheckError( *this, "glDrawBuffer" );
	}

	bool OpenGl::DrawPixels( int width, int height, GlFormat format, GlType type, void const * data )const
	{
		m_pfnDrawPixels( width, height, uint32_t( format ), uint32_t( type ), data );
		return glCheckError( *this, "glDrawPixels" );
	}

	bool OpenGl::PixelStore( eGL_STORAGE_MODE p_mode, int p_iParam )const
	{
		m_pfnPixelStorei( uint32_t( p_mode ), p_iParam );
		return glCheckError( *this, "glPixelStorei" );
	}

	bool OpenGl::PixelStore( eGL_STORAGE_MODE p_mode, float p_fParam )const
	{
		m_pfnPixelStoref( uint32_t( p_mode ), p_fParam );
		return glCheckError( *this, "glPixelStoref" );
	}

	bool OpenGl::TexStorage1D( GLenum target, GLint levels, GlInternal internalformat, GLsizei width )const
	{
		m_pfnTexStorage1D( target, levels, uint32_t( internalformat ), width );
		return glCheckError( *this, "glTexStorage1D" );
	}

	bool OpenGl::TexStorage2D( GLenum target, GLint levels, GlInternal internalformat, GLsizei width, GLsizei height )const
	{
		m_pfnTexStorage2D( target, levels, uint32_t( internalformat ), width, height );
		return glCheckError( *this, "glTexStorage2D" );
	}

	bool OpenGl::TexStorage3D( GLenum target, GLint levels, GlInternal internalformat, GLsizei width, GLsizei height, GLsizei depth )const
	{
		m_pfnTexStorage3D( target, levels, uint32_t( internalformat ), width, height, depth );
		return glCheckError( *this, "glTexStorage3D" );
	}

	bool OpenGl::TexStorage2DMultisample( GLenum target, GLsizei samples, GlInternal internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations )const
	{
		m_pfnTexStorage2DMultisample( target, samples, uint32_t( internalformat ), width, height, fixedsamplelocations );
		return glCheckError( *this, "glTexStorage2DMultisample" );
	}

	bool OpenGl::TexStorage3DMultisample( GLenum target, GLsizei samples, GlInternal internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations )const
	{
		m_pfnTexStorage3DMultisample( target, samples, uint32_t( internalformat ), width, height, depth, fixedsamplelocations );
		return glCheckError( *this, "glTexStorage3DMultisample" );
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
		m_pfnGetSamplerParameteruiv( sampler, uint32_t( pname ), params );
		return glCheckError( *this, "glGetSamplerParameterIuiv" );
	}

	bool OpenGl::GetSamplerParameter( uint32_t sampler, eGL_SAMPLER_PARAMETER pname, float * params )const
	{
		m_pfnGetSamplerParameterfv( sampler, uint32_t( pname ), params );
		return glCheckError( *this, "glGetSamplerParameterfv" );
	}

	bool OpenGl::GetSamplerParameter( uint32_t sampler, eGL_SAMPLER_PARAMETER pname, int * params )const
	{
		m_pfnGetSamplerParameteriv( sampler, uint32_t( pname ), params );
		return glCheckError( *this, "glGetSamplerParameteriv" );
	}

	bool OpenGl::SetSamplerParameter( uint32_t sampler, eGL_SAMPLER_PARAMETER pname, const uint32_t * params )const
	{
		if ( pname != eGL_SAMPLER_PARAMETER_MAXANISOTROPY || m_bHasAnisotropic )
		{
			m_pfnSamplerParameteruiv( sampler, uint32_t( pname ), params );
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
			m_pfnSamplerParameterf( sampler, uint32_t( pname ), param );
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
			m_pfnSamplerParameterfv( sampler, uint32_t( pname ), params );
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
			m_pfnSamplerParameteri( sampler, uint32_t( pname ), param );
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
			m_pfnSamplerParameteriv( sampler, uint32_t( pname ), params );
			return glCheckError( *this, "glSamplerParameteriv" );
		}
		else
		{
			return true;
		}
	}

	bool OpenGl::TexBuffer( GlTexDim p_target, GlInternal p_internalFormat, uint32_t buffer )const
	{
		m_pfnTexBuffer( uint32_t( p_target ), uint32_t( p_internalFormat ), buffer );
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

	bool OpenGl::BindBuffer( GlBufferTarget p_target, uint32_t buffer )const
	{
		return m_pBufFunctions->BindBuffer( p_target, buffer );
	}

	bool OpenGl::BufferData( GlBufferTarget p_target, ptrdiff_t size, void const * data, GlBufferMode usage )const
	{
		return m_pBufFunctions->BufferData( p_target, size, data, usage );
	}

	bool OpenGl::BufferSubData( GlBufferTarget p_target, ptrdiff_t offset, ptrdiff_t size, void const * data )const
	{
		return m_pBufFunctions->BufferSubData( p_target, offset, size, data );
	}

	bool OpenGl::CopyBufferSubData( GlBufferTarget readtarget, GlBufferTarget writetarget, ptrdiff_t readoffset, ptrdiff_t writeoffset, ptrdiff_t size )const
	{
		return m_pBufFunctions->CopyBufferSubData( readtarget, writetarget, readoffset, writeoffset, size );
	}

	bool OpenGl::GetBufferParameter( GlBufferTarget p_target, eGL_BUFFER_PARAMETER pname, int * params )const
	{
		return m_pBufFunctions->GetBufferParameter( p_target, pname, params );
	}

	bool OpenGl::GetBufferParameter( GlBufferTarget p_target, eGL_BUFFER_PARAMETER pname, uint64_t * params )const
	{
		return m_pBufFunctions->GetBufferParameter( p_target, pname, params );
	}

	void * OpenGl::MapBuffer( GlBufferTarget p_target, GlAccessType access )const
	{
		return m_pBufFunctions->MapBuffer( p_target, access );
	}

	bool OpenGl::UnmapBuffer( GlBufferTarget p_target )const
	{
		return m_pBufFunctions->UnmapBuffer( p_target );
	}

	void * OpenGl::MapBufferRange( GlBufferTarget p_target, ptrdiff_t offset, ptrdiff_t length, uint32_t access )const
	{
		return m_pBufFunctions->MapBufferRange( p_target, offset, length, access );
	}

	bool OpenGl::FlushMappedBufferRange( GlBufferTarget p_target, ptrdiff_t offset, ptrdiff_t length )const
	{
		return m_pBufFunctions->FlushMappedBufferRange( p_target, offset, length );
	}

	bool OpenGl::BufferAddressRange( eGL_ADDRESS pname, uint32_t index, uint64_t address, size_t length )const
	{
		return m_pBufFunctions->BufferAddressRange( pname, index, address, length );
	}

	bool OpenGl::VertexFormat( int size, GlType type, int stride )const
	{
		return m_pBufFunctions->VertexFormat( size, type, stride );
	}

	bool OpenGl::NormalFormat( GlType type, int stride )const
	{
		return m_pBufFunctions->NormalFormat( type, stride );
	}

	bool OpenGl::ColorFormat( int size, GlType type, int stride )const
	{
		return m_pBufFunctions->ColorFormat( size, type, stride );
	}

	bool OpenGl::IndexFormat( GlType type, int stride )const
	{
		return m_pBufFunctions->IndexFormat( type, stride );
	}

	bool OpenGl::TexCoordFormat( int size, GlType type, int stride )const
	{
		return m_pBufFunctions->TexCoordFormat( size, type, stride );
	}

	bool OpenGl::EdgeFlagFormat( int stride )const
	{
		return m_pBufFunctions->EdgeFlagFormat( stride );
	}

	bool OpenGl::SecondaryColorFormat( int size, GlType type, int stride )const
	{
		return m_pBufFunctions->SecondaryColorFormat( size, type, stride );
	}

	bool OpenGl::FogCoordFormat( uint32_t type, int stride )const
	{
		return m_pBufFunctions->FogCoordFormat( type, stride );
	}

	bool OpenGl::VertexAttribFormat( uint32_t index, int size, GlType type, bool normalized, int stride )const
	{
		return m_pBufFunctions->VertexAttribFormat( index, size, type, normalized, stride );
	}

	bool OpenGl::VertexAttribIFormat( uint32_t index, int size, GlType type, int stride )const
	{
		return m_pBufFunctions->VertexAttribIFormat( index, size, type, stride );
	}

	bool OpenGl::MakeBufferResident( GlBufferTarget target, eGL_RESIDENT_BUFFER_ACCESS access )const
	{
		return m_pBufFunctions->MakeBufferResident( target, access );
	}

	bool OpenGl::MakeBufferNonResident( GlBufferTarget target )const
	{
		return m_pBufFunctions->MakeBufferNonResident( target );
	}

	bool OpenGl::IsBufferResident( GlBufferTarget target )const
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

	bool OpenGl::GenTransformFeedbacks( int n, uint32_t * buffers )const
	{
		m_pfnGenTransformFeedbacks( n, buffers );
		return glCheckError( *this, "glGenTransformFeedbacks" );
	}

	bool OpenGl::DeleteTransformFeedbacks( int n, uint32_t const * buffers )const
	{
		m_pfnDeleteTransformFeedbacks( n, buffers );
		return glCheckError( *this, "glDeleteTransformFeedbacks" );
	}

	bool OpenGl::BindTransformFeedback( GlBufferTarget target, uint32_t buffer )const
	{
		m_pfnBindTransformFeedback( target, buffer );
		return glCheckError( *this, "glBindTransformFeedback" );
	}

	bool OpenGl::IsTransformFeedback( uint32_t buffer )const
	{
		m_pfnIsTransformFeedback( buffer );
		return glCheckError( *this, "glIsTransformFeedback" );
	}

	bool OpenGl::BeginTransformFeedback( GlTopology primitive )const
	{
		m_pfnBeginTransformFeedback( uint32_t( primitive ) );
		return glCheckError( *this, "glBeginTransformFeedback" );
	}

	bool OpenGl::PauseTransformFeedback()const
	{
		m_pfnPauseTransformFeedback();
		return glCheckError( *this, "glPauseTransformFeedback" );
	}

	bool OpenGl::ResumeTransformFeedback()const
	{
		m_pfnResumeTransformFeedback();
		return glCheckError( *this, "glResumeTransformFeedback" );
	}

	bool OpenGl::EndTransformFeedback()const
	{
		m_pfnEndTransformFeedback();
		return glCheckError( *this, "glEndTransformFeedback" );
	}

	bool OpenGl::TransformFeedbackVaryings( uint32_t program, int count, char const ** varyings, eGL_ATTRIBS_LAYOUT bufferMode )const
	{
		m_pfnTransformFeedbackVaryings( program, count, varyings, bufferMode );
		return glCheckError( *this, "glTransformFeedbackVaryings" );
	}

	bool OpenGl::DrawTransformFeedback( GlTopology mode, uint32_t id )const
	{
		m_pfnDrawTransformFeedback( uint32_t( mode ), id );
		return glCheckError( *this, "glDrawTransformFeedback" );
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

	bool OpenGl::BindFramebuffer( GlFrameBufferMode p_eBindingMode, uint32_t framebuffer )const
	{
		m_pfnBindFramebuffer( uint32_t( p_eBindingMode ), framebuffer );
		return glCheckError( *this, "glBindFramebuffer" );
	}

	bool OpenGl::FramebufferTexture( GlFrameBufferMode p_target, GlAttachmentPoint p_eAttachment, uint32_t texture, int level )const
	{
		m_pfnFramebufferTexture( uint32_t( p_target ), uint32_t( p_eAttachment ), texture, level );
		return glCheckError( *this, "glFramebufferTexture" );
	}

	bool OpenGl::FramebufferTexture1D( GlFrameBufferMode p_target, GlAttachmentPoint p_eAttachment, GlTexDim p_texTarget, uint32_t texture, int level )const
	{
		m_pfnFramebufferTexture1D( uint32_t( p_target ), uint32_t( p_eAttachment ), uint32_t( p_texTarget ), texture, level );
		return glCheckError( *this, "glFramebufferTexture1D" );
	}

	bool OpenGl::FramebufferTexture2D( GlFrameBufferMode p_target, GlAttachmentPoint p_eAttachment, GlTexDim p_texTarget, uint32_t texture, int level )const
	{
		m_pfnFramebufferTexture2D( uint32_t( p_target ), uint32_t( p_eAttachment ), uint32_t( p_texTarget ), texture, level );
		return glCheckError( *this, "glFramebufferTexture2D" );
	}

	bool OpenGl::FramebufferTexture3D( GlFrameBufferMode p_target, GlAttachmentPoint p_eAttachment, GlTexDim p_texTarget, uint32_t texture, int level, int layer )const
	{
		m_pfnFramebufferTexture3D( uint32_t( p_target ), uint32_t( p_eAttachment ), uint32_t( p_texTarget ), texture, level, layer );
		return glCheckError( *this, "glFramebufferTexture3D" );
	}

	bool OpenGl::FramebufferTextureLayer( GlFrameBufferMode p_target, GlAttachmentPoint p_eAttachment, uint32_t texture, int level, int layer )const
	{
		m_pfnFramebufferTextureLayer( uint32_t( p_target ), uint32_t( p_eAttachment ), texture, level, layer );
		return glCheckError( *this, "glFramebufferTextureLayer" );
	}

	bool OpenGl::FramebufferRenderbuffer( GlFrameBufferMode p_target, GlAttachmentPoint p_eAttachment, GlRenderBufferMode p_eRenderbufferTarget, uint32_t renderbufferId )const
	{
		m_pfnFramebufferRenderbuffer( uint32_t( p_target ), uint32_t( p_eAttachment ), uint32_t( p_eRenderbufferTarget ), renderbufferId );
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

	bool OpenGl::BindRenderbuffer( GlRenderBufferMode p_eBindingMode, uint32_t id )const
	{
		m_pfnBindRenderbuffer( uint32_t( p_eBindingMode ), id );
		return glCheckError( *this, "glBindRenderbuffer" );
	}

	bool OpenGl::RenderbufferStorage( GlRenderBufferMode p_eBindingMode, GlInternal p_format, int width, int height )const
	{
		m_pfnRenderbufferStorage( uint32_t( p_eBindingMode ), uint32_t( p_format ), width, height );
		return glCheckError( *this, "glRenderbufferStorage" );
	}

	bool OpenGl::RenderbufferStorageMultisample( GlRenderBufferMode p_eBindingMode, int p_iSamples, GlInternal p_format, int width, int height )const
	{
		bool l_return = true;
		int l_iMaxSamples;
		int l_iMaxSize;
		OpenGl::GetIntegerv( GlMax::Samples, &l_iMaxSamples );
		OpenGl::GetIntegerv( GlMax::RenderbufferSize, &l_iMaxSize );

		if ( p_iSamples <= l_iMaxSamples && width <= l_iMaxSize && height < l_iMaxSize )
		{
			m_pfnRenderbufferStorageMultisample( uint32_t( p_eBindingMode ), p_iSamples, uint32_t( p_format ), width, height );
			l_return = glCheckError( *this, "glRenderbufferStorageMultisample" );
		}
		else if ( p_iSamples > l_iMaxSamples )
		{
			Castor::Logger::LogWarning( cuT( "glRenderbufferStorageMultisample - Asked for more samples than available, setting it to max available" ) );
			m_pfnRenderbufferStorageMultisample( uint32_t( p_eBindingMode ), l_iMaxSamples, uint32_t( p_format ), width, height );
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

	bool OpenGl::RenderbufferStorage( GlRenderBufferMode p_eBindingMode, GlInternal p_format, Castor::Size const & size )const
	{
		m_pfnRenderbufferStorage( uint32_t( p_eBindingMode ), uint32_t( p_format ), size.width(), size.height() );
		return glCheckError( *this, "glRenderbufferStorage" );
	}

	bool OpenGl::RenderbufferStorageMultisample( GlRenderBufferMode p_eBindingMode, int p_iSamples, GlInternal p_format, Castor::Size const & size )const
	{
		bool l_return = true;
		int l_iMaxSamples;
		int l_iMaxSize;
		OpenGl::GetIntegerv( GlMax::Samples, &l_iMaxSamples );
		OpenGl::GetIntegerv( GlMax::RenderbufferSize, &l_iMaxSize );

		if ( p_iSamples <= l_iMaxSamples && int( size.width() ) <= l_iMaxSize && int( size.height() ) < l_iMaxSize )
		{
			m_pfnRenderbufferStorageMultisample( uint32_t( p_eBindingMode ), p_iSamples, uint32_t( p_format ), size.width(), size.height() );
			l_return = glCheckError( *this, "glRenderbufferStorageMultisample" );
		}
		else if ( p_iSamples > l_iMaxSamples )
		{
			Castor::Logger::LogWarning( cuT( "glRenderbufferStorageMultisample - Asked for more samples than available, setting it to max available" ) );
			m_pfnRenderbufferStorageMultisample( uint32_t( p_eBindingMode ), l_iMaxSamples, uint32_t( p_format ), size.width(), size.height() );
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

	bool OpenGl::GetRenderbufferParameteriv( GlRenderBufferMode p_eBindingMode, GlRenderBufferParameter p_param, int * values )const
	{
		m_pfnGetRenderbufferParameteriv( uint32_t( p_eBindingMode ), uint32_t( p_param ), values );
		return glCheckError( *this, "glGetRenderbufferParameteriv" );
	}

	bool OpenGl::BlitFramebuffer( int srcX0, int srcY0, int srcX1, int srcY1, int dstX0, int dstY0, int dstX1, int dstY1, uint32_t mask, GlInterpolationMode filter )const
	{
		m_pfnBlitFramebuffer( srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, uint32_t( filter ) );
		return glCheckError( *this, "glBlitFramebuffer" );
	}

	bool OpenGl::BlitFramebuffer( Castor::Rectangle const & rcSrc, Castor::Rectangle const & rcDst, uint32_t mask, GlInterpolationMode filter )const
	{
		m_pfnBlitFramebuffer( rcSrc.left(), rcSrc.top(), rcSrc.right(), rcSrc.bottom(), rcDst.left(), rcDst.top(), rcDst.right(), rcDst.bottom(), mask, uint32_t( filter ) );
		return glCheckError( *this, "glBlitFramebuffer" );
	}

	bool OpenGl::BlitFramebuffer( int srcX0, int srcY0, int srcX1, int srcY1, int dstX0, int dstY0, int dstX1, int dstY1, GlComponent mask, GlInterpolationMode filter )const
	{
		m_pfnBlitFramebuffer( srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, uint32_t( mask ), uint32_t( filter ) );
		return glCheckError( *this, "glBlitFramebuffer" );
	}

	bool OpenGl::BlitFramebuffer( Castor::Rectangle const & rcSrc, Castor::Rectangle const & rcDst, GlComponent mask, GlInterpolationMode filter )const
	{
		m_pfnBlitFramebuffer( rcSrc.left(), rcSrc.top(), rcSrc.right(), rcSrc.bottom(), rcDst.left(), rcDst.top(), rcDst.right(), rcDst.bottom(), uint32_t( mask ), uint32_t( filter ) );
		return glCheckError( *this, "glBlitFramebuffer" );
	}

	bool OpenGl::DrawBuffers( int n, const uint32_t * bufs )const
	{
		m_pfnDrawBuffers( n, bufs );
		return glCheckError( *this, "glDrawBuffers" );
	}

	GlFramebufferStatus OpenGl::CheckFramebufferStatus( GlFrameBufferMode p_target )const
	{
		GlFramebufferStatus l_return = GlFramebufferStatus( m_pfnCheckFramebufferStatus( uint32_t( p_target ) ) );

		if ( !glCheckError( *this, "glCheckFramebufferStatus" ) )
		{
			l_return = GlFramebufferStatus::Invalid;
		}

		return l_return;
	}

	bool OpenGl::TexImage2DMultisample( eGL_TEXTURE_STORAGE p_target, int p_iSamples, GlInternal p_internalFormat, int p_iWidth, int p_iHeight, bool p_bFixedSampleLocations )const
	{
		m_pfnTexImage2DMultisample( p_target, p_iSamples, uint32_t( p_internalFormat ), p_iWidth, p_iHeight, p_bFixedSampleLocations );
		return glCheckError( *this, "glTexImage2DMultisample" );
	}

	bool OpenGl::TexImage2DMultisample( eGL_TEXTURE_STORAGE p_target, int p_iSamples, GlInternal p_internalFormat, Castor::Size const & p_size, bool p_bFixedSampleLocations )const
	{
		m_pfnTexImage2DMultisample( p_target, p_iSamples, uint32_t( p_internalFormat ), p_size.width(), p_size.height(), p_bFixedSampleLocations );
		return glCheckError( *this, "glTexImage2DMultisample" );
	}

	int OpenGl::GetUniformLocation( uint32_t program, char const * name )const
	{
		int l_iReturn = m_pfnGetUniformLocation( program, name );

		if ( ! glCheckError( *this, "glGetUniformLocation" ) )
		{
			l_iReturn = int( GlInvalidIndex );
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
			l_uiReturn = GlInvalidIndex;
		}

		return l_uiReturn;
	}

	bool OpenGl::BindBufferBase( GlBufferTarget target, uint32_t index, uint32_t buffer )const
	{
		m_pfnBindBufferBase( uint32_t( target ), index, buffer );
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

	bool OpenGl::GetActiveUniformsiv( uint32_t program, int uniformCount, uint32_t const * uniformIndices, GlUniformBlockValue pname, int * params )const
	{
		m_pfnGetActiveUniformsiv( program, uniformCount, uniformIndices, uint32_t( pname ), params );
		return glCheckError( *this, "glGetActiveUniformsiv" );
	}

	bool OpenGl::GetActiveUniformBlockiv( uint32_t program, uint32_t uniformBlockIndex, GlUniformBlockValue pname, int * params )const
	{
		m_pfnGetActiveUniformBlockiv( program, uniformBlockIndex, uint32_t( pname ), params );
		return glCheckError( *this, "glGetActiveUniformBlockiv" );
	}

	uint32_t OpenGl::CreateShader( GlShaderType type )const
	{
		uint32_t l_uiReturn = m_pfnCreateShader( uint32_t( type ) );

		if ( ! glCheckError( *this, "glCreateShader" ) )
		{
			l_uiReturn = GlInvalidIndex;
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
		return glCheckError( *this, "glIsShader" ) && l_return != 0;
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

	bool OpenGl::GetShaderiv( uint32_t shader, GlShaderStatus pname, int * param )const
	{
		m_pfnGetShaderiv( shader, uint32_t( pname ), param );
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
			l_uiReturn = GlInvalidIndex;
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
		return glCheckError( *this, "glIsProgram" ) && l_return != 0;
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

	bool OpenGl::GetProgramiv( uint32_t program, GlShaderStatus pname, int * param )const
	{
		m_pfnGetProgramiv( program, uint32_t( pname ), param );
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
			l_iReturn = int( GlInvalidIndex );
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

	bool OpenGl::VertexAttribPointer( uint32_t index, int size, GlType type, bool normalized, int stride, void const * pointer )const
	{
		m_pfnVertexAttribPointer( index, size, uint32_t( type ), normalized, stride, pointer );
		return glCheckError( *this, "glVertexAttribPointer" );
	}

	bool OpenGl::VertexAttribPointer( uint32_t index, int size, GlType type, int stride, void const * pointer )const
	{
		m_pfnVertexAttribIPointer( index, size, uint32_t( type ), stride, pointer );
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
		return glCheckError( *this, "glIsVertexArray" ) && l_return != 0;
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

	bool OpenGl::PatchParameter( GlPatchParameter p_param, int p_iValue )const
	{
		bool l_return = false;

		if ( m_pfnPatchParameteri )
		{
			m_pfnPatchParameteri( uint32_t( p_param ), p_iValue );
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
		return glCheckError( *this, "glIsQuery" ) && l_return != 0;
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
