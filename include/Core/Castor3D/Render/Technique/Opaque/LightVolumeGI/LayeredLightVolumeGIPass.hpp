/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LayeredLightVolumeGIPass_HPP___
#define ___C3D_LayeredLightVolumeGIPass_HPP___

#include "LightVolumeGIModule.hpp"

#include "Castor3D/Buffer/UniformBuffer.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapModule.hpp"
#include "Castor3D/Render/Technique/Opaque/OpaqueModule.hpp"
#include "Castor3D/Render/Passes/RenderQuad.hpp"
#include "Castor3D/Scene/Light/LightModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include <ShaderAST/Shader.hpp>

namespace castor3d
{
	class LayeredLightVolumeGIPass
		: public RenderQuad
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine			The engine.
		 *\param[in]	device			The GPU device.
		 *\param[in]	prefix			The pass name's prefix.
		 *\param[in]	gpInfo			The GBuffer configuration UBO.
		 *\param[in]	lpvConfigUbo	The LPV configuration UBO.
		 *\param[in]	gpResult		The GBuffer.
		 *\param[in]	lpvResult		The LPV result.
		 *\param[in]	dst				The GI result.
		 *\param[in]	blendMode		The blend mode.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine			Le moteur.
		 *\param[in]	device			Le device GPU.
		 *\param[in]	prefix			Le préfixe du nom de la passe.
		 *\param[in]	gpInfo			L'UBO de configuration du GBuffer.
		 *\param[in]	lpvConfigUbo	L'UBO de configuration des LPV.
		 *\param[in]	gpResult		Le GBuffer.
		 *\param[in]	lpvResult		Le résultat du LPV.
		 *\param[in]	dst				Le résultat de GI.
		 *\param[in]	blendMode		Le mode de mélange.
		 */
		C3D_API LayeredLightVolumeGIPass( Engine & engine
			, RenderDevice const & device
			, castor::String const & prefix
			, GpInfoUbo const & gpInfo
			, LayeredLpvGridConfigUbo const & lpvConfigUbo
			, OpaquePassResult const & gpResult
			, LightVolumePassResultArray const & lpvResult
			, TextureUnit const & dst
			, BlendMode blendMode );
		/**
		 *\~english
		 *\brief		Renders the pass.
		 *\param[in]	toWait	The semaphore from the previous render pass.
		 *\~french
		 *\brief		Dessine la passe.
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
		GpInfoUbo const & m_gpInfo;
		LayeredLpvGridConfigUbo const & m_lpvConfigUbo;
		OpaquePassResult const & m_gpResult;
		LightVolumePassResultArray const & m_lpvResult;
		TextureUnit const & m_result;
		ShaderModule m_vertexShader;
		ShaderModule m_pixelShader;
		ashes::RenderPassPtr m_renderPass;
		ashes::FrameBufferPtr m_frameBuffer;
		RenderPassTimerSPtr m_timer;
		CommandsSemaphore m_commands;
	};
}

#endif
