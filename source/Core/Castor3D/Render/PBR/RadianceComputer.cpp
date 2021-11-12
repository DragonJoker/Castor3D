#include "Castor3D/Render/PBR/RadianceComputer.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Miscellaneous/DebugName.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/Graphics/Size.hpp>
#include <CastorUtils/Math/Angle.hpp>

#include <ashespp/Image/Image.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>
#include <ashespp/Sync/Fence.hpp>

#include <ShaderWriter/Source.hpp>

#include <RenderGraph/ResourceHandler.hpp>

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		Texture doCreateRadianceTexture( RenderDevice const & device
			, crg::ResourceHandler & handler
			, Size const & size )
		{
			Texture result{ device
				, handler
				, "RadianceComputerResult"
				, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT
				, { size[0], size[1], 1u }
				, 6u
				, 1u
				, VK_FORMAT_R32G32B32A32_SFLOAT
				, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT };
			result.create();
			return result;
		}

		SamplerResPtr doCreateSampler( Engine & engine
			, RenderDevice const & device )
		{
			auto name = cuT( "IblTexturesRadiance" );
			auto result = engine.getSamplerCache().tryFind( name );

			if ( !result.lock() )
			{
				auto created = engine.getSamplerCache().create( name, engine );
				created->setMinFilter( VK_FILTER_LINEAR );
				created->setMagFilter( VK_FILTER_LINEAR );
				created->setWrapS( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
				created->setWrapT( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
				created->setWrapR( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
				result = engine.getSamplerCache().add( name, created, false );
			}

			result.lock()->initialise( engine.getRenderSystem()->getRenderDevice() );
			return result;
		}

		ashes::ImageView doCreateSrcView( ashes::Image const & texture )
		{
			return texture.createView( VK_IMAGE_VIEW_TYPE_CUBE
				, texture.getFormat()
				, 0u
				, 1u
				, 0u
				, 6u );
		}

		ashes::PipelineShaderStageCreateInfoArray doCreateProgram( RenderDevice const & device )
		{
			ShaderModule vtx{ VK_SHADER_STAGE_VERTEX_BIT, "RadianceCompute" };
			{
				using namespace sdw;
				VertexWriter writer;

				// Inputs
				auto position = writer.declInput< Vec3 >( "position", 0u );
				Ubo matrix{ writer, "Matrix", 0u, 0u };
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

			ShaderModule pxl{ VK_SHADER_STAGE_FRAGMENT_BIT, "RadianceCompute" };
			{
				using namespace sdw;
				FragmentWriter writer;

				// Inputs
				auto vtx_worldPosition = writer.declInput< Vec3 >( "vtx_worldPosition", 0u );
				auto c3d_mapEnvironment = writer.declSampledImage< FImgCubeRgba32 >( "c3d_mapEnvironment", 1u, 0u );

				// Outputs
				auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_FragColor", 0u );

				writer.implementMainT< VoidT, VoidT >( [&]( FragmentIn in
					, FragmentOut out )
					{
						// From https://learnopengl.com/#!PBR/Lighting
						// the sample direction equals the hemisphere's orientation 
						auto normal = writer.declLocale( "normal"
							, normalize( vtx_worldPosition ) );

						auto irradiance = writer.declLocale( "irradiance"
							, vec3( 0.0_f ) );

						auto up = writer.declLocale( "up"
							, vec3( 0.0_f, 1.0_f, 0.0_f ) );
						auto right = writer.declLocale( "right"
							, cross( up, normal ) );
						up = cross( normal, right );

						auto sampleDelta = writer.declLocale( "sampleDelta"
							, 0.025_f );
						auto nrSamples = writer.declLocale( "nrSamples"
							, 0_i );

						FOR( writer, Float, phi, 0.0_f, phi < Float{ PiMult2< float > }, phi += sampleDelta )
						{
							FOR( writer, Float, theta, 0.0_f, theta < Float{ PiDiv2< float > }, theta += sampleDelta )
							{
								// spherical to cartesian (in tangent space)
								auto tangentSample = writer.declLocale( "tangentSample"
									, vec3( sin( theta ) * cos( phi ), sin( theta ) * sin( phi ), cos( theta ) ) );
								// tangent space to world
								auto sampleVec = writer.declLocale( "sampleVec"
									, right * tangentSample.x() + up * tangentSample.y() + normal * tangentSample.z() );

								irradiance += c3d_mapEnvironment.sample( sampleVec ).rgb() * cos( theta ) * sin( theta );
								nrSamples = nrSamples + 1;
							}
							ROF;
						}
						ROF;

						irradiance = irradiance * Float{ Pi< float > } * ( 1.0_f / writer.cast< Float >( nrSamples ) );
						pxl_fragColor = vec4( irradiance, 1.0_f );
					} );

				pxl.shader = std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
			}

			return ashes::PipelineShaderStageCreateInfoArray
			{
				makeShaderState( device, vtx ),
				makeShaderState( device, pxl ),
			};
		}

		ashes::RenderPassPtr doCreateRenderPass( RenderDevice const & device
			, VkFormat format )
		{
			ashes::VkAttachmentDescriptionArray attaches
			{
				{
					0u,
					format,
					VK_SAMPLE_COUNT_1_BIT,
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,
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
			auto result = device->createRenderPass( "RadianceComputer"
				, std::move( createInfo ) );
			return result;
		}
	}

	//*********************************************************************************************

	RadianceComputer::RadianceComputer( Engine & engine
		, RenderDevice const & device
		, Size const & size
		, Texture const & srcTexture )
		: RenderCube{ device, false }
		, m_result{ doCreateRadianceTexture( m_device, engine.getGraphResourceHandler(), size ) }
		, m_sampler{ doCreateSampler( engine, m_device ) }
		, m_srcView{ srcTexture }
		, m_srcImage{ std::make_unique< ashes::Image >( *device, m_srcView.image, m_srcView.imageId.data->info ) }
		, m_srcImageView{ doCreateSrcView( *m_srcImage ) }
		, m_renderPass{ doCreateRenderPass( m_device, m_result.getFormat() ) }
		, m_commands{ m_device, *m_device.graphicsData(), "RadianceComputer" }
	{
		auto & handler = engine.getGraphResourceHandler();
		auto & dstTexture = m_result;
		
		for ( auto face = 0u; face < 6u; ++face )
		{
			auto & facePass = m_renderPasses[face];
			auto name = "RadianceComputer" + string::toString( face );
			// Create the views.
			facePass.dstView = handler.createImageView( device.makeContext()
				, dstTexture.subViewsId[face] );
			// Initialise the frame buffer.
			VkFramebufferCreateInfo createInfo{ VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO
				, nullptr
				, 0u
				, *m_renderPass
				, 1u
				, &facePass.dstView
				, size.getWidth()
				, size.getHeight()
				, 1u };
			facePass.frameBuffer = m_renderPass->createFrameBuffer( name
				, std::move( createInfo ) );
		}

		auto program = doCreateProgram( m_device );
		createPipelines( { size.getWidth(), size.getHeight() }
			, Position{}
			, program
			, m_srcImageView
			, *m_renderPass
			, {} );

		auto & cmd = *m_commands.commandBuffer;
		cmd.begin();
		cmd.beginDebugBlock( { "Generating irradiance map"
			, makeFloatArray( m_device.renderSystem.getEngine()->getNextRainbowColour() ) } );

		for ( auto face = 0u; face < 6u; ++face )
		{
			auto & facePass = m_renderPasses[face];
			cmd.beginRenderPass( *m_renderPass
				, *facePass.frameBuffer
				, { transparentBlackClearColor }
				, VK_SUBPASS_CONTENTS_INLINE );
			registerFrame( *m_commands.commandBuffer, face );
			cmd.endRenderPass();
		}

		cmd.endDebugBlock();
		cmd.end();
	}

	void RadianceComputer::render( QueueData const & queueData )
	{
		m_commands.submit( *queueData.queue );
	}

	ashes::Semaphore const & RadianceComputer::render( QueueData const & queueData
		, ashes::Semaphore const & toWait )
	{
		return m_commands.submit( *queueData.queue, toWait );
	}

	ashes::Sampler const & RadianceComputer::getSampler()const
	{
		return m_sampler.lock()->getSampler();
	}

	//*********************************************************************************************
}
