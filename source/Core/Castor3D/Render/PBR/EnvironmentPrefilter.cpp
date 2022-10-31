#include "Castor3D/Render/PBR/EnvironmentPrefilter.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Limits.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslBRDFHelpers.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/Graphics/Size.hpp>
#include <CastorUtils/Math/Angle.hpp>

#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>
#include <ashespp/Sync/Fence.hpp>

#include <ShaderWriter/Source.hpp>

#include <RenderGraph/ResourceHandler.hpp>

namespace castor3d
{
	//*********************************************************************************************

	namespace envpref
	{
		static Texture doCreatePrefilteredTexture( RenderDevice const & device
			, crg::ResourceHandler & handler
			, castor::Size const & size
			, std::string const & prefix )
		{
			Texture result{ device
				, handler
				, prefix + "EnvironmentPrefilterResult"
				, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT
				, { size[0], size[1], 1u }
				, 6u
				, MaxIblReflectionLod + 1u
				, VK_FORMAT_R32G32B32A32_SFLOAT
				, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT };
			result.create();
			return result;
		}

		static SamplerResPtr doCreateSampler( Engine & engine
			, RenderDevice const & device
			, std::string const & prefix
			, uint32_t maxLod )
		{
			SamplerResPtr result;
			auto stream = castor::makeStringStream();
			stream << prefix << cuT( "IblTexturesPrefiltered_" ) << maxLod;
			auto name = stream.str();

			if ( engine.hasSampler( name ) )
			{
				result = engine.findSampler( name );
			}
			else
			{
				auto created = engine.createSampler( name, engine );
				created->setMinFilter( VK_FILTER_LINEAR );
				created->setMagFilter( VK_FILTER_LINEAR );
				created->setMipFilter( VK_SAMPLER_MIPMAP_MODE_LINEAR );
				created->setWrapS( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
				created->setWrapT( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
				created->setWrapR( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
				created->setMinLod( 0.0f );
				created->setMaxLod( float( maxLod ) );
				result = engine.addSampler( name, created, false );
			}

			result.lock()->initialise( device );
			return result;
		}

		static ashes::PipelineShaderStageCreateInfoArray doCreateProgram( RenderDevice const & device
			, VkExtent2D const & size
			, uint32_t mipLevel
			, bool isCharlie )
		{
			ShaderModule vtx{ VK_SHADER_STAGE_VERTEX_BIT, "EnvironmentPrefilter" };
			{
				using namespace sdw;
				VertexWriter writer;

				// Inputs
				auto position = writer.declInput< Vec3 >( "position", 0u );
				UniformBuffer matrix{ writer, "Matrix", 0u, 0u };
				auto c3d_viewProjection = matrix.declMember< Mat4 >( "c3d_viewProjection" );
				matrix.end();

				// Outputs
				auto vtx_worldPosition = writer.declOutput< Vec3 >( "vtx_worldPosition", 0u );

				writer.implementMainT< VoidT, VoidT >( [&]( VertexIn in
					, VertexOut out )
					{
						vtx_worldPosition = position;
						out.vtx.position = ( c3d_viewProjection * vec4( position, 1.0_f ) ).xyww();
					} );
				vtx.shader = std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
			}

			ShaderModule pxl{ VK_SHADER_STAGE_FRAGMENT_BIT, "EnvironmentPrefilter" };
			{
				using namespace sdw;
				FragmentWriter writer;
				shader::BRDFHelpers brdf{ writer };

				// Inputs
				auto vtx_worldPosition = writer.declInput< Vec3 >( "vtx_worldPosition", 0u );
				auto c3d_mapEnvironment = writer.declCombinedImg< FImgCubeRgba32 >( "c3d_mapEnvironment", 1u, 0u );
				auto c3d_roughness = writer.declConstant< Float >( "c3d_roughness"
					, writer.cast< Float >( float( mipLevel ) / float( MaxIblReflectionLod ) ) );

				// Outputs
				auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_FragColor", 0u );

				writer.implementMainT< VoidT, VoidT >( [&]( FragmentIn in
					, FragmentOut out )
					{
						// From https://learnopengl.com/#!PBR/Lighting
						auto N = writer.declLocale( "N"
							, normalize( vtx_worldPosition ) );
						auto R = writer.declLocale( "R"
							, N );
						auto V = writer.declLocale( "V"
							, R );

						auto sampleCount = writer.declLocale( "sampleCount"
							, 1024_u );
						auto totalWeight = writer.declLocale( "totalWeight"
							, 0.0_f );
						auto prefilteredColor = writer.declLocale( "prefilteredColor"
							, vec3( 0.0_f ) );

						FOR( writer, UInt, i, 0_u, i < sampleCount, ++i )
						{
							auto xi = writer.declLocale( "xi"
								, brdf.hammersley( i, sampleCount ) );
							auto importanceSample = writer.declLocale( "importanceSample"
								, ( isCharlie
									? brdf.getImportanceSample( brdf.importanceSampleCharlie( xi, c3d_roughness ), N )
									: brdf.getImportanceSample( brdf.importanceSampleGGX( xi, c3d_roughness ), N ) ) );
							auto H = writer.declLocale( "H"
								, importanceSample.xyz() );
							auto pdf = writer.declLocale( "pdf"
								, importanceSample.w() );

							auto L = writer.declLocale( "L"
								, normalize( vec3( 2.0_f ) * dot( V, H ) * H - V ) );
							auto NdotL = writer.declLocale( "NdotL"
								, dot( N, L ) );

							IF( writer, NdotL > 0.0_f )
							{
								auto resolution = Float{ float( size.width ) }; // resolution of source cubemap (per face)
								auto omegaP = writer.declLocale( "omegaP"
									, ( 4.0f * castor::Pi< float > ) / ( 6.0_f * resolution * resolution ) );
								auto omegaS = writer.declLocale( "omegaS"
									, 1.0_f / ( writer.cast< Float >( sampleCount ) * pdf + 0.0001_f ) );
								auto lod = writer.declLocale( "lod"
									, writer.ternary( c3d_roughness == 0.0_f
										, 0.0_f
										, 0.5_f * log2( omegaS / omegaP ) ) );

								prefilteredColor += c3d_mapEnvironment.sample( L, lod ).rgb() * NdotL;
								totalWeight += NdotL;
							}
							FI;
						}
						ROF;

						IF( writer, totalWeight != 0.0f )
						{
							prefilteredColor /= totalWeight;
						}
						ELSE
						{
							prefilteredColor /= writer.cast< sdw::Float >( sampleCount );
						}
						FI;

						pxl_fragColor = vec4( prefilteredColor, 1.0_f );
					} );

				pxl.shader = std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
			}

			return ashes::PipelineShaderStageCreateInfoArray
			{
				makeShaderState( device, vtx ),
				makeShaderState( device, pxl ),
			};
		}

		static ashes::RenderPassPtr doCreateRenderPass( RenderDevice const & device
			, std::string const & prefix
			, VkFormat format )
		{
			ashes::VkAttachmentDescriptionArray attaches
			{
				{
					0u,
					format,
					VK_SAMPLE_COUNT_1_BIT,
					VK_ATTACHMENT_LOAD_OP_CLEAR,
					VK_ATTACHMENT_STORE_OP_STORE,
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					VK_ATTACHMENT_STORE_OP_DONT_CARE,
					VK_IMAGE_LAYOUT_UNDEFINED,
					VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				}
			};
			ashes::SubpassDescriptionArray subpasses;
			subpasses.emplace_back( ashes::SubpassDescription
				{
					0u,
					VK_PIPELINE_BIND_POINT_GRAPHICS,
					{},
					{ { 0u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } },
					{},
					ashes::nullopt,
					{},
				} );
			ashes::VkSubpassDependencyArray dependencies
			{
				{
					0u,
					VK_SUBPASS_EXTERNAL,
					VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
					VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
					VK_ACCESS_SHADER_READ_BIT,
					VK_DEPENDENCY_BY_REGION_BIT,
				},
			};
			ashes::RenderPassCreateInfo createInfo
			{
				0u,
				std::move( attaches ),
				std::move( subpasses ),
				std::move( dependencies ),
			};
			auto result = device->createRenderPass( prefix + "EnvironmentPrefilter"
				, std::move( createInfo ) );
			return result;
		}
	}

	//*********************************************************************************************

	EnvironmentPrefilter::MipRenderCube::MipRenderCube( RenderDevice const & device
		, QueueData const & queueData
		, crg::ResourceHandler & handler
		, ashes::RenderPass const & renderPass
		, uint32_t mipLevel
		, VkExtent2D const & originalSize
		, VkExtent2D const & size
		, ashes::ImageView const & srcView
		, Texture const & dstTexture
		, SamplerResPtr sampler
		, bool isCharlie )
		: RenderCube{ device, false, std::move( sampler ) }
		, m_renderPass{ renderPass }
		, m_prefix{ isCharlie ? std::string{ "Sheen" } : std::string{} }
		, m_commands{ m_device, queueData, "EnvironmentPrefilter" }
	{
		for ( auto face = 0u; face < 6u; ++face )
		{
			auto name = m_prefix + "EnvironmentPrefilterL" + castor::string::toString( face ) + "M" + castor::string::toString( mipLevel );
			auto & facePass = m_frameBuffers[face];
			// Create the views.
			auto data = *dstTexture.wholeViewId.data;
			data.name = name;
			data.info.viewType = VK_IMAGE_VIEW_TYPE_2D;
			data.info.subresourceRange.baseArrayLayer = face;
			data.info.subresourceRange.layerCount = 1u;
			data.info.subresourceRange.baseMipLevel = mipLevel;
			data.info.subresourceRange.levelCount = 1u;
			auto viewId = handler.createViewId( data );
			facePass.dstView = handler.createImageView( device.makeContext(), viewId );
			// Initialise the frame buffer.
			auto createInfo = makeVkStruct< VkFramebufferCreateInfo >( 0u
				, renderPass
				, 1u
				, &facePass.dstView
				, size.width
				, size.height
				, 1u );
			facePass.frameBuffer = renderPass.createFrameBuffer( name
				, std::move( createInfo ) );
		}

		createPipelines( size
			, castor::Position{}
			, envpref::doCreateProgram( m_device, originalSize, mipLevel, isCharlie )
			, srcView
			, renderPass
			, {} );
	}

	void EnvironmentPrefilter::MipRenderCube::registerFrames()
	{
		auto & cmd = *m_commands.commandBuffer;
		cmd.begin();
		cmd.beginDebugBlock( { "Prefiltering " + m_prefix + " Environment map"
			, makeFloatArray( m_device.renderSystem.getEngine()->getNextRainbowColour() ) } );

		for ( uint32_t face = 0u; face < 6u; ++face )
		{
			auto & frameBuffer = m_frameBuffers[face];
			cmd.beginRenderPass( m_renderPass
				, *frameBuffer.frameBuffer
				, { transparentBlackClearColor }
				, VK_SUBPASS_CONTENTS_INLINE );
			registerFrame( *m_commands.commandBuffer, face );
			cmd.endRenderPass();
		}

		cmd.endDebugBlock();
		cmd.end();
	}

	void EnvironmentPrefilter::MipRenderCube::render( QueueData const & queueData )
	{
		m_commands.submit( *queueData.queue );
	}

	crg::SemaphoreWaitArray EnvironmentPrefilter::MipRenderCube::render( crg::SemaphoreWaitArray signalsToWait
		, ashes::Queue const & queue )const
	{
		return { 1u
			, { m_commands.submit( queue, signalsToWait )
				, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT } };
	}

	//*********************************************************************************************

	EnvironmentPrefilter::EnvironmentPrefilter( Engine & engine
		, RenderDevice const & device
		, castor::Size const & size
		, Texture const & srcTexture
		, SamplerResPtr sampler
		, bool isCharlie )
		: m_device{ device }
		, m_srcView{ srcTexture }
		, m_prefix{ isCharlie ? std::string{ "Sheen" } : std::string{} }
		, m_srcImage{ std::make_unique< ashes::Image >( *device, m_srcView.image, m_srcView.imageId.data->info ) }
		, m_srcImageView{ m_srcImage->createView( "EnvironmentPrefilterSrc", VK_IMAGE_VIEW_TYPE_CUBE, srcTexture.getFormat(), 0u, m_srcView.getMipLevels(), 0u, 6u ) }
		, m_result{ envpref::doCreatePrefilteredTexture( m_device, engine.getGraphResourceHandler(), size, m_prefix ) }
		, m_sampler{ envpref::doCreateSampler( engine, m_device, m_prefix, m_result.getMipLevels() - 1u ) }
		, m_renderPass{ envpref::doCreateRenderPass( m_device, m_prefix, m_result.getFormat() ) }
	{
		VkExtent2D originalSize{ size.getWidth(), size.getHeight() };
		auto data = m_device.graphicsData();

		for ( auto mipLevel = 0u; mipLevel < MaxIblReflectionLod + 1u; ++mipLevel )
		{
			VkExtent2D mipSize{ originalSize.width >> mipLevel
				, originalSize.height >> mipLevel };
			m_renderPasses.emplace_back( std::make_unique< MipRenderCube >( m_device
				, *data
				, engine.getGraphResourceHandler()
				, *m_renderPass
				, mipLevel
				, originalSize
				, mipSize
				, m_srcImageView
				, m_result
				, sampler
				, isCharlie ) );
		}

		for ( auto & cubePass : m_renderPasses )
		{
			cubePass->registerFrames();
		}
	}

	EnvironmentPrefilter::~EnvironmentPrefilter()
	{
		m_result.destroy();
	}

	void EnvironmentPrefilter::render( QueueData const & queueData )
	{
		for ( auto & cubePass : m_renderPasses )
		{
			cubePass->render( queueData );
		}
	}

	crg::SemaphoreWaitArray EnvironmentPrefilter::render( crg::SemaphoreWaitArray signalsToWait
		, ashes::Queue const & queue )const
	{
		for ( auto & cubePass : m_renderPasses )
		{
			signalsToWait = cubePass->render( signalsToWait, queue );
		}

		return signalsToWait;
	}

	//*********************************************************************************************
}
