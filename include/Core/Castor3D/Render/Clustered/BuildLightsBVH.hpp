/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BuildLightsBVH_H___
#define ___C3D_BuildLightsBVH_H___

#include "ClusteredModule.hpp"

namespace c3d
{
	C3D_API void createBuildLightsBVHPass( crg::FramePassGroup & graph
		, RenderDevice const & device
		, FrustumClusters & clusters
		, BufferBase const & allLightsAABB
		, ClustersLightSortAttachs const & outputSortAttachs
		, BufferBase const & mergePathPartitions
		, BufferBase & pointLightBVH
		, BufferBase & spotLightBVH );
	C3D_API void createDisplayPointLightsBVHProgram( RenderDevice const & device
		, FrustumClusters const & clusters
		, CameraUbo const & mainCameraUbo
		, CameraUbo const & clustersCameraUbo
		, ashes::PipelineShaderStageCreateInfoArray & program
		, ashes::VkDescriptorSetLayoutBindingArray & bindings
		, ashes::WriteDescriptorSetArray & writes
		, BufferBase const & pointLightBVH );
	C3D_API void createDisplaySpotLightsBVHProgram( RenderDevice const & device
		, FrustumClusters const & clusters
		, CameraUbo const & mainCameraUbo
		, CameraUbo const & clustersCameraUbo
		, ashes::PipelineShaderStageCreateInfoArray & program
		, ashes::VkDescriptorSetLayoutBindingArray & bindings
		, ashes::WriteDescriptorSetArray & writes
		, BufferBase const & spotLightBVH );
}

#endif
