/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RsmInterpolatePass_HPP___
#define ___C3D_RsmInterpolatePass_HPP___

#include "ReflectiveShadowMapGIModule.hpp"

#include "Castor3D/Buffer/UniformBuffer.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapModule.hpp"
#include "Castor3D/Render/Technique/Opaque/OpaqueModule.hpp"
#include "Castor3D/Render/ToTexture/RenderQuad.hpp"
#include "Castor3D/Scene/Light/LightModule.hpp"
#include "Castor3D/Shader/Ubos/RsmConfigUbo.hpp"

#include <ShaderAST/Shader.hpp>

namespace castor3d
{
	class RsmInterpolatePass
		: public RenderQuad
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\param[in]	src		The source RSM GI view.
		 *\param[in]	src		The source Normals view.
		 *\param[in]	dst		The destination view.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 *\param[in]	src		La vue RSM GI source.
		 *\param[in]	src		La vue Normales source.
		 *\param[in]	dst		La vue destination.
		 */
		C3D_API RsmInterpolatePass( Engine & engine
			, LightCache const & lightCache
			, LightType lightType
			, VkExtent2D const & size
			, GpInfoUbo const & gpInfoUbo
			, OpaquePassResult const & gpResult
			, ShadowMapResult const & smResult
			, RsmConfigUbo const & rsmConfigUbo
			, ashes::Buffer< castor::Point2f > const & rsmSamplesSsbo
			, TextureUnit const & gi
			, TextureUnit const & nml
			, TextureUnit const & dst );
		/**
		 *\~english
		 *\brief		Renders the SSGI pass.
		 *\param[in]	toWait	The semaphore from the previous render pass.
		 *\~french
		 *\brief		Dessine la passe SSGI.
		 *\param[in]	toWait	Le sémaphore de la passe de rendu précédente.
		 */
		C3D_API ashes::Semaphore const & compute( ashes::Semaphore const & toWait )const;
		C3D_API CommandsSemaphore getCommands( RenderPassTimer const & timer
			, uint32_t index )const;
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		C3D_API void accept( PipelineVisitorBase & visitor );

	private:
		void doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
			, ashes::DescriptorSet & descriptorSet )override;
		void doRegisterFrame( ashes::CommandBuffer & commandBuffer )const override;

	private:
		LightCache const & m_lightCache;
		GpInfoUbo const & m_gpInfo;
		OpaquePassResult const & m_gpResult;
		ShadowMapResult const & m_smResult;
		TextureUnit const & m_gi;
		TextureUnit const & m_nml;
		RsmConfigUbo const & m_rsmConfigUbo;
		ashes::Buffer< castor::Point2f > const & m_rsmSamplesSsbo;
		ShaderModule m_vertexShader;
		ShaderModule m_pixelShader;
		ashes::RenderPassPtr m_renderPass;
		ashes::FrameBufferPtr m_frameBuffer;
		RenderPassTimerSPtr m_timer;
		ashes::SemaphorePtr m_finished;
	};
}

#endif
