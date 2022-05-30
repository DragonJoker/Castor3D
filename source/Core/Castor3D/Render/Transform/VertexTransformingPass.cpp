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
		size_t makeHash( ObjectBufferOffset const & input
			, ObjectBufferOffset const & output )
		{
			auto result = input.hash;
			castor::hashCombinePtr( result, input.getBuffer( SubmeshFlag::ePositions ) );
			castor::hashCombine( result, input.getOffset( SubmeshFlag::ePositions ) );
			castor::hashCombine( result, output.hash );
			castor::hashCombinePtr( result, output.getBuffer( SubmeshFlag::ePositions ) );
			castor::hashCombine( result, output.getOffset( SubmeshFlag::ePositions ) );
			return result;
		}
	}

	//*********************************************************************************************

	VertexTransformingPass::VertexTransformingPass( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, ashes::Buffer< ModelBufferConfiguration > const & modelsBuffer )
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
		, m_modelsBuffer{ modelsBuffer }
	{
	}

	void VertexTransformingPass::registerNode( SubmeshRenderNode const & node
		, TransformPipeline const & pipeline
		, GpuBufferOffsetT< castor::Point4f > const & morphTargets
		, GpuBufferOffsetT< castor3d::MorphingWeightsConfiguration > const & morphingWeights
		, GpuBufferOffsetT< SkinningTransformsConfiguration > const & skinTransforms )
	{
		auto & input = node.getSourceBufferOffsets();
		auto & output = node.getFinalBufferOffsets();
		auto submeshFlags = node.getSubmeshFlags();
		auto morphFlags = node.getMorphFlags();
		auto programFlags = node.getProgramFlags();
		auto hash = vtxtrsg::makeHash( input, output );
		auto ires = m_passes.emplace( hash, nullptr );

		if ( ires.second )
		{
			ires.first->second = castor::makeUnique< VertexTransformPass >( m_device
				, node
				, pipeline
				, input
				, output
				, m_modelsBuffer
				, morphTargets
				, morphingWeights
				, skinTransforms );
			resetCommandBuffer();
		}
	}

	void VertexTransformingPass::unregisterNode( SubmeshRenderNode const & node )
	{
		auto & input = node.getSourceBufferOffsets();
		auto & output = node.getFinalBufferOffsets();
		auto hash = vtxtrsg::makeHash( input, output );
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
		context.memoryBarrier( commandBuffer
			, m_modelsBuffer.getBuffer()
			, { 0u, ashes::WholeSize }
			, VK_ACCESS_HOST_WRITE_BIT
			, VK_PIPELINE_STAGE_HOST_BIT
			, { VK_ACCESS_SHADER_READ_BIT
				, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT } );

		for ( auto & passIt : m_passes )
		{
			passIt.second->recordInto( context, commandBuffer, index );
		}
		
		context.memoryBarrier( commandBuffer
			, m_modelsBuffer.getBuffer()
			, { 0u, ashes::WholeSize }
			, VK_ACCESS_SHADER_READ_BIT
			, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT
			, { VK_ACCESS_UNIFORM_READ_BIT
				, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT } );
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
