/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightingModule_H___
#define ___C3D_LightingModule_H___

#include "Castor3D/Render/Technique/Opaque/OpaqueModule.hpp"

#include "Castor3D/Scene/Light/LightModule.hpp"

namespace castor3d
{
	/**@name Render */
	//@{
	/**@name Technique */
	//@{
	/**@name Opaque */
	//@{
	/**@name Lighting */
	//@{

	/**
	*\~english
	*\brief
	*	Enumerator of textures used by the lighting pass in deferred rendering.
	*\~french
	*\brief
	*	Enumération des textures utilisées par la passe d'éclairage lors du rendu différé.
	*/
	enum class LpTexture
		: uint8_t
	{
		eDepth,
		eDiffuse,
		eSpecular,
		CU_ScopedEnumBounds( eDepth ),
	};
	castor::String getTextureName( LpTexture texture );
	castor::String getName( LpTexture texture );
	VkFormat getFormat( LpTexture texture );
	VkClearValue getClearValue( LpTexture texture );
	VkImageUsageFlags getUsageFlags( LpTexture texture );
	VkBorderColor getBorderColor( LpTexture texture );
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
	*	Reflective Shadow Maps based GI UBO configuration.
	*\~french
	*\brief
	*	Configuration de l'UBO du GI basé sur les Reflective Shadow Maps.
	*/
	struct RsmGIConfig
	{
		castor::Matrix4x4f lightViewProj;
		float intensity;
		float maxRadius;
		uint32_t sampleCount;
		uint32_t index;
	};
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
	*	Handles the light passes.
	*\~french
	*\brief
	*	Gère les passes d'éclairage.
	*/
	class LightingPass;
	/**
	*\~english
	*\brief
	*	The result of the light pass in deferred rendering.
	*\~french
	*\brief
	*	Résultat de la passe d'éclairage dans le rendu différé.
	*/
	class LightPassResult;
	/**
	*\~english
	*\brief
	*	Base class for all light passes.
	*\remarks
	*	The result of each light pass is blended with the previous one.
	*\~french
	*\brief
	*	Classe de base pour toutes les passes d'éclairage.
	*\remarks
	*	Le résultat de chaque passe d'éclairage est mélangé avec celui de la précédente.
	*/
	class LightPass;
	/**
	*\~english
	*\brief
	*	Traits structure to specialise light passes with shadows.
	*\~french
	*\brief
	*	Structure de traits pour spécialiser les passes d'éclairage avec des ombres.
	*/
	template< LightType LtType >
	struct LightPassShadowTraits;
	/**
	*\~english
	*\brief
	*	Base class for all light passes with shadow.
	*\~french
	*\brief
	*	Classe de base pour toutes les passes d'éclairage avec des ombres.
	*/
	template< LightType LtType >
	class LightPassShadow;
	/**
	*\~english
	*\brief
	*	Base class for all light passes with reflective shadow maps.
	*\~french
	*\brief
	*	Classe de base pour toutes les passes d'éclairage avec des reflective shadow maps.
	*/
	template< LightType LtType >
	class LightPassReflectiveShadow;
	/**
	*\~english
	*\brief
	*	Directional light pass.
	*\~french
	*\brief
	*	Passe de lumière directionnelle.
	*/
	class DirectionalLightPass;
	/**
	*\~english
	*\brief
	*	Base class for light passes that need a mesh instead of a quad.
	*\~french
	*\brief
	*	Classe de base pour les passes d'éclairage nécessitant un maillage plutôt qu'un quad.
	*/
	class MeshLightPass;
	/**
	*\~english
	*\brief
	*	Point light pass.
	*\~french
	*\brief
	*	Passe de lumière omnidirectionnelle.
	*/
	class PointLightPass;
	/**
	*\~english
	*\brief
	*	Spot light pass.
	*\~french
	*\brief
	*	Passe de lumière projecteur.
	*/
	class SpotLightPass;
	/**
	*\~english
	*\brief
	*	Subsurface scattering pass.
	*\~french
	*\brief
	*	Passe de Subsurface scattering.
	*/
	class SubsurfaceScatteringPass;
	/**
	*\~english
	*\brief
	*	Retrieve the maximum litten distance for given light and attenuation.
	*\param[in] light
	*	The light source.
	*\param[in] attenuation
	*	The attenuation values.
	*\return
	*	The value.
	*\~french
	*\brief
	*	Récupère l'indice d'attache pour la valeur d'énumeration de texture.
	*\param[in] light
	*	La source lumineuse.
	*\param[in] attenuation
	*	Les valeurs d'atténuation.
	*\return
	*	La valeur.
	*/
	float getMaxDistance( LightCategory const & light
		, castor::Point3f const & attenuation );
	/**
	*\~english
	*\brief
	*	Retrieve the maximum litten distance for given light and attenuation.
	*\param[in] light
	*	The light source.
	*\param[in] attenuation
	*	The attenuation values.
	*\param[in] max
	*	The viewer max value.
	*\return
	*	The value.
	*\~french
	*\brief
	*	Récupère l'indice d'attache pour la valeur d'énumeration de texture.
	*\param[in] light
	*	La source lumineuse.
	*\param[in] attenuation
	*	Les valeurs d'atténuation.
	*\param[in] max
	*	La valeur maximale de l'observateur.
	*\return
	*	La valeur.
	*/
	float getMaxDistance( LightCategory const & light
		, castor::Point3f const & attenuation
		, float max );

	//@}
	//@}
	//@}
	//@}
}

#endif
