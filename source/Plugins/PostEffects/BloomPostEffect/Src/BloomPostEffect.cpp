#include "BloomPostEffect.hpp"

#include <Engine.hpp>
#include <Cache/SamplerCache.hpp>
#include <Cache/ShaderCache.hpp>

#include <FrameBuffer/BackBuffers.hpp>
#include <FrameBuffer/FrameBufferAttachment.hpp>
#include <FrameBuffer/TextureAttachment.hpp>
#include <Mesh/Vertex.hpp>
#include <Mesh/Buffer/BufferDeclaration.hpp>
#include <Mesh/Buffer/BufferElementDeclaration.hpp>
#include <Mesh/Buffer/GeometryBuffers.hpp>
#include <Mesh/Buffer/VertexBuffer.hpp>
#include <Miscellaneous/Parameter.hpp>
#include <Render/Context.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Render/RenderTarget.hpp>
#include <Render/RenderWindow.hpp>
#include <Render/Viewport.hpp>
#include <Shader/UniformBuffer.hpp>
#include <Shader/ShaderProgram.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureLayout.hpp>
#include <Texture/TextureUnit.hpp>

#include <Graphics/Image.hpp>

#include <numeric>

#include <GlslSource.hpp>

using namespace Castor;
using namespace Castor3D;

namespace Bloom
{
	namespace
	{
		GLSL::Shader GetVertexProgram( RenderSystem * p_renderSystem )
		{
			using namespace GLSL;
			GlslWriter writer = p_renderSystem->CreateGlslWriter();

			UBO_MATRIX( writer );

			// Shader inputs
			Vec2 position = writer.DeclAttribute< Vec2 >( ShaderProgram::Position );

			// Shader outputs
			auto vtx_texture = writer.DeclOutput< Vec2 >( cuT( "vtx_texture" ) );
			auto gl_Position = writer.DeclBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				vtx_texture = position;
				gl_Position = c3d_mtxProjection * vec4( position.xy(), 0.0, 1.0 );
			} );
			return writer.Finalise();
		}

		GLSL::Shader GetHiPassProgram( RenderSystem * p_renderSystem )
		{
			using namespace GLSL;
			GlslWriter writer = p_renderSystem->CreateGlslWriter();

			// Shader inputs
			auto c3d_mapDiffuse = writer.DeclUniform< Sampler2D >( ShaderProgram::MapDiffuse );
			auto vtx_texture = writer.DeclInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto pxl_v4FragColor = writer.DeclFragData< Vec4 >( cuT( "pxl_v4FragColor" ), 0 );

			writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				pxl_v4FragColor = vec4( texture( c3d_mapDiffuse, vec2( vtx_texture.x(), vtx_texture.y() ) ).xyz(), 1.0 );
				auto maxComponent = writer.DeclLocale( cuT( "maxComponent" ), GLSL::max( pxl_v4FragColor.r(), pxl_v4FragColor.g() ) );
				maxComponent = GLSL::max( maxComponent, pxl_v4FragColor.b() );

				IF( writer, maxComponent > 1.0_f )
				{
					pxl_v4FragColor.xyz() /= maxComponent;
				}
				ELSE
				{
					pxl_v4FragColor.xyz() = vec3( 0.0_f, 0.0_f, 0.0_f );
				}
				FI;
			} );
			return writer.Finalise();
		}

		GLSL::Shader GetBlurXProgram( RenderSystem * p_renderSystem )
		{
			using namespace GLSL;
			GlslWriter writer = p_renderSystem->CreateGlslWriter();

			// Shader inputs
			Ubo config{ writer, BloomPostEffect::FilterConfig, 2u };
			auto c3d_fCoefficients = config.DeclMember< Float >( BloomPostEffect::FilterConfigCoefficients, BloomPostEffect::MaxCoefficients );
			auto c3d_fCoefficientsCount = config.DeclMember< UInt >( BloomPostEffect::FilterConfigCoefficientsCount );
			config.End();
			auto c3d_mapDiffuse = writer.DeclUniform< Sampler2D >( ShaderProgram::MapDiffuse );
			auto vtx_texture = writer.DeclInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto plx_v4FragColor = writer.DeclFragData< Vec4 >( cuT( "plx_v4FragColor" ), 0 );

			writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				auto base = writer.DeclLocale( cuT( "base" ), vec2( 1.0_f, 0 ) / textureSize( c3d_mapDiffuse, 0 ) );
				auto offset = writer.DeclLocale( cuT( "offset" ), vec2( 0.0_f, 0 ) );
				plx_v4FragColor = texture( c3d_mapDiffuse, vtx_texture ) * c3d_fCoefficients[0];

				FOR( writer, Int, i, 1, cuT( "i < c3d_fCoefficientsCount" ), cuT( "++i" ) )
				{
					offset += base;
					plx_v4FragColor += c3d_fCoefficients[i] * texture( c3d_mapDiffuse, vtx_texture - offset );
					plx_v4FragColor += c3d_fCoefficients[i] * texture( c3d_mapDiffuse, vtx_texture + offset );
				}
				ROF;
			} );
			return writer.Finalise();
		}

		GLSL::Shader GetBlurYProgram( RenderSystem * p_renderSystem )
		{
			using namespace GLSL;
			GlslWriter writer = p_renderSystem->CreateGlslWriter();

			// Shader inputs
			Ubo config{ writer, BloomPostEffect::FilterConfig, 2u };
			auto c3d_fCoefficients = config.DeclMember< Float >( BloomPostEffect::FilterConfigCoefficients, BloomPostEffect::MaxCoefficients );
			auto c3d_fCoefficientsCount = config.DeclMember< UInt >( BloomPostEffect::FilterConfigCoefficientsCount );
			config.End();
			auto c3d_mapDiffuse = writer.DeclUniform< Sampler2D >( ShaderProgram::MapDiffuse );
			auto vtx_texture = writer.DeclInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto plx_v4FragColor = writer.DeclFragData< Vec4 >( cuT( "plx_v4FragColor" ), 0 );

			writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				auto base = writer.DeclLocale( cuT( "base" ), vec2( 0.0_f, 1 ) / textureSize( c3d_mapDiffuse, 0 ) );
				auto offset = writer.DeclLocale( cuT( "offset" ), vec2( 0.0_f, 0 ) );
				plx_v4FragColor = texture( c3d_mapDiffuse, vtx_texture ) * c3d_fCoefficients[0];

				FOR( writer, Int, i, 1, cuT( "i < c3d_fCoefficientsCount" ), cuT( "++i" ) )
				{
					offset += base;
					plx_v4FragColor += c3d_fCoefficients[i] * texture( c3d_mapDiffuse, vtx_texture - offset );
					plx_v4FragColor += c3d_fCoefficients[i] * texture( c3d_mapDiffuse, vtx_texture + offset );
				}
				ROF;
			} );
			return writer.Finalise();
		}

		GLSL::Shader GetCombineProgram( RenderSystem * p_renderSystem )
		{
			using namespace GLSL;
			GlslWriter writer = p_renderSystem->CreateGlslWriter();

			// Shader inputs
			auto c3d_mapPass0 = writer.DeclUniform< Sampler2D >( BloomPostEffect::CombineMapPass0 );
			auto c3d_mapPass1 = writer.DeclUniform< Sampler2D >( BloomPostEffect::CombineMapPass1 );
			auto c3d_mapPass2 = writer.DeclUniform< Sampler2D >( BloomPostEffect::CombineMapPass2 );
			auto c3d_mapPass3 = writer.DeclUniform< Sampler2D >( BloomPostEffect::CombineMapPass3 );
			auto c3d_mapScene = writer.DeclUniform< Sampler2D >( BloomPostEffect::CombineMapScene );
			auto vtx_texture = writer.DeclInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto plx_v4FragColor = writer.DeclFragData< Vec4 >( cuT( "plx_v4FragColor" ), 0 );

			writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				plx_v4FragColor = texture( c3d_mapPass0, vtx_texture );
				plx_v4FragColor += texture( c3d_mapPass1, vtx_texture );
				plx_v4FragColor += texture( c3d_mapPass2, vtx_texture );
				plx_v4FragColor += texture( c3d_mapPass3, vtx_texture );
				plx_v4FragColor += texture( c3d_mapScene, vtx_texture );
			} );
			return writer.Finalise();
		}
	}

	std::vector< float > GetHalfPascal( uint32_t p_height )
	{
		std::vector< float > result;
		result.resize( p_height );
		auto x = 1.0f;
		auto max = 1 + p_height;

		for ( uint32_t i = 0u; i <= max; ++i )
		{
			auto index = max - i;

			if ( index < p_height )
			{
				result[index] = x;
			}

			x = x * ( ( p_height + 1 ) * 2 - i ) / ( i + 1 );
		}

		return result;
	}

	//*********************************************************************************************

	String const BloomPostEffect::Type = cuT( "bloom" );
	String const BloomPostEffect::Name = cuT( "Bloom PostEffect" );
	String const BloomPostEffect::FilterConfig = cuT( "FilterConfig" );
	String const BloomPostEffect::FilterConfigCoefficients = cuT( "c3d_fCoefficients" );
	String const BloomPostEffect::FilterConfigCoefficientsCount = cuT( "c3d_fCoefficientsCount" );
	String const BloomPostEffect::CombineMapPass0 = cuT( "c3d_mapPass0" );
	String const BloomPostEffect::CombineMapPass1 = cuT( "c3d_mapPass1" );
	String const BloomPostEffect::CombineMapPass2 = cuT( "c3d_mapPass2" );
	String const BloomPostEffect::CombineMapPass3 = cuT( "c3d_mapPass3" );
	String const BloomPostEffect::CombineMapScene = cuT( "c3d_mapScene" );

	BloomPostEffect::BloomPostEffect( RenderTarget & p_renderTarget
		, RenderSystem & p_renderSystem
		, Parameters const & p_param )
		: PostEffect( BloomPostEffect::Type
			, p_renderTarget
			, p_renderSystem
			, p_param )
		, m_viewport{ *p_renderSystem.GetEngine() }
		, m_size( 5u )
		, m_matrixUbo{ *p_renderSystem.GetEngine() }
		, m_blurXUbo{ BloomPostEffect::FilterConfig, p_renderSystem }
		, m_blurYUbo{ BloomPostEffect::FilterConfig, p_renderSystem }
		, m_declaration(
		{
			BufferElementDeclaration( ShaderProgram::Position
				, uint32_t( ElementUsage::ePosition )
				, ElementType::eVec2 ),
		} )
		, m_hiPassSurfaces(
		{
			{
				PostEffectSurface{ *p_renderSystem.GetEngine() },
				PostEffectSurface{ *p_renderSystem.GetEngine() },
				PostEffectSurface{ *p_renderSystem.GetEngine() },
				PostEffectSurface{ *p_renderSystem.GetEngine() }
			}
		} )
		, m_blurSurfaces(
		{
			{
				PostEffectSurface{ *p_renderSystem.GetEngine() },
				PostEffectSurface{ *p_renderSystem.GetEngine() },
				PostEffectSurface{ *p_renderSystem.GetEngine() },
				PostEffectSurface{ *p_renderSystem.GetEngine() }
			}
		} )
	{
		m_blurXCoeffs = m_blurXUbo.CreateUniform< UniformType::eFloat >( BloomPostEffect::FilterConfigCoefficients
			, BloomPostEffect::MaxCoefficients );
		m_blurXCoeffCount = m_blurXUbo.CreateUniform< UniformType::eUInt >( BloomPostEffect::FilterConfigCoefficientsCount );

		m_blurYCoeffs = m_blurYUbo.CreateUniform< UniformType::eFloat >( BloomPostEffect::FilterConfigCoefficients
			, BloomPostEffect::MaxCoefficients );
		m_blurYCoeffCount = m_blurYUbo.CreateUniform< UniformType::eUInt >( BloomPostEffect::FilterConfigCoefficientsCount );

		String count;

		if ( p_param.Get( cuT( "Size" ), count ) )
		{
			m_size = uint32_t( string::to_long( count ) );
		}

		// Normalize kernel coefficients
		m_kernel = GetHalfPascal( m_size );
		float sum = m_kernel[0];

		for ( uint32_t i = 1u; i < m_size; ++i )
		{
			sum += m_kernel[i] * 2;
		}

		std::transform( std::begin( m_kernel )
			, std::end( m_kernel )
			, std::begin( m_kernel )
			, [&sum]( float & p_value )
			{
				return p_value /= sum;
			} );

		real pBuffer[] =
		{
			0, 0,
			1, 1,
			0, 1,
			0, 0,
			1, 0,
			1, 1,
		};

		std::memcpy( m_buffer, pBuffer, sizeof( pBuffer ) );
		uint32_t i = 0;

		for ( auto & vertex : m_vertices )
		{
			vertex = std::make_shared< BufferElementGroup >( &reinterpret_cast< uint8_t * >( m_buffer )[i++ * m_declaration.stride()] );
		}

		m_linearSampler = DoCreateSampler( true );
		m_nearestSampler = DoCreateSampler( false );
	}

	BloomPostEffect::~BloomPostEffect()
	{
	}

	PostEffectSPtr BloomPostEffect::Create( RenderTarget & p_renderTarget
		, RenderSystem & p_renderSystem
		, Parameters const & p_param )
	{
		return std::make_shared< BloomPostEffect >( p_renderTarget
			, p_renderSystem
			, p_param );
	}

	bool BloomPostEffect::Initialise()
	{
		bool result = false;
		m_viewport.Initialise();

		result = DoInitialiseHiPassProgram();

		if ( result )
		{
			result = DoInitialiseBlurXProgram();
		}

		if ( result )
		{
			result = DoInitialiseBlurYProgram();
		}

		if ( result )
		{
			result = DoInitialiseCombineProgram();
		}

		Size size = m_renderTarget.GetSize();
		uint32_t index = 0;

		for ( auto & surface : m_hiPassSurfaces )
		{
			surface.Initialise( m_renderTarget
				, size
				, index++
				, m_linearSampler );
			size.width() >>= 1;
			size.height() >>= 1;
		}

		size = m_renderTarget.GetSize();
		index = 0;

		for ( auto & surface : m_blurSurfaces )
		{
			surface.Initialise( m_renderTarget
				, size
				, index++
				, m_nearestSampler );
			size.width() >>= 1;
			size.height() >>= 1;
		}

		return result;
	}

	void BloomPostEffect::Cleanup()
	{
		m_matrixUbo.GetUbo().Cleanup();
		m_blurXUbo.Cleanup();
		m_blurYUbo.Cleanup();

		m_hiPassMapDiffuse.reset();
		m_blurXCoeffCount.reset();
		m_blurXCoeffs.reset();
		m_blurXMapDiffuse.reset();
		m_blurYCoeffCount.reset();
		m_blurYCoeffs.reset();
		m_blurYMapDiffuse.reset();

		m_vertexBuffer->Cleanup();
		m_vertexBuffer.reset();
		m_geometryBuffers->Cleanup();
		m_geometryBuffers.reset();

		for ( auto & surface : m_blurSurfaces )
		{
			surface.Cleanup();
		}

		for ( auto & surface : m_hiPassSurfaces )
		{
			surface.Cleanup();
		}

		m_viewport.Cleanup();

		m_combinePipeline->Cleanup();
		m_combinePipeline.reset();

		m_blurYPipeline->Cleanup();
		m_blurYPipeline.reset();

		m_blurXPipeline->Cleanup();
		m_blurXPipeline.reset();

		m_hiPassPipeline->Cleanup();
		m_hiPassPipeline.reset();
	}

	bool BloomPostEffect::Apply( FrameBuffer & p_framebuffer )
	{
		auto attach = p_framebuffer.GetAttachment( AttachmentPoint::eColour, 0 );

		if ( attach && attach->GetAttachmentType() == AttachmentType::eTexture )
		{
			auto const & texture = *std::static_pointer_cast< TextureAttachment >( attach )->GetTexture();

			DoHiPassFilter( texture );
			DoDownSample( texture );
			DoBlur( texture
				, m_hiPassSurfaces
				, m_blurSurfaces
				, *m_blurXPipeline
				, m_blurXUbo );
			DoBlur( texture
				, m_blurSurfaces
				, m_hiPassSurfaces
				, *m_blurYPipeline
				, m_blurYUbo );
#if 0

			for ( auto & surface : m_hiPassSurfaces )
			{
				auto pxbuffer = surface.m_colourTexture.GetTexture()->GetImage().GetBuffer();

				if ( pxbuffer )
				{
					if ( surface.m_colourTexture.GetTexture()->Bind( 0 ) )
					{
						auto buffer = surface.m_colourTexture.GetTexture()->Lock( AccessType::eRead );

						if ( buffer )
						{
							std::memcpy( pxbuffer->ptr(), buffer, pxbuffer->size() );
							surface.m_colourTexture.GetTexture()->Unlock( false );
							StringStream name;
							name << Engine::GetEngineDirectory() << cuT( "\\Bloom_" ) << ( void * )pxbuffer.get() << cuT( "_Blur.png" );
							Image::BinaryWriter()( Image( cuT( "tmp" ), *pxbuffer ), Path( name.str() ) );
						}

						surface.m_colourTexture.GetTexture()->Unbind( 0 );
					}
				}
			}

#endif
			DoCombine( texture );

			p_framebuffer.Bind( FrameBufferTarget::eDraw );
			GetRenderSystem()->GetCurrentContext()->RenderTexture( texture.GetDimensions()
				, *m_blurSurfaces[0].m_colourTexture.GetTexture() );
			p_framebuffer.Unbind();
		}

		return true;
	}

	bool BloomPostEffect::DoWriteInto( TextFile & p_file )
	{
		return p_file.WriteText( cuT( " -Size=" ) + string::to_string( m_size ) ) > 0;
	}

	void BloomPostEffect::DoHiPassFilter( TextureLayout const & p_origin )
	{
		auto source = &m_hiPassSurfaces[0];
		source->m_fbo->Bind( FrameBufferTarget::eDraw );
		source->m_fbo->Clear( BufferComponent::eColour );
		m_hiPassMapDiffuse->SetValue( 0 );
		GetRenderSystem()->GetCurrentContext()->RenderTexture( source->m_size
			, p_origin
			, *m_hiPassPipeline
			, m_matrixUbo );
		source->m_fbo->Unbind();
	}

	void BloomPostEffect::DoDownSample( TextureLayout const & p_origin )
	{
		auto context = GetRenderSystem()->GetCurrentContext();
		auto source = &m_hiPassSurfaces[0];

		for ( uint32_t i = 1; i < m_hiPassSurfaces.size(); ++i )
		{
			auto destination = &m_hiPassSurfaces[i];
			destination->m_fbo->Bind( FrameBufferTarget::eDraw );
			destination->m_fbo->Clear( BufferComponent::eColour );
			context->RenderTexture( destination->m_size
				, *source->m_colourTexture.GetTexture() );
			destination->m_fbo->Unbind();
			source = destination;
		}
	}

	void BloomPostEffect::DoBlur( TextureLayout const & p_origin
		, SurfaceArray & p_sources
		, SurfaceArray & p_destinations
		, RenderPipeline & p_pipeline
		, UniformBuffer & p_ubo )
	{
		auto context = GetRenderSystem()->GetCurrentContext();
		p_ubo.BindTo( 2u );

		for ( uint32_t i = 0; i < FILTER_COUNT; ++i )
		{
			auto source = &p_sources[i];
			auto destination = &p_destinations[i];
			destination->m_fbo->Bind( FrameBufferTarget::eDraw );
			destination->m_fbo->Clear( BufferComponent::eColour );
			context->RenderTexture( source->m_size
				, *source->m_colourTexture.GetTexture()
				, p_pipeline
				, m_matrixUbo );
			destination->m_fbo->Unbind();
		}
	}

	void BloomPostEffect::DoCombine( TextureLayout const & p_origin )
	{
		m_blurSurfaces[0].m_fbo->Bind( FrameBufferTarget::eDraw );
		m_blurSurfaces[0].m_fbo->Clear( BufferComponent::eColour );
		m_viewport.Resize( p_origin.GetDimensions() );
		m_viewport.Update();
		m_viewport.Apply();

		auto const & texture0 = m_hiPassSurfaces[0].m_colourTexture;
		auto const & texture1 = m_hiPassSurfaces[1].m_colourTexture;
		auto const & texture2 = m_hiPassSurfaces[2].m_colourTexture;
		auto const & texture3 = m_hiPassSurfaces[3].m_colourTexture;
		m_matrixUbo.Update( m_viewport.GetProjection() );
		m_combinePipeline->Apply();

		texture0.Bind();
		texture1.Bind();
		texture2.Bind();
		texture3.Bind();
		p_origin.Bind( 4 );
		m_linearSampler->Bind( 4 );

		m_geometryBuffers->Draw( uint32_t( m_vertices.size() ), 0 );

		texture0.Unbind();
		texture1.Unbind();
		texture2.Unbind();
		texture3.Unbind();
		p_origin.Unbind( 4 );
		m_linearSampler->Unbind( 4 );

		m_blurSurfaces[0].m_fbo->Unbind();

		m_blurSurfaces[0].m_colourTexture.Bind();
		m_blurSurfaces[0].m_colourTexture.GetTexture()->GenerateMipmaps();
		m_blurSurfaces[0].m_colourTexture.Unbind();
	}

	SamplerSPtr BloomPostEffect::DoCreateSampler( bool p_linear )
	{
		String name = cuT( "BloomSampler_" );
		InterpolationMode mode;

		if ( p_linear )
		{
			name += cuT( "Linear" );
			mode = InterpolationMode::eLinear;
		}
		else
		{
			name += cuT( "Nearest" );
			mode = InterpolationMode::eNearest;
		}

		SamplerSPtr sampler;

		if ( !m_renderTarget.GetEngine()->GetSamplerCache().Has( name ) )
		{
			sampler = m_renderTarget.GetEngine()->GetSamplerCache().Add( name );
			sampler->SetInterpolationMode( InterpolationFilter::eMin, mode );
			sampler->SetInterpolationMode( InterpolationFilter::eMag, mode );
			sampler->SetWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
			sampler->SetWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
			sampler->SetWrappingMode( TextureUVW::eW, WrapMode::eClampToEdge );
		}
		else
		{
			sampler = m_renderTarget.GetEngine()->GetSamplerCache().Find( name );
		}

		return sampler;
	}

	bool BloomPostEffect::DoInitialiseHiPassProgram()
	{
		auto & cache = GetRenderSystem()->GetEngine()->GetShaderProgramCache();
		auto const vertex = GetVertexProgram( GetRenderSystem() );
		auto const hipass = GetHiPassProgram( GetRenderSystem() );

		ShaderProgramSPtr program = cache.GetNewProgram( false );
		program->CreateObject( ShaderType::eVertex );
		program->CreateObject( ShaderType::ePixel );
		m_hiPassMapDiffuse = program->CreateUniform < UniformType::eSampler >( ShaderProgram::MapDiffuse
			, ShaderType::ePixel );
		program->SetSource( ShaderType::eVertex, vertex );
		program->SetSource( ShaderType::ePixel, hipass );
		bool result = program->Initialise();

		if ( result )
		{
			DepthStencilState dsstate;
			dsstate.SetDepthTest( false );
			dsstate.SetDepthMask( WritingMask::eZero );
			m_hiPassPipeline = GetRenderSystem()->CreateRenderPipeline( std::move( dsstate )
				, RasteriserState{}
				, BlendState{}
				, MultisampleState{}
				, *program
				, PipelineFlags{} );
			m_hiPassPipeline->AddUniformBuffer( m_matrixUbo.GetUbo() );
		}

		return result;
	}

	bool BloomPostEffect::DoInitialiseBlurXProgram()
	{
		auto & cache = GetRenderSystem()->GetEngine()->GetShaderProgramCache();
		auto const vertex = GetVertexProgram( GetRenderSystem() );
		auto const blurX = GetBlurXProgram( GetRenderSystem() );

		ShaderProgramSPtr program = cache.GetNewProgram( false );
		program->CreateObject( ShaderType::eVertex );
		program->CreateObject( ShaderType::ePixel );
		m_blurXMapDiffuse = program->CreateUniform< UniformType::eSampler >( ShaderProgram::MapDiffuse
			, ShaderType::ePixel );
		m_blurXCoeffCount->SetValue( m_size );
		m_blurXCoeffs->SetValues( m_kernel );

		program->SetSource( ShaderType::eVertex, vertex );
		program->SetSource( ShaderType::ePixel, blurX );
		bool result = program->Initialise();

		if ( result )
		{
			DepthStencilState dsstate;
			dsstate.SetDepthTest( false );
			dsstate.SetDepthMask( WritingMask::eZero );
			m_blurXPipeline = GetRenderSystem()->CreateRenderPipeline( std::move( dsstate )
				, RasteriserState{}
				, BlendState{}
				, MultisampleState{}
				, *program
				, PipelineFlags{} );
			m_blurXPipeline->AddUniformBuffer( m_matrixUbo.GetUbo() );
			m_blurXPipeline->AddUniformBuffer( m_blurXUbo );
		}

		return result;
	}

	bool BloomPostEffect::DoInitialiseBlurYProgram()
	{
		auto & cache = GetRenderSystem()->GetEngine()->GetShaderProgramCache();
		auto const vertex = GetVertexProgram( GetRenderSystem() );
		auto const blurY = GetBlurYProgram( GetRenderSystem() );

		ShaderProgramSPtr program = cache.GetNewProgram( false );
		program->CreateObject( ShaderType::eVertex );
		program->CreateObject( ShaderType::ePixel );
		m_blurYMapDiffuse = program->CreateUniform< UniformType::eSampler >( ShaderProgram::MapDiffuse
			, ShaderType::ePixel );
		m_blurYCoeffCount->SetValue( m_size );
		m_blurYCoeffs->SetValues( m_kernel );

		program->SetSource( ShaderType::eVertex, vertex );
		program->SetSource( ShaderType::ePixel, blurY );
		bool result = program->Initialise();

		if ( result )
		{
			DepthStencilState dsstate;
			dsstate.SetDepthTest( false );
			dsstate.SetDepthMask( WritingMask::eZero );
			m_blurYPipeline = GetRenderSystem()->CreateRenderPipeline( std::move( dsstate )
				, RasteriserState{}
				, BlendState{}
				, MultisampleState{}
				, *program
				, PipelineFlags{} );
			m_blurYPipeline->AddUniformBuffer( m_matrixUbo.GetUbo() );
			m_blurYPipeline->AddUniformBuffer( m_blurYUbo );
		}

		return result;
	}

	bool BloomPostEffect::DoInitialiseCombineProgram()
	{
		auto & cache = GetRenderSystem()->GetEngine()->GetShaderProgramCache();
		auto const vertex = GetVertexProgram( GetRenderSystem() );
		auto const combine = GetCombineProgram( GetRenderSystem() );

		ShaderProgramSPtr program = cache.GetNewProgram( false );
		program->CreateObject( ShaderType::eVertex );
		program->CreateObject( ShaderType::ePixel );
		program->CreateUniform< UniformType::eSampler >( BloomPostEffect::CombineMapPass0
			, ShaderType::ePixel )->SetValue( 0 );
		program->CreateUniform< UniformType::eSampler >( BloomPostEffect::CombineMapPass1
			, ShaderType::ePixel )->SetValue( 1 );
		program->CreateUniform< UniformType::eSampler >( BloomPostEffect::CombineMapPass2
			, ShaderType::ePixel )->SetValue( 2 );
		program->CreateUniform< UniformType::eSampler >( BloomPostEffect::CombineMapPass3
			, ShaderType::ePixel )->SetValue( 3 );
		program->CreateUniform< UniformType::eSampler >( BloomPostEffect::CombineMapScene
			, ShaderType::ePixel )->SetValue( 4 );

		program->SetSource( ShaderType::eVertex, vertex );
		program->SetSource( ShaderType::ePixel, combine );
		bool result = program->Initialise();

		if ( result )
		{
			m_vertexBuffer = std::make_shared< VertexBuffer >( *GetRenderSystem()->GetEngine()
				, m_declaration );
			m_vertexBuffer->Resize( uint32_t( m_vertices.size() * m_declaration.stride() ) );
			m_vertexBuffer->LinkCoords( m_vertices.begin(), m_vertices.end() );
			m_vertexBuffer->Initialise( BufferAccessType::eStatic, BufferAccessNature::eDraw );
			m_geometryBuffers = GetRenderSystem()->CreateGeometryBuffers( Topology::eTriangles
				, *program );
			result = m_geometryBuffers->Initialise( { *m_vertexBuffer }, nullptr );
		}

		if ( result )
		{
			DepthStencilState dsstate;
			dsstate.SetDepthTest( false );
			dsstate.SetDepthMask( WritingMask::eZero );
			m_combinePipeline = GetRenderSystem()->CreateRenderPipeline( std::move( dsstate )
				, RasteriserState{}
				, BlendState{}
				, MultisampleState{}
				, *program
				, PipelineFlags{} );
			m_combinePipeline->AddUniformBuffer( m_matrixUbo.GetUbo() );
		}

		return result;
	}
}
