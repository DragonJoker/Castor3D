#include "BloomPostEffect.hpp"

#include <Engine.hpp>
#include <SamplerCache.hpp>
#include <ShaderCache.hpp>

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
#include <Render/Pipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Render/RenderTarget.hpp>
#include <Render/RenderWindow.hpp>
#include <Render/Viewport.hpp>
#include <Shader/FrameVariableBuffer.hpp>
#include <Shader/OneFrameVariable.hpp>
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
		Castor::String GetVertexProgram( RenderSystem * p_renderSystem )
		{
			using namespace GLSL;
			GlslWriter l_writer = p_renderSystem->CreateGlslWriter();

			UBO_MATRIX( l_writer );

			// Shader inputs
			Vec2 position = l_writer.GetAttribute< Vec2 >( ShaderProgram::Position );

			// Shader outputs
			auto vtx_texture = l_writer.GetOutput< Vec2 >( cuT( "vtx_texture" ) );
			auto gl_Position = l_writer.GetBuiltin< Vec4 >( cuT( "gl_Position" ) );

			l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				vtx_texture = position;
				gl_Position = c3d_mtxProjection * vec4( position.xy(), 0.0, 1.0 );
			} );
			return l_writer.Finalise();
		}

		Castor::String GetHiPassProgram( RenderSystem * p_renderSystem )
		{
			using namespace GLSL;
			GlslWriter l_writer = p_renderSystem->CreateGlslWriter();

			// Shader inputs
			auto c3d_mapDiffuse = l_writer.GetUniform< Sampler2D >( ShaderProgram::MapDiffuse );
			auto vtx_texture = l_writer.GetInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto plx_v4FragColor = l_writer.GetFragData< Vec4 >( cuT( "plx_v4FragColor" ), 0 );

			l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				plx_v4FragColor = vec4( texture( c3d_mapDiffuse, vec2( vtx_texture.x(), vtx_texture.y() ) ).xyz(), 1.0 );

				plx_v4FragColor.x() = TERNARY( l_writer, Float, plx_v4FragColor.x() > 1.0, Float( 1 ), Float( 0 ) );
				plx_v4FragColor.y() = TERNARY( l_writer, Float, plx_v4FragColor.y() > 1.0, Float( 1 ), Float( 0 ) );
				plx_v4FragColor.z() = TERNARY( l_writer, Float, plx_v4FragColor.z() > 1.0, Float( 1 ), Float( 0 ) );
			} );
			return l_writer.Finalise();
		}

		Castor::String GetBlurXProgram( RenderSystem * p_renderSystem )
		{
			using namespace GLSL;
			GlslWriter l_writer = p_renderSystem->CreateGlslWriter();

			// Shader inputs
			Ubo l_config{ l_writer, BloomPostEffect::FilterConfig };
			auto c3d_fCoefficients = l_config.GetUniform< Float >( BloomPostEffect::FilterConfigCoefficients, BloomPostEffect::MaxCoefficients );
			auto c3d_fCoefficientsCount = l_config.GetUniform< UInt >( BloomPostEffect::FilterConfigCoefficientsCount );
			l_config.End();
			auto c3d_mapDiffuse = l_writer.GetUniform< Sampler2D >( ShaderProgram::MapDiffuse );
			auto vtx_texture = l_writer.GetInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto plx_v4FragColor = l_writer.GetFragData< Vec4 >( cuT( "plx_v4FragColor" ), 0 );

			l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				auto l_base = l_writer.GetLocale( cuT( "l_base" ), vec2( Float( 1.0f ), 0 ) / textureSize( c3d_mapDiffuse, 0 ) );
				auto l_offset = l_writer.GetLocale( cuT( "l_offset" ), vec2( Float( 0 ), 0 ) );
				plx_v4FragColor = c3d_fCoefficients[0] * texture( c3d_mapDiffuse, vtx_texture );

				FOR( l_writer, Int, i, 1, cuT( "i < c3d_fCoefficientsCount" ), cuT( "++i" ) )
				{
					l_offset += l_base;
					plx_v4FragColor += c3d_fCoefficients[i] * texture( c3d_mapDiffuse, vtx_texture - l_offset );
					plx_v4FragColor += c3d_fCoefficients[i] * texture( c3d_mapDiffuse, vtx_texture + l_offset );
				}
				ROF;
			} );
			return l_writer.Finalise();
		}

		Castor::String GetBlurYProgram( RenderSystem * p_renderSystem )
		{
			using namespace GLSL;
			GlslWriter l_writer = p_renderSystem->CreateGlslWriter();

			// Shader inputs
			Ubo l_config{ l_writer, BloomPostEffect::FilterConfig };
			auto c3d_fCoefficients = l_config.GetUniform< Float >( BloomPostEffect::FilterConfigCoefficients, BloomPostEffect::MaxCoefficients );
			auto c3d_fCoefficientsCount = l_config.GetUniform< UInt >( BloomPostEffect::FilterConfigCoefficientsCount );
			l_config.End();
			auto c3d_mapDiffuse = l_writer.GetUniform< Sampler2D >( ShaderProgram::MapDiffuse );
			auto vtx_texture = l_writer.GetInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto plx_v4FragColor = l_writer.GetFragData< Vec4 >( cuT( "plx_v4FragColor" ), 0 );

			l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				auto l_base = l_writer.GetLocale( cuT( "l_base" ), vec2( Float( 0.0f ), 1 ) / textureSize( c3d_mapDiffuse, 0 ) );
				auto l_offset = l_writer.GetLocale( cuT( "l_offset" ), vec2( Float( 0 ), 0 ) );
				plx_v4FragColor = c3d_fCoefficients[0] * texture( c3d_mapDiffuse, vtx_texture );

				FOR( l_writer, Int, i, 1, cuT( "i < c3d_fCoefficientsCount" ), cuT( "++i" ) )
				{
					l_offset += l_base;
					plx_v4FragColor += c3d_fCoefficients[i] * texture( c3d_mapDiffuse, vtx_texture - l_offset );
					plx_v4FragColor += c3d_fCoefficients[i] * texture( c3d_mapDiffuse, vtx_texture + l_offset );
				}
				ROF;
			} );
			return l_writer.Finalise();
		}

		Castor::String GetCombineProgram( RenderSystem * p_renderSystem )
		{
			using namespace GLSL;
			GlslWriter l_writer = p_renderSystem->CreateGlslWriter();

			// Shader inputs
			auto c3d_mapPass0 = l_writer.GetUniform< Sampler2D >( BloomPostEffect::CombineMapPass0 );
			auto c3d_mapPass1 = l_writer.GetUniform< Sampler2D >( BloomPostEffect::CombineMapPass1 );
			auto c3d_mapPass2 = l_writer.GetUniform< Sampler2D >( BloomPostEffect::CombineMapPass2 );
			auto c3d_mapPass3 = l_writer.GetUniform< Sampler2D >( BloomPostEffect::CombineMapPass3 );
			auto c3d_mapScene = l_writer.GetUniform< Sampler2D >( BloomPostEffect::CombineMapScene );
			auto vtx_texture = l_writer.GetInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto plx_v4FragColor = l_writer.GetFragData< Vec4 >( cuT( "plx_v4FragColor" ), 0 );

			l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				plx_v4FragColor = texture( c3d_mapPass0, vtx_texture );
				plx_v4FragColor += texture( c3d_mapPass1, vtx_texture );
				plx_v4FragColor += texture( c3d_mapPass2, vtx_texture );
				plx_v4FragColor += texture( c3d_mapPass3, vtx_texture );
				plx_v4FragColor += texture( c3d_mapScene, vtx_texture );
			} );
			return l_writer.Finalise();
		}
	}

	std::vector< float > GetHalfPascal( uint32_t p_height )
	{
		std::vector< float > l_return;
		l_return.resize( p_height );
		auto l_x = 1.0f;
		auto l_max = 1 + p_height;

		for ( uint32_t i = 0u; i <= l_max; ++i )
		{
			auto l_index = l_max - i;

			if ( l_index < p_height )
			{
				l_return[l_index] = l_x;
			}

			l_x = l_x * ( ( p_height + 1 ) * 2 - i ) / ( i + 1 );
		}

		return l_return;
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

	BloomPostEffect::BloomPostEffect( RenderTarget & p_renderTarget, RenderSystem & p_renderSystem, Parameters const & p_param )
		: PostEffect( BloomPostEffect::Type, p_renderTarget, p_renderSystem, p_param )
		, m_viewport{ *p_renderSystem.GetEngine() }
		, m_size( 5u )
		, m_declaration(
		{
			BufferElementDeclaration( ShaderProgram::Position, uint32_t( ElementUsage::ePosition ), ElementType::eVec2 ),
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
		String l_count;

		if ( p_param.Get( cuT( "Size" ), l_count ) )
		{
			m_size = uint32_t( string::to_long( l_count ) );
		}

		// Normalize kernel coefficients
		m_kernel = GetHalfPascal( m_size );
		float l_sum = m_kernel[0];

		for ( uint32_t i = 1u; i < m_size; ++i )
		{
			l_sum += m_kernel[i] * 2;
		}

		std::transform( std::begin( m_kernel ), std::end( m_kernel ), std::begin( m_kernel ), [&l_sum]( float & p_value )
		{
			return p_value /= l_sum;
		} );

		real l_pBuffer[] =
		{
			0, 0,
			1, 1,
			0, 1,
			0, 0,
			1, 0,
			1, 1,
		};

		std::memcpy( m_buffer, l_pBuffer, sizeof( l_pBuffer ) );
		uint32_t i = 0;

		for ( auto & l_vertex : m_vertices )
		{
			l_vertex = std::make_shared< BufferElementGroup >( &reinterpret_cast< uint8_t * >( m_buffer )[i++ * m_declaration.stride()] );
		}

		m_linearSampler = DoCreateSampler( true );
		m_nearestSampler = DoCreateSampler( false );
	}

	BloomPostEffect::~BloomPostEffect()
	{
	}

	PostEffectSPtr BloomPostEffect::Create( RenderTarget & p_renderTarget, RenderSystem & p_renderSystem, Parameters const & p_param )
	{
		return std::make_shared< BloomPostEffect >( p_renderTarget, p_renderSystem, p_param );
	}

	bool BloomPostEffect::Initialise()
	{
		bool l_return = false;
		m_viewport.Initialise();

		l_return = DoInitialiseHiPassProgram();

		if ( l_return )
		{
			l_return = DoInitialiseBlurXProgram();
		}

		if ( l_return )
		{
			l_return = DoInitialiseBlurYProgram();
		}

		if ( l_return )
		{
			l_return = DoInitialiseCombineProgram();
		}

		Size l_size = m_renderTarget.GetSize();
		uint32_t l_index = 0;

		for ( auto & l_surface : m_hiPassSurfaces )
		{
			l_surface.Initialise( m_renderTarget, l_size, l_index++, m_linearSampler );
			l_size.width() >>= 1;
			l_size.height() >>= 1;
		}

		l_size = m_renderTarget.GetSize();
		l_index = 0;

		for ( auto & l_surface : m_blurSurfaces )
		{
			l_surface.Initialise( m_renderTarget, l_size, l_index++, m_nearestSampler );
			l_size.width() >>= 1;
			l_size.height() >>= 1;
		}

		return l_return;
	}

	void BloomPostEffect::Cleanup()
	{
		m_hiPassMapDiffuse.reset();
		m_blurXCoeffCount.reset();
		m_blurXCoeffs.reset();
		m_blurXMapDiffuse.reset();
		m_blurYCoeffCount.reset();
		m_blurYCoeffs.reset();
		m_blurYMapDiffuse.reset();

		m_vertexBuffer->Destroy();
		m_vertexBuffer.reset();
		m_geometryBuffers->Cleanup();
		m_geometryBuffers.reset();

		for ( auto & l_surface : m_blurSurfaces )
		{
			l_surface.Cleanup();
		}

		for ( auto & l_surface : m_hiPassSurfaces )
		{
			l_surface.Cleanup();
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
		auto l_attach = p_framebuffer.GetAttachment( AttachmentPoint::eColour, 0 );

		if ( l_attach && l_attach->GetAttachmentType() == AttachmentType::eTexture )
		{
			auto const & l_texture = *std::static_pointer_cast< TextureAttachment >( l_attach )->GetTexture();

			if ( DoHiPassFilter( l_texture ) )
			{
				DoDownSample( l_texture );
				DoBlur( l_texture, m_hiPassSurfaces, m_blurSurfaces, *m_blurXPipeline );
				DoBlur( l_texture, m_blurSurfaces, m_hiPassSurfaces, *m_blurYPipeline );
#if 0

				for ( auto & l_surface : m_hiPassSurfaces )
				{
					auto l_pxbuffer = l_surface.m_colourTexture.GetTexture()->GetImage().GetBuffer();

					if ( l_pxbuffer )
					{
						if ( l_surface.m_colourTexture.GetTexture()->Bind( 0 ) )
						{
							auto l_buffer = l_surface.m_colourTexture.GetTexture()->Lock( AccessType::eRead );

							if ( l_buffer )
							{
								std::memcpy( l_pxbuffer->ptr(), l_buffer, l_pxbuffer->size() );
								l_surface.m_colourTexture.GetTexture()->Unlock( false );
								StringStream l_name;
								l_name << Engine::GetEngineDirectory() << cuT( "\\Bloom_" ) << ( void * )l_pxbuffer.get() << cuT( "_Blur.png" );
								Image::BinaryWriter()( Image( cuT( "tmp" ), *l_pxbuffer ), Path( l_name.str() ) );
							}

							l_surface.m_colourTexture.GetTexture()->Unbind( 0 );
						}
					}
				}

#endif
				DoCombine( l_texture );
			}

			if ( p_framebuffer.Bind( FrameBufferMode::eAutomatic, FrameBufferTarget::eDraw ) )
			{
				GetRenderSystem()->GetCurrentContext()->RenderTexture( l_texture.GetDimensions(), *m_blurSurfaces[0].m_colourTexture.GetTexture() );
				p_framebuffer.Unbind();
			}
		}

		return true;
	}

	bool BloomPostEffect::DoWriteInto( TextFile & p_file )
	{
		return p_file.WriteText( cuT( " -Size " ) + string::to_string( m_size ) ) > 0;
	}

	bool BloomPostEffect::DoHiPassFilter( TextureLayout const & p_origin )
	{
		auto l_source = &m_hiPassSurfaces[0];
		bool l_return = l_source->m_fbo->Bind( FrameBufferMode::eAutomatic, FrameBufferTarget::eDraw );

		if ( l_return )
		{
			l_source->m_fbo->Clear();
			m_hiPassMapDiffuse->SetValue( 0 );
			GetRenderSystem()->GetCurrentContext()->RenderTexture( l_source->m_size, p_origin, *m_hiPassPipeline );
			l_source->m_fbo->Unbind();
		}

		return l_return;
	}

	void BloomPostEffect::DoDownSample( TextureLayout const & p_origin )
	{
		auto l_context = GetRenderSystem()->GetCurrentContext();
		auto l_source = &m_hiPassSurfaces[0];

		for ( uint32_t i = 1; i < m_hiPassSurfaces.size(); ++i )
		{
			auto l_destination = &m_hiPassSurfaces[i];

			if ( l_destination->m_fbo->Bind( FrameBufferMode::eAutomatic, FrameBufferTarget::eDraw ) )
			{
				l_destination->m_fbo->Clear();
				l_context->RenderTexture( l_destination->m_size, *l_source->m_colourTexture.GetTexture() );
				l_destination->m_fbo->Unbind();
			}

			l_source = l_destination;
		}
	}

	void BloomPostEffect::DoBlur( TextureLayout const & p_origin, SurfaceArray & p_sources, SurfaceArray & p_destinations, Castor3D::Pipeline & p_pipeline )
	{
		auto l_context = GetRenderSystem()->GetCurrentContext();

		for ( uint32_t i = 0; i < FILTER_COUNT; ++i )
		{
			auto l_source = &p_sources[i];
			auto l_destination = &p_destinations[i];

			if ( l_destination->m_fbo->Bind( FrameBufferMode::eAutomatic, FrameBufferTarget::eDraw ) )
			{
				l_destination->m_fbo->Clear();
				l_context->RenderTexture( l_source->m_size, *l_source->m_colourTexture.GetTexture(), p_pipeline );
				l_destination->m_fbo->Unbind();
			}
		}
	}

	void BloomPostEffect::DoCombine( TextureLayout const & p_origin )
	{
		if ( m_blurSurfaces[0].m_fbo->Bind( FrameBufferMode::eAutomatic, FrameBufferTarget::eDraw ) )
		{
			m_blurSurfaces[0].m_fbo->Clear();
			m_viewport.Resize( p_origin.GetDimensions() );
			m_viewport.Update();
			m_viewport.Apply();
			m_combinePipeline->SetProjectionMatrix( m_viewport.GetProjection() );
			m_combinePipeline->Apply();

			auto const & l_texture0 = m_hiPassSurfaces[0].m_colourTexture;
			auto const & l_texture1 = m_hiPassSurfaces[1].m_colourTexture;
			auto const & l_texture2 = m_hiPassSurfaces[2].m_colourTexture;
			auto const & l_texture3 = m_hiPassSurfaces[3].m_colourTexture;
			FrameVariableBufferSPtr l_matrices = m_combinePipeline->GetProgram().FindFrameVariableBuffer( ShaderProgram::BufferMatrix );

			if ( l_matrices )
			{
				m_combinePipeline->ApplyProjection( *l_matrices );
			}

			m_combinePipeline->Apply();
			m_combinePipeline->GetProgram().BindUbos();

			l_texture0.Bind();
			l_texture1.Bind();
			l_texture2.Bind();
			l_texture3.Bind();
			p_origin.Bind( 4 );
			m_linearSampler->Bind( 4 );

			m_geometryBuffers->Draw( uint32_t( m_vertices.size() ), 0 );

			l_texture0.Unbind();
			l_texture1.Unbind();
			l_texture2.Unbind();
			l_texture3.Unbind();
			p_origin.Unbind( 4 );
			m_linearSampler->Unbind( 4 );

			m_combinePipeline->GetProgram().UnbindUbos();

			m_blurSurfaces[0].m_fbo->Unbind();

			m_blurSurfaces[0].m_colourTexture.Bind();
			m_blurSurfaces[0].m_colourTexture.GetTexture()->GenerateMipmaps();
			m_blurSurfaces[0].m_colourTexture.Unbind();
		}
	}

	SamplerSPtr BloomPostEffect::DoCreateSampler( bool p_linear )
	{
		String l_name = cuT( "BloomSampler_" );
		InterpolationMode l_mode;

		if ( p_linear )
		{
			l_name += cuT( "Linear" );
			l_mode = InterpolationMode::eLinear;
		}
		else
		{
			l_name += cuT( "Nearest" );
			l_mode = InterpolationMode::eNearest;
		}

		SamplerSPtr l_sampler;

		if ( !m_renderTarget.GetEngine()->GetSamplerCache().Has( l_name ) )
		{
			l_sampler = m_renderTarget.GetEngine()->GetSamplerCache().Add( l_name );
			l_sampler->SetInterpolationMode( InterpolationFilter::eMin, l_mode );
			l_sampler->SetInterpolationMode( InterpolationFilter::eMag, l_mode );
			l_sampler->SetWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
			l_sampler->SetWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
			l_sampler->SetWrappingMode( TextureUVW::eW, WrapMode::eClampToEdge );
		}
		else
		{
			l_sampler = m_renderTarget.GetEngine()->GetSamplerCache().Find( l_name );
		}

		return l_sampler;
	}

	bool BloomPostEffect::DoInitialiseHiPassProgram()
	{
		auto & l_cache = GetRenderSystem()->GetEngine()->GetShaderProgramCache();
		ShaderModel const l_model = GetRenderSystem()->GetGpuInformations().GetMaxShaderModel();
		auto const l_vertex = GetVertexProgram( GetRenderSystem() );
		auto const l_hipass = GetHiPassProgram( GetRenderSystem() );

		ShaderProgramSPtr l_program = l_cache.GetNewProgram( false );
		m_hiPassMapDiffuse = l_program->CreateFrameVariable< OneIntFrameVariable >( ShaderProgram::MapDiffuse, ShaderType::ePixel );
		l_cache.CreateMatrixBuffer( *l_program, 0u, MASK_SHADER_TYPE_VERTEX );
		l_program->SetSource( ShaderType::eVertex, l_model, l_vertex );
		l_program->SetSource( ShaderType::ePixel, l_model, l_hipass );
		bool l_return = l_program->Initialise();

		if ( l_return )
		{
			DepthStencilState l_dsstate;
			l_dsstate.SetDepthTest( false );
			l_dsstate.SetDepthMask( WritingMask::eZero );
			m_hiPassPipeline = GetRenderSystem()->CreatePipeline( std::move( l_dsstate ), RasteriserState{}, BlendState{}, MultisampleState{}, *l_program, PipelineFlags{} );
		}

		return l_return;
	}

	bool BloomPostEffect::DoInitialiseBlurXProgram()
	{
		auto & l_cache = GetRenderSystem()->GetEngine()->GetShaderProgramCache();
		ShaderModel const l_model = GetRenderSystem()->GetGpuInformations().GetMaxShaderModel();
		auto const l_vertex = GetVertexProgram( GetRenderSystem() );
		auto const l_blurX = GetBlurXProgram( GetRenderSystem() );

		ShaderProgramSPtr l_program = l_cache.GetNewProgram( false );
		m_blurXMapDiffuse = l_program->CreateFrameVariable< OneIntFrameVariable >( ShaderProgram::MapDiffuse, ShaderType::ePixel );
		l_cache.CreateMatrixBuffer( *l_program, 0u, MASK_SHADER_TYPE_VERTEX );
		auto & l_filterConfig = l_program->CreateFrameVariableBuffer( BloomPostEffect::FilterConfig, MASK_SHADER_TYPE_PIXEL );
		m_blurXCoeffs = l_filterConfig.CreateVariable< OneFloatFrameVariable >( BloomPostEffect::FilterConfigCoefficients, BloomPostEffect::MaxCoefficients );
		m_blurXCoeffCount = l_filterConfig.CreateVariable< OneUIntFrameVariable >( BloomPostEffect::FilterConfigCoefficientsCount );
		m_blurXCoeffCount->SetValue( m_size );
		m_blurXCoeffs->SetValues( m_kernel );

		l_program->SetSource( ShaderType::eVertex, l_model, l_vertex );
		l_program->SetSource( ShaderType::ePixel, l_model, l_blurX );
		bool l_return = l_program->Initialise();

		if ( l_return )
		{
			DepthStencilState l_dsstate;
			l_dsstate.SetDepthTest( false );
			l_dsstate.SetDepthMask( WritingMask::eZero );
			m_blurXPipeline = GetRenderSystem()->CreatePipeline( std::move( l_dsstate ), RasteriserState{}, BlendState{}, MultisampleState{}, *l_program, PipelineFlags{} );
		}

		return l_return;
	}

	bool BloomPostEffect::DoInitialiseBlurYProgram()
	{
		auto & l_cache = GetRenderSystem()->GetEngine()->GetShaderProgramCache();
		ShaderModel const l_model = GetRenderSystem()->GetGpuInformations().GetMaxShaderModel();
		auto const l_vertex = GetVertexProgram( GetRenderSystem() );
		auto const l_blurY = GetBlurYProgram( GetRenderSystem() );

		ShaderProgramSPtr l_program = l_cache.GetNewProgram( false );
		m_blurYMapDiffuse = l_program->CreateFrameVariable< OneIntFrameVariable >( ShaderProgram::MapDiffuse, ShaderType::ePixel );
		l_cache.CreateMatrixBuffer( *l_program, 0u, MASK_SHADER_TYPE_VERTEX );
		auto & l_filterConfig = l_program->CreateFrameVariableBuffer( FilterConfig, MASK_SHADER_TYPE_PIXEL );
		m_blurYCoeffs = l_filterConfig.CreateVariable< OneFloatFrameVariable >( BloomPostEffect::FilterConfigCoefficients, BloomPostEffect::MaxCoefficients );
		m_blurYCoeffCount = l_filterConfig.CreateVariable< OneUIntFrameVariable >( BloomPostEffect::FilterConfigCoefficientsCount );
		m_blurYCoeffCount->SetValue( m_size );
		m_blurYCoeffs->SetValues( m_kernel );

		l_program->SetSource( ShaderType::eVertex, l_model, l_vertex );
		l_program->SetSource( ShaderType::ePixel, l_model, l_blurY );
		bool l_return = l_program->Initialise();

		if ( l_return )
		{
			DepthStencilState l_dsstate;
			l_dsstate.SetDepthTest( false );
			l_dsstate.SetDepthMask( WritingMask::eZero );
			m_blurYPipeline = GetRenderSystem()->CreatePipeline( std::move( l_dsstate ), RasteriserState{}, BlendState{}, MultisampleState{}, *l_program, PipelineFlags{} );
		}

		return l_return;
	}

	bool BloomPostEffect::DoInitialiseCombineProgram()
	{
		auto & l_cache = GetRenderSystem()->GetEngine()->GetShaderProgramCache();
		ShaderModel const l_model = GetRenderSystem()->GetGpuInformations().GetMaxShaderModel();
		auto const l_vertex = GetVertexProgram( GetRenderSystem() );
		auto const l_combine = GetCombineProgram( GetRenderSystem() );

		ShaderProgramSPtr l_program = l_cache.GetNewProgram( false );
		l_program->CreateFrameVariable< OneIntFrameVariable >( BloomPostEffect::CombineMapPass0, ShaderType::ePixel )->SetValue( 0 );
		l_program->CreateFrameVariable< OneIntFrameVariable >( BloomPostEffect::CombineMapPass1, ShaderType::ePixel )->SetValue( 1 );
		l_program->CreateFrameVariable< OneIntFrameVariable >( BloomPostEffect::CombineMapPass2, ShaderType::ePixel )->SetValue( 2 );
		l_program->CreateFrameVariable< OneIntFrameVariable >( BloomPostEffect::CombineMapPass3, ShaderType::ePixel )->SetValue( 3 );
		l_program->CreateFrameVariable< OneIntFrameVariable >( BloomPostEffect::CombineMapScene, ShaderType::ePixel )->SetValue( 4 );
		l_cache.CreateMatrixBuffer( *l_program, 0u, MASK_SHADER_TYPE_VERTEX );

		l_program->SetSource( ShaderType::eVertex, l_model, l_vertex );
		l_program->SetSource( ShaderType::ePixel, l_model, l_combine );
		bool l_return = l_program->Initialise();

		if ( l_return )
		{
			m_vertexBuffer = std::make_shared< VertexBuffer >( *GetRenderSystem()->GetEngine(), m_declaration );
			m_vertexBuffer->Resize( uint32_t( m_vertices.size() * m_declaration.stride() ) );
			m_vertexBuffer->LinkCoords( m_vertices.begin(), m_vertices.end() );
			m_vertexBuffer->Create();
			m_vertexBuffer->Upload( BufferAccessType::eStatic, BufferAccessNature::eDraw );
			m_geometryBuffers = GetRenderSystem()->CreateGeometryBuffers( Topology::eTriangles, *l_program );
			l_return = m_geometryBuffers->Initialise( { *m_vertexBuffer }, nullptr );
		}

		if ( l_return )
		{
			DepthStencilState l_dsstate;
			l_dsstate.SetDepthTest( false );
			l_dsstate.SetDepthMask( WritingMask::eZero );
			m_combinePipeline = GetRenderSystem()->CreatePipeline( std::move( l_dsstate ), RasteriserState{}, BlendState{}, MultisampleState{}, *l_program, PipelineFlags{} );
		}

		return l_return;
	}
}
