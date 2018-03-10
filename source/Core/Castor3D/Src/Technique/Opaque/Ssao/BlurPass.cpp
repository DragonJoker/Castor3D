#include "BlurPass.hpp"

#include "Engine.hpp"
#include "Render/RenderPassTimer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Shader/Ubos/MatrixUbo.hpp"
#include "Technique/Opaque/Ssao/SsaoConfig.hpp"
#include "Technique/Opaque/Ssao/SsaoConfigUbo.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"
#include "Texture/TextureUnit.hpp"

#include <Image/Texture.hpp>
#include <Image/TextureView.hpp>
#include <Pipeline/ColourBlendState.hpp>
#include <Pipeline/DepthStencilState.hpp>
#include <Pipeline/MultisampleState.hpp>
#include <Pipeline/RasterisationState.hpp>
#include <RenderPass/FrameBuffer.hpp>
#include <RenderPass/FrameBufferAttachment.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/RenderPassAttachment.hpp>
#include <RenderPass/RenderPassState.hpp>
#include <RenderPass/RenderSubpass.hpp>
#include <RenderPass/RenderSubpassState.hpp>

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

#include "Shader/Shaders/GlslLight.hpp"
#include "Shader/Shaders/GlslShadow.hpp"

using namespace castor;

namespace castor3d
{
	namespace
	{
		static PixelFormat constexpr ColourFormat = PixelFormat::eA8R8G8B8;

		glsl::Shader doGetVertexProgram( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			auto position = writer.declAttribute< Vec2 >( cuT( "position" ), 0u );

			// Shader outputs
			auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
					gl_Position = vec4( position, 0.0, 1.0 );
				} );
			return writer.finalise();
		}
		
		glsl::Shader doGetPixelProgram( Engine & engine
			, SsaoConfig const & config )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			UBO_SSAO_CONFIG( writer, 0u, 0u );
			auto c3d_mapNormal = writer.declSampler< Sampler2D >( cuT( "c3d_mapNormal" ), 1u, 0u, config.m_useNormalsBuffer );
			auto c3d_mapInput = writer.declSampler< Sampler2D >( cuT( "c3d_mapInput" ), 2u, 0u );

			/** (1, 0) or (0, 1)*/
			auto c3d_axis = writer.declUniform< IVec2 >( cuT( "c3d_axis" ), 3u );

			/** Same size as result buffer, do not offset by guard band when reading from it */
			auto c3d_readMultiplyFirst = writer.declUniform( cuT( "c3d_readMultiplyFirst" ), 4u, config.m_useNormalsBuffer, vec4( 2.0_f ) );
			auto c3d_readAddSecond = writer.declUniform( cuT( "c3d_readAddSecond" ), 5u, config.m_useNormalsBuffer, vec4( 1.0_f ) );

			auto gl_FragCoord = writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) );

			REQUIRE( config.m_blurRadius > 0 && config.m_blurRadius < 7 );
			auto gaussian = writer.declUniformArray( cuT( "gaussian" ), 4u, config.m_blurRadius + 1u
				, ( config.m_blurRadius == 1u
					? std::vector< Float >{ { 0.5_f, 0.25_f } }
					: ( config.m_blurRadius == 2u
						? std::vector< Float >{ { 0.153170_f, 0.144893_f, 0.122649_f } }
						: ( config.m_blurRadius == 3u
							? std::vector< Float >{ { 0.153170_f, 0.144893_f, 0.122649_f, 0.092902_f } }
							: ( config.m_blurRadius == 4u
								? std::vector< Float >{ { 0.153170_f, 0.144893_f, 0.122649_f, 0.092902_f, 0.062970_f } }
								: ( config.m_blurRadius == 5u
									? std::vector< Float >{ { 0.111220_f, 0.107798_f, 0.098151_f, 0.083953_f, 0.067458_f, 0.050920_f } }
									: std::vector< Float >{ { 0.111220_f, 0.107798_f, 0.098151_f, 0.083953_f, 0.067458_f, 0.050920_f, 0.036108_f } } ) ) ) ) ) );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec3 >( cuT( "pxl_fragColor" ), 0u );
#define  result         pxl_fragColor.r()
#define  keyPassThrough pxl_fragColor.g()

			/** Returns a number on (0, 1) */
			auto unpackKey = writer.implementFunction< Float >( cuT( "unpackKey" )
				, [&]( Float const & p )
				{
					writer.returnStmt( p );
				}
				, InFloat{ &writer, cuT( "p" ) } );

			// Reconstruct camera-space P.xyz from screen-space S = (x, y) in
			// pixels and camera-space z < 0.  Assumes that the upper-left pixel center
			// is at (0.5, 0.5) [but that need not be the location at which the sample tap
			// was placed!]
			// Costs 3 MADD.  Error is on the order of 10^3 at the far plane, partly due to z precision.
			auto reconstructCSPosition = writer.implementFunction< Vec3 >( cuT( "reconstructCSPosition" )
				, [&]( Vec2 const & S
					, Float const & z
					, Vec4 const & projInfo )
				{
					writer.returnStmt( vec3( glsl::fma( S.xy(), projInfo.xy(), projInfo.zw() ) * z, z ) );
				}
				, InVec2{ &writer, cuT( "S" ) }
				, InFloat{ &writer, cuT( "z" ) }
				, InVec4{ &writer, cuT( "projInfo" ) } );

			auto positionFromKey = writer.implementFunction< Vec3 >( cuT( "positionFromKey" )
				, [&]( Float const & key
					, IVec2 const & ssCenter
					, Vec4 const & projInfo )
				{
					auto z = writer.declLocale( cuT( "z" )
						, key * c3d_farPlaneZ );
					auto position = writer.declLocale( cuT( "position" )
						, reconstructCSPosition( vec2( ssCenter ) + vec2( 0.5_f )
							, z
							, projInfo ) );
					writer.returnStmt( position );
				}
				, InFloat{ &writer, cuT( "key" ) }
				, InIVec2{ &writer, cuT( "ssCenter" ) }
				, InVec4{ &writer, cuT( "projInfo" ) } );


			auto getTapInformation = writer.implementFunction< Void >( cuT( "getTapInformation" )
				, [&]( IVec2 const & tapLoc
					, Float tapKey
					, Float value
					, Vec3 tapNormal )
				{
					auto temp = writer.declLocale( cuT( "temp" )
						, texelFetch( c3d_mapInput, tapLoc, 0 ) );
					tapKey = unpackKey( temp.g() );
					value = temp.r();

					if ( config.m_useNormalsBuffer )
					{
						tapNormal = texelFetch( c3d_mapNormal, tapLoc, 0 ).xyz();
						tapNormal = normalize( tapNormal * c3d_readMultiplyFirst.xyz() + c3d_readAddSecond.xyz() );
					}
					else
					{
						tapNormal = vec3( 0.0_f );
					}
				}
				, InIVec2{ &writer, cuT( "tapLoc" ) }
				, OutFloat{ &writer, cuT( "tapKey" ) }
				, OutFloat{ &writer, cuT( "value" ) }
				, OutVec3{ &writer, cuT( "tapNormal" ) } );

			auto square = writer.implementFunction< Float >( cuT( "square" )
				, [&]( Float const & x )
				{
					writer.returnStmt( x * x );
				}
				, InFloat{ &writer, cuT( "x") } );

			auto calculateBilateralWeight = writer.implementFunction< Float >( cuT( "calculateBilateralWeight" )
				, [&]( Float const & key
					, Float const & tapKey
					, IVec2 const & tapLoc
					, Vec3 const & normal
					, Vec3 const & tapNormal
					, Vec3 const & position )
				{
					auto scale = writer.declLocale( cuT( "scale" )
						, 1.5_f * c3d_invRadius );

					// The "bilateral" weight. As depth difference increases, decrease weight.
					// The key values are in scene-specific scale. To make them scale-invariant, factor in
					// the AO radius, which should be based on the scene scale.
					auto depthWeight = writer.declLocale( cuT( "depthWeight" )
						, max( 0.0_f, 1.0_f - writer.paren( c3d_edgeSharpness * 2000.0 ) * abs( tapKey - key ) * scale ) );
					auto k_normal = writer.declLocale( cuT( "k_normal" )
						, 1.0_f );
					auto k_plane = writer.declLocale( cuT( "k_plane" )
						, 1.0_f );

					// Prevents blending over creases. 
					auto normalWeight = writer.declLocale( cuT( "normalWeight" )
						, 1.0_f );
					auto planeWeight = writer.declLocale( cuT( "planeWeight" )
						, 1.0_f );

					if ( config.m_useNormalsBuffer )
					{
						auto normalCloseness = writer.declLocale( cuT( "normalCloseness" )
							, dot( tapNormal, normal ) );

						if ( !config.m_blurHighQuality )
						{
							normalCloseness = normalCloseness * normalCloseness;
							normalCloseness = normalCloseness * normalCloseness;
							k_normal = 4.0_f;
						}

						auto normalError = writer.declLocale( cuT( "normalError" )
							, writer.paren( 1.0_f - normalCloseness ) * k_normal );
						normalWeight = max( writer.paren( 1.0_f - c3d_edgeSharpness * normalError ), 0.0_f );

						if ( config.m_blurHighQuality )
						{
							auto lowDistanceThreshold2 = writer.declLocale( cuT( "lowDistanceThreshold2" )
								, 0.001_f );

							auto tapPosition = writer.declLocale( cuT( "tapPosition" )
								, positionFromKey( tapKey, tapLoc, c3d_projInfo ) );

							// Change position in camera space
							auto dq = writer.declLocale( cuT( "dq" )
								, position - tapPosition );

							// How far away is this point from the original sample
							// in camera space? (Max value is unbounded)
							auto distance2 = writer.declLocale( cuT( "distance2" )
								, dot( dq, dq ) );

							// How far off the expected plane (on the perpendicular) is this point?  Max value is unbounded.
							auto planeError = writer.declLocale( cuT( "planeError" )
								, max( abs( dot( dq, tapNormal ) ), abs( dot( dq, normal ) ) ) );

							// Minimum distance threshold must be scale-invariant, so factor in the radius
							planeWeight = writer.ternary( distance2 * square( scale ) < lowDistanceThreshold2
								, 1.0_f
								, pow( max( 0.0_f
										, 1.0_f - c3d_edgeSharpness * 2.0 * k_plane * planeError / sqrt( distance2 ) )
									, 2.0_f ) );
						}
					}

					writer.returnStmt( depthWeight * normalWeight * planeWeight );
				}
				, InFloat{ &writer, cuT( "key" ) }
				, InFloat{ &writer, cuT( "tapKey" ) }
				, InIVec2{ &writer, cuT( "tapLoc" ) }
				, InVec3{ &writer, cuT( "normal" ) }
				, InVec3{ &writer, cuT( "tapNormal" ) }
				, InVec3{ &writer, cuT( "position" ) } );

			writer.implementFunction< Void >( cuT( "main" )
				, [&]()
				{
					auto ssCenter = writer.declLocale( cuT( "ssCenter" )
						, ivec2( gl_FragCoord.xy() ) );

					auto temp = writer.declLocale( cuT( "temp" )
						, texelFetch( c3d_mapInput, ssCenter, 0 ) );
					auto sum = writer.declLocale( cuT( "sum" )
						, temp.r() );

					keyPassThrough = temp.g();
					auto key = writer.declLocale( cuT( "key" )
						, unpackKey( keyPassThrough ) );
					auto normal = writer.declLocale( cuT( "normal" )
						, vec3( 0.0_f ) );

					if ( config.m_useNormalsBuffer )
					{
						normal = texelFetch( c3d_mapNormal, ssCenter, 0 ).xyz();
						normal = normalize( glsl::fma( normal, c3d_readMultiplyFirst.xyz(), c3d_readAddSecond.xyz() ) );
					}

					IF( writer, key == 1.0_f )
					{
						// Sky pixel (if you aren't using depth keying, disable this test)
						result = sum;
						writer.returnStmt();
					}
					FI;

					// Base weight for depth falloff.  Increase this for more blurriness,
					// decrease it for better edge discrimination
					auto BASE = writer.declLocale( cuT( "BASE" )
						, gaussian[0] );
					auto totalWeight = writer.declLocale( cuT( "totalWeight" )
						, BASE );
					sum *= totalWeight;

					auto position = writer.declLocale( cuT( "position" )
						, positionFromKey( key, ssCenter, c3d_projInfo ) );

					FOR( writer, Int, r, -c3d_blurRadius, "r <= c3d_blurRadius", "++r" )
					{
						// We already handled the zero case above.  This loop should be unrolled and the static branch optimized out,
						// so the IF statement has no runtime cost
						IF( writer, r != 0_i )
						{
							auto tapLoc = writer.declLocale( cuT( "tapLoc" )
								, ssCenter + c3d_axis * writer.paren( r * c3d_blurStepSize ) );

							// spatial domain: offset gaussian tap
							auto weight = writer.declLocale( cuT( "weight" )
								, 0.3_f + gaussian[abs( r )] );

							auto tapKey = writer.declLocale< Float >( cuT( "tapKey" ) );
							auto value = writer.declLocale< Float >( cuT( "value" ) );
							auto tapNormal = writer.declLocale< Vec3 >( cuT( "tapNormal" ) );
							writer << getTapInformation( tapLoc, tapKey, value, tapNormal ) << endi;

							auto bilateralWeight = writer.declLocale( cuT( "bilateralWeight" )
								, calculateBilateralWeight( key
									, tapKey
									, tapLoc
									, normal
									, tapNormal
									, position ) );

							weight *= bilateralWeight;
							sum += value * weight;
							totalWeight += weight;
						}
						FI;
					}
					ROF;

					auto const epsilon = writer.declLocale( cuT( "epsilon" )
						, 0.0001_f );
					result = sum / writer.paren( totalWeight + epsilon );
				} );
			return writer.finalise();

#undef result
#undef keyPassThrough
		}

		renderer::ShaderProgram & doGetProgram( Engine & engine
			, SsaoConfig const & config )
		{
			auto vtx = doGetVertexProgram( engine );
			auto pxl = doGetPixelProgram( engine, config );
			auto & program = engine.getShaderProgramCache().getNewProgram( false );
			program.createModule( vtx.getSource(), renderer::ShaderStageFlag::eVertex );
			program.createModule( pxl.getSource(), renderer::ShaderStageFlag::eFragment );
			return program;
		}

		SamplerSPtr doCreateSampler( Engine & engine
			, String const & name
			, renderer::WrapMode mode )
		{
			SamplerSPtr sampler;

			if ( engine.getSamplerCache().has( name ) )
			{
				sampler = engine.getSamplerCache().find( name );
			}
			else
			{
				sampler = engine.getSamplerCache().add( name );
				sampler->setMinFilter( renderer::Filter::eNearest );
				sampler->setMagFilter( renderer::Filter::eNearest );
				sampler->setWrapS( mode );
				sampler->setWrapT( mode );
			}

			return sampler;
		}

		TextureUnit doCreateTexture( Engine & engine, Size const & size )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto sampler = doCreateSampler( engine, cuT( "SSAOBlur_Result" ), renderer::WrapMode::eClampToEdge );
			auto ssaoResult = std::make_shared< TextureLayout >( renderSystem
				, renderer::TextureType::e2D
				, renderer::ImageUsageFlag::eColourAttachment | renderer::ImageUsageFlag::eSampled
				, renderer::MemoryPropertyFlag::eDeviceLocal
				, ColourFormat
				, size );
			TextureUnit result{ engine };
			result.setTexture( ssaoResult );
			result.setSampler( sampler );
			result.initialise();
			return result;
		}

		renderer::RenderPassPtr doCreateRenderPass( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto & device = *renderSystem.getCurrentDevice();
			std::vector< renderer::PixelFormat > formats
			{
				ColourFormat
			};
			renderer::RenderPassAttachmentArray attaches
			{
				renderer::RenderPassAttachment::createColourAttachment( 0u, ColourFormat, true ),
			};
			renderer::ImageLayoutArray const initialLayouts
			{
				renderer::ImageLayout::eColourAttachmentOptimal,
			};
			renderer::ImageLayoutArray const finalLayouts
			{
				renderer::ImageLayout::eShaderReadOnlyOptimal,
			};
			renderer::RenderSubpassPtrArray subpasses;
			subpasses.emplace_back( device.createRenderSubpass( attaches
				, { renderer::PipelineStageFlag::eColourAttachmentOutput, renderer::AccessFlag::eColourAttachmentWrite } ) );
			return device.createRenderPass( attaches
				, std::move( subpasses )
				, renderer::RenderPassState{ renderer::PipelineStageFlag::eColourAttachmentOutput
					, renderer::AccessFlag::eColourAttachmentWrite
					, initialLayouts }
				, renderer::RenderPassState{ renderer::PipelineStageFlag::eColourAttachmentOutput
					, renderer::AccessFlag::eColourAttachmentWrite
					, finalLayouts } );
		}

		renderer::FrameBufferPtr doCreateFrameBuffer( renderer::RenderPass const & renderPass
			, TextureUnit const & texture )
		{
			renderer::FrameBufferAttachmentArray attaches;
			attaches.emplace_back( *( renderPass.begin() ), texture.getTexture()->getView() );
			auto size = texture.getTexture()->getDimensions();
			return renderPass.createFrameBuffer( renderer::Extent2D{ size.getWidth(), size.getHeight() }
				, std::move( attaches ) );
		}
	}

	//*********************************************************************************************

	SsaoBlurPass::SsaoBlurPass( Engine & engine
		, Size const & size
		, SsaoConfig const & config
		, SsaoConfigUbo & ssaoConfigUbo
		, Point2i const & axis
		, TextureUnit const & input
		, TextureUnit const & normals )
		: RenderQuad{ *engine.getRenderSystem(), true }
		, m_engine{ engine }
		, m_ssaoConfigUbo{ ssaoConfigUbo }
		, m_input{ input }
		, m_normals{ normals }
		, m_program{ doGetProgram( m_engine, config ) }
		, m_size{ size }
		, m_result{ doCreateTexture( m_engine, m_size ) }
		, m_axisUniform{ renderer::ShaderStageFlag::eFragment, { { 3u, 0u, renderer::AttributeFormat::eVec2i } } }
		, m_pushConstantRange{ renderer::ShaderStageFlag::eFragment, 0u, renderer::getSize( renderer::AttributeFormat::eVec2i ) }
		, m_renderPass{ doCreateRenderPass( m_engine ) }
		, m_fbo{ doCreateFrameBuffer( *m_renderPass, m_result ) }
		, m_timer{ std::make_shared< RenderPassTimer >( m_engine, cuT( "SSAO" ), cuT( "Blur" ) ) }
	{
		*m_axisUniform.getData() = axis;
		renderer::DescriptorSetLayoutBindingArray bindings
		{
			{ 0u, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eFragment },
			{ 1u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment },
		};
		createPipeline( size
			, Position{}
			, m_program
			, input.getTexture()->getView()
			, *m_renderPass
			, bindings
			, { m_pushConstantRange } );
		RgbaColour colour{ RgbaColour::fromPredefined( PredefinedRgbaColour::eOpaqueWhite ) };
		auto & device = *m_renderSystem.getCurrentDevice();
		m_commandBuffer = device.getGraphicsCommandPool().createCommandBuffer();

		if ( m_commandBuffer->begin( renderer::CommandBufferUsageFlag::eSimultaneousUse ) )
		{
			m_commandBuffer->resetQueryPool( m_timer->getQuery()
				, 0u
				, 2u );
			m_commandBuffer->writeTimestamp( renderer::PipelineStageFlag::eTopOfPipe
				, m_timer->getQuery()
				, 0u );
			m_commandBuffer->beginRenderPass( *m_renderPass
				, *m_fbo
				, { colour }
				, renderer::SubpassContents::eInline );
			registerFrame( *m_commandBuffer );
			m_commandBuffer->writeTimestamp( renderer::PipelineStageFlag::eTopOfPipe
				, m_timer->getQuery()
				, 1u );
			m_commandBuffer->endRenderPass();
			m_commandBuffer->end();
		}
	}

	SsaoBlurPass::~SsaoBlurPass()
	{
		m_timer.reset();
		m_fbo.reset();
		m_renderPass.reset();
		m_result.cleanup();
		m_ssaoConfigUbo.cleanup();
	}

	void SsaoBlurPass::doFillDescriptorSet( renderer::DescriptorSetLayout & descriptorSetLayout
		, renderer::DescriptorSet & descriptorSet )
	{
		descriptorSet.createBinding( descriptorSetLayout.getBinding( 0u )
			, m_ssaoConfigUbo.getUbo()
			, 0u
			, 1u );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( 1u )
			, m_normals.getTexture()->getView()
			, m_sampler->getSampler() );
	}

	void SsaoBlurPass::doRegisterFrame( renderer::CommandBuffer & commandBuffer )
	{
		commandBuffer.pushConstants( m_pipeline->getPipelineLayout(), m_axisUniform );
	}

	void SsaoBlurPass::blur()
	{
		m_timer->start();
		auto & device = *m_renderSystem.getCurrentDevice();
		device.getGraphicsQueue().submit( *m_commandBuffer, nullptr );
		m_timer->stop();
	}
}
