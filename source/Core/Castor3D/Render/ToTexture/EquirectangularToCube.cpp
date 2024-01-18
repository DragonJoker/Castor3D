#include "Castor3D/Render/ToTexture/EquirectangularToCube.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Shaders/GlslBaseIO.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"

#include <CastorUtils/Graphics/Position.hpp>
#include <CastorUtils/Math/TransformationMatrix.hpp>

#include <ashespp/Sync/Queue.hpp>
#include <ashespp/Core/Device.hpp>
#include <ashespp/Image/Image.hpp>
#include <ashespp/Pipeline/PipelineDepthStencilStateCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineInputAssemblyStateCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineMultisampleStateCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineVertexInputStateCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineViewportStateCreateInfo.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>
#include <ashespp/Sync/Fence.hpp>

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

CU_ImplementSmartPtr( castor3d, EquirectangularToCube )

namespace castor3d
{
	//*********************************************************************************************

	namespace equitocube
	{
		static ashes::PipelineShaderStageCreateInfoArray doCreateProgram( RenderDevice const & device
			, VkFormat format )
		{
			auto & engine = *device.renderSystem.getEngine();
			ProgramModule module{ "EquirectangularToCube" };
			{
				sdw::TraditionalGraphicsWriter writer{ &engine.getShaderAllocator() };

				shader::Utils utils{ writer };

				auto matrixUbo = sdw::UniformBuffer{ writer, "Matrix", 0u, 0u };
				auto mtxViewProjection = matrixUbo.declMember< sdw::Mat4 >( "mtxViewProjection" );
				matrixUbo.end();
				auto mapColour = writer.declCombinedImg< FImg2DRgba32 >( "mapColour", 1u, 0u );

				auto sampleSphericalMap = writer.implementFunction< sdw::Vec2 >( "sampleSphericalMap"
					, [&]( sdw::Vec3 const & v )
					{
						auto uv = writer.declLocale( "uv"
							, vec2( atan2( v.z(), v.x() ), asin( v.y() ) ) );
						uv *= vec2( 0.1591_f, 0.3183_f );
						uv += 0.5_f;
						writer.returnStmt( uv );
					}
				, sdw::InVec3{ writer, "v" } );

				writer.implementEntryPointT< shader::Position4FT, shader::Position3FT>( [&]( sdw::VertexInT< shader::Position4FT > in
					, sdw::VertexOutT< shader::Position3FT > out )
					{
						out.position() = in.position().xyz();
						out.vtx.position = mtxViewProjection * in.position();
					} );

				writer.implementEntryPointT< shader::Position3FT, shader::Colour4FT >( [&]( sdw::FragmentInT< shader::Position3FT > in
					, sdw::FragmentOutT< shader::Colour4FT > out )
					{
						auto uv = writer.declLocale( "uv"
							, sampleSphericalMap( normalize( in.position() ) ) );
						out.colour() = vec4( mapColour.sample( utils.topDownToBottomUp( uv ) ).rgb(), 1.0_f );
					} );
				module.shader = writer.getBuilder().releaseShader();
			}

			return makeProgramStates( device, module );
		}

		static ashes::RenderPassPtr doCreateRenderPass( RenderDevice const & device
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
					VK_SUBPASS_EXTERNAL,
					0u,
					VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
					VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
					VK_ACCESS_SHADER_READ_BIT,
					VK_DEPENDENCY_BY_REGION_BIT,
				},
				{
					0u,
					VK_SUBPASS_EXTERNAL,
					VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
					VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
					VK_ACCESS_SHADER_READ_BIT,
					VK_DEPENDENCY_BY_REGION_BIT,
				}
			};
			ashes::RenderPassCreateInfo createInfo
			{
				0u,
				std::move( attaches ),
				std::move( subpasses ),
				std::move( dependencies ),
			};
			return device->createRenderPass( "EquirectangularToCube"
				, std::move( createInfo ) );
		}
	}

	//*********************************************************************************************

	EquirectangularToCube::EquirectangularToCube( TextureLayout const & equiRectangular
		, RenderDevice const & device
		, TextureLayout const & target )
		: RenderCube{ device, false }
		, m_commandBuffer{ device.graphicsData()->commandPool->createCommandBuffer( "EquirectangularToCube" ) }
		, m_view{ equiRectangular.getDefaultSampledView() }
		, m_renderPass{ equitocube::doCreateRenderPass( m_device, target.getPixelFormat() ) }
	{
		auto size = VkExtent2D{ target.getWidth(), target.getHeight() };
		auto program = equitocube::doCreateProgram( device, equiRectangular.getPixelFormat() );
		uint32_t face = 0u;

		for ( auto & facePipeline : m_frameBuffers )
		{
			ashes::ImageViewCRefArray attaches;
			facePipeline.view = target.getTexture().createView( "EquirectangularToCube" + castor::string::toString( face )
				, VK_IMAGE_VIEW_TYPE_2D
				, target.getPixelFormat()
				, 0u
				, 1u
				, face
				, 1u );
			attaches.emplace_back( facePipeline.view );
			facePipeline.frameBuffer = m_renderPass->createFrameBuffer( "EquirectangularToCube"
				, size
				, std::move( attaches ) );
			++face;
		}

		createPipelines( size
			, castor::Position{}
			, program
			, m_view
			, *m_renderPass
			, {} );
	}

	void EquirectangularToCube::render( QueueData const & queueData )
	{
		CU_Require( !m_frameBuffers.empty() );
		uint32_t face = 0u;
		m_commandBuffer->begin( VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT );
		m_commandBuffer->beginDebugBlock(
			{
				"Equirectangular to cube",
				makeFloatArray( m_device.renderSystem.getEngine()->getNextRainbowColour() ),
			} );

		for ( auto & frameBuffer : m_frameBuffers )
		{

			m_commandBuffer->beginRenderPass( *m_renderPass
				, *frameBuffer.frameBuffer
				, { transparentBlackClearColor }
			, VK_SUBPASS_CONTENTS_INLINE );
			registerFrame( *m_commandBuffer, face );
			m_commandBuffer->endRenderPass();

			++face;
		}

		m_commandBuffer->endDebugBlock();
		m_commandBuffer->end();

		queueData.queue->submit( *m_commandBuffer, nullptr );
		queueData.queue->waitIdle();
	}

	//*********************************************************************************************
}
