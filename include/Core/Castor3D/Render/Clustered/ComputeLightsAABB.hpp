/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ComputeLightsAABB_H___
#define ___C3D_ComputeLightsAABB_H___

#include "ClusteredModule.hpp"

namespace c3d
{
	C3D_API void createComputeLightsAABBPass( crg::FramePassGroup & graph
		, RenderDevice const & device
		, FrustumClusters const & clusters
		, CameraUbo const & clustersCameraUbo
		, BufferBase & allLightsAABBB );
	C3D_API void createDisplayLightsAABBProgram( RenderDevice const & device
		, FrustumClusters const & clusters
		, CameraUbo const & mainCameraUbo
		, CameraUbo const & clustersCameraUbo
		, ashes::PipelineShaderStageCreateInfoArray & program
		, ashes::VkDescriptorSetLayoutBindingArray & bindings
		, ashes::WriteDescriptorSetArray & writes
		, BufferBase const & allLightsAABBB );
}

#endif
