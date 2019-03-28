/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PREREQUISITES_MATERIAL_H___
#define ___C3D_PREREQUISITES_MATERIAL_H___

#include <CastorUtils/Design/Signal.hpp>

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
		//!\~english	Phong (non PBR).
		//!\~french		Phong (non PBR).
		ePhong,
		//!\~english	Metallic/Roughness PBR.
		//!\~french		PBR Metallic/Roughness.
		eMetallicRoughness,
		//!\~english	Specular/Glossiness PBR.
		//!\~french		PBR Specular/Glossiness.
		eSpecularGlossiness,
		CU_ScopedEnumBounds( ePhong )
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
		//!\~english	No flag.
		//\~french		Aucun indicateur.
		eNone = 0x00,
		//!\~english	Shader using alpha blending.
		//\~french		Shader utilisant le mélange alpha.
		eAlphaBlending = 0x01,
		//!\~english	Shader using alpha test.
		//\~french		Shader utilisant un test alpha.
		eAlphaTest = 0x02,
		//!\~english	Shader for Metallic/Roughness PBR material (if not set, will use phong material code).
		//\~french		Shader pour un matériau PBR Metallic/Roughness (si non défini, utilisera le code de matériau phong).
		eMetallicRoughness = 0x04,
		//!\~english	Shader for Specular/Glossiness PBR material (if not set, will use phong material code).
		//\~french		Shader pour un matériau PBR Specular/Glossiness (si non défini, utilisera le code de matériau phong).
		eSpecularGlossiness = 0x08,
		//!\~english	Shader for Subsurface Scattering.
		//\~french		Shader pour le subsurface scattering.
		eSubsurfaceScattering = 0x10,
		//!\~english	Shader using distance based transmittance when computing Subsurface Scattering.
		//\~french		Shader utilisant la transmission basée sur la distance, lors du calcul du Subsurface Scattering.
		eDistanceBasedTransmittance = 0x20,
		//!\~english	Shader using parallax occlusion mapping.
		//\~french		Shader utilisant le parallax occlusion mapping.
		eParallaxOcclusionMapping = 0x40,
		//!\~english	All flags.
		//\~french		Tous les indicateurs.
		eAll = 0x7F,
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

	struct TextureConfiguration;

	class TextureLayout;
	class TextureUnit;
	class TextureView;
	class Material;
	class Pass;
	class PhongPass;
	class MetallicRoughnessPbrPass;
	class SpecularGlossinessPbrPass;
	class Sampler;
	class SubsurfaceScattering;

	CU_DeclareSmartPtr( TextureLayout );
	CU_DeclareSmartPtr( TextureUnit );
	CU_DeclareSmartPtr( TextureView );
	CU_DeclareSmartPtr( Material );
	CU_DeclareSmartPtr( Pass );
	CU_DeclareSmartPtr( PhongPass );
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

	using OnTextureUnitChangedFunction = std::function< void( TextureUnit const & ) >;
	using OnTextureUnitChanged = castor::Signal< OnTextureUnitChangedFunction >;
	using OnTextureUnitChangedConnection = OnTextureUnitChanged::connection;

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
