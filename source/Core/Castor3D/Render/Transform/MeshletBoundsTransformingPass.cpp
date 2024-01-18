#include "Castor3D/Render/Transform/MeshletBoundsTransformingPass.hpp"

#include "Castor3D/Buffer/ObjectBufferOffset.hpp"
#include "Castor3D/Render/Transform/MeshletBoundsTransformPass.hpp"
#include "Castor3D/Render/Node/SubmeshRenderNode.hpp"

#include <CastorUtils/Miscellaneous/Hash.hpp>

namespace castor3d
{
	//*********************************************************************************************

	namespace mlttrsg
	{
		static size_t makeHash( GpuBufferOffsetT< MeshletCullData > const & output )
		{
			auto result = std::hash< VkDeviceSize >{}( output.getOffset() );
			castor::hashCombinePtr( result, output.getBuffer() );
			return result;
		}
	}

	//*********************************************************************************************

	MeshletBoundsTransformingPass::MeshletBoundsTransformingPass( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device )
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
	{
	}

	void MeshletBoundsTransformingPass::registerNode( SubmeshRenderNode const & node
		, BoundsTransformPipeline const & pipeline )
	{
		auto & output = node.getFinalMeshletsBounds();
		auto hash = mlttrsg::makeHash( output );

		if ( auto [it, res] = m_transformPasses.emplace( hash, nullptr );
			res )
		{
			it->second = castor::makeUnique< MeshletBoundsTransformPass >( m_device
				, node
				, pipeline
				, output );
			resetCommandBuffer( 0u );
		}
	}

	void MeshletBoundsTransformingPass::unregisterNode( SubmeshRenderNode const & node )
	{
		auto & output = node.getFinalMeshletsBounds();
		auto hash = mlttrsg::makeHash( output );

		if ( auto it = m_transformPasses.find( hash );
			it != m_transformPasses.end() )
		{
			m_transformPasses.erase( it );
		}
	}

	void MeshletBoundsTransformingPass::doRecordInto( crg::RecordContext & context
		, VkCommandBuffer commandBuffer
		, uint32_t index )const
	{
		for ( auto const& [_, pass] : m_transformPasses )
		{
			pass->recordInto( context, commandBuffer, index );
		}
	}

	bool MeshletBoundsTransformingPass::doIsComputePass()const noexcept
	{
		return true;
	}

	//*********************************************************************************************
}
