/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightVolumeGIModule_H___
#define ___C3D_LightVolumeGIModule_H___

#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightPropagationVolumesModule.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/LightingModule.hpp"

#include "Castor3D/Scene/Light/LightModule.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"

#include <CastorUtils/Graphics/Size.hpp>

namespace castor3d
{
	/**@name Render */
	//@{
	/**@name Technique */
	//@{
	/**@name Opaque */
	//@{
	/**@name Light Propagation Volumes */
	//@{

	/**
	*\~english
	*\brief
	*	Lighting propagation volumes resolution pass.
	*\~french
	*\brief
	*	Passe de résolution du light propagation volumes.
	*/
	class LayeredLightVolumeGIPass;
	/**
	*\~english
	*\brief
	*	Lighting propagation volumes resolution pass.
	*\~french
	*\brief
	*	Passe de résolution du light propagation volumes.
	*/
	class LightVolumeGIPass;
	/**
	*\~english
	*\brief
	*	Base class for all light passes with light propagation volumes.
	*\~french
	*\brief
	*	Classe de base pour toutes les passes d'éclairage avec du light propagation volumes.
	*/
	template< LightType LtType >
	class LightPassVolumePropagationShadowT;
	/**
	*\~english
	*\brief
	*	Base class for all light passes with light propagation volumes.
	*\~french
	*\brief
	*	Classe de base pour toutes les passes d'éclairage avec du light propagation volumes.
	*/
	template< LightType LtType >
	class LightPassLayeredVolumePropagationShadowT;

	//!\~english	The directional lights light pass with shadows and LPV.
	//!\~french		La passe d'éclairage avec ombres et LPV pour les lumières directionnelles.
	using DirectionalLightPassLPVShadow = LightPassVolumePropagationShadowT< LightType::eDirectional >;
	//!\~english	The spot lights light pass with shadows and LPV.
	//!\~french		La passe d'éclairage avec ombres et LPV pour les lumières projecteurs.
	using SpotLightPassLPVShadow = LightPassVolumePropagationShadowT< LightType::eSpot >;
	//!\~english	The directional lights light pass with shadows and LPV.
	//!\~french		La passe d'éclairage avec ombres et LPV pour les lumières directionnelles.
	using DirectionalLightPassLayeredLPVShadow = LightPassLayeredVolumePropagationShadowT< LightType::eDirectional >;

	CU_DeclareCUSmartPtr( castor3d, LayeredLightVolumeGIPass, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, LightVolumeGIPass, C3D_API );

	CU_DeclareArray( LayeredLightVolumeGIPassUPtr, lpv::ResolvePassCount, LayeredLightVolumeGIPass );
	CU_DeclareArray( LightVolumeGIPassUPtr, lpv::ResolvePassCount, LightVolumeGIPass );

	struct LpvLightPassConfig
	{
		LightPassConfig base;
		LightCache const & lightCache;
		OpaquePassResult const & gpResult;
		ShadowMapResult const & smResult;
		LightVolumePassResult const & lpvResult;
		LpvConfigUbo const & lpvConfigUbo;
	};
	
	struct LayeredLpvLightPassConfig
	{
		LightPassConfig base;
		LightCache const & lightCache;
		OpaquePassResult const & gpResult;
		ShadowMapResult const & smResult;
		LightVolumePassResult const & lpvResult;
		LayeredLpvConfigUbo const & lpvConfigUbo;
		std::vector< LpvConfigUbo > const & lpvConfigUbos;
	};

	//@}
	//@}
	//@}
	//@}
}

#endif
