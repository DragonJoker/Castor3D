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
	*	Lists the possible spaces for a texture.
	*\~french
	*\brief
	*	Liste les espaces possibles pour une texture.
	*/
	enum class TextureSpace
		: uint16_t
	{
		//!\~english Modifier: Normalized value (Colour in [-1, 1] range, depth in [0, 1] range).
		//!\~french Modificateur: Valeur normalisée (Couleur dans l'intervalle [-1, 1], profondeur dans l'intervalle [0, 1]).
		eNormalised = 0x0001 << 0,
		//!\~english Modifier: Y inverted space.
		//!\~french Modificateur: Espace Y inversé.
		eYInverted = 0x0001 << 1,
		//!\~english Colour texture in R.
		//!\~french Texture couleur dans R.
		eColour = 0x0001 << 2,
		//!\~english Depth in [near, far] range.
		//!\~french Profondeur dans l'intervalle [near, far].
		eDepth = 0x0001 << 3,
		//!\~english Tangent space data.
		//!\~french Données en espace tangent.
		eTangentSpace = 0x0001 << 4,
		//!\~english Object space data.
		//!\~french Données en espace objet.
		eObjectSpace = 0x0001 << 5,
		//!\~english World space data.
		//!\~french Données en espace monde.
		eWorldSpace = 0x0001 << 6,
		//!\~english View space data.
		//!\~french Données en espace vue.
		eViewSpace = 0x0001 << 7,
		//!\~english Clip space data.
		//!\~french Données en espace de clipping.
		eClipSpace = 0x0001 << 8,
		//!\~english Stencil data.
		//!\~french Données de stencil.
		eStencil = 0x0001 << 9,
	};
	CU_ImplementFlags( TextureSpace )
	C3D_API castor::String getName( TextureSpace value );
	C3D_API castor::String getName( TextureSpaces value );
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
	C3D_API castor::String getName( CubeMapFace value );
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

	std::ostream & operator<<( std::ostream & stream, TextureLayout const & layout );

	//@}
	//@}
}

#endif
