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
		static inline RetT Call( OpenGlES3 const & p_gl, RetT( *p_function )( ParamsT ... ), char const * const p_name, ParamsT const & ... p_params )
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
		static inline void Call( OpenGlES3 const & p_gl, void( *p_function )( ParamsT ... ), char const * const p_name, ParamsT const & ... p_params )
		{
			REQUIRE( p_function );
			p_function( p_params... );
			glCheckError( p_gl, p_name );
		}
	};

	template< typename RetT >
	struct GlES3FuncCaller< RetT, void >
	{
		static inline void Call( OpenGlES3 const & p_gl, RetT( *p_function )(), char const * const p_name )
		{
			REQUIRE( p_function );
			p_function();
			glCheckError( p_gl, p_name );
		}
	};

	template< typename RetT, typename ... ParamsT >
	inline RetT ExecuteFunction( OpenGlES3 const & p_gl, RetT( *p_function )( ParamsT ... ), char const * const p_name, ParamsT const & ... p_params )
	{
		return GlES3FuncCaller< RetT, ParamsT... >::Call( p_gl, p_function, p_name, p_params... );
	}

#	define EXEC_FUNCTION( Name, ... )\
	ExecuteFunction( GetOpenGlES3(), gl##Name, "gl"#Name, __VA_ARGS__ )

#	define EXEC_VOID_FUNCTION( Name )\
	ExecuteFunction( GetOpenGlES3(), gl##Name, "gl"#Name )

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
		EXEC_FUNCTION( ClearDepthf, float( value ) );
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
		EXEC_FUNCTION( GetInteger64v, pname, reinterpret_cast< int64_t * >( params ) );
	}

	void OpenGlES3::MakeCurrent( EGLDisplay display, EGLSurface draw, EGLSurface read, EGLContext context )const
	{
		eglMakeCurrent( display, draw, read, context );
	}

	void OpenGlES3::SwapBuffers( EGLDisplay display, EGLSurface surface )const
	{
		eglSwapBuffers( display, surface );
	}

	void OpenGlES3::SwapInterval( EGLDisplay display, int interval )const
	{
		eglSwapInterval( display, interval );
	}

	EGLContext OpenGlES3::CreateContext( EGLDisplay display, EGLConfig config, EGLContext share, int * attribs )const
	{
		return eglCreateContext( display, config, share, attribs );
	}

	bool OpenGlES3::DeleteContext( EGLDisplay display, EGLContext context )const
	{
		eglDestroyContext( display, context );
	}

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

	void OpenGlES3::ActiveTexture( GlES3TextureIndex target )const
	{
		EXEC_FUNCTION( ActiveTexture, uint32_t( target ) );
	}

	void OpenGlES3::GenerateMipmap( GlES3TexDim mode )const
	{
		EXEC_FUNCTION( GenerateMipmap, uint32_t( mode ) );
	}

	void OpenGlES3::BindTexture( GlES3TexDim mode, uint32_t texture )const
	{
		EXEC_FUNCTION( BindTexture, uint32_t( mode ), texture );
	}

	void OpenGlES3::TexSubImage1D( GlES3TextureStorageType mode, int level, int xoffset, int width, GlES3Format format, GlES3Type type, void const * data )const
	{
		EXEC_FUNCTION( TexSubImage2D, uint32_t( mode ), level, xoffset, 0, width, 1, uint32_t( format ), uint32_t( type ), data );
	}

	void OpenGlES3::TexSubImage2D( GlES3TextureStorageType mode, int level, int xoffset, int yoffset, int width, int height, GlES3Format format, GlES3Type type, void const * data )const
	{
		EXEC_FUNCTION( TexSubImage2D, uint32_t( mode ), level, xoffset, yoffset, width, height, uint32_t( format ), uint32_t( type ), data );
	}

	void OpenGlES3::TexSubImage2D( GlES3TextureStorageType mode, int level, Castor::Position const & p_position, Castor::Size const & p_size, GlES3Format format, GlES3Type type, void const * data )const
	{
		EXEC_FUNCTION( TexSubImage2D, uint32_t( mode ), level, int( p_position.x() ), int( p_position.y() ), int( p_size.width() ), int( p_size.height() ), uint32_t( format ), uint32_t( type ), data );
	}

	void OpenGlES3::TexSubImage2D( GlES3TextureStorageType mode, int level, Castor::Rectangle const & p_rect, GlES3Format format, GlES3Type type, void const * data )const
	{
		EXEC_FUNCTION( TexSubImage2D, uint32_t( mode ), level, p_rect.left(), p_rect.top(), p_rect.width(), p_rect.height(), uint32_t( format ), uint32_t( type ), data );
	}

	void OpenGlES3::TexSubImage3D( GlES3TextureStorageType mode, int level, int xoffset, int yoffset, int zoffset, int width, int height, int depth, GlES3Format format, GlES3Type type, void const * data )const
	{
		EXEC_FUNCTION( TexSubImage3D, uint32_t( mode ), level, xoffset, yoffset, zoffset, width, height, depth, uint32_t( format ), uint32_t( type ), data );
	}

	void OpenGlES3::TexImage1D( GlES3TextureStorageType mode, int level, GlES3Internal internalFormat, int width, int border, GlES3Format format, GlES3Type type, void const * data )const
	{
		EXEC_FUNCTION( TexImage2D, uint32_t( mode ), level, int( internalFormat ), width, 1, border, uint32_t( format ), uint32_t( type ), data );
	}

	void OpenGlES3::TexImage2D( GlES3TextureStorageType mode, int level, GlES3Internal internalFormat, int width, int height, int border, GlES3Format format, GlES3Type type, void const * data )const
	{
		EXEC_FUNCTION( TexImage2D, uint32_t( mode ), level, int( internalFormat ), width, height, border, uint32_t( format ), uint32_t( type ), data );
	}

	void OpenGlES3::TexImage2D( GlES3TextureStorageType mode, int level, GlES3Internal internalFormat, Castor::Size const & p_size, int border, GlES3Format format, GlES3Type type, void const * data )const
	{
		EXEC_FUNCTION( TexImage2D, uint32_t( mode ), level, int( internalFormat ), int( p_size.width() ), int( p_size.height() ), border, uint32_t( format ), uint32_t( type ), data );
	}

	void OpenGlES3::TexImage3D( GlES3TextureStorageType mode, int level, GlES3Internal internalFormat, int width, int height, int depth, int border, GlES3Format format, GlES3Type type, void const * data )const
	{
		EXEC_FUNCTION( TexImage3D, uint32_t( mode ), level, int( internalFormat ), width, height, depth, border, uint32_t( format ), uint32_t( type ), data );
	}

	void OpenGlES3::BlendFunc( GlES3BlendFactor sfactor, GlES3BlendFactor dfactor )const
	{
		EXEC_FUNCTION( BlendFunc, uint32_t( sfactor ), uint32_t( dfactor ) );
	}

	void OpenGlES3::BlendFunc( GlES3BlendFactor p_eRgbSrc, GlES3BlendFactor p_eRgbDst, GlES3BlendFactor p_eAlphaSrc, GlES3BlendFactor p_eAlphaDst )const
	{
		EXEC_FUNCTION( BlendFuncSeparate, uint32_t( p_eRgbSrc ), uint32_t( p_eRgbDst ), uint32_t( p_eAlphaSrc ), uint32_t( p_eAlphaDst ) );
	}

	void OpenGlES3::BlendEquation( GlES3BlendOp p_eOp )const
	{
		EXEC_FUNCTION( BlendEquation, uint32_t( p_eOp ) );
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

	void OpenGlES3::PixelStore( GlES3StorageMode p_mode, int p_iParam )const
	{
		EXEC_FUNCTION( PixelStorei, uint32_t( p_mode ), p_iParam );
	}

	void OpenGlES3::TexStorage1D( GlES3TextureStorageType target, GLint levels, GlES3Internal internalformat, GLsizei width )const
	{
		EXEC_FUNCTION( TexStorage2D, GLenum( target ), GLsizei( levels ), GLenum( internalformat ), width, 1 );
	}

	void OpenGlES3::TexStorage2D( GlES3TextureStorageType target, GLint levels, GlES3Internal internalformat, GLsizei width, GLsizei height )const
	{
		EXEC_FUNCTION( TexStorage2D, GLenum( target ), GLsizei( levels ), GLenum( internalformat ), width, height );
	}

	void OpenGlES3::TexStorage3D( GlES3TextureStorageType target, GLint levels, GlES3Internal internalformat, GLsizei width, GLsizei height, GLsizei depth )const
	{
		EXEC_FUNCTION( TexStorage3D, GLenum( target ), GLsizei( levels ), GLenum( internalformat ), width, height, depth );
	}

	void OpenGlES3::TexStorage2DMultisample( GlES3TextureStorageType target, GLsizei samples, GlES3Internal internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations )const
	{
		FAILURE( "glTexStorage2DMultisample Not supported in OpenGL ES 3.X" );
	}

	void OpenGlES3::TexStorage3DMultisample( GlES3TextureStorageType target, GLsizei samples, GlES3Internal internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations )const
	{
		FAILURE( "glTexStorage3DMultisample Not supported in OpenGL ES 3.X" );
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

	void OpenGlES3::GetSamplerParameter( uint32_t sampler, GlES3SamplerParameter pname, float * params )const
	{
		EXEC_FUNCTION( GetSamplerParameterfv, sampler, uint32_t( pname ), params );
	}

	void OpenGlES3::GetSamplerParameter( uint32_t sampler, GlES3SamplerParameter pname, int * params )const
	{
		EXEC_FUNCTION( GetSamplerParameteriv, sampler, uint32_t( pname ), params );
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
		FAILURE( "glTexBuffer Not supported in OpenGL ES 3.X" );
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
		EXEC_FUNCTION( BindBuffer, uint32_t( p_target ), buffer );
	}

	void OpenGlES3::BufferData( GlES3BufferTarget p_target, ptrdiff_t size, void const * data, GlES3BufferMode usage )const
	{
		EXEC_FUNCTION( BufferData, uint32_t( p_target ), GLsizeiptr( size ), data, uint32_t( usage ) );
	}

	void OpenGlES3::CopyBufferSubData( GlES3BufferTarget readtarget, GlES3BufferTarget writetarget, ptrdiff_t readoffset, ptrdiff_t writeoffset, ptrdiff_t size )const
	{
		EXEC_FUNCTION( CopyBufferSubData, uint32_t( readtarget ), uint32_t( writetarget ), GLintptr( readoffset ), GLintptr( writeoffset ), GLsizeiptr( size ) );
	}

	void OpenGlES3::BufferSubData( GlES3BufferTarget p_target, ptrdiff_t offset, ptrdiff_t size, void const * data )const
	{
		EXEC_FUNCTION( BufferSubData, uint32_t( p_target ), GLintptr( offset ), GLsizeiptr( size ), data );
	}

	void OpenGlES3::GetBufferParameter( GlES3BufferTarget p_target, GlES3BufferParameter pname, int * params )const
	{
		EXEC_FUNCTION( GetBufferParameteriv, uint32_t( p_target ), uint32_t( pname ), params );
	}

	void OpenGlES3::UnmapBuffer( GlES3BufferTarget p_target )const
	{
		EXEC_FUNCTION( UnmapBuffer, uint32_t( p_target ) );
	}

	void * OpenGlES3::MapBufferRange( GlES3BufferTarget p_target, ptrdiff_t offset, ptrdiff_t length, uint32_t access )const
	{
		return EXEC_FUNCTION( MapBufferRange, GLenum( p_target ), GLintptr( offset ), GLsizeiptr( length ), GLbitfield( access ) );
	}

	void OpenGlES3::FlushMappedBufferRange( GlES3BufferTarget p_target, ptrdiff_t offset, ptrdiff_t length )const
	{
		EXEC_FUNCTION( FlushMappedBufferRange, GLenum( p_target ), GLintptr( offset ), GLsizeiptr( length ) );
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
		EXEC_FUNCTION( BindTransformFeedback, GLenum( target ), GLuint( buffer ) );
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

	void OpenGlES3::TransformFeedbackVaryings( uint32_t program, int count, char const * const * varyings, GlES3AttributeLayout bufferMode )const
	{
		EXEC_FUNCTION( TransformFeedbackVaryings, program, GLsizei( count ), varyings, GLenum( bufferMode ) );
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

	void OpenGlES3::FramebufferTexture2D( GlES3FrameBufferMode p_target, GlES3AttachmentPoint p_eAttachment, GlES3TexDim p_texTarget, uint32_t texture, int level )const
	{
		EXEC_FUNCTION( FramebufferTexture2D, uint32_t( p_target ), uint32_t( p_eAttachment ), uint32_t( p_texTarget ), texture, level );
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
		FAILURE( "glTexImage2DMultisample not supported by OpenGL ES 3" );
	}

	void OpenGlES3::TexImage2DMultisample( GlES3TextureStorageType p_target, int p_iSamples, GlES3Internal p_internalFormat, Castor::Size const & p_size, bool p_bFixedSampleLocations )const
	{
		FAILURE( "glTexImage2DMultisample not supported by OpenGL ES 3" );
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

	void OpenGlES3::GetUniformIndices( uint32_t program, int uniformCount, char const * const * uniformNames, uint32_t * uniformIndices )const
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

	void OpenGlES3::ShaderSource( uint32_t shader, int count, char const * const * strings, int const * lengths )const
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

	void OpenGlES3::GetQueryObjectInfos( uint32_t p_id, GlES3QueryInfo p_name, int32_t * p_params )const
	{
		EXEC_FUNCTION( GetQueryiv, p_id, uint32_t( p_name ), p_params );
	}

	void OpenGlES3::GetQueryObjectInfos( uint32_t p_id, GlES3QueryInfo p_name, uint32_t * p_params )const
	{
		EXEC_FUNCTION( GetQueryObjectuiv, p_id, uint32_t( p_name ), p_params );
	}

	//*************************************************************************************************
}
