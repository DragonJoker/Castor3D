#include "Castor3D/Render/Transform/MeshletBoundsTransformPass.hpp"

#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/GpuBufferOffset.hpp"
#include "Castor3D/Buffer/GpuBufferPool.hpp"
#include "Castor3D/Buffer/ObjectBufferOffset.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/Node/SubmeshRenderNode.hpp"
#include "Castor3D/Render/Transform/VertexTransforming.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/Animation/AnimatedMesh.hpp"
#include "Castor3D/Scene/Animation/AnimatedSkeleton.hpp"

#include <RenderGraph/RecordContext.hpp>

#include <ashespp/Descriptor/DescriptorSetPool.hpp>
#include <ashespp/Pipeline/ComputePipeline.hpp>
#include <ashespp/Pipeline/PipelineLayout.hpp>

CU_ImplementSmartPtr( c3d, MeshletBoundsTransformPass )

namespace c3d
{
	//*********************************************************************************************

	namespace vtxtrs
	{
		static ashes::DescriptorSetPtr createDescriptorSet( BoundsTransformPipeline const & pipeline
			, ObjectBufferOffset const & sourceOffsets
			, ObjectBufferOffset const & finalOffsets
			, GpuBufferOffsetT< MeshletCullData > const & output )
		{
			ashes::WriteDescriptorSetArray writes;
			writes.push_back( finalOffsets.getStorageBinding( SubmeshData::ePositions
				, MeshletBoundsTransformPass::ePositions ) );

			if ( pipeline.normals )
			{
				writes.push_back( finalOffsets.getStorageBinding( SubmeshData::eNormals
					, MeshletBoundsTransformPass::eNormals ) );
			}

			writes.push_back( sourceOffsets.getStorageBinding( SubmeshData::eMeshlets
				, MeshletBoundsTransformPass::eMeshlets ) );
			writes.push_back( output.getStorageBinding( MeshletBoundsTransformPass::eOutCullData ) );

			auto descriptorSet = pipeline.descriptorSetPool->createDescriptorSet( toUtf8(  pipeline.getName() ) );
			descriptorSet->setBindings( writes );
			descriptorSet->update();
			return descriptorSet;
		}
	}

	//*********************************************************************************************

	MeshletBoundsTransformPass::MeshletBoundsTransformPass( RenderDevice const & device
		, SubmeshRenderNode const & node
		, BoundsTransformPipeline const & pipeline
		, GpuBufferOffsetT< MeshletCullData > const & output )
		: m_device{ device }
		, m_pipeline{ pipeline }
		, m_sourceOffsets{ node.getSourceBufferOffsets() }
		, m_finalOffsets{ node.getFinalBufferOffsets() }
		, m_output{ output }
		, m_meshletCount{ uint32_t( m_output.getCount() ) }
		, m_descriptorSet{ vtxtrs::createDescriptorSet( pipeline
			, m_sourceOffsets
			, m_finalOffsets
			, m_output ) }
	{
	}

	void MeshletBoundsTransformPass::recordInto( crg::RecordContext & context
		, VkCommandBuffer commandBuffer )const
	{
		if ( m_output.buffer )
		{
			context.memoryBarrier( commandBuffer
				, m_output.getBuffer().bufferViewId
				, HostWriteState, ComputeShaderWriteState );
		}

		context.getContext().vkCmdBindPipeline( commandBuffer
			, VK_PIPELINE_BIND_POINT_COMPUTE
			, *m_pipeline.pipeline );
		context.getContext().vkCmdPushConstants( commandBuffer
			, *m_pipeline.pipelineLayout
			, VK_SHADER_STAGE_COMPUTE_BIT
			, 0u
			, sizeof( m_meshletCount )
			, &m_meshletCount );
		VkDescriptorSet set = *m_descriptorSet;
		context.getContext().vkCmdBindDescriptorSets( commandBuffer
			, VK_PIPELINE_BIND_POINT_COMPUTE
			, *m_pipeline.pipelineLayout
			, 0u
			, 1u
			, &set
			, 0u
			, nullptr );
		auto size = uint32_t( m_device.properties.limits.nonCoherentAtomSize );
		context.getContext().vkCmdDispatch( commandBuffer
			, uint32_t( ashes::getAlignedSize( m_output.getCount(), size ) ) / size
			, 1u
			, 1u );

		if ( m_output.buffer )
		{
			context.memoryBarrier( commandBuffer
				, m_output.getBuffer().bufferViewId
				, ComputeShaderWriteState, MeshShaderReadState );
		}

		auto & meshletsChunk = m_sourceOffsets.getBufferChunk( SubmeshData::eMeshlets );
		context.memoryBarrier( commandBuffer
			, meshletsChunk.buffer->getBuffer().bufferViewId
			, ComputeShaderReadState, MeshShaderReadState );

		for ( auto & buffer : m_finalOffsets.buffers )
		{
			if ( buffer.buffer )
			{
				context.memoryBarrier( commandBuffer
					, buffer.getBuffer().bufferViewId
					, ComputeShaderReadState, VertexAttributeInputState );
			}
		}
	}

	//*********************************************************************************************
}
