/*
See LICENSE file in root folder
*/
#ifndef ___C3D_VertexTransformPass_H___
#define ___C3D_VertexTransformPass_H___

#include "TransformModule.hpp"
#include "Castor3D/Buffer/BufferModule.hpp"
#include "Castor3D/Render/Node/RenderNodeModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include "Castor3D/Buffer/GpuBufferOffset.hpp"

#include <ashespp/Descriptor/DescriptorSet.hpp>

namespace castor3d
{
	class VertexTransformPass
	{
	public:
		enum Ids
		{
			eModelsData,
			eMorphTargets,
			eMorphingWeights,
			eSkinTransforms,
			eInPosition,
			eOutPosition,
			eInNormal,
			eOutNormal,
			eInTangent,
			eOutTangent,
			eInBitangent,
			eOutBitangent,
			eInTexcoord0,
			eOutTexcoord0,
			eInTexcoord1,
			eOutTexcoord1,
			eInTexcoord2,
			eOutTexcoord2,
			eInTexcoord3,
			eOutTexcoord3,
			eInColour,
			eOutColour,
			eInSkin,
			eOutVelocity,
		};

	public:
		VertexTransformPass( RenderDevice const & device
			, SubmeshRenderNode const & node
			, TransformPipeline const & pipeline
			, ObjectBufferOffset const & input
			, ObjectBufferOffset const & output
			, ashes::Buffer< ModelBufferConfiguration > const & modelsBuffer
			, GpuBufferOffsetT< castor::Point4f > const & morphTargets
			, GpuBufferOffsetT< MorphingWeightsConfiguration > const & morphingWeights
			, GpuBufferOffsetT< SkinningTransformsConfiguration > const & skinTransforms );

		void recordInto( crg::RecordContext & context
			, VkCommandBuffer commandBuffer
			, uint32_t index );

	private:
		RenderDevice const & m_device;
		TransformPipeline const & m_pipeline;
		ObjectBufferOffset const & m_input;
		ObjectBufferOffset const & m_output;
		GpuBufferOffsetT< castor::Point4f > const & m_morphTargets;
		GpuBufferOffsetT< MorphingWeightsConfiguration > const & m_morphingWeights;
		GpuBufferOffsetT< SkinningTransformsConfiguration > const & m_skinTransforms;
		ObjectIdsConfiguration m_objectIds;
		ashes::DescriptorSetPtr m_descriptorSet;
	};
}

#endif
