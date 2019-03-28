#include "Castor3D/Technique/Opaque/Ssao/BlurPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Technique/Opaque/Ssao/SsaoConfig.hpp"
#include "Castor3D/Technique/Opaque/Ssao/SsaoConfigUbo.hpp"
#include "Castor3D/Texture/Sampler.hpp"
#include "Castor3D/Texture/TextureLayout.hpp"
#include "Castor3D/Texture/TextureUnit.hpp"

#include <Ashes/Image/Texture.hpp>
#include <Ashes/Image/TextureView.hpp>
#include <Ashes/Pipeline/ColourBlendState.hpp>
#include <Ashes/Pipeline/DepthStencilState.hpp>
#include <Ashes/Pipeline/MultisampleState.hpp>
#include <Ashes/Pipeline/RasterisationState.hpp>
#include <Ashes/Pipeline/ShaderStageState.hpp>
#include <Ashes/RenderPass/FrameBuffer.hpp>
#include <Ashes/RenderPass/FrameBufferAttachment.hpp>
#include <Ashes/RenderPass/RenderPass.hpp>
#include <Ashes/RenderPass/RenderPassCreateInfo.hpp>

#include "Castor3D/Shader/GlslToSpv.hpp"

#include <ShaderWriter/Source.hpp>
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslShadow.hpp"

using namespace castor;

namespace castor3d
{
	namespace
	{
		ShaderPtr doGetVertexProgram( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			auto position = writer.declInput< Vec2 >( cuT( "position" ), 0u );

			// Shader outputs
			auto out = writer.getOut();

			writer.implementFunction< Void >( cuT( "main" )
				, [&]()
				{
					out.gl_out.gl_Position = vec4( position, 0.0, 1.0 );
				} );
			return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}
		
		ShaderPtr doGetPixelProgram( Engine & engine
			, SsaoConfig const & config )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace sdw;
			FragmentWriter writer;

			UBO_SSAO_CONFIG( writer, 0u, 0u );
			/** (1, 0) or (0, 1)*/
			Ubo configuration{ writer, cuT( "BlurConfiguration" ), 1u, 0u };
			auto c3d_axis = configuration.declMember< IVec2 >( cuT( "c3d_axis" ) );
			auto c3d_dummy = configuration.declMember< IVec2 >( cuT( "c3d_dummy" ) );
			auto c3d_gaussian = configuration.declMember< Vec4 >( cuT( "c3d_gaussian" ), 2u );
			configuration.end();
			auto c3d_mapNormal = writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapNormal" ), 2u, 0u, config.useNormalsBuffer );
			auto c3d_mapInput = writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapInput" ), 3u, 0u );

			/** Same size as result buffer, do not offset by guard band when reading from it */
			auto c3d_readMultiplyFirst = writer.declConstant( cuT( "c3d_readMultiplyFirst" ), vec3( 2.0_f ), config.useNormalsBuffer );
			auto c3d_readAddSecond = writer.declConstant( cuT( "c3d_readAddSecond" ), vec3( 1.0_f ), config.useNormalsBuffer );

			auto in = writer.getIn();

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
				, InFloat{ writer, cuT( "p" ) } );

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
					writer.returnStmt( vec3( sdw::fma( S.xy(), projInfo.xy(), projInfo.zw() ) * z, z ) );
				}
				, InVec2{ writer, cuT( "S" ) }
				, InFloat{ writer, cuT( "z" ) }
				, InVec4{ writer, cuT( "projInfo" ) } );

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
				, InFloat{ writer, cuT( "key" ) }
				, InIVec2{ writer, cuT( "ssCenter" ) }
				, InVec4{ writer, cuT( "projInfo" ) } );

			auto getTapInformation = writer.implementFunction< Void >( cuT( "getTapInformation" )
				, [&]( IVec2 const & tapLoc
					, Float tapKey
					, Float value
					, Vec3 tapNormal )
				{
					auto temp = writer.declLocale( cuT( "temp" )
						, texelFetch( c3d_mapInput, tapLoc, 0_i ) );
					tapKey = unpackKey( temp.g() );
					value = temp.r();

					if ( config.useNormalsBuffer )
					{
						tapNormal = texelFetch( c3d_mapNormal, tapLoc, 0_i ).xyz();
						tapNormal = normalize( sdw::fma( tapNormal, c3d_readMultiplyFirst.xyz(), c3d_readAddSecond.xyz() ) );
					}
					else
					{
						tapNormal = vec3( 0.0_f );
					}
				}
				, InIVec2{ writer, cuT( "tapLoc" ) }
				, OutFloat{ writer, cuT( "tapKey" ) }
				, OutFloat{ writer, cuT( "value" ) }
				, OutVec3{ writer, cuT( "tapNormal" ) } );

			auto square = writer.implementFunction< Float >( cuT( "square" )
				, [&]( Float const & x )
				{
					writer.returnStmt( x * x );
				}
				, InFloat{ writer, cuT( "x") } );

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

					if ( config.useNormalsBuffer )
					{
						auto normalCloseness = writer.declLocale( cuT( "normalCloseness" )
							, dot( tapNormal, normal ) );

						if ( !config.blurHighQuality )
						{
							normalCloseness = normalCloseness * normalCloseness;
							normalCloseness = normalCloseness * normalCloseness;
							k_normal = 4.0_f;
						}

						auto normalError = writer.declLocale( cuT( "normalError" )
							, writer.paren( 1.0_f - normalCloseness ) * k_normal );
						normalWeight = max( writer.paren( 1.0_f - c3d_edgeSharpness * normalError ), 0.0_f );

						if ( config.blurHighQuality )
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
								, Float{ pow( max( 0.0_f
										, 1.0_f - c3d_edgeSharpness * 2.0 * k_plane * planeError / sqrt( distance2 ) )
									, 2.0_f ) } );
						}
					}

					writer.returnStmt( depthWeight * normalWeight * planeWeight );
				}
				, InFloat{ writer, cuT( "key" ) }
				, InFloat{ writer, cuT( "tapKey" ) }
				, InIVec2{ writer, cuT( "tapLoc" ) }
				, InVec3{ writer, cuT( "normal" ) }
				, InVec3{ writer, cuT( "tapNormal" ) }
				, InVec3{ writer, cuT( "position" ) } );

			writer.implementFunction< Void >( cuT( "main" )
				, [&]()
				{
					auto ssCenter = writer.declLocale( cuT( "ssCenter" )
						, ivec2( in.gl_FragCoord.xy() ) );

					auto temp = writer.declLocale( cuT( "temp" )
						, texelFetch( c3d_mapInput, ssCenter, 0_i ) );
					auto sum = writer.declLocale( cuT( "sum" )
						, temp.r() );

					keyPassThrough = temp.g();
					auto key = writer.declLocale( cuT( "key" )
						, unpackKey( keyPassThrough ) );
					auto normal = writer.declLocale( cuT( "normal" )
						, vec3( 0.0_f ) );

					if ( config.useNormalsBuffer )
					{
						normal = texelFetch( c3d_mapNormal, ssCenter, 0_i ).xyz();
						normal = normalize( sdw::fma( normal, c3d_readMultiplyFirst.xyz(), c3d_readAddSecond.xyz() ) );
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
						, c3d_gaussian[0][0] );
					auto totalWeight = writer.declLocale( cuT( "totalWeight" )
						, BASE );
					sum *= totalWeight;

					auto position = writer.declLocale( cuT( "position" )
						, positionFromKey( key, ssCenter, c3d_projInfo ) );

					FOR( writer, Int, r, -c3d_blurRadius, r <= c3d_blurRadius, ++r )
					{
						// We already handled the zero case above.  This loop should be unrolled and the static branch optimized out,
						// so the IF statement has no runtime cost
						IF( writer, r != 0_i )
						{
							auto tapLoc = writer.declLocale( cuT( "tapLoc" )
								, ssCenter + c3d_axis * writer.paren( r * c3d_blurStepSize ) );

							// spatial domain: offset gaussian tap
							auto absR = writer.declLocale( cuT( "absR" )
								, writer.cast< UInt >( abs( r ) ) );
							auto weight = writer.declLocale( cuT( "weight" )
								, 0.3_f + c3d_gaussian[absR % 2_u][absR / 2_u] );

							auto tapKey = writer.declLocale< Float >( cuT( "tapKey" ) );
							auto value = writer.declLocale< Float >( cuT( "value" ) );
							auto tapNormal = writer.declLocale< Vec3 >( cuT( "tapNormal" ) );
							getTapInformation( tapLoc, tapKey, value, tapNormal );

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
			return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );

#undef result
#undef keyPassThrough
		}

		ashes::ShaderStageStateArray doGetProgram( Engine & engine
			, SsaoConfig const & config
			, ShaderModule & vertexShader
			, ShaderModule & pixelShader )
		{
			vertexShader.shader = doGetVertexProgram( engine );
			pixelShader.shader = doGetPixelProgram( engine, config );
			auto & device = getCurrentDevice( engine );
			ashes::ShaderStageStateArray result
			{
				{ device.createShaderModule( ashes::ShaderStageFlag::eVertex ) },
				{ device.createShaderModule( ashes::ShaderStageFlag::eFragment ) },
			};
			result[0].module->loadShader( engine.getRenderSystem()->compileShader( vertexShader ) );
			result[1].module->loadShader( engine.getRenderSystem()->compileShader( pixelShader ) );
			return result;
		}

		SamplerSPtr doCreateSampler( Engine & engine
			, String const & name
			, ashes::WrapMode mode )
		{
			SamplerSPtr sampler;

			if ( engine.getSamplerCache().has( name ) )
			{
				sampler = engine.getSamplerCache().find( name );
			}
			else
			{
				sampler = engine.getSamplerCache().add( name );
				sampler->setMinFilter( ashes::Filter::eNearest );
				sampler->setMagFilter( ashes::Filter::eNearest );
				sampler->setWrapS( mode );
				sampler->setWrapT( mode );
			}

			return sampler;
		}

		TextureUnit doCreateTexture( Engine & engine, ashes::Extent2D const & size )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto sampler = doCreateSampler( engine, cuT( "SSAOBlur_Result" ), ashes::WrapMode::eClampToEdge );

			ashes::ImageCreateInfo image{};
			image.arrayLayers = 1u;
			image.extent.width = size.width;
			image.extent.height = size.height;
			image.extent.depth = 1u;
			image.format = SsaoBlurPass::ResultFormat;
			image.imageType = ashes::TextureType::e2D;
			image.mipLevels = 1u;
			image.samples = ashes::SampleCountFlag::e1;
			image.usage = ashes::ImageUsageFlag::eColourAttachment | ashes::ImageUsageFlag::eSampled;

			auto ssaoResult = std::make_shared< TextureLayout >( renderSystem
				, image
				, ashes::MemoryPropertyFlag::eDeviceLocal
				, cuT( "SSAOBlur_Result" ) );
			TextureUnit result{ engine };
			result.setTexture( ssaoResult );
			result.setSampler( sampler );
			result.initialise();
			return result;
		}

		ashes::RenderPassPtr doCreateRenderPass( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto & device = getCurrentDevice( renderSystem );
			ashes::RenderPassCreateInfo renderPass{};
			renderPass.flags = 0u;

			renderPass.attachments.resize( 1u );
			renderPass.attachments[0].format = SsaoBlurPass::ResultFormat;
			renderPass.attachments[0].samples = ashes::SampleCountFlag::e1;
			renderPass.attachments[0].initialLayout = ashes::ImageLayout::eUndefined;
			renderPass.attachments[0].finalLayout = ashes::ImageLayout::eShaderReadOnlyOptimal;
			renderPass.attachments[0].loadOp = ashes::AttachmentLoadOp::eClear;
			renderPass.attachments[0].storeOp = ashes::AttachmentStoreOp::eStore;
			renderPass.attachments[0].stencilLoadOp = ashes::AttachmentLoadOp::eDontCare;
			renderPass.attachments[0].stencilStoreOp = ashes::AttachmentStoreOp::eDontCare;

			renderPass.subpasses.resize( 1u );
			renderPass.subpasses[0].colorAttachments.push_back( { 0u, ashes::ImageLayout::eColourAttachmentOptimal } );
			renderPass.subpasses[0].pipelineBindPoint = ashes::PipelineBindPoint::eGraphics;

			renderPass.dependencies.resize( 2u );
			renderPass.dependencies[0].srcSubpass = ashes::ExternalSubpass;
			renderPass.dependencies[0].dstSubpass = 0u;
			renderPass.dependencies[0].srcAccessMask = ashes::AccessFlag::eColourAttachmentWrite;
			renderPass.dependencies[0].dstAccessMask = ashes::AccessFlag::eShaderRead;
			renderPass.dependencies[0].srcStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
			renderPass.dependencies[0].dstStageMask = ashes::PipelineStageFlag::eFragmentShader;
			renderPass.dependencies[0].dependencyFlags = ashes::DependencyFlag::eByRegion;

			renderPass.dependencies[1].srcSubpass = 0u;
			renderPass.dependencies[1].dstSubpass = ashes::ExternalSubpass;
			renderPass.dependencies[1].srcAccessMask = ashes::AccessFlag::eColourAttachmentWrite;
			renderPass.dependencies[1].dstAccessMask = ashes::AccessFlag::eShaderRead;
			renderPass.dependencies[1].srcStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
			renderPass.dependencies[1].dstStageMask = ashes::PipelineStageFlag::eFragmentShader;
			renderPass.dependencies[1].dependencyFlags = ashes::DependencyFlag::eByRegion;

			return device.createRenderPass( renderPass );
		}

		ashes::FrameBufferPtr doCreateFrameBuffer( ashes::RenderPass const & renderPass
			, TextureUnit const & texture )
		{
			ashes::FrameBufferAttachmentArray attaches;
			attaches.emplace_back( *( renderPass.getAttachments().begin() ), texture.getTexture()->getDefaultView() );
			auto size = texture.getTexture()->getDimensions();
			return renderPass.createFrameBuffer( ashes::Extent2D{ size.width, size.height }
				, std::move( attaches ) );
		}
	}

	//*********************************************************************************************

	SsaoBlurPass::SsaoBlurPass( Engine & engine
		, ashes::Extent2D const & size
		, SsaoConfig const & config
		, SsaoConfigUbo & ssaoConfigUbo
		, Point2i const & axis
		, TextureUnit const & input
		, ashes::TextureView const & normals )
		: RenderQuad{ *engine.getRenderSystem(), true }
		, m_engine{ engine }
		, m_ssaoConfigUbo{ ssaoConfigUbo }
		, m_input{ input }
		, m_normals{ normals }
		, m_config{ config }
		, m_program{ doGetProgram( m_engine, config, m_vertexShader, m_pixelShader ) }
		, m_size{ size }
		, m_result{ doCreateTexture( m_engine, m_size ) }
		, m_renderPass{ doCreateRenderPass( m_engine ) }
		, m_fbo{ doCreateFrameBuffer( *m_renderPass, m_result ) }
		, m_timer{ std::make_shared< RenderPassTimer >( m_engine, cuT( "SSAO" ), cuT( "Blur" ) ) }
		, m_finished{ getCurrentDevice( engine ).createSemaphore() }
		, m_configurationUbo{ ashes::makeUniformBuffer< Configuration >( getCurrentDevice( engine ), 1u, 0u, ashes::MemoryPropertyFlag::eHostVisible ) }
		, m_vertexShader{ ashes::ShaderStageFlag::eVertex, "SsaoBlur" }
		, m_pixelShader{ ashes::ShaderStageFlag::eFragment, "SsaoBlur" }
	{
		auto & device = getCurrentDevice( m_renderSystem );
		device.debugMarkerSetObjectName(
			{
				ashes::DebugReportObjectType::eBuffer,
				&m_configurationUbo->getUbo().getBuffer(),
				"SsaoBlurConfigUbo"
			} );
		auto & configuration = m_configurationUbo->getData();
		configuration.axis = axis;

		ashes::DescriptorSetLayoutBindingArray bindings
		{
			{ 0u, ashes::DescriptorType::eUniformBuffer, ashes::ShaderStageFlag::eFragment },
			{ 1u, ashes::DescriptorType::eUniformBuffer, ashes::ShaderStageFlag::eFragment },
			{ 2u, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment },
		};
		createPipeline( size
			, Position{}
			, m_program
			, input.getTexture()->getDefaultView()
			, *m_renderPass
			, bindings
			, {} );
		static ashes::ClearColorValue const colour{ 1.0, 1.0, 1.0, 1.0 };
		m_commandBuffer = device.getGraphicsCommandPool().createCommandBuffer();

		m_commandBuffer->begin( ashes::CommandBufferUsageFlag::eSimultaneousUse );
		m_timer->beginPass( *m_commandBuffer );
		m_commandBuffer->beginRenderPass( *m_renderPass
			, *m_fbo
			, { colour }
			, ashes::SubpassContents::eInline );
		registerFrame( *m_commandBuffer );
		m_timer->endPass( *m_commandBuffer );
		m_commandBuffer->endRenderPass();
		m_commandBuffer->end();
	}

	SsaoBlurPass::~SsaoBlurPass()
	{
		m_timer.reset();
		m_fbo.reset();
		m_renderPass.reset();
		m_result.cleanup();
	}

	void SsaoBlurPass::update()const
	{
		if ( m_config.blurRadius.isDirty() )
		{
			auto & configuration = m_configurationUbo->getData();

			switch ( m_config.blurRadius.value().value() )
			{
			case 1u:
				configuration.gaussian[0][0] = 0.5f;
				configuration.gaussian[0][1] = 0.25f;
				break;
			case 2u:
				configuration.gaussian[0][0] = 0.153170f;
				configuration.gaussian[0][1] = 0.144893f;
				configuration.gaussian[0][2] = 0.122649f;
				break;
			case 3u:
				configuration.gaussian[0][0] = 0.153170f;
				configuration.gaussian[0][1] = 0.144893f;
				configuration.gaussian[0][2] = 0.122649f;
				configuration.gaussian[0][3] = 0.092902f;
				break;
			case 4u:
				configuration.gaussian[0][0] = 0.153170f;
				configuration.gaussian[0][1] = 0.144893f;
				configuration.gaussian[0][2] = 0.122649f;
				configuration.gaussian[0][3] = 0.092902f;
				configuration.gaussian[1][0] = 0.062970f;
				break;
			case 5u:
				configuration.gaussian[0][0] = 0.111220f;
				configuration.gaussian[0][1] = 0.107798f;
				configuration.gaussian[0][2] = 0.098151f;
				configuration.gaussian[0][3] = 0.083953f;
				configuration.gaussian[1][0] = 0.067458f;
				configuration.gaussian[1][1] = 0.050920f;
				break;
			default:
				configuration.gaussian[0][0] = 0.111220f;
				configuration.gaussian[0][1] = 0.107798f;
				configuration.gaussian[0][2] = 0.098151f;
				configuration.gaussian[0][3] = 0.083953f;
				configuration.gaussian[1][0] = 0.067458f;
				configuration.gaussian[1][1] = 0.050920f;
				configuration.gaussian[1][2] = 0.036108f;
				break;
			}

			m_configurationUbo->upload();
		}
	}

	ashes::Semaphore const & SsaoBlurPass::blur( ashes::Semaphore const & toWait )const
	{
		auto timerBlock = m_timer->start();
		m_timer->notifyPassRender();
		auto * result = &toWait;
		auto & device = getCurrentDevice( m_renderSystem );

		device.getGraphicsQueue().submit( *m_commandBuffer
			, *result
			, ashes::PipelineStageFlag::eColourAttachmentOutput
			, *m_finished
			, nullptr );
		result = m_finished.get();

		return *result;
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
			, ashes::ShaderStageFlag::eVertex
			, *m_vertexShader.shader );
		visitor.visit( name
			, ashes::ShaderStageFlag::eFragment
			, *m_pixelShader.shader );
		config.accept( name, visitor );
	}

	void SsaoBlurPass::doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
		, ashes::DescriptorSet & descriptorSet )
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

	void SsaoBlurPass::doRegisterFrame( ashes::CommandBuffer & commandBuffer )const
	{
	}
}
