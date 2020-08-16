/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MaterialPassModule_H___
#define ___C3D_MaterialPassModule_H___

#include "Castor3D/Material/MaterialModule.hpp"

#include <CastorUtils/Design/Signal.hpp>

namespace castor3d
{
	/**@name Material */
	//@{
	/**@name Pass */
	//@{

	/**
	*\~english
	*\brief
	*	Parallax occlusion modes enumeration.
	*\~french
	*\brief
	*	Enumération des modes de parallax occlusion.
	*/
	enum class ParallaxOcclusionMode
	{
		eNone,
		eOne,
		eRepeat,
		CU_ScopedEnumBounds( eNone )
	};
	C3D_API castor::String getName( ParallaxOcclusionMode value );
	/**
	*\~english
	*\brief
	*	Blending modes enumeration
	*\~french
	*\brief
	*	Enumération des modes de mélange
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
	C3D_API castor::String getName( BlendMode value );
	/**
	*\~english
	*\brief
	*	Pass specific flags.
	*\~french
	*\brief
	*	Indicateurs spécifiques à la passe.
	*/
	enum class PassFlag
		: uint16_t
	{
		//!\~english	No flag.
		//\~french		Aucun indicateur.
		eNone = 0x0000,
		//!\~english	Shader using alpha blending.
		//\~french		Shader utilisant le mélange alpha.
		eAlphaBlending = 0x0001,
		//!\~english	Shader using alpha test.
		//\~french		Shader utilisant un test alpha.
		eAlphaTest = 0x0002,
		//!\~english	Shader for Metallic/Roughness PBR material (if not set, will use phong material code).
		//\~french		Shader pour un matériau PBR Metallic/Roughness (si non défini, utilisera le code de matériau phong).
		eMetallicRoughness = 0x0004,
		//!\~english	Shader for Specular/Glossiness PBR material (if not set, will use phong material code).
		//\~french		Shader pour un matériau PBR Specular/Glossiness (si non défini, utilisera le code de matériau phong).
		eSpecularGlossiness = 0x0008,
		//!\~english	Shader for Subsurface Scattering.
		//\~french		Shader pour le subsurface scattering.
		eSubsurfaceScattering = 0x0010,
		//!\~english	Shader using distance based transmittance when computing Subsurface Scattering.
		//\~french		Shader utilisant la transmission basée sur la distance, lors du calcul du Subsurface Scattering.
		eDistanceBasedTransmittance = 0x0020,
		//!\~english	Shader using parallax occlusion mapping.
		//\~french		Shader utilisant le parallax occlusion mapping.
		eParallaxOcclusionMappingOne = 0x0040,
		//!\~english	Shader using parallax occlusion mapping.
		//\~french		Shader utilisant le parallax occlusion mapping.
		eParallaxOcclusionMappingRepeat = 0x0080,
		//!\~english	Reflection map.
		//!\~french		Map de réflexion.
		eReflection = 0x0100,
		//!\~english	Refraction map.
		//!\~french		Map de réfraction.
		eRefraction = 0x0200,
		//!\~english	All flags.
		//\~french		Tous les indicateurs.
		eAll = 0x03FF,
	};
	CU_ImplementFlags( PassFlag )
	C3D_API castor::String getName( PassFlag value );
	/**
	*\~english
	*\brief
	*	Base class of a material pass.
	*\~french
	*\brief
	*	Classe de base d'une passe d'un matériau.
	*/
	class Pass;
	/**
	*\~english
	*\brief
	*	Definition of a Phong pass.
	*\remarks
	*	A Phong pass is composed of : base colours (ambient, diffuse, specular, emissive), and shininess.
	*\~french
	*\brief
	*	Définition d'une passe Phong.
	*\remarks
	*	Une passe Phong est composée de : couleurs (ambiante, diffuse, spéculaire, émissive), et d'exposant.
	*/
	class PhongPass;
	/**
	*\~english
	*\brief
	*	Definition of a PBR Metallic/Roughness pass.
	*\remark
	*	A PBR M/R pass is composed of : colour, roughness, reflectance.
	*\~french
	*\brief
	*	Définition d'une passe PBR Metallic/Roughness.
	*\remarks
	*	Une passe PBR M/R est composée de : couleur, rugosité, réflectivité.
	*/
	class MetallicRoughnessPbrPass;
	/**
	*\~english
	*\brief
	*	Definition of a PBR Specular/Glossiness pass.
	*\remarks
	*	A PBR S/R pass is composed of : colour, glossiness, specular.
	*\~french
	*\brief
	*	Définition d'une passe PBR Specular/Glossiness.
	*\remarks
	*	Une passe PBR S/R est composée de : couleur, polissage, spéculaire.
	*/
	class SpecularGlossinessPbrPass;
	/**
	*\~english
	*\brief
	*	Extended pass information, related to subsurface scattering.
	*\~french
	*\brief
	*	Informations étendues de la passe, relatives au subsurface scattering.
	*/
	class SubsurfaceScattering;
	/**
	*\~english
	*\brief
	*	Helper class to retrieve a pass type from a MaterialType.
	*\~french
	*\brief
	*	Classe d'aide permettant de récupérer le type de passe depuis un MaterialType.
	*/
	template< MaterialType Type >
	struct PassTyper;
	/**
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		02/12/2016
	\~english
	\brief		Helper class to retrieve a pass type from a MaterialType.
	\remarks	Specialisation for MaterialType::ePhong.
	\~french
	\brief		Classe d'aide permettant de récupérer le type de passe depuis un MaterialType.
	\remarks	Spécialisation pour MaterialType::ePhong.
	*/
	template<>
	struct PassTyper< MaterialType::ePhong >
	{
		using Type = PhongPass;
	};
	/**
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		02/12/2016
	\~english
	\brief		Helper class to retrieve a pass type from a MaterialType.
	\remarks	Specialisation for MaterialType::eMetallicRoughness.
	\~french
	\brief		Classe d'aide permettant de récupérer le type de passe depuis un MaterialType.
	\remarks	Spécialisation pour MaterialType::eMetallicRoughness.
	*/
	template<>
	struct PassTyper< MaterialType::eMetallicRoughness >
	{
		using Type = MetallicRoughnessPbrPass;
	};
	/**
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		02/12/2016
	\~english
	\brief		Helper class to retrieve a pass type from a MaterialType.
	\remarks	Specialisation for MaterialType::eMetallicRoughness.
	\~french
	\brief		Classe d'aide permettant de récupérer le type de passe depuis un MaterialType.
	\remarks	Spécialisation pour MaterialType::eMetallicRoughness.
	*/
	template<>
	struct PassTyper< MaterialType::eSpecularGlossiness >
	{
		using Type = SpecularGlossinessPbrPass;
	};

	CU_DeclareSmartPtr( Pass );
	CU_DeclareSmartPtr( PhongPass );
	CU_DeclareSmartPtr( MetallicRoughnessPbrPass );
	CU_DeclareSmartPtr( SpecularGlossinessPbrPass );
	CU_DeclareSmartPtr( SubsurfaceScattering );

	//! Pass array
	CU_DeclareVector( Pass, Pass );
	//! Pass pointer array
	CU_DeclareVector( PassSPtr, PassPtr );

	using OnPassChangedFunction = std::function< void( Pass const & ) >;
	using OnPassChanged = castor::Signal< OnPassChangedFunction >;
	using OnPassChangedConnection = OnPassChanged::connection;

	//@}
	//@}
}

#endif
