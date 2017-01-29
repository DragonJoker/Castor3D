#include <Log/Logger.hpp>
#include <Graphics/Position.hpp>
#include <Graphics/Rectangle.hpp>
#include <Graphics/Size.hpp>
#include <Miscellaneous/Utils.hpp>

namespace GlES3Render
{
	template< typename RetT, typename ... ParamsT >
	struct GlES3FuncCaller
	{
		static inline RetT Call( OpenGlES3 const & p_gl, std::function< RetT( ParamsT ... ) > const & p_function, char const * const p_name, ParamsT const & ... p_params )
		{
			REQUIRE( p_function );
			RetT l_return = p_function( p_params... );
			glCheckError( p_gl, p_name );
			return l_return;
		}
	};

	template< typename ... ParamsT >
	struct GlES3FuncCaller< void, ParamsT... >
	{
		static inline void Call( OpenGlES3 const & p_gl, std::function< void( ParamsT ... ) > const & p_function, char const * const p_name, ParamsT const & ... p_params )
		{
			REQUIRE( p_function );
			p_function( p_params... );
			glCheckError( p_gl, p_name );
		}
	};

	template< typename RetT >
	struct GlES3FuncCaller< RetT, void >
	{
		static inline void Call( OpenGlES3 const & p_gl, std::function< RetT() > const & p_function, char const * const p_name )
		{
			REQUIRE( p_function );
			p_function();
			glCheckError( p_gl, p_name );
		}
	};

	template< typename RetT, typename ... ParamsT >
	inline RetT ExecuteFunction( OpenGlES3 const & p_gl, std::function< RetT( ParamsT ... ) > const & p_function, char const * const p_name, ParamsT const & ... p_params )
	{
		return GlES3FuncCaller< RetT, ParamsT... >::Call( p_gl, p_function, p_name, p_params... );
	}

#	define EXEC_FUNCTION( Name, ... )\
	ExecuteFunction( GetOpenGlES3(), m_pfn##Name, "gl"#Name, __VA_ARGS__ )

#	define EXEC_VOID_FUNCTION( Name )\
	ExecuteFunction( GetOpenGlES3(), m_pfn##Name, "gl"#Name )


	//*************************************************************************************************

	void TexFunctions::GenerateMipmap( GlES3TexDim mode )const
	{
		EXEC_FUNCTION( GenerateMipmap, uint32_t( mode ) );
	}

	void TexFunctions::BindTexture( GlES3TexDim mode, uint32_t texture )const
	{
		EXEC_FUNCTION( BindTexture, uint32_t( mode ), texture );
	}

	void TexFunctions::TexSubImage1D( GlES3TextureStorageType mode, int level, int xoffset, int width, GlES3Format format, GlES3Type type, void const * data )const
	{
		EXEC_FUNCTION( TexSubImage1D, uint32_t( mode ), level, xoffset, width, uint32_t( format ), uint32_t( type ), data );
	}

	void TexFunctions::TexSubImage2D( GlES3TextureStorageType mode, int level, int xoffset, int yoffset, int width, int height, GlES3Format format, GlES3Type type, void const * data )const
	{
		EXEC_FUNCTION( TexSubImage2D, uint32_t( mode ), level, xoffset, yoffset, width, height, uint32_t( format ), uint32_t( type ), data );
	}

	void TexFunctions::TexSubImage2D( GlES3TextureStorageType mode, int level, Castor::Position const & p_position, Castor::Size const & p_size, GlES3Format format, GlES3Type type, void const * data )const
	{
		EXEC_FUNCTION( TexSubImage2D, uint32_t( mode ), level, int( p_position.x() ), int( p_position.y() ), int( p_size.width() ), int( p_size.height() ), uint32_t( format ), uint32_t( type ), data );
	}

	void TexFunctions::TexSubImage2D( GlES3TextureStorageType mode, int level, Castor::Rectangle const & p_rect, GlES3Format format, GlES3Type type, void const * data )const
	{
		EXEC_FUNCTION( TexSubImage2D, uint32_t( mode ), level, p_rect.left(), p_rect.top(), p_rect.width(), p_rect.height(), uint32_t( format ), uint32_t( type ), data );
	}

	void TexFunctions::TexSubImage3D( GlES3TextureStorageType mode, int level, int xoffset, int yoffset, int zoffset, int width, int height, int depth, GlES3Format format, GlES3Type type, void const * data )const
	{
		EXEC_FUNCTION( TexSubImage3D, uint32_t( mode ), level, xoffset, yoffset, zoffset, width, height, depth, uint32_t( format ), uint32_t( type ), data );
	}

	void TexFunctions::TexImage1D( GlES3TextureStorageType mode, int level, GlES3Internal internalFormat, int width, int border, GlES3Format format, GlES3Type type, void const * data )const
	{
		EXEC_FUNCTION( TexImage1D, uint32_t( mode ), level, int( internalFormat ), width, border, uint32_t( format ), uint32_t( type ), data );
	}

	void TexFunctions::TexImage2D( GlES3TextureStorageType mode, int level, GlES3Internal internalFormat, int width, int height, int border, GlES3Format format, GlES3Type type, void const * data )const
	{
		EXEC_FUNCTION( TexImage2D, uint32_t( mode ), level, int( internalFormat ), width, height, border, uint32_t( format ), uint32_t( type ), data );
	}

	void TexFunctions::TexImage2D( GlES3TextureStorageType mode, int level, GlES3Internal internalFormat, Castor::Size const & p_size, int border, GlES3Format format, GlES3Type type, void const * data )const
	{
		EXEC_FUNCTION( TexImage2D, uint32_t( mode ), level, int( internalFormat ), int( p_size.width() ), int( p_size.height() ), border, uint32_t( format ), uint32_t( type ), data );
	}

	void TexFunctions::TexImage3D( GlES3TextureStorageType mode, int level, GlES3Internal internalFormat, int width, int height, int depth, int border, GlES3Format format, GlES3Type type, void const * data )const
	{
		EXEC_FUNCTION( TexImage3D, uint32_t( mode ), level, int( internalFormat ), width, height, depth, border, uint32_t( format ), uint32_t( type ), data );
	}

	void TexFunctions::GetTexImage( GlES3TextureStorageType mode, int level, GlES3Format format, GlES3Type type, void * img )const
	{
		EXEC_FUNCTION( GetTexImage, uint32_t( mode ), level, uint32_t( format ), uint32_t( type ), img );
	}

	//*************************************************************************************************

	void TexFunctionsDSA::GenerateMipmap( GlES3TexDim mode )const
	{
		EXEC_FUNCTION( GenerateMipmap, m_uiTexture, uint32_t( mode ) );
	}

	void TexFunctionsDSA::TexSubImage1D( GlES3TextureStorageType mode, int level, int xoffset, int width, GlES3Format format, GlES3Type type, void const * data )const
	{
		EXEC_FUNCTION( TextureSubImage1D, m_uiTexture, uint32_t( mode ), level, xoffset, width, uint32_t( format ), uint32_t( type ), data );
	}

	void TexFunctionsDSA::TexSubImage2D( GlES3TextureStorageType mode, int level, int xoffset, int yoffset, int width, int height, GlES3Format format, GlES3Type type, void const * data )const
	{
		EXEC_FUNCTION( TextureSubImage2D, m_uiTexture, uint32_t( mode ), level, xoffset, yoffset, width, height, uint32_t( format ), uint32_t( type ), data );
	}

	void TexFunctionsDSA::TexSubImage2D( GlES3TextureStorageType mode, int level, Castor::Position const & p_position, Castor::Size const & p_size, GlES3Format format, GlES3Type type, void const * data )const
	{
		EXEC_FUNCTION( TextureSubImage2D, m_uiTexture, uint32_t( mode ), level, int( p_position.x() ), int( p_position.y() ), int( p_size.width() ), int( p_size.height() ), uint32_t( format ), uint32_t( type ), data );
	}

	void TexFunctionsDSA::TexSubImage2D( GlES3TextureStorageType mode, int level, Castor::Rectangle const & p_rect, GlES3Format format, GlES3Type type, void const * data )const
	{
		EXEC_FUNCTION( TextureSubImage2D, m_uiTexture, uint32_t( mode ), level, p_rect.left(), p_rect.top(), p_rect.width(), p_rect.height(), uint32_t( format ), uint32_t( type ), data );
	}

	void TexFunctionsDSA::TexSubImage3D( GlES3TextureStorageType mode, int level, int xoffset, int yoffset, int zoffset, int width, int height, int depth, GlES3Format format, GlES3Type type, void const * data )const
	{
		EXEC_FUNCTION( TextureSubImage3D, m_uiTexture, uint32_t( mode ), level, xoffset, yoffset, zoffset, width, height, depth, uint32_t( format ), uint32_t( type ), data );
	}

	void TexFunctionsDSA::TexImage1D( GlES3TextureStorageType mode, int level, GlES3Internal internalFormat, int width, int border, GlES3Format format, GlES3Type type, void const * data )const
	{
		EXEC_FUNCTION( TextureImage1D, m_uiTexture, uint32_t( mode ), level, int( internalFormat ), width, border, uint32_t( format ), uint32_t( type ), data );
	}

	void TexFunctionsDSA::TexImage2D( GlES3TextureStorageType mode, int level, GlES3Internal internalFormat, int width, int height, int border, GlES3Format format, GlES3Type type, void const * data )const
	{
		EXEC_FUNCTION( TextureImage2D, m_uiTexture, uint32_t( mode ), level, int( internalFormat ), width, height, border, uint32_t( format ), uint32_t( type ), data );
	}

	void TexFunctionsDSA::TexImage2D( GlES3TextureStorageType mode, int level, GlES3Internal internalFormat, Castor::Size const & p_size, int border, GlES3Format format, GlES3Type type, void const * data )const
	{
		EXEC_FUNCTION( TextureImage2D, m_uiTexture, uint32_t( mode ), level, int( internalFormat ), int( p_size.width() ), int( p_size.height() ), border, uint32_t( format ), uint32_t( type ), data );
	}

	void TexFunctionsDSA::TexImage3D( GlES3TextureStorageType mode, int level, GlES3Internal internalFormat, int width, int height, int depth, int border, GlES3Format format, GlES3Type type, void const * data )const
	{
		EXEC_FUNCTION( TextureImage3D, m_uiTexture, uint32_t( mode ), level, int( internalFormat ), width, height, depth, border, uint32_t( format ), uint32_t( type ), data );
	}

	void TexFunctionsDSA::GetTexImage( GlES3TextureStorageType mode, int level, GlES3Format format, GlES3Type type, void * img )const
	{
		EXEC_FUNCTION( GetTextureImage, m_uiTexture, uint32_t( mode ), level, uint32_t( format ), uint32_t( type ), img );
	}

	//*************************************************************************************************

	void BufFunctionsBase::BufferAddressRange( GlES3Address pname, uint32_t index, uint64_t address, size_t length )const
	{
		EXEC_FUNCTION( BufferAddressRange, uint32_t( pname ), index, address, length );
	}

	void BufFunctionsBase::VertexFormat( int size, GlES3Type type, int stride )const
	{
		EXEC_FUNCTION( VertexFormat, size, uint32_t( type ), stride );
	}

	void BufFunctionsBase::NormalFormat( GlES3Type type, int stride )const
	{
		EXEC_FUNCTION( NormalFormat, uint32_t( type ), stride );
	}

	void BufFunctionsBase::ColorFormat( int size, GlES3Type type, int stride )const
	{
		EXEC_FUNCTION( ColorFormat, size, uint32_t( type ), stride );
	}

	void BufFunctionsBase::IndexFormat( GlES3Type type, int stride )const
	{
		EXEC_FUNCTION( IndexFormat, uint32_t( type ), stride );
	}

	void BufFunctionsBase::TexCoordFormat( int size, GlES3Type type, int stride )const
	{
		EXEC_FUNCTION( TexCoordFormat, size, uint32_t( type ), stride );
	}

	void BufFunctionsBase::EdgeFlagFormat( int stride )const
	{
		EXEC_FUNCTION( EdgeFlagFormat, stride );
	}

	void BufFunctionsBase::SecondaryColorFormat( int size, GlES3Type type, int stride )const
	{
		EXEC_FUNCTION( SecondaryColorFormat, size, uint32_t( type ), stride );
	}

	void BufFunctionsBase::FogCoordFormat( uint32_t type, int stride )const
	{
		EXEC_FUNCTION( FogCoordFormat, uint32_t( type ), stride );
	}

	void BufFunctionsBase::VertexAttribFormat( uint32_t index, int size, GlES3Type type, bool normalized, int stride )const
	{
		EXEC_FUNCTION( VertexAttribFormat, index, size, uint32_t( type ), normalized, stride );
	}

	void BufFunctionsBase::VertexAttribIFormat( uint32_t index, int size, GlES3Type type, int stride )const
	{
		EXEC_FUNCTION( VertexAttribIFormat, index, size, uint32_t( type ), stride );
	}

	void BufFunctionsBase::MakeBufferResident( GlES3BufferTarget target, GlES3AccessType access )const
	{
		EXEC_FUNCTION( MakeBufferResident, uint32_t( target ), uint32_t( access ) );
	}

	void BufFunctionsBase::MakeBufferNonResident( GlES3BufferTarget target )const
	{
		EXEC_FUNCTION( MakeBufferNonResident, uint32_t( target ) );
	}

	bool BufFunctionsBase::IsBufferResident( GlES3BufferTarget target )const
	{
		return EXEC_FUNCTION( IsBufferResident, uint32_t( target ) ) != GL_FALSE;
	}

	void BufFunctionsBase::MakeNamedBufferResident( uint32_t buffer, GlES3AccessType access )const
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

	void BufFunctionsBase::GetBufferParameter( GlES3BufferTarget target, GlES3BufferParameter pname, uint64_t * params )const
	{
		EXEC_FUNCTION( GetBufferParameterui64v, uint32_t( target ), uint32_t( pname ), params );
	}

	void BufFunctionsBase::GetNamedBufferParameter( uint32_t buffer, GlES3BufferParameter pname, uint64_t * params )const
	{
		EXEC_FUNCTION( GetNamedBufferParameterui64v, buffer, uint32_t( pname ), params );
	}

	//*************************************************************************************************

	void BufFunctions::BindBuffer( GlES3BufferTarget p_target, uint32_t buffer )const
	{
		EXEC_FUNCTION( BindBuffer, uint32_t( p_target ), buffer );
	}

	void BufFunctions::BufferData( GlES3BufferTarget p_target, ptrdiff_t size, void const * data, GlES3BufferMode usage )const
	{
		EXEC_FUNCTION( BufferData, uint32_t( p_target ), size, data, uint32_t( usage ) );
	}

	void BufFunctions::CopyBufferSubData( GlES3BufferTarget readtarget, GlES3BufferTarget writetarget, ptrdiff_t readoffset, ptrdiff_t writeoffset, ptrdiff_t size )const
	{
		EXEC_FUNCTION( CopyBufferSubData, uint32_t( readtarget ), uint32_t( writetarget ), readoffset, writeoffset, size );
	}

	void BufFunctions::BufferSubData( GlES3BufferTarget p_target, ptrdiff_t offset, ptrdiff_t size, void const * data )const
	{
		EXEC_FUNCTION( BufferSubData, uint32_t( p_target ), offset, size, data );
	}

	void BufFunctions::GetBufferParameter( GlES3BufferTarget p_target, GlES3BufferParameter pname, int * params )const
	{
		EXEC_FUNCTION( GetBufferParameteriv, uint32_t( p_target ), uint32_t( pname ), params );
	}

	void * BufFunctions::MapBuffer( GlES3BufferTarget p_target, GlES3AccessType access )const
	{
		return EXEC_FUNCTION( MapBuffer, uint32_t( p_target ), uint32_t( access ) );
	}

	void BufFunctions::UnmapBuffer( GlES3BufferTarget p_target )const
	{
		EXEC_FUNCTION( UnmapBuffer, uint32_t( p_target ) );
	}

	void * BufFunctions::MapBufferRange( GlES3BufferTarget p_target, ptrdiff_t offset, ptrdiff_t length, uint32_t access )const
	{
		return EXEC_FUNCTION( MapBufferRange, uint32_t( p_target ), offset, length, access );
	}

	void BufFunctions::FlushMappedBufferRange( GlES3BufferTarget p_target, ptrdiff_t offset, ptrdiff_t length )const
	{
		EXEC_FUNCTION( FlushMappedBufferRange, uint32_t( p_target ), offset, length );
	}

	//*************************************************************************************************

	void BufFunctionsDSA::BufferData( GlES3BufferTarget p_target, ptrdiff_t size, void const * data, GlES3BufferMode usage )const
	{
		EXEC_FUNCTION( NamedBufferData, m_uiBuffer, size, data, uint32_t( usage ) );
	}

	void BufFunctionsDSA::BufferSubData( GlES3BufferTarget p_target, ptrdiff_t offset, ptrdiff_t size, void const * data )const
	{
		EXEC_FUNCTION( NamedBufferSubData, m_uiBuffer, offset, size, data );
	}

	void BufFunctionsDSA::CopyBufferSubData( GlES3BufferTarget readtarget, GlES3BufferTarget writetarget, ptrdiff_t readoffset, ptrdiff_t writeoffset, ptrdiff_t size )const
	{
		EXEC_FUNCTION( CopyNamedBufferSubData, uint32_t( readtarget ), uint32_t( writetarget ), readoffset, writeoffset, size );
	}

	void BufFunctionsDSA::GetBufferParameter( GlES3BufferTarget p_target, GlES3BufferParameter pname, int * params )const
	{
		EXEC_FUNCTION( GetNamedBufferParameteriv, m_uiBuffer, uint32_t( pname ), params );
	}

	void * BufFunctionsDSA::MapBuffer( GlES3BufferTarget p_target, GlES3AccessType access )const
	{
		return EXEC_FUNCTION( MapNamedBuffer, m_uiBuffer, uint32_t( access ) );
	}

	void BufFunctionsDSA::UnmapBuffer( GlES3BufferTarget p_target )const
	{
		EXEC_FUNCTION( UnmapNamedBuffer, m_uiBuffer );
	}

	void * BufFunctionsDSA::MapBufferRange( GlES3BufferTarget p_target, ptrdiff_t offset, ptrdiff_t length, uint32_t access )const
	{
		return EXEC_FUNCTION( MapNamedBufferRange, m_uiBuffer, offset, length, access );
	}

	void BufFunctionsDSA::FlushMappedBufferRange( GlES3BufferTarget p_target, ptrdiff_t offset, ptrdiff_t length )const
	{
		EXEC_FUNCTION( FlushMappedNamedBufferRange, m_uiBuffer, offset, length );
	}

	//*************************************************************************************************

	bool OpenGlES3::HasVao()const
	{
		return m_bHasVao;
	}

	bool OpenGlES3::HasUbo()const
	{
		return m_bHasUbo;
	}

	bool OpenGlES3::HasPbo()const
	{
		return m_bHasPbo;
	}

	bool OpenGlES3::HasTbo()const
	{
		return m_bHasTbo;
	}

	bool OpenGlES3::HasFbo()const
	{
		return m_bHasFbo;
	}

	bool OpenGlES3::HasVSh()const
	{
		return m_bHasVSh;
	}

	bool OpenGlES3::HasPSh()const
	{
		return m_bHasPSh;
	}

	bool OpenGlES3::HasGSh()const
	{
		return m_bHasGSh;
	}

	bool OpenGlES3::HasTSh()const
	{
		return m_bHasTSh;
	}

	bool OpenGlES3::HasCSh()const
	{
		return m_bHasCSh;
	}

	bool OpenGlES3::HasSpl()const
	{
		return m_bHasSpl;
	}

	bool OpenGlES3::HasVbo()const
	{
		return m_bHasVbo;
	}

	bool OpenGlES3::HasSsbo()const
	{
		return m_bHasSsbo;
	}

	bool OpenGlES3::HasInstancing()const
	{
		return m_bHasInstancedDraw && m_bHasInstancedArrays;
	}

	bool OpenGlES3::HasComputeVariableGroupSize()const
	{
		return m_bHasComputeVariableGroupSize;
	}

	bool OpenGlES3::HasNonPowerOfTwoTextures()const
	{
		return m_bHasNonPowerOfTwoTextures;
	}

	bool OpenGlES3::CanBindVboToGpuAddress()const
	{
		return m_bBindVboToGpuAddress;
	}

	Castor3D::ElementType OpenGlES3::Get( GlslAttributeType p_type )const
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

	Castor::String const & OpenGlES3::GetGlslErrorString( int p_index )const
	{
		return GlslStrings[p_index];
	}

	inline GlES3Topology OpenGlES3::Get( Castor3D::Topology p_index )const
	{
		return PrimitiveTypes[uint32_t( p_index )];
	}

	inline GlES3TexDim OpenGlES3::Get( Castor3D::TextureType p_index )const
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

	inline GlES3Comparator OpenGlES3::Get( Castor3D::ComparisonFunc p_eAlphaFunc )const
	{
		return AlphaFuncs[uint32_t( p_eAlphaFunc )];
	}

	inline GlES3WrapMode OpenGlES3::Get( Castor3D::WrapMode p_eWrapMode )const
	{
		return TextureWrapMode[uint32_t( p_eWrapMode )];
	}

	inline GlES3InterpolationMode OpenGlES3::Get( Castor3D::InterpolationMode p_interpolation )const
	{
		return TextureInterpolation[uint32_t( p_interpolation )];
	}

	inline GlES3BlendSource OpenGlES3::Get( Castor3D::BlendSource p_eArgument )const
	{
		return TextureArguments[uint32_t( p_eArgument )];
	}

	inline GlES3BlendFunc OpenGlES3::Get( Castor3D::ColourBlendFunc p_mode )const
	{
		return RgbBlendFuncs[uint32_t( p_mode )];
	}

	inline GlES3BlendFunc OpenGlES3::Get( Castor3D::AlphaBlendFunc p_mode )const
	{
		return AlphaBlendFuncs[uint32_t( p_mode )];
	}

	inline GlES3BlendFactor OpenGlES3::Get( Castor3D::BlendOperand p_eBlendFactor )const
	{
		return BlendFactors[uint32_t( p_eBlendFactor )];
	}

	inline OpenGlES3::PixelFmt const & OpenGlES3::Get( Castor::PixelFormat p_pixelFormat )const
	{
		return PixelFormats[uint32_t( p_pixelFormat )];
	}

	inline GlES3ShaderType OpenGlES3::Get( Castor3D::ShaderType p_type )const
	{
		return ShaderTypes[uint32_t( p_type )];
	}

	inline GlES3Internal OpenGlES3::GetInternal( Castor::PixelFormat p_format )const
	{
		return Internals[uint32_t( p_format )];
	}

	inline Castor::FlagCombination< GlES3BufferBit > OpenGlES3::GetComponents( Castor::FlagCombination< Castor3D::BufferComponent > const & p_components )const
	{
		Castor::FlagCombination< GlES3BufferBit > l_return;
		l_return |= ( Castor::CheckFlag( p_components, Castor3D::BufferComponent::eColour ) ? GlES3BufferBit::eColour : GlES3BufferBit( 0u ) );
		l_return |= ( Castor::CheckFlag( p_components, Castor3D::BufferComponent::eDepth ) ? GlES3BufferBit::eDepth : GlES3BufferBit( 0u ) );
		l_return |= ( Castor::CheckFlag( p_components, Castor3D::BufferComponent::eStencil ) ? GlES3BufferBit::eStencil : GlES3BufferBit( 0u ) );
		return l_return;
	}

	inline GlES3AttachmentPoint OpenGlES3::Get( Castor3D::AttachmentPoint p_eAttachment )const
	{
		return Attachments[uint32_t( p_eAttachment )];
	}

	inline GlES3Internal OpenGlES3::GetRboStorage( Castor::PixelFormat p_pixelFormat )const
	{
		return RboStorages[uint32_t( p_pixelFormat )];
	}

	inline GlES3FrameBufferMode OpenGlES3::Get( Castor3D::FrameBufferTarget p_target )const
	{
		return FramebufferModes[uint32_t( p_target )];
	}

	inline GlES3BufferBinding OpenGlES3::Get( Castor3D::WindowBuffer p_buffer )const
	{
		return Buffers[uint32_t( p_buffer )];
	}

	inline GlES3BufferBinding OpenGlES3::Get( GlES3AttachmentPoint p_buffer )const
	{
		return BuffersTA.find( p_buffer )->second;
	}

	inline GlES3Face OpenGlES3::Get( Castor3D::Culling p_eFace )const
	{
		return Faces[uint32_t( p_eFace )];
	}

	inline GlES3FillMode OpenGlES3::Get( Castor3D::FillMode p_mode )const
	{
		return FillModes[uint32_t( p_mode )];
	}

	inline GlES3Comparator OpenGlES3::Get( Castor3D::StencilFunc p_func )const
	{
		return StencilFuncs[uint32_t( p_func )];
	}

	inline GlES3StencilOp OpenGlES3::Get( Castor3D::StencilOp p_eOp )const
	{
		return StencilOps[uint32_t( p_eOp )];
	}

	inline GlES3BlendOp OpenGlES3::Get( Castor3D::BlendOperation p_eOp )const
	{
		return BlendOps[uint32_t( p_eOp )];
	}

	inline GlES3Comparator OpenGlES3::Get( Castor3D::DepthFunc p_func )const
	{
		return DepthFuncs[uint32_t( p_func )];
	}

	inline GlES3QueryType OpenGlES3::Get( Castor3D::QueryType p_value )const
	{
		return Queries[uint32_t( p_value )];
	}

	inline GlES3QueryInfo OpenGlES3::Get( Castor3D::QueryInfo p_value )const
	{
		return QueryInfos[uint32_t( p_value )];
	}

	inline GlES3TextureStorageType OpenGlES3::Get( Castor3D::TextureStorageType p_value )const
	{
		return TextureStorages[uint32_t( p_value )];
	}

	inline GlES3TexDim OpenGlES3::Get( Castor3D::CubeMapFace p_value )const
	{
		return CubeMapFaces[uint32_t( p_value )];
	}

	inline GlES3CompareMode OpenGlES3::Get( Castor3D::ComparisonMode p_value )const
	{
		return ComparisonModes[uint32_t( p_value )];
	}

	inline bool OpenGlES3::Get( Castor3D::WritingMask p_eMask )const
	{
		return WriteMasks[uint32_t( p_eMask )];
	}

	inline Castor::FlagCombination< GlES3BarrierBit > OpenGlES3::Get( Castor::FlagCombination< Castor3D::MemoryBarrier > const & p_barriers )const
	{
		Castor::FlagCombination< GlES3BarrierBit > l_return;

		if ( CheckFlag( p_barriers, Castor3D::MemoryBarrier::eVertexBuffer ) )
		{
			l_return |= GlES3BarrierBit::eVertexArrayAttrib;
		}

		if ( CheckFlag( p_barriers, Castor3D::MemoryBarrier::eIndexBuffer ) )
		{
			l_return |= GlES3BarrierBit::eElementArray;
		}

		if ( CheckFlag( p_barriers, Castor3D::MemoryBarrier::eUniformBuffer ) )
		{
			l_return |= GlES3BarrierBit::eUniform;
		}

		if ( CheckFlag( p_barriers, Castor3D::MemoryBarrier::eAtomicCounterBuffer ) )
		{
			l_return |= GlES3BarrierBit::eAtomicCounter;
		}

		if ( CheckFlag( p_barriers, Castor3D::MemoryBarrier::eQueryBuffer ) )
		{
			l_return |= GlES3BarrierBit::eQueryBuffer;
		}

		if ( CheckFlag( p_barriers, Castor3D::MemoryBarrier::eShaderStorageBuffer ) )
		{
			l_return |= GlES3BarrierBit::eShaderStorage;
		}

		return l_return;
	}

	inline Castor::String const & OpenGlES3::GetVendor()const
	{
		return m_vendor;
	}

	inline Castor::String const & OpenGlES3::GetRenderer()const
	{
		return m_renderer;
	}

	inline Castor::String const & OpenGlES3::GetStrVersion()const
	{
		return m_version;
	}

	inline int OpenGlES3::GetVersion()const
	{
		return m_iVersion;
	}

	inline int OpenGlES3::GetGlslVersion()const
	{
		return m_iGlslVersion;
	}

	inline GlES3RenderSystem & OpenGlES3::GetRenderSystem()
	{
		return m_renderSystem;
	}

	inline GlES3RenderSystem const & OpenGlES3::GetRenderSystem()const
	{
		return m_renderSystem;
	}

	inline GlES3BufferMode OpenGlES3::GetBufferFlags( uint32_t p_flags )const
	{
		GlES3BufferMode l_eReturn = GlES3BufferMode( 0 );

		if ( Castor::CheckFlag( p_flags, Castor3D::BufferAccessType::eDynamic ) )
		{
			if ( Castor::CheckFlag( p_flags, Castor3D::BufferAccessNature::eRead ) )
			{
				l_eReturn = GlES3BufferMode::eDynamicRead;
			}
			else if ( Castor::CheckFlag( p_flags, Castor3D::BufferAccessNature::eDraw ) )
			{
				l_eReturn = GlES3BufferMode::eDynamicDraw;
			}
			else if ( Castor::CheckFlag( p_flags, Castor3D::BufferAccessNature::eCopy ) )
			{
				l_eReturn = GlES3BufferMode::eDynamicCopy;
			}
		}
		else if ( Castor::CheckFlag( p_flags, Castor3D::BufferAccessType::eStatic ) )
		{
			if ( Castor::CheckFlag( p_flags, Castor3D::BufferAccessNature::eRead ) )
			{
				l_eReturn = GlES3BufferMode::eStaticRead;
			}
			else if ( Castor::CheckFlag( p_flags, Castor3D::BufferAccessNature::eDraw ) )
			{
				l_eReturn = GlES3BufferMode::eStaticDraw;
			}
			else if ( Castor::CheckFlag( p_flags, Castor3D::BufferAccessNature::eCopy ) )
			{
				l_eReturn = GlES3BufferMode::eStaticCopy;
			}
		}
		else if ( Castor::CheckFlag( p_flags, Castor3D::BufferAccessType::eStream ) )
		{
			if ( Castor::CheckFlag( p_flags, Castor3D::BufferAccessNature::eRead ) )
			{
				l_eReturn = GlES3BufferMode::eStreamRead;
			}
			else if ( Castor::CheckFlag( p_flags, Castor3D::BufferAccessNature::eDraw ) )
			{
				l_eReturn = GlES3BufferMode::eStreamDraw;
			}
			else if ( Castor::CheckFlag( p_flags, Castor3D::BufferAccessNature::eCopy ) )
			{
				l_eReturn = GlES3BufferMode::eStreamCopy;
			}
		}

		return l_eReturn;
	}

	GlES3Provider OpenGlES3::GetProvider()const
	{
		return m_gpu;
	}

	GlES3AccessType OpenGlES3::GetLockFlags( Castor3D::AccessTypes const & p_flags )const
	{
		GlES3AccessType l_eLockFlags = GlES3AccessType::eReadWrite;

		if ( Castor::CheckFlag( p_flags, Castor3D::AccessType::eRead ) )
		{
			if ( Castor::CheckFlag( p_flags, Castor3D::AccessType::eWrite ) )
			{
				l_eLockFlags = GlES3AccessType::eReadWrite;
			}
			else
			{
				l_eLockFlags = GlES3AccessType::eRead;
			}
		}
		else if ( Castor::CheckFlag( p_flags, Castor3D::AccessType::eWrite ) )
		{
			l_eLockFlags = GlES3AccessType::eWrite;
		}

		return l_eLockFlags;
	}

	Castor::FlagCombination< GlES3BufferMappingBit > OpenGlES3::GetBitfieldFlags( Castor3D::AccessTypes const & p_flags )const
	{
		Castor::FlagCombination< GlES3BufferMappingBit > l_uiFlags = 0;

		if ( Castor::CheckFlag( p_flags, Castor3D::AccessType::eRead ) )
		{
			Castor::AddFlag( l_uiFlags, GlES3BufferMappingBit::eRead );

			if ( Castor::CheckFlag( p_flags, Castor3D::AccessType::eWrite ) )
			{
				Castor::AddFlag( l_uiFlags, GlES3BufferMappingBit::eWrite );
			}
		}
		else if ( Castor::CheckFlag( p_flags, Castor3D::AccessType::eWrite ) )
		{
			Castor::AddFlag( l_uiFlags, GlES3BufferMappingBit::eWrite );
			Castor::AddFlag( l_uiFlags, GlES3BufferMappingBit::eInvalidateRange );
		}

		return l_uiFlags;
	}

	bool OpenGlES3::HasExtension( Castor::String const & p_strExtName, bool p_log )const
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

	void OpenGlES3::ClearColor( float red, float green, float blue, float alpha )const
	{
		EXEC_FUNCTION( ClearColor, red, green, blue, alpha );
	}

	void OpenGlES3::ClearColor( Castor::Colour const & p_colour )const
	{
		EXEC_FUNCTION( ClearColor, p_colour.red().value(), p_colour.green().value(), p_colour.blue().value(), p_colour.alpha().value() );
	}

	void OpenGlES3::ClearDepth( double value )const
	{
		EXEC_FUNCTION( ClearDepth, value );
	}

	void OpenGlES3::Clear( uint32_t mask )const
	{
		EXEC_FUNCTION( Clear, mask );
	}

	void OpenGlES3::DrawArrays( GlES3Topology mode, int first, int count )const
	{
		EXEC_FUNCTION( DrawArrays, uint32_t( mode ), first, count );
	}

	void OpenGlES3::DrawElements( GlES3Topology mode, int count, GlES3Type type, void const * indices )const
	{
		EXEC_FUNCTION( DrawElements, uint32_t( mode ), count, uint32_t( type ), indices );
	}

	void OpenGlES3::Enable( GlES3Tweak mode )const
	{
		EXEC_FUNCTION( Enable, uint32_t( mode ) );
	}

	void OpenGlES3::Disable( GlES3Tweak mode )const
	{
		EXEC_FUNCTION( Disable, uint32_t( mode ) );
	}

	void OpenGlES3::Enable( GlES3TexDim texture )const
	{
		EXEC_FUNCTION( Enable, uint32_t( texture ) );
	}

	void OpenGlES3::Disable( GlES3TexDim texture )const
	{
		EXEC_FUNCTION( Disable, uint32_t( texture ) );
	}

	void OpenGlES3::SelectBuffer( int size, uint32_t * buffer )const
	{
		EXEC_FUNCTION( SelectBuffer, size, buffer );
	}

	void OpenGlES3::GetIntegerv( uint32_t pname, int * params )const
	{
		EXEC_FUNCTION( GetIntegerv, pname, params );
	}

	void OpenGlES3::GetIntegerv( GlES3Max pname, int * params )const
	{
		return GetIntegerv( uint32_t( pname ), params );
	}

	void OpenGlES3::GetIntegerv( GlES3Min pname, int * params )const
	{
		return GetIntegerv( uint32_t( pname ), params );
	}

	void OpenGlES3::GetIntegerv( GlES3GpuInfo pname, int * params )const
	{
		return GetIntegerv( uint32_t( pname ), params );
	}

	void OpenGlES3::GetIntegerv( uint32_t pname, uint64_t * params )const
	{
		EXEC_FUNCTION( GetIntegerui64v, pname, params );
	}

#if defined( _WIN32 )

	void OpenGlES3::MakeCurrent( HDC hdc, HGLRC hglrc )const
	{
		wglMakeCurrent( hdc, hglrc );
	}

	void OpenGlES3::SwapBuffers( HDC hdc )const
	{
		::SwapBuffers( hdc );
	}

	void OpenGlES3::SwapInterval( int interval )const
	{
		EXEC_FUNCTION( SwapInterval, interval );
	}

	HGLRC OpenGlES3::CreateContext( HDC hdc )const
	{
		return wglCreateContext( hdc );
	}

	bool OpenGlES3::HasCreateContextAttribs()const
	{
		return m_pfnCreateContextAttribs != nullptr;
	}

	HGLRC OpenGlES3::CreateContextAttribs( HDC hDC, HGLRC hShareContext, int const * attribList )const
	{
		return EXEC_FUNCTION( CreateContextAttribs, hDC, hShareContext, attribList );
	}

	bool OpenGlES3::DeleteContext( HGLRC hContext )const
	{
		return EXEC_FUNCTION( DeleteContext, hContext ) == GL_TRUE;
	}

#elif defined( __linux__ )

	void OpenGlES3::MakeCurrent( Display * pDisplay, GLXDrawable drawable, GLXContext context )const
	{
		EXEC_FUNCTION( MakeCurrent, pDisplay, drawable, context );
	}

	void OpenGlES3::SwapBuffers( Display * pDisplay, GLXDrawable drawable )const
	{
		EXEC_FUNCTION( SwapBuffers, pDisplay, drawable );
	}

	void OpenGlES3::SwapInterval( Display * pDisplay, GLXDrawable drawable, int interval )const
	{
		EXEC_FUNCTION( SwapInterval, pDisplay, drawable, interval );
	}

	GLXContext OpenGlES3::CreateContext( Display * pDisplay, XVisualInfo * pVisualInfo, GLXContext shareList, Bool direct )const
	{
		return EXEC_FUNCTION( CreateContext, pDisplay, pVisualInfo, shareList, direct );
	}

	bool OpenGlES3::HasCreateContextAttribs()const
	{
		return m_pfnCreateContextAttribs != nullptr;
	}

	GLXContext OpenGlES3::CreateContextAttribs( Display * pDisplay, GLXFBConfig fbconfig, GLXContext shareList, Bool direct, int const * attribList )const
	{
		return EXEC_FUNCTION( CreateContextAttribs, pDisplay, fbconfig, shareList, direct, attribList );
	}

	bool OpenGlES3::DeleteContext( Display * pDisplay, GLXContext context )const
	{
		EXEC_FUNCTION( DeleteContext, pDisplay, context );
		return true;
	}

#else

#	error "Yet unsupported OS"

#endif

	void OpenGlES3::Viewport( int x, int y, int width, int height )const
	{
		EXEC_FUNCTION( Viewport, x, y, width, height );
	}

	void OpenGlES3::CullFace( GlES3Face face )const
	{
		EXEC_FUNCTION( CullFace, uint32_t( face ) );
	}

	void OpenGlES3::FrontFace( GlES3FrontFaceDirection face )const
	{
		EXEC_FUNCTION( FrontFace, uint32_t( face ) );
	}

	void OpenGlES3::GenTextures( int n, uint32_t * textures )const
	{
		EXEC_FUNCTION( GenTextures, n, textures );
	}

	void OpenGlES3::DeleteTextures( int n, uint32_t const * textures )const
	{
		EXEC_FUNCTION( DeleteTextures, n, textures );
	}

	bool OpenGlES3::IsTexture( uint32_t texture )const
	{
		return EXEC_FUNCTION( IsTexture, texture ) != GL_FALSE;
	}

	void OpenGlES3::BindTexture( GlES3TexDim mode, uint32_t texture )const
	{
		m_pTexFunctions->BindTexture( mode, texture );
	}

	void OpenGlES3::GenerateMipmap( GlES3TexDim mode )const
	{
		m_pTexFunctions->GenerateMipmap( mode );
	}

	void OpenGlES3::ActiveTexture( GlES3TextureIndex target )const
	{
		EXEC_FUNCTION( ActiveTexture, uint32_t( target ) );
	}

	void OpenGlES3::ClientActiveTexture( GlES3TextureIndex target )const
	{
		EXEC_FUNCTION( ClientActiveTexture, uint32_t( target ) );
	}

	void OpenGlES3::TexSubImage1D( GlES3TextureStorageType mode, int level, int xoffset, int width, GlES3Format format, GlES3Type type, void const * data )const
	{
		m_pTexFunctions->TexSubImage1D( mode, level, xoffset, width, format, type, data );
	}

	void OpenGlES3::TexSubImage2D( GlES3TextureStorageType mode, int level, int xoffset, int yoffset, int width, int height, GlES3Format format, GlES3Type type, void const * data )const
	{
		m_pTexFunctions->TexSubImage2D( mode, level, xoffset, yoffset, width, height, format, type, data );
	}

	void OpenGlES3::TexSubImage2D( GlES3TextureStorageType mode, int level, Castor::Position const & p_position, Castor::Size const & p_size, GlES3Format format, GlES3Type type, void const * data )const
	{
		m_pTexFunctions->TexSubImage2D( mode, level, p_position.x(), p_position.y(), p_size.width(), p_size.height(), format, type, data );
	}

	void OpenGlES3::TexSubImage2D( GlES3TextureStorageType mode, int level, Castor::Rectangle const & p_rect, GlES3Format format, GlES3Type type, void const * data )const
	{
		m_pTexFunctions->TexSubImage2D( mode, level, p_rect.left(), p_rect.top(), p_rect.width(), p_rect.height(), format, type, data );
	}

	void OpenGlES3::TexSubImage3D( GlES3TextureStorageType mode, int level, int xoffset, int yoffset, int zoffset, int width, int height, int depth, GlES3Format format, GlES3Type type, void const * data )const
	{
		m_pTexFunctions->TexSubImage3D( mode, level, xoffset, yoffset, zoffset, width, height, depth, format, type, data );
	}

	void OpenGlES3::TexImage1D( GlES3TextureStorageType mode, int level, GlES3Internal internalFormat, int width, int border, GlES3Format format, GlES3Type type, void const * data )const
	{
		m_pTexFunctions->TexImage1D( mode, level, internalFormat, width, border, format, type, data );
	}

	void OpenGlES3::TexImage2D( GlES3TextureStorageType mode, int level, GlES3Internal internalFormat, int width, int height, int border, GlES3Format format, GlES3Type type, void const * data )const
	{
		m_pTexFunctions->TexImage2D( mode, level, internalFormat, width, height, border, format, type, data );
	}

	void OpenGlES3::TexImage2D( GlES3TextureStorageType mode, int level, GlES3Internal internalFormat, Castor::Size const & p_size, int border, GlES3Format format, GlES3Type type, void const * data )const
	{
		m_pTexFunctions->TexImage2D( mode, level, internalFormat, p_size.width(), p_size.height(), border, format, type, data );
	}

	void OpenGlES3::TexImage3D( GlES3TextureStorageType mode, int level, GlES3Internal internalFormat, int width, int height, int depth, int border, GlES3Format format, GlES3Type type, void const * data )const
	{
		m_pTexFunctions->TexImage3D( mode, level, internalFormat, width, height, depth, border, format, type, data );
	}

	void OpenGlES3::GetTexImage( GlES3TextureStorageType mode, int level, GlES3Format format, GlES3Type type, void * img )const
	{
		m_pTexFunctions->GetTexImage( mode, level, format, type, img );
	}

	void OpenGlES3::BlendFunc( GlES3BlendFactor sfactor, GlES3BlendFactor dfactor )const
	{
		EXEC_FUNCTION( BlendFunc, uint32_t( sfactor ), uint32_t( dfactor ) );
	}

	void OpenGlES3::BlendFunc( GlES3BlendFactor p_eRgbSrc, GlES3BlendFactor p_eRgbDst, GlES3BlendFactor p_eAlphaSrc, GlES3BlendFactor p_eAlphaDst )const
	{
		EXEC_FUNCTION( BlendFuncSeparate, uint32_t( p_eRgbSrc ), uint32_t( p_eRgbDst ), uint32_t( p_eAlphaSrc ), uint32_t( p_eAlphaDst ) );
	}

	void OpenGlES3::BlendFunc( uint32_t p_uiBuffer, GlES3BlendFactor p_eRgbSrc, GlES3BlendFactor p_eRgbDst, GlES3BlendFactor p_eAlphaSrc, GlES3BlendFactor p_eAlphaDst )const
	{
		EXEC_FUNCTION( BlendFuncSeparatei, p_uiBuffer, uint32_t( p_eRgbSrc ), uint32_t( p_eRgbDst ), uint32_t( p_eAlphaSrc ), uint32_t( p_eAlphaDst ) );
	}

	void OpenGlES3::BlendEquation( GlES3BlendOp p_eOp )const
	{
		EXEC_FUNCTION( BlendEquation, uint32_t( p_eOp ) );
	}

	void OpenGlES3::BlendEquation( uint32_t p_uiBuffer, GlES3BlendOp p_eOp )const
	{
		EXEC_FUNCTION( BlendEquationi, p_uiBuffer, uint32_t( p_eOp ) );
	}

	void OpenGlES3::ComparisonFunc( GlES3Comparator func, float ref )const
	{
		EXEC_FUNCTION( AlphaFunc, uint32_t( func ), ref );
	}

	void OpenGlES3::DepthFunc( GlES3Comparator p_func )const
	{
		EXEC_FUNCTION( DepthFunc, uint32_t( p_func ) );
	}

	void OpenGlES3::DepthMask( bool p_bMask )const
	{
		EXEC_FUNCTION( DepthMask, uint8_t( p_bMask ) );
	}

	void OpenGlES3::ColorMask( bool p_r, bool p_g, bool p_b, bool p_a )const
	{
		EXEC_FUNCTION( ColorMask, uint8_t( p_r ), uint8_t( p_g ), uint8_t( p_b ), uint8_t( p_a ) );
	}

	void OpenGlES3::PolygonMode( GlES3Face p_eFacing, GlES3FillMode p_mode )const
	{
		EXEC_FUNCTION( PolygonMode, uint32_t( p_eFacing ), uint32_t( p_mode ) );
	}

	void OpenGlES3::StencilOp( GlES3StencilOp p_eStencilFail, GlES3StencilOp p_eDepthFail, GlES3StencilOp p_eStencilPass )const
	{
		EXEC_FUNCTION( StencilOp, uint32_t( p_eStencilFail ), uint32_t( p_eDepthFail ), uint32_t( p_eStencilPass ) );
	}

	void OpenGlES3::StencilFunc( GlES3Comparator p_func, int p_iRef, uint32_t p_uiMask )const
	{
		EXEC_FUNCTION( StencilFunc, uint32_t( p_func ), p_iRef, p_uiMask );
	}

	void OpenGlES3::StencilMask( uint32_t p_uiMask )const
	{
		EXEC_FUNCTION( StencilMask, p_uiMask );
	}

	void OpenGlES3::StencilOpSeparate( GlES3Face p_eFacing, GlES3StencilOp p_eStencilFail, GlES3StencilOp p_eDepthFail, GlES3StencilOp p_eStencilPass )const
	{
		EXEC_FUNCTION( StencilOpSeparate, uint32_t( p_eFacing ), uint32_t( p_eStencilFail ), uint32_t( p_eDepthFail ), uint32_t( p_eStencilPass ) );
	}

	void OpenGlES3::StencilFuncSeparate( GlES3Face p_eFacing, GlES3Comparator p_func, int p_iRef, uint32_t p_uiMask )const
	{
		EXEC_FUNCTION( StencilFuncSeparate, uint32_t( p_eFacing ), uint32_t( p_func ), p_iRef, p_uiMask );
	}

	void OpenGlES3::StencilMaskSeparate( GlES3Face p_eFacing, uint32_t p_uiMask )const
	{
		EXEC_FUNCTION( StencilMaskSeparate, uint32_t( p_eFacing ), p_uiMask );
	}

	void OpenGlES3::Hint( GlES3Hint p_eHint, GlES3HintValue p_eValue )const
	{
		EXEC_FUNCTION( Hint, uint32_t( p_eHint ), uint32_t( p_eValue ) );
	}

	void OpenGlES3::PolygonOffset( float p_fFactor, float p_fUnits )const
	{
		EXEC_FUNCTION( PolygonOffset, p_fFactor, p_fUnits );
	}

	void OpenGlES3::BlendColor( Castor::Colour const & p_colour )const
	{
		EXEC_FUNCTION( BlendColor, p_colour.red().value(), p_colour.green().value(), p_colour.blue().value(), p_colour.alpha().value() );
	}

	void OpenGlES3::SampleCoverage( float fValue, bool invert )const
	{
		EXEC_FUNCTION( SampleCoverage, fValue, uint8_t( invert ) );
	}

	void OpenGlES3::DrawArraysInstanced( GlES3Topology mode, int first, int count, int primcount )const
	{
		EXEC_FUNCTION( DrawArraysInstanced, uint32_t( mode ), first, count, primcount );
	}

	void OpenGlES3::DrawElementsInstanced( GlES3Topology mode, int count, GlES3Type type, const void * indices, int primcount )const
	{
		EXEC_FUNCTION( DrawElementsInstanced, uint32_t( mode ), count, uint32_t( type ), indices, primcount );
	}

	void OpenGlES3::VertexAttribDivisor( uint32_t index, uint32_t divisor )const
	{
		EXEC_FUNCTION( VertexAttribDivisor, index, divisor );
	}

	void OpenGlES3::ReadBuffer( GlES3BufferBinding p_buffer )const
	{
		EXEC_FUNCTION( ReadBuffer, uint32_t( p_buffer ) );
	}

	void OpenGlES3::ReadPixels( int x, int y, int width, int height, GlES3Format format, GlES3Type type, void * pixels )const
	{
		EXEC_FUNCTION( ReadPixels, x, y, width, height, uint32_t( format ), uint32_t( type ), pixels );
	}

	void OpenGlES3::ReadPixels( Castor::Position const & p_position, Castor::Size const & p_size, GlES3Format format, GlES3Type type, void * pixels )const
	{
		EXEC_FUNCTION( ReadPixels, int( p_position.x() ), int( p_position.y() ), int( p_size.width() ), int( p_size.height() ), uint32_t( format ), uint32_t( type ), pixels );
	}

	void OpenGlES3::ReadPixels( Castor::Rectangle const & p_rect, GlES3Format format, GlES3Type type, void * pixels )const
	{
		EXEC_FUNCTION( ReadPixels, p_rect.left(), p_rect.top(), p_rect.width(), p_rect.height(), uint32_t( format ), uint32_t( type ), pixels );
	}

	void OpenGlES3::DrawBuffer( GlES3BufferBinding p_buffer )const
	{
		EXEC_FUNCTION( DrawBuffer, uint32_t( p_buffer ) );
	}

	void OpenGlES3::DrawPixels( int width, int height, GlES3Format format, GlES3Type type, void const * data )const
	{
		EXEC_FUNCTION( DrawPixels, width, height, uint32_t( format ), uint32_t( type ), data );
	}

	void OpenGlES3::PixelStore( GlES3StorageMode p_mode, int p_iParam )const
	{
		EXEC_FUNCTION( PixelStorei, uint32_t( p_mode ), p_iParam );
	}

	void OpenGlES3::PixelStore( GlES3StorageMode p_mode, float p_fParam )const
	{
		EXEC_FUNCTION( PixelStoref, uint32_t( p_mode ), p_fParam );
	}

	void OpenGlES3::TexStorage1D( GlES3TextureStorageType target, GLint levels, GlES3Internal internalformat, GLsizei width )const
	{
		EXEC_FUNCTION( TexStorage1D, uint32_t( target ), levels, int( internalformat ), width );
	}

	void OpenGlES3::TexStorage2D( GlES3TextureStorageType target, GLint levels, GlES3Internal internalformat, GLsizei width, GLsizei height )const
	{
		EXEC_FUNCTION( TexStorage2D, uint32_t( target ), levels, int( internalformat ), width, height );
	}

	void OpenGlES3::TexStorage3D( GlES3TextureStorageType target, GLint levels, GlES3Internal internalformat, GLsizei width, GLsizei height, GLsizei depth )const
	{
		EXEC_FUNCTION( TexStorage3D, uint32_t( target ), levels, int( internalformat ), width, height, depth );
	}

	void OpenGlES3::TexStorage2DMultisample( GlES3TextureStorageType target, GLsizei samples, GlES3Internal internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations )const
	{
		EXEC_FUNCTION( TexStorage2DMultisample, uint32_t( target ), samples, int( internalformat ), width, height, fixedsamplelocations );
	}

	void OpenGlES3::TexStorage3DMultisample( GlES3TextureStorageType target, GLsizei samples, GlES3Internal internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations )const
	{
		EXEC_FUNCTION( TexStorage3DMultisample, uint32_t( target ), samples, int( internalformat ), width, height, depth, fixedsamplelocations );
	}

	void OpenGlES3::GenSamplers( int count, uint32_t * samplers )const
	{
		EXEC_FUNCTION( GenSamplers, count, samplers );
	}

	void OpenGlES3::DeleteSamplers( int count, const uint32_t * samplers )const
	{
		EXEC_FUNCTION( DeleteSamplers, count, samplers );
	}

	bool OpenGlES3::IsSampler( uint32_t sampler )const
	{
		return EXEC_FUNCTION( IsSampler, sampler ) != GL_FALSE;
	}

	void OpenGlES3::BindSampler( uint32_t unit, uint32_t sampler )const
	{
		EXEC_FUNCTION( BindSampler, unit, sampler );
	}

	void OpenGlES3::GetSamplerParameter( uint32_t sampler, GlES3SamplerParameter pname, uint32_t * params )const
	{
		EXEC_FUNCTION( GetSamplerParameteruiv, sampler, uint32_t( pname ), params );
	}

	void OpenGlES3::GetSamplerParameter( uint32_t sampler, GlES3SamplerParameter pname, float * params )const
	{
		EXEC_FUNCTION( GetSamplerParameterfv, sampler, uint32_t( pname ), params );
	}

	void OpenGlES3::GetSamplerParameter( uint32_t sampler, GlES3SamplerParameter pname, int * params )const
	{
		EXEC_FUNCTION( GetSamplerParameteriv, sampler, uint32_t( pname ), params );
	}

	void OpenGlES3::SetSamplerParameter( uint32_t sampler, GlES3SamplerParameter pname, const uint32_t * params )const
	{
		REQUIRE( pname != GlES3SamplerParameter::eMaxAnisotropy || m_bHasAnisotropic );
		EXEC_FUNCTION( SamplerParameteruiv, sampler, uint32_t( pname ), params );
	}

	void OpenGlES3::SetSamplerParameter( uint32_t sampler, GlES3SamplerParameter pname, float param )const
	{
		REQUIRE( pname != GlES3SamplerParameter::eMaxAnisotropy || m_bHasAnisotropic );
		EXEC_FUNCTION( SamplerParameterf, sampler, uint32_t( pname ), param );
	}

	void OpenGlES3::SetSamplerParameter( uint32_t sampler, GlES3SamplerParameter pname, const float * params )const
	{
		REQUIRE( pname != GlES3SamplerParameter::eMaxAnisotropy || m_bHasAnisotropic );
		EXEC_FUNCTION( SamplerParameterfv, sampler, uint32_t( pname ), params );
	}

	void OpenGlES3::SetSamplerParameter( uint32_t sampler, GlES3SamplerParameter pname, int param )const
	{
		REQUIRE( pname != GlES3SamplerParameter::eMaxAnisotropy || m_bHasAnisotropic );
		EXEC_FUNCTION( SamplerParameteri, sampler, uint32_t( pname ), param );
	}

	void OpenGlES3::SetSamplerParameter( uint32_t sampler, GlES3SamplerParameter pname, const int * params )const
	{
		REQUIRE( pname != GlES3SamplerParameter::eMaxAnisotropy || m_bHasAnisotropic );
		EXEC_FUNCTION( SamplerParameteriv, sampler, uint32_t( pname ), params );
	}

	void OpenGlES3::TexBuffer( GlES3TexDim p_target, GlES3Internal p_internalFormat, uint32_t buffer )const
	{
		EXEC_FUNCTION( TexBuffer, uint32_t( p_target ), uint32_t( p_internalFormat ), buffer );
	}

	void OpenGlES3::GenBuffers( int n, uint32_t * buffers )const
	{
		EXEC_FUNCTION( GenBuffers, n, buffers );
	}

	void OpenGlES3::DeleteBuffers( int n, uint32_t const * buffers )const
	{
		EXEC_FUNCTION( DeleteBuffers, n, buffers );
	}

	bool OpenGlES3::IsBuffer( uint32_t buffer )const
	{
		return EXEC_FUNCTION( IsBuffer, buffer ) != GL_FALSE;
	}

	void OpenGlES3::BindBuffer( GlES3BufferTarget p_target, uint32_t buffer )const
	{
		m_pBufFunctions->BindBuffer( p_target, buffer );
	}

	void OpenGlES3::BufferData( GlES3BufferTarget p_target, ptrdiff_t size, void const * data, GlES3BufferMode usage )const
	{
		m_pBufFunctions->BufferData( p_target, size, data, usage );
	}

	void OpenGlES3::BufferSubData( GlES3BufferTarget p_target, ptrdiff_t offset, ptrdiff_t size, void const * data )const
	{
		m_pBufFunctions->BufferSubData( p_target, offset, size, data );
	}

	void OpenGlES3::CopyBufferSubData( GlES3BufferTarget readtarget, GlES3BufferTarget writetarget, ptrdiff_t readoffset, ptrdiff_t writeoffset, ptrdiff_t size )const
	{
		m_pBufFunctions->CopyBufferSubData( readtarget, writetarget, readoffset, writeoffset, size );
	}

	void OpenGlES3::GetBufferParameter( GlES3BufferTarget p_target, GlES3BufferParameter pname, int * params )const
	{
		m_pBufFunctions->GetBufferParameter( p_target, pname, params );
	}

	void OpenGlES3::GetBufferParameter( GlES3BufferTarget p_target, GlES3BufferParameter pname, uint64_t * params )const
	{
		m_pBufFunctions->GetBufferParameter( p_target, pname, params );
	}

	void * OpenGlES3::MapBuffer( GlES3BufferTarget p_target, GlES3AccessType access )const
	{
		return m_pBufFunctions->MapBuffer( p_target, access );
	}

	void OpenGlES3::UnmapBuffer( GlES3BufferTarget p_target )const
	{
		m_pBufFunctions->UnmapBuffer( p_target );
	}

	void * OpenGlES3::MapBufferRange( GlES3BufferTarget p_target, ptrdiff_t offset, ptrdiff_t length, uint32_t access )const
	{
		return m_pBufFunctions->MapBufferRange( p_target, offset, length, access );
	}

	void OpenGlES3::FlushMappedBufferRange( GlES3BufferTarget p_target, ptrdiff_t offset, ptrdiff_t length )const
	{
		m_pBufFunctions->FlushMappedBufferRange( p_target, offset, length );
	}

	void OpenGlES3::BufferAddressRange( GlES3Address pname, uint32_t index, uint64_t address, size_t length )const
	{
		m_pBufFunctions->BufferAddressRange( pname, index, address, length );
	}

	void OpenGlES3::VertexFormat( int size, GlES3Type type, int stride )const
	{
		m_pBufFunctions->VertexFormat( size, type, stride );
	}

	void OpenGlES3::NormalFormat( GlES3Type type, int stride )const
	{
		m_pBufFunctions->NormalFormat( type, stride );
	}

	void OpenGlES3::ColorFormat( int size, GlES3Type type, int stride )const
	{
		m_pBufFunctions->ColorFormat( size, type, stride );
	}

	void OpenGlES3::IndexFormat( GlES3Type type, int stride )const
	{
		m_pBufFunctions->IndexFormat( type, stride );
	}

	void OpenGlES3::TexCoordFormat( int size, GlES3Type type, int stride )const
	{
		m_pBufFunctions->TexCoordFormat( size, type, stride );
	}

	void OpenGlES3::EdgeFlagFormat( int stride )const
	{
		m_pBufFunctions->EdgeFlagFormat( stride );
	}

	void OpenGlES3::SecondaryColorFormat( int size, GlES3Type type, int stride )const
	{
		m_pBufFunctions->SecondaryColorFormat( size, type, stride );
	}

	void OpenGlES3::FogCoordFormat( uint32_t type, int stride )const
	{
		m_pBufFunctions->FogCoordFormat( type, stride );
	}

	void OpenGlES3::VertexAttribFormat( uint32_t index, int size, GlES3Type type, bool normalized, int stride )const
	{
		m_pBufFunctions->VertexAttribFormat( index, size, type, normalized, stride );
	}

	void OpenGlES3::VertexAttribIFormat( uint32_t index, int size, GlES3Type type, int stride )const
	{
		m_pBufFunctions->VertexAttribIFormat( index, size, type, stride );
	}

	void OpenGlES3::MakeBufferResident( GlES3BufferTarget target, GlES3AccessType access )const
	{
		m_pBufFunctions->MakeBufferResident( target, access );
	}

	void OpenGlES3::MakeBufferNonResident( GlES3BufferTarget target )const
	{
		m_pBufFunctions->MakeBufferNonResident( target );
	}

	bool OpenGlES3::IsBufferResident( GlES3BufferTarget target )const
	{
		return m_pBufFunctions->IsBufferResident( target );
	}

	void OpenGlES3::MakeNamedBufferResident( uint32_t buffer, GlES3AccessType access )const
	{
		m_pBufFunctions->MakeNamedBufferResident( buffer, access );
	}

	void OpenGlES3::MakeNamedBufferNonResident( uint32_t buffer )const
	{
		m_pBufFunctions->MakeNamedBufferNonResident( buffer );
	}

	bool OpenGlES3::IsNamedBufferResident( uint32_t buffer )const
	{
		return m_pBufFunctions->IsNamedBufferResident( buffer );
	}

	void OpenGlES3::GetNamedBufferParameter( uint32_t buffer, GlES3BufferParameter pname, uint64_t * params )const
	{
		m_pBufFunctions->GetNamedBufferParameter( buffer, pname, params );
	}

	void OpenGlES3::GenTransformFeedbacks( int n, uint32_t * buffers )const
	{
		EXEC_FUNCTION( GenTransformFeedbacks, n, buffers );
	}

	void OpenGlES3::DeleteTransformFeedbacks( int n, uint32_t const * buffers )const
	{
		EXEC_FUNCTION( DeleteTransformFeedbacks, n, buffers );
	}

	void OpenGlES3::BindTransformFeedback( GlES3BufferTarget target, uint32_t buffer )const
	{
		EXEC_FUNCTION( BindTransformFeedback, target, buffer );
	}

	bool OpenGlES3::IsTransformFeedback( uint32_t buffer )const
	{
		return EXEC_FUNCTION( IsTransformFeedback, buffer ) != GL_FALSE;
	}

	void OpenGlES3::BeginTransformFeedback( GlES3Topology primitive )const
	{
		EXEC_FUNCTION( BeginTransformFeedback, uint32_t( primitive ) );
	}

	void OpenGlES3::PauseTransformFeedback()const
	{
		EXEC_VOID_FUNCTION( PauseTransformFeedback );
	}

	void OpenGlES3::ResumeTransformFeedback()const
	{
		EXEC_VOID_FUNCTION( ResumeTransformFeedback );
	}

	void OpenGlES3::EndTransformFeedback()const
	{
		EXEC_VOID_FUNCTION( EndTransformFeedback );
	}

	void OpenGlES3::TransformFeedbackVaryings( uint32_t program, int count, char const ** varyings, GlES3AttributeLayout bufferMode )const
	{
		EXEC_FUNCTION( TransformFeedbackVaryings, program, count, varyings, bufferMode );
	}

	void OpenGlES3::DrawTransformFeedback( GlES3Topology mode, uint32_t id )const
	{
		EXEC_FUNCTION( DrawTransformFeedback, uint32_t( mode ), id );
	}

	void OpenGlES3::GenFramebuffers( int n, uint32_t * framebuffers )const
	{
		EXEC_FUNCTION( GenFramebuffers, n, framebuffers );
	}

	void OpenGlES3::DeleteFramebuffers( int n, uint32_t const * framebuffers )const
	{
		EXEC_FUNCTION( DeleteFramebuffers, n, framebuffers );
	}

	bool OpenGlES3::IsFramebuffer( uint32_t framebuffer )const
	{
		return EXEC_FUNCTION( IsFramebuffer, framebuffer ) != GL_FALSE;
	}

	void OpenGlES3::BindFramebuffer( GlES3FrameBufferMode p_eBindingMode, uint32_t framebuffer )const
	{
		EXEC_FUNCTION( BindFramebuffer, uint32_t( p_eBindingMode ), framebuffer );
	}

	void OpenGlES3::FramebufferTexture( GlES3FrameBufferMode p_target, GlES3AttachmentPoint p_eAttachment, uint32_t texture, int level )const
	{
		EXEC_FUNCTION( FramebufferTexture, uint32_t( p_target ), uint32_t( p_eAttachment ), texture, level );
	}

	void OpenGlES3::FramebufferTexture1D( GlES3FrameBufferMode p_target, GlES3AttachmentPoint p_eAttachment, GlES3TexDim p_texTarget, uint32_t texture, int level )const
	{
		EXEC_FUNCTION( FramebufferTexture1D, uint32_t( p_target ), uint32_t( p_eAttachment ), uint32_t( p_texTarget ), texture, level );
	}

	void OpenGlES3::FramebufferTexture2D( GlES3FrameBufferMode p_target, GlES3AttachmentPoint p_eAttachment, GlES3TexDim p_texTarget, uint32_t texture, int level )const
	{
		EXEC_FUNCTION( FramebufferTexture2D, uint32_t( p_target ), uint32_t( p_eAttachment ), uint32_t( p_texTarget ), texture, level );
	}

	void OpenGlES3::FramebufferTexture3D( GlES3FrameBufferMode p_target, GlES3AttachmentPoint p_eAttachment, GlES3TexDim p_texTarget, uint32_t texture, int level, int layer )const
	{
		EXEC_FUNCTION( FramebufferTexture3D, uint32_t( p_target ), uint32_t( p_eAttachment ), uint32_t( p_texTarget ), texture, level, layer );
	}

	void OpenGlES3::FramebufferTextureLayer( GlES3FrameBufferMode p_target, GlES3AttachmentPoint p_eAttachment, uint32_t texture, int level, int layer )const
	{
		EXEC_FUNCTION( FramebufferTextureLayer, uint32_t( p_target ), uint32_t( p_eAttachment ), texture, level, layer );
	}

	void OpenGlES3::FramebufferRenderbuffer( GlES3FrameBufferMode p_target, GlES3AttachmentPoint p_eAttachment, GlES3RenderBufferMode p_eRenderbufferTarget, uint32_t renderbufferId )const
	{
		EXEC_FUNCTION( FramebufferRenderbuffer, uint32_t( p_target ), uint32_t( p_eAttachment ), uint32_t( p_eRenderbufferTarget ), renderbufferId );
	}

	void OpenGlES3::GenRenderbuffers( int n, uint32_t * ids )const
	{
		EXEC_FUNCTION( GenRenderbuffers, n, ids );
	}

	void OpenGlES3::DeleteRenderbuffers( int n, uint32_t const * ids )const
	{
		EXEC_FUNCTION( DeleteRenderbuffers, n, ids );
	}

	bool OpenGlES3::IsRenderbuffer( uint32_t framebuffer )const
	{
		return EXEC_FUNCTION( IsRenderbuffer, framebuffer ) != GL_FALSE;
	}

	void OpenGlES3::BindRenderbuffer( GlES3RenderBufferMode p_eBindingMode, uint32_t id )const
	{
		EXEC_FUNCTION( BindRenderbuffer, uint32_t( p_eBindingMode ), id );
	}

	void OpenGlES3::RenderbufferStorage( GlES3RenderBufferMode p_eBindingMode, GlES3Internal p_format, int width, int height )const
	{
		EXEC_FUNCTION( RenderbufferStorage, uint32_t( p_eBindingMode ), uint32_t( p_format ), width, height );
	}

	void OpenGlES3::RenderbufferStorageMultisample( GlES3RenderBufferMode p_eBindingMode, int p_iSamples, GlES3Internal p_format, int width, int height )const
	{
		int l_iMaxSamples;
		int l_iMaxSize;
		OpenGlES3::GetIntegerv( GlES3Max::eSamples, &l_iMaxSamples );
		OpenGlES3::GetIntegerv( GlES3Max::eRenderbufferSize, &l_iMaxSize );

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

	void OpenGlES3::RenderbufferStorage( GlES3RenderBufferMode p_eBindingMode, GlES3Internal p_format, Castor::Size const & size )const
	{
		EXEC_FUNCTION( RenderbufferStorage, uint32_t( p_eBindingMode ), uint32_t( p_format ), int( size.width() ), int( size.height() ) );
	}

	void OpenGlES3::RenderbufferStorageMultisample( GlES3RenderBufferMode p_eBindingMode, int p_iSamples, GlES3Internal p_format, Castor::Size const & size )const
	{
		int l_iMaxSamples;
		int l_iMaxSize;
		OpenGlES3::GetIntegerv( GlES3Max::eSamples, &l_iMaxSamples );
		OpenGlES3::GetIntegerv( GlES3Max::eRenderbufferSize, &l_iMaxSize );

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

	void OpenGlES3::GetRenderbufferParameteriv( GlES3RenderBufferMode p_eBindingMode, GlES3RenderBufferParameter p_param, int * values )const
	{
		EXEC_FUNCTION( GetRenderbufferParameteriv, uint32_t( p_eBindingMode ), uint32_t( p_param ), values );
	}

	void OpenGlES3::BlitFramebuffer( int srcX0, int srcY0, int srcX1, int srcY1, int dstX0, int dstY0, int dstX1, int dstY1, uint32_t mask, GlES3InterpolationMode filter )const
	{
		EXEC_FUNCTION( BlitFramebuffer, srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, uint32_t( filter ) );
	}

	void OpenGlES3::BlitFramebuffer( Castor::Rectangle const & rcSrc, Castor::Rectangle const & rcDst, uint32_t mask, GlES3InterpolationMode filter )const
	{
		EXEC_FUNCTION( BlitFramebuffer, rcSrc.left(), rcSrc.top(), rcSrc.right(), rcSrc.bottom(), rcDst.left(), rcDst.top(), rcDst.right(), rcDst.bottom(), mask, uint32_t( filter ) );
	}

	void OpenGlES3::BlitFramebuffer( int srcX0, int srcY0, int srcX1, int srcY1, int dstX0, int dstY0, int dstX1, int dstY1, GlES3Component mask, GlES3InterpolationMode filter )const
	{
		EXEC_FUNCTION( BlitFramebuffer, srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, uint32_t( mask ), uint32_t( filter ) );
	}

	void OpenGlES3::BlitFramebuffer( Castor::Rectangle const & rcSrc, Castor::Rectangle const & rcDst, GlES3Component mask, GlES3InterpolationMode filter )const
	{
		EXEC_FUNCTION( BlitFramebuffer, rcSrc.left(), rcSrc.top(), rcSrc.right(), rcSrc.bottom(), rcDst.left(), rcDst.top(), rcDst.right(), rcDst.bottom(), uint32_t( mask ), uint32_t( filter ) );
	}

	void OpenGlES3::DrawBuffers( int n, const uint32_t * bufs )const
	{
		EXEC_FUNCTION( DrawBuffers, n, bufs );
	}

	GlES3FramebufferStatus OpenGlES3::CheckFramebufferStatus( GlES3FrameBufferMode p_target )const
	{
		return GlES3FramebufferStatus( EXEC_FUNCTION( CheckFramebufferStatus, uint32_t( p_target ) ) );
	}

	void OpenGlES3::TexImage2DMultisample( GlES3TextureStorageType p_target, int p_iSamples, GlES3Internal p_internalFormat, int p_iWidth, int p_iHeight, bool p_bFixedSampleLocations )const
	{
		EXEC_FUNCTION( TexImage2DMultisample, uint32_t( p_target ), p_iSamples, int( p_internalFormat ), p_iWidth, p_iHeight, uint8_t( p_bFixedSampleLocations ) );
	}

	void OpenGlES3::TexImage2DMultisample( GlES3TextureStorageType p_target, int p_iSamples, GlES3Internal p_internalFormat, Castor::Size const & p_size, bool p_bFixedSampleLocations )const
	{
		EXEC_FUNCTION( TexImage2DMultisample, uint32_t( p_target ), p_iSamples, int( p_internalFormat ), int( p_size.width() ), int( p_size.height() ), uint8_t( p_bFixedSampleLocations ) );
	}

	int OpenGlES3::GetUniformLocation( uint32_t program, char const * name )const
	{
		return EXEC_FUNCTION( GetUniformLocation, program, name );
	}

	void OpenGlES3::SetUniform( int location, int v0 )const
	{
		EXEC_FUNCTION( Uniform1i, location, v0 );
	}

	void OpenGlES3::SetUniform( int location, int v0, int v1 )const
	{
		EXEC_FUNCTION( Uniform2i, location, v0, v1 );
	}

	void OpenGlES3::SetUniform( int location, int v0, int v1, int v2 )const
	{
		EXEC_FUNCTION( Uniform3i, location, v0, v1, v2 );
	}

	void OpenGlES3::SetUniform( int location, int v0, int v1, int v2, int v3 )const
	{
		EXEC_FUNCTION( Uniform4i, location, v0, v1, v2, v3 );
	}

	void OpenGlES3::SetUniform1v( int location, int count, int const * params )const
	{
		EXEC_FUNCTION( Uniform1iv, location, count, params );
	}

	void OpenGlES3::SetUniform2v( int location, int count, int const * params )const
	{
		EXEC_FUNCTION( Uniform2iv, location, count, params );
	}

	void OpenGlES3::SetUniform3v( int location, int count, int const * params )const
	{
		EXEC_FUNCTION( Uniform3iv, location, count, params );
	}

	void OpenGlES3::SetUniform4v( int location, int count, int const * params )const
	{
		EXEC_FUNCTION( Uniform4iv, location, count, params );
	}

	void OpenGlES3::SetUniform( int location, uint32_t v0 )const
	{
		EXEC_FUNCTION( Uniform1ui, location, v0 );
	}

	void OpenGlES3::SetUniform( int location, uint32_t v0, uint32_t v1 )const
	{
		EXEC_FUNCTION( Uniform2ui, location, v0, v1 );
	}

	void OpenGlES3::SetUniform( int location, uint32_t v0, uint32_t v1, uint32_t v2 )const
	{
		EXEC_FUNCTION( Uniform3ui, location, v0, v1, v2 );
	}

	void OpenGlES3::SetUniform( int location, uint32_t v0, uint32_t v1, uint32_t v2, uint32_t v3 )const
	{
		EXEC_FUNCTION( Uniform4ui, location, v0, v1, v2, v3 );
	}

	void OpenGlES3::SetUniform1v( int location, int count, uint32_t const * params )const
	{
		EXEC_FUNCTION( Uniform1uiv, location, count, params );
	}

	void OpenGlES3::SetUniform2v( int location, int count, uint32_t const * params )const
	{
		EXEC_FUNCTION( Uniform2uiv, location, count, params );
	}

	void OpenGlES3::SetUniform3v( int location, int count, uint32_t const * params )const
	{
		EXEC_FUNCTION( Uniform3uiv, location, count, params );
	}

	void OpenGlES3::SetUniform4v( int location, int count, uint32_t const * params )const
	{
		EXEC_FUNCTION( Uniform4uiv, location, count, params );
	}

	void OpenGlES3::SetUniform( int location, float v0 )const
	{
		EXEC_FUNCTION( Uniform1f, location, v0 );
	}

	void OpenGlES3::SetUniform( int location, float v0, float v1 )const
	{
		EXEC_FUNCTION( Uniform2f, location, v0, v1 );
	}

	void OpenGlES3::SetUniform( int location, float v0, float v1, float v2 )const
	{
		EXEC_FUNCTION( Uniform3f, location, v0, v1, v2 );
	}

	void OpenGlES3::SetUniform( int location, float v0, float v1, float v2, float v3 )const
	{
		EXEC_FUNCTION( Uniform4f, location, v0, v1, v2, v3 );
	}

	void OpenGlES3::SetUniform1v( int location, int count, float const * params )const
	{
		EXEC_FUNCTION( Uniform1fv, location, count, params );
	}

	void OpenGlES3::SetUniform2v( int location, int count, float const * params )const
	{
		EXEC_FUNCTION( Uniform2fv, location, count, params );
	}

	void OpenGlES3::SetUniform3v( int location, int count, float const * params )const
	{
		EXEC_FUNCTION( Uniform3fv, location, count, params );
	}

	void OpenGlES3::SetUniform4v( int location, int count, float const * params )const
	{
		EXEC_FUNCTION( Uniform4fv, location, count, params );
	}

	void OpenGlES3::SetUniform( int location, double v0 )const
	{
		EXEC_FUNCTION( Uniform1d, location, v0 );
	}

	void OpenGlES3::SetUniform( int location, double v0, double v1 )const
	{
		EXEC_FUNCTION( Uniform2d, location, v0, v1 );
	}

	void OpenGlES3::SetUniform( int location, double v0, double v1, double v2 )const
	{
		EXEC_FUNCTION( Uniform3d, location, v0, v1, v2 );
	}

	void OpenGlES3::SetUniform( int location, double v0, double v1, double v2, double v3 )const
	{
		EXEC_FUNCTION( Uniform4d, location, v0, v1, v2, v3 );
	}

	void OpenGlES3::SetUniform1v( int location, int count, double const * params )const
	{
		EXEC_FUNCTION( Uniform1dv, location, count, params );
	}

	void OpenGlES3::SetUniform2v( int location, int count, double const * params )const
	{
		EXEC_FUNCTION( Uniform2dv, location, count, params );
	}

	void OpenGlES3::SetUniform3v( int location, int count, double const * params )const
	{
		EXEC_FUNCTION( Uniform3dv, location, count, params );
	}

	void OpenGlES3::SetUniform4v( int location, int count, double const * params )const
	{
		EXEC_FUNCTION( Uniform4dv, location, count, params );
	}

	void OpenGlES3::SetUniform2x2v( int location, int count, bool transpose, float const * value )const
	{
		EXEC_FUNCTION( UniformMatrix2fv, location, count, uint8_t( transpose ), value );
	}

	void OpenGlES3::SetUniform2x3v( int location, int count, bool transpose, float const * value )const
	{
		EXEC_FUNCTION( UniformMatrix2x3fv, location, count, uint8_t( transpose ), value );
	}

	void OpenGlES3::SetUniform2x4v( int location, int count, bool transpose, float const * value )const
	{
		EXEC_FUNCTION( UniformMatrix2x4fv, location, count, uint8_t( transpose ), value );
	}

	void OpenGlES3::SetUniform3x2v( int location, int count, bool transpose, float const * value )const
	{
		EXEC_FUNCTION( UniformMatrix3x2fv, location, count, uint8_t( transpose ), value );
	}

	void OpenGlES3::SetUniform3x3v( int location, int count, bool transpose, float const * value )const
	{
		EXEC_FUNCTION( UniformMatrix3fv, location, count, uint8_t( transpose ), value );
	}

	void OpenGlES3::SetUniform3x4v( int location, int count, bool transpose, float const * value )const
	{
		EXEC_FUNCTION( UniformMatrix3x4fv, location, count, uint8_t( transpose ), value );
	}

	void OpenGlES3::SetUniform4x2v( int location, int count, bool transpose, float const * value )const
	{
		EXEC_FUNCTION( UniformMatrix4x2fv, location, count, uint8_t( transpose ), value );
	}

	void OpenGlES3::SetUniform4x3v( int location, int count, bool transpose, float const * value )const
	{
		EXEC_FUNCTION( UniformMatrix4x3fv, location, count, uint8_t( transpose ), value );
	}

	void OpenGlES3::SetUniform4x4v( int location, int count, bool transpose, float const * value )const
	{
		EXEC_FUNCTION( UniformMatrix4fv, location, count, uint8_t( transpose ), value );
	}

	void OpenGlES3::SetUniform2x2v( int location, int count, bool transpose, double const * value )const
	{
		EXEC_FUNCTION( UniformMatrix2dv, location, count, uint8_t( transpose ), value );
	}

	void OpenGlES3::SetUniform2x3v( int location, int count, bool transpose, double const * value )const
	{
		EXEC_FUNCTION( UniformMatrix2x3dv, location, count, uint8_t( transpose ), value );
	}

	void OpenGlES3::SetUniform2x4v( int location, int count, bool transpose, double const * value )const
	{
		EXEC_FUNCTION( UniformMatrix2x4dv, location, count, uint8_t( transpose ), value );
	}

	void OpenGlES3::SetUniform3x2v( int location, int count, bool transpose, double const * value )const
	{
		EXEC_FUNCTION( UniformMatrix3x2dv, location, count, uint8_t( transpose ), value );
	}

	void OpenGlES3::SetUniform3x3v( int location, int count, bool transpose, double const * value )const
	{
		EXEC_FUNCTION( UniformMatrix3dv, location, count, uint8_t( transpose ), value );
	}

	void OpenGlES3::SetUniform3x4v( int location, int count, bool transpose, double const * value )const
	{
		EXEC_FUNCTION( UniformMatrix3x4dv, location, count, uint8_t( transpose ), value );
	}

	void OpenGlES3::SetUniform4x2v( int location, int count, bool transpose, double const * value )const
	{
		EXEC_FUNCTION( UniformMatrix4x2dv, location, count, uint8_t( transpose ), value );
	}

	void OpenGlES3::SetUniform4x3v( int location, int count, bool transpose, double const * value )const
	{
		EXEC_FUNCTION( UniformMatrix4x3dv, location, count, uint8_t( transpose ), value );
	}

	void OpenGlES3::SetUniform4x4v( int location, int count, bool transpose, double const * value )const
	{
		EXEC_FUNCTION( UniformMatrix4dv, location, count, uint8_t( transpose ), value );
	}

	uint32_t OpenGlES3::GetUniformBlockIndex( uint32_t program, char const * uniformBlockName )const
	{
		return EXEC_FUNCTION( GetUniformBlockIndex, program, uniformBlockName );
	}

	void OpenGlES3::BindBufferBase( GlES3BufferTarget target, uint32_t index, uint32_t buffer )const
	{
		EXEC_FUNCTION( BindBufferBase, uint32_t( target ), index, buffer );
	}

	void OpenGlES3::UniformBlockBinding( uint32_t program, uint32_t uniformBlockIndex, uint32_t uniformBlockBinding )const
	{
		EXEC_FUNCTION( UniformBlockBinding, program, uniformBlockIndex, uniformBlockBinding );
	}

	void OpenGlES3::GetUniformIndices( uint32_t program, int uniformCount, char const ** uniformNames, uint32_t * uniformIndices )const
	{
		EXEC_FUNCTION( GetUniformIndices, program, uniformCount, uniformNames, uniformIndices );
	}

	void OpenGlES3::GetActiveUniformsiv( uint32_t program, int uniformCount, uint32_t const * uniformIndices, GlES3UniformValue pname, int * params )const
	{
		EXEC_FUNCTION( GetActiveUniformsiv, program, uniformCount, uniformIndices, uint32_t( pname ), params );
	}

	void OpenGlES3::GetActiveUniformBlockiv( uint32_t program, uint32_t uniformBlockIndex, GlES3UniformBlockValue pname, int * params )const
	{
		EXEC_FUNCTION( GetActiveUniformBlockiv, program, uniformBlockIndex, uint32_t( pname ), params );
	}

	uint32_t OpenGlES3::CreateShader( GlES3ShaderType type )const
	{
		return EXEC_FUNCTION( CreateShader, uint32_t( type ) );
	}

	void OpenGlES3::DeleteShader( uint32_t shader )const
	{
		EXEC_FUNCTION( DeleteShader, shader );
	}

	bool OpenGlES3::IsShader( uint32_t shader )const
	{
		return EXEC_FUNCTION( IsShader, shader ) != GL_FALSE;
	}

	void OpenGlES3::AttachShader( uint32_t program, uint32_t shader )const
	{
		EXEC_FUNCTION( AttachShader, program, shader );
	}

	void OpenGlES3::DetachShader( uint32_t program, uint32_t shader )const
	{
		EXEC_FUNCTION( DetachShader, program, shader );
	}

	void OpenGlES3::CompileShader( uint32_t shader )const
	{
		EXEC_FUNCTION( CompileShader, shader );
	}

	void OpenGlES3::GetShaderiv( uint32_t shader, GlES3ShaderStatus pname, int * param )const
	{
		EXEC_FUNCTION( GetShaderiv, shader, uint32_t( pname ), param );
	}

	void OpenGlES3::GetShaderInfoLog( uint32_t shader, int bufSize, int * length, char * infoLog )const
	{
		EXEC_FUNCTION( GetShaderInfoLog, shader, bufSize, length, infoLog );
	}

	void OpenGlES3::ShaderSource( uint32_t shader, int count, char const ** strings, int const * lengths )const
	{
		EXEC_FUNCTION( ShaderSource, shader, count, strings, lengths );
	}

	void OpenGlES3::GetActiveAttrib( uint32_t program, uint32_t index, int bufSize, int * length, int * size, uint32_t * type, char * name )const
	{
		EXEC_FUNCTION( GetActiveAttrib, program, index, bufSize, length, size, type, name );
	}

	void OpenGlES3::GetActiveUniform( uint32_t program, uint32_t index, int bufSize, int * length, int * size, uint32_t * type, char * name )const
	{
		EXEC_FUNCTION( GetActiveUniform, program, index, bufSize, length, size, type, name );
	}

	uint32_t OpenGlES3::CreateProgram()const
	{
		return EXEC_VOID_FUNCTION( CreateProgram );
	}

	void OpenGlES3::DeleteProgram( uint32_t program )const
	{
		EXEC_FUNCTION( DeleteProgram, program );
	}

	bool OpenGlES3::IsProgram( uint32_t program )const
	{
		return EXEC_FUNCTION( IsProgram, program ) != GL_FALSE;
	}

	void OpenGlES3::LinkProgram( uint32_t program )const
	{
		EXEC_FUNCTION( LinkProgram, program );
	}

	void OpenGlES3::UseProgram( uint32_t program )const
	{
		EXEC_FUNCTION( UseProgram, program );
	}

	void OpenGlES3::GetProgramiv( uint32_t program, GlES3ShaderStatus pname, int * param )const
	{
		EXEC_FUNCTION( GetProgramiv, program, uint32_t( pname ), param );
	}

	void OpenGlES3::GetProgramInfoLog( uint32_t program, int bufSize, int * length, char * infoLog )const
	{
		EXEC_FUNCTION( GetProgramInfoLog, program, bufSize, length, infoLog );
	}

	int OpenGlES3::GetAttribLocation( uint32_t program, char const * name )const
	{
		return EXEC_FUNCTION( GetAttribLocation, program, name );
	}

	void OpenGlES3::ProgramParameteri( uint32_t program, uint32_t pname, int value )const
	{
		EXEC_FUNCTION( ProgramParameteri, program, pname, value );
	}

	void OpenGlES3::DispatchCompute( uint32_t num_groups_x, uint32_t num_groups_y, uint32_t num_groups_z )const
	{
		EXEC_FUNCTION( DispatchCompute, num_groups_x, num_groups_y, num_groups_z );
	}

	void OpenGlES3::DispatchComputeGroupSize( uint32_t num_groups_x, uint32_t num_groups_y, uint32_t num_groups_z, uint32_t work_group_size_x, uint32_t work_group_size_y, uint32_t work_group_size_z )const
	{
		EXEC_FUNCTION( DispatchComputeGroupSize, num_groups_x, num_groups_y, num_groups_z, work_group_size_x, work_group_size_y, work_group_size_z );
	}

	void OpenGlES3::ShaderStorageBlockBinding( uint32_t shader, uint32_t storageBlockIndex, uint32_t storageBlockBinding )const
	{
		EXEC_FUNCTION( ShaderStorageBlockBinding, shader, storageBlockIndex, storageBlockBinding );
	}

	void OpenGlES3::GetProgramInterfaceInfos( uint32_t program, GlslInterface programInterface, GlslDataName name, int * params )
	{
		EXEC_FUNCTION( GetProgramInterfaceiv, program, uint32_t( programInterface ), uint32_t( name ), params );
	}

	int OpenGlES3::GetProgramResourceIndex( uint32_t program, GlslInterface programInterface, char const * const name )
	{
		return EXEC_FUNCTION( GetProgramResourceIndex, program, uint32_t( programInterface ), name );
	}

	int OpenGlES3::GetProgramResourceLocation( uint32_t program, GlslInterface programInterface, char const * const name )
	{
		return EXEC_FUNCTION( GetProgramResourceLocation, program, uint32_t( programInterface ), name );
	}

	int OpenGlES3::GetProgramResourceLocationIndex( uint32_t program, GlslInterface programInterface, char const * const name )
	{
		return EXEC_FUNCTION( GetProgramResourceLocationIndex, program, uint32_t( programInterface ), name );
	}

	void OpenGlES3::GetProgramResourceName( uint32_t program, GlslInterface programInterface, uint32_t index, int bufSize, int * length, char * name )
	{
		EXEC_FUNCTION( GetProgramResourceName, program, uint32_t( programInterface ), index, bufSize, length, name );
	}

	void OpenGlES3::GetProgramResourceInfos( uint32_t program, GlslInterface programInterface, uint32_t index, int propCount, uint32_t * props, int bufSize, int * length, int * params )
	{
		EXEC_FUNCTION( GetProgramResourceiv, program, uint32_t( programInterface ), index, propCount, props, bufSize, length, params );
	}

	void OpenGlES3::MemoryBarrier( Castor::FlagCombination< GlES3BarrierBit > const & barriers )const
	{
		EXEC_FUNCTION( MemoryBarrier, uint32_t( barriers ) );
	}

	void OpenGlES3::MemoryBarrierByRegion( Castor::FlagCombination< GlES3BarrierBit > const & barriers )const
	{
		EXEC_FUNCTION( MemoryBarrierByRegion, uint32_t( barriers ) );
	}

	void OpenGlES3::EnableVertexAttribArray( uint32_t index )const
	{
		EXEC_FUNCTION( EnableVertexAttribArray, index );
	}

	void OpenGlES3::VertexAttribPointer( uint32_t index, int size, GlES3Type type, bool normalized, int stride, void const * pointer )const
	{
		EXEC_FUNCTION( VertexAttribPointer, index, size, uint32_t( type ), uint8_t( normalized ), stride, pointer );
	}

	void OpenGlES3::VertexAttribPointer( uint32_t index, int size, GlES3Type type, int stride, void const * pointer )const
	{
		EXEC_FUNCTION( VertexAttribIPointer, index, size, uint32_t( type ), stride, pointer );
	}

	void OpenGlES3::DisableVertexAttribArray( uint32_t index )const
	{
		EXEC_FUNCTION( DisableVertexAttribArray, index );
	}

	void OpenGlES3::GenVertexArrays( int n, uint32_t * arrays )const
	{
		EXEC_FUNCTION( GenVertexArrays, n, arrays );
	}

	bool OpenGlES3::IsVertexArray( uint32_t array )const
	{
		return EXEC_FUNCTION( IsVertexArray, array ) != GL_FALSE;
	}

	void OpenGlES3::BindVertexArray( uint32_t array )const
	{
		EXEC_FUNCTION( BindVertexArray, array );
	}

	void OpenGlES3::DeleteVertexArrays( int n, uint32_t const * arrays )const
	{
		EXEC_FUNCTION( DeleteVertexArrays, n, arrays );
	}

	void OpenGlES3::PatchParameter( GlES3PatchParameter p_param, int p_iValue )const
	{
		EXEC_FUNCTION( PatchParameteri, uint32_t( p_param ), p_iValue );
	}

	void OpenGlES3::GenQueries( int p_n, uint32_t * p_queries )const
	{
		EXEC_FUNCTION( GenQueries, p_n, p_queries );
	}

	void OpenGlES3::DeleteQueries( int p_n, uint32_t const * p_queries )const
	{
		EXEC_FUNCTION( DeleteQueries, p_n, p_queries );
	}

	bool OpenGlES3::IsQuery( uint32_t p_query )const
	{
		return EXEC_FUNCTION( IsQuery, p_query ) != GL_FALSE;
	}

	void OpenGlES3::BeginQuery( GlES3QueryType p_target, uint32_t p_query )const
	{
		EXEC_FUNCTION( BeginQuery, uint32_t( p_target ), p_query );
	}

	void OpenGlES3::EndQuery( GlES3QueryType p_target )const
	{
		EXEC_FUNCTION( EndQuery, uint32_t( p_target ) );
	}

	void OpenGlES3::QueryCounter( uint32_t p_id, GlES3QueryType p_target )const
	{
		EXEC_FUNCTION( QueryCounter, p_id, uint32_t( p_target ) );
	}

	void OpenGlES3::GetQueryObjectInfos( uint32_t p_id, GlES3QueryInfo p_name, int32_t * p_params )const
	{
		EXEC_FUNCTION( GetQueryObjectiv, p_id, uint32_t( p_name ), p_params );
	}

	void OpenGlES3::GetQueryObjectInfos( uint32_t p_id, GlES3QueryInfo p_name, uint32_t * p_params )const
	{
		EXEC_FUNCTION( GetQueryObjectuiv, p_id, uint32_t( p_name ), p_params );
	}

	void OpenGlES3::GetQueryObjectInfos( uint32_t p_id, GlES3QueryInfo p_name, int64_t * p_params )const
	{
		EXEC_FUNCTION( GetQueryObjecti64v, p_id, uint32_t( p_name ), p_params );
	}

	void OpenGlES3::GetQueryObjectInfos( uint32_t p_id, GlES3QueryInfo p_name, uint64_t * p_params )const
	{
		EXEC_FUNCTION( GetQueryObjectui64v, p_id, uint32_t( p_name ), p_params );
	}

	//*************************************************************************************************
}
