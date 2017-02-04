#include <Log/Logger.hpp>
#include <Graphics/Position.hpp>
#include <Graphics/Rectangle.hpp>
#include <Graphics/Size.hpp>
#include <Miscellaneous/Utils.hpp>

namespace GlRender
{
	template< typename RetT, typename ... ParamsT >
	struct GlFuncCaller
	{
		static inline RetT Call( OpenGl const & p_gl, std::function< RetT( ParamsT ... ) > const & p_function, char const * const p_name, ParamsT const & ... p_params )
		{
			REQUIRE( p_function );
			RetT l_return = p_function( p_params... );
			glCheckError( p_gl, p_name );
			return l_return;
		}
	};

	template< typename ... ParamsT >
	struct GlFuncCaller< void, ParamsT... >
	{
		static inline void Call( OpenGl const & p_gl, std::function< void( ParamsT ... ) > const & p_function, char const * const p_name, ParamsT const & ... p_params )
		{
			REQUIRE( p_function );
			p_function( p_params... );
			glCheckError( p_gl, p_name );
		}
	};

	template< typename RetT >
	struct GlFuncCaller< RetT, void >
	{
		static inline void Call( OpenGl const & p_gl, std::function< RetT() > const & p_function, char const * const p_name )
		{
			REQUIRE( p_function );
			p_function();
			glCheckError( p_gl, p_name );
		}
	};

	template< typename RetT, typename ... ParamsT >
	inline RetT ExecuteFunction( OpenGl const & p_gl, std::function< RetT( ParamsT ... ) > const & p_function, char const * const p_name, ParamsT const & ... p_params )
	{
		return GlFuncCaller< RetT, ParamsT... >::Call( p_gl, p_function, p_name, p_params... );
	}

#	define EXEC_FUNCTION( Name, ... )\
	ExecuteFunction( GetOpenGl(), m_pfn##Name, "gl"#Name, __VA_ARGS__ )

#	define EXEC_VOID_FUNCTION( Name )\
	ExecuteFunction( GetOpenGl(), m_pfn##Name, "gl"#Name )


	//*************************************************************************************************

	void TexFunctions::GenerateMipmap( GlTexDim mode )const
	{
		EXEC_FUNCTION( GenerateMipmap, uint32_t( mode ) );
	}

	void TexFunctions::BindTexture( GlTexDim mode, uint32_t texture )const
	{
		EXEC_FUNCTION( BindTexture, uint32_t( mode ), texture );
	}

	void TexFunctions::TexSubImage1D( GlTextureStorageType mode, int level, int xoffset, int width, GlFormat format, GlType type, void const * data )const
	{
		EXEC_FUNCTION( TexSubImage1D, uint32_t( mode ), level, xoffset, width, uint32_t( format ), uint32_t( type ), data );
	}

	void TexFunctions::TexSubImage2D( GlTextureStorageType mode, int level, int xoffset, int yoffset, int width, int height, GlFormat format, GlType type, void const * data )const
	{
		EXEC_FUNCTION( TexSubImage2D, uint32_t( mode ), level, xoffset, yoffset, width, height, uint32_t( format ), uint32_t( type ), data );
	}

	void TexFunctions::TexSubImage2D( GlTextureStorageType mode, int level, Castor::Position const & p_position, Castor::Size const & p_size, GlFormat format, GlType type, void const * data )const
	{
		EXEC_FUNCTION( TexSubImage2D, uint32_t( mode ), level, int( p_position.x() ), int( p_position.y() ), int( p_size.width() ), int( p_size.height() ), uint32_t( format ), uint32_t( type ), data );
	}

	void TexFunctions::TexSubImage2D( GlTextureStorageType mode, int level, Castor::Rectangle const & p_rect, GlFormat format, GlType type, void const * data )const
	{
		EXEC_FUNCTION( TexSubImage2D, uint32_t( mode ), level, p_rect.left(), p_rect.top(), p_rect.width(), p_rect.height(), uint32_t( format ), uint32_t( type ), data );
	}

	void TexFunctions::TexSubImage3D( GlTextureStorageType mode, int level, int xoffset, int yoffset, int zoffset, int width, int height, int depth, GlFormat format, GlType type, void const * data )const
	{
		EXEC_FUNCTION( TexSubImage3D, uint32_t( mode ), level, xoffset, yoffset, zoffset, width, height, depth, uint32_t( format ), uint32_t( type ), data );
	}

	void TexFunctions::TexImage1D( GlTextureStorageType mode, int level, GlInternal internalFormat, int width, int border, GlFormat format, GlType type, void const * data )const
	{
		EXEC_FUNCTION( TexImage1D, uint32_t( mode ), level, int( internalFormat ), width, border, uint32_t( format ), uint32_t( type ), data );
	}

	void TexFunctions::TexImage2D( GlTextureStorageType mode, int level, GlInternal internalFormat, int width, int height, int border, GlFormat format, GlType type, void const * data )const
	{
		EXEC_FUNCTION( TexImage2D, uint32_t( mode ), level, int( internalFormat ), width, height, border, uint32_t( format ), uint32_t( type ), data );
	}

	void TexFunctions::TexImage2D( GlTextureStorageType mode, int level, GlInternal internalFormat, Castor::Size const & p_size, int border, GlFormat format, GlType type, void const * data )const
	{
		EXEC_FUNCTION( TexImage2D, uint32_t( mode ), level, int( internalFormat ), int( p_size.width() ), int( p_size.height() ), border, uint32_t( format ), uint32_t( type ), data );
	}

	void TexFunctions::TexImage3D( GlTextureStorageType mode, int level, GlInternal internalFormat, int width, int height, int depth, int border, GlFormat format, GlType type, void const * data )const
	{
		EXEC_FUNCTION( TexImage3D, uint32_t( mode ), level, int( internalFormat ), width, height, depth, border, uint32_t( format ), uint32_t( type ), data );
	}

	void TexFunctions::GetTexImage( GlTextureStorageType mode, int level, GlFormat format, GlType type, void * img )const
	{
		EXEC_FUNCTION( GetTexImage, uint32_t( mode ), level, uint32_t( format ), uint32_t( type ), img );
	}

	//*************************************************************************************************

	void TexFunctionsDSA::GenerateMipmap( GlTexDim mode )const
	{
		EXEC_FUNCTION( GenerateMipmap, m_uiTexture, uint32_t( mode ) );
	}

	void TexFunctionsDSA::TexSubImage1D( GlTextureStorageType mode, int level, int xoffset, int width, GlFormat format, GlType type, void const * data )const
	{
		EXEC_FUNCTION( TextureSubImage1D, m_uiTexture, uint32_t( mode ), level, xoffset, width, uint32_t( format ), uint32_t( type ), data );
	}

	void TexFunctionsDSA::TexSubImage2D( GlTextureStorageType mode, int level, int xoffset, int yoffset, int width, int height, GlFormat format, GlType type, void const * data )const
	{
		EXEC_FUNCTION( TextureSubImage2D, m_uiTexture, uint32_t( mode ), level, xoffset, yoffset, width, height, uint32_t( format ), uint32_t( type ), data );
	}

	void TexFunctionsDSA::TexSubImage2D( GlTextureStorageType mode, int level, Castor::Position const & p_position, Castor::Size const & p_size, GlFormat format, GlType type, void const * data )const
	{
		EXEC_FUNCTION( TextureSubImage2D, m_uiTexture, uint32_t( mode ), level, int( p_position.x() ), int( p_position.y() ), int( p_size.width() ), int( p_size.height() ), uint32_t( format ), uint32_t( type ), data );
	}

	void TexFunctionsDSA::TexSubImage2D( GlTextureStorageType mode, int level, Castor::Rectangle const & p_rect, GlFormat format, GlType type, void const * data )const
	{
		EXEC_FUNCTION( TextureSubImage2D, m_uiTexture, uint32_t( mode ), level, p_rect.left(), p_rect.top(), p_rect.width(), p_rect.height(), uint32_t( format ), uint32_t( type ), data );
	}

	void TexFunctionsDSA::TexSubImage3D( GlTextureStorageType mode, int level, int xoffset, int yoffset, int zoffset, int width, int height, int depth, GlFormat format, GlType type, void const * data )const
	{
		EXEC_FUNCTION( TextureSubImage3D, m_uiTexture, uint32_t( mode ), level, xoffset, yoffset, zoffset, width, height, depth, uint32_t( format ), uint32_t( type ), data );
	}

	void TexFunctionsDSA::TexImage1D( GlTextureStorageType mode, int level, GlInternal internalFormat, int width, int border, GlFormat format, GlType type, void const * data )const
	{
		EXEC_FUNCTION( TextureImage1D, m_uiTexture, uint32_t( mode ), level, int( internalFormat ), width, border, uint32_t( format ), uint32_t( type ), data );
	}

	void TexFunctionsDSA::TexImage2D( GlTextureStorageType mode, int level, GlInternal internalFormat, int width, int height, int border, GlFormat format, GlType type, void const * data )const
	{
		EXEC_FUNCTION( TextureImage2D, m_uiTexture, uint32_t( mode ), level, int( internalFormat ), width, height, border, uint32_t( format ), uint32_t( type ), data );
	}

	void TexFunctionsDSA::TexImage2D( GlTextureStorageType mode, int level, GlInternal internalFormat, Castor::Size const & p_size, int border, GlFormat format, GlType type, void const * data )const
	{
		EXEC_FUNCTION( TextureImage2D, m_uiTexture, uint32_t( mode ), level, int( internalFormat ), int( p_size.width() ), int( p_size.height() ), border, uint32_t( format ), uint32_t( type ), data );
	}

	void TexFunctionsDSA::TexImage3D( GlTextureStorageType mode, int level, GlInternal internalFormat, int width, int height, int depth, int border, GlFormat format, GlType type, void const * data )const
	{
		EXEC_FUNCTION( TextureImage3D, m_uiTexture, uint32_t( mode ), level, int( internalFormat ), width, height, depth, border, uint32_t( format ), uint32_t( type ), data );
	}

	void TexFunctionsDSA::GetTexImage( GlTextureStorageType mode, int level, GlFormat format, GlType type, void * img )const
	{
		EXEC_FUNCTION( GetTextureImage, m_uiTexture, uint32_t( mode ), level, uint32_t( format ), uint32_t( type ), img );
	}

	//*************************************************************************************************

	void BufFunctionsBase::BufferAddressRange( GlAddress pname, uint32_t index, uint64_t address, size_t length )const
	{
		EXEC_FUNCTION( BufferAddressRange, uint32_t( pname ), index, address, length );
	}

	void BufFunctionsBase::VertexFormat( int size, GlType type, int stride )const
	{
		EXEC_FUNCTION( VertexFormat, size, uint32_t( type ), stride );
	}

	void BufFunctionsBase::NormalFormat( GlType type, int stride )const
	{
		EXEC_FUNCTION( NormalFormat, uint32_t( type ), stride );
	}

	void BufFunctionsBase::ColorFormat( int size, GlType type, int stride )const
	{
		EXEC_FUNCTION( ColorFormat, size, uint32_t( type ), stride );
	}

	void BufFunctionsBase::IndexFormat( GlType type, int stride )const
	{
		EXEC_FUNCTION( IndexFormat, uint32_t( type ), stride );
	}

	void BufFunctionsBase::TexCoordFormat( int size, GlType type, int stride )const
	{
		EXEC_FUNCTION( TexCoordFormat, size, uint32_t( type ), stride );
	}

	void BufFunctionsBase::EdgeFlagFormat( int stride )const
	{
		EXEC_FUNCTION( EdgeFlagFormat, stride );
	}

	void BufFunctionsBase::SecondaryColorFormat( int size, GlType type, int stride )const
	{
		EXEC_FUNCTION( SecondaryColorFormat, size, uint32_t( type ), stride );
	}

	void BufFunctionsBase::FogCoordFormat( uint32_t type, int stride )const
	{
		EXEC_FUNCTION( FogCoordFormat, uint32_t( type ), stride );
	}

	void BufFunctionsBase::VertexAttribFormat( uint32_t index, int size, GlType type, bool normalized, int stride )const
	{
		EXEC_FUNCTION( VertexAttribFormat, index, size, uint32_t( type ), normalized, stride );
	}

	void BufFunctionsBase::VertexAttribIFormat( uint32_t index, int size, GlType type, int stride )const
	{
		EXEC_FUNCTION( VertexAttribIFormat, index, size, uint32_t( type ), stride );
	}

	void BufFunctionsBase::MakeBufferResident( GlBufferTarget target, GlAccessType access )const
	{
		EXEC_FUNCTION( MakeBufferResident, uint32_t( target ), uint32_t( access ) );
	}

	void BufFunctionsBase::MakeBufferNonResident( GlBufferTarget target )const
	{
		EXEC_FUNCTION( MakeBufferNonResident, uint32_t( target ) );
	}

	bool BufFunctionsBase::IsBufferResident( GlBufferTarget target )const
	{
		return EXEC_FUNCTION( IsBufferResident, uint32_t( target ) ) != GL_FALSE;
	}

	void BufFunctionsBase::MakeNamedBufferResident( uint32_t buffer, GlAccessType access )const
	{
		EXEC_FUNCTION( MakeNamedBufferResident, buffer, uint32_t( access ) );
	}

	void BufFunctionsBase::MakeNamedBufferNonResident( uint32_t buffer )const
	{
		EXEC_FUNCTION( MakeNamedBufferNonResident, buffer );
	}

	bool BufFunctionsBase::IsNamedBufferResident( uint32_t buffer )const
	{
		return EXEC_FUNCTION( IsNamedBufferResident, uint32_t( buffer ) ) != GL_FALSE;
	}

	void BufFunctionsBase::GetBufferParameter( GlBufferTarget target, GlBufferParameter pname, uint64_t * params )const
	{
		EXEC_FUNCTION( GetBufferParameterui64v, uint32_t( target ), uint32_t( pname ), params );
	}

	void BufFunctionsBase::GetNamedBufferParameter( uint32_t buffer, GlBufferParameter pname, uint64_t * params )const
	{
		EXEC_FUNCTION( GetNamedBufferParameterui64v, buffer, uint32_t( pname ), params );
	}

	//*************************************************************************************************

	void BufFunctions::BindBuffer( GlBufferTarget p_target, uint32_t buffer )const
	{
		EXEC_FUNCTION( BindBuffer, uint32_t( p_target ), buffer );
	}

	void BufFunctions::BufferData( GlBufferTarget p_target, ptrdiff_t size, void const * data, GlBufferMode usage )const
	{
		EXEC_FUNCTION( BufferData, uint32_t( p_target ), size, data, uint32_t( usage ) );
	}

	void BufFunctions::CopyBufferSubData( GlBufferTarget readtarget, GlBufferTarget writetarget, ptrdiff_t readoffset, ptrdiff_t writeoffset, ptrdiff_t size )const
	{
		EXEC_FUNCTION( CopyBufferSubData, uint32_t( readtarget ), uint32_t( writetarget ), readoffset, writeoffset, size );
	}

	void BufFunctions::BufferSubData( GlBufferTarget p_target, ptrdiff_t offset, ptrdiff_t size, void const * data )const
	{
		EXEC_FUNCTION( BufferSubData, uint32_t( p_target ), offset, size, data );
	}

	void BufFunctions::GetBufferParameter( GlBufferTarget p_target, GlBufferParameter pname, int * params )const
	{
		EXEC_FUNCTION( GetBufferParameteriv, uint32_t( p_target ), uint32_t( pname ), params );
	}

	void * BufFunctions::MapBuffer( GlBufferTarget p_target, GlAccessType access )const
	{
		return EXEC_FUNCTION( MapBuffer, uint32_t( p_target ), uint32_t( access ) );
	}

	void BufFunctions::UnmapBuffer( GlBufferTarget p_target )const
	{
		EXEC_FUNCTION( UnmapBuffer, uint32_t( p_target ) );
	}

	void * BufFunctions::MapBufferRange( GlBufferTarget p_target, ptrdiff_t offset, ptrdiff_t length, uint32_t access )const
	{
		return EXEC_FUNCTION( MapBufferRange, uint32_t( p_target ), offset, length, access );
	}

	void BufFunctions::FlushMappedBufferRange( GlBufferTarget p_target, ptrdiff_t offset, ptrdiff_t length )const
	{
		EXEC_FUNCTION( FlushMappedBufferRange, uint32_t( p_target ), offset, length );
	}

	//*************************************************************************************************

	void BufFunctionsDSA::BufferData( GlBufferTarget p_target, ptrdiff_t size, void const * data, GlBufferMode usage )const
	{
		EXEC_FUNCTION( NamedBufferData, m_uiBuffer, size, data, uint32_t( usage ) );
	}

	void BufFunctionsDSA::BufferSubData( GlBufferTarget p_target, ptrdiff_t offset, ptrdiff_t size, void const * data )const
	{
		EXEC_FUNCTION( NamedBufferSubData, m_uiBuffer, offset, size, data );
	}

	void BufFunctionsDSA::CopyBufferSubData( GlBufferTarget readtarget, GlBufferTarget writetarget, ptrdiff_t readoffset, ptrdiff_t writeoffset, ptrdiff_t size )const
	{
		EXEC_FUNCTION( CopyNamedBufferSubData, uint32_t( readtarget ), uint32_t( writetarget ), readoffset, writeoffset, size );
	}

	void BufFunctionsDSA::GetBufferParameter( GlBufferTarget p_target, GlBufferParameter pname, int * params )const
	{
		EXEC_FUNCTION( GetNamedBufferParameteriv, m_uiBuffer, uint32_t( pname ), params );
	}

	void * BufFunctionsDSA::MapBuffer( GlBufferTarget p_target, GlAccessType access )const
	{
		return EXEC_FUNCTION( MapNamedBuffer, m_uiBuffer, uint32_t( access ) );
	}

	void BufFunctionsDSA::UnmapBuffer( GlBufferTarget p_target )const
	{
		EXEC_FUNCTION( UnmapNamedBuffer, m_uiBuffer );
	}

	void * BufFunctionsDSA::MapBufferRange( GlBufferTarget p_target, ptrdiff_t offset, ptrdiff_t length, uint32_t access )const
	{
		return EXEC_FUNCTION( MapNamedBufferRange, m_uiBuffer, offset, length, access );
	}

	void BufFunctionsDSA::FlushMappedBufferRange( GlBufferTarget p_target, ptrdiff_t offset, ptrdiff_t length )const
	{
		EXEC_FUNCTION( FlushMappedNamedBufferRange, m_uiBuffer, offset, length );
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

	bool OpenGl::HasSsbo()const
	{
		return m_bHasSsbo;
	}

	bool OpenGl::HasInstancing()const
	{
		return m_bHasInstancedDraw && m_bHasInstancedArrays;
	}

	bool OpenGl::HasComputeVariableGroupSize()const
	{
		return m_bHasComputeVariableGroupSize;
	}

	bool OpenGl::HasNonPowerOfTwoTextures()const
	{
		return m_bHasNonPowerOfTwoTextures;
	}

	bool OpenGl::CanBindVboToGpuAddress()const
	{
		return m_bBindVboToGpuAddress;
	}

	Castor3D::ElementType OpenGl::Get( GlslAttributeType p_type )const
	{
		switch ( p_type )
		{
		case GlslAttributeType::eFloat:
			return Castor3D::ElementType::eFloat;
			break;

		case GlslAttributeType::eFloatVec2:
			return Castor3D::ElementType::eVec2;
			break;

		case GlslAttributeType::eFloatVec3:
			return Castor3D::ElementType::eVec3;
			break;

		case GlslAttributeType::eFloatVec4:
			return Castor3D::ElementType::eVec4;
			break;

		case GlslAttributeType::eInt:
			return Castor3D::ElementType::eInt;
			break;

		case GlslAttributeType::eUnsignedInt:
			return Castor3D::ElementType::eInt;
			break;

		case GlslAttributeType::eIntVec2:
			return Castor3D::ElementType::eIVec2;
			break;

		case GlslAttributeType::eIntVec3:
			return Castor3D::ElementType::eIVec3;
			break;

		case GlslAttributeType::eIntVec4:
			return Castor3D::ElementType::eIVec4;
			break;

		case GlslAttributeType::eUnsignedIntVec2:
			return Castor3D::ElementType::eIVec2;
			break;

		case GlslAttributeType::eUnsignedIntVec3:
			return Castor3D::ElementType::eIVec3;
			break;

		case GlslAttributeType::eUnsignedIntVec4:
			return Castor3D::ElementType::eIVec4;
			break;

		case GlslAttributeType::eFloatMat2:
			return Castor3D::ElementType::eMat2;
			break;

		case GlslAttributeType::eFloatMat3:
			return Castor3D::ElementType::eMat3;
			break;

		case GlslAttributeType::eFloatMat4:
			return Castor3D::ElementType::eMat4;
			break;

		default:
			assert( false && "Unsupported attribute type." );
			break;
		}

		return Castor3D::ElementType::eCount;
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
		if ( p_index == Castor3D::TextureType::eBuffer )
		{
			if ( !HasTbo() )
			{
				p_index = Castor3D::TextureType::eOneDimension;
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

	inline Castor::FlagCombination< GlBufferBit > OpenGl::GetComponents( Castor::FlagCombination< Castor3D::BufferComponent > const & p_components )const
	{
		Castor::FlagCombination< GlBufferBit > l_return;
		l_return |= ( Castor::CheckFlag( p_components, Castor3D::BufferComponent::eColour ) ? GlBufferBit::eColour : GlBufferBit( 0u ) );
		l_return |= ( Castor::CheckFlag( p_components, Castor3D::BufferComponent::eDepth ) ? GlBufferBit::eDepth : GlBufferBit( 0u ) );
		l_return |= ( Castor::CheckFlag( p_components, Castor3D::BufferComponent::eStencil ) ? GlBufferBit::eStencil : GlBufferBit( 0u ) );
		return l_return;
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

	inline GlFace OpenGl::Get( Castor3D::Culling p_eFace )const
	{
		return Faces[uint32_t( p_eFace )];
	}

	inline GlFillMode OpenGl::Get( Castor3D::FillMode p_mode )const
	{
		return FillModes[uint32_t( p_mode )];
	}

	inline GlComparator OpenGl::Get( Castor3D::StencilFunc p_func )const
	{
		return StencilFuncs[uint32_t( p_func )];
	}

	inline GlStencilOp OpenGl::Get( Castor3D::StencilOp p_eOp )const
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

	inline GlQueryType OpenGl::Get( Castor3D::QueryType p_value )const
	{
		return Queries[uint32_t( p_value )];
	}

	inline GlQueryInfo OpenGl::Get( Castor3D::QueryInfo p_value )const
	{
		return QueryInfos[uint32_t( p_value )];
	}

	inline GlTextureStorageType OpenGl::Get( Castor3D::TextureStorageType p_value )const
	{
		return TextureStorages[uint32_t( p_value )];
	}

	inline GlTexDim OpenGl::Get( Castor3D::CubeMapFace p_value )const
	{
		return CubeMapFaces[uint32_t( p_value )];
	}

	inline GlCompareMode OpenGl::Get( Castor3D::ComparisonMode p_value )const
	{
		return ComparisonModes[uint32_t( p_value )];
	}

	inline bool OpenGl::Get( Castor3D::WritingMask p_eMask )const
	{
		return WriteMasks[uint32_t( p_eMask )];
	}

	inline Castor::FlagCombination< GlBarrierBit > OpenGl::Get( Castor3D::MemoryBarriers const & p_barriers )const
	{
		Castor::FlagCombination< GlBarrierBit > l_return;

		if ( CheckFlag( p_barriers, Castor3D::MemoryBarrier::eVertexBuffer ) )
		{
			l_return |= GlBarrierBit::eVertexArrayAttrib;
		}

		if ( CheckFlag( p_barriers, Castor3D::MemoryBarrier::eIndexBuffer ) )
		{
			l_return |= GlBarrierBit::eElementArray;
		}

		if ( CheckFlag( p_barriers, Castor3D::MemoryBarrier::eUniformBuffer ) )
		{
			l_return |= GlBarrierBit::eUniform;
		}

		if ( CheckFlag( p_barriers, Castor3D::MemoryBarrier::eAtomicCounterBuffer ) )
		{
			l_return |= GlBarrierBit::eAtomicCounter;
		}

		if ( CheckFlag( p_barriers, Castor3D::MemoryBarrier::eQueryBuffer ) )
		{
			l_return |= GlBarrierBit::eQueryBuffer;
		}

		if ( CheckFlag( p_barriers, Castor3D::MemoryBarrier::eShaderStorageBuffer ) )
		{
			l_return |= GlBarrierBit::eShaderStorage;
		}

		return l_return;
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

		if ( Castor::CheckFlag( p_flags, Castor3D::BufferAccessType::eDynamic ) )
		{
			if ( Castor::CheckFlag( p_flags, Castor3D::BufferAccessNature::eRead ) )
			{
				l_eReturn = GlBufferMode::eDynamicRead;
			}
			else if ( Castor::CheckFlag( p_flags, Castor3D::BufferAccessNature::eDraw ) )
			{
				l_eReturn = GlBufferMode::eDynamicDraw;
			}
			else if ( Castor::CheckFlag( p_flags, Castor3D::BufferAccessNature::eCopy ) )
			{
				l_eReturn = GlBufferMode::eDynamicCopy;
			}
		}
		else if ( Castor::CheckFlag( p_flags, Castor3D::BufferAccessType::eStatic ) )
		{
			if ( Castor::CheckFlag( p_flags, Castor3D::BufferAccessNature::eRead ) )
			{
				l_eReturn = GlBufferMode::eStaticRead;
			}
			else if ( Castor::CheckFlag( p_flags, Castor3D::BufferAccessNature::eDraw ) )
			{
				l_eReturn = GlBufferMode::eStaticDraw;
			}
			else if ( Castor::CheckFlag( p_flags, Castor3D::BufferAccessNature::eCopy ) )
			{
				l_eReturn = GlBufferMode::eStaticCopy;
			}
		}
		else if ( Castor::CheckFlag( p_flags, Castor3D::BufferAccessType::eStream ) )
		{
			if ( Castor::CheckFlag( p_flags, Castor3D::BufferAccessNature::eRead ) )
			{
				l_eReturn = GlBufferMode::eStreamRead;
			}
			else if ( Castor::CheckFlag( p_flags, Castor3D::BufferAccessNature::eDraw ) )
			{
				l_eReturn = GlBufferMode::eStreamDraw;
			}
			else if ( Castor::CheckFlag( p_flags, Castor3D::BufferAccessNature::eCopy ) )
			{
				l_eReturn = GlBufferMode::eStreamCopy;
			}
		}

		return l_eReturn;
	}

	GlProvider OpenGl::GetProvider()const
	{
		return m_gpu;
	}

	GlAccessType OpenGl::GetLockFlags( Castor3D::AccessTypes const & p_flags )const
	{
		GlAccessType l_eLockFlags = GlAccessType::eReadWrite;

		if ( Castor::CheckFlag( p_flags, Castor3D::AccessType::eRead ) )
		{
			if ( Castor::CheckFlag( p_flags, Castor3D::AccessType::eWrite ) )
			{
				l_eLockFlags = GlAccessType::eReadWrite;
			}
			else
			{
				l_eLockFlags = GlAccessType::eRead;
			}
		}
		else if ( Castor::CheckFlag( p_flags, Castor3D::AccessType::eWrite ) )
		{
			l_eLockFlags = GlAccessType::eWrite;
		}

		return l_eLockFlags;
	}

	Castor::FlagCombination< GlBufferMappingBit > OpenGl::GetBitfieldFlags( Castor3D::AccessTypes const & p_flags )const
	{
		Castor::FlagCombination< GlBufferMappingBit > l_uiFlags = 0;

		if ( Castor::CheckFlag( p_flags, Castor3D::AccessType::eRead ) )
		{
			Castor::AddFlag( l_uiFlags, GlBufferMappingBit::eRead );

			if ( Castor::CheckFlag( p_flags, Castor3D::AccessType::eWrite ) )
			{
				Castor::AddFlag( l_uiFlags, GlBufferMappingBit::eWrite );
			}
		}
		else if ( Castor::CheckFlag( p_flags, Castor3D::AccessType::eWrite ) )
		{
			Castor::AddFlag( l_uiFlags, GlBufferMappingBit::eWrite );
			Castor::AddFlag( l_uiFlags, GlBufferMappingBit::eInvalidateRange );
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

	void OpenGl::ClearColor( float red, float green, float blue, float alpha )const
	{
		EXEC_FUNCTION( ClearColor, red, green, blue, alpha );
	}

	void OpenGl::ClearColor( Castor::Colour const & p_colour )const
	{
		EXEC_FUNCTION( ClearColor, p_colour.red().value(), p_colour.green().value(), p_colour.blue().value(), p_colour.alpha().value() );
	}

	void OpenGl::ClearDepth( double value )const
	{
		EXEC_FUNCTION( ClearDepth, value );
	}

	void OpenGl::Clear( uint32_t mask )const
	{
		EXEC_FUNCTION( Clear, mask );
	}

	void OpenGl::DrawArrays( GlTopology mode, int first, int count )const
	{
		EXEC_FUNCTION( DrawArrays, uint32_t( mode ), first, count );
	}

	void OpenGl::DrawElements( GlTopology mode, int count, GlType type, void const * indices )const
	{
		EXEC_FUNCTION( DrawElements, uint32_t( mode ), count, uint32_t( type ), indices );
	}

	void OpenGl::Enable( GlTweak mode )const
	{
		EXEC_FUNCTION( Enable, uint32_t( mode ) );
	}

	void OpenGl::Disable( GlTweak mode )const
	{
		EXEC_FUNCTION( Disable, uint32_t( mode ) );
	}

	void OpenGl::Enable( GlTexDim texture )const
	{
		EXEC_FUNCTION( Enable, uint32_t( texture ) );
	}

	void OpenGl::Disable( GlTexDim texture )const
	{
		EXEC_FUNCTION( Disable, uint32_t( texture ) );
	}

	void OpenGl::SelectBuffer( int size, uint32_t * buffer )const
	{
		EXEC_FUNCTION( SelectBuffer, size, buffer );
	}

	void OpenGl::GetIntegerv( uint32_t pname, int * params )const
	{
		EXEC_FUNCTION( GetIntegerv, pname, params );
	}

	void OpenGl::GetIntegerv( GlMax pname, int * params )const
	{
		return GetIntegerv( uint32_t( pname ), params );
	}

	void OpenGl::GetIntegerv( GlMin pname, int * params )const
	{
		return GetIntegerv( uint32_t( pname ), params );
	}

	void OpenGl::GetIntegerv( GlGpuInfo pname, int * params )const
	{
		return GetIntegerv( uint32_t( pname ), params );
	}

	void OpenGl::GetIntegerv( uint32_t pname, uint64_t * params )const
	{
		EXEC_FUNCTION( GetIntegerui64v, pname, params );
	}

#if defined( CASTOR_PLATFORM_WINDOWS )

	void OpenGl::MakeCurrent( HDC hdc, HGLRC hglrc )const
	{
		wglMakeCurrent( hdc, hglrc );
	}

	void OpenGl::SwapBuffers( HDC hdc )const
	{
		::SwapBuffers( hdc );
	}

	void OpenGl::SwapInterval( int interval )const
	{
		EXEC_FUNCTION( SwapInterval, interval );
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
		return EXEC_FUNCTION( CreateContextAttribs, hDC, hShareContext, attribList );
	}

	bool OpenGl::DeleteContext( HGLRC hContext )const
	{
		return EXEC_FUNCTION( DeleteContext, hContext ) == GL_TRUE;
	}

#elif defined( CASTOR_PLATFORM_LINUX )

	void OpenGl::MakeCurrent( Display * pDisplay, GLXDrawable drawable, GLXContext context )const
	{
		EXEC_FUNCTION( MakeCurrent, pDisplay, drawable, context );
	}

	void OpenGl::SwapBuffers( Display * pDisplay, GLXDrawable drawable )const
	{
		EXEC_FUNCTION( SwapBuffers, pDisplay, drawable );
	}

	void OpenGl::SwapInterval( Display * pDisplay, GLXDrawable drawable, int interval )const
	{
		EXEC_FUNCTION( SwapInterval, pDisplay, drawable, interval );
	}

	GLXContext OpenGl::CreateContext( Display * pDisplay, XVisualInfo * pVisualInfo, GLXContext shareList, Bool direct )const
	{
		return EXEC_FUNCTION( CreateContext, pDisplay, pVisualInfo, shareList, direct );
	}

	bool OpenGl::HasCreateContextAttribs()const
	{
		return m_pfnCreateContextAttribs != nullptr;
	}

	GLXContext OpenGl::CreateContextAttribs( Display * pDisplay, GLXFBConfig fbconfig, GLXContext shareList, Bool direct, int const * attribList )const
	{
		return EXEC_FUNCTION( CreateContextAttribs, pDisplay, fbconfig, shareList, direct, attribList );
	}

	bool OpenGl::DeleteContext( Display * pDisplay, GLXContext context )const
	{
		EXEC_FUNCTION( DeleteContext, pDisplay, context );
		return true;
	}

#else

#	error "Yet unsupported OS"

#endif

	void OpenGl::Viewport( int x, int y, int width, int height )const
	{
		EXEC_FUNCTION( Viewport, x, y, width, height );
	}

	void OpenGl::CullFace( GlFace face )const
	{
		EXEC_FUNCTION( CullFace, uint32_t( face ) );
	}

	void OpenGl::FrontFace( GlFrontFaceDirection face )const
	{
		EXEC_FUNCTION( FrontFace, uint32_t( face ) );
	}

	void OpenGl::GenTextures( int n, uint32_t * textures )const
	{
		EXEC_FUNCTION( GenTextures, n, textures );
	}

	void OpenGl::DeleteTextures( int n, uint32_t const * textures )const
	{
		EXEC_FUNCTION( DeleteTextures, n, textures );
	}

	bool OpenGl::IsTexture( uint32_t texture )const
	{
		return EXEC_FUNCTION( IsTexture, texture ) != GL_FALSE;
	}

	void OpenGl::BindTexture( GlTexDim mode, uint32_t texture )const
	{
		m_pTexFunctions->BindTexture( mode, texture );
	}

	void OpenGl::GenerateMipmap( GlTexDim mode )const
	{
		m_pTexFunctions->GenerateMipmap( mode );
	}

	void OpenGl::ActiveTexture( GlTextureIndex target )const
	{
		EXEC_FUNCTION( ActiveTexture, uint32_t( target ) );
	}

	void OpenGl::ClientActiveTexture( GlTextureIndex target )const
	{
		EXEC_FUNCTION( ClientActiveTexture, uint32_t( target ) );
	}

	void OpenGl::TexSubImage1D( GlTextureStorageType mode, int level, int xoffset, int width, GlFormat format, GlType type, void const * data )const
	{
		m_pTexFunctions->TexSubImage1D( mode, level, xoffset, width, format, type, data );
	}

	void OpenGl::TexSubImage2D( GlTextureStorageType mode, int level, int xoffset, int yoffset, int width, int height, GlFormat format, GlType type, void const * data )const
	{
		m_pTexFunctions->TexSubImage2D( mode, level, xoffset, yoffset, width, height, format, type, data );
	}

	void OpenGl::TexSubImage2D( GlTextureStorageType mode, int level, Castor::Position const & p_position, Castor::Size const & p_size, GlFormat format, GlType type, void const * data )const
	{
		m_pTexFunctions->TexSubImage2D( mode, level, p_position.x(), p_position.y(), p_size.width(), p_size.height(), format, type, data );
	}

	void OpenGl::TexSubImage2D( GlTextureStorageType mode, int level, Castor::Rectangle const & p_rect, GlFormat format, GlType type, void const * data )const
	{
		m_pTexFunctions->TexSubImage2D( mode, level, p_rect.left(), p_rect.top(), p_rect.width(), p_rect.height(), format, type, data );
	}

	void OpenGl::TexSubImage3D( GlTextureStorageType mode, int level, int xoffset, int yoffset, int zoffset, int width, int height, int depth, GlFormat format, GlType type, void const * data )const
	{
		m_pTexFunctions->TexSubImage3D( mode, level, xoffset, yoffset, zoffset, width, height, depth, format, type, data );
	}

	void OpenGl::TexImage1D( GlTextureStorageType mode, int level, GlInternal internalFormat, int width, int border, GlFormat format, GlType type, void const * data )const
	{
		m_pTexFunctions->TexImage1D( mode, level, internalFormat, width, border, format, type, data );
	}

	void OpenGl::TexImage2D( GlTextureStorageType mode, int level, GlInternal internalFormat, int width, int height, int border, GlFormat format, GlType type, void const * data )const
	{
		m_pTexFunctions->TexImage2D( mode, level, internalFormat, width, height, border, format, type, data );
	}

	void OpenGl::TexImage2D( GlTextureStorageType mode, int level, GlInternal internalFormat, Castor::Size const & p_size, int border, GlFormat format, GlType type, void const * data )const
	{
		m_pTexFunctions->TexImage2D( mode, level, internalFormat, p_size.width(), p_size.height(), border, format, type, data );
	}

	void OpenGl::TexImage3D( GlTextureStorageType mode, int level, GlInternal internalFormat, int width, int height, int depth, int border, GlFormat format, GlType type, void const * data )const
	{
		m_pTexFunctions->TexImage3D( mode, level, internalFormat, width, height, depth, border, format, type, data );
	}

	void OpenGl::GetTexImage( GlTextureStorageType mode, int level, GlFormat format, GlType type, void * img )const
	{
		m_pTexFunctions->GetTexImage( mode, level, format, type, img );
	}

	void OpenGl::BlendFunc( GlBlendFactor sfactor, GlBlendFactor dfactor )const
	{
		EXEC_FUNCTION( BlendFunc, uint32_t( sfactor ), uint32_t( dfactor ) );
	}

	void OpenGl::BlendFunc( GlBlendFactor p_eRgbSrc, GlBlendFactor p_eRgbDst, GlBlendFactor p_eAlphaSrc, GlBlendFactor p_eAlphaDst )const
	{
		EXEC_FUNCTION( BlendFuncSeparate, uint32_t( p_eRgbSrc ), uint32_t( p_eRgbDst ), uint32_t( p_eAlphaSrc ), uint32_t( p_eAlphaDst ) );
	}

	void OpenGl::BlendFunc( uint32_t p_uiBuffer, GlBlendFactor p_eRgbSrc, GlBlendFactor p_eRgbDst, GlBlendFactor p_eAlphaSrc, GlBlendFactor p_eAlphaDst )const
	{
		EXEC_FUNCTION( BlendFuncSeparatei, p_uiBuffer, uint32_t( p_eRgbSrc ), uint32_t( p_eRgbDst ), uint32_t( p_eAlphaSrc ), uint32_t( p_eAlphaDst ) );
	}

	void OpenGl::BlendEquation( GlBlendOp p_eOp )const
	{
		EXEC_FUNCTION( BlendEquation, uint32_t( p_eOp ) );
	}

	void OpenGl::BlendEquation( uint32_t p_uiBuffer, GlBlendOp p_eOp )const
	{
		EXEC_FUNCTION( BlendEquationi, p_uiBuffer, uint32_t( p_eOp ) );
	}

	void OpenGl::ComparisonFunc( GlComparator func, float ref )const
	{
		EXEC_FUNCTION( AlphaFunc, uint32_t( func ), ref );
	}

	void OpenGl::DepthFunc( GlComparator p_func )const
	{
		EXEC_FUNCTION( DepthFunc, uint32_t( p_func ) );
	}

	void OpenGl::DepthMask( bool p_bMask )const
	{
		EXEC_FUNCTION( DepthMask, uint8_t( p_bMask ) );
	}

	void OpenGl::ColorMask( bool p_r, bool p_g, bool p_b, bool p_a )const
	{
		EXEC_FUNCTION( ColorMask, uint8_t( p_r ), uint8_t( p_g ), uint8_t( p_b ), uint8_t( p_a ) );
	}

	void OpenGl::PolygonMode( GlFace p_eFacing, GlFillMode p_mode )const
	{
		EXEC_FUNCTION( PolygonMode, uint32_t( p_eFacing ), uint32_t( p_mode ) );
	}

	void OpenGl::StencilOp( GlStencilOp p_eStencilFail, GlStencilOp p_eDepthFail, GlStencilOp p_eStencilPass )const
	{
		EXEC_FUNCTION( StencilOp, uint32_t( p_eStencilFail ), uint32_t( p_eDepthFail ), uint32_t( p_eStencilPass ) );
	}

	void OpenGl::StencilFunc( GlComparator p_func, int p_iRef, uint32_t p_uiMask )const
	{
		EXEC_FUNCTION( StencilFunc, uint32_t( p_func ), p_iRef, p_uiMask );
	}

	void OpenGl::StencilMask( uint32_t p_uiMask )const
	{
		EXEC_FUNCTION( StencilMask, p_uiMask );
	}

	void OpenGl::StencilOpSeparate( GlFace p_eFacing, GlStencilOp p_eStencilFail, GlStencilOp p_eDepthFail, GlStencilOp p_eStencilPass )const
	{
		EXEC_FUNCTION( StencilOpSeparate, uint32_t( p_eFacing ), uint32_t( p_eStencilFail ), uint32_t( p_eDepthFail ), uint32_t( p_eStencilPass ) );
	}

	void OpenGl::StencilFuncSeparate( GlFace p_eFacing, GlComparator p_func, int p_iRef, uint32_t p_uiMask )const
	{
		EXEC_FUNCTION( StencilFuncSeparate, uint32_t( p_eFacing ), uint32_t( p_func ), p_iRef, p_uiMask );
	}

	void OpenGl::StencilMaskSeparate( GlFace p_eFacing, uint32_t p_uiMask )const
	{
		EXEC_FUNCTION( StencilMaskSeparate, uint32_t( p_eFacing ), p_uiMask );
	}

	void OpenGl::Hint( GlHint p_eHint, GlHintValue p_eValue )const
	{
		EXEC_FUNCTION( Hint, uint32_t( p_eHint ), uint32_t( p_eValue ) );
	}

	void OpenGl::PolygonOffset( float p_fFactor, float p_fUnits )const
	{
		EXEC_FUNCTION( PolygonOffset, p_fFactor, p_fUnits );
	}

	void OpenGl::BlendColor( Castor::Colour const & p_colour )const
	{
		EXEC_FUNCTION( BlendColor, p_colour.red().value(), p_colour.green().value(), p_colour.blue().value(), p_colour.alpha().value() );
	}

	void OpenGl::SampleCoverage( float fValue, bool invert )const
	{
		EXEC_FUNCTION( SampleCoverage, fValue, uint8_t( invert ) );
	}

	void OpenGl::DrawArraysInstanced( GlTopology mode, int first, int count, int primcount )const
	{
		EXEC_FUNCTION( DrawArraysInstanced, uint32_t( mode ), first, count, primcount );
	}

	void OpenGl::DrawElementsInstanced( GlTopology mode, int count, GlType type, const void * indices, int primcount )const
	{
		EXEC_FUNCTION( DrawElementsInstanced, uint32_t( mode ), count, uint32_t( type ), indices, primcount );
	}

	void OpenGl::VertexAttribDivisor( uint32_t index, uint32_t divisor )const
	{
		EXEC_FUNCTION( VertexAttribDivisor, index, divisor );
	}

	void OpenGl::ReadBuffer( GlBufferBinding p_buffer )const
	{
		EXEC_FUNCTION( ReadBuffer, uint32_t( p_buffer ) );
	}

	void OpenGl::ReadPixels( int x, int y, int width, int height, GlFormat format, GlType type, void * pixels )const
	{
		EXEC_FUNCTION( ReadPixels, x, y, width, height, uint32_t( format ), uint32_t( type ), pixels );
	}

	void OpenGl::ReadPixels( Castor::Position const & p_position, Castor::Size const & p_size, GlFormat format, GlType type, void * pixels )const
	{
		EXEC_FUNCTION( ReadPixels, int( p_position.x() ), int( p_position.y() ), int( p_size.width() ), int( p_size.height() ), uint32_t( format ), uint32_t( type ), pixels );
	}

	void OpenGl::ReadPixels( Castor::Rectangle const & p_rect, GlFormat format, GlType type, void * pixels )const
	{
		EXEC_FUNCTION( ReadPixels, p_rect.left(), p_rect.top(), p_rect.width(), p_rect.height(), uint32_t( format ), uint32_t( type ), pixels );
	}

	void OpenGl::DrawBuffer( GlBufferBinding p_buffer )const
	{
		EXEC_FUNCTION( DrawBuffer, uint32_t( p_buffer ) );
	}

	void OpenGl::DrawPixels( int width, int height, GlFormat format, GlType type, void const * data )const
	{
		EXEC_FUNCTION( DrawPixels, width, height, uint32_t( format ), uint32_t( type ), data );
	}

	void OpenGl::PixelStore( GlStorageMode p_mode, int p_iParam )const
	{
		EXEC_FUNCTION( PixelStorei, uint32_t( p_mode ), p_iParam );
	}

	void OpenGl::PixelStore( GlStorageMode p_mode, float p_fParam )const
	{
		EXEC_FUNCTION( PixelStoref, uint32_t( p_mode ), p_fParam );
	}

	void OpenGl::TexStorage1D( GlTextureStorageType target, GLint levels, GlInternal internalformat, GLsizei width )const
	{
		EXEC_FUNCTION( TexStorage1D, uint32_t( target ), levels, int( internalformat ), width );
	}

	void OpenGl::TexStorage2D( GlTextureStorageType target, GLint levels, GlInternal internalformat, GLsizei width, GLsizei height )const
	{
		EXEC_FUNCTION( TexStorage2D, uint32_t( target ), levels, int( internalformat ), width, height );
	}

	void OpenGl::TexStorage3D( GlTextureStorageType target, GLint levels, GlInternal internalformat, GLsizei width, GLsizei height, GLsizei depth )const
	{
		EXEC_FUNCTION( TexStorage3D, uint32_t( target ), levels, int( internalformat ), width, height, depth );
	}

	void OpenGl::TexStorage2DMultisample( GlTextureStorageType target, GLsizei samples, GlInternal internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations )const
	{
		EXEC_FUNCTION( TexStorage2DMultisample, uint32_t( target ), samples, int( internalformat ), width, height, fixedsamplelocations );
	}

	void OpenGl::TexStorage3DMultisample( GlTextureStorageType target, GLsizei samples, GlInternal internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations )const
	{
		EXEC_FUNCTION( TexStorage3DMultisample, uint32_t( target ), samples, int( internalformat ), width, height, depth, fixedsamplelocations );
	}

	void OpenGl::GenSamplers( int count, uint32_t * samplers )const
	{
		EXEC_FUNCTION( GenSamplers, count, samplers );
	}

	void OpenGl::DeleteSamplers( int count, const uint32_t * samplers )const
	{
		EXEC_FUNCTION( DeleteSamplers, count, samplers );
	}

	bool OpenGl::IsSampler( uint32_t sampler )const
	{
		return EXEC_FUNCTION( IsSampler, sampler ) != GL_FALSE;
	}

	void OpenGl::BindSampler( uint32_t unit, uint32_t sampler )const
	{
		EXEC_FUNCTION( BindSampler, unit, sampler );
	}

	void OpenGl::GetSamplerParameter( uint32_t sampler, GlSamplerParameter pname, uint32_t * params )const
	{
		EXEC_FUNCTION( GetSamplerParameteruiv, sampler, uint32_t( pname ), params );
	}

	void OpenGl::GetSamplerParameter( uint32_t sampler, GlSamplerParameter pname, float * params )const
	{
		EXEC_FUNCTION( GetSamplerParameterfv, sampler, uint32_t( pname ), params );
	}

	void OpenGl::GetSamplerParameter( uint32_t sampler, GlSamplerParameter pname, int * params )const
	{
		EXEC_FUNCTION( GetSamplerParameteriv, sampler, uint32_t( pname ), params );
	}

	void OpenGl::SetSamplerParameter( uint32_t sampler, GlSamplerParameter pname, const uint32_t * params )const
	{
		REQUIRE( pname != GlSamplerParameter::eMaxAnisotropy || m_bHasAnisotropic );
		EXEC_FUNCTION( SamplerParameteruiv, sampler, uint32_t( pname ), params );
	}

	void OpenGl::SetSamplerParameter( uint32_t sampler, GlSamplerParameter pname, float param )const
	{
		REQUIRE( pname != GlSamplerParameter::eMaxAnisotropy || m_bHasAnisotropic );
		EXEC_FUNCTION( SamplerParameterf, sampler, uint32_t( pname ), param );
	}

	void OpenGl::SetSamplerParameter( uint32_t sampler, GlSamplerParameter pname, const float * params )const
	{
		REQUIRE( pname != GlSamplerParameter::eMaxAnisotropy || m_bHasAnisotropic );
		EXEC_FUNCTION( SamplerParameterfv, sampler, uint32_t( pname ), params );
	}

	void OpenGl::SetSamplerParameter( uint32_t sampler, GlSamplerParameter pname, int param )const
	{
		REQUIRE( pname != GlSamplerParameter::eMaxAnisotropy || m_bHasAnisotropic );
		EXEC_FUNCTION( SamplerParameteri, sampler, uint32_t( pname ), param );
	}

	void OpenGl::SetSamplerParameter( uint32_t sampler, GlSamplerParameter pname, const int * params )const
	{
		REQUIRE( pname != GlSamplerParameter::eMaxAnisotropy || m_bHasAnisotropic );
		EXEC_FUNCTION( SamplerParameteriv, sampler, uint32_t( pname ), params );
	}

	void OpenGl::TexBuffer( GlTexDim p_target, GlInternal p_internalFormat, uint32_t buffer )const
	{
		EXEC_FUNCTION( TexBuffer, uint32_t( p_target ), uint32_t( p_internalFormat ), buffer );
	}

	void OpenGl::GenBuffers( int n, uint32_t * buffers )const
	{
		EXEC_FUNCTION( GenBuffers, n, buffers );
	}

	void OpenGl::DeleteBuffers( int n, uint32_t const * buffers )const
	{
		EXEC_FUNCTION( DeleteBuffers, n, buffers );
	}

	bool OpenGl::IsBuffer( uint32_t buffer )const
	{
		return EXEC_FUNCTION( IsBuffer, buffer ) != GL_FALSE;
	}

	void OpenGl::BindBuffer( GlBufferTarget p_target, uint32_t buffer )const
	{
		m_pBufFunctions->BindBuffer( p_target, buffer );
	}

	void OpenGl::BufferData( GlBufferTarget p_target, ptrdiff_t size, void const * data, GlBufferMode usage )const
	{
		m_pBufFunctions->BufferData( p_target, size, data, usage );
	}

	void OpenGl::BufferSubData( GlBufferTarget p_target, ptrdiff_t offset, ptrdiff_t size, void const * data )const
	{
		m_pBufFunctions->BufferSubData( p_target, offset, size, data );
	}

	void OpenGl::CopyBufferSubData( GlBufferTarget readtarget, GlBufferTarget writetarget, ptrdiff_t readoffset, ptrdiff_t writeoffset, ptrdiff_t size )const
	{
		m_pBufFunctions->CopyBufferSubData( readtarget, writetarget, readoffset, writeoffset, size );
	}

	void OpenGl::GetBufferParameter( GlBufferTarget p_target, GlBufferParameter pname, int * params )const
	{
		m_pBufFunctions->GetBufferParameter( p_target, pname, params );
	}

	void OpenGl::GetBufferParameter( GlBufferTarget p_target, GlBufferParameter pname, uint64_t * params )const
	{
		m_pBufFunctions->GetBufferParameter( p_target, pname, params );
	}

	void * OpenGl::MapBuffer( GlBufferTarget p_target, GlAccessType access )const
	{
		return m_pBufFunctions->MapBuffer( p_target, access );
	}

	void OpenGl::UnmapBuffer( GlBufferTarget p_target )const
	{
		m_pBufFunctions->UnmapBuffer( p_target );
	}

	void * OpenGl::MapBufferRange( GlBufferTarget p_target, ptrdiff_t offset, ptrdiff_t length, uint32_t access )const
	{
		return m_pBufFunctions->MapBufferRange( p_target, offset, length, access );
	}

	void OpenGl::FlushMappedBufferRange( GlBufferTarget p_target, ptrdiff_t offset, ptrdiff_t length )const
	{
		m_pBufFunctions->FlushMappedBufferRange( p_target, offset, length );
	}

	void OpenGl::BufferAddressRange( GlAddress pname, uint32_t index, uint64_t address, size_t length )const
	{
		m_pBufFunctions->BufferAddressRange( pname, index, address, length );
	}

	void OpenGl::VertexFormat( int size, GlType type, int stride )const
	{
		m_pBufFunctions->VertexFormat( size, type, stride );
	}

	void OpenGl::NormalFormat( GlType type, int stride )const
	{
		m_pBufFunctions->NormalFormat( type, stride );
	}

	void OpenGl::ColorFormat( int size, GlType type, int stride )const
	{
		m_pBufFunctions->ColorFormat( size, type, stride );
	}

	void OpenGl::IndexFormat( GlType type, int stride )const
	{
		m_pBufFunctions->IndexFormat( type, stride );
	}

	void OpenGl::TexCoordFormat( int size, GlType type, int stride )const
	{
		m_pBufFunctions->TexCoordFormat( size, type, stride );
	}

	void OpenGl::EdgeFlagFormat( int stride )const
	{
		m_pBufFunctions->EdgeFlagFormat( stride );
	}

	void OpenGl::SecondaryColorFormat( int size, GlType type, int stride )const
	{
		m_pBufFunctions->SecondaryColorFormat( size, type, stride );
	}

	void OpenGl::FogCoordFormat( uint32_t type, int stride )const
	{
		m_pBufFunctions->FogCoordFormat( type, stride );
	}

	void OpenGl::VertexAttribFormat( uint32_t index, int size, GlType type, bool normalized, int stride )const
	{
		m_pBufFunctions->VertexAttribFormat( index, size, type, normalized, stride );
	}

	void OpenGl::VertexAttribIFormat( uint32_t index, int size, GlType type, int stride )const
	{
		m_pBufFunctions->VertexAttribIFormat( index, size, type, stride );
	}

	void OpenGl::MakeBufferResident( GlBufferTarget target, GlAccessType access )const
	{
		m_pBufFunctions->MakeBufferResident( target, access );
	}

	void OpenGl::MakeBufferNonResident( GlBufferTarget target )const
	{
		m_pBufFunctions->MakeBufferNonResident( target );
	}

	bool OpenGl::IsBufferResident( GlBufferTarget target )const
	{
		return m_pBufFunctions->IsBufferResident( target );
	}

	void OpenGl::MakeNamedBufferResident( uint32_t buffer, GlAccessType access )const
	{
		m_pBufFunctions->MakeNamedBufferResident( buffer, access );
	}

	void OpenGl::MakeNamedBufferNonResident( uint32_t buffer )const
	{
		m_pBufFunctions->MakeNamedBufferNonResident( buffer );
	}

	bool OpenGl::IsNamedBufferResident( uint32_t buffer )const
	{
		return m_pBufFunctions->IsNamedBufferResident( buffer );
	}

	void OpenGl::GetNamedBufferParameter( uint32_t buffer, GlBufferParameter pname, uint64_t * params )const
	{
		m_pBufFunctions->GetNamedBufferParameter( buffer, pname, params );
	}

	void OpenGl::GenTransformFeedbacks( int n, uint32_t * buffers )const
	{
		EXEC_FUNCTION( GenTransformFeedbacks, n, buffers );
	}

	void OpenGl::DeleteTransformFeedbacks( int n, uint32_t const * buffers )const
	{
		EXEC_FUNCTION( DeleteTransformFeedbacks, n, buffers );
	}

	void OpenGl::BindTransformFeedback( GlBufferTarget target, uint32_t buffer )const
	{
		EXEC_FUNCTION( BindTransformFeedback, target, buffer );
	}

	bool OpenGl::IsTransformFeedback( uint32_t buffer )const
	{
		return EXEC_FUNCTION( IsTransformFeedback, buffer ) != GL_FALSE;
	}

	void OpenGl::BeginTransformFeedback( GlTopology primitive )const
	{
		EXEC_FUNCTION( BeginTransformFeedback, uint32_t( primitive ) );
	}

	void OpenGl::PauseTransformFeedback()const
	{
		EXEC_VOID_FUNCTION( PauseTransformFeedback );
	}

	void OpenGl::ResumeTransformFeedback()const
	{
		EXEC_VOID_FUNCTION( ResumeTransformFeedback );
	}

	void OpenGl::EndTransformFeedback()const
	{
		EXEC_VOID_FUNCTION( EndTransformFeedback );
	}

	void OpenGl::TransformFeedbackVaryings( uint32_t program, int count, char const ** varyings, GlAttributeLayout bufferMode )const
	{
		EXEC_FUNCTION( TransformFeedbackVaryings, program, count, varyings, bufferMode );
	}

	void OpenGl::DrawTransformFeedback( GlTopology mode, uint32_t id )const
	{
		EXEC_FUNCTION( DrawTransformFeedback, uint32_t( mode ), id );
	}

	void OpenGl::GenFramebuffers( int n, uint32_t * framebuffers )const
	{
		EXEC_FUNCTION( GenFramebuffers, n, framebuffers );
	}

	void OpenGl::DeleteFramebuffers( int n, uint32_t const * framebuffers )const
	{
		EXEC_FUNCTION( DeleteFramebuffers, n, framebuffers );
	}

	bool OpenGl::IsFramebuffer( uint32_t framebuffer )const
	{
		return EXEC_FUNCTION( IsFramebuffer, framebuffer ) != GL_FALSE;
	}

	void OpenGl::BindFramebuffer( GlFrameBufferMode p_eBindingMode, uint32_t framebuffer )const
	{
		EXEC_FUNCTION( BindFramebuffer, uint32_t( p_eBindingMode ), framebuffer );
	}

	void OpenGl::FramebufferTexture( GlFrameBufferMode p_target, GlAttachmentPoint p_eAttachment, uint32_t texture, int level )const
	{
		EXEC_FUNCTION( FramebufferTexture, uint32_t( p_target ), uint32_t( p_eAttachment ), texture, level );
	}

	void OpenGl::FramebufferTexture1D( GlFrameBufferMode p_target, GlAttachmentPoint p_eAttachment, GlTexDim p_texTarget, uint32_t texture, int level )const
	{
		EXEC_FUNCTION( FramebufferTexture1D, uint32_t( p_target ), uint32_t( p_eAttachment ), uint32_t( p_texTarget ), texture, level );
	}

	void OpenGl::FramebufferTexture2D( GlFrameBufferMode p_target, GlAttachmentPoint p_eAttachment, GlTexDim p_texTarget, uint32_t texture, int level )const
	{
		EXEC_FUNCTION( FramebufferTexture2D, uint32_t( p_target ), uint32_t( p_eAttachment ), uint32_t( p_texTarget ), texture, level );
	}

	void OpenGl::FramebufferTexture3D( GlFrameBufferMode p_target, GlAttachmentPoint p_eAttachment, GlTexDim p_texTarget, uint32_t texture, int level, int layer )const
	{
		EXEC_FUNCTION( FramebufferTexture3D, uint32_t( p_target ), uint32_t( p_eAttachment ), uint32_t( p_texTarget ), texture, level, layer );
	}

	void OpenGl::FramebufferTextureLayer( GlFrameBufferMode p_target, GlAttachmentPoint p_eAttachment, uint32_t texture, int level, int layer )const
	{
		EXEC_FUNCTION( FramebufferTextureLayer, uint32_t( p_target ), uint32_t( p_eAttachment ), texture, level, layer );
	}

	void OpenGl::FramebufferRenderbuffer( GlFrameBufferMode p_target, GlAttachmentPoint p_eAttachment, GlRenderBufferMode p_eRenderbufferTarget, uint32_t renderbufferId )const
	{
		EXEC_FUNCTION( FramebufferRenderbuffer, uint32_t( p_target ), uint32_t( p_eAttachment ), uint32_t( p_eRenderbufferTarget ), renderbufferId );
	}

	void OpenGl::GenRenderbuffers( int n, uint32_t * ids )const
	{
		EXEC_FUNCTION( GenRenderbuffers, n, ids );
	}

	void OpenGl::DeleteRenderbuffers( int n, uint32_t const * ids )const
	{
		EXEC_FUNCTION( DeleteRenderbuffers, n, ids );
	}

	bool OpenGl::IsRenderbuffer( uint32_t framebuffer )const
	{
		return EXEC_FUNCTION( IsRenderbuffer, framebuffer ) != GL_FALSE;
	}

	void OpenGl::BindRenderbuffer( GlRenderBufferMode p_eBindingMode, uint32_t id )const
	{
		EXEC_FUNCTION( BindRenderbuffer, uint32_t( p_eBindingMode ), id );
	}

	void OpenGl::RenderbufferStorage( GlRenderBufferMode p_eBindingMode, GlInternal p_format, int width, int height )const
	{
		EXEC_FUNCTION( RenderbufferStorage, uint32_t( p_eBindingMode ), uint32_t( p_format ), width, height );
	}

	void OpenGl::RenderbufferStorageMultisample( GlRenderBufferMode p_eBindingMode, int p_iSamples, GlInternal p_format, int width, int height )const
	{
		int l_iMaxSamples;
		int l_iMaxSize;
		OpenGl::GetIntegerv( GlMax::eSamples, &l_iMaxSamples );
		OpenGl::GetIntegerv( GlMax::eRenderbufferSize, &l_iMaxSize );

		if ( p_iSamples <= l_iMaxSamples && width <= l_iMaxSize && height < l_iMaxSize )
		{
			EXEC_FUNCTION( RenderbufferStorageMultisample, uint32_t( p_eBindingMode ), p_iSamples, uint32_t( p_format ), width, height );
		}
		else if ( p_iSamples > l_iMaxSamples )
		{
			Castor::Logger::LogWarning( cuT( "glRenderbufferStorageMultisample - Asked for more samples than available, setting it to max available" ) );
			EXEC_FUNCTION( RenderbufferStorageMultisample, uint32_t( p_eBindingMode ), l_iMaxSamples, uint32_t( p_format ), width, height );
		}
		else if ( width > l_iMaxSize )
		{
			Castor::Logger::LogError( cuT( "glRenderbufferStorageMultisample - Asked for greater width than available" ) );
		}
		else if ( height > l_iMaxSize )
		{
			Castor::Logger::LogError( cuT( "glRenderbufferStorageMultisample - Asked for greater height than available" ) );
		}
	}

	void OpenGl::RenderbufferStorage( GlRenderBufferMode p_eBindingMode, GlInternal p_format, Castor::Size const & size )const
	{
		EXEC_FUNCTION( RenderbufferStorage, uint32_t( p_eBindingMode ), uint32_t( p_format ), int( size.width() ), int( size.height() ) );
	}

	void OpenGl::RenderbufferStorageMultisample( GlRenderBufferMode p_eBindingMode, int p_iSamples, GlInternal p_format, Castor::Size const & size )const
	{
		int l_iMaxSamples;
		int l_iMaxSize;
		OpenGl::GetIntegerv( GlMax::eSamples, &l_iMaxSamples );
		OpenGl::GetIntegerv( GlMax::eRenderbufferSize, &l_iMaxSize );

		if ( p_iSamples <= l_iMaxSamples && int( size.width() ) <= l_iMaxSize && int( size.height() ) < l_iMaxSize )
		{
			EXEC_FUNCTION( RenderbufferStorageMultisample, uint32_t( p_eBindingMode ), p_iSamples, uint32_t( p_format ), int( size.width() ), int( size.height() ) );
		}
		else if ( p_iSamples > l_iMaxSamples )
		{
			Castor::Logger::LogWarning( cuT( "glRenderbufferStorageMultisample - Asked for more samples than available, setting it to max available" ) );
			EXEC_FUNCTION( RenderbufferStorageMultisample, uint32_t( p_eBindingMode ), l_iMaxSamples, uint32_t( p_format ), int( size.width() ), int( size.height() ) );
		}
		else if ( int( size.width() ) > l_iMaxSize )
		{
			Castor::Logger::LogError( cuT( "glRenderbufferStorageMultisample - Asked for greater width than available" ) );
		}
		else if ( int( size.height() ) > l_iMaxSize )
		{
			Castor::Logger::LogError( cuT( "glRenderbufferStorageMultisample - Asked for greater height than available" ) );
		}
	}

	void OpenGl::GetRenderbufferParameteriv( GlRenderBufferMode p_eBindingMode, GlRenderBufferParameter p_param, int * values )const
	{
		EXEC_FUNCTION( GetRenderbufferParameteriv, uint32_t( p_eBindingMode ), uint32_t( p_param ), values );
	}

	void OpenGl::BlitFramebuffer( int srcX0, int srcY0, int srcX1, int srcY1, int dstX0, int dstY0, int dstX1, int dstY1, uint32_t mask, GlInterpolationMode filter )const
	{
		EXEC_FUNCTION( BlitFramebuffer, srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, uint32_t( filter ) );
	}

	void OpenGl::BlitFramebuffer( Castor::Rectangle const & rcSrc, Castor::Rectangle const & rcDst, uint32_t mask, GlInterpolationMode filter )const
	{
		EXEC_FUNCTION( BlitFramebuffer, rcSrc.left(), rcSrc.top(), rcSrc.right(), rcSrc.bottom(), rcDst.left(), rcDst.top(), rcDst.right(), rcDst.bottom(), mask, uint32_t( filter ) );
	}

	void OpenGl::BlitFramebuffer( int srcX0, int srcY0, int srcX1, int srcY1, int dstX0, int dstY0, int dstX1, int dstY1, GlComponent mask, GlInterpolationMode filter )const
	{
		EXEC_FUNCTION( BlitFramebuffer, srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, uint32_t( mask ), uint32_t( filter ) );
	}

	void OpenGl::BlitFramebuffer( Castor::Rectangle const & rcSrc, Castor::Rectangle const & rcDst, GlComponent mask, GlInterpolationMode filter )const
	{
		EXEC_FUNCTION( BlitFramebuffer, rcSrc.left(), rcSrc.top(), rcSrc.right(), rcSrc.bottom(), rcDst.left(), rcDst.top(), rcDst.right(), rcDst.bottom(), uint32_t( mask ), uint32_t( filter ) );
	}

	void OpenGl::DrawBuffers( int n, const uint32_t * bufs )const
	{
		EXEC_FUNCTION( DrawBuffers, n, bufs );
	}

	GlFramebufferStatus OpenGl::CheckFramebufferStatus( GlFrameBufferMode p_target )const
	{
		return GlFramebufferStatus( EXEC_FUNCTION( CheckFramebufferStatus, uint32_t( p_target ) ) );
	}

	void OpenGl::TexImage2DMultisample( GlTextureStorageType p_target, int p_iSamples, GlInternal p_internalFormat, int p_iWidth, int p_iHeight, bool p_bFixedSampleLocations )const
	{
		EXEC_FUNCTION( TexImage2DMultisample, uint32_t( p_target ), p_iSamples, int( p_internalFormat ), p_iWidth, p_iHeight, uint8_t( p_bFixedSampleLocations ) );
	}

	void OpenGl::TexImage2DMultisample( GlTextureStorageType p_target, int p_iSamples, GlInternal p_internalFormat, Castor::Size const & p_size, bool p_bFixedSampleLocations )const
	{
		EXEC_FUNCTION( TexImage2DMultisample, uint32_t( p_target ), p_iSamples, int( p_internalFormat ), int( p_size.width() ), int( p_size.height() ), uint8_t( p_bFixedSampleLocations ) );
	}

	int OpenGl::GetUniformLocation( uint32_t program, char const * name )const
	{
		return EXEC_FUNCTION( GetUniformLocation, program, name );
	}

	void OpenGl::SetUniform( int location, int v0 )const
	{
		EXEC_FUNCTION( Uniform1i, location, v0 );
	}

	void OpenGl::SetUniform( int location, int v0, int v1 )const
	{
		EXEC_FUNCTION( Uniform2i, location, v0, v1 );
	}

	void OpenGl::SetUniform( int location, int v0, int v1, int v2 )const
	{
		EXEC_FUNCTION( Uniform3i, location, v0, v1, v2 );
	}

	void OpenGl::SetUniform( int location, int v0, int v1, int v2, int v3 )const
	{
		EXEC_FUNCTION( Uniform4i, location, v0, v1, v2, v3 );
	}

	void OpenGl::SetUniform1v( int location, int count, int const * params )const
	{
		EXEC_FUNCTION( Uniform1iv, location, count, params );
	}

	void OpenGl::SetUniform2v( int location, int count, int const * params )const
	{
		EXEC_FUNCTION( Uniform2iv, location, count, params );
	}

	void OpenGl::SetUniform3v( int location, int count, int const * params )const
	{
		EXEC_FUNCTION( Uniform3iv, location, count, params );
	}

	void OpenGl::SetUniform4v( int location, int count, int const * params )const
	{
		EXEC_FUNCTION( Uniform4iv, location, count, params );
	}

	void OpenGl::SetUniform( int location, uint32_t v0 )const
	{
		EXEC_FUNCTION( Uniform1ui, location, v0 );
	}

	void OpenGl::SetUniform( int location, uint32_t v0, uint32_t v1 )const
	{
		EXEC_FUNCTION( Uniform2ui, location, v0, v1 );
	}

	void OpenGl::SetUniform( int location, uint32_t v0, uint32_t v1, uint32_t v2 )const
	{
		EXEC_FUNCTION( Uniform3ui, location, v0, v1, v2 );
	}

	void OpenGl::SetUniform( int location, uint32_t v0, uint32_t v1, uint32_t v2, uint32_t v3 )const
	{
		EXEC_FUNCTION( Uniform4ui, location, v0, v1, v2, v3 );
	}

	void OpenGl::SetUniform1v( int location, int count, uint32_t const * params )const
	{
		EXEC_FUNCTION( Uniform1uiv, location, count, params );
	}

	void OpenGl::SetUniform2v( int location, int count, uint32_t const * params )const
	{
		EXEC_FUNCTION( Uniform2uiv, location, count, params );
	}

	void OpenGl::SetUniform3v( int location, int count, uint32_t const * params )const
	{
		EXEC_FUNCTION( Uniform3uiv, location, count, params );
	}

	void OpenGl::SetUniform4v( int location, int count, uint32_t const * params )const
	{
		EXEC_FUNCTION( Uniform4uiv, location, count, params );
	}

	void OpenGl::SetUniform( int location, float v0 )const
	{
		EXEC_FUNCTION( Uniform1f, location, v0 );
	}

	void OpenGl::SetUniform( int location, float v0, float v1 )const
	{
		EXEC_FUNCTION( Uniform2f, location, v0, v1 );
	}

	void OpenGl::SetUniform( int location, float v0, float v1, float v2 )const
	{
		EXEC_FUNCTION( Uniform3f, location, v0, v1, v2 );
	}

	void OpenGl::SetUniform( int location, float v0, float v1, float v2, float v3 )const
	{
		EXEC_FUNCTION( Uniform4f, location, v0, v1, v2, v3 );
	}

	void OpenGl::SetUniform1v( int location, int count, float const * params )const
	{
		EXEC_FUNCTION( Uniform1fv, location, count, params );
	}

	void OpenGl::SetUniform2v( int location, int count, float const * params )const
	{
		EXEC_FUNCTION( Uniform2fv, location, count, params );
	}

	void OpenGl::SetUniform3v( int location, int count, float const * params )const
	{
		EXEC_FUNCTION( Uniform3fv, location, count, params );
	}

	void OpenGl::SetUniform4v( int location, int count, float const * params )const
	{
		EXEC_FUNCTION( Uniform4fv, location, count, params );
	}

	void OpenGl::SetUniform( int location, double v0 )const
	{
		EXEC_FUNCTION( Uniform1d, location, v0 );
	}

	void OpenGl::SetUniform( int location, double v0, double v1 )const
	{
		EXEC_FUNCTION( Uniform2d, location, v0, v1 );
	}

	void OpenGl::SetUniform( int location, double v0, double v1, double v2 )const
	{
		EXEC_FUNCTION( Uniform3d, location, v0, v1, v2 );
	}

	void OpenGl::SetUniform( int location, double v0, double v1, double v2, double v3 )const
	{
		EXEC_FUNCTION( Uniform4d, location, v0, v1, v2, v3 );
	}

	void OpenGl::SetUniform1v( int location, int count, double const * params )const
	{
		EXEC_FUNCTION( Uniform1dv, location, count, params );
	}

	void OpenGl::SetUniform2v( int location, int count, double const * params )const
	{
		EXEC_FUNCTION( Uniform2dv, location, count, params );
	}

	void OpenGl::SetUniform3v( int location, int count, double const * params )const
	{
		EXEC_FUNCTION( Uniform3dv, location, count, params );
	}

	void OpenGl::SetUniform4v( int location, int count, double const * params )const
	{
		EXEC_FUNCTION( Uniform4dv, location, count, params );
	}

	void OpenGl::SetUniform2x2v( int location, int count, bool transpose, float const * value )const
	{
		EXEC_FUNCTION( UniformMatrix2fv, location, count, uint8_t( transpose ), value );
	}

	void OpenGl::SetUniform2x3v( int location, int count, bool transpose, float const * value )const
	{
		EXEC_FUNCTION( UniformMatrix2x3fv, location, count, uint8_t( transpose ), value );
	}

	void OpenGl::SetUniform2x4v( int location, int count, bool transpose, float const * value )const
	{
		EXEC_FUNCTION( UniformMatrix2x4fv, location, count, uint8_t( transpose ), value );
	}

	void OpenGl::SetUniform3x2v( int location, int count, bool transpose, float const * value )const
	{
		EXEC_FUNCTION( UniformMatrix3x2fv, location, count, uint8_t( transpose ), value );
	}

	void OpenGl::SetUniform3x3v( int location, int count, bool transpose, float const * value )const
	{
		EXEC_FUNCTION( UniformMatrix3fv, location, count, uint8_t( transpose ), value );
	}

	void OpenGl::SetUniform3x4v( int location, int count, bool transpose, float const * value )const
	{
		EXEC_FUNCTION( UniformMatrix3x4fv, location, count, uint8_t( transpose ), value );
	}

	void OpenGl::SetUniform4x2v( int location, int count, bool transpose, float const * value )const
	{
		EXEC_FUNCTION( UniformMatrix4x2fv, location, count, uint8_t( transpose ), value );
	}

	void OpenGl::SetUniform4x3v( int location, int count, bool transpose, float const * value )const
	{
		EXEC_FUNCTION( UniformMatrix4x3fv, location, count, uint8_t( transpose ), value );
	}

	void OpenGl::SetUniform4x4v( int location, int count, bool transpose, float const * value )const
	{
		EXEC_FUNCTION( UniformMatrix4fv, location, count, uint8_t( transpose ), value );
	}

	void OpenGl::SetUniform2x2v( int location, int count, bool transpose, double const * value )const
	{
		EXEC_FUNCTION( UniformMatrix2dv, location, count, uint8_t( transpose ), value );
	}

	void OpenGl::SetUniform2x3v( int location, int count, bool transpose, double const * value )const
	{
		EXEC_FUNCTION( UniformMatrix2x3dv, location, count, uint8_t( transpose ), value );
	}

	void OpenGl::SetUniform2x4v( int location, int count, bool transpose, double const * value )const
	{
		EXEC_FUNCTION( UniformMatrix2x4dv, location, count, uint8_t( transpose ), value );
	}

	void OpenGl::SetUniform3x2v( int location, int count, bool transpose, double const * value )const
	{
		EXEC_FUNCTION( UniformMatrix3x2dv, location, count, uint8_t( transpose ), value );
	}

	void OpenGl::SetUniform3x3v( int location, int count, bool transpose, double const * value )const
	{
		EXEC_FUNCTION( UniformMatrix3dv, location, count, uint8_t( transpose ), value );
	}

	void OpenGl::SetUniform3x4v( int location, int count, bool transpose, double const * value )const
	{
		EXEC_FUNCTION( UniformMatrix3x4dv, location, count, uint8_t( transpose ), value );
	}

	void OpenGl::SetUniform4x2v( int location, int count, bool transpose, double const * value )const
	{
		EXEC_FUNCTION( UniformMatrix4x2dv, location, count, uint8_t( transpose ), value );
	}

	void OpenGl::SetUniform4x3v( int location, int count, bool transpose, double const * value )const
	{
		EXEC_FUNCTION( UniformMatrix4x3dv, location, count, uint8_t( transpose ), value );
	}

	void OpenGl::SetUniform4x4v( int location, int count, bool transpose, double const * value )const
	{
		EXEC_FUNCTION( UniformMatrix4dv, location, count, uint8_t( transpose ), value );
	}

	uint32_t OpenGl::GetUniformBlockIndex( uint32_t program, char const * uniformBlockName )const
	{
		return EXEC_FUNCTION( GetUniformBlockIndex, program, uniformBlockName );
	}

	void OpenGl::BindBufferBase( GlBufferTarget target, uint32_t index, uint32_t buffer )const
	{
		EXEC_FUNCTION( BindBufferBase, uint32_t( target ), index, buffer );
	}

	void OpenGl::UniformBlockBinding( uint32_t program, uint32_t uniformBlockIndex, uint32_t uniformBlockBinding )const
	{
		EXEC_FUNCTION( UniformBlockBinding, program, uniformBlockIndex, uniformBlockBinding );
	}

	void OpenGl::GetUniformIndices( uint32_t program, int uniformCount, char const ** uniformNames, uint32_t * uniformIndices )const
	{
		EXEC_FUNCTION( GetUniformIndices, program, uniformCount, uniformNames, uniformIndices );
	}

	void OpenGl::GetActiveUniformsiv( uint32_t program, int uniformCount, uint32_t const * uniformIndices, GlUniformValue pname, int * params )const
	{
		EXEC_FUNCTION( GetActiveUniformsiv, program, uniformCount, uniformIndices, uint32_t( pname ), params );
	}

	void OpenGl::GetActiveUniformBlockiv( uint32_t program, uint32_t uniformBlockIndex, GlUniformBlockValue pname, int * params )const
	{
		EXEC_FUNCTION( GetActiveUniformBlockiv, program, uniformBlockIndex, uint32_t( pname ), params );
	}

	uint32_t OpenGl::CreateShader( GlShaderType type )const
	{
		return EXEC_FUNCTION( CreateShader, uint32_t( type ) );
	}

	void OpenGl::DeleteShader( uint32_t shader )const
	{
		EXEC_FUNCTION( DeleteShader, shader );
	}

	bool OpenGl::IsShader( uint32_t shader )const
	{
		return EXEC_FUNCTION( IsShader, shader ) != GL_FALSE;
	}

	void OpenGl::AttachShader( uint32_t program, uint32_t shader )const
	{
		EXEC_FUNCTION( AttachShader, program, shader );
	}

	void OpenGl::DetachShader( uint32_t program, uint32_t shader )const
	{
		EXEC_FUNCTION( DetachShader, program, shader );
	}

	void OpenGl::CompileShader( uint32_t shader )const
	{
		EXEC_FUNCTION( CompileShader, shader );
	}

	void OpenGl::GetShaderiv( uint32_t shader, GlShaderStatus pname, int * param )const
	{
		EXEC_FUNCTION( GetShaderiv, shader, uint32_t( pname ), param );
	}

	void OpenGl::GetShaderInfoLog( uint32_t shader, int bufSize, int * length, char * infoLog )const
	{
		EXEC_FUNCTION( GetShaderInfoLog, shader, bufSize, length, infoLog );
	}

	void OpenGl::ShaderSource( uint32_t shader, int count, char const ** strings, int const * lengths )const
	{
		EXEC_FUNCTION( ShaderSource, shader, count, strings, lengths );
	}

	void OpenGl::GetActiveAttrib( uint32_t program, uint32_t index, int bufSize, int * length, int * size, uint32_t * type, char * name )const
	{
		EXEC_FUNCTION( GetActiveAttrib, program, index, bufSize, length, size, type, name );
	}

	void OpenGl::GetActiveUniform( uint32_t program, uint32_t index, int bufSize, int * length, int * size, uint32_t * type, char * name )const
	{
		EXEC_FUNCTION( GetActiveUniform, program, index, bufSize, length, size, type, name );
	}

	uint32_t OpenGl::CreateProgram()const
	{
		return EXEC_VOID_FUNCTION( CreateProgram );
	}

	void OpenGl::DeleteProgram( uint32_t program )const
	{
		EXEC_FUNCTION( DeleteProgram, program );
	}

	bool OpenGl::IsProgram( uint32_t program )const
	{
		return EXEC_FUNCTION( IsProgram, program ) != GL_FALSE;
	}

	void OpenGl::LinkProgram( uint32_t program )const
	{
		EXEC_FUNCTION( LinkProgram, program );
	}

	void OpenGl::UseProgram( uint32_t program )const
	{
		EXEC_FUNCTION( UseProgram, program );
	}

	void OpenGl::GetProgramiv( uint32_t program, GlShaderStatus pname, int * param )const
	{
		EXEC_FUNCTION( GetProgramiv, program, uint32_t( pname ), param );
	}

	void OpenGl::GetProgramInfoLog( uint32_t program, int bufSize, int * length, char * infoLog )const
	{
		EXEC_FUNCTION( GetProgramInfoLog, program, bufSize, length, infoLog );
	}

	int OpenGl::GetAttribLocation( uint32_t program, char const * name )const
	{
		return EXEC_FUNCTION( GetAttribLocation, program, name );
	}

	void OpenGl::ProgramParameteri( uint32_t program, uint32_t pname, int value )const
	{
		EXEC_FUNCTION( ProgramParameteri, program, pname, value );
	}

	void OpenGl::DispatchCompute( uint32_t num_groups_x, uint32_t num_groups_y, uint32_t num_groups_z )const
	{
		EXEC_FUNCTION( DispatchCompute, num_groups_x, num_groups_y, num_groups_z );
	}

	void OpenGl::DispatchComputeGroupSize( uint32_t num_groups_x, uint32_t num_groups_y, uint32_t num_groups_z, uint32_t work_group_size_x, uint32_t work_group_size_y, uint32_t work_group_size_z )const
	{
		EXEC_FUNCTION( DispatchComputeGroupSize, num_groups_x, num_groups_y, num_groups_z, work_group_size_x, work_group_size_y, work_group_size_z );
	}

	void OpenGl::ShaderStorageBlockBinding( uint32_t shader, uint32_t storageBlockIndex, uint32_t storageBlockBinding )const
	{
		EXEC_FUNCTION( ShaderStorageBlockBinding, shader, storageBlockIndex, storageBlockBinding );
	}

	void OpenGl::GetProgramInterfaceInfos( uint32_t program, GlslInterface programInterface, GlslDataName name, int * params )
	{
		EXEC_FUNCTION( GetProgramInterfaceiv, program, uint32_t( programInterface ), uint32_t( name ), params );
	}

	int OpenGl::GetProgramResourceIndex( uint32_t program, GlslInterface programInterface, char const * const name )
	{
		return EXEC_FUNCTION( GetProgramResourceIndex, program, uint32_t( programInterface ), name );
	}

	int OpenGl::GetProgramResourceLocation( uint32_t program, GlslInterface programInterface, char const * const name )
	{
		return EXEC_FUNCTION( GetProgramResourceLocation, program, uint32_t( programInterface ), name );
	}

	int OpenGl::GetProgramResourceLocationIndex( uint32_t program, GlslInterface programInterface, char const * const name )
	{
		return EXEC_FUNCTION( GetProgramResourceLocationIndex, program, uint32_t( programInterface ), name );
	}

	void OpenGl::GetProgramResourceName( uint32_t program, GlslInterface programInterface, uint32_t index, int bufSize, int * length, char * name )
	{
		EXEC_FUNCTION( GetProgramResourceName, program, uint32_t( programInterface ), index, bufSize, length, name );
	}

	void OpenGl::GetProgramResourceInfos( uint32_t program, GlslInterface programInterface, uint32_t index, int propCount, uint32_t * props, int bufSize, int * length, int * params )
	{
		EXEC_FUNCTION( GetProgramResourceiv, program, uint32_t( programInterface ), index, propCount, props, bufSize, length, params );
	}

	void OpenGl::MemoryBarrier( Castor::FlagCombination< GlBarrierBit > const & barriers )const
	{
		EXEC_FUNCTION( MemoryBarrier, uint32_t( barriers ) );
	}

	void OpenGl::MemoryBarrierByRegion( Castor::FlagCombination< GlBarrierBit > const & barriers )const
	{
		EXEC_FUNCTION( MemoryBarrierByRegion, uint32_t( barriers ) );
	}

	void OpenGl::EnableVertexAttribArray( uint32_t index )const
	{
		EXEC_FUNCTION( EnableVertexAttribArray, index );
	}

	void OpenGl::VertexAttribPointer( uint32_t index, int size, GlType type, bool normalized, int stride, void const * pointer )const
	{
		EXEC_FUNCTION( VertexAttribPointer, index, size, uint32_t( type ), uint8_t( normalized ), stride, pointer );
	}

	void OpenGl::VertexAttribPointer( uint32_t index, int size, GlType type, int stride, void const * pointer )const
	{
		EXEC_FUNCTION( VertexAttribIPointer, index, size, uint32_t( type ), stride, pointer );
	}

	void OpenGl::DisableVertexAttribArray( uint32_t index )const
	{
		EXEC_FUNCTION( DisableVertexAttribArray, index );
	}

	void OpenGl::GenVertexArrays( int n, uint32_t * arrays )const
	{
		EXEC_FUNCTION( GenVertexArrays, n, arrays );
	}

	bool OpenGl::IsVertexArray( uint32_t array )const
	{
		return EXEC_FUNCTION( IsVertexArray, array ) != GL_FALSE;
	}

	void OpenGl::BindVertexArray( uint32_t array )const
	{
		EXEC_FUNCTION( BindVertexArray, array );
	}

	void OpenGl::DeleteVertexArrays( int n, uint32_t const * arrays )const
	{
		EXEC_FUNCTION( DeleteVertexArrays, n, arrays );
	}

	void OpenGl::PatchParameter( GlPatchParameter p_param, int p_iValue )const
	{
		EXEC_FUNCTION( PatchParameteri, uint32_t( p_param ), p_iValue );
	}

	void OpenGl::GenQueries( int p_n, uint32_t * p_queries )const
	{
		EXEC_FUNCTION( GenQueries, p_n, p_queries );
	}

	void OpenGl::DeleteQueries( int p_n, uint32_t const * p_queries )const
	{
		EXEC_FUNCTION( DeleteQueries, p_n, p_queries );
	}

	bool OpenGl::IsQuery( uint32_t p_query )const
	{
		return EXEC_FUNCTION( IsQuery, p_query ) != GL_FALSE;
	}

	void OpenGl::BeginQuery( GlQueryType p_target, uint32_t p_query )const
	{
		EXEC_FUNCTION( BeginQuery, uint32_t( p_target ), p_query );
	}

	void OpenGl::EndQuery( GlQueryType p_target )const
	{
		EXEC_FUNCTION( EndQuery, uint32_t( p_target ) );
	}

	void OpenGl::QueryCounter( uint32_t p_id, GlQueryType p_target )const
	{
		EXEC_FUNCTION( QueryCounter, p_id, uint32_t( p_target ) );
	}

	void OpenGl::GetQueryObjectInfos( uint32_t p_id, GlQueryInfo p_name, int32_t * p_params )const
	{
		EXEC_FUNCTION( GetQueryObjectiv, p_id, uint32_t( p_name ), p_params );
	}

	void OpenGl::GetQueryObjectInfos( uint32_t p_id, GlQueryInfo p_name, uint32_t * p_params )const
	{
		EXEC_FUNCTION( GetQueryObjectuiv, p_id, uint32_t( p_name ), p_params );
	}

	void OpenGl::GetQueryObjectInfos( uint32_t p_id, GlQueryInfo p_name, int64_t * p_params )const
	{
		EXEC_FUNCTION( GetQueryObjecti64v, p_id, uint32_t( p_name ), p_params );
	}

	void OpenGl::GetQueryObjectInfos( uint32_t p_id, GlQueryInfo p_name, uint64_t * p_params )const
	{
		EXEC_FUNCTION( GetQueryObjectui64v, p_id, uint32_t( p_name ), p_params );
	}

	//*************************************************************************************************
}
