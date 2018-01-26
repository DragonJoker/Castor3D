#include "SubsurfaceScatteringPass.hpp"

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
#include "Shader/Shaders/GlslMaterial.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Shader/UniformBuffer.hpp"
#include "Shader/Shaders/GlslLight.hpp"
#include "Shader/Shaders/GlslShadow.hpp"
#include "Shader/Shaders/GlslSssTransmittance.hpp"
#include "Shader/Ubos/GpInfoUbo.hpp"
#include "Shader/Ubos/SceneUbo.hpp"
#include "Shader/Uniform/Uniform.hpp"
#include "State/BlendState.hpp"
#include "State/DepthStencilState.hpp"
#include "State/MultisampleState.hpp"
#include "State/RasteriserState.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"
#include "Texture/TextureUnit.hpp"

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

#include <random>

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	namespace
	{
		VertexBufferSPtr doCreateVbo( Engine & engine )
		{
			auto declaration = ParticleDeclaration(
			{
				ParticleElementDeclaration( cuT( "position" ), uint32_t( ElementUsage::ePosition ), renderer::AttributeFormat::eVec2 ),
				ParticleElementDeclaration{ cuT( "texcoord" ), uint32_t( ElementUsage::eTexCoords ), renderer::AttributeFormat::eVec2 },
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

			auto & renderSystem = *engine.getRenderSystem();
			auto vertexBuffer = std::make_shared< VertexBuffer >( engine, declaration );
			uint32_t stride = declaration.stride();
			vertexBuffer->resize( uint32_t( sizeof( data ) ) );
			uint8_t * buffer = vertexBuffer->getData();
			std::memcpy( buffer, data, sizeof( data ) );
			vertexBuffer->initialise( renderer::MemoryPropertyFlag::eHostVisible );
			return vertexBuffer;
		}

		GeometryBuffersSPtr doCreateVao( Engine & engine
			, ShaderProgram & program
			, VertexBuffer & vbo )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto result = renderSystem.createGeometryBuffers( Topology::eTriangles
				, program );
			result->initialise( { vbo }, nullptr );
			return result;
		}

		glsl::Shader doGetVertexProgram( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			UBO_MATRIX( writer, 0u );
			auto position = writer.declAttribute< Vec2 >( cuT( "position" ) );
			auto texture = writer.declAttribute< Vec2 >( cuT( "texcoord" ) );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ) );
			auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
					vtx_texture = texture;
					gl_Position = c3d_projection * vec4( position, 0.0, 1.0 );
				} );
			return writer.finalise();
		}

		glsl::Shader doGetBlurProgram( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			UBO_SCENE( writer, 0u );
			UBO_GPINFO( writer, 0u );
			Ubo config{ writer, SubsurfaceScatteringPass::Config, 2u, 0u };
			auto c3d_step = config.declMember< Vec2 >( SubsurfaceScatteringPass::Step );
			auto c3d_correction = config.declMember< Float >( SubsurfaceScatteringPass::Correction );
			auto c3d_pixelSize = config.declMember< Float >( SubsurfaceScatteringPass::PixelSize );
			config.end();
			// Gaussian weights for the six samples around the current pixel:
			//   -3 -2 -1 +1 +2 +3
			auto w = writer.declUniform( cuT( "w" )
				, 6u
				, std::vector< Float >{ { 0.006_f, 0.061_f, 0.242_f, 0.242_f, 0.061_f, 0.006_f } } );
			auto o = writer.declUniform( cuT( "o" )
				, 6u
				, std::vector< Float >{ { -1.0, -0.6667, -0.3333, 0.3333, 0.6667, 1.0 } } );
			auto index = MinTextureIndex;
			auto c3d_mapDepth = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eDepth ), index++, 0u );
			auto c3d_mapData4 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData4 ), index++, 0u );
			auto c3d_mapData5 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData5 ), index++, 0u );
			auto c3d_mapLightDiffuse = writer.declSampler< Sampler2D >( cuT( "c3d_mapLightDiffuse" ), index++, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ) );

			auto materials = shader::createMaterials( writer
				, engine.getMaterialsType() == MaterialType::ePbrMetallicRoughness
					? PassFlag::ePbrMetallicRoughness
					: engine.getMaterialsType() == MaterialType::ePbrSpecularGlossiness
						? PassFlag::ePbrSpecularGlossiness
						: PassFlag( 0u ) );
			materials->declare();

			glsl::Utils utils{ writer };
			utils.declareCalcVSPosition();

			// Shader outputs
			auto pxl_fragColor = writer.declFragData< Vec4 >( cuT( "pxl_fragColor" ), 0 );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				auto data4 = writer.declLocale( cuT( "data4" )
					, texture( c3d_mapData4, vtx_texture ) );
				auto data5 = writer.declLocale( cuT( "data5" )
					, texture( c3d_mapData5, vtx_texture ) );
				auto materialId = writer.declLocale( cuT( "materialId" )
					, writer.cast< Int >( data5.z() ) );
				auto translucency = writer.declLocale( cuT( "translucency" )
					, data4.w() );
				auto material = materials->getBaseMaterial( materialId );

				IF( writer, material->m_subsurfaceScatteringEnabled() == 0_i )
				{
					writer.discard();
				}
				FI;

				// Fetch color and linear depth for current pixel:
				auto colorM = writer.declLocale( cuT( "colorM" )
					, texture( c3d_mapLightDiffuse, vtx_texture ) );
				auto depthM = writer.declLocale( cuT( "depthM" )
					, texture( c3d_mapDepth, vtx_texture ).r() );
				depthM = utils.calcVSPosition( vtx_texture
					, depthM
					, c3d_mtxInvProj ).z();

				// Accumulate center sample, multiplying it with its gaussian weight:
				pxl_fragColor = colorM;
				pxl_fragColor.rgb() *= 0.382_f;

				// Calculate the step that we will use to fetch the surrounding pixels,
				// where "step" is:
				//     step = sssStrength * gaussianWidth * pixelSize * dir
				// The closer the pixel, the stronger the effect needs to be, hence
				// the factor 1.0 / depthM.
				auto finalStep = writer.declLocale( cuT( "finalStep" )
					, translucency * c3d_step * c3d_pixelSize * material->m_subsurfaceScatteringStrength() * material->m_gaussianWidth() / depthM );

				auto offset = writer.declLocale< Vec2 >( cuT( "offset" ) );
				auto color = writer.declLocale< Vec3 >( cuT( "color" ) );
				auto depth = writer.declLocale< Float >( cuT( "depth" ) );
				auto s = writer.declLocale< Float >( cuT( "s" ) );

				// Accumulate the other samples:
				for ( int i = 0; i < 6; i++ )
				{
					// Fetch color and depth for current sample:
					offset = glsl::fma( vec2( o[i] ), finalStep, vtx_texture );
					color = texture( c3d_mapLightDiffuse, offset, 0.0_f ).rgb();
					depth = texture( c3d_mapDepth, offset, 0.0_f ).r();
					depth = utils.calcVSPosition( vtx_texture
						, depth
						, c3d_mtxInvProj ).z();

					// If the difference in depth is huge, we lerp color back to "colorM":
					s = min( 0.0125_f * c3d_correction * abs( depthM - depth ), 1.0_f );
					color = mix( color, colorM.rgb(), s );

					// Accumulate:
					pxl_fragColor.rgb() += w[i] * color;
				}
			} );
			return writer.finalise();
		}

		glsl::Shader doGetCombineProgram( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ) );
			auto index = MinTextureIndex;
			auto c3d_mapData4 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData4 ), index++, 0u );
			auto c3d_mapData5 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData5 ), index++, 0u );
			auto c3d_mapLightDiffuse = writer.declSampler< Sampler2D >( cuT( "c3d_mapLightDiffuse" ), index++, 0u );
			auto c3d_mapBlur1 = writer.declSampler< Sampler2D >( cuT( "c3d_mapBlur1" ), index++, 0u );
			auto c3d_mapBlur2 = writer.declSampler< Sampler2D >( cuT( "c3d_mapBlur2" ), index++, 0u );
			auto c3d_mapBlur3 = writer.declSampler< Sampler2D >( cuT( "c3d_mapBlur3" ), index++, 0u );
			auto c3d_originalWeight = writer.declUniform< Vec4 >( cuT( "c3d_originalWeight" )
				, vec4( 0.2406_f, 0.4475_f, 0.6159_f, 0.25_f ) );
			auto c3d_blurWeights = writer.declUniform< Vec4 >( cuT( "c3d_blurWeights" )
				, 3u
				, {
					vec4( 0.1158_f, 0.3661_f, 0.3439_f, 0.25_f ),
					vec4( 0.1836_f, 0.1864_f, 0.0_f, 0.25_f ),
					vec4( 0.46_f, 0.0_f, 0.0402_f, 0.25_f )
				} );
			auto c3d_blurVariances = writer.declUniform< Float >( cuT( "c3d_blurVariances" )
				, 3u
				, {
					0.0516_f,
					0.2719_f,
					2.0062_f
				} );

			auto materials = shader::createMaterials( writer
				, engine.getMaterialsType() == MaterialType::ePbrMetallicRoughness
					? PassFlag::ePbrMetallicRoughness
					: engine.getMaterialsType() == MaterialType::ePbrSpecularGlossiness
						? PassFlag::ePbrSpecularGlossiness
						: PassFlag( 0u ) );
			materials->declare();

			// Shader outputs
			auto pxl_fragColor = writer.declFragData< Vec4 >( cuT( "pxl_fragColor" ), 0 );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				auto data4 = writer.declLocale( cuT( "data4" )
					, texture( c3d_mapData4, vtx_texture ) );
				auto data5 = writer.declLocale( cuT( "data5" )
					, texture( c3d_mapData5, vtx_texture ) );
				auto original = writer.declLocale( cuT( "original" )
					, texture( c3d_mapLightDiffuse, vtx_texture ) );
				auto blur1 = writer.declLocale( cuT( "blur1" )
					, texture( c3d_mapBlur1, vtx_texture ) );
				auto blur2 = writer.declLocale( cuT( "blur2" )
					, texture( c3d_mapBlur2, vtx_texture ) );
				auto blur3 = writer.declLocale( cuT( "blur3" )
					, texture( c3d_mapBlur3, vtx_texture ) );
				auto materialId = writer.declLocale( cuT( "materialId" )
					, writer.cast< Int >( data5.z() ) );
				auto translucency = writer.declLocale( cuT( "translucency" )
					, data4.w() );
				auto material = materials->getBaseMaterial( materialId );

#if !C3D_DEBUG_SSS_TRANSMITTANCE
				IF( writer, material->m_subsurfaceScatteringEnabled() == 0_i )
				{
					pxl_fragColor = original;
				}
				ELSE
				{
					pxl_fragColor = original * c3d_originalWeight
						+ blur1 * c3d_blurWeights[0]
						+ blur2 * c3d_blurWeights[1]
						+ blur3 * c3d_blurWeights[2];
				}
				FI;
#else
				pxl_fragColor = original;
#endif
			} );
			return writer.finalise();
		}

		ShaderProgramSPtr doCreateBlurProgram( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto & cache = engine.getShaderProgramCache();
			auto const vtx = doGetVertexProgram( engine );
			auto const pxl = doGetBlurProgram( engine );

			ShaderProgramSPtr program = cache.getNewProgram( false );
			program->createObject( ShaderType::eVertex );
			program->createObject( ShaderType::ePixel );
			program->setSource( ShaderType::eVertex, vtx );
			program->setSource( ShaderType::ePixel, pxl );
			program->initialise();
			return program;
		}

		ShaderProgramSPtr doCreateCombineProgram( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto & cache = engine.getShaderProgramCache();
			auto const vtx = doGetVertexProgram( engine );
			auto const pxl = doGetCombineProgram( engine );

			ShaderProgramSPtr program = cache.getNewProgram( false );
			program->createObject( ShaderType::eVertex );
			program->createObject( ShaderType::ePixel );
			program->setSource( ShaderType::eVertex, vtx );
			program->setSource( ShaderType::ePixel, pxl );
			program->initialise();
			return program;
		}

		SamplerSPtr doCreateSampler( Engine & engine )
		{
			SamplerSPtr sampler;
			String const name{ cuT( "SubsurfaceScatteringPass" ) };

			if ( engine.getSamplerCache().has( name ) )
			{
				sampler = engine.getSamplerCache().find( name );
			}
			else
			{
				sampler = engine.getSamplerCache().add( name );
				sampler->setInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eNearest );
				sampler->setInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eNearest );
				sampler->setWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
				sampler->setWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
			}

			return sampler;
		}

		TextureUnit doCreateTexture( Engine & engine
			, Size const & size )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto sampler = doCreateSampler( engine );
			auto texture = renderSystem.createTexture( TextureType::eTwoDimensions
				, AccessType::eNone
				, AccessType::eRead | AccessType::eWrite
				, PixelFormat::eRGBA32F
				, size );
			texture->getImage().initialiseSource();
			TextureUnit unit{ engine };
			unit.setTexture( texture );
			unit.setSampler( sampler );
			unit.setIndex( MinTextureIndex );
			unit.initialise();
			return unit;
		}

		FrameBufferSPtr doCreateFbo( Engine & engine
			, Size const & size )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto fbo = renderSystem.createFrameBuffer();
			fbo->initialise();
			return fbo;
		}

		RenderPipelineUPtr doCreateBlurPipeline( Engine & engine
			, ShaderProgram & program
			, MatrixUbo & matrixUbo
			, GpInfoUbo & gpInfoUbo
			, SceneUbo & sceneUbo
			, UniformBuffer & blurUbo )
		{
			DepthStencilState dsstate;
			dsstate.setDepthTest( false );
			dsstate.setDepthMask( WritingMask::eZero );
			RasteriserState rsstate;
			rsstate.setCulledFaces( Culling::eNone );
			BlendState blState;
			auto pipeline = engine.getRenderSystem()->createRenderPipeline( std::move( dsstate )
				, std::move( rsstate )
				, std::move( blState )
				, MultisampleState{}
				, program
				, PipelineFlags{} );
			pipeline->addUniformBuffer( matrixUbo.getUbo() );
			pipeline->addUniformBuffer( gpInfoUbo.getUbo() );
			pipeline->addUniformBuffer( sceneUbo.getUbo() );
			pipeline->addUniformBuffer( blurUbo );
			return pipeline;
		}

		RenderPipelineUPtr doCreateCombinePipeline( Engine & engine
			, ShaderProgram & program
			, MatrixUbo & matrixUbo )
		{
			DepthStencilState dsstate;
			dsstate.setDepthTest( false );
			dsstate.setDepthMask( WritingMask::eZero );
			RasteriserState rsstate;
			rsstate.setCulledFaces( Culling::eNone );
			BlendState blState;
			auto pipeline = engine.getRenderSystem()->createRenderPipeline( std::move( dsstate )
				, std::move( rsstate )
				, std::move( blState )
				, MultisampleState{}
				, program
				, PipelineFlags{} );
			pipeline->addUniformBuffer( matrixUbo.getUbo() );
			return pipeline;
		}
	}

	//*********************************************************************************************

	String const SubsurfaceScatteringPass::Config = cuT( "Config" );
	String const SubsurfaceScatteringPass::Step = cuT( "c3d_step" );
	String const SubsurfaceScatteringPass::Correction = cuT( "c3d_correction" );
	String const SubsurfaceScatteringPass::PixelSize = cuT( "c3d_pixelSize" );

	SubsurfaceScatteringPass::SubsurfaceScatteringPass( Engine & engine
		, GpInfoUbo & gpInfoUbo
		, SceneUbo & sceneUbo
		, Size const & textureSize )
		: OwnedBy< Engine >{ engine }
		, m_size{ textureSize }
		, m_viewport{ engine }
		, m_matrixUbo{ engine }
		, m_blurUbo{ SubsurfaceScatteringPass::Config
			, *engine.getRenderSystem()
			, 2u }
		, m_blurStep{ m_blurUbo.createUniform< UniformType::eVec2f >( SubsurfaceScatteringPass::Step ) }
		, m_blurCorrection{ m_blurUbo.createUniform< UniformType::eFloat >( SubsurfaceScatteringPass::Correction ) }
		, m_blurPixelSize{ m_blurUbo.createUniform< UniformType::eFloat >( SubsurfaceScatteringPass::PixelSize ) }
		, m_intermediate{ doCreateTexture( engine
			, textureSize ) }
		, m_blurResults
		{
			{
				doCreateTexture( engine, textureSize ),
				doCreateTexture( engine, textureSize ),
				doCreateTexture( engine, textureSize ),
			}
		}
		, m_result{ doCreateTexture( engine
			, textureSize ) }
		, m_fbo{ doCreateFbo( engine
			, textureSize ) }
		, m_vertexBuffer{ doCreateVbo( engine ) }
		, m_blurProgram{ doCreateBlurProgram( engine ) }
		, m_blurGeometryBuffers{ doCreateVao( engine
			, *m_blurProgram
			, *m_vertexBuffer ) }
		, m_blurPipeline{ doCreateBlurPipeline( engine
			, *m_blurProgram
			, m_matrixUbo
			, gpInfoUbo
			, sceneUbo
			, m_blurUbo ) }
		, m_combineProgram{ doCreateCombineProgram( engine ) }
		, m_combineGeometryBuffers{ doCreateVao( engine
			, *m_combineProgram
			, *m_vertexBuffer ) }
		, m_combinePipeline{ doCreateCombinePipeline( engine
			, *m_combineProgram
			, m_matrixUbo ) }
	{
		m_viewport.setOrtho( 0, 1, 0, 1, 0, 1 );
		m_viewport.initialise();
		m_viewport.resize( m_size );
		m_viewport.update();
		m_matrixUbo.update( m_viewport.getProjection() );
		m_blurCorrection->setValue( 1.0f );
		m_fbo->setClearColour( RgbaColour::fromPredefined( PredefinedRgbaColour::eOpaqueBlack ) );

		for ( size_t i{ 0u }; i < m_blurResultsAttaches.size(); ++i )
		{
			m_blurResultsAttaches[i] = m_fbo->createAttachment( m_blurResults[i].getTexture() );
			m_blurResultsAttaches[i]->setLayer( 0u );
			m_blurResultsAttaches[i]->setTarget( TextureType::eTwoDimensions );
		}

		m_intermediateAttach = m_fbo->createAttachment( m_intermediate.getTexture() );
		m_intermediateAttach->setLayer( 0u );
		m_intermediateAttach->setTarget( TextureType::eTwoDimensions );

		m_resultAttach = m_fbo->createAttachment( m_result.getTexture() );
		m_resultAttach->setLayer( 0u );
		m_resultAttach->setTarget( TextureType::eTwoDimensions );
	}

	SubsurfaceScatteringPass::~SubsurfaceScatteringPass()
	{
		m_blurCorrection.reset();
		m_blurPixelSize.reset();
		m_blurStep.reset();

		m_blurPipeline->cleanup();
		m_blurPipeline.reset();
		m_combinePipeline->cleanup();
		m_combinePipeline.reset();

		m_blurGeometryBuffers->cleanup();
		m_blurGeometryBuffers.reset();
		m_combineGeometryBuffers->cleanup();
		m_combineGeometryBuffers.reset();
		m_vertexBuffer->cleanup();
		m_vertexBuffer.reset();

		m_blurProgram->cleanup();
		m_blurProgram.reset();
		m_combineProgram->cleanup();
		m_combineProgram.reset();

		m_blurUbo.cleanup();
		m_matrixUbo.getUbo().cleanup();
		m_fbo->cleanup();
		m_fbo.reset();
		m_result.cleanup();
		m_intermediate.cleanup();

		for ( auto & result : m_blurResults )
		{
			result.cleanup();
		}
	}

	void SubsurfaceScatteringPass::render( GeometryPassResult const & gp
		, TextureUnit const & lightDiffuse )const
	{
		static const Point2f horiz{ 1.0f, 0.0f };
		static const Point2f vertic{ 0.0f, 1.0f };
		m_blurUbo.bindTo( 2u );
		TextureUnit const * prv = &lightDiffuse;

		for ( size_t i{ 0u }; i < m_blurResults.size(); ++i )
		{
			doBlur( gp
				, *prv
				, m_intermediate
				, m_intermediateAttach
				, horiz );
			doBlur( gp
				, m_intermediate
				, m_blurResults[i]
				, m_blurResultsAttaches[i]
				, vertic );
			prv = &m_blurResults[i];
		}

		doCombine( gp
			, lightDiffuse );
	}

	void SubsurfaceScatteringPass::debugDisplay( castor::Size const & size )const
	{
		auto count = 9u;
		int width = int( m_size.getWidth() ) / count;
		int height = int( m_size.getHeight() ) / count;
		int top = int( m_size.getHeight() ) - height;
		auto renderSize = Size( width, height );
		auto & context = *getEngine()->getRenderSystem()->getCurrentContext();
		auto index = 0;
		context.renderTexture( Position{ width * index++, top }
			, renderSize
			, *m_blurResults[0].getTexture() );
		context.renderTexture( Position{ width * index++, top }
			, renderSize
			, *m_blurResults[1].getTexture() );
		context.renderTexture( Position{ width * index++, top }
			, renderSize
			, *m_blurResults[2].getTexture() );
		context.renderTexture( Position{ width * index++, top }
			, renderSize
			, *m_result.getTexture() );
	}

	void SubsurfaceScatteringPass::doBlur( GeometryPassResult const & gp
		, TextureUnit const & source
		, TextureUnit const & destination
		, TextureAttachmentSPtr attach
		, castor::Point2r const & direction )const
	{
		uint32_t index{ MinTextureIndex };
		gp[size_t( DsTexture::eDepth )]->getTexture()->bind( index );
		gp[size_t( DsTexture::eDepth )]->getSampler()->bind( index );
		++index;
		gp[size_t( DsTexture::eData4 )]->getTexture()->bind( index );
		gp[size_t( DsTexture::eData4 )]->getSampler()->bind( index );
		++index;
		gp[size_t( DsTexture::eData5 )]->getTexture()->bind( index );
		gp[size_t( DsTexture::eData5 )]->getSampler()->bind( index );
		++index;

		m_blurStep->setValue( direction );
		auto size = 1.0f / ( m_size.getWidth() * direction[0] + m_size.getHeight() * direction[1] );
		m_blurPixelSize->setValue( size );
		m_blurUbo.update();

		m_viewport.apply();
		m_fbo->bind( FrameBufferTarget::eDraw );
		attach->attach( AttachmentPoint::eColour, 0u );
		REQUIRE( m_fbo->isComplete() );
		m_fbo->setDrawBuffer( attach );
		m_fbo->clear( BufferComponent::eColour );

		source.getTexture()->bind( index );
		source.getSampler()->bind( index );
		m_blurPipeline->apply();
		m_blurGeometryBuffers->draw( 6u, 0u );
		source.getTexture()->unbind( index );
		source.getSampler()->unbind( index );
		m_fbo->unbind();

		--index;
		gp[size_t( DsTexture::eData5 )]->getTexture()->unbind( index );
		gp[size_t( DsTexture::eData5 )]->getSampler()->unbind( index );
		--index;
		gp[size_t( DsTexture::eData4 )]->getTexture()->unbind( index );
		gp[size_t( DsTexture::eData4 )]->getSampler()->unbind( index );
		--index;
		gp[size_t( DsTexture::eDepth )]->getTexture()->unbind( index );
		gp[size_t( DsTexture::eDepth )]->getSampler()->unbind( index );
	}

	void SubsurfaceScatteringPass::doCombine( GeometryPassResult const & gp
		, TextureUnit const & source )const
	{
		uint32_t index{ MinTextureIndex };
		gp[size_t( DsTexture::eData4 )]->getTexture()->bind( index );
		gp[size_t( DsTexture::eData4 )]->getSampler()->bind( index );
		++index;
		gp[size_t( DsTexture::eData5 )]->getTexture()->bind( index );
		gp[size_t( DsTexture::eData5 )]->getSampler()->bind( index );
		++index;
		source.getTexture()->bind( index );
		source.getSampler()->bind( index );
		++index;

		for ( auto & result : m_blurResults )
		{
			result.getTexture()->bind( index );
			result.getSampler()->bind( index );
			++index;
		}

		m_fbo->bind( FrameBufferTarget::eDraw );
		m_resultAttach->attach( AttachmentPoint::eColour, 0u );
		REQUIRE( m_fbo->isComplete() );
		m_fbo->setDrawBuffer( m_resultAttach );
		m_fbo->clear( BufferComponent::eColour );

		m_combinePipeline->apply();
		m_combineGeometryBuffers->draw( 6u, 0u );
		m_fbo->unbind();
		--index;
		source.getTexture()->unbind( index );
		source.getSampler()->unbind( index );

		for ( auto & result : m_blurResults )
		{
			--index;
			result.getTexture()->unbind( index );
			result.getSampler()->unbind( index );
		}

		--index;
		gp[size_t( DsTexture::eData5 )]->getTexture()->unbind( index );
		gp[size_t( DsTexture::eData5 )]->getSampler()->unbind( index );
		--index;
		gp[size_t( DsTexture::eData4 )]->getTexture()->unbind( index );
		gp[size_t( DsTexture::eData4 )]->getSampler()->unbind( index );
	}
}
