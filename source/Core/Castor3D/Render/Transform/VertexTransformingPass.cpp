#include "Castor3D/Render/Transform/VertexTransformingPass.hpp"

#include "Castor3D/Buffer/ObjectBufferOffset.hpp"
#include "Castor3D/Render/Transform/VertexTransformPass.hpp"
#include "Castor3D/Render/Node/SubmeshRenderNode.hpp"

#include <CastorUtils/Miscellaneous/Hash.hpp>

namespace castor3d
{
	//*********************************************************************************************

	namespace vtxtrsg
	{
		size_t makeHash( ObjectBufferOffset const & value )
		{
			auto result = value.hash;
			castor::hashCombinePtr( result, value.getBuffer( SubmeshFlag::ePositions ) );
			return castor::hashCombine( result, value.getOffset( SubmeshFlag::ePositions ) );
		}
	}

	//*********************************************************************************************

	VertexTransformingPass::VertexTransformingPass( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device )
		: crg::RunnablePass{ pass
			, context
			, graph
			, { [this](){ doInitialise(); }
				, GetSemaphoreWaitFlagsCallback( [this](){ return doGetSemaphoreWaitFlags(); } )
				, [this]( crg::RecordContext & context, VkCommandBuffer cb, uint32_t i ){ doRecordInto( context, cb, i ); }
				, crg::defaultV< crg::RunnablePass::GetPassIndexCallback >
				, crg::RunnablePass::IsEnabledCallback( [this](){ return !m_passes.empty(); } )
				, IsComputePassCallback( [this](){ return doIsComputePass(); } ) }
			, crg::ru::Config{ 1u, true } }
		, m_device{ device }
	{
	}

	void VertexTransformingPass::registerNode( SubmeshRenderNode const & node
		, TransformPipeline const & pipeline
		, GpuBufferOffsetT< castor::Point4f > const & morphTargets
		, GpuBufferOffsetT< castor3d::MorphingWeightsConfiguration > const & morphingWeights )
	{
		auto & input = node.getSourceBufferOffsets();
		auto & output = node.getFinalBufferOffsets();
		auto submeshFlags = node.getSubmeshFlags();
		auto morphFlags = node.getMorphFlags();
		auto programFlags = node.getProgramFlags();
		auto hash = vtxtrsg::makeHash( input );
		auto ires = m_passes.emplace( hash, nullptr );

		if ( ires.second )
		{
			ires.first->second = castor::makeUnique< VertexTransformPass >( m_device
				, node
				, pipeline
				, input
				, output
				, morphTargets
				, morphingWeights );
			resetCommandBuffer();
		}
	}

	void VertexTransformingPass::unregisterNode( ObjectBufferOffset const & input )
	{
		auto hash = vtxtrsg::makeHash( input );
		auto it = m_passes.find( hash );

		if ( it != m_passes.end() )
		{
			m_passes.erase( it );
		}
	}

	void VertexTransformingPass::doInitialise()
	{
	}

	void VertexTransformingPass::doRecordInto( crg::RecordContext & context
		, VkCommandBuffer commandBuffer
		, uint32_t index )
	{
		for ( auto & passIt : m_passes )
		{
			passIt.second->recordInto( context, commandBuffer, index );
		}
	}

	VkPipelineStageFlags VertexTransformingPass::doGetSemaphoreWaitFlags()const
	{
		return VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
	}

	bool VertexTransformingPass::doIsComputePass()const
	{
		return true;
	}

	//*********************************************************************************************
}
