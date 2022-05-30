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
			, RenderDevice const & device );

		void registerNode( SubmeshRenderNode const & node
			, TransformPipeline const & pipeline
			, GpuBufferOffsetT< castor::Point4f > const & morphTargets
			, GpuBufferOffsetT< MorphingWeightsConfiguration > const & morphingWeights );
		void unregisterNode( ObjectBufferOffset const & input );

	private:
		void doInitialise();
		void doRecordInto( crg::RecordContext & context
			, VkCommandBuffer commandBuffer
			, uint32_t index );
		VkPipelineStageFlags doGetSemaphoreWaitFlags()const;
		bool doIsComputePass()const;

	private:
		RenderDevice const & m_device;
		std::unordered_map< size_t, VertexTransformPassUPtr > m_passes;
	};
}

#endif
