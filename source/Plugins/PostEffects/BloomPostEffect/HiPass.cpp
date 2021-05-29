#include "BloomPostEffect/HiPass.hpp"

#include "BloomPostEffect/BloomPostEffect.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Shader/Program.hpp>

#include <CastorUtils/Graphics/Image.hpp>

#include <ShaderWriter/Source.hpp>

#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <numeric>

using namespace castor;

namespace Bloom
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
				, crg::GraphContext const & context
				, crg::RunnableGraph & graph
				, crg::VkPipelineShaderStageCreateInfoArray program
				, VkExtent2D const & renderSize )
				: crg::RenderQuad{ pass
					, context
					, graph
					, 1u
					, crg::rq::Config{ std::move( program )
						, crg::rq::Texcoord{}
						, renderSize
						, VkOffset2D{} } }
#if !Bloom_DebugHiPass
				, m_viewDesc{ pass.colourInOuts.front().view() }
				, m_imageDesc{ m_viewDesc.data->image }
				, m_image{ graph.getImage( m_imageDesc ) }
#endif
			{
#if !Bloom_DebugHiPass
				auto const imageViewType = VkImageViewType( m_imageDesc.data->info.imageType );
				crg::ImageViewData viewData{ m_imageDesc.data->name
					, m_imageDesc
					, 0u
					, imageViewType
					, m_imageDesc.data->info.format
					, { m_viewDesc.data->info.subresourceRange.aspectMask, 0u, 1u, 0u, 1u } };

				viewData.info.subresourceRange.baseMipLevel = m_viewDesc.data->info.subresourceRange.baseMipLevel;
				crg::ImageViewId prevLevelView = m_graph.createView( viewData );

				for ( uint32_t levelIdx = 1u; levelIdx < m_imageDesc.data->info.mipLevels; ++levelIdx )
				{
					viewData.info.subresourceRange.baseMipLevel = m_viewDesc.data->info.subresourceRange.baseMipLevel + levelIdx;
					crg::ImageViewId currLevelView = m_graph.createView( viewData );
					m_mipGens.push_back( { prevLevelView, currLevelView } );
					prevLevelView = currLevelView;
				}
#endif
			}

		private:
			void doRecordInto( VkCommandBuffer commandBuffer
				, uint32_t index )override
			{
				crg::RenderQuad::doRecordInto( commandBuffer, index );

#if !Bloom_DebugHiPass
				auto const width = int32_t( m_imageDesc.data->info.extent.width );
				auto const height = int32_t( m_imageDesc.data->info.extent.height );
				auto const depth = int32_t( m_imageDesc.data->info.extent.depth );
				auto const imageViewType = VkImageViewType( m_imageDesc.data->info.imageType );
				auto const aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				auto transition = doGetTransition( m_viewDesc );

				// Transition source view to transfer src layout
				m_graph.memoryBarrier( commandBuffer
					, m_viewDesc
					, transition.toLayout
					, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL );

				for ( auto & mipGen : m_mipGens )
				{
					VkImageBlit imageBlit{};

					imageBlit.srcSubresource.aspectMask = aspectMask;
					imageBlit.srcSubresource.mipLevel = mipGen.src.data->info.subresourceRange.baseMipLevel;
					imageBlit.srcSubresource.baseArrayLayer = 0u;
					imageBlit.srcSubresource.layerCount = 1;

					imageBlit.srcOffsets[0].x = 0;
					imageBlit.srcOffsets[0].y = 0;
					imageBlit.srcOffsets[0].z = 0;
					imageBlit.srcOffsets[1].x = getSubresourceDimension( width, imageBlit.srcSubresource.mipLevel );
					imageBlit.srcOffsets[1].y = getSubresourceDimension( height, imageBlit.srcSubresource.mipLevel );
					imageBlit.srcOffsets[1].z = getSubresourceDimension( depth, imageBlit.srcSubresource.mipLevel );

					imageBlit.dstSubresource.aspectMask = aspectMask;
					imageBlit.dstSubresource.mipLevel = mipGen.dst.data->info.subresourceRange.baseMipLevel;
					imageBlit.dstSubresource.baseArrayLayer = 0u;
					imageBlit.dstSubresource.layerCount = 1;

					imageBlit.dstOffsets[0].x = 0;
					imageBlit.dstOffsets[0].y = 0;
					imageBlit.dstOffsets[0].z = 0;
					imageBlit.dstOffsets[1].x = getSubresourceDimension( width, imageBlit.dstSubresource.mipLevel );
					imageBlit.dstOffsets[1].y = getSubresourceDimension( height, imageBlit.dstSubresource.mipLevel );
					imageBlit.dstOffsets[1].z = getSubresourceDimension( depth, imageBlit.dstSubresource.mipLevel );

					// Transition destination mip level to transfer dst layout
					m_graph.memoryBarrier( commandBuffer
						, mipGen.dst
						, VK_IMAGE_LAYOUT_UNDEFINED
						, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL );

					// Perform blit
					m_context.vkCmdBlitImage( commandBuffer
						, m_image
						, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
						, m_image
						, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
						, 1u
						, &imageBlit
						, VK_FILTER_LINEAR );

					// Transition destination mip level to transfer src layout
					m_graph.memoryBarrier( commandBuffer
						, mipGen.dst
						, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
						, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL );

					// Transition source mip level to wanted output layout
					m_graph.memoryBarrier( commandBuffer
						, mipGen.src
						, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
						, transition.toLayout );
				}

				// Transition last destination mip level to wanted output layout
				auto & mipGen = m_mipGens.back();
				m_graph.memoryBarrier( commandBuffer
					, mipGen.dst
					, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
					, transition.toLayout );
#endif
			}

#if !Bloom_DebugHiPass
		private:
			struct LevelMipGen
			{
				crg::ImageViewId src;
				crg::ImageViewId dst;
			};
			crg::ImageViewId m_viewDesc;
			crg::ImageId m_imageDesc;
			VkImage m_image{};
			std::vector< LevelMipGen > m_mipGens;
#endif
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
			auto out = writer.getOut();

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
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

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
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
		, VkExtent2D size
		, uint32_t blurPassesCount )
		: m_sceneView{ sceneView }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, "BloomHiPass", getVertexProgram() }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, "BloomHiPass", getPixelProgram() }
		, m_stages{ makeShaderState( device, m_vertexShader )
			, makeShaderState( device, m_pixelShader ) }
#if !Bloom_DebugHiPass
		, m_resultImg{ graph.createImage( crg::ImageData{ "BLHi"
			, 0u
			, VK_IMAGE_TYPE_2D
			, sceneView.data->info.format
			, VkExtent3D{ size.width >> 1, size.height >> 1, 1u }
			, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_TRANSFER_SRC_BIT
				| VK_IMAGE_USAGE_TRANSFER_DST_BIT )
			, blurPassesCount } ) }
#else
		, m_resultImg{ graph.createImage( crg::ImageData{ "BLHi"
			, 0u
			, VK_IMAGE_TYPE_2D
			, sceneView.data->info.format
			, VkExtent3D{ size.width, size.height, 1u }
			, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_TRANSFER_SRC_BIT ) } ) }
#endif
		, m_pass{ graph.createPass( "BloomHiPass"
			, [this, size]( crg::FramePass const & pass
				, crg::GraphContext const & context
				, crg::RunnableGraph & graph )
			{
				return std::make_unique< HiPassQuad >( pass
					, context
					, graph
					, ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages )
					, VkExtent2D{ m_resultImg.data->info.extent.width, m_resultImg.data->info.extent.height } );
			} ) }
	{
		for ( uint32_t i = 0u; i < blurPassesCount; ++i )
		{
			m_resultViews.push_back( graph.createView( crg::ImageViewData{ m_resultImg.data->name + castor::string::toString( i )
				, m_resultImg
				, 0u
				, VK_IMAGE_VIEW_TYPE_2D
				, m_resultImg.data->info.format
				, { VK_IMAGE_ASPECT_COLOR_BIT, i, 1u, 0u, 1u } } ) );
		}

		m_pass.addDependency( previousPass );
		m_pass.addSampledView( m_sceneView, 0u );
		m_pass.addOutputColourView( m_pass.mergeViews( m_resultViews, false ) );
	}

	void HiPass::accept( castor3d::PipelineVisitorBase & visitor )
	{
		visitor.visit( m_vertexShader );
		visitor.visit( m_pixelShader );
	}
}
