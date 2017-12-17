#include "SsaoPass.hpp"

#include "LightPass.hpp"

#include "Engine.hpp"
#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "Mesh/Buffer/BufferElementGroup.hpp"
#include "Mesh/Buffer/GeometryBuffers.hpp"
#include "Mesh/Buffer/VertexBuffer.hpp"
#include "Render/RenderPassTimer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/Camera.hpp"
#include "Shader/ShaderProgram.hpp"
#include "State/BlendState.hpp"
#include "State/DepthStencilState.hpp"
#include "State/MultisampleState.hpp"
#include "State/RasteriserState.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"
#include "Texture/TextureUnit.hpp"

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

#include "Shader/Shaders/GlslLight.hpp"
#include "Shader/Shaders/GlslShadow.hpp"

#include <random>

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	namespace
	{
		float doLerp( float a, float b, float f )
		{
			return a + f * ( b - a );
		}

		Point3fArray doGetKernel( uint32_t size )
		{
			std::uniform_real_distribution< float > distribution( 0.0f, 1.0f );
			std::default_random_engine generator;
			Point3fArray result;
			result.reserve( size );

			for ( auto i = 0u; i < size; ++i )
			{
				auto sample = point::getNormalised( Point3f{ distribution( generator ) * 2.0f - 1.0f
					, distribution( generator ) * 2.0f - 1.0f
					, distribution( generator ) } );
				sample *= distribution( generator );
				auto scale = i / float( size );
				scale = doLerp( 0.1f, 1.0f, scale * scale );
				sample *= scale;
				result.push_back( sample );
			}

			return result;
		}

		VertexBufferSPtr doCreateVbo( Engine & engine )
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

			auto vertexBuffer = std::make_shared< VertexBuffer >( engine, declaration );
			vertexBuffer->resize( uint32_t( sizeof( data ) ) );
			uint8_t * buffer = vertexBuffer->getData();
			std::memcpy( buffer, data, sizeof( data ) );
			vertexBuffer->initialise( BufferAccessType::eStatic
				, BufferAccessNature::eDraw );
			return vertexBuffer;
		}

		TextureUnit doGetNoise( Engine & engine )
		{
			constexpr uint32_t size = 16;
			std::uniform_real_distribution< float > distribution( 0.0f, 1.0f );
			std::default_random_engine generator;
			Point3fArray noise;
			noise.reserve( size );

			for ( auto i = 0u; i < size; i++ )
			{
				noise.push_back( point::getNormalised( Point3r{ distribution( generator ) * 2.0 - 1.0
					, distribution( generator ) * 2.0 - 1.0
					, 0.0f } ) );
			}

			auto buffer = PxBufferBase::create( Size{ 4, 4 }
				, PixelFormat::eRGB32F
				, reinterpret_cast< uint8_t const * >( noise[0].constPtr() )
				, PixelFormat::eRGB32F );

			auto texture = engine.getRenderSystem()->createTexture( TextureType::eTwoDimensions
				, AccessType::eNone
				, AccessType::eRead );
			texture->setSource( buffer );

			SamplerSPtr sampler;

			if ( engine.getSamplerCache().has( cuT( "SSAO_Noise" ) ) )
			{
				sampler = engine.getSamplerCache().find( cuT( "SSAO_Noise" ) );
			}
			else
			{
				sampler = engine.getSamplerCache().add( cuT( "SSAO_Noise" ) );
				sampler->setInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eNearest );
				sampler->setInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eNearest );
				sampler->setWrappingMode( TextureUVW::eU, WrapMode::eRepeat );
				sampler->setWrappingMode( TextureUVW::eV, WrapMode::eRepeat );
			}

			TextureUnit result{ engine };
			result.setSampler( sampler );
			result.setTexture( texture );
			result.initialise();
			result.setIndex( MinTextureIndex + 2u );
			return result;
		}

		glsl::Shader doGetSsaoVertexProgram( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			UBO_MATRIX( writer );
			UBO_GPINFO( writer );
			auto position = writer.declAttribute< Vec2 >( ShaderProgram::Position );
			auto texture = writer.declAttribute< Vec2 >( ShaderProgram::Texture );

			// Shader outputs
			auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
					gl_Position = c3d_projection * vec4( position, 0.0, 1.0 );
				} );
			return writer.finalise();
		}
		
		glsl::Shader doGetSsaoPixelProgram( Engine & engine
			, SsaoConfig const & config )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			UBO_MATRIX( writer );
			UBO_GPINFO( writer );
			auto index = MinTextureIndex;
			auto c3d_mapDepth = writer.declSampler< Sampler2D >( cuT( "c3d_mapDepth" ), index++ );
			auto c3d_mapNormal = writer.declSampler< Sampler2D >( cuT( "c3d_mapNormal" ), index++ );
			auto c3d_mapNoise = writer.declSampler< Sampler2D >( cuT( "c3d_mapNoise" ), index++ );

			Ubo ssaoConfig{ writer, cuT( "SsaoConfig" ), 8u };
			auto c3d_kernel = ssaoConfig.declMember< Vec3 >( cuT( "c3d_kernel" ), config.m_kernelSize.range().max() );
			auto c3d_kernelSize = ssaoConfig.declMember< Int >( cuT( "c3d_kernelSize" ) );
			auto c3d_radius = ssaoConfig.declMember< Float >( cuT( "c3d_radius" ) );
			auto c3d_bias = ssaoConfig.declMember< Float >( cuT( "c3d_bias" ) );
			auto c3d_noiseScale = ssaoConfig.declMember< Vec2 >( cuT( "c3d_noiseScale" ) );
			auto c3d_ssaoSize = ssaoConfig.declMember< Vec2 >( cuT( "c3d_ssaoSize" ) );
			ssaoConfig.end();

			glsl::Utils utils{ writer };
			utils.declareCalcTexCoord();
			utils.declareCalcVSPosition();

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Float >( cuT( "pxl_fragColor" ) );

			writer.implementFunction< Void >( cuT( "main" )
				, [&]()
				{
					auto texCoord = writer.declLocale( cuT( "texCoord" )
						, utils.calcTexCoord( c3d_ssaoSize ) );
					auto vsPosition = writer.declLocale( cuT( "vsPosition" )
						, utils.calcVSPosition( texCoord
							, texture( c3d_mapDepth, texCoord ).r()
							, c3d_mtxInvProj ) );
					auto vsNormal = writer.declLocale( cuT( "vsNormal" )
						, normalize( writer.paren( c3d_mtxInvView * vec4( texture( c3d_mapNormal, texCoord ).xyz(), 1.0_f ) ).xyz() ) );

					auto randomVec = writer.declLocale( cuT( "randomVec" )
						, normalize( texture( c3d_mapNoise, texCoord * c3d_noiseScale ).xyz() ) );
					auto tangent = writer.declLocale( cuT( "tangent" )
						, normalize( glsl::fma( -vsNormal, vec3( dot( randomVec, vsNormal ) ), randomVec ) ) );
					auto bitangent = writer.declLocale( cuT( "bitangent" )
						, cross( vsNormal, tangent ) );
					auto tbn = writer.declLocale( cuT( "tbn" )
						, mat3( tangent, bitangent, vsNormal ) );
					auto occlusion = writer.declLocale( cuT( "occlusion" )
						, 0.0_f );
					auto radius = writer.declLocale( cuT( "radius" )
						, vec3( c3d_radius ) );
					auto samplePos = writer.declLocale< Vec3 >( cuT( "samplePos" ) );
					auto offset = writer.declLocale< Vec4 >( cuT( "offset" ) );
					auto sampleDepth = writer.declLocale< Float >( cuT( "sampleDepth" ) );
					auto rangeCheck = writer.declLocale< Float >( cuT( "rangeCheck" ) );

					for ( auto i = 0u; i < config.m_kernelSize.value(); ++i )
					{
						// get sample position
						samplePos = tbn * c3d_kernel[i];                            // From tangent to view-space
						samplePos = glsl::fma( samplePos, radius, vsPosition );
						offset = vec4( samplePos, 1.0 );
						offset = c3d_mtxGProj * offset;                             // from view to clip-space
						offset.xyz() = offset.xyz() / offset.w();                   // perspective divide
						offset.xyz() = glsl::fma( offset.xyz()
							, vec3( 0.5_f )
							, vec3( 0.5_f ) );                                      // transform to range 0.0 - 1.0 
						sampleDepth = utils.calcVSPosition( offset.xy()
							, texture( c3d_mapDepth, offset.xy() ).r()
							, c3d_mtxInvProj ).z();
						rangeCheck = smoothstep( 0.0_f
							, 1.0_f
							, c3d_radius / glsl::abs( vsPosition.z() - sampleDepth ) );
						occlusion += writer.ternary( sampleDepth >= samplePos.z() + c3d_bias
							, 1.0_f
							, 0.0_f ) * rangeCheck;
					}

					occlusion = 1.0_f - writer.paren( occlusion / c3d_kernelSize );
					pxl_fragColor = occlusion;
				} );
			return writer.finalise();
		}
		
		glsl::Shader doGetBlurVertexProgram( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			UBO_MATRIX( writer );

			// Shader inputs
			auto position = writer.declAttribute< Vec2 >( ShaderProgram::Position );
			auto texture = writer.declAttribute< Vec2 >( ShaderProgram::Texture );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ) );
			auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
					vtx_texture = texture;
					gl_Position = c3d_projection * vec4( position.xy(), 0.0, 1.0 );
				} );
			return writer.finalise();
		}

		glsl::Shader doGetBlurPixelProgram( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			Ubo blurConfig{ writer, cuT( "BlurConfig" ), 2u };
			auto c3d_texelSize = blurConfig.declMember< Vec2 >( cuT( "c3d_texelSize" ) );
			auto c3d_noiseSize = blurConfig.declMember< Int >( cuT( "c3d_noiseSize" ) );
			blurConfig.end();
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ) );
			auto c3d_mapColour = writer.declSampler< Sampler2D >( cuT( "c3d_mapColour" ), MinTextureIndex );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Float >( cuT( "pxl_fragColor" ) );

			writer.implementFunction< Void >( cuT( "main" )
				, [&]()
				{
					auto result = writer.declLocale( cuT( "result" )
						, 0.0_f );
					auto hlim = writer.declLocale( cuT( "hlim" )
						, vec2( glsl::fma( writer.cast< Float >( -c3d_noiseSize ), 0.5_f, 0.5_f ) ) );
					auto offset = writer.declLocale< Vec2 >( cuT( "offset" ) );

					FOR( writer, Int, x, 0, "x < c3d_noiseSize", "++x" )
					{
						FOR( writer, Int, y, 0, "y < c3d_noiseSize", "++y" )
						{
							offset = writer.paren( hlim + vec2( Float( x ), Float( y ) ) ) * c3d_texelSize;
							result += texture( c3d_mapColour, vtx_texture + offset ).r();
						}
						ROF;
					}
					ROF;

					pxl_fragColor = result / writer.cast< Float >( c3d_noiseSize * c3d_noiseSize );
				} );
			return writer.finalise();
		}

		ShaderProgramSPtr doGetSsaoProgram( Engine & engine
			, SsaoConfig const & config )
		{
			auto vtx = doGetSsaoVertexProgram( engine );
			auto pxl = doGetSsaoPixelProgram( engine, config );
			ShaderProgramSPtr program = engine.getShaderProgramCache().getNewProgram( false );
			program->createObject( ShaderType::eVertex );
			program->createObject( ShaderType::ePixel );
			program->createUniform< UniformType::eSampler >( cuT( "c3d_mapDepth" ), ShaderType::ePixel )->setValue( MinTextureIndex + 0 );
			program->createUniform< UniformType::eSampler >( cuT( "c3d_mapNormal" ), ShaderType::ePixel )->setValue( MinTextureIndex + 1 );
			program->createUniform< UniformType::eSampler >( cuT( "c3d_mapNoise" ), ShaderType::ePixel )->setValue( MinTextureIndex + 2 );
			program->setSource( ShaderType::eVertex, vtx );
			program->setSource( ShaderType::ePixel, pxl );
			program->initialise();
			return program;
		}

		ShaderProgramSPtr doGetBlurProgram( Engine & engine )
		{
			auto vtx = doGetBlurVertexProgram( engine );
			auto pxl = doGetBlurPixelProgram( engine );
			ShaderProgramSPtr program = engine.getShaderProgramCache().getNewProgram( false );
			program->createObject( ShaderType::eVertex );
			program->createObject( ShaderType::ePixel );
			program->createUniform< UniformType::eSampler >( cuT( "c3d_mapColour" ), ShaderType::ePixel )->setValue( MinTextureIndex + 0 );
			program->setSource( ShaderType::eVertex, vtx );
			program->setSource( ShaderType::ePixel, pxl );
			program->initialise();
			return program;
		}

		RenderPipelineUPtr doCreatePipeline( Engine & engine
			, ShaderProgram & program )
		{
			DepthStencilState dsstate;
			dsstate.setDepthTest( false );
			dsstate.setDepthMask( WritingMask::eZero );
			RasteriserState rsstate;
			rsstate.setCulledFaces( Culling::eNone );
			return engine.getRenderSystem()->createRenderPipeline( std::move( dsstate )
				, std::move( rsstate )
				, BlendState{}
				, MultisampleState{}
				, program
				, PipelineFlags{} );
		}

		SamplerSPtr doCreateSampler( Engine & engine
			, String const & name
			, WrapMode mode )
		{
			SamplerSPtr sampler;

			if ( engine.getSamplerCache().has( name ) )
			{
				sampler = engine.getSamplerCache().find( name );
			}
			else
			{
				sampler = engine.getSamplerCache().add( name );
				sampler->setInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eNearest );
				sampler->setInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eNearest );
				sampler->setWrappingMode( TextureUVW::eU, mode );
				sampler->setWrappingMode( TextureUVW::eV, mode );
			}

			return sampler;
		}
	}

	//*********************************************************************************************

	SsaoPass::SsaoPass( Engine & engine
		, Size const & size
		, SsaoConfig const & config
		, GpInfoUbo & gpInfoUbo )
		: m_engine{ engine }
		, m_size{ size/*.getWidth() / 2, size.getHeight() / 2*/ }
		, m_matrixUbo{ engine }
		, m_ssaoKernel{ doGetKernel( config.m_kernelSize.value() ) }
		, m_ssaoNoise{ doGetNoise( engine ) }
		, m_ssaoResult{ engine }
		, m_ssaoProgram{ doGetSsaoProgram( engine, config ) }
		, m_ssaoConfig{ cuT( "SsaoConfig" )
			, *engine.getRenderSystem()
			, 8u }
		, m_viewport{ engine }
		, m_config{ config }
		, m_ssaoTimer{ std::make_shared< RenderPassTimer >( engine, cuT( "Ssao raw" ), cuT( "Ssao raw" ) ) }
		, m_gpInfoUbo{ gpInfoUbo }
		, m_blurProgram{ doGetBlurProgram( engine ) }
		, m_blurResult{ engine }
		, m_blurConfig{ cuT( "BlurConfig" )
			, *engine.getRenderSystem()
			, 2u }
		, m_blurTimer{ std::make_shared< RenderPassTimer >( engine, cuT( "Ssao blur" ), cuT( "Ssao blur" ) ) }
	{
		doInitialiseQuadRendering();
		doInitialiseSsaoPass();
		doInitialiseBlurPass();
	}

	SsaoPass::~SsaoPass()
	{
		doCleanupBlurPass();
		doCleanupSsaoPass();
		doCleanupQuadRendering();
	}

	void SsaoPass::render( GeometryPassResult const & gp
		, RenderInfo & info )
	{
		doRenderSsao( gp );
		doRenderBlur();
	}

	void SsaoPass::doInitialiseQuadRendering()
	{
		m_viewport.setOrtho( 0, 1, 0, 1, 0, 1 );
		m_viewport.initialise();
		m_viewport.resize( m_size );
		m_viewport.update();
	}

	void SsaoPass::doInitialiseSsaoPass()
	{
		auto & renderSystem = *m_engine.getRenderSystem();
		float const wScale = m_size.getWidth() / 4.0f;
		float const hScale = m_size.getHeight() / 4.0f;
		m_kernelUniform = m_ssaoConfig.createUniform< UniformType::eVec3f >( cuT( "c3d_kernel" ), m_config.m_kernelSize.range().max() );
		m_ssaoConfig.createUniform< UniformType::eInt >( cuT( "c3d_kernelSize" ) )->setValue( m_config.m_kernelSize.value() );
		m_ssaoConfig.createUniform< UniformType::eFloat >( cuT( "c3d_radius" ) )->setValue( m_config.m_radius );
		m_ssaoConfig.createUniform< UniformType::eFloat >( cuT( "c3d_bias" ) )->setValue( m_config.m_bias );
		m_ssaoConfig.createUniform< UniformType::eVec2f >( cuT( "c3d_noiseScale" ) )->setValue( Point2f( wScale, hScale ) );
		m_ssaoConfig.createUniform< UniformType::eVec2f >( cuT( "c3d_ssaoSize" ) )->setValue( Point2f{ m_size.getWidth(), m_size.getHeight() } );
		m_kernelUniform->setValues( m_ssaoKernel );

		auto sampler = doCreateSampler( m_engine, cuT( "SSAO_Result" ), WrapMode::eClampToEdge );
		auto ssaoResult = renderSystem.createTexture( TextureType::eTwoDimensions
			, AccessType::eNone
			, AccessType::eRead | AccessType::eWrite );
		ssaoResult->setSource( PxBufferBase::create( m_size
			, PixelFormat::eL32F ) );
		m_ssaoResult.setTexture( ssaoResult );
		m_ssaoResult.setSampler( sampler );
		m_ssaoResult.setIndex( MinTextureIndex );
		m_ssaoResult.initialise();

		m_ssaoFbo = renderSystem.createFrameBuffer();
		m_ssaoFbo->initialise();

		m_ssaoResultAttach = m_ssaoFbo->createAttachment( ssaoResult );
		m_ssaoFbo->bind();
		m_ssaoFbo->attach( AttachmentPoint::eColour, 0u, m_ssaoResultAttach, ssaoResult->getType() );
		m_ssaoFbo->setDrawBuffer( m_ssaoResultAttach );
		ENSURE( m_ssaoFbo->isComplete() );
		m_ssaoFbo->unbind();

		m_ssaoPipeline = doCreatePipeline( m_engine, *m_ssaoProgram );
		m_ssaoPipeline->addUniformBuffer( m_matrixUbo.getUbo() );
		m_ssaoPipeline->addUniformBuffer( m_ssaoConfig );
		m_ssaoPipeline->addUniformBuffer( m_gpInfoUbo.getUbo() );

		m_ssaoVertexBuffer = doCreateVbo( m_engine );
		m_ssaoGeometryBuffers = renderSystem.createGeometryBuffers( Topology::eTriangles
			, *m_ssaoProgram );
		m_ssaoGeometryBuffers->initialise( { *m_ssaoVertexBuffer }
			, nullptr );
		m_matrixUbo.update( m_viewport.getProjection() );
	}

	void SsaoPass::doInitialiseBlurPass()
	{
		auto & renderSystem = *m_engine.getRenderSystem();
		m_blurConfig.createUniform< UniformType::eVec2f >( cuT( "c3d_texelSize" ) )->setValue( Point2f{ 1.0 / m_size.getWidth(), 1.0 / m_size.getHeight() } );
		m_blurConfig.createUniform< UniformType::eInt >( cuT( "c3d_noiseSize" ) )->setValue( 4 );
		auto sampler = doCreateSampler( m_engine, cuT( "SSAO_Blurred" ), WrapMode::eClampToEdge );
		auto blurResult = renderSystem.createTexture( TextureType::eTwoDimensions
			, AccessType::eNone
			, AccessType::eRead | AccessType::eWrite );
		blurResult->setSource( PxBufferBase::create( m_size
			, PixelFormat::eL32F ) );
		m_blurResult.setTexture( blurResult );
		m_blurResult.setSampler( sampler );
		m_blurResult.initialise();

		m_blurFbo = renderSystem.createFrameBuffer();
		m_blurFbo->initialise();

		m_blurResultAttach = m_blurFbo->createAttachment( blurResult );
		m_blurFbo->bind();
		m_blurFbo->attach( AttachmentPoint::eColour, 0u, m_blurResultAttach, blurResult->getType() );
		m_blurFbo->setDrawBuffer( m_blurResultAttach );
		ENSURE( m_blurFbo->isComplete() );
		m_blurFbo->unbind();

		m_blurPipeline = doCreatePipeline( m_engine, *m_blurProgram );
		m_blurPipeline->addUniformBuffer( m_matrixUbo.getUbo() );
		m_blurPipeline->addUniformBuffer( m_blurConfig );

		m_blurVertexBuffer = doCreateVbo( m_engine );
		m_blurGeometryBuffers = renderSystem.createGeometryBuffers( Topology::eTriangles
			, *m_blurProgram );
		m_blurGeometryBuffers->initialise( { *m_blurVertexBuffer }
			, nullptr );
	}

	void SsaoPass::doCleanupQuadRendering()
	{
		m_viewport.cleanup();
		m_matrixUbo.getUbo().cleanup();
	}

	void SsaoPass::doCleanupSsaoPass()
	{
		m_ssaoGeometryBuffers->cleanup();
		m_ssaoGeometryBuffers.reset();
		m_ssaoVertexBuffer->cleanup();
		m_ssaoVertexBuffer.reset();
		m_ssaoPipeline->cleanup();
		m_ssaoPipeline.reset();
		m_ssaoConfig.cleanup();
		m_ssaoProgram.reset();
		m_ssaoNoise.cleanup();
		m_ssaoFbo->bind();
		m_ssaoFbo->detachAll();
		m_ssaoFbo->unbind();
		m_ssaoFbo->cleanup();
		m_ssaoFbo.reset();
		m_ssaoResultAttach.reset();
		m_ssaoResult.cleanup();
	}

	void SsaoPass::doCleanupBlurPass()
	{
		m_blurGeometryBuffers->cleanup();
		m_blurGeometryBuffers.reset();
		m_blurVertexBuffer->cleanup();
		m_blurVertexBuffer.reset();
		m_blurPipeline->cleanup();
		m_blurPipeline.reset();
		m_blurProgram.reset();
		m_blurFbo->bind();
		m_blurConfig.cleanup();
		m_blurFbo->detachAll();
		m_blurFbo->unbind();
		m_blurFbo->cleanup();
		m_blurFbo.reset();
		m_blurResultAttach.reset();
		m_blurResult.cleanup();
	}

	void SsaoPass::doRenderSsao( GeometryPassResult const & gp )
	{
		m_ssaoTimer->start();
		m_ssaoConfig.bindTo( 8u );
		m_ssaoFbo->bind( FrameBufferTarget::eDraw );
		m_ssaoFbo->clear( BufferComponent::eColour );
		auto index = MinTextureIndex;
		gp[size_t( DsTexture::eDepth )]->getTexture()->bind( index );
		gp[size_t( DsTexture::eDepth )]->getTexture()->generateMipmaps();
		gp[size_t( DsTexture::eDepth )]->getSampler()->bind( index++ );
		gp[size_t( DsTexture::eData1 )]->getTexture()->bind( index );
		gp[size_t( DsTexture::eData1 )]->getSampler()->bind( index++ );
		m_ssaoNoise.bind();
		m_ssaoPipeline->apply();
		m_ssaoGeometryBuffers->draw( 6u, 0u );
		m_ssaoNoise.unbind();
		gp[size_t( DsTexture::eData1 )]->getTexture()->unbind( index );
		gp[size_t( DsTexture::eData1 )]->getSampler()->unbind( index-- );
		gp[size_t( DsTexture::eDepth )]->getTexture()->unbind( index );
		gp[size_t( DsTexture::eDepth )]->getSampler()->unbind( index-- );
		m_ssaoFbo->unbind();
		m_ssaoTimer->stop();
	}

	void SsaoPass::doRenderBlur()
	{
		m_blurTimer->start();
		m_blurConfig.bindTo( 2u );
		m_viewport.apply();
		m_blurFbo->bind( FrameBufferTarget::eDraw );
		m_blurFbo->clear( BufferComponent::eColour );
		m_ssaoResult.bind();
		m_blurPipeline->apply();
		m_blurGeometryBuffers->draw( 6u, 0u );
		m_ssaoResult.unbind();
		m_blurFbo->unbind();
		m_blurTimer->stop();
	}
}
