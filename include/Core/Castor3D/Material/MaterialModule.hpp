/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MaterialModule_H___
#define ___C3D_MaterialModule_H___

#include "Castor3D/Castor3DModule.hpp"

#include <CastorUtils/Design/Signal.hpp>

namespace castor3d
{
	/**@name Material */
	//@{

	/**
	*\~english
	*\brief
	*	Supported material types enumeration.
	*\~french
	*\brief
	*	Enumération des types de matétiaux supportés.
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
	C3D_API castor::String getName( MaterialType value );
	/**
	*\~english
	*\brief
	*	Texture channels flags.
	*\~french
	*\brief
	*	Indicateurs des canaux de texture.
	*/
	enum class TextureFlag
		: uint16_t
	{
		//!\~english	No texture.
		//!\~french		Pas de texture.
		eNone = 0x0000,
		//!\~english	Diffuse map.
		//!\~french		Map de diffuse.
		eDiffuse = 0x0001,
		//!\~english	Albedo map.
		//!\~french		Map d'albedo.
		eAlbedo = eDiffuse,
		//!\~english	Normal map.
		//!\~french		Map de normales.
		eNormal = 0x0002,
		//!\~english	Opacity map.
		//!\~french		Map d'opacité.
		eOpacity = 0x0004,
		//!\~english	Specular map.
		//!\~french		Map de spéculaire.
		eSpecular = 0x0008,
		//!\~english	Metalness map.
		//!\~french		Map de metalness.
		eMetalness = eSpecular,
		//!\~english	Height map.
		//!\~french		Map de hauteur.
		eHeight = 0x0010,
		//!\~english	Glossiness map.
		//!\~french		Map de glossiness.
		eGlossiness = 0x0020,
		//!\~english	Shininess map.
		//!\~french		Map de shininess.
		eShininess = eGlossiness,
		//!\~english	Roughness map.
		//!\~french		Map de roughness.
		eRoughness = eGlossiness,
		//!\~english	Emissive map.
		//!\~french		Map d'émissive.
		eEmissive = 0x040,
		//!\~english	Reflection map.
		//!\~french		Map de réflexion.
		eReflection = 0x0080,
		//!\~english	Refraction map.
		//!\~french		Map de réfraction.
		eRefraction = 0x0100,
		//!\~english	Occlusion map.
		//!\~french		Map d'occlusion.
		eOcclusion = 0x0200,
		//!\~english	Light transmittance map.
		//!\~french		Map de transmission de lumière.
		eTransmittance = 0x0400,
		//!\~english	Mask for all the texture channels.
		//!\~french		Masque pour les canaux de texture.
		eAll = 0x7FF,
		//!\~english	Mask for all the texture channels except for opacity.
		//!\~french		Masque pour les canaux de texture sauf l'opacité.
		eAllButOpacity = eAll & ~( eOpacity ),
		//!\~english	Mask for all the texture channels except for opacity and colour.
		//!\~french		Masque pour les canaux de texture sauf l'opacité et la couleur.
		eAllButColourAndOpacity = eAllButOpacity & ~( eDiffuse ),
		//!\~english	Mask for all the texture channels except for opacity and normal.
		//!\~french		Masque pour les canaux de texture sauf l'opacité et la normale.
		eAllButNormalAndOpacity = eAllButOpacity & ~( eNormal ),
	};
	CU_ImplementFlags( TextureFlag )
	C3D_API castor::String getName( TextureFlag value
		, MaterialType material );
	/**
	*\~english
	*\brief
	*	Definition of a material
	*\remarks
	*	A material is composed of one or more passes.
	*\~french
	*\brief
	*	Définition d'un matériau
	*\remarks
	*	Un matériau est composé d'une ou plusieurs passes
	*/
	class Material;

	CU_DeclareSmartPtr( Material );

	//! Material pointer array
	CU_DeclareVector( MaterialSPtr, MaterialPtr );
	//! Material pointer map, sorted by name
	CU_DeclareMap( castor::String, MaterialSPtr, MaterialPtrStr );
	//! Material pointer map
	CU_DeclareMap( uint32_t, MaterialSPtr, MaterialPtrUInt );

	using OnMaterialChangedFunction = std::function< void( Material const & ) >;
	using OnMaterialChanged = castor::Signal< OnMaterialChangedFunction >;
	using OnMaterialChangedConnection = OnMaterialChanged::connection;

	C3D_API VkFormat convert( castor::PixelFormat format );
	C3D_API castor::PixelFormat convert( VkFormat format );

	//@}
}

#endif
