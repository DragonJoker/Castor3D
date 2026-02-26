/*
See LICENSE file in root folder
*/
#ifndef ___C3D_IntegrateFroxels_H___
#define ___C3D_IntegrateFroxels_H___

#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/Volumetric/VolumetricModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

namespace c3d
{
	C3D_API void createIntegrateFroxelsPass( crg::FramePassGroup & graph
		, RenderDevice const & device
		, FrustumFroxels const & froxels
		, Texture & rawFroxelsLightingU32R
		, Texture & rawFroxelsLightingU32G
		, Texture & rawFroxelsLightingU32B );
	C3D_API void createDisplayFroxelsAABBProgram( RenderDevice const & device
		, FrustumFroxels const & froxels
		, CameraUbo const & mainCameraUbo
		, CameraUbo const & clustersCameraUbo
		, RenderUbo const & renderUbo
		, ashes::PipelineShaderStageCreateInfoArray & program
		, ashes::VkDescriptorSetLayoutBindingArray & bindings
		, ashes::WriteDescriptorSetArray & writes );
}

#endif
