/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MeshletBoundsTransformPass_H___
#define ___C3D_MeshletBoundsTransformPass_H___

#include "TransformModule.hpp"
#include "Castor3D/Buffer/BufferModule.hpp"
#include "Castor3D/Render/Node/RenderNodeModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include "Castor3D/Buffer/GpuBufferOffset.hpp"

#include <ashespp/Descriptor/DescriptorSet.hpp>

namespace castor3d
{
	class MeshletBoundsTransformPass
	{
	public:
		enum Ids
		{
			ePositions,
			eNormals,
			eMeshlets,
			eOutCullData,
		};

	public:
		MeshletBoundsTransformPass( RenderDevice const & device
			, SubmeshRenderNode const & node
			, BoundsTransformPipeline const & pipeline
			, ObjectBufferOffset const & source
			, GpuBufferOffsetT< Meshlet > const & meshlets
			, GpuBufferOffsetT< MeshletCullData > const & output );

		void recordInto( crg::RecordContext & context
			, VkCommandBuffer commandBuffer
			, uint32_t index );

	private:
		RenderDevice const & m_device;
		BoundsTransformPipeline const & m_pipeline;
		ObjectBufferOffset const & m_source;
		GpuBufferOffsetT< Meshlet > const & m_meshlets;
		GpuBufferOffsetT< MeshletCullData > const & m_output;
		uint32_t m_meshletCount{};
		ashes::DescriptorSetPtr m_descriptorSet;
	};
}

#endif
