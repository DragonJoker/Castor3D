#include "SsaoPass.hpp"

#include "LightPass.hpp"

#include <Engine.hpp>
#include <FrameBuffer/FrameBuffer.hpp>
#include <FrameBuffer/TextureAttachment.hpp>
#include <Mesh/Buffer/BufferElementGroup.hpp>
#include <Mesh/Buffer/GeometryBuffers.hpp>
#include <Mesh/Buffer/VertexBuffer.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Scene/Camera.hpp>
#include <Shader/ShaderProgram.hpp>
#include <State/BlendState.hpp>
#include <State/DepthStencilState.hpp>
#include <State/MultisampleState.hpp>
#include <State/RasteriserState.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureLayout.hpp>
#include <Texture/TextureUnit.hpp>

#include <GlslSource.hpp>
#include <GlslLight.hpp>
#include <GlslShadow.hpp>
#include <GlslUtils.hpp>

#include <random>

using namespace Castor;
using namespace Castor3D;

namespace deferred_common
{
	namespace
	{
		float DoLerp( float a, float b, float f )
		{
			return a + f * ( b - a );
		}

		Point3fArray DoGetKernel()
		{
			constexpr uint32_t l_size = 64;
			std::uniform_real_distribution< float > l_distribution( 0.0f, 1.0f );
			std::default_random_engine l_generator;
			Point3fArray l_result;
			l_result.reserve( l_size );

			for ( auto i = 0u; i < l_size; ++i )
			{
				auto l_sample = point::get_normalised( Point3f{ l_distribution( l_generator ) * 2.0f - 1.0f
					, l_distribution( l_generator ) * 2.0f - 1.0f
					, l_distribution( l_generator ) } );
				l_sample *= l_distribution( l_generator );
				auto l_scale = i / float( l_size );
				l_scale = DoLerp( 0.1f, 1.0f, l_scale * l_scale );
				l_sample *= l_scale;
				l_result.push_back( l_sample );
			}

			return l_result;
		}

		VertexBufferSPtr DoCreateVbo( Engine & p_engine )
		{
			auto l_declaration = BufferDeclaration(
			{
				BufferElementDeclaration( ShaderProgram::Position, uint32_t( ElementUsage::ePosition ), ElementType::eVec2 ),
				BufferElementDeclaration{ ShaderProgram::Texture, uint32_t( ElementUsage::eTexCoords ), ElementType::eVec2 },
			} );

			float l_data[] =
			{
				0, 0, 0, 0,
				1, 1, 1, 1,
				0, 1, 0, 1,
				0, 0, 0, 0,
				1, 0, 1, 0,
				1, 1, 1, 1
			};

			auto & l_renderSystem = *p_engine.GetRenderSystem();
			auto l_vertexBuffer = std::make_shared< VertexBuffer >( p_engine, l_declaration );
			uint32_t l_stride = l_declaration.stride();
			l_vertexBuffer->Resize( uint32_t( sizeof( l_data ) ) );
			uint8_t * l_buffer = l_vertexBuffer->data();
			std::memcpy( l_buffer, l_data, sizeof( l_data ) );
			l_vertexBuffer->Initialise( BufferAccessType::eStatic
				, BufferAccessNature::eDraw );
			return l_vertexBuffer;
		}

		TextureUnit DoGetNoise( Engine & p_engine )
		{
			constexpr uint32_t l_size = 16;
			std::uniform_real_distribution< float > l_distribution( 0.0f, 1.0f );
			std::default_random_engine l_generator;
			Point3fArray l_noise;
			l_noise.reserve( l_size );

			for ( auto i = 0u; i < l_size; i++ )
			{
				l_noise.emplace_back( l_distribution( l_generator ) * 2.0 - 1.0
					, l_distribution( l_generator ) * 2.0 - 1.0
					, 0.0f );
			}

			auto l_buffer = PxBufferBase::create( Size{ 4, 4 }
				, PixelFormat::eRGB32F
				, reinterpret_cast< uint8_t const * >( l_noise[0].const_ptr() )
				, PixelFormat::eRGB32F );

			auto l_texture = p_engine.GetRenderSystem()->CreateTexture( TextureType::eTwoDimensions
				, AccessType::eNone
				, AccessType::eRead );
			l_texture->SetSource( l_buffer );

			SamplerSPtr l_sampler;

			if ( p_engine.GetSamplerCache().Has( cuT( "SSAO_Noise" ) ) )
			{
				l_sampler = p_engine.GetSamplerCache().Find( cuT( "SSAO_Noise" ) );
			}
			else
			{
				l_sampler = p_engine.GetSamplerCache().Add( cuT( "SSAO_Noise" ) );
				l_sampler->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eNearest );
				l_sampler->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eNearest );
				l_sampler->SetWrappingMode( TextureUVW::eU, WrapMode::eRepeat );
				l_sampler->SetWrappingMode( TextureUVW::eV, WrapMode::eRepeat );
			}

			TextureUnit l_result{ p_engine };
			l_result.SetSampler( l_sampler );
			l_result.SetTexture( l_texture );
			l_result.Initialise();
			l_result.SetIndex( 2u );
			return l_result;
		}

		String DoGetSsaoVertexProgram( Engine & p_engine )
		{
			auto & l_renderSystem = *p_engine.GetRenderSystem();
			String l_vtx;
			{
				using namespace GLSL;
				auto l_writer = l_renderSystem.CreateGlslWriter();

				// Shader inputs
				UBO_MATRIX( l_writer );
				UBO_GPINFO( l_writer );
				auto position = l_writer.DeclAttribute< Vec2 >( ShaderProgram::Position );
				auto texture = l_writer.DeclAttribute< Vec2 >( ShaderProgram::Texture );

				// Shader outputs
				auto gl_Position = l_writer.DeclBuiltin< Vec4 >( cuT( "gl_Position" ) );

				l_writer.ImplementFunction< void >( cuT( "main" )
					, [&]()
					{
						gl_Position = c3d_mtxProjection * vec4( position, 0.0, 1.0 );
					} );
				l_vtx = l_writer.Finalise();
			}

			return l_vtx;
		}
		
		String DoGetSsaoPixelProgram( Engine & p_engine )
		{
			auto & l_renderSystem = *p_engine.GetRenderSystem();
			using namespace GLSL;
			auto l_writer = l_renderSystem.CreateGlslWriter();

			// Shader inputs
			UBO_MATRIX( l_writer );
			UBO_GPINFO( l_writer );
			auto c3d_mapDepth = l_writer.DeclUniform< Sampler2D >( cuT( "c3d_mapDepth" ) );
			auto c3d_mapNormal = l_writer.DeclUniform< Sampler2D >( cuT( "c3d_mapNormal" ) );
			auto c3d_mapNoise = l_writer.DeclUniform< Sampler2D >( cuT( "c3d_mapNoise" ) );

			Ubo l_ssaoConfig{ l_writer, cuT( "SsaoConfig" ) };
			auto c3d_kernel = l_ssaoConfig.GetUniform< Vec3 >( cuT( "c3d_kernel" ), 64u );
			auto c3d_kernelSize = l_ssaoConfig.GetUniform< Int >( cuT( "c3d_kernelSize" ) );
			auto c3d_radius = l_ssaoConfig.GetUniform< Float >( cuT( "c3d_radius" ) );
			auto c3d_bias = l_ssaoConfig.GetUniform< Float >( cuT( "c3d_bias" ) );
			auto c3d_noiseScale = l_ssaoConfig.GetUniform< Vec2 >( cuT( "c3d_noiseScale" ) );
			l_ssaoConfig.End();

			GLSL::Utils l_utils{ l_writer };
			l_utils.DeclareCalcTexCoord();
			l_utils.DeclareCalcVSPosition();
			l_utils.DeclareCalcVSDepth();

			// Shader outputs
			auto pxl_fragColor = l_writer.DeclOutput< Vec4 >( cuT( "pxl_fragColor" ) );

			l_writer.ImplementFunction< Void >( cuT( "main" )
				, [&]()
				{
					auto l_texCoord = l_writer.DeclLocale( cuT( "l_texCoord" ), l_utils.CalcTexCoord() );

					auto l_vsPosition = l_writer.DeclLocale( cuT( "l_vsPosition" )
						, l_utils.CalcVSPosition( l_texCoord, c3d_mtxInvProj ) );
					auto l_vsNormal = l_writer.DeclLocale( cuT( "l_vsNormal" )
						, normalize( l_writer.Paren( c3d_mtxInvView * vec4( texture( c3d_mapNormal, l_texCoord ).xyz(), 1.0_f ) ).xyz() ) );

					auto l_randomVec = l_writer.DeclLocale( cuT( "l_randomVec" )
						, normalize( texture( c3d_mapNoise, l_texCoord * c3d_noiseScale ).xyz() ) );
					auto l_tangent = l_writer.DeclLocale( cuT( "l_tangent" )
						, normalize( l_randomVec - l_vsNormal * dot( l_randomVec, l_vsNormal ) ) );
					auto l_bitangent = l_writer.DeclLocale( cuT( "l_bitangent" )
						, cross( l_vsNormal, l_tangent ) );
					auto l_tbn = l_writer.DeclLocale( cuT( "l_tbn" )
						, mat3( l_tangent, l_bitangent, l_vsNormal ) );
					auto l_occlusion = l_writer.DeclLocale( cuT( "l_occlusion" ), 0.0_f );

					FOR( l_writer, Int, i, 0, cuT( "i < c3d_kernelSize" ), cuT( "++i" ) )
					{
						// get sample position
						auto l_sample = l_writer.DeclLocale( cuT( "l_sample" )
							, l_tbn * c3d_kernel[i] );                       // From tangent to view-space
						l_sample = l_vsPosition + l_sample * c3d_radius;
						auto l_offset = l_writer.DeclLocale( cuT( "l_offset" )
							, vec4( l_sample, 1.0 ) );
						l_offset = c3d_mtxGProj * l_offset;                  // from view to clip-space
						l_offset.xyz() = l_offset.xyz() / l_offset.w();      // perspective divide
						l_offset.xyz() = l_offset.xyz() * 0.5 + 0.5;         // transform to range 0.0 - 1.0 
						auto l_sampleDepth = l_writer.DeclLocale( cuT( "l_sampleDepth" )
							, l_utils.CalcVSPosition( l_offset.xy(), c3d_mtxInvProj ).z() );
						auto l_rangeCheck = l_writer.DeclLocale( cuT( "l_rangeCheck" )
							, smoothstep( 0.0_f, 1.0_f, c3d_radius / GLSL::abs( l_vsPosition.z() - l_sampleDepth ) ) );
						l_occlusion += l_writer.Ternary( l_sampleDepth >= l_sample.z() + c3d_bias, 1.0_f, 0.0_f ) * l_rangeCheck;
					}
					ROF;

					l_occlusion = 1.0_f - l_writer.Paren( l_occlusion / c3d_kernelSize );
					pxl_fragColor = vec4( vec3( l_occlusion ), 1.0 );
			} );
			return l_writer.Finalise();
		}
		
		String DoGetBlurVertexProgram( Engine & p_engine )
		{
			auto & l_renderSystem = *p_engine.GetRenderSystem();
			using namespace GLSL;
			auto l_writer = l_renderSystem.CreateGlslWriter();

			UBO_MATRIX( l_writer );

			// Shader inputs
			auto position = l_writer.DeclAttribute< Vec2 >( ShaderProgram::Position );
			auto texture = l_writer.DeclAttribute< Vec2 >( ShaderProgram::Texture );

			// Shader outputs
			auto vtx_texture = l_writer.DeclOutput< Vec2 >( cuT( "vtx_texture" ) );
			auto gl_Position = l_writer.DeclBuiltin< Vec4 >( cuT( "gl_Position" ) );

			l_writer.ImplementFunction< void >( cuT( "main" )
				, [&]()
				{
					vtx_texture = texture;
					gl_Position = c3d_mtxProjection * vec4( position.x(), position.y(), 0.0, 1.0 );
				} );
			return l_writer.Finalise();
		}

		String DoGetBlurPixelProgram( Engine & p_engine )
		{
			auto & l_renderSystem = *p_engine.GetRenderSystem();
			using namespace GLSL;
			auto l_writer = l_renderSystem.CreateGlslWriter();

			// Shader inputs
			auto vtx_texture = l_writer.DeclInput< Vec2 >( cuT( "vtx_texture" ) );
			auto c3d_mapColour = l_writer.DeclUniform< Sampler2D >( cuT( "c3d_mapColour" ) );

			// Shader outputs
			auto pxl_fragColor = l_writer.DeclOutput< Vec4 >( cuT( "pxl_fragColor" ) );

			l_writer.ImplementFunction< Void >( cuT( "main" )
				, [&]()
				{
					auto l_texelSize = l_writer.DeclLocale( cuT( "l_texelSize" ), vec2( 1.0_f, 1.0_f ) / vec2( textureSize( c3d_mapColour, 0 ) ) );
					auto l_result = l_writer.DeclLocale( cuT( "l_result" ), 0.0_f );

					FOR( l_writer, Int, x, -2, cuT( "x < 2" ), cuT( "++x" ) )
					{
						FOR( l_writer, Int, y, -2, cuT( "y < 2" ), cuT( "++y" ) )
						{
							auto l_offset = l_writer.DeclLocale( cuT( "l_offset" ), vec2( l_writer.Cast< Float >( x ), l_writer.Cast< Float >( y ) ) * l_texelSize );
							l_result += texture( c3d_mapColour, vtx_texture + l_offset ).r();
						}
						ROF;
					}
					ROF;

					l_result /= l_writer.Paren( 4.0_f * 4.0_f );
					pxl_fragColor = vec4( vec3( l_result ), 1.0 );
				} );
			return l_writer.Finalise();
		}

		ShaderProgramSPtr DoGetSsaoProgram( Engine & p_engine )
		{
			auto & l_renderSystem = *p_engine.GetRenderSystem();
			String l_vtx = DoGetSsaoVertexProgram( p_engine );
			String l_pxl = DoGetSsaoPixelProgram( p_engine );
			ShaderProgramSPtr l_program = p_engine.GetShaderProgramCache().GetNewProgram( false );
			l_program->CreateObject( ShaderType::eVertex );
			l_program->CreateObject( ShaderType::ePixel );
			l_program->CreateUniform< UniformType::eSampler >( cuT( "c3d_mapDepth" ), ShaderType::ePixel )->SetValue( 0 );
			l_program->CreateUniform< UniformType::eSampler >( cuT( "c3d_mapNormal" ), ShaderType::ePixel )->SetValue( 1 );
			l_program->CreateUniform< UniformType::eSampler >( cuT( "c3d_mapNoise" ), ShaderType::ePixel )->SetValue( 2 );
			auto const l_model = l_renderSystem.GetGpuInformations().GetMaxShaderModel();
			l_program->SetSource( ShaderType::eVertex, l_model, l_vtx );
			l_program->SetSource( ShaderType::ePixel, l_model, l_pxl );
			l_program->Initialise();
			return l_program;
		}

		ShaderProgramSPtr DoGetBlurProgram( Engine & p_engine )
		{
			auto & l_renderSystem = *p_engine.GetRenderSystem();
			String l_vtx = DoGetBlurVertexProgram( p_engine );
			String l_pxl = DoGetBlurPixelProgram( p_engine );
			ShaderProgramSPtr l_program = p_engine.GetShaderProgramCache().GetNewProgram( false );
			l_program->CreateObject( ShaderType::eVertex );
			l_program->CreateObject( ShaderType::ePixel );
			l_program->CreateUniform< UniformType::eSampler >( cuT( "c3d_mapColour" ), ShaderType::ePixel )->SetValue( 0 );
			auto const l_model = l_renderSystem.GetGpuInformations().GetMaxShaderModel();
			l_program->SetSource( ShaderType::eVertex, l_model, l_vtx );
			l_program->SetSource( ShaderType::ePixel, l_model, l_pxl );
			l_program->Initialise();
			return l_program;
		}

		RenderPipelineUPtr DoCreatePipeline( Engine & p_engine
			, ShaderProgram & p_program )
		{
			DepthStencilState l_dsstate;
			l_dsstate.SetDepthTest( false );
			l_dsstate.SetDepthMask( WritingMask::eZero );
			RasteriserState l_rsstate;
			l_rsstate.SetCulledFaces( Culling::eNone );
			return p_engine.GetRenderSystem()->CreateRenderPipeline( std::move( l_dsstate )
				, std::move( l_rsstate )
				, BlendState{}
				, MultisampleState{}
				, p_program
				, PipelineFlags{} );
		}

		SamplerSPtr DoCreateSampler( Engine & p_engine, String const & p_name, WrapMode p_mode )
		{
			SamplerSPtr l_sampler;

			if ( p_engine.GetSamplerCache().Has( p_name ) )
			{
				l_sampler = p_engine.GetSamplerCache().Find( p_name );
			}
			else
			{
				l_sampler = p_engine.GetSamplerCache().Add( p_name );
				l_sampler->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eNearest );
				l_sampler->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eNearest );
				l_sampler->SetWrappingMode( TextureUVW::eU, p_mode );
				l_sampler->SetWrappingMode( TextureUVW::eV, p_mode );
			}

			return l_sampler;
		}

		TextureUnit DoCreateTexture( Engine & p_engine
			, Size const & p_size
			, String const & p_name )
		{
			auto & l_renderSystem = *p_engine.GetRenderSystem();
			auto l_sampler = DoCreateSampler( p_engine, p_name, WrapMode::eClampToEdge );
			auto l_ssaoResult = l_renderSystem.CreateTexture( TextureType::eTwoDimensions
				, AccessType::eNone
				, AccessType::eRead | AccessType::eWrite );
			l_ssaoResult->SetSource( PxBufferBase::create( p_size
				, PixelFormat::eL32F ) );
			TextureUnit l_unit{ p_engine };
			l_unit.SetTexture( l_ssaoResult );
			l_unit.SetSampler( l_sampler );
			l_unit.SetIndex( 0u );
			l_unit.Initialise();
			return l_unit;
		}

		FrameBufferSPtr DoCreateFbo( Engine & p_engine
			, Size const & p_size )
		{
			auto & l_renderSystem = *p_engine.GetRenderSystem();
			auto l_fbo = l_renderSystem.CreateFrameBuffer();
			l_fbo->Create();
			l_fbo->Initialise( p_size );
			return l_fbo;
		}

		TextureAttachmentSPtr DoCreateAttach( FrameBuffer & p_fbo
			, TextureUnit const & p_unit )
		{
			auto l_attach = p_fbo.CreateAttachment( p_unit.GetTexture() );
			p_fbo.Bind();
			p_fbo.Attach( AttachmentPoint::eColour, 0u, l_attach, p_unit.GetTexture()->GetType() );
			p_fbo.SetDrawBuffer( l_attach );
			ENSURE( p_fbo.IsComplete() );
			p_fbo.Unbind();
			return l_attach;
		}
	}

	SsaoPass::SsaoPass( Engine & p_engine
		, Size const & p_size )
		: m_engine{ p_engine }
		, m_size{ p_size }
		, m_matrixUbo{ p_engine }
		, m_ssaoKernel{ DoGetKernel() }
		, m_ssaoNoise{ DoGetNoise( p_engine ) }
		, m_ssaoResult{ p_engine }
		//, m_ssaoResult{ DoCreateTexture( p_engine, p_size, cuT( "SSAO_Result" ) ) }
		//, m_ssaoFbo{ DoCreateFbo( p_engine, p_size ) }
		//, m_ssaoResultAttach{ DoCreateAttach( *m_ssaoFbo, m_ssaoResult ) }
		, m_ssaoProgram{ DoGetSsaoProgram( p_engine ) }
		, m_ssaoConfig{ cuT( "SsaoConfig" )
			, *p_engine.GetRenderSystem() }
		, m_blurProgram{ DoGetBlurProgram( p_engine ) }
		, m_blurResult{ p_engine }
		//, m_blurResult{ DoCreateTexture( p_engine, p_size, cuT( "SSAO_Blurred" ) ) }
		//, m_blurFbo{ DoCreateFbo( p_engine, p_size ) }
		//, m_blurResultAttach{ DoCreateAttach( *m_blurFbo, m_blurResult ) }
		, m_viewport{ p_engine }
	{
		DoInitialiseQuadRendering();
		DoInitialiseSsaoPass();
		DoInitialiseBlurPass();
	}

	SsaoPass::~SsaoPass()
	{
		DoCleanupBlurPass();
		DoCleanupSsaoPass();
		DoCleanupQuadRendering();
	}

	void SsaoPass::Render( GeometryPassResult const & p_gp
		, Camera const & p_camera
		, Matrix4x4r const & p_invViewProj
		, Matrix4x4r const & p_invView
		, Matrix4x4r const & p_invProj )
	{
		m_gpInfo->Update( m_size
			, p_camera
			, p_invViewProj
			, p_invView
			, p_invProj );
		DoRenderSsao( p_gp );
		DoRenderBlur();
	}

	void SsaoPass::DoInitialiseQuadRendering()
	{
		m_viewport.SetOrtho( 0, 1, 0, 1, 0, 1 );
		m_viewport.Initialise();
		m_viewport.Resize( m_size );
		m_viewport.Update();
	}

	void SsaoPass::DoInitialiseSsaoPass()
	{
		auto & l_renderSystem = *m_engine.GetRenderSystem();
		float const l_wScale = m_size.width() / 4.0f;
		float const l_hScale = m_size.height() / 4.0f;
		m_kernelUniform = m_ssaoConfig.CreateUniform< UniformType::eVec3f >( cuT( "c3d_kernel" ), 64u );
		m_ssaoConfig.CreateUniform< UniformType::eInt >( cuT( "c3d_kernelSize" ) )->SetValue( 64 );
		m_ssaoConfig.CreateUniform< UniformType::eFloat >( cuT( "c3d_radius" ) )->SetValue( 0.5f );
		m_ssaoConfig.CreateUniform< UniformType::eFloat >( cuT( "c3d_bias" ) )->SetValue( 0.025f );
		m_ssaoConfig.CreateUniform< UniformType::eVec2f >( cuT( "c3d_noiseScale" ) )->SetValue( Point2f( l_wScale, l_hScale ) );
		m_kernelUniform->SetValues( m_ssaoKernel );

		auto l_sampler = DoCreateSampler( m_engine, cuT( "SSAO_Result" ), WrapMode::eClampToEdge );
		auto l_ssaoResult = l_renderSystem.CreateTexture( TextureType::eTwoDimensions
			, AccessType::eNone
			, AccessType::eRead | AccessType::eWrite );
		l_ssaoResult->SetSource( PxBufferBase::create( m_size
			, PixelFormat::eA8R8G8B8 ) );
		m_ssaoResult.SetTexture( l_ssaoResult );
		m_ssaoResult.SetSampler( l_sampler );
		m_ssaoResult.SetIndex( 0u );
		m_ssaoResult.Initialise();

		m_ssaoFbo = l_renderSystem.CreateFrameBuffer();
		m_ssaoFbo->Create();
		m_ssaoFbo->Initialise( m_size );

		m_ssaoResultAttach = m_ssaoFbo->CreateAttachment( l_ssaoResult );
		m_ssaoFbo->Bind();
		m_ssaoFbo->Attach( AttachmentPoint::eColour, 0u, m_ssaoResultAttach, l_ssaoResult->GetType() );
		m_ssaoFbo->SetDrawBuffer( m_ssaoResultAttach );
		ENSURE( m_ssaoFbo->IsComplete() );
		m_ssaoFbo->Unbind();

		m_gpInfo = std::make_unique< GpInfo >( m_engine );

		m_ssaoPipeline = DoCreatePipeline( m_engine, *m_ssaoProgram );
		m_ssaoPipeline->AddUniformBuffer( m_matrixUbo.GetUbo() );
		m_ssaoPipeline->AddUniformBuffer( m_ssaoConfig );
		m_ssaoPipeline->AddUniformBuffer( m_gpInfo->GetUbo() );

		m_ssaoVertexBuffer = DoCreateVbo( m_engine );
		m_ssaoGeometryBuffers = l_renderSystem.CreateGeometryBuffers( Topology::eTriangles
			, *m_ssaoProgram );
		m_ssaoGeometryBuffers->Initialise( { *m_ssaoVertexBuffer }
			, nullptr );
		m_matrixUbo.Update( m_viewport.GetProjection() );
	}

	void SsaoPass::DoInitialiseBlurPass()
	{
		auto & l_renderSystem = *m_engine.GetRenderSystem();
		auto l_sampler = DoCreateSampler( m_engine, cuT( "SSAO_Blurred" ), WrapMode::eClampToEdge );
		auto l_blurResult = l_renderSystem.CreateTexture( TextureType::eTwoDimensions
			, AccessType::eNone
			, AccessType::eRead | AccessType::eWrite );
		l_blurResult->SetSource( PxBufferBase::create( m_size
			, PixelFormat::eA8R8G8B8 ) );
		m_blurResult.SetTexture( l_blurResult );
		m_blurResult.SetSampler( l_sampler );
		m_blurResult.Initialise();

		m_blurFbo = l_renderSystem.CreateFrameBuffer();
		m_blurFbo->Create();
		m_blurFbo->Initialise( m_size );

		m_blurResultAttach = m_blurFbo->CreateAttachment( l_blurResult );
		m_blurFbo->Bind();
		m_blurFbo->Attach( AttachmentPoint::eColour, 0u, m_blurResultAttach, l_blurResult->GetType() );
		m_blurFbo->SetDrawBuffer( m_blurResultAttach );
		ENSURE( m_blurFbo->IsComplete() );
		m_blurFbo->Unbind();

		m_blurPipeline = DoCreatePipeline( m_engine, *m_blurProgram );
		m_blurPipeline->AddUniformBuffer( m_matrixUbo.GetUbo() );

		m_blurVertexBuffer = DoCreateVbo( m_engine );
		m_blurGeometryBuffers = l_renderSystem.CreateGeometryBuffers( Topology::eTriangles
			, *m_blurProgram );
		m_blurGeometryBuffers->Initialise( { *m_blurVertexBuffer }
			, nullptr );
	}

	void SsaoPass::DoCleanupQuadRendering()
	{
		m_viewport.Cleanup();
		m_matrixUbo.GetUbo().Cleanup();
	}

	void SsaoPass::DoCleanupSsaoPass()
	{
		m_ssaoGeometryBuffers->Cleanup();
		m_ssaoGeometryBuffers.reset();
		m_ssaoVertexBuffer->Cleanup();
		m_ssaoVertexBuffer.reset();
		m_ssaoPipeline->Cleanup();
		m_ssaoPipeline.reset();
		m_ssaoConfig.Cleanup();
		m_ssaoProgram.reset();
		m_ssaoNoise.Cleanup();
		m_ssaoFbo->Bind();
		m_ssaoFbo->DetachAll();
		m_ssaoFbo->Unbind();
		m_ssaoFbo->Cleanup();
		m_ssaoFbo->Destroy();
		m_ssaoFbo.reset();
		m_ssaoResultAttach.reset();
		m_ssaoResult.Cleanup();
		m_gpInfo.reset();
	}

	void SsaoPass::DoCleanupBlurPass()
	{
		m_blurGeometryBuffers->Cleanup();
		m_blurGeometryBuffers.reset();
		m_blurVertexBuffer->Cleanup();
		m_blurVertexBuffer.reset();
		m_blurPipeline->Cleanup();
		m_blurPipeline.reset();
		m_blurProgram.reset();
		m_blurFbo->Bind();
		m_blurFbo->DetachAll();
		m_blurFbo->Unbind();
		m_blurFbo->Cleanup();
		m_blurFbo->Destroy();
		m_blurFbo.reset();
		m_blurResultAttach.reset();
		m_blurResult.Cleanup();
	}

	void SsaoPass::DoRenderSsao( GeometryPassResult const & p_gp )
	{
		m_ssaoFbo->Bind( FrameBufferTarget::eDraw );
		m_ssaoFbo->Clear( BufferComponent::eColour );
		p_gp[size_t( DsTexture::eDepth )]->GetTexture()->Bind( 0u );
		p_gp[size_t( DsTexture::eDepth )]->GetSampler()->Bind( 0u );
		p_gp[size_t( DsTexture::eNormal )]->GetTexture()->Bind( 1u );
		p_gp[size_t( DsTexture::eNormal )]->GetSampler()->Bind( 1u );
		m_ssaoNoise.Bind();
		m_ssaoPipeline->Apply();
		m_ssaoGeometryBuffers->Draw( 6u, 0 );
		m_ssaoNoise.Unbind();
		p_gp[size_t( DsTexture::eNormal )]->GetTexture()->Unbind( 1u );
		p_gp[size_t( DsTexture::eNormal )]->GetSampler()->Unbind( 1u );
		p_gp[size_t( DsTexture::eDepth )]->GetTexture()->Unbind( 0u );
		p_gp[size_t( DsTexture::eDepth )]->GetSampler()->Unbind( 0u );
		m_ssaoFbo->Unbind();
	}

	void SsaoPass::DoRenderBlur()
	{
		m_viewport.Apply();
		m_blurFbo->Bind( FrameBufferTarget::eDraw );
		m_blurFbo->Clear( BufferComponent::eColour );
		m_ssaoResult.Bind();
		m_blurPipeline->Apply();
		m_blurGeometryBuffers->Draw( 6u, 0 );
		m_ssaoResult.Unbind();
		m_blurFbo->Unbind();
	}
}
