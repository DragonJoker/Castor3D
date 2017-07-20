/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___C3D_PREREQUISITES_MATERIAL_H___
#define ___C3D_PREREQUISITES_MATERIAL_H___

namespace Castor3D
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
		CASTOR_SCOPED_ENUM_BOUNDS( eLegacy )
	};
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Alpha functions enumeration
	\~french
	\brief		Enumération des fonctions alpha
	*/
	enum class ComparisonFunc
		: uint8_t
	{
		//!\~english Always take texture colour
		//!\~french Prend toujours la couleur de la texture
		eAlways = 0x0,
		//!\~english Takes texture colour if alpha is less than given value
		//!\~french Prend la couleur de la texture si l'alpha est inferieur a la valeur donnée
		eLess = 0x1,
		//!\~english Takes texture colour if alpha is less than or equal to given value
		//!\~french Prend la couleur de la texture si l'alpha est inferieur ou egal a la valeur donnée
		eLEqual = 0x2,
		//!\~english Takes texture colour if alpha is equal to given value
		//!\~french Prend la couleur de la texture si l'alpha est egal a la valeur donnée
		eEqual = 0x3,
		//!\~english Takes texture colour if alpha is different of given value
		//!\~french Prend la couleur de la texture si l'alpha est different de la valeur donnée
		eNEqual = 0x4,
		//!\~english Takes texture colour if alpha is grater than or equal to given value
		//!\~french Prend la couleur de la texture si l'alpha est superieur ou egal a la valeur donnée
		eGEqual = 0x5,
		//!\~english Takes texture colour if alpha is greater than given value
		//!\~french Prend la couleur de la texture si l'alpha est superieur a la valeur donnée
		eGreater = 0x6,
		//!\~english Never take texture colour
		//!\~french Ne prend jamais la couleur de la texture
		eNever = 0x7,
		CASTOR_SCOPED_ENUM_BOUNDS( eAlways )
	};
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Texture UVW enumeration
	\~french
	\brief		Enumération des UVW
	*/
	enum class TextureUVW
		: uint8_t
	{
		eU,
		eV,
		eW,
		CASTOR_SCOPED_ENUM_BOUNDS( eU )
	};
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Texture wrap modes enumeration
	\~french
	\brief		Enumération des modes de wrapping de texture
	*/
	enum class WrapMode
		: uint8_t
	{
		eRepeat,
		eMirroredRepeat,
		eClampToBorder,
		eClampToEdge,
		CASTOR_SCOPED_ENUM_BOUNDS( eRepeat )
	};
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Texture interpolation filters enumeration
	\~french
	\brief		Enumération des filtres d'interpolation
	*/
	enum class InterpolationFilter
		: uint8_t
	{
		eMin,
		eMag,
		eMip,
		eCount
	};
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Texture comparison modes enumeration.
	\~french
	\brief		Enumération des modes de comparaison de texture.
	*/
	enum class ComparisonMode
		: uint8_t
	{
		eNone,
		eRefToTexture,
		CASTOR_SCOPED_ENUM_BOUNDS( eNone )
	};
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Texture filter interpolation modes enumeration
	\~french
	\brief		Enumération des modes d'interpolation
	*/
	enum class InterpolationMode
		: uint8_t
	{
		eUndefined,
		eNearest,
		eLinear,
		CASTOR_SCOPED_ENUM_BOUNDS( eUndefined )
	};
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Texture blending source indices enumeration
	\~french
	\brief		Enumération des indices de source de blending
	*/
	enum class BlendSrcIndex
		: uint8_t
	{
		eIndex0,
		eIndex1,
		eIndex2,
		CASTOR_SCOPED_ENUM_BOUNDS( eIndex0 )
	};
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Texture blending sources enumeration
	\~french
	\brief		Enumération des sources de blending
	*/
	enum class BlendSource
		: uint8_t
	{
		eTexture,
		eTexture0,
		eTexture1,
		eTexture2,
		eTexture3,
		eConstant,
		eDiffuse,
		ePrevious,
		CASTOR_SCOPED_ENUM_BOUNDS( eTexture )
	};
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		RGB blending functions enumeration
	\~french
	\brief		Enumération de fonctions de mélange RGB
	*/
	enum class TextureBlendFunc
		: uint8_t
	{
		//!\~english No blend
		//!\~french Pas de mélange
		eNoBlend,
		//!\~english Arg0
		//!\~french Arg0
		eFirstArg,
		//!\~english Arg0 + Arg1
		//!\~french Arg0 + Arg1
		eAdd,
		//!\~english Arg0 + Arg1 - 0.5
		//!\~french Arg0 + Arg1 - 0.5
		eAddSigned,
		//!\~english Arg0 x Arg1
		//!\~french Arg0 x Arg1
		eModulate,
		//!\~english Arg0 × Arg2 + Arg1 × (1 - Arg2)
		//!\~french Arg0 × Arg2 + Arg1 × (1 - Arg2)
		eInterpolate,
		//!\~english Arg0 - Arg1
		//!\~french Arg0 - Arg1
		eSubtract,
		CASTOR_SCOPED_ENUM_BOUNDS( eNoBlend )
	};
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Blending operations enumeration
	\~french
	\brief		Enumération de opérations de mélange
	*/
	enum class BlendOperation
		: uint8_t
	{
		//!\~english Src2 + Src1.
		//!\~french Src2 + Src1.
		eAdd,
		//!\~english Src2 - Src1.
		//!\~french Src2 - Src1.
		eSubtract,
		//!\~english Src1 - Src2.
		//!\~french Src1 - Src2.
		eRevSubtract,
		//!\~english min( Src1, Src2 ).
		//!\~french min( Src1, Src2 ).
		eMin,
		//!\~english max( Src1, Src2 ).
		//!\~french max( Src1, Src2 ).
		eMax,
		eCount
	};
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Blend operands enumeration
	\~french
	\brief		Enumération des opérandes de mélange
	*/
	enum class BlendOperand
		: uint8_t
	{
		eZero,
		eOne,
		eSrcColour,
		eInvSrcColour,
		eDstColour,
		eInvDstColour,
		eSrcAlpha,
		eInvSrcAlpha,
		eDstAlpha,
		eInvDstAlpha,
		eConstant,
		eInvConstant,
		eSrcAlphaSaturate,
		eSrc1Colour,
		eInvSrc1Colour,
		eSrc1Alpha,
		eInvSrc1Alpha,
		CASTOR_SCOPED_ENUM_BOUNDS( eZero )
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
		CASTOR_SCOPED_ENUM_BOUNDS( eNoBlend )
	};
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Texture dimensions enumeration
	\~french
	\brief		Enumération des dimensions de texture
	*/
	enum class TextureType
		: uint8_t
	{
		//!\~english 1 dimension texture buffer, used to store data.
		//!\~french tampons de texture à 1 dimension, utilisé pour stocker des données.
		eBuffer,
		//!\~english 1 dimension textures => one coordinate => U mapping
		//!\~french Textures à 1 dimension => une coordonnée => U mapping
		eOneDimension,
		//!\~english 1 dimension textures array => one coordinate => U mapping
		//!\~french Tableau de textures à 1 dimension => une coordonnée => U mapping
		eOneDimensionArray,
		//!\~english 2 dimensions textures => two coordinates => UV mapping
		//!\~french Textures à 2 dimensions => deux coordonnées => UV mapping
		eTwoDimensions,
		//!\~english 2 dimensions textures array => two coordinates => UV mapping
		//!\~french Tableau de textures à 2 dimensions => deux coordonnées => UV mapping
		eTwoDimensionsArray,
		//!\~english 2 dimensions textures with multisample support => two coordinates => UV mapping
		//!\~french Textures à 2 dimensions avec support du multisampling => deux coordonnées => UV mapping
		eTwoDimensionsMS,
		//!\~english 2 dimensions textures array with multisample support => two coordinates => UV mapping
		//!\~french Tableau de textures à 2 dimensions => deux coordonnées => UV mapping
		eTwoDimensionsMSArray,
		//!\~english 3 dimensions textures => three coordinates => UVW mapping
		//!\~french Textures à 3 dimensions => trois coordonnées => UVW mapping
		eThreeDimensions,
		//!\~english Cube texture.
		//!\~french Textures cube.
		eCube,
		//!\~english Cube textures array.
		//!\~french Tableaux de textures cube.
		eCubeArray,
		CASTOR_SCOPED_ENUM_BOUNDS( eBuffer )
	};
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Texture dimensions enumeration
	\~french
	\brief		Enumération des dimensions de texture
	*/
	enum class TextureStorageType
		: uint8_t
	{
		//!\~english 1 dimension texture buffer, used to store data.
		//!\~french tampons de texture à 1 dimension, utilisé pour stocker des données.
		eBuffer,
		//!\~english 1 dimension textures.
		//!\~french Textures à 1 dimension.
		eOneDimension,
		//!\~english 1 dimension texture arrays.
		//!\~french Tableaux de textures à 1 dimension.
		eOneDimensionArray,
		//!\~english 2 dimensions textures.
		//!\~french Textures à 2 dimensions.
		eTwoDimensions,
		//!\~english 2 dimensions texture arrays.
		//!\~french Tableaux de textures à 2 dimensions.
		eTwoDimensionsArray,
		//!\~english 2 dimensions textures with multisample support.
		//!\~french Textures à 2 dimensions avec support du multisampling.
		eTwoDimensionsMS,
		//!\~english 3 dimensions textures.
		//!\~french Textures à 3 dimensions.
		eThreeDimensions,
		//!\~english Cube map.
		//!\~french Cube map.
		eCubeMap,
		//!\~english Cube map array.
		//!\~french Tableau de cube maps.
		eCubeMapArray,
		CASTOR_SCOPED_ENUM_BOUNDS( eBuffer )
	};
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
		CASTOR_SCOPED_ENUM_BOUNDS( ePositiveX )
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
		//!\~english Undefined channel.
		//!\~french Canal indéfini.
		eUndefined = 0x0000,
		//!\~english Diffuse map
		//!\~french Map de diffuse
		eDiffuse = 0x0001,
		//!\~english Diffuse map
		//!\~french Map de diffuse
		eAlbedo = eDiffuse,
		//!\~english Normal map
		//!\~french Map de normales
		eNormal = 0x0002,
		//!\~english Opacity map
		//!\~french Map d'opacité
		eOpacity = 0x0004,
		//!\~english Specular map
		//!\~french Map de spéculaire
		eSpecular = 0x0008,
		//!\~english Specular map
		//!\~french Map de spéculaire
		eRoughness = eSpecular,
		//!\~english Height map
		//!\~french Map de hauteur
		eHeight = 0x0010,
		//!\~english Gloss map
		//!\~french Map de gloss
		eGloss = 0x0020,
		//!\~english Specular map
		//!\~french Map de spéculaire
		eMetallic = eGloss,
		//!\~english Emissive map
		//!\~french Map d'émissive
		eEmissive = 0x040,
		//!\~english Reflection map
		//!\~french Map de réflexion
		eReflection = 0x0080,
		//!\~english Refraction map
		//!\~french Map de réfraction
		eRefraction = 0x0100,
		//!\~english Ambient occlusion map
		//!\~french Map d'occlusion ambiante.
		eAmbientOcclusion = 0x0200,
		//!\~english Mask for all the texture channels
		//!\~french Masque pour les canaux de texture
		eAll = 0x0FFF,
		//!\~english Not really a texture channel (it is out of TextureChannel::eAll), used to tell we want text overlay shader source
		//!\~french Pas vraiment un canal de texture (hors de TextureChannel::eAll), utilisé pour dire que nous voulons un shader d'incrustation texte
		eText = 0x1000,
	};
	IMPLEMENT_FLAGS( TextureChannel )

	class TextureImage;
	class TextureLayout;
	class TextureStorage;
	class TextureUnit;
	class Material;
	class Pass;
	class LegacyPass;
	class MetallicRoughnessPbrPass;
	class SpecularGlossinessPbrPass;
	class Sampler;

	DECLARE_SMART_PTR( TextureImage );
	DECLARE_SMART_PTR( TextureLayout );
	DECLARE_SMART_PTR( TextureStorage );
	DECLARE_SMART_PTR( TextureUnit );
	DECLARE_SMART_PTR( Material );
	DECLARE_SMART_PTR( Pass );
	DECLARE_SMART_PTR( LegacyPass );
	DECLARE_SMART_PTR( MetallicRoughnessPbrPass );
	DECLARE_SMART_PTR( SpecularGlossinessPbrPass );
	DECLARE_SMART_PTR( Sampler );

	//! Material pointer array
	DECLARE_VECTOR( MaterialSPtr, MaterialPtr );
	//! TextureUnit array
	DECLARE_VECTOR( TextureUnit, TextureUnit );
	//! TextureUnit pointer array
	DECLARE_VECTOR( TextureUnitSPtr, TextureUnitPtr );
	//! Pass array
	DECLARE_VECTOR( Pass, Pass );
	//! Pass pointer array
	DECLARE_VECTOR( PassSPtr, PassPtr );
	//! uint32_t array
	DECLARE_VECTOR( uint32_t, UInt );
	//! Material pointer map, sorted by name
	DECLARE_MAP( Castor::String, MaterialSPtr, MaterialPtrStr );
	//! Material pointer map
	DECLARE_MAP( uint32_t, MaterialSPtr, MaterialPtrUInt );
	//! TextureUnit reference array
	DECLARE_VECTOR( std::reference_wrapper< TextureUnit >, DepthMap );

	//@}
}

#endif
