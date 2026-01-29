#include "Castor3D/Render/ToTexture/EquirectangularToCube.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/ToTexture/RenderCube.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Shaders/GlslBaseIO.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"

#include <CastorUtils/Graphics/Position.hpp>

#include <ashespp/Sync/Queue.hpp>
#include <ashespp/Core/Device.hpp>
#include <ashespp/Image/Image.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

namespace c3d
{
	//*********************************************************************************************

	namespace equitocube
	{
		static ashes::PipelineShaderStageCreateInfoArray createProgram( RenderDevice const & device )
		{
			auto & engine = c3d::getEngine( device );
			ProgramModule programModule{ cuT( "EquirectangularToCube" ) };
			{
				sdw::TraditionalGraphicsWriter writer{ &engine.getShaderAllocator() };

				shader::Utils utils{ writer };

				auto matrixUbo = sdw::UniformBuffer{ writer, "Matrix", 0u, 0u };
				auto mtxViewProjection = matrixUbo.declMember< sdw::Mat4 >( "mtxViewProjection" );
				matrixUbo.end();
				auto mapColour = writer.declCombinedImg< Img2DRgba >( "mapColour", 1u, 0u );

				auto sampleSphericalMap = [&writer]( sdw::Vec3 const & v )
				{
					auto uv = writer.declLocale( "uv"
						, vec2( atan2( v.z(), v.x() ), asin( v.y() ) ) );
					uv *= vec2( 0.1591_f, 0.3183_f );
					uv += 0.5_f;
					return uv;
				};

				writer.implementEntryPointT< shader::Position4FT, shader::Position3FT>( [&mtxViewProjection]( sdw::VertexInT< shader::Position4FT > const & in
					, sdw::VertexOutT< shader::Position3FT > out )
					{
						out.position() = in.position().xyz();
						out.vtx.position = mtxViewProjection * in.position();
					} );

				writer.implementEntryPointT< shader::Position3FT, shader::Colour4FT >( [&]( sdw::FragmentInT< shader::Position3FT > const & in
					, sdw::FragmentOutT< shader::Colour4FT > const & out )
					{
						auto normPos = writer.declLocale( "normPos"
							, normalize( in.position() ) );
						auto uv = sampleSphericalMap( normPos );
						out.colour() = vec4( mapColour.sample( utils.topDownToBottomUp( uv ) ).rgb(), 1.0_f );
					} );
				programModule.shader = writer.getBuilder().releaseShader();
			}

			return makeProgramStates( device, programModule );
		}

		static ashes::RenderPassPtr createRenderPass( RenderDevice const & device
			, PixelFormat format )
		{
			ashes::VkAttachmentDescriptionArray attaches
			{
				{
					0u,
					convert( format ),
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
				c3d::move( attaches ),
				c3d::move( subpasses ),
				c3d::move( dependencies ),
			};
			return device->createRenderPass( "EquirectangularToCube"
				, c3d::move( createInfo ) );
		}

		class EquirectangularToCube
			: private RenderCube
		{
		public:
			EquirectangularToCube( TextureLayout const & equiRectangular
				, RenderDevice const & device
				, TextureLayout const & target )
				: RenderCube{ device, false }
				, m_commandBuffer{ device.graphicsData()->commandPool->createCommandBuffer( "EquirectangularToCube" ) }
				, m_view{ equiRectangular.getDefaultSampledView() }
				, m_renderPass{ createRenderPass( m_device, target.getPixelFormat() ) }
			{
				auto size = Extent2D{ target.getWidth(), target.getHeight() };
				auto program = createProgram( device );
				uint32_t face = 0u;

				for ( auto & facePipeline : m_frameBuffers )
				{
					ashes::ImageViewCRefArray attaches;
					facePipeline.view = target.getTexture().createView( "EquirectangularToCube" + string::toMbString( face )
						, VK_IMAGE_VIEW_TYPE_2D
						, convert( target.getPixelFormat() )
						, 0u
						, 1u
						, face
						, 1u );
					attaches.emplace_back( facePipeline.view );
					facePipeline.frameBuffer = m_renderPass->createFrameBuffer( "EquirectangularToCube" + string::toMbString( face )
						, convert( size )
						, c3d::move( attaches ) );
					++face;
				}

				createPipelines( size
					, program
					, m_view
					, *m_renderPass
					, {} );
			}

			void render( QueueData const & queueData )
			{
				CU_Require( !m_frameBuffers.empty() );
				uint32_t face = 0u;
				m_commandBuffer->begin( VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT );
				m_commandBuffer->beginDebugBlock( { "Equirectangular to cube"
					, makeFloatArray( c3d::getEngine( m_device ).getNextRainbowColour() ) } );

				for ( auto const & frameBuffer : m_frameBuffers )
				{
					m_commandBuffer->beginRenderPass( *m_renderPass
						, *frameBuffer.frameBuffer
						, { convert( ClearValue{ transparentBlackClearColor } ) }
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

		private:
			struct FrameBuffer
			{
				ashes::ImageView view;
				ashes::FrameBufferPtr frameBuffer;
			};
			Array< FrameBuffer, 6u > m_frameBuffers;
			ashes::CommandBufferPtr m_commandBuffer;
			ashes::ImageView const & m_view;
			ashes::RenderPassPtr m_renderPass;
		};
	}

	//*********************************************************************************************

	void transformEquirectangularToCube( TextureLayout const & equiRectangularSource
		, TextureLayout const & cubeTarget
		, RenderDevice const & device
		, QueueData const & queueData )
	{
		equitocube::EquirectangularToCube equiToCube{ equiRectangularSource, device, cubeTarget };
		equiToCube.render( queueData );
	}

	//*********************************************************************************************
}
