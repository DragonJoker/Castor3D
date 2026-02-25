#include "Castor3D/Render/Transform/VertexTransformPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/ObjectBufferOffset.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/SubmeshComponentRegister.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Node/SubmeshRenderNode.hpp"
#include "Castor3D/Render/Transform/VertexTransforming.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/Animation/AnimatedMesh.hpp"
#include "Castor3D/Scene/Animation/AnimatedSkeleton.hpp"

#include <RenderGraph/RecordContext.hpp>

#include <ashespp/Descriptor/DescriptorSetPool.hpp>

CU_ImplementSmartPtr( c3d, VertexTransformPass )

namespace c3d
{
	//*********************************************************************************************

	namespace vtxtrs
	{
		static ashes::DescriptorSetPtr createDescriptorSet( Engine const & engine
			, TransformPipeline const & pipeline
			, ObjectBufferOffset const & input
			, ObjectBufferOffset const & output
			, BufferBase const & modelsBuffer
			, GpuBufferOffsetT< Point4f > const & morphTargets
			, GpuBufferOffsetT< MorphingWeightsConfiguration > const & morphingWeights
			, GpuBufferOffsetT< SkinningTransformsConfiguration > const & skinTransforms )
		{
			ashes::WriteDescriptorSetArray writes;
			auto combine = engine.getSubmeshComponentsRegister().getSubmeshComponentCombine( pipeline.combineID );
			CU_Require( morphTargets || skinTransforms );
			modelsBuffer.addDescriptorWriteT( writes, VertexTransformPass::eModelsData );

			if ( morphTargets )
				morphTargets.addDescriptorWriteT( writes, VertexTransformPass::eMorphTargets );
			if ( morphingWeights )
				morphingWeights.addDescriptorWriteT( writes, VertexTransformPass::eMorphingWeights );
			if ( skinTransforms )
				skinTransforms.addDescriptorWriteT( writes, VertexTransformPass::eSkinTransforms );

			if ( combine.hasPositionFlag )
			{
				input.addDescriptorWriteT( writes, SubmeshData::ePositions, VertexTransformPass::eInPosition );
				output.addDescriptorWriteT( writes, SubmeshData::ePositions, VertexTransformPass::eOutPosition );
			}

			if ( combine.hasNormalFlag )
			{
				input.addDescriptorWriteT( writes, SubmeshData::eNormals, VertexTransformPass::eInNormal );
				output.addDescriptorWriteT( writes, SubmeshData::eNormals, VertexTransformPass::eOutNormal );
			}

			if ( combine.hasTangentFlag )
			{
				input.addDescriptorWriteT( writes, SubmeshData::eTangents, VertexTransformPass::eInTangent );
				output.addDescriptorWriteT( writes, SubmeshData::eTangents, VertexTransformPass::eOutTangent );
			}

			if ( combine.hasBitangentFlag )
			{
				input.addDescriptorWriteT( writes, SubmeshData::eBitangents, VertexTransformPass::eInBitangent );
				output.addDescriptorWriteT( writes, SubmeshData::eBitangents, VertexTransformPass::eOutBitangent );
			}

			if ( combine.hasTexcoord0Flag )
			{
				input.addDescriptorWriteT( writes, SubmeshData::eTexcoords0, VertexTransformPass::eInTexcoord0 );
				output.addDescriptorWriteT( writes, SubmeshData::eTexcoords0, VertexTransformPass::eOutTexcoord0 );
			}

			if ( combine.hasTexcoord1Flag )
			{
				input.addDescriptorWriteT( writes, SubmeshData::eTexcoords1, VertexTransformPass::eInTexcoord1 );
				output.addDescriptorWriteT( writes, SubmeshData::eTexcoords1, VertexTransformPass::eOutTexcoord1 );
			}

			if ( combine.hasTexcoord2Flag )
			{
				input.addDescriptorWriteT( writes, SubmeshData::eTexcoords2, VertexTransformPass::eInTexcoord2 );
				output.addDescriptorWriteT( writes, SubmeshData::eTexcoords2, VertexTransformPass::eOutTexcoord2 );
			}

			if ( combine.hasTexcoord3Flag )
			{
				input.addDescriptorWriteT( writes, SubmeshData::eTexcoords3, VertexTransformPass::eInTexcoord3 );
				output.addDescriptorWriteT( writes, SubmeshData::eTexcoords3, VertexTransformPass::eOutTexcoord3 );
			}

			if ( combine.hasColourFlag )
			{
				input.addDescriptorWriteT( writes, SubmeshData::eColours, VertexTransformPass::eInColour );
				output.addDescriptorWriteT( writes, SubmeshData::eColours, VertexTransformPass::eOutColour );
			}

			if ( combine.hasSkinFlag )
				input.addDescriptorWriteT( writes, SubmeshData::eSkin, VertexTransformPass::eInSkin );

			output.addDescriptorWriteT( writes, SubmeshData::eVelocity, VertexTransformPass::eOutVelocity );

			auto descriptorSet = pipeline.descriptorSetPool->createDescriptorSet( toUtf8( pipeline.getName( engine ) ) );
			descriptorSet->setBindings( writes );
			descriptorSet->update();
			return descriptorSet;
		}
	}

	//*********************************************************************************************

	VertexTransformPass::VertexTransformPass( RenderDevice const & device
		, SubmeshRenderNode const & node
		, TransformPipeline const & pipeline
		, ObjectBufferOffset const & input
		, ObjectBufferOffset const & output
		, BufferBase const & modelsBuffer
		, GpuBufferOffsetT< Point4f > const & morphTargets
		, GpuBufferOffsetT< MorphingWeightsConfiguration > const & morphingWeights
		, GpuBufferOffsetT< SkinningTransformsConfiguration > const & skinTransforms )
		: m_device{ device }
		, m_pipeline{ pipeline }
		, m_input{ input }
		, m_output{ output }
		, m_morphTargets{ morphTargets }
		, m_morphingWeights{ morphingWeights }
		, m_skinTransforms{ skinTransforms }
		, m_descriptorSet{ vtxtrs::createDescriptorSet( c3d::getEngine( device )
			, pipeline
			, m_input
			, m_output
			, modelsBuffer
			, morphTargets
			, morphingWeights
			, skinTransforms ) }
	{
		m_objectIds.nodeId = node.instance.getId( *node.pass, node.data ) - 1u;
		m_objectIds.morphingId = node.mesh ? node.mesh->getId( node.data ) - 1u : ~0u;
		m_objectIds.skinningId = node.skeleton ? node.skeleton->getId() - 1u : ~0u;
	}

	void VertexTransformPass::recordInto( crg::RecordContext & context
		, VkCommandBuffer commandBuffer )const
	{
		if ( m_morphTargets )
		{
			context.memoryBarrier( commandBuffer
				, m_morphTargets.getBuffer().bufferViewId
				, HostWriteState, ComputeShaderReadState );
		}

		if ( m_morphingWeights )
		{
			context.memoryBarrier( commandBuffer
				, m_morphingWeights.getBuffer().bufferViewId
				, HostWriteState, ComputeShaderReadState );
		}

		if ( m_skinTransforms )
		{
			context.memoryBarrier( commandBuffer
				, m_skinTransforms.getBuffer().bufferViewId
				, HostWriteState, ComputeShaderReadState );
		}

		auto itInput = m_input.buffers.begin();
		auto itOutput = m_output.buffers.begin();

		while ( itInput != m_input.buffers.end() )
		{
			if ( itInput->buffer && itOutput->buffer )
			{
				context.memoryBarrier( commandBuffer
					, itInput->getBuffer().bufferViewId
					, HostWriteState, ComputeShaderReadState );
				context.memoryBarrier( commandBuffer
					, itOutput->getBuffer().bufferViewId
					, HostWriteState, ComputeShaderWriteState );
			}

			++itInput;
			++itOutput;
		}

		context.getContext().vkCmdBindPipeline( commandBuffer
			, VK_PIPELINE_BIND_POINT_COMPUTE
			, *m_pipeline.pipeline );
		context.getContext().vkCmdPushConstants( commandBuffer
			, *m_pipeline.pipelineLayout
			, VK_SHADER_STAGE_COMPUTE_BIT
			, 0u
			, sizeof( ObjectIdsConfiguration )
			, &m_objectIds );
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
			, uint32_t( ashes::getAlignedSize( m_input.getCount< Point4f >( SubmeshData::ePositions ), size ) ) / size
			, 1u
			, 1u );

		itInput = m_input.buffers.begin();
		itOutput = m_output.buffers.begin();

		while ( itInput != m_input.buffers.end() )
		{
			if ( itInput->buffer && itOutput->buffer )
			{
				context.memoryBarrier( commandBuffer
					, itOutput->getBuffer().bufferViewId
					, ComputeShaderReadState, HostWriteState );

				if ( m_pipeline.meshletsBounds )
				{
					context.memoryBarrier( commandBuffer
						, itInput->getBuffer().bufferViewId
						, ComputeShaderWriteState, ComputeShaderReadState );
				}
				else
				{
					context.memoryBarrier( commandBuffer
						, itInput->getBuffer().bufferViewId
						, ComputeShaderWriteState, VertexAttributeInputState );
				}
			}

			++itInput;
			++itOutput;
		}

		if ( !m_pipeline.meshletsBounds )
		{
			if ( m_skinTransforms )
			{
				context.memoryBarrier( commandBuffer
					, m_skinTransforms.getBuffer().bufferViewId
					, ComputeShaderReadState, HostWriteState );
			}

			if ( m_morphTargets )
			{
				context.memoryBarrier( commandBuffer
					, m_morphTargets.getBuffer().bufferViewId
					, ComputeShaderReadState, HostWriteState );
			}

			if ( m_morphingWeights )
			{
				context.memoryBarrier( commandBuffer
					, m_morphingWeights.getBuffer().bufferViewId
					, ComputeShaderReadState, HostWriteState );
			}
		}
	}

	//*********************************************************************************************
}
