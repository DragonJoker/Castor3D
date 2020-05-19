/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightPassReflectiveShadow_H___
#define ___C3D_LightPassReflectiveShadow_H___

#include "LightPassShadow.hpp"

#include "Castor3D/Render/Technique/Opaque/Lighting/RsmGIPass.hpp"

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
			, LightPassResult const & lpResult
			, GpInfoUbo const & gpInfoUbo )
			: my_shadow_pass_type{ engine
				, lpResult
				, gpInfoUbo
				, true }
			, m_rsmGiPass{ }
		{
		}

	private:
		RsmGIPass m_rsmGiPass;
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
