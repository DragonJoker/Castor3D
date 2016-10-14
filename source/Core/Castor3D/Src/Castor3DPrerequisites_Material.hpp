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
		Always,
		//!\~english Takes texture colour if alpha is less than given value
		//!\~french Prend la couleur de la texture si l'alpha est inferieur a la valeur donnée
		Less,
		//!\~english Takes texture colour if alpha is less than or equal to given value
		//!\~french Prend la couleur de la texture si l'alpha est inferieur ou egal a la valeur donnée
		LEqual,
		//!\~english Takes texture colour if alpha is equal to given value
		//!\~french Prend la couleur de la texture si l'alpha est egal a la valeur donnée
		Equal,
		//!\~english Takes texture colour if alpha is different of given value
		//!\~french Prend la couleur de la texture si l'alpha est different de la valeur donnée
		NEqual,
		//!\~english Takes texture colour if alpha is grater than or equal to given value
		//!\~french Prend la couleur de la texture si l'alpha est superieur ou egal a la valeur donnée
		GEqual,
		//!\~english Takes texture colour if alpha is greater than given value
		//!\~french Prend la couleur de la texture si l'alpha est superieur a la valeur donnée
		Greater,
		//!\~english Never take texture colour
		//!\~french Ne prend jamais la couleur de la texture
		Never,
		CASTOR_ENUM_CLASS_BOUNDS( Always )
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
		U,
		V,
		W,
		CASTOR_ENUM_CLASS_BOUNDS( U )
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
		Repeat,
		MirroredRepeat,
		ClampToBorder,
		ClampToEdge,
		CASTOR_ENUM_CLASS_BOUNDS( Repeat )
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
		Min,
		Mag,
		Mip,
		Count
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
		None,
		RefToTexture,
		CASTOR_ENUM_CLASS_BOUNDS( None )
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
		Undefined,
		Nearest,
		Linear,
		CASTOR_ENUM_CLASS_BOUNDS( Undefined )
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
		Index0,
		Index1,
		Index2,
		CASTOR_ENUM_CLASS_BOUNDS( Index0 )
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
		Texture,
		Texture0,
		Texture1,
		Texture2,
		Texture3,
		Constant,
		Diffuse,
		Previous,
		CASTOR_ENUM_CLASS_BOUNDS( Texture )
	};
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		RGB blending functions enumeration
	\~french
	\brief		Enumération de fonctions de mélange RGB
	*/
	enum class ColourBlendFunc
		: uint8_t
	{
		//!\~english No blend
		//!\~french Pas de mélange
		NoBlend,
		//!\~english Arg0
		//!\~french Arg0
		FirstArg,
		//!\~english Arg0 + Arg1
		//!\~french Arg0 + Arg1
		Add,
		//!\~english Arg0 + Arg1 - 0.5
		//!\~french Arg0 + Arg1 - 0.5
		AddSigned,
		//!\~english Arg0 x Arg1
		//!\~french Arg0 x Arg1
		Modulate,
		//!\~english Arg0 × Arg2 + Arg1 × (1 - Arg2)
		//!\~french Arg0 × Arg2 + Arg1 × (1 - Arg2)
		Interpolate,
		//!\~english Arg0 - Arg1
		//!\~french Arg0 - Arg1
		Subtract,
		//!\~english 4 × (((Arg0.r - 0.5) × (Arg1.r - 0.5)) + ((Arg0.g - 0.5) × (Arg1.g - 0.5)) + ((Arg0.b - 0.5) × (Arg1.b - 0.5)))
		//!\~french 4 × (((Arg0.r - 0.5) × (Arg1.r - 0.5)) + ((Arg0.g - 0.5) × (Arg1.g - 0.5)) + ((Arg0.b - 0.5) × (Arg1.b - 0.5)))
		Dot3RGB,
		//!\~english 4 × (((Arg0.r - 0.5) × (Arg1.r - 0.5)) + ((Arg0.g - 0.5) × (Arg1.g - 0.5)) + ((Arg0.b - 0.5) × (Arg1.b - 0.5)) + ((Arg0.a - 0.5) × (Arg1.a - 0.5)))
		//!\~french 4 × (((Arg0.r - 0.5) × (Arg1.r - 0.5)) + ((Arg0.g - 0.5) × (Arg1.g - 0.5)) + ((Arg0.b - 0.5) × (Arg1.b - 0.5)) + ((Arg0.a - 0.5) × (Arg1.a - 0.5)))
		Dot3RGBA,
		CASTOR_ENUM_CLASS_BOUNDS( NoBlend )
	};
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Alpha blending functions enumeration
	\~french
	\brief		Enumération de fonctions de mélange alpha
	*/
	enum class AlphaBlendFunc
		: uint8_t
	{
		//!\~english No blend
		//!\~french Pas de mélange
		NoBlend,
		//!\~english Arg0
		//!\~french Arg0
		FirstArg,
		//!\~english Arg0 + Arg1
		//!\~french Arg0 + Arg1
		Add,
		//!\~english Arg0 + Arg1 - 0.5
		//!\~french Arg0 + Arg1 - 0.5
		AddSigned,
		//!\~english Arg0 x Arg1
		//!\~french Arg0 x Arg1
		Modulate,
		//!\~english Arg0 × Arg2 + Arg1 × (1 - Arg2)
		//!\~french Arg0 × Arg2 + Arg1 × (1 - Arg2)
		Interpolate,
		//!\~english Arg0 - Arg1
		//!\~french Arg0 - Arg1
		Subtract,
		CASTOR_ENUM_CLASS_BOUNDS( NoBlend )
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
		Add,
		//!\~english Src2 - Src1.
		//!\~french Src2 - Src1.
		Subtract,
		//!\~english Src1 - Src2.
		//!\~french Src1 - Src2.
		RevSubtract,
		//!\~english min( Src1, Src2 ).
		//!\~french min( Src1, Src2 ).
		Min,
		//!\~english max( Src1, Src2 ).
		//!\~french max( Src1, Src2 ).
		Max,
		Count
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
		Zero,
		One,
		SrcColour,
		InvSrcColour,
		DstColour,
		InvDstColour,
		SrcAlpha,
		InvSrcAlpha,
		DstAlpha,
		InvDstAlpha,
		Constant,
		InvConstant,
		SrcAlphaSaturate,
		Src1Colour,
		InvSrc1Colour,
		Src1Alpha,
		InvSrc1Alpha,
		CASTOR_ENUM_CLASS_BOUNDS( Zero )
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
		NoBlend,
		//!\~english Order independent, add the components.
		//!\~french Mélange indépendant de l'ordre, additionnant les composantes.
		Additive,
		//!\~english Order independent, multiply the components.
		//!\~french Indépendant de l'ordre, multipliant les composantes.
		Multiplicative,
		//!\~english Order dependent, interpolate the components.
		//!\~french Indépendant de l'ordre, interpolant les composantes.
		Interpolative,
		//!\~english Order independent, using A-buffer, not implemented yet.
		//!\~french Indépendant de l'ordre, utilisant les A-Buffer (non implémenté).
		ABuffer,
		//!\~english Order independent, using depth peeling, not implemented yet.
		//!\~french Indépendant de l'ordre, utilisant le pelage en profondeur (non implémenté).
		DepthPeeling,
		CASTOR_ENUM_CLASS_BOUNDS( NoBlend )
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
		Buffer,
		//!\~english 1 dimension textures => one coordinate => U mapping
		//!\~french Textures à 1 dimension => une coordonnée => U mapping
		OneDimension,
		//!\~english 1 dimension textures array => one coordinate => U mapping
		//!\~french Tableau de textures à 1 dimension => une coordonnée => U mapping
		OneDimensionArray,
		//!\~english 2 dimensions textures => two coordinates => UV mapping
		//!\~french Textures à 2 dimensions => deux coordonnées => UV mapping
		TwoDimensions,
		//!\~english 2 dimensions textures array => two coordinates => UV mapping
		//!\~french Tableau de textures à 2 dimensions => deux coordonnées => UV mapping
		TwoDimensionsArray,
		//!\~english 2 dimensions textures with multisample support => two coordinates => UV mapping
		//!\~french Textures à 2 dimensions avec support du multisampling => deux coordonnées => UV mapping
		TwoDimensionsMS,
		//!\~english 2 dimensions textures array with multisample support => two coordinates => UV mapping
		//!\~french Tableau de textures à 2 dimensions => deux coordonnées => UV mapping
		TwoDimensionsMSArray,
		//!\~english 3 dimensions textures => three coordinates => UVW mapping
		//!\~french Textures à 3 dimensions => trois coordonnées => UVW mapping
		ThreeDimensions,
		//!\~english Cube texture.
		//!\~french Textures cube.
		Cube,
		//!\~english Cube textures array.
		//!\~french Tableaux de textures cube.
		CubeArray,
		CASTOR_ENUM_CLASS_BOUNDS( Buffer )
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
		Buffer,
		//!\~english 1 dimension textures.
		//!\~french Textures à 1 dimension.
		OneDimension,
		//!\~english 2 dimensions textures.
		//!\~french Textures à 2 dimensions.
		TwoDimensions,
		//!\~english 2 dimensions textures with multisample support.
		//!\~french Textures à 2 dimensions avec support du multisampling.
		TwoDimensionsMS,
		//!\~english 3 dimensions textures.
		//!\~french Textures à 3 dimensions.
		ThreeDimensions,
		//!\~english Face on positive X.
		//!\~french Face des X positifs.
		CubeMapPositiveX,
		//!\~english Face on negative X.
		//!\~french Face des X négatifs.
		CubeMapNegativeX,
		//!\~english Face on positive Y.
		//!\~french Face des Y positifs.
		CubeMapPositiveY,
		//!\~english Face on negative Y.
		//!\~french Face des Y négatifs.
		CubeMapNegativeY,
		//!\~english Face on positive Z.
		//!\~french Face des Z positifs.
		CubeMapPositiveZ,
		//!\~english Face on negative Z.
		//!\~french Face des Z négatifs.
		CubeMapNegativeZ,
		CASTOR_ENUM_CLASS_BOUNDS( Buffer )
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
		PositiveX,
		//!\~english Face on negative X
		//!\~french Face des X négatifs.
		NegativeX,
		//!\~english Face on positive Y
		//!\~french Face des Y positifs.
		PositiveY,
		//!\~english Face on negative Y
		//!\~french Face des Y négatifs.
		NegativeY,
		//!\~english Face on positive Z
		//!\~french Face des Z positifs.
		PositiveZ,
		//!\~english Face on negative Z
		//!\~french Face des Z négatifs.
		NegativeZ,
		CASTOR_ENUM_CLASS_BOUNDS( PositiveX )
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
		Undefined = 0x0000,
		//!\~english Colour map
		//!\~french Map de couleurs
		Colour = 0x0001,
		//!\~english Diffuse map
		//!\~french Map de diffuse
		Diffuse = 0x0002,
		//!\~english Normal map
		//!\~french Map de normales
		Normal = 0x0004,
		//!\~english Opacity map
		//!\~french Map d'opacité
		Opacity = 0x0008,
		//!\~english Specular map
		//!\~french Map de spéculaire
		Specular = 0x0010,
		//!\~english Height map
		//!\~french Map de hauteur
		Height = 0x0020,
		//!\~english Ambient map
		//!\~french Map d'ambiante
		Ambient = 0x0040,
		//!\~english Gloss map
		//!\~french Map de gloss
		Gloss = 0x0080,
		//!\~english Emissive map
		//!\~french Map d'émissive
		Emissive = 0x0100,
		//!\~english Mask for all the texture channels
		//!\~french Masque pour les canaux de texture
		All = 0x0FFF,
		//!\~english Not really a texture channel (it is out of TextureChannel::All), used to tell we want text overlay shader source
		//!\~french Pas vraiment un canal de texture (hors de TextureChannel::All), utilisé pour dire que nous voulons un shader d'incrustation texte
		Text = 0x1000,
	};

	class TextureImage;
	class TextureLayout;
	class TextureStorage;
	class TextureUnit;
	class Material;
	class Pass;
	class Sampler;

	DECLARE_SMART_PTR( TextureImage );
	DECLARE_SMART_PTR( TextureLayout );
	DECLARE_SMART_PTR( TextureStorage );
	DECLARE_SMART_PTR( TextureUnit );
	DECLARE_SMART_PTR( Material );
	DECLARE_SMART_PTR( Pass );
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
