/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RsmInterpolatePass_HPP___
#define ___C3D_RsmInterpolatePass_HPP___

#include "ReflectiveShadowMapGIModule.hpp"
#include "Castor3D/Cache/CacheModule.hpp"

#include "Castor3D/Buffer/UniformBuffer.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapModule.hpp"
#include "Castor3D/Render/Technique/Opaque/OpaqueModule.hpp"
#include "Castor3D/Render/Passes/RenderQuad.hpp"
#include "Castor3D/Scene/Light/LightModule.hpp"
#include "Castor3D/Shader/Ubos/RsmConfigUbo.hpp"

#include <ShaderAST/Shader.hpp>

namespace castor3d
{
	class RsmInterpolatePass
		: public RenderQuad
	{
	public:
		C3D_API RsmInterpolatePass( crg::FrameGraph & graph
			, crg::FramePass const *& previousPass
			, RenderDevice const & device
			, LightCache const & lightCache
			, LightType lightType
			, VkExtent2D const & size
			, GpInfoUbo const & gpInfoUbo
			, OpaquePassResult const & gpResult
			, ShadowMapResult const & smResult
			, RsmConfigUbo const & rsmConfigUbo
			, ashes::Buffer< castor::Point2f > const & rsmSamplesSsbo
			, crg::ImageViewId const & gi
			, crg::ImageViewId const & nml
			, crg::ImageViewId const & dst );
		C3D_API ashes::Semaphore const & compute( ashes::Semaphore const & toWait )const;
		C3D_API CommandsSemaphore getCommands( RenderPassTimer const & timer
			, uint32_t index )const;
		C3D_API void accept( PipelineVisitorBase & visitor );

	private:
		LightCache const & m_lightCache;
		GpInfoUbo const & m_gpInfo;
		OpaquePassResult const & m_gpResult;
		ShadowMapResult const & m_smResult;
		crg::ImageViewId const & m_gi;
		crg::ImageViewId const & m_nml;
		RsmConfigUbo const & m_rsmConfigUbo;
		ashes::Buffer< castor::Point2f > const & m_rsmSamplesSsbo;
		ShaderModule m_vertexShader;
		ShaderModule m_pixelShader;
		ashes::RenderPassPtr m_renderPass;
		ashes::FrameBufferPtr m_frameBuffer;
		RenderPassTimerSPtr m_timer;
		CommandsSemaphore m_commands;
	};
}

#endif
