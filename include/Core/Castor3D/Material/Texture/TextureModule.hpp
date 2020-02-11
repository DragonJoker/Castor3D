/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextureModule_H___
#define ___C3D_TextureModule_H___

#include "Castor3D/Material/MaterialModule.hpp"

#include <CastorUtils/Design/Signal.hpp>

namespace castor3d
{
	/**@name Material */
	//@{
	/**@name Texture */
	//@{

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
		//!\~english	Mask for all the texture channels except for opacity and colour.
		//!\~french		Masque pour les canaux de texture sauf l'opacité et la couleur.
		eAllButColourAndOpacity = 0x07FA,
		//!\~english	Mask for all the texture channels except for opacity.
		//!\~french		Masque pour les canaux de texture sauf l'opacité.
		eAllButOpacity = eAllButColourAndOpacity | eDiffuse,
		//!\~english	Mask for all the texture channels.
		//!\~french		Masque pour les canaux de texture.
		eAll = eAllButOpacity | eOpacity,
	};
	CU_ImplementFlags( TextureFlag )
	C3D_API castor::String getName( TextureFlag value
		, MaterialType material );
	/**
	*\~english
	*\brief
	*	Cube map faces.
	*\~french
	*\brief
	*	Les faces d'une cube map.
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

	/**
	*\~english
	*\brief
	*	Defines a sampler for a texture
	*\~french
	*\brief
	*	Définit un sampler pour une texture
	*/
	class Sampler;
	/**
	*\~english
	*\brief
	*	Specifies the usages of a texture, per image component.
	*\~french
	*\brief
	*	Définit les utilisations d'une texture, par composante d'image.
	*/
	struct TextureConfiguration;
	/**
	*\~english
	*\brief
	*	Texture base class
	*\~french
	*\brief
	*	Class de base d'une texture
	*/
	class TextureLayout;
	/**
	*\~english
	*\brief
	*	Texture unit representation.
	*\remarks
	*	A texture unit is a texture with few options, like channel, blend modes, transformations...
	*\~french
	*\brief
	*	Représentation d'une unité de texture.
	*\remarks
	*	Une unité de texture se compose d'une texture avec des options telles que son canal, modes de mélange, transformations...
	*/
	class TextureUnit;
	/**
	*\~english
	*\brief
	*	Texture image source.
	*\~french
	*\brief
	*	Classe de source d'image de texture.
	*/
	class TextureSource;
	/**
	*\~english
	*\brief
	*	Texture image.
	*\remarks
	*	Holds the GPU texture storage.
	*\~french
	*\brief
	*	Classe d'image de texture.
	*\remarks
	*	Contient le stockage de la texture au niveau GPU.
	*/
	class TextureView;

	CU_DeclareSmartPtr( Sampler );
	CU_DeclareSmartPtr( TextureLayout );
	CU_DeclareSmartPtr( TextureSource );
	CU_DeclareSmartPtr( TextureUnit );
	CU_DeclareSmartPtr( TextureView );

	//! TextureUnit array
	CU_DeclareVector( TextureUnit, TextureUnit );
	//! TextureUnit pointer array
	CU_DeclareVector( TextureUnitSPtr, TextureUnitPtr );
	//! TextureUnit reference array
	CU_DeclareVector( std::reference_wrapper< TextureUnit >, DepthMap );

	using OnTextureUnitChangedFunction = std::function< void( TextureUnit const & ) >;
	using OnTextureUnitChanged = castor::Signal< OnTextureUnitChangedFunction >;
	using OnTextureUnitChangedConnection = OnTextureUnitChanged::connection;

	//@}
	//@}
}

#endif
