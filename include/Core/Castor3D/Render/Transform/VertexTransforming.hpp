/*
See LICENSE file in root folder
*/
#ifndef ___C3D_VertexTransforming_H___
#define ___C3D_VertexTransforming_H___

#include "Castor3D/Render/Transform/TransformPipeline.hpp"
#include "Castor3D/Buffer/BufferModule.hpp"
#include "Castor3D/Render/Node/RenderNodeModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include <CastorUtils/Design/OwnedBy.hpp>

#include <unordered_map>

namespace castor3d
{
	class VertexTransforming
		: public castor::OwnedBy< Scene const >
	{
	public:
		C3D_API VertexTransforming( Scene const & scene
			, RenderDevice const & device );

		C3D_API crg::FramePass const & createPass( crg::FrameGraph & graph
			, crg::FramePass const * previousPass = nullptr );

		C3D_API void registerNode( SubmeshRenderNode const & node
			, GpuBufferOffsetT< castor::Point4f > const & morphTargets
			, GpuBufferOffsetT< MorphingWeightsConfiguration > const & morphingWeights
			, GpuBufferOffsetT< SkinningTransformsConfiguration > const & skinTransforms );

	private:
		TransformPipeline const & doGetPipeline( uint32_t index );
		void doProcessPending();

	private:
		RenderDevice const & m_device;
		std::unordered_map< uint32_t, TransformPipeline > m_pipelines;
		std::unique_ptr< BoundsTransformPipeline > m_boundsPipeline;
		struct PendingNode
		{
			SubmeshRenderNode const * node;
			GpuBufferOffsetT< castor::Point4f > const * morphTargets;
			GpuBufferOffsetT< MorphingWeightsConfiguration > const * morphingWeights;
			GpuBufferOffsetT< SkinningTransformsConfiguration > const * skinTransforms;
		};
		std::vector< PendingNode > m_pending;
		VertexTransformingPass * m_pass{};
		MeshletBoundsTransformingPass * m_boundsPass{};
	};
}

#endif
