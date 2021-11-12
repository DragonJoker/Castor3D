#include "LightStreaksPostEffect/HiPass.hpp"

#include "LightStreaksPostEffect/LightStreaksPostEffect.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderDevice.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Shader/Program.hpp>

#include <CastorUtils/Graphics/Image.hpp>

#include <ShaderWriter/Source.hpp>

#include <RenderGraph/RunnablePasses/ImageCopy.hpp>
#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <numeric>

using namespace castor;

namespace light_streaks
{
	namespace
	{
		template< typename T >
		inline constexpr T getSubresourceDimension( T const & extent
			, uint32_t mipLevel )noexcept
		{
			return std::max( T( 1 ), T( extent >> mipLevel ) );
		}

		class HiPassQuad
			: public crg::RenderQuad
		{
		public:
			HiPassQuad( crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph
				, crg::VkPipelineShaderStageCreateInfoArray program
				, VkExtent2D const & renderSize
				, bool const * enabled )
				: crg::RenderQuad{ pass
					, context
					, graph
					, 1u
					, crg::rq::Config{}
						.baseConfig( { std::vector< crg::VkPipelineShaderStageCreateInfoArray >{ std::move( program ) }, {} } )
						.texcoordConfig( crg::Texcoord{} )
						.enabled( enabled )
						.recordInto( [this]( VkCommandBuffer cb, uint32_t i ){ doRecordInto( cb, i ); } )
						.renderSize( renderSize ) }
				, m_viewDesc{ pass.images.back().view() }
				, m_imageDesc{ m_viewDesc.data->image }
				, m_image{ graph.createImage( m_imageDesc ) }
			{
				auto const imageViewType = VkImageViewType( m_imageDesc.data->info.imageType );
				crg::ImageViewData viewData{ m_imageDesc.data->name
					, m_imageDesc
					, 0u
					, imageViewType
					, m_imageDesc.data->info.format
					, { m_viewDesc.data->info.subresourceRange.aspectMask, 0u, 1u, 0u, 1u } };

				viewData.info.subresourceRange.baseArrayLayer = m_viewDesc.data->info.subresourceRange.baseArrayLayer;

				for ( uint32_t layerIdx = 1u; layerIdx < m_imageDesc.data->info.arrayLayers; ++layerIdx )
				{
					viewData.info.subresourceRange.baseArrayLayer = m_viewDesc.data->info.subresourceRange.baseArrayLayer + layerIdx;
					m_copyDestinations.push_back( m_graph.createView( viewData ) );
				}
			}

		private:
			void doRecordInto( VkCommandBuffer commandBuffer
				, uint32_t index )
			{
				auto const aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				auto transition = getTransition( index, m_viewDesc );

				VkImageCopy imageCopy{};

				imageCopy.srcSubresource.aspectMask = aspectMask;
				imageCopy.srcSubresource.mipLevel = 0u;
				imageCopy.srcSubresource.baseArrayLayer = m_viewDesc.data->info.subresourceRange.baseArrayLayer;
				imageCopy.srcSubresource.layerCount = 1u;
				imageCopy.srcOffset = {};
				imageCopy.extent = m_imageDesc.data->info.extent;

				imageCopy.dstSubresource.aspectMask = aspectMask;
				imageCopy.dstSubresource.mipLevel = 0u;
				imageCopy.dstSubresource.layerCount = 1u;
				imageCopy.dstOffset = {};

				// Transition source layer to transfer src layout
				m_graph.memoryBarrier( commandBuffer
					, m_viewDesc
					, transition.to
					, { VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
						, crg::getAccessMask( VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL )
						, crg::getStageMask( VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL ) } );

				for ( auto & dst : m_copyDestinations )
				{
					imageCopy.dstSubresource.baseArrayLayer = dst.data->info.subresourceRange.baseArrayLayer;

					// Transition destination layer to transfer dst layout
					m_graph.memoryBarrier( commandBuffer
						, dst
						, { VK_IMAGE_LAYOUT_UNDEFINED
							, crg::getAccessMask( VK_IMAGE_LAYOUT_UNDEFINED )
							, crg::getStageMask( VK_IMAGE_LAYOUT_UNDEFINED ) }
						, { VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
							, crg::getAccessMask( VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL )
							, crg::getStageMask( VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ) } );

					// Perform blit
					m_context.vkCmdCopyImage( commandBuffer
						, m_image
						, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
						, m_image
						, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
						, 1u
						, &imageCopy );

					// Transition destination layer to wanted output layout
					m_graph.memoryBarrier( commandBuffer
						, dst
						, { VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
							, crg::getAccessMask( VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL )
							, crg::getStageMask( VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ) }
						, transition.to );
				}

				// Transition source layer to wanted output layout
				m_graph.memoryBarrier( commandBuffer
					, m_viewDesc
					, { VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
						, crg::getAccessMask( VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL )
						, crg::getStageMask( VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL ) }
					, transition.to );
			}

		private:
			crg::ImageViewId m_viewDesc;
			crg::ImageId m_imageDesc;
			VkImage m_image{};
			crg::ImageViewIdArray m_copyDestinations;
		};

		std::unique_ptr< ast::Shader > getVertexProgram()
		{
			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			Vec2 position = writer.declInput< Vec2 >( "position", 0u );
			Vec2 uv = writer.declInput< Vec2 >( "uv", 1u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( "vtx_texture", 0u );

			writer.implementMainT< VoidT, VoidT >( [&]( VertexIn in
				, VertexOut out )
				{
					vtx_texture = uv;
					out.vtx.position = vec4( position, 0.0_f, 1.0_f );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		std::unique_ptr< ast::Shader > getPixelProgram()
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			auto c3d_mapColor = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapColor", 0u, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_fragColor", 0 );

			writer.implementMainT< VoidT, VoidT >( [&]( FragmentIn in
				, FragmentOut out )
				{
					pxl_fragColor = vec4( c3d_mapColor.sample( vtx_texture, 0.0_f ).xyz(), 1.0_f );
					auto maxComponent = writer.declLocale( "maxComponent"
						, max( pxl_fragColor.r(), pxl_fragColor.g() ) );
					maxComponent = max( maxComponent, pxl_fragColor.b() );

					IF( writer, maxComponent > 1.0_f )
					{
						pxl_fragColor.xyz() /= maxComponent;
					}
					ELSE
					{
						pxl_fragColor.xyz() = vec3( 0.0_f, 0.0_f, 0.0_f );
					}
					FI;
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
	}

	//*********************************************************************************************

	HiPass::HiPass( crg::FrameGraph & graph
		, crg::FramePass const & previousPass
		, castor3d::RenderDevice const & device
		, crg::ImageViewId const & sceneView
		, crg::ImageViewIdArray const & resultViews
		, VkExtent2D size
		, bool const * enabled )
		: m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, "LightStreaksHiPass", getVertexProgram() }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, "LightStreaksHiPass", getPixelProgram() }
		, m_stages{ makeShaderState( device, m_vertexShader )
			, makeShaderState( device, m_pixelShader ) }
		, m_lastPass{ &previousPass }
	{
		auto & hiPass = graph.createPass( "LightStreaksHiPass"
			, [this, &device, size, enabled]( crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = std::make_unique< HiPassQuad >( pass
					, context
					, graph
					, ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages )
					, size
					, enabled );
				device.renderSystem.getEngine()->registerTimer( graph.getName() + "/LightStreaks"
					, result->getTimer() );
				return result;
			} );
		crg::SamplerDesc linearSampler{ VK_FILTER_LINEAR
			, VK_FILTER_LINEAR
			, VK_SAMPLER_MIPMAP_MODE_NEAREST
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE };
		hiPass.addDependency( *m_lastPass );
		hiPass.addSampledView( sceneView
			, 0u
			, VK_IMAGE_LAYOUT_UNDEFINED
			, linearSampler );
		hiPass.addOutputColourView( resultViews[0] );
		m_lastPass = &hiPass;

		for ( uint32_t i = 1u; i < resultViews.size(); ++i )
		{
			auto & pass = graph.createPass( "LightStreaksCopy" + std::to_string( i )
				, [&device, size, enabled]( crg::FramePass const & pass
					, crg::GraphContext & context
					, crg::RunnableGraph & graph )
				{
					auto result = std::make_unique< crg::ImageCopy >( pass
						, context
						, graph
						, VkExtent3D{ size.width, size.height, 1u }
						, 1u
						, false
						, nullptr
						, enabled );
					device.renderSystem.getEngine()->registerTimer( graph.getName() + "/LightStreaks"
						, result->getTimer() );
					return result;
				} );
			pass.addDependency( *m_lastPass );
			pass.addTransferInputView( resultViews[i - 1u]
				, VK_IMAGE_LAYOUT_UNDEFINED );
			pass.addTransferOutputView( resultViews[i]
				, VK_IMAGE_LAYOUT_UNDEFINED );
			m_lastPass = &pass;
		}
	}

	void HiPass::accept( castor3d::PipelineVisitorBase & visitor )
	{
		visitor.visit( m_vertexShader );
		visitor.visit( m_pixelShader );
	}
}
