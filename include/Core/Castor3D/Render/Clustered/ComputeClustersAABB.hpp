/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ComputeClustersAABB_H___
#define ___C3D_ComputeClustersAABB_H___

#include "ClusteredModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

namespace c3d
{
	C3D_API void createComputeClustersAABBPass( crg::FramePassGroup & graph
		, RenderDevice const & device
		, FrustumClusters const & clusters
		, CameraUbo const & clustersCameraUbo
		, RenderUbo const & renderUbo
		, BufferBase const & reducedLightsAABB
		, BufferBase & clustersAABB );
	C3D_API void createDisplayClustersAABBProgram( RenderDevice const & device
		, FrustumClusters const & clusters
		, CameraUbo const & mainCameraUbo
		, CameraUbo const & clustersCameraUbo
		, ashes::PipelineShaderStageCreateInfoArray & program
		, ashes::VkDescriptorSetLayoutBindingArray & bindings
		, ashes::WriteDescriptorSetArray & writes
		, BufferBase const & clustersAABB );
}

#endif
