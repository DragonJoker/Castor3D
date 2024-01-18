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
		static size_t makeHash( ObjectBufferOffset const & input
			, ObjectBufferOffset const & output
			, Geometry const & geometry )
		{
			auto result = input.hash;
			castor::hashCombinePtr( result, input.getBuffer( SubmeshData::ePositions ) );
			castor::hashCombine( result, input.getOffset( SubmeshData::ePositions ) );
			castor::hashCombine( result, output.hash );
			castor::hashCombinePtr( result, output.getBuffer( SubmeshData::ePositions ) );
			castor::hashCombine( result, output.getOffset( SubmeshData::ePositions ) );
			castor::hashCombinePtr( result, geometry );
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
			, { []( uint32_t index ){}
				, GetPipelineStateCallback( [](){ return crg::getPipelineState( VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT ); } )
				, [this]( crg::RecordContext & ctx, VkCommandBuffer cb, uint32_t i ){ doRecordInto( ctx, cb, i ); }
				, crg::defaultV< crg::RunnablePass::GetPassIndexCallback >
				, crg::RunnablePass::IsEnabledCallback( [this](){ return !m_transformPasses.empty(); } )
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
		auto hash = vtxtrsg::makeHash( input, output, node.instance );
		auto [it, res] = m_transformPasses.try_emplace( hash );

		if ( res )
		{
			it->second = castor::makeUnique< VertexTransformPass >( m_device
				, node
				, pipeline
				, input
				, output
				, m_modelsBuffer
				, morphTargets
				, morphingWeights
				, skinTransforms );
			reRecordCurrent();
		}
	}

	void VertexTransformingPass::unregisterNode( SubmeshRenderNode const & node )
	{
		auto & input = node.getSourceBufferOffsets();
		auto & output = node.getFinalBufferOffsets();
		auto hash = vtxtrsg::makeHash( input, output, node.instance );
		
		if ( auto it = m_transformPasses.find( hash );
			it != m_transformPasses.end() )
		{
			m_transformPasses.erase( it );
		}
	}

	void VertexTransformingPass::doRecordInto( crg::RecordContext & context
		, VkCommandBuffer commandBuffer
		, uint32_t index )const
	{
		context.memoryBarrier( commandBuffer
			, m_modelsBuffer.getBuffer()
			, { 0u, ashes::WholeSize }
			, VK_ACCESS_HOST_WRITE_BIT
			, VK_PIPELINE_STAGE_HOST_BIT
			, { VK_ACCESS_SHADER_READ_BIT
				, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT } );

		for ( auto const & [_, pass] : m_transformPasses )
		{
			pass->recordInto( context, commandBuffer, index );
		}
		
		context.memoryBarrier( commandBuffer
			, m_modelsBuffer.getBuffer()
			, { 0u, ashes::WholeSize }
			, VK_ACCESS_SHADER_READ_BIT
			, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT
			, { VK_ACCESS_UNIFORM_READ_BIT
				, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT } );
	}

	bool VertexTransformingPass::doIsComputePass()const noexcept
	{
		return true;
	}

	//*********************************************************************************************
}
