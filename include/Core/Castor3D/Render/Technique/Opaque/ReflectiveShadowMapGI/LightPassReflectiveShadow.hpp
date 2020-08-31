/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightPassReflectiveShadow_H___
#define ___C3D_LightPassReflectiveShadow_H___

#include "ReflectiveShadowMapGIModule.hpp"

#include "Castor3D/Render/Technique/Opaque/Lighting/LightPassShadow.hpp"

#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Render/Passes/DownscalePass.hpp"
#include "Castor3D/Render/Technique/Opaque/OpaquePassResult.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/LightPassResult.hpp"
#include "Castor3D/Render/Technique/Opaque/ReflectiveShadowMapGI/RsmGIPass.hpp"
#include "Castor3D/Render/Technique/Opaque/ReflectiveShadowMapGI/RsmInterpolatePass.hpp"

namespace castor3d
{
	template< LightType LtType >
	class LightPassReflectiveShadow
		: public LightPassShadow< LtType >
	{
	public:
		using my_traits = LightPassShadowTraits< LtType >;
		using my_light_type = typename my_traits::light_type;
		using my_pass_type = typename my_traits::light_pass_type;
		using my_shadow_pass_type = typename my_traits::shadow_pass_type;

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
		LightPassReflectiveShadow( Engine & engine
			, LightCache const & lightCache
			, OpaquePassResult const & gpResult
			, ShadowMapResult const & smResult
			, LightPassResult const & lpResult
			, GpInfoUbo const & gpInfoUbo )
			: LightPassShadow< LtType >{ engine
				, lpResult
				, gpInfoUbo }
		{
		}


		void initialise( Scene const & scene
			, OpaquePassResult const & gp
			, SceneUbo & sceneUbo
			, RenderPassTimer & timer )override
		{
			auto & lightCache = scene.getLightCache();
			m_downscalePass = std::make_unique< DownscalePass >( m_engine
				, cuT( "Reflective Shadow Maps" )
				, ashes::ImageViewArray
				{
					m_lpResult[LpTexture::eDiffuse].getTexture()->getDefaultView().getTargetView(),
					m_gpResult[DsTexture::eData1].getTexture()->getDefaultView().getTargetView(),
				}
				, VkExtent2D
				{
					m_lpResult[LpTexture::eDiffuse].getTexture()->getWidth() >> 2,
					m_lpResult[LpTexture::eDiffuse].getTexture()->getHeight() >> 2,
				} );
			m_rsmGiPass = std::make_unique< RsmGIPass >( m_engine
				, lightCache
				, LtType
				, VkExtent2D
				{
					m_lpResult[LpTexture::eDiffuse].getTexture()->getWidth() >> 2,
					m_lpResult[LpTexture::eDiffuse].getTexture()->getHeight() >> 2,
				}
				, m_gpInfoUbo
				, m_gpResult
				, m_smResult
				, m_downscalePass.getResult() );
			m_interpolatePass = std::make_unique< RsmInterpolatePass >( m_engine
					, lightCache
					, LtType
					, VkExtent2D
					{
						m_lpResult[LpTexture::eDiffuse].getTexture()->getWidth(),
						m_lpResult[LpTexture::eDiffuse].getTexture()->getHeight(),
					}
					, m_gpInfoUbo
					, m_gpResult
					, m_smResult
					, m_rsmGiPass.getConfigUbo()
					, m_rsmGiPass.getSamplesSsbo()
					, m_rsmGiPass.getResult()[0]
					, m_rsmGiPass.getResult()[1]
					, m_lpResult[LpTexture::eDiffuse] );
			LightPassShadow< LtType >::initialise( scene, gp, sceneUbo, timer );
		}

		void cleanup()override
		{
			LightPassShadow< LtType >::cleanup();
			m_interpolatePass.reset();
			m_rsmGiPass.reset();
			m_downscalePass.reset();
		}

		ashes::Semaphore const & render( uint32_t index
			, ashes::Semaphore const & toWait )override
		{
			auto result = &toWait;
			result = &my_pass_type::render( index, *result );
			result = &m_downscalePass->compute( *result );
			result = &m_rsmGiPass->compute( *result );
			result = &m_interpolatePass->compute( *result );

			return *result;
		}

		void accept( PipelineVisitorBase & visitor )override
		{
			LightPassShadow< LtType >::accept( visitor );

			if ( m_rsmGiPass )
			{
				m_rsmGiPass->accept( visitor );
			}

			if ( m_downscalePass )
			{
				m_downscalePass->accept( visitor );
			}
		}

	protected:
		void doUpdate( bool first
			, castor::Size const & size
			, Light const & light
			, Camera const & camera
			, ShadowMap const * shadowMap
			, uint32_t shadowMapIndex )override
		{
			my_pass_type::doUpdate( first
				, size
				, light
				, camera
				, shadowMap
				, shadowMapIndex );
			m_rsmGiPass->update( light );
		}

	private:
		OpaquePassResult const & m_gpResult;
		ShadowMapResult const & m_smResult;
		LightPassResult const & m_lpResult;
		GpInfoUbo const & m_gpInfoUbo;
		std::unique_ptr< DownscalePass > m_downscalePass;
		std::unique_ptr< RsmGIPass > m_rsmGiPass;
		std::unique_ptr< RsmInterpolatePass > m_interpolatePass;
	};
	//!\~english	The directional lights light pass with shadows.
	//!\~french		La passe d'éclairage avec ombres pour les lumières directionnelles.
	using DirectionalLightPassReflectiveShadow = LightPassReflectiveShadow< LightType::eDirectional >;
	//!\~english	The point lights light pass with shadows.
	//!\~french		La passe d'éclairage avec ombres pour les lumières omnidirectionnelles.
	using PointLightPassReflectiveShadow = LightPassReflectiveShadow< LightType::ePoint >;
	//!\~english	The spot lights light pass with shadows.
	//!\~french		La passe d'éclairage avec ombres pour les lumières projecteurs.
	using SpotLightPassReflectiveShadow = LightPassReflectiveShadow< LightType::eSpot >;
}

#endif
