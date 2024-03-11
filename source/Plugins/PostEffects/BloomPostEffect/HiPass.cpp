#include "BloomPostEffect/HiPass.hpp"

#include "BloomPostEffect/BloomPostEffect.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderDevice.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/Shaders/GlslBaseIO.hpp>

#include <CastorUtils/Graphics/Image.hpp>

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <numeric>

namespace Bloom
{
	namespace hi
	{
		namespace c3d = castor3d::shader;

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
				, bool const * enabled
				, uint32_t const * passIndex )
				: crg::RenderQuad{ pass
					, context
					, graph
					, crg::ru::Config{ 2u }
					, crg::rq::Config{}
						.baseConfig( crg::pp::Config{ castor::Vector< crg::VkPipelineShaderStageCreateInfoArray >{ castor::move( program ) }, {}, {} } )
						.texcoordConfig( crg::Texcoord{} )
						.enabled( enabled )
						.passIndex( passIndex )
						.end( [this]( crg::RecordContext & recContext, VkCommandBuffer cb, uint32_t i ){ doRecordInto( recContext, cb, i ); } )
						.renderSize( renderSize ) }
			{
#if !Bloom_DebugHiPass
				uint32_t dataIndex{};

				for ( auto & data : m_hiPasses )
				{
					data.viewDesc = pass.images.back().view( dataIndex );
					data.imageDesc = data.viewDesc.data->image;
					data.image = graph.createImage( data.imageDesc );
					auto const imageViewType = VkImageViewType( data.imageDesc.data->info.imageType );
					crg::ImageViewData viewData{ data.imageDesc.data->name
						, data.imageDesc
						, 0u
						, imageViewType
						, data.imageDesc.data->info.format
						, { data.viewDesc.data->info.subresourceRange.aspectMask, 0u, 1u, 0u, 1u } };

					viewData.info.subresourceRange.baseMipLevel = data.viewDesc.data->info.subresourceRange.baseMipLevel;
					crg::ImageViewId prevLevelView = m_graph.getResources().getHandler().createViewId( viewData );

					for ( uint32_t levelIdx = 1u; levelIdx < data.imageDesc.data->info.mipLevels; ++levelIdx )
					{
						viewData.info.subresourceRange.baseMipLevel = data.viewDesc.data->info.subresourceRange.baseMipLevel + levelIdx;
						crg::ImageViewId currLevelView = m_graph.getResources().getHandler().createViewId( viewData );
						data.mipGens.push_back( { prevLevelView, currLevelView } );
						prevLevelView = currLevelView;
					}

					++dataIndex;
				}
#endif
			}

		private:
			void doRecordInto( crg::RecordContext & recordContext
				, VkCommandBuffer commandBuffer
				, uint32_t index )
			{
#if !Bloom_DebugHiPass
				auto & data = m_hiPasses[index];
				auto const width = int32_t( data.imageDesc.data->info.extent.width );
				auto const height = int32_t( data.imageDesc.data->info.extent.height );
				auto const depth = int32_t( data.imageDesc.data->info.extent.depth );
				auto const aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				auto layoutState = recordContext.getNextLayoutState( data.viewDesc );

				// Transition source view to transfer src layout
				recordContext.memoryBarrier( commandBuffer
					, data.viewDesc
					, layoutState.layout
					, { VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
						, crg::getAccessMask( VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL )
						, crg::getStageMask( VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL ) } );

				for ( auto & mipGen : data.mipGens )
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
					recordContext.memoryBarrier( commandBuffer
						, mipGen.dst
						, VK_IMAGE_LAYOUT_UNDEFINED
						, { VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
							, crg::getAccessMask( VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL )
							, crg::getStageMask( VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ) } );

					// Perform blit
					m_context.vkCmdBlitImage( commandBuffer
						, data.image
						, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
						, data.image
						, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
						, 1u
						, &imageBlit
						, VK_FILTER_LINEAR );

					// Transition destination mip level to transfer src layout
					recordContext.memoryBarrier( commandBuffer
						, mipGen.dst
						, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
						, { VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
							, crg::getAccessMask( VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL )
							, crg::getStageMask( VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL ) } );

					// Transition source mip level to wanted output layout
					recordContext.memoryBarrier( commandBuffer
						, mipGen.src
						, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
						, layoutState );
				}

				// Transition last destination mip level to wanted output layout
				auto & mipGen = data.mipGens.back();
				recordContext.memoryBarrier( commandBuffer
					, mipGen.dst
					, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
					, layoutState );
#endif
			}

#if !Bloom_DebugHiPass
		private:
			struct LevelMipGen
			{
				crg::ImageViewId src;
				crg::ImageViewId dst;
			};
			struct PassData
			{
				crg::ImageViewId viewDesc;
				crg::ImageId imageDesc;
				VkImage image{};
				castor::Vector< LevelMipGen > mipGens;
			};
			castor::Array< PassData, 2u > m_hiPasses;
#endif
		};

		static castor3d::ShaderPtr getProgram( castor3d::RenderDevice const & device )
		{
			sdw::TraditionalGraphicsWriter writer{ &device.renderSystem.getEngine()->getShaderAllocator() };

			auto c3d_mapColor = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapColor", 0u, 0u );

			writer.implementEntryPointT< c3d::PosUv2FT, c3d::Uv2FT >( [&]( sdw::VertexInT< c3d::PosUv2FT > in
				, sdw::VertexOutT< c3d::Uv2FT > out )
				{
					out.uv() = in.uv();
					out.vtx.position = vec4( in.position(), 0.0_f, 1.0_f );
				} );

			writer.implementEntryPointT< c3d::Uv2FT, c3d::Colour4FT >( [&]( sdw::FragmentInT< c3d::Uv2FT > in
				, sdw::FragmentOutT< c3d::Colour4FT > out )
				{
					out.colour() = vec4( c3d_mapColor.sample( in.uv(), 0.0_f ).xyz(), 1.0_f );
					auto maxComponent = writer.declLocale( "maxComponent"
						, max( out.colour().r(), out.colour().g() ) );
					maxComponent = max( maxComponent, out.colour().b() );

					IF( writer, maxComponent <= 1.0_f )
					{
						out.colour().xyz() = vec3( 0.0_f, 0.0_f, 0.0_f );
					}
					FI;
				} );
			return writer.getBuilder().releaseShader();
		}
	}

	//*********************************************************************************************

	HiPass::HiPass( crg::FramePassGroup & graph
		, crg::FramePass const & previousPass
		, castor3d::RenderDevice const & device
		, crg::ImageViewIdArray const & sceneView
		, VkExtent2D size
		, uint32_t blurPassesCount
		, bool const * enabled
		, uint32_t const * passIndex )
		: m_graph{ graph }
		, m_shader{ cuT( "BloomHiPass" ), hi::getProgram( device ) }
		, m_stages{ makeProgramStates( device, m_shader ) }
#if !Bloom_DebugHiPass
		, m_resultImg{ graph.createImage( crg::ImageData{ "BLHi"
			, 0u
			, VK_IMAGE_TYPE_2D
			, getFormat( sceneView.front() )
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
			, getFormat( sceneView.front() )
			, VkExtent3D{ size.width, size.height, 1u }
			, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_TRANSFER_SRC_BIT ) } ) }
#endif
		, m_pass{ graph.createPass( "HDR"
			, [this, &device, enabled, passIndex]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = castor::make_unique< hi::HiPassQuad >( framePass
					, context
					, graph
					, ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages )
					, VkExtent2D{ m_resultImg.data->info.extent.width, m_resultImg.data->info.extent.height }
					, enabled
					, passIndex );
				device.renderSystem.getEngine()->registerTimer( castor::makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} ) }
	{
		for ( uint32_t i = 0u; i < blurPassesCount; ++i )
		{
			m_resultViews.push_back( graph.createView( crg::ImageViewData{ m_resultImg.data->name + castor::string::toMbString( i )
				, m_resultImg
				, 0u
				, VK_IMAGE_VIEW_TYPE_2D
				, m_resultImg.data->info.format
				, { VK_IMAGE_ASPECT_COLOR_BIT, i, 1u, 0u, 1u } } ) );
		}

		m_pass.addDependency( previousPass );
		m_pass.addSampledView( sceneView, 0u );
		m_pass.addOutputColourView( m_pass.mergeViews( m_resultViews, false ) );
	}

	void HiPass::accept( castor3d::ConfigurationVisitorBase & visitor )
	{
		visitor.visit( m_shader );

		for ( auto & view : m_resultViews )
		{
			visitor.visit( cuT( "PostFX: HDRB - Hi " ) + castor::string::toString( view.data->info.subresourceRange.baseMipLevel )
				, view
				, m_graph.getFinalLayoutState( view ).layout
				, castor3d::TextureFactors{}.invert( true ) );
		}
	}
}
