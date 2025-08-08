#include "Castor3D/Render/Upscale/UpscalePass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/Upscale/UpscaleWrapper.hpp"

#include <RenderGraph/FramePassGroup.hpp>

namespace c3d
{
	UpscalingFramePass::UpscalingFramePass( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, RenderTarget & target
		, UpscaleConfig const & config
		, crg::ru::Config const & ruConfig )
		: RunnablePass{ pass
			, context
			, graph
			, { [this]( uint32_t index ){ doInitialise( index ); }
				, GetPipelineStateCallback( [](){ return crg::getPipelineState( PipelineStageFlags::eComputeShader ); } )
				, [this]( crg::RecordContext & recContext, VkCommandBuffer cb, uint32_t i ){ doRecordInto( recContext, cb, i ); }
				, GetPassIndexCallback( [this](){ return doGetPassIndex(); } )
				, IsEnabledCallback( [this](){ return doIsEnabled(); } )
				, IsComputePassCallback( [](){ return true; } ) }
			, ruConfig }
		, m_device{ device }
		, m_target{ target }
		, m_config{ config }
	{
	}

	void UpscalingFramePass::update()
	{
		if ( m_upscaler )
			m_upscaler->update();
		++m_updateCount;
	}

	void UpscalingFramePass::doInitialise( uint32_t index )
	{
		if ( m_upscaler )
			m_upscaler.reset();

		auto itResolved = m_pass.inputs.begin();
		auto itUnresolved = std::next( itResolved );
		auto resolved = itResolved->second->view( index );
		auto unresolved = itUnresolved->second->view( index );

		auto renderSize = getExtent( unresolved );
		auto displaySize = getExtent( resolved );

		m_upscaler = m_device.upscaling->createInstance( m_target
			, { renderSize.width, renderSize.height }
			, { displaySize.width, displaySize.height }
			, m_config );
	}

	void UpscalingFramePass::doRecordInto( crg::RecordContext & context
		, VkCommandBuffer commandBuffer
		, uint32_t index )
	{
		if ( m_updateCount == 0 )
			return;
		auto itResolved = m_pass.inputs.begin();
		auto itUnresolved = std::next( itResolved );
		auto itMotion = std::next( itUnresolved );
		auto itDepth = std::next( itMotion );
		m_upscaler->evaluateUpscaling( context, commandBuffer
			, itResolved->second->view( index )
			, itUnresolved->second->view( index )
			, itMotion->second->view( index )
			, itDepth->second->view( index )
			, m_updateCount < 2
			, m_target.getJitter() );
	}

	uint32_t UpscalingFramePass::doGetPassIndex()const
	{
		return 0;
	}

	bool UpscalingFramePass::doIsEnabled()const
	{
		return m_config.enabled;
	}
}
