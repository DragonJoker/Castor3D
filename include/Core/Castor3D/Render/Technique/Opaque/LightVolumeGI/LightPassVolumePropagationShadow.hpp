/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightPassVolumePropagationShadow_H___
#define ___C3D_LightPassVolumePropagationShadow_H___

#include "LightVolumeGIModule.hpp"

#include "Castor3D/Render/Technique/Opaque/Lighting/LightPassShadow.hpp"

#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"
#include "Castor3D/Render/Technique/Opaque/LightVolumeGI/LightVolumeGIPass.hpp"
#include "Castor3D/Render/Passes/DownscalePass.hpp"
#include "Castor3D/Render/Technique/Opaque/OpaquePassResult.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Light/DirectionalLight.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"
#include "Castor3D/Shader/Ubos/LpvGridConfigUbo.hpp"

#include <CastorUtils/Miscellaneous/StringUtils.hpp>

namespace castor3d
{
	template< LightType LtType >
	class LightPassVolumePropagationShadowT
		: public LightPassShadow< LtType >
	{
	public:
		using my_traits = LightPassShadowTraits< LtType >;
		using my_light_type = typename my_traits::light_type;
		using my_pass_type = typename my_traits::light_pass_type;
		using my_shadow_pass_type = typename my_traits::shadow_pass_type;

		static constexpr uint32_t MaxPropagationSteps = 8u;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	device		The GPU device.
		 *\param[in]	lpConfig	The light pass configuration.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	device		Le device GPU.
		 *\param[in]	lpConfig	La configuration de la passe d'éclairage.
		 */
		LightPassVolumePropagationShadowT( crg::FrameGraph & graph
			, crg::FramePass const *& previousPass
			, RenderDevice const & device
			, LpvLightPassConfig lpConfig )
			: LightPassShadow< LtType >{ graph
				, previousPass
				, device
				, "LPVShadow"
				, { lpConfig.base.lpResult, lpConfig.base.gpInfoUbo, true, false, true } }
			, m_gpResult{ lpConfig.gpResult }
			, m_smResult{ lpConfig.smResult }
			, m_lpResult{ lpConfig.base.lpResult }
			, m_lpvResult{ lpConfig.lpvResult }
			, m_gpInfoUbo{ lpConfig.base.gpInfoUbo }
			, m_lpvConfigUbo{ lpConfig.lpvConfigUbo }
			, m_lightVolumeGIPasses{ castor::makeUnique< LightVolumeGIPass >( graph
					, previousPass
					, m_device
					, getName()
					, LtType
					, m_gpInfoUbo
					, m_lpvConfigUbo
					, m_gpResult
					, m_lpvResult
					, m_lpResult[LpTexture::eIndirectDiffuse].targetViewId
					, BlendMode::eNoBlend )
				, castor::makeUnique< LightVolumeGIPass >( graph
					, previousPass
					, m_device
					, getName()
					, LtType
					, m_gpInfoUbo
					, m_lpvConfigUbo
					, m_gpResult
					, m_lpvResult
					, m_lpResult[LpTexture::eIndirectDiffuse].targetViewId
					, BlendMode::eAdditive ) }
		{
		}
		/**
		 *\copydoc		castor3d::LightPass::initialise
		 */
		void initialise( Scene const & scene
			, OpaquePassResult const & gp
			, SceneUbo & sceneUbo
			, RenderPassTimer & timer )override
		{
			auto & lightCache = scene.getLightCache();
			LightPassShadow< LtType >::initialise( scene, gp, sceneUbo, timer );
		}
		/**
		 *\copydoc		castor3d::LightPass::cleanup
		 */
		void cleanup()override
		{
			LightPassShadow< LtType >::cleanup();
			m_lightVolumeGIPasses = {};
		}
		/**
		 *\copydoc		castor3d::LightPass::render
		 */
		ashes::Semaphore const & render( uint32_t index
			, ashes::Semaphore const & toWait )override
		{
			auto result = &toWait;
			result = &my_pass_type::render( index, *result );
			result = &m_lightVolumeGIPasses[index == 0 ? 0 : 1]->compute( *result );
			return *result;
		}
		/**
		 *\copydoc		castor3d::LightPass::accept
		 */
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
		LightVolumePassResult const & m_lpvResult;
		GpInfoUbo const & m_gpInfoUbo;
		LpvGridConfigUbo const & m_lpvConfigUbo;
		LightVolumeGIPassArray m_lightVolumeGIPasses;
	};
}

#endif
