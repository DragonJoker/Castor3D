/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RsmInterpolatePass_HPP___
#define ___C3D_RsmInterpolatePass_HPP___

#include "ReflectiveShadowMapsModule.hpp"
#include "Castor3D/Cache/CacheModule.hpp"

#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapModule.hpp"
#include "Castor3D/Render/Opaque/OpaqueModule.hpp"
#include "Castor3D/Scene/Light/LightModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include "Castor3D/Buffer/GpuBufferOffset.hpp"

#include <ShaderAST/Shader.hpp>

namespace castor3d
{
	class RsmInterpolatePass
		: public castor::Named
	{
	public:
		C3D_API RsmInterpolatePass( crg::FrameGraph & graph
			, crg::FramePass const & previousPass
			, RenderDevice const & device
			, LightType lightType
			, ShadowBuffer const & shadowBuffer
			, VkExtent3D const & size
			, CameraUbo const & cameraUbo
			, crg::ImageViewId const & depthObj
			, crg::ImageViewId const & nmlOcc
			, ShadowMapResult const & smResult
			, RsmConfigUbo const & rsmConfigUbo
			, GpuBufferOffsetT< castor::Point4f > const & rsmSamplesSsbo
			, Texture const & gi
			, Texture const & nml
			, Texture const & dst );
		C3D_API void accept( ConfigurationVisitorBase & visitor );

		crg::FramePass const & getPass()const
		{
			return *m_pass;
		}

	private:
		ShaderModule m_vertexShader;
		ShaderModule m_pixelShader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
		crg::FramePass const * m_pass{};
	};
}

#endif
