/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightPassLayeredVolumePropagationShadow_H___
#define ___C3D_LightPassLayeredVolumePropagationShadow_H___

#include "Castor3D/Render/Technique/Opaque/LightVolumeGI/LightVolumeGIModule.hpp"

#include "Castor3D/Render/Technique/Opaque/Lighting/LightPassShadow.hpp"

#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"
#include "Castor3D/Render/Passes/DownscalePass.hpp"
#include "Castor3D/Render/Technique/Opaque/OpaquePassResult.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/LightPassResult.hpp"
#include "Castor3D/Render/Technique/Opaque/LightVolumeGI/LayeredLightVolumeGIPass.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Light/DirectionalLight.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Shader/Ubos/LayeredLpvGridConfigUbo.hpp"

#include <CastorUtils/Miscellaneous/StringUtils.hpp>

#include <array>

namespace castor3d
{
	template<>
	class LightPassLayeredVolumePropagationShadowT< LightType::eDirectional >
		: public LightPassShadow< LightType::eDirectional >
	{
	public:
		using my_traits = LightPassShadowTraits< LightType::eDirectional >;
		using my_light_type = typename my_traits::light_type;
		using my_pass_type = typename my_traits::light_pass_type;
		using my_shadow_pass_type = typename my_traits::shadow_pass_type;

		static constexpr uint32_t MaxPropagationSteps = 8u;
		static constexpr LightType LtType = LightType::eDirectional;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine		The engine.
		 *\param[in]	lpResult	The light pass result.
		 *\param[in]	gpInfoUbo	The geometry pass UBO.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	lpResult	Le résultat de la passe d'éclairage.
		 *\param[in]	gpInfoUbo	L'UBO de la geometry pass.
		 */
		LightPassLayeredVolumePropagationShadowT( RenderDevice const & device
			, LayeredLpvLightPassConfig lpConfig )
			: LightPassShadow< LtType >{ device
				, "LayeredLPVShadow"
				, { lpConfig.base.lpResult, lpConfig.base.gpInfoUbo, true, true } }
			, m_gpResult{ lpConfig.gpResult }
			, m_smResult{ lpConfig.smResult }
			, m_lpResult{ lpConfig.base.lpResult }
			, m_lpvResult{ lpConfig.lpvResult }
			, m_gpInfoUbo{ lpConfig.base.gpInfoUbo }
			, m_lpvConfigUbo{ lpConfig.lpvConfigUbo }
		{
		}

		void initialise( Scene const & scene
			, OpaquePassResult const & gp
			, SceneUbo & sceneUbo
			, RenderPassTimer & timer )override
		{
			auto & lightCache = scene.getLightCache();
			m_lightVolumeGIPasses = { castor::makeUnique< LayeredLightVolumeGIPass >( this->m_engine
					, this->m_device
					, this->getName()
					, m_gpInfoUbo
					, m_lpvConfigUbo
					, m_gpResult
					, m_lpvResult
					, m_lpResult[LpTexture::eIndirectDiffuse]
					, BlendMode::eNoBlend )
				, castor::makeUnique< LayeredLightVolumeGIPass >( this->m_engine
					, this->m_device
					, this->getName()
					, m_gpInfoUbo
					, m_lpvConfigUbo
					, m_gpResult
					, m_lpvResult
					, m_lpResult[LpTexture::eIndirectDiffuse]
					, BlendMode::eAdditive ) };
			LightPassShadow< LtType >::initialise( scene, gp, sceneUbo, timer );
		}

		void cleanup()override
		{
			LightPassShadow< LtType >::cleanup();
			m_lightVolumeGIPasses = {};
		}

		ashes::Semaphore const & render( uint32_t index
			, ashes::Semaphore const & toWait )override
		{
			auto result = &toWait;
			result = &my_pass_type::render( index, *result );
			result = &m_lightVolumeGIPasses[index == 0 ? 0 : 1]->compute( *result );
			return *result;
		}

		void accept( PipelineVisitorBase & visitor )override
		{
			LightPassShadow< LtType >::accept( visitor );

			if ( m_lightVolumeGIPasses[0] )
			{
				m_lightVolumeGIPasses[0]->accept( visitor );
			}
		}

	private:
		VkClearValue doGetIndirectClearColor()const override
		{
			return opaqueBlackClearColor;
		}

	private:
		OpaquePassResult const & m_gpResult;
		ShadowMapResult const & m_smResult;
		LightPassResult const & m_lpResult;
		LightVolumePassResultArray const & m_lpvResult;
		GpInfoUbo const & m_gpInfoUbo;
		LayeredLpvGridConfigUbo const & m_lpvConfigUbo;
		LayeredLightVolumeGIPassArray m_lightVolumeGIPasses;
	};
}

#endif
