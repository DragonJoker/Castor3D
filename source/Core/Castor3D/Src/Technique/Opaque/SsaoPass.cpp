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

namespace Castor3D
{
	namespace
	{
		float DoLerp( float a, float b, float f )
		{
			return a + f * ( b - a );
		}

		Point3fArray DoGetKernel()
		{
			constexpr uint32_t size = 64;
			std::uniform_real_distribution< float > distribution( 0.0f, 1.0f );
			std::default_random_engine generator;
			Point3fArray result;
			result.reserve( size );

			for ( auto i = 0u; i < size; ++i )
			{
				auto sample = point::get_normalised( Point3f{ distribution( generator ) * 2.0f - 1.0f
					, distribution( generator ) * 2.0f - 1.0f
					, distribution( generator ) } );
				sample *= distribution( generator );
				auto scale = i / float( size );
				scale = DoLerp( 0.1f, 1.0f, scale * scale );
				sample *= scale;
				result.push_back( sample );
			}

			return result;
		}

		VertexBufferSPtr DoCreateVbo( Engine & p_engine )
		{
			auto declaration = BufferDeclaration(
			{
				BufferElementDeclaration( ShaderProgram::Position, uint32_t( ElementUsage::ePosition ), ElementType::eVec2 ),
				BufferElementDeclaration{ ShaderProgram::Texture, uint32_t( ElementUsage::eTexCoords ), ElementType::eVec2 },
			} );

			float data[] =
			{
				0, 0, 0, 0,
				1, 1, 1, 1,
				0, 1, 0, 1,
				0, 0, 0, 0,
				1, 0, 1, 0,
				1, 1, 1, 1
			};

			auto & renderSystem = *p_engine.GetRenderSystem();
			auto vertexBuffer = std::make_shared< VertexBuffer >( p_engine, declaration );
			uint32_t stride = declaration.stride();
			vertexBuffer->Resize( uint32_t( sizeof( data ) ) );
			uint8_t * buffer = vertexBuffer->GetData();
			std::memcpy( buffer, data, sizeof( data ) );
			vertexBuffer->Initialise( BufferAccessType::eStatic
				, BufferAccessNature::eDraw );
			return vertexBuffer;
		}

		TextureUnit DoGetNoise( Engine & p_engine )
		{
			constexpr uint32_t size = 16;
			std::uniform_real_distribution< float > distribution( 0.0f, 1.0f );
			std::default_random_engine generator;
			Point3fArray noise;
			noise.reserve( size );

			for ( auto i = 0u; i < size; i++ )
			{
				noise.emplace_back( distribution( generator ) * 2.0 - 1.0
					, distribution( generator ) * 2.0 - 1.0
					, 0.0f );
			}

			auto buffer = PxBufferBase::create( Size{ 4, 4 }
				, PixelFormat::eRGB32F
				, reinterpret_cast< uint8_t const * >( noise[0].const_ptr() )
				, PixelFormat::eRGB32F );

			auto texture = p_engine.GetRenderSystem()->CreateTexture( TextureType::eTwoDimensions
				, AccessType::eNone
				, AccessType::eRead );
			texture->SetSource( buffer );

			SamplerSPtr sampler;

			if ( p_engine.GetSamplerCache().Has( cuT( "SSAO_Noise" ) ) )
			{
				sampler = p_engine.GetSamplerCache().Find( cuT( "SSAO_Noise" ) );
			}
			else
			{
				sampler = p_engine.GetSamplerCache().Add( cuT( "SSAO_Noise" ) );
				sampler->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eNearest );
				sampler->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eNearest );
				sampler->SetWrappingMode( TextureUVW::eU, WrapMode::eRepeat );
				sampler->SetWrappingMode( TextureUVW::eV, WrapMode::eRepeat );
			}

			TextureUnit result{ p_engine };
			result.SetSampler( sampler );
			result.SetTexture( texture );
			result.Initialise();
			result.SetIndex( 2u );
			return result;
		}

		GLSL::Shader DoGetSsaoVertexProgram( Engine & p_engine )
		{
			auto & renderSystem = *p_engine.GetRenderSystem();
			using namespace GLSL;
			auto writer = renderSystem.CreateGlslWriter();

			// Shader inputs
			UBO_MATRIX( writer );
			UBO_GPINFO( writer );
			auto position = writer.DeclAttribute< Vec2 >( ShaderProgram::Position );
			auto texture = writer.DeclAttribute< Vec2 >( ShaderProgram::Texture );

			// Shader outputs
			auto gl_Position = writer.DeclBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.ImplementFunction< void >( cuT( "main" )
				, [&]()
				{
					gl_Position = c3d_mtxProjection * vec4( position, 0.0, 1.0 );
				} );
			return writer.Finalise();
		}
		
		GLSL::Shader DoGetSsaoPixelProgram( Engine & p_engine )
		{
			auto & renderSystem = *p_engine.GetRenderSystem();
			using namespace GLSL;
			auto writer = renderSystem.CreateGlslWriter();

			// Shader inputs
			UBO_MATRIX( writer );
			UBO_GPINFO( writer );
			auto c3d_mapDepth = writer.DeclUniform< Sampler2D >( cuT( "c3d_mapDepth" ) );
			auto c3d_mapNormal = writer.DeclUniform< Sampler2D >( cuT( "c3d_mapNormal" ) );
			auto c3d_mapNoise = writer.DeclUniform< Sampler2D >( cuT( "c3d_mapNoise" ) );

			Ubo ssaoConfig{ writer, cuT( "SsaoConfig" ), 8u };
			auto c3d_kernel = ssaoConfig.DeclMember< Vec3 >( cuT( "c3d_kernel" ), 64u );
			auto c3d_kernelSize = ssaoConfig.DeclMember< Int >( cuT( "c3d_kernelSize" ) );
			auto c3d_radius = ssaoConfig.DeclMember< Float >( cuT( "c3d_radius" ) );
			auto c3d_bias = ssaoConfig.DeclMember< Float >( cuT( "c3d_bias" ) );
			auto c3d_noiseScale = ssaoConfig.DeclMember< Vec2 >( cuT( "c3d_noiseScale" ) );
			ssaoConfig.End();

			GLSL::Utils utils{ writer };
			utils.DeclareCalcTexCoord();
			utils.DeclareCalcVSPosition();
			utils.DeclareCalcVSDepth();

			// Shader outputs
			auto pxl_fragColor = writer.DeclOutput< Vec4 >( cuT( "pxl_fragColor" ) );

			writer.ImplementFunction< Void >( cuT( "main" )
				, [&]()
				{
					auto texCoord = writer.DeclLocale( cuT( "texCoord" ), utils.CalcTexCoord() );

					auto vsPosition = writer.DeclLocale( cuT( "vsPosition" )
						, utils.CalcVSPosition( texCoord, c3d_mtxInvProj ) );
					auto vsNormal = writer.DeclLocale( cuT( "vsNormal" )
						, normalize( writer.Paren( c3d_mtxInvView * vec4( texture( c3d_mapNormal, texCoord ).xyz(), 1.0_f ) ).xyz() ) );

					auto randomVec = writer.DeclLocale( cuT( "randomVec" )
						, normalize( texture( c3d_mapNoise, texCoord * c3d_noiseScale ).xyz() ) );
					auto tangent = writer.DeclLocale( cuT( "tangent" )
						, normalize( randomVec - vsNormal * dot( randomVec, vsNormal ) ) );
					auto bitangent = writer.DeclLocale( cuT( "bitangent" )
						, cross( vsNormal, tangent ) );
					auto tbn = writer.DeclLocale( cuT( "tbn" )
						, mat3( tangent, bitangent, vsNormal ) );
					auto occlusion = writer.DeclLocale( cuT( "occlusion" ), 0.0_f );

					FOR( writer, Int, i, 0, cuT( "i < c3d_kernelSize" ), cuT( "++i" ) )
					{
						// get sample position
						auto samplePos = writer.DeclLocale( cuT( "samplePos" )
							, tbn * c3d_kernel[i] );                       // From tangent to view-space
						samplePos = vsPosition + samplePos * c3d_radius;
						auto offset = writer.DeclLocale( cuT( "offset" )
							, vec4( samplePos, 1.0 ) );
						offset = c3d_mtxGProj * offset;                  // from view to clip-space
						offset.xyz() = offset.xyz() / offset.w();      // perspective divide
						offset.xyz() = offset.xyz() * 0.5 + 0.5;         // transform to range 0.0 - 1.0 
						auto sampleDepth = writer.DeclLocale( cuT( "sampleDepth" )
							, utils.CalcVSPosition( offset.xy(), c3d_mtxInvProj ).z() );
						auto rangeCheck = writer.DeclLocale( cuT( "rangeCheck" )
							, smoothstep( 0.0_f, 1.0_f, c3d_radius / GLSL::abs( vsPosition.z() - sampleDepth ) ) );
						occlusion += writer.Ternary( sampleDepth >= samplePos.z() + c3d_bias, 1.0_f, 0.0_f ) * rangeCheck;
					}
					ROF;

					occlusion = 1.0_f - writer.Paren( occlusion / c3d_kernelSize );
					pxl_fragColor = vec4( vec3( occlusion ), 1.0 );
			} );
			return writer.Finalise();
		}
		
		GLSL::Shader DoGetBlurVertexProgram( Engine & p_engine )
		{
			auto & renderSystem = *p_engine.GetRenderSystem();
			using namespace GLSL;
			auto writer = renderSystem.CreateGlslWriter();

			UBO_MATRIX( writer );

			// Shader inputs
			auto position = writer.DeclAttribute< Vec2 >( ShaderProgram::Position );
			auto texture = writer.DeclAttribute< Vec2 >( ShaderProgram::Texture );

			// Shader outputs
			auto vtx_texture = writer.DeclOutput< Vec2 >( cuT( "vtx_texture" ) );
			auto gl_Position = writer.DeclBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.ImplementFunction< void >( cuT( "main" )
				, [&]()
				{
					vtx_texture = texture;
					gl_Position = c3d_mtxProjection * vec4( position.x(), position.y(), 0.0, 1.0 );
				} );
			return writer.Finalise();
		}

		GLSL::Shader DoGetBlurPixelProgram( Engine & p_engine )
		{
			auto & renderSystem = *p_engine.GetRenderSystem();
			using namespace GLSL;
			auto writer = renderSystem.CreateGlslWriter();

			// Shader inputs
			auto vtx_texture = writer.DeclInput< Vec2 >( cuT( "vtx_texture" ) );
			auto c3d_mapColour = writer.DeclUniform< Sampler2D >( cuT( "c3d_mapColour" ) );

			// Shader outputs
			auto pxl_fragColor = writer.DeclOutput< Vec4 >( cuT( "pxl_fragColor" ) );

			writer.ImplementFunction< Void >( cuT( "main" )
				, [&]()
				{
					auto texelSize = writer.DeclLocale( cuT( "texelSize" ), vec2( 1.0_f, 1.0_f ) / vec2( textureSize( c3d_mapColour, 0 ) ) );
					auto result = writer.DeclLocale( cuT( "result" ), 0.0_f );

					FOR( writer, Int, x, -2, cuT( "x < 2" ), cuT( "++x" ) )
					{
						FOR( writer, Int, y, -2, cuT( "y < 2" ), cuT( "++y" ) )
						{
							auto offset = writer.DeclLocale( cuT( "offset" ), vec2( writer.Cast< Float >( x ), writer.Cast< Float >( y ) ) * texelSize );
							result += texture( c3d_mapColour, vtx_texture + offset ).r();
						}
						ROF;
					}
					ROF;

					result /= writer.Paren( 4.0_f * 4.0_f );
					pxl_fragColor = vec4( vec3( result ), 1.0 );
				} );
			return writer.Finalise();
		}

		ShaderProgramSPtr DoGetSsaoProgram( Engine & p_engine )
		{
			auto & renderSystem = *p_engine.GetRenderSystem();
			auto vtx = DoGetSsaoVertexProgram( p_engine );
			auto pxl = DoGetSsaoPixelProgram( p_engine );
			ShaderProgramSPtr program = p_engine.GetShaderProgramCache().GetNewProgram( false );
			program->CreateObject( ShaderType::eVertex );
			program->CreateObject( ShaderType::ePixel );
			program->CreateUniform< UniformType::eSampler >( cuT( "c3d_mapDepth" ), ShaderType::ePixel )->SetValue( 0 );
			program->CreateUniform< UniformType::eSampler >( cuT( "c3d_mapNormal" ), ShaderType::ePixel )->SetValue( 1 );
			program->CreateUniform< UniformType::eSampler >( cuT( "c3d_mapNoise" ), ShaderType::ePixel )->SetValue( 2 );
			program->SetSource( ShaderType::eVertex, vtx );
			program->SetSource( ShaderType::ePixel, pxl );
			program->Initialise();
			return program;
		}

		ShaderProgramSPtr DoGetBlurProgram( Engine & p_engine )
		{
			auto & renderSystem = *p_engine.GetRenderSystem();
			auto vtx = DoGetBlurVertexProgram( p_engine );
			auto pxl = DoGetBlurPixelProgram( p_engine );
			ShaderProgramSPtr program = p_engine.GetShaderProgramCache().GetNewProgram( false );
			program->CreateObject( ShaderType::eVertex );
			program->CreateObject( ShaderType::ePixel );
			program->CreateUniform< UniformType::eSampler >( cuT( "c3d_mapColour" ), ShaderType::ePixel )->SetValue( 0 );
			program->SetSource( ShaderType::eVertex, vtx );
			program->SetSource( ShaderType::ePixel, pxl );
			program->Initialise();
			return program;
		}

		RenderPipelineUPtr DoCreatePipeline( Engine & p_engine
			, ShaderProgram & p_program )
		{
			DepthStencilState dsstate;
			dsstate.SetDepthTest( false );
			dsstate.SetDepthMask( WritingMask::eZero );
			RasteriserState rsstate;
			rsstate.SetCulledFaces( Culling::eNone );
			return p_engine.GetRenderSystem()->CreateRenderPipeline( std::move( dsstate )
				, std::move( rsstate )
				, BlendState{}
				, MultisampleState{}
				, p_program
				, PipelineFlags{} );
		}

		SamplerSPtr DoCreateSampler( Engine & p_engine, String const & p_name, WrapMode p_mode )
		{
			SamplerSPtr sampler;

			if ( p_engine.GetSamplerCache().Has( p_name ) )
			{
				sampler = p_engine.GetSamplerCache().Find( p_name );
			}
			else
			{
				sampler = p_engine.GetSamplerCache().Add( p_name );
				sampler->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eNearest );
				sampler->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eNearest );
				sampler->SetWrappingMode( TextureUVW::eU, p_mode );
				sampler->SetWrappingMode( TextureUVW::eV, p_mode );
			}

			return sampler;
		}

		TextureUnit DoCreateTexture( Engine & p_engine
			, Size const & p_size
			, String const & p_name )
		{
			auto & renderSystem = *p_engine.GetRenderSystem();
			auto sampler = DoCreateSampler( p_engine, p_name, WrapMode::eClampToEdge );
			auto ssaoResult = renderSystem.CreateTexture( TextureType::eTwoDimensions
				, AccessType::eNone
				, AccessType::eRead | AccessType::eWrite );
			ssaoResult->SetSource( PxBufferBase::create( p_size
				, PixelFormat::eL32F ) );
			TextureUnit unit{ p_engine };
			unit.SetTexture( ssaoResult );
			unit.SetSampler( sampler );
			unit.SetIndex( 0u );
			unit.Initialise();
			return unit;
		}

		FrameBufferSPtr DoCreateFbo( Engine & p_engine
			, Size const & p_size )
		{
			auto & renderSystem = *p_engine.GetRenderSystem();
			auto fbo = renderSystem.CreateFrameBuffer();
			fbo->Create();
			fbo->Initialise( p_size );
			return fbo;
		}

		TextureAttachmentSPtr DoCreateAttach( FrameBuffer & p_fbo
			, TextureUnit const & p_unit )
		{
			auto attach = p_fbo.CreateAttachment( p_unit.GetTexture() );
			p_fbo.Bind();
			p_fbo.Attach( AttachmentPoint::eColour, 0u, attach, p_unit.GetTexture()->GetType() );
			p_fbo.SetDrawBuffer( attach );
			ENSURE( p_fbo.IsComplete() );
			p_fbo.Unbind();
			return attach;
		}
	}

	//*********************************************************************************************

	SsaoPass::SsaoPass( Engine & p_engine
		, Size const & p_size
		, SsaoConfig const & p_config )
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
		, m_config{ p_config }
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
		auto & renderSystem = *m_engine.GetRenderSystem();
		float const wScale = m_size.width() / 4.0f;
		float const hScale = m_size.height() / 4.0f;
		m_kernelUniform = m_ssaoConfig.CreateUniform< UniformType::eVec3f >( cuT( "c3d_kernel" ), 64u );
		m_ssaoConfig.CreateUniform< UniformType::eInt >( cuT( "c3d_kernelSize" ) )->SetValue( 64 );
		m_ssaoConfig.CreateUniform< UniformType::eFloat >( cuT( "c3d_radius" ) )->SetValue( m_config.m_radius );
		m_ssaoConfig.CreateUniform< UniformType::eFloat >( cuT( "c3d_bias" ) )->SetValue( m_config.m_bias );
		m_ssaoConfig.CreateUniform< UniformType::eVec2f >( cuT( "c3d_noiseScale" ) )->SetValue( Point2f( wScale, hScale ) );
		m_kernelUniform->SetValues( m_ssaoKernel );

		auto sampler = DoCreateSampler( m_engine, cuT( "SSAO_Result" ), WrapMode::eClampToEdge );
		auto ssaoResult = renderSystem.CreateTexture( TextureType::eTwoDimensions
			, AccessType::eNone
			, AccessType::eRead | AccessType::eWrite );
		ssaoResult->SetSource( PxBufferBase::create( m_size
			, PixelFormat::eA8R8G8B8 ) );
		m_ssaoResult.SetTexture( ssaoResult );
		m_ssaoResult.SetSampler( sampler );
		m_ssaoResult.SetIndex( 0u );
		m_ssaoResult.Initialise();

		m_ssaoFbo = renderSystem.CreateFrameBuffer();
		m_ssaoFbo->Create();
		m_ssaoFbo->Initialise( m_size );

		m_ssaoResultAttach = m_ssaoFbo->CreateAttachment( ssaoResult );
		m_ssaoFbo->Bind();
		m_ssaoFbo->Attach( AttachmentPoint::eColour, 0u, m_ssaoResultAttach, ssaoResult->GetType() );
		m_ssaoFbo->SetDrawBuffer( m_ssaoResultAttach );
		ENSURE( m_ssaoFbo->IsComplete() );
		m_ssaoFbo->Unbind();

		m_gpInfo = std::make_unique< GpInfoUbo >( m_engine );

		m_ssaoPipeline = DoCreatePipeline( m_engine, *m_ssaoProgram );
		m_ssaoPipeline->AddUniformBuffer( m_matrixUbo.GetUbo() );
		m_ssaoPipeline->AddUniformBuffer( m_ssaoConfig );
		m_ssaoPipeline->AddUniformBuffer( m_gpInfo->GetUbo() );

		m_ssaoVertexBuffer = DoCreateVbo( m_engine );
		m_ssaoGeometryBuffers = renderSystem.CreateGeometryBuffers( Topology::eTriangles
			, *m_ssaoProgram );
		m_ssaoGeometryBuffers->Initialise( { *m_ssaoVertexBuffer }
			, nullptr );
		m_matrixUbo.Update( m_viewport.GetProjection() );
	}

	void SsaoPass::DoInitialiseBlurPass()
	{
		auto & renderSystem = *m_engine.GetRenderSystem();
		auto sampler = DoCreateSampler( m_engine, cuT( "SSAO_Blurred" ), WrapMode::eClampToEdge );
		auto blurResult = renderSystem.CreateTexture( TextureType::eTwoDimensions
			, AccessType::eNone
			, AccessType::eRead | AccessType::eWrite );
		blurResult->SetSource( PxBufferBase::create( m_size
			, PixelFormat::eA8R8G8B8 ) );
		m_blurResult.SetTexture( blurResult );
		m_blurResult.SetSampler( sampler );
		m_blurResult.Initialise();

		m_blurFbo = renderSystem.CreateFrameBuffer();
		m_blurFbo->Create();
		m_blurFbo->Initialise( m_size );

		m_blurResultAttach = m_blurFbo->CreateAttachment( blurResult );
		m_blurFbo->Bind();
		m_blurFbo->Attach( AttachmentPoint::eColour, 0u, m_blurResultAttach, blurResult->GetType() );
		m_blurFbo->SetDrawBuffer( m_blurResultAttach );
		ENSURE( m_blurFbo->IsComplete() );
		m_blurFbo->Unbind();

		m_blurPipeline = DoCreatePipeline( m_engine, *m_blurProgram );
		m_blurPipeline->AddUniformBuffer( m_matrixUbo.GetUbo() );

		m_blurVertexBuffer = DoCreateVbo( m_engine );
		m_blurGeometryBuffers = renderSystem.CreateGeometryBuffers( Topology::eTriangles
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
		m_ssaoConfig.BindTo( 8u );
		m_ssaoFbo->Bind( FrameBufferTarget::eDraw );
		m_ssaoFbo->Clear( BufferComponent::eColour );
		p_gp[size_t( DsTexture::eDepth )]->GetTexture()->Bind( 0u );
		p_gp[size_t( DsTexture::eDepth )]->GetSampler()->Bind( 0u );
		p_gp[size_t( DsTexture::eData1 )]->GetTexture()->Bind( 1u );
		p_gp[size_t( DsTexture::eData1 )]->GetSampler()->Bind( 1u );
		m_ssaoNoise.Bind();
		m_ssaoPipeline->Apply();
		m_ssaoGeometryBuffers->Draw( 6u, 0 );
		m_ssaoNoise.Unbind();
		p_gp[size_t( DsTexture::eData1 )]->GetTexture()->Unbind( 1u );
		p_gp[size_t( DsTexture::eData1 )]->GetSampler()->Unbind( 1u );
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
