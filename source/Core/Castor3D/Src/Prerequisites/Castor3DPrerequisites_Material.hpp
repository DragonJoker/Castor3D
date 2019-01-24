/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PREREQUISITES_MATERIAL_H___
#define ___C3D_PREREQUISITES_MATERIAL_H___

#include <Design/Signal.hpp>

namespace castor3d
{
	/**@name Material */
	//@{
	
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		02/12/2016
	\~english
	\brief		Supported material types enumeration.
	\~french
	\brief		Enumération des types de matétiaux supportés.
	*/
	enum class MaterialType
	{
		//!\~english	Traditional (pre-PBR).
		//!\~french		Traditionnel (pré-PBR).
		eLegacy,
		//!\~english	Metallic/Roughness PBR.
		//!\~french		PBR Metallic/Roughness.
		ePbrMetallicRoughness,
		//!\~english	Specular/Glossiness PBR.
		//!\~french		PBR Specular/Glossiness.
		ePbrSpecularGlossiness,
		CU_ScopedEnumBounds( eLegacy )
	};
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Blending modes enumeration
	\~french
	\brief		Enumération des modes de mélange
	*/
	enum class BlendMode
		: uint8_t
	{
		//!\~english Order dependent blending.
		//!\~french Mélange dépendant de l'ordre.
		eNoBlend,
		//!\~english Order independent, add the components.
		//!\~french Mélange indépendant de l'ordre, additionnant les composantes.
		eAdditive,
		//!\~english Order independent, multiply the components.
		//!\~french Indépendant de l'ordre, multipliant les composantes.
		eMultiplicative,
		//!\~english Order dependent, interpolate the components.
		//!\~french Indépendant de l'ordre, interpolant les composantes.
		eInterpolative,
		//!\~english Order independent, using A-buffer, not implemented yet.
		//!\~french Indépendant de l'ordre, utilisant les A-Buffer (non implémenté).
		eABuffer,
		//!\~english Order independent, using depth peeling, not implemented yet.
		//!\~french Indépendant de l'ordre, utilisant le pelage en profondeur (non implémenté).
		eDepthPeeling,
		CU_ScopedEnumBounds( eNoBlend )
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/09/2017
	\version	0.10.0
	\~english
	\brief		Pass specific flags.
	\~french
	\brief		Indicateurs spécifiques à la passe.
	*/
	enum class PassFlag
		: uint8_t
	{
		eAlphaBlending = 0x01,
		//!\~english	Shader using alpha test.
		//\~french		Shader utilisant un test alpha.
		eAlphaTest = 0x02,
		//!\~english	Shader for Metallic/Roughness PBR material (if not set, will use legacy material code).
		//\~french		Shader pour un matériau PBR Metallic/Roughness (si non défini, utilisera le code de matériau traditionnel).
		ePbrMetallicRoughness = 0x04,
		//!\~english	Shader for Specular/Glossiness PBR material (if not set, will use legacy material code).
		//\~french		Shader pour un matériau PBR Specular/Glossiness (si non défini, utilisera le code de matériau traditionnel).
		ePbrSpecularGlossiness = 0x08,
		//!\~english	Shader for Subsurface Scattering.
		//\~french		Shader pour le subsurface scattering.
		eSubsurfaceScattering = 0x10,
		//!\~english	Shader using distance based transmittance when computing Subsurface Scattering.
		//\~french		Shader utilisant la transmission basée sur la distance, lors du calcul du Subsurface Scattering.
		eDistanceBasedTransmittance = 0x20,
		//!\~english	Shader using parallax occlusion mapping.
		//\~french		Shader utilisant le parallax occlusion mapping.
		eParallaxOcclusionMapping = 0x40,
	};
	CU_ImplementFlags( PassFlag )
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Cube map faces.
	\~french
	\brief		Les faces d'une cube map.
	*/
	enum class CubeMapFace
		: uint8_t
	{
		//!\~english Face on positive X
		//!\~french Face des X positifs.
		ePositiveX,
		//!\~english Face on negative X
		//!\~french Face des X négatifs.
		eNegativeX,
		//!\~english Face on positive Y
		//!\~french Face des Y positifs.
		ePositiveY,
		//!\~english Face on negative Y
		//!\~french Face des Y négatifs.
		eNegativeY,
		//!\~english Face on positive Z
		//!\~french Face des Z positifs.
		ePositiveZ,
		//!\~english Face on negative Z
		//!\~french Face des Z négatifs.
		eNegativeZ,
		CU_ScopedEnumBounds( ePositiveX )
	};
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Texture channels enumeration
	\~french
	\brief		Enumération des canaux de texture
	*/
	enum class TextureChannel
		: uint16_t
	{
		//!\~english	Undefined channel.
		//!\~french		Canal indéfini.
		eUndefined = 0x0000,
		//!\~english	Diffuse map
		//!\~french		Map de diffuse
		eDiffuse = 0x0001,
		//!\~english	Diffuse map
		//!\~french		Map de diffuse
		eAlbedo = eDiffuse,
		//!\~english	Normal map
		//!\~french		Map de normales
		eNormal = 0x0002,
		//!\~english	Opacity map
		//!\~french		Map d'opacité
		eOpacity = 0x0004,
		//!\~english	Specular map
		//!\~french		Map de spéculaire
		eSpecular = 0x0008,
		//!\~english	Specular map
		//!\~french		Map de spéculaire
		eRoughness = eSpecular,
		//!\~english	Height map
		//!\~french		Map de hauteur
		eHeight = 0x0010,
		//!\~english	Gloss map
		//!\~french		Map de gloss
		eGloss = 0x0020,
		//!\~english	Specular map
		//!\~french		Map de spéculaire
		eMetallic = eGloss,
		//!\~english	Emissive map
		//!\~french		Map d'émissive
		eEmissive = 0x040,
		//!\~english	Reflection map
		//!\~french		Map de réflexion
		eReflection = 0x0080,
		//!\~english	Refraction map
		//!\~french		Map de réfraction
		eRefraction = 0x0100,
		//!\~english	Ambient occlusion map
		//!\~french		Map d'occlusion ambiante.
		eAmbientOcclusion = 0x0200,
		//!\~english	Light transmittance map
		//!\~french		Map de transmission de lumière.
		eTransmittance = 0x0400,
		//!\~english	Mask for all the texture channels
		//!\~french		Masque pour les canaux de texture
		eAll = 0x0FFF,
		//!\~english Not really a texture channel (it is out of TextureChannel::eAll), used to tell we want text overlay shader source
		//!\~french Pas vraiment un canal de texture (hors de TextureChannel::eAll), utilisé pour dire que nous voulons un shader d'incrustation texte
		eText = 0x1000,
	};
	CU_ImplementFlags( TextureChannel )
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		The components kept while loading an image.
	\~french
	\brief		Les composantes gardées lors du chargement d'une image.
	*/
	enum class ImageComponents
		: uint8_t
	{
		eR,
		eRG,
		eRGB,
		eA,
		eAll,
	};

	class TextureLayout;
	class TextureUnit;
	class TextureView;
	class Material;
	class Pass;
	class LegacyPass;
	class MetallicRoughnessPbrPass;
	class SpecularGlossinessPbrPass;
	class Sampler;
	class SubsurfaceScattering;

	CU_DeclareSmartPtr( TextureLayout );
	CU_DeclareSmartPtr( TextureUnit );
	CU_DeclareSmartPtr( TextureView );
	CU_DeclareSmartPtr( Material );
	CU_DeclareSmartPtr( Pass );
	CU_DeclareSmartPtr( LegacyPass );
	CU_DeclareSmartPtr( MetallicRoughnessPbrPass );
	CU_DeclareSmartPtr( SpecularGlossinessPbrPass );
	CU_DeclareSmartPtr( Sampler );
	CU_DeclareSmartPtr( SubsurfaceScattering );

	//! Material pointer array
	CU_DeclareVector( MaterialSPtr, MaterialPtr );
	//! TextureUnit array
	CU_DeclareVector( TextureUnit, TextureUnit );
	//! TextureUnit pointer array
	CU_DeclareVector( TextureUnitSPtr, TextureUnitPtr );
	//! Pass array
	CU_DeclareVector( Pass, Pass );
	//! Pass pointer array
	CU_DeclareVector( PassSPtr, PassPtr );
	//! uint32_t array
	CU_DeclareVector( uint32_t, UInt32 );
	//! Material pointer map, sorted by name
	CU_DeclareMap( castor::String, MaterialSPtr, MaterialPtrStr );
	//! Material pointer map
	CU_DeclareMap( uint32_t, MaterialSPtr, MaterialPtrUInt );
	//! TextureUnit reference array
	CU_DeclareVector( std::reference_wrapper< TextureUnit >, DepthMap );

	using OnPassChangedFunction = std::function< void( Pass const & ) >;
	using OnPassChanged = castor::Signal< OnPassChangedFunction >;
	using OnPassChangedConnection = OnPassChanged::connection;

	using OnMaterialChangedFunction = std::function< void( Material const & ) >;
	using OnMaterialChanged = castor::Signal< OnMaterialChangedFunction >;
	using OnMaterialChangedConnection = OnMaterialChanged::connection;

	C3D_API ashes::Format convert( castor::PixelFormat format );
	C3D_API castor::PixelFormat convert( ashes::Format format );
	//@}
}

#endif
