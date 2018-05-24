#include "BlurPass.hpp"

#include "Engine.hpp"
#include "Render/RenderPassTimer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderSystem.hpp"
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
#include <Pipeline/ShaderStageState.hpp>
#include <RenderPass/FrameBuffer.hpp>
#include <RenderPass/FrameBufferAttachment.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/RenderPassCreateInfo.hpp>

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

#include "Shader/Shaders/GlslLight.hpp"
#include "Shader/Shaders/GlslShadow.hpp"

using namespace castor;

namespace castor3d
{
	namespace
	{
		static renderer::Format constexpr ColourFormat = renderer::Format::eR8G8B8A8_UNORM;

		glsl::Shader doGetVertexProgram( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			auto position = writer.declAttribute< Vec2 >( cuT( "position" ), 0u );

			// Shader outputs
			auto out = gl_PerVertex{ writer };

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
					out.gl_Position() = vec4( position, 0.0, 1.0 );
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
			/** (1, 0) or (0, 1)*/
			Ubo configuration{ writer, cuT( "BlurConfiguration" ), 1u, 0u };
			auto c3d_axis = configuration.declMember< IVec2 >( cuT( "c3d_axis" ) );
			REQUIRE( config.m_blurRadius > 0 && config.m_blurRadius < 7 );
			auto gaussian = configuration.declMember< Float >( cuT( "gaussian" ), config.m_blurRadius + 1u );
			configuration.end();
			auto c3d_mapNormal = writer.declSampler< Sampler2D >( cuT( "c3d_mapNormal" ), 2u, 0u, config.m_useNormalsBuffer );
			auto c3d_mapInput = writer.declSampler< Sampler2D >( cuT( "c3d_mapInput" ), 3u, 0u );

			/** Same size as result buffer, do not offset by guard band when reading from it */
			auto c3d_readMultiplyFirst = writer.declConstant< Vec4 >( cuT( "c3d_readMultiplyFirst" ), vec4( 2.0_f ), config.m_useNormalsBuffer );
			auto c3d_readAddSecond = writer.declConstant< Vec4 >( cuT( "c3d_readAddSecond" ), vec4( 1.0_f ), config.m_useNormalsBuffer );

			auto gl_FragCoord = writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) );

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
						tapNormal = normalize( glsl::fma( tapNormal, c3d_readMultiplyFirst.xyz(), c3d_readAddSecond.xyz() ) );
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

		renderer::ShaderStageStateArray doGetProgram( Engine & engine
			, SsaoConfig const & config
			, glsl::Shader & vertexShader
			, glsl::Shader & pixelShader )
		{
			vertexShader = doGetVertexProgram( engine );
			pixelShader = doGetPixelProgram( engine, config );
			auto & device = *engine.getRenderSystem()->getCurrentDevice();
			renderer::ShaderStageStateArray result
			{
				{ device.createShaderModule( renderer::ShaderStageFlag::eVertex ) },
				{ device.createShaderModule( renderer::ShaderStageFlag::eFragment ) },
			};
			result[0].module->loadShader( vertexShader.getSource() );
			result[1].module->loadShader( pixelShader.getSource() );
			return result;
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

		TextureUnit doCreateTexture( Engine & engine, renderer::Extent2D const & size )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto sampler = doCreateSampler( engine, cuT( "SSAOBlur_Result" ), renderer::WrapMode::eClampToEdge );

			renderer::ImageCreateInfo image{};
			image.arrayLayers = 1u;
			image.extent.width = size.width;
			image.extent.height = size.height;
			image.extent.depth = 1u;
			image.format = ColourFormat;
			image.imageType = renderer::TextureType::e2D;
			image.mipLevels = 1u;
			image.samples = renderer::SampleCountFlag::e1;
			image.usage = renderer::ImageUsageFlag::eColourAttachment | renderer::ImageUsageFlag::eSampled;

			auto ssaoResult = std::make_shared< TextureLayout >( renderSystem
				, image
				, renderer::MemoryPropertyFlag::eDeviceLocal );
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
			renderer::RenderPassCreateInfo renderPass{};
			renderPass.flags = 0u;

			renderPass.attachments.resize( 1u );
			renderPass.attachments[0].format = ColourFormat;
			renderPass.attachments[0].samples = renderer::SampleCountFlag::e1;
			renderPass.attachments[0].initialLayout = renderer::ImageLayout::eUndefined;
			renderPass.attachments[0].finalLayout = renderer::ImageLayout::eShaderReadOnlyOptimal;
			renderPass.attachments[0].loadOp = renderer::AttachmentLoadOp::eClear;
			renderPass.attachments[0].storeOp = renderer::AttachmentStoreOp::eStore;
			renderPass.attachments[0].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
			renderPass.attachments[0].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;

			renderPass.dependencies.resize( 1u );
			renderPass.dependencies[0].srcSubpass = renderer::ExternalSubpass;
			renderPass.dependencies[0].dstSubpass = 0u;
			renderPass.dependencies[0].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
			renderPass.dependencies[0].dstStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
			renderPass.dependencies[0].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
			renderPass.dependencies[0].dstAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
			renderPass.dependencies[0].dependencyFlags = renderer::DependencyFlag::eByRegion;

			renderPass.subpasses.resize( 1u );
			renderPass.subpasses[0].colorAttachments.push_back( { 0u, renderer::ImageLayout::eColourAttachmentOptimal } );
			renderPass.subpasses[0].pipelineBindPoint = renderer::PipelineBindPoint::eGraphics;

			renderPass.dependencies.resize( 2u );
			renderPass.dependencies[0].srcSubpass = renderer::ExternalSubpass;
			renderPass.dependencies[0].dstSubpass = 0u;
			renderPass.dependencies[0].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
			renderPass.dependencies[0].dstAccessMask = renderer::AccessFlag::eShaderRead;
			renderPass.dependencies[0].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
			renderPass.dependencies[0].dstStageMask = renderer::PipelineStageFlag::eFragmentShader;
			renderPass.dependencies[0].dependencyFlags = renderer::DependencyFlag::eByRegion;

			renderPass.dependencies[1].srcSubpass = 0u;
			renderPass.dependencies[1].dstSubpass = renderer::ExternalSubpass;
			renderPass.dependencies[1].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
			renderPass.dependencies[1].dstAccessMask = renderer::AccessFlag::eShaderRead;
			renderPass.dependencies[1].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
			renderPass.dependencies[1].dstStageMask = renderer::PipelineStageFlag::eFragmentShader;
			renderPass.dependencies[1].dependencyFlags = renderer::DependencyFlag::eByRegion;

			return device.createRenderPass( renderPass );
		}

		renderer::FrameBufferPtr doCreateFrameBuffer( renderer::RenderPass const & renderPass
			, TextureUnit const & texture )
		{
			renderer::FrameBufferAttachmentArray attaches;
			attaches.emplace_back( *( renderPass.getAttachments().begin() ), texture.getTexture()->getDefaultView() );
			auto size = texture.getTexture()->getDimensions();
			return renderPass.createFrameBuffer( renderer::Extent2D{ size.width, size.height }
				, std::move( attaches ) );
		}
	}

	//*********************************************************************************************

	SsaoBlurPass::SsaoBlurPass( Engine & engine
		, renderer::Extent2D const & size
		, SsaoConfig const & config
		, SsaoConfigUbo & ssaoConfigUbo
		, Point2i const & axis
		, TextureUnit const & input
		, renderer::TextureView const & normals )
		: RenderQuad{ *engine.getRenderSystem(), true }
		, m_engine{ engine }
		, m_ssaoConfigUbo{ ssaoConfigUbo }
		, m_input{ input }
		, m_normals{ normals }
		, m_program{ doGetProgram( m_engine, config, m_vertexShader, m_pixelShader ) }
		, m_size{ size }
		, m_result{ doCreateTexture( m_engine, m_size ) }
		, m_renderPass{ doCreateRenderPass( m_engine ) }
		, m_fbo{ doCreateFrameBuffer( *m_renderPass, m_result ) }
		, m_timer{ std::make_shared< RenderPassTimer >( m_engine, cuT( "SSAO" ), cuT( "Blur" ) ) }
		, m_finished{ engine.getRenderSystem()->getCurrentDevice()->createSemaphore() }
		, m_configurationUbo{ renderer::makeUniformBuffer< Configuration >( *engine.getRenderSystem()->getCurrentDevice(), 1u, 0u, renderer::MemoryPropertyFlag::eHostVisible ) }
	{
		auto & configuration = m_configurationUbo->getData();

		switch ( config.m_blurRadius )
		{
		case 1u:
			configuration.gaussian[0] = 0.5f;
			configuration.gaussian[1] = 0.25f;
			break;
		case 2u:
			configuration.gaussian[0] = 0.153170f;
			configuration.gaussian[1] = 0.144893f;
			configuration.gaussian[2] = 0.122649f;
			break;
		case 3u:
			configuration.gaussian[0] = 0.153170f;
			configuration.gaussian[1] = 0.144893f;
			configuration.gaussian[2] = 0.122649f;
			configuration.gaussian[3] = 0.092902f;
			break;
		case 4u:
			configuration.gaussian[0] = 0.153170f;
			configuration.gaussian[1] = 0.144893f;
			configuration.gaussian[2] = 0.122649f;
			configuration.gaussian[3] = 0.092902f;
			configuration.gaussian[4] = 0.062970f;
			break;
		case 5u:
			configuration.gaussian[0] = 0.111220f;
			configuration.gaussian[1] = 0.107798f;
			configuration.gaussian[2] = 0.098151f;
			configuration.gaussian[3] = 0.083953f;
			configuration.gaussian[4] = 0.067458f;
			configuration.gaussian[5] = 0.050920f;
			break;
		default:
			configuration.gaussian[0] = 0.111220f;
			configuration.gaussian[1] = 0.107798f;
			configuration.gaussian[2] = 0.098151f;
			configuration.gaussian[3] = 0.083953f;
			configuration.gaussian[4] = 0.067458f;
			configuration.gaussian[5] = 0.050920f;
			configuration.gaussian[6] = 0.036108f;
			break;
		};

		configuration.axis = axis;
		m_configurationUbo->upload();

		renderer::DescriptorSetLayoutBindingArray bindings
		{
			{ 0u, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eFragment },
			{ 1u, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eFragment },
			{ 2u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment },
		};
		createPipeline( size
			, Position{}
			, m_program
			, input.getTexture()->getDefaultView()
			, *m_renderPass
			, bindings
			, {} );
		static renderer::ClearColorValue const colour{ 1.0, 1.0, 1.0, 1.0 };
		auto & device = *m_renderSystem.getCurrentDevice();
		m_commandBuffer = device.getGraphicsCommandPool().createCommandBuffer();

		if ( m_commandBuffer->begin( renderer::CommandBufferUsageFlag::eSimultaneousUse ) )
		{
			m_commandBuffer->resetQueryPool( m_timer->getQuery()
				, 0u
				, 2u );
			m_commandBuffer->writeTimestamp( renderer::PipelineStageFlag::eBottomOfPipe
				, m_timer->getQuery()
				, 0u );
			m_commandBuffer->beginRenderPass( *m_renderPass
				, *m_fbo
				, { colour }
				, renderer::SubpassContents::eInline );
			registerFrame( *m_commandBuffer );
			m_commandBuffer->writeTimestamp( renderer::PipelineStageFlag::eBottomOfPipe
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
	}

	void SsaoBlurPass::blur( renderer::Semaphore const & toWait )const
	{
		m_timer->start();
		auto & device = *m_renderSystem.getCurrentDevice();
		device.getGraphicsQueue().submit( *m_commandBuffer
			, toWait
			, renderer::PipelineStageFlag::eColourAttachmentOutput
			, *m_finished
			, nullptr );
		m_timer->step();
		m_timer->stop();
	}

	void SsaoBlurPass::accept( bool horizontal
		, SsaoConfig & config
		, RenderTechniqueVisitor & visitor )
	{
		castor::StringStream stream{ castor::makeStringStream() };
		stream << cuT( "SSAO - " );

		if ( horizontal )
		{
			stream << cuT( "Horizontal - " );
		}
		else
		{
			stream << cuT( "Vertical - " );
		}

		stream << cuT( "Blur" );
		auto name = stream.str();
		visitor.visit( name
			, renderer::ShaderStageFlag::eVertex
			, m_vertexShader );
		visitor.visit( name
			, renderer::ShaderStageFlag::eFragment
			, m_pixelShader );
		config.accept( name, visitor );
	}

	void SsaoBlurPass::doFillDescriptorSet( renderer::DescriptorSetLayout & descriptorSetLayout
		, renderer::DescriptorSet & descriptorSet )
	{
		descriptorSet.createBinding( descriptorSetLayout.getBinding( 0u )
			, m_ssaoConfigUbo.getUbo()
			, 0u
			, 1u );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( 1u )
			, *m_configurationUbo
			, 0u
			, 1u );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( 2u )
			, m_normals
			, m_sampler->getSampler() );
	}

	void SsaoBlurPass::doRegisterFrame( renderer::CommandBuffer & commandBuffer )const
	{
	}
}
