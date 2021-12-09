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
		//!\~english	Shader using alpha test.
		//\~french		Shader utilisant un test alpha.
		eBlendAlphaTest = 0x0004,
		//!\~english	Shader with Image Based Lighting support.
		//\~french		Shader supportant l'Image Based Lighting.
		eImageBasedLighting = 0x0008,
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
		//!\~english	The pass receives lighting.
		//!\~french		La passe reçoit l'éclairage.
		eLighting = 0x0400,
		//!\~english	The pass sets an object as pickable.
		//!\~french		La passe rend l'objet sélectionnable.
		ePickable = 0x0800,
		//!\~english	The pass needs an edge to be drawn.
		//!\~french		La passe demande qu'une bordure soit dessinée.
		eDrawEdge = 0x1000,
		//!\~english	All flags.
		//\~french		Tous les indicateurs.
		eAll = 0x1FFF,
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
	*	Factory for material passes.
	*\~french
	*\brief
	*	Fabrique de passe de matériau.
	*/
	class PassFactory;
	/**
	*\~english
	*\brief
	*	Pass visitor.
	*\~french
	*\brief
	*	Visiteur de passe.
	*/
	class PassVisitorBase;
	/**
	*\~english
	*\brief
	*	Extended pass information, related to subsurface scattering.
	*\~french
	*\brief
	*	Informations étendues de la passe, relatives au subsurface scattering.
	*/
	class SubsurfaceScattering;

	CU_DeclareSmartPtr( Pass );
	CU_DeclareSmartPtr( SubsurfaceScattering );

	CU_DeclareCUSmartPtr( castor3d, PassFactory, C3D_API );

	//! Pass array
	CU_DeclareVector( Pass, Pass );
	//! Pass pointer array
	CU_DeclareVector( PassSPtr, PassPtr );

	using OnPassChangedFunction = std::function< void( Pass const & ) >;
	using OnPassChanged = castor::SignalT< OnPassChangedFunction >;
	using OnPassChangedConnection = OnPassChanged::connection;

	struct RenderPassRegisterInfo;
	struct PassRegisterInfo;

	using PassFactoryBase = castor::Factory< Pass, PassTypeID, PassSPtr, std::function< PassSPtr( Material & ) > >;

	CU_DeclareCUSmartPtr( castor3d, RenderPassRegisterInfo, C3D_API );
	//@}
	//@}
}

#endif
