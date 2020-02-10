/*
See LICENSE file in root folder
*/
#ifndef ___C3D_OpaqueTechniqueModule_H___
#define ___C3D_OpaqueTechniqueModule_H___

#include "Castor3D/Technique/TechniqueModule.hpp"

#include "Castor3D/Scene/Light/LightModule.hpp"

namespace castor3d
{
	/**@name Render */
	//@{
	/**@name Technique */
	//@{
	/**@name Opaque */
	//@{

	/**
	*\~english
	*\brief
	*	Enumerator of textures used in deferred rendering.
	*\~french
	*\brief
	*	Enumération des textures utilisées lors du rendu différé.
	*/
	enum class DsTexture
		: uint8_t
	{
		eDepth,
		eData1, // RGB => Normal, A => Object Material flags
		eData2, // RGB => Diffuse/Albedo - SSR: A => Shininess - PBRMR: A => Unused - PBRSG: A => Glossiness
		eData3, // A => AO - SSR/PBRSG: RGB => Specular - PBRMR: R => Metallic, G => Roughness, B => Unused
		eData4, // RGB => Emissive, A => Transmittance
		eData5, // RG => Velocity, B => Material Index, A => Unused
		CU_ScopedEnumBounds( eDepth ),
	};
	/*
	*\~english
	*\brief
	*	The deferred rendering class.
	*\~french
	*\brief
	*	Classe de rendu différé.
	*/
	class DeferredRendering;
	/**
	*\~english
	*\brief
	*	The result of the geometry pass in deferred rendering.
	*\~french
	*\brief
	*	Résultat de la passe de géométries dans le rendu différé.
	*/
	class GeometryPassResult;
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
	*	Deferred lighting Render technique pass.
	*\~french
	*\brief
	*	Classe de passe de technique de rendu implémentant le Deferred lighting.
	*/
	class OpaquePass;
	/**
	*\~english
	*\brief
	*	The post lighting reflection pass.
	*\~french
	*\brief
	*	La passe de réflexion post éclairage.
	*/
	class ReflectionPass;
	/**
	*\~english
	*\brief
	*	Screen Space Ambient Occlusion pass.
	*\~french
	*\brief
	*	Passe de Occlusion Ambiante en Espace Ecran.
	*/
	class SsaoPass;
	/**
	*\~english
	*\brief
	*	Stencil pre-pass for light passes needing a mesh.
	*\~french
	*\brief
	*	Pré-passe de stencil pour les passes d'éclairage nécessitant un maillage.
	*/
	class StencilPass;
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
	*	Retrieve the name for given texture enum value.
	*\param[in] texture
	*	The value.
	*\return
	*	The name.
	*\~french
	*\brief
	*	Récupère le nom pour la valeur d'énumeration de texture.
	*\param[in] texture
	*	La valeur.
	*\return
	*	Le nom.
	*/
	castor::String getTextureName( DsTexture texture );
	/**
	*\~english
	*\brief
	*	Retrieve the pixel format for given texture enum value.
	*\param[in] texture
	*	The value.
	*\return
	*	The pixel format.
	*\~french
	*\brief
	*	Récupère le format de pixels pour la valeur d'énumeration de texture.
	*\param[in] texture
	*	La valeur.
	*\return
	*	Le format de pixels.
	*/
	VkFormat getTextureFormat( DsTexture texture );
	/**
	*\~english
	*\brief
	*	Retrieve the attachment index for given texture enum value.
	*\param[in] texture
	*	The value.
	*\return
	*	The attachment index.
	*\~french
	*\brief
	*	Récupère l'indice d'attache pour la valeur d'énumeration de texture.
	*\param[in] texture
	*	La valeur.
	*\return
	*	L'indice d'attache.
	*/
	/**
	*\~english
	*\brief		Retrieve the attachment index for given texture enum value.
	*\param[in]	texture	The value.
	*\return		The name.
	*\~french
	*\brief		Récupère l'indice d'attache pour la valeur d'énumeration de texture.
	*\param[in]	texture	La valeur.
	*\return		Le nom.
	*/
	uint32_t getTextureAttachmentIndex( DsTexture texture );
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
}

#endif
