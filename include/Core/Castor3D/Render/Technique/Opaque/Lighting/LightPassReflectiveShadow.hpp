/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightPassReflectiveShadow_H___
#define ___C3D_LightPassReflectiveShadow_H___

#include "LightPassShadow.hpp"

#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Render/Passes/DownscalePass.hpp"
#include "Castor3D/Render/Technique/Opaque/OpaquePassResult.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/LightPassResult.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/RsmGIPass.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/RsmInterpolatePass.hpp"

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
			, m_downscalePass{ engine
				, {
					lpResult[LpTexture::eDiffuse].getTexture()->getDefaultView().getTargetView(),
					gpResult[DsTexture::eData1].getTexture()->getDefaultView().getTargetView(),
				}
				, {
					lpResult[LpTexture::eDiffuse].getTexture()->getWidth() >> 2,
					lpResult[LpTexture::eDiffuse].getTexture()->getHeight() >> 2,
				} }
			, m_rsmGiPass{ engine
				, lightCache
				, LtType
				, {
					lpResult[LpTexture::eDiffuse].getTexture()->getWidth() >> 2,
					lpResult[LpTexture::eDiffuse].getTexture()->getHeight() >> 2,
				}
				, gpInfoUbo
				, gpResult
				, smResult
				, m_downscalePass.getResult() }
			, m_interpolatePass{ engine
				, lightCache
				, LtType
				, {
					lpResult[LpTexture::eDiffuse].getTexture()->getWidth(),
					lpResult[LpTexture::eDiffuse].getTexture()->getHeight(),
				}
				, gpInfoUbo
				, gpResult
				, smResult
				, m_rsmGiPass.getConfigUbo()
				, m_rsmGiPass.getSamplesSsbo()
				, m_rsmGiPass.getResult()[0]
				, m_rsmGiPass.getResult()[1]
				, lpResult[LpTexture::eDiffuse] }
		{
		}

		ashes::Semaphore const & render( uint32_t index
			, ashes::Semaphore const & toWait )override
		{
			auto result = &toWait;
			result = &my_pass_type::render( index, *result );
			result = &m_downscalePass.compute( *result );
			result = &m_rsmGiPass.compute( *result );
			result = &m_interpolatePass.compute( *result );
			return *result;
		}

		void accept( PipelineVisitorBase & visitor )override
		{
			LightPassShadow< LtType >::accept( visitor );
			m_rsmGiPass.accept( visitor );
			m_downscalePass.accept( visitor );
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
			m_rsmGiPass.update( light );
		}

	private:
		DownscalePass m_downscalePass;
		RsmGIPass m_rsmGiPass;
		RsmInterpolatePass m_interpolatePass;
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
