/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ReflectiveShadowMapsModule_H___
#define ___C3D_ReflectiveShadowMapsModule_H___

#include "Castor3D/Cache/CacheModule.hpp"
#include "Castor3D/Render/Opaque/OpaqueModule.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapModule.hpp"
#include "Castor3D/Scene/Light/LightModule.hpp"

namespace castor3d
{
	/**@name Render */
	//@{
	/**@name Global Illumination */
	//@{
	/**@name Reflective Shadow Maps */
	//@{

	/**
	*\~english
	*\brief
	*	Reflective Shadow Map shader helpers.
	*\~french
	*\brief
	*	classe d'aide pour les shaders de Reflective Shadow Map.
	*/
	class ReflectiveShadowMapping;
	/**
	*\~english
	*\brief
	*	Reflective Shadow Maps configuration values.
	*\~french
	*\brief
	*	Valeurs de configuration des Reflective Shadow Maps.
	*/
	struct RsmConfig;
	/**
	*\~english
	*\brief
	*	SSGI pass based on Reflective Shadow Maps.
	*\~french
	*\brief
	*	Passe de SSGI basée sur les Reflective Shadow Maps.
	*/
	class RsmGIPass;
	/**
	*\~english
	*\brief
	*	RSM GI interpolation pass, used to optimise the algorithm.
	*\~french
	*\brief
	*	Passe d'interpolation du RSM GI, pour optimiser l'algorithme.
	*/
	class RsmInterpolatePass;
	/**
	*\~english
	*\brief
	*	RSM full pass.
	*\~french
	*\brief
	*	Passe de RSM.
	*/
	class ReflectiveShadowMaps;

	CU_DeclareSmartPtr( castor3d, RsmGIPass, C3D_API );
	CU_DeclareSmartPtr( castor3d, RsmInterpolatePass, C3D_API );
	CU_DeclareSmartPtr( castor3d, ReflectiveShadowMaps, C3D_API );

	//@}
	//@}
	//@}
}

#endif
