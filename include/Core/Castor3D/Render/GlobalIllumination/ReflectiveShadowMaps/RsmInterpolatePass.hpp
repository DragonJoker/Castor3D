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

namespace c3d
{
	class RsmInterpolatePass
		: public Named
	{
	public:
		C3D_API RsmInterpolatePass( crg::FrameGraph & graph
			, RenderDevice const & device
			, LightType lightType
			, ShadowBuffer const & shadowBuffer
			, Extent3D const & size
			, CameraUbo const & cameraUbo
			, Texture const & depthObj
			, Texture const & nmlOcc
			, ShadowMapResult const & smResult
			, RsmConfigUbo const & rsmConfigUbo
			, GpuBufferOffsetT< Point4f > const & rsmSamplesSsbo
			, Texture const & gi
			, Texture const & nml
			, Texture & dst );
		C3D_API void accept( ConfigurationVisitorBase & visitor )const;

	private:
		ShaderModule m_vertexShader;
		ShaderModule m_pixelShader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
	};
}

#endif
