#include "Castor3D/Render/PostEffect/PostEffect.hpp"

#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"

#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>

#include <RenderGraph/FrameGraph.hpp>
#include <RenderGraph/GraphContext.hpp>
#include <RenderGraph/RunnableGraph.hpp>

namespace castor3d
{
	PostEffect::PostEffect( castor::String const & name
		, castor::String const & groupName
		, castor::String const & fullName
		, RenderTarget & renderTarget
		, RenderSystem & renderSystem
		, Parameters const & CU_UnusedParam( parameters )
		, uint32_t passesCount
		, Kind kind )
		: castor::OwnedBy< RenderSystem >{ renderSystem }
		, castor::Named{ name }
		, m_fullName{ fullName }
		, m_renderTarget{ renderTarget }
		, m_graph{ m_renderTarget.getGraph().createPassGroup( groupName ) }
		, m_passesCount{ passesCount }
		, m_kind{ kind }
		, m_enabled{ true }
	{
	}

	PostEffect::~PostEffect()
	{
	}

	bool PostEffect::writeInto( castor::StringStream & file, castor::String const & tabs )
	{
		return doWriteInto( file, tabs );
	}

	bool PostEffect::initialise( castor3d::RenderDevice const & device
		, Texture const & source
		, Texture const & target
		, crg::FramePass const & previousPass )
	{
		m_source = &source;
		return doInitialise( device
			, source
			, target
			, previousPass );
	}

	void PostEffect::cleanup( castor3d::RenderDevice const & device )
	{
		doCleanup( device );
	}

	bool PostEffect::update( CpuUpdater & updater
		, Texture const & source )
	{
		m_passIndex = ( &source == m_source ) ? 0u : 1u;
		doCpuUpdate( updater );
		return isEnabled();
	}

	void PostEffect::update( GpuUpdater & updater )
	{
		doGpuUpdate( updater );
	}

	void PostEffect::doCpuUpdate( CpuUpdater & updater )
	{
	}

	void PostEffect::doGpuUpdate( GpuUpdater & updater )
	{
	}

	void PostEffect::doCopyImage( crg::RunnableGraph & graph
		, crg::RunnablePass const & runnable
		, crg::RecordContext & recordContext
		, VkCommandBuffer commandBuffer
		, uint32_t index
		, crg::ImageViewId const & source
		, crg::ImageViewId const & target )
	{
		auto extent = getExtent( source );
		auto & context = getRenderSystem()->getRenderDevice().makeContext();
		auto & srcSubresource = source.data->info.subresourceRange;
		auto & dstSubresource = target.data->info.subresourceRange;
		VkImageCopy region{ VkImageSubresourceLayers{ srcSubresource.aspectMask, srcSubresource.baseMipLevel, srcSubresource.baseArrayLayer, 1u }
			, VkOffset3D{ 0u, 0u, 0u }
			, VkImageSubresourceLayers{ dstSubresource.aspectMask, dstSubresource.baseMipLevel, dstSubresource.baseArrayLayer, 1u }
			, VkOffset3D{ 0u, 0u, 0u }
			, { extent.width, extent.height, 1u } };
		auto srcTransition = runnable.getTransition( index, source );
		auto dstTransition = runnable.getTransition( index, target );
		graph.memoryBarrier( recordContext
			, commandBuffer
			, source.data->image
			, source.data->info.viewType
			, source.data->info.subresourceRange
			, srcTransition.to.layout
			, { VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
				, crg::getAccessMask( VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL )
				, crg::getStageMask( VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL ) } );
		graph.memoryBarrier( recordContext
			, commandBuffer
			, target.data->image
			, target.data->info.viewType
			, target.data->info.subresourceRange
			, dstTransition.to.layout
			, { VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
				, crg::getAccessMask( VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL )
				, crg::getStageMask( VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ) } );
		context.vkCmdCopyImage( commandBuffer
			, graph.createImage( source.data->image )
			, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
			, graph.createImage( target.data->image )
			, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
			, 1u
			, &region );
		graph.memoryBarrier( recordContext
			, commandBuffer
			, target.data->image
			, target.data->info.viewType
			, target.data->info.subresourceRange
			, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
			, dstTransition.to );
		graph.memoryBarrier( recordContext
			, commandBuffer
			, source.data->image
			, source.data->info.viewType
			, source.data->info.subresourceRange
			, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
			, srcTransition.to );
	}
}
