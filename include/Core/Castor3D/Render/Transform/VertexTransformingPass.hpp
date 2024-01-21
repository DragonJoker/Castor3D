/*
See LICENSE file in root folder
*/
#ifndef ___C3D_VertexTransformingPass_H___
#define ___C3D_VertexTransformingPass_H___

#include "TransformModule.hpp"
#include "Castor3D/Buffer/BufferModule.hpp"
#include "Castor3D/Render/Node/RenderNodeModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include <RenderGraph/RunnablePass.hpp>

#include <unordered_map>

namespace castor3d
{
	class VertexTransformingPass
		: public crg::RunnablePass
	{
	public:
		VertexTransformingPass( crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, RenderDevice const & device
			, ashes::Buffer< ModelBufferConfiguration > const & modelsBuffer );

		void registerNode( SubmeshRenderNode const & node
			, TransformPipeline const & pipeline
			, GpuBufferOffsetT< castor::Point4f > const & morphTargets
			, GpuBufferOffsetT< MorphingWeightsConfiguration > const & morphingWeights
			, GpuBufferOffsetT< SkinningTransformsConfiguration > const & skinTransforms );
		void unregisterNode( SubmeshRenderNode const & node );

	private:
		void doRecordInto( crg::RecordContext & context
			, VkCommandBuffer commandBuffer )const;
		bool doIsComputePass()const noexcept;

	private:
		RenderDevice const & m_device;
		ashes::Buffer< ModelBufferConfiguration > const & m_modelsBuffer;
		std::unordered_map< size_t, VertexTransformPassUPtr > m_transformPasses;
	};
}

#endif
