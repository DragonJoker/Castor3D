/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
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
	typedef enum eALPHA_FUNC
		: uint8_t
	{
		//!\~english Always take texture colour
		//!\~french Prend toujours la couleur de la texture
		eALPHA_FUNC_ALWAYS,
		//!\~english Takes texture colour if alpha is less than given value
		//!\~french Prend la couleur de la texture si l'alpha est inferieur a la valeur donnée
		eALPHA_FUNC_LESS,
		//!\~english Takes texture colour if alpha is less than or equal to given value
		//!\~french Prend la couleur de la texture si l'alpha est inferieur ou egal a la valeur donnée
		eALPHA_FUNC_LESS_OR_EQUAL,
		//!\~english Takes texture colour if alpha is equal to given value
		//!\~french Prend la couleur de la texture si l'alpha est egal a la valeur donnée
		eALPHA_FUNC_EQUAL,
		//!\~english Takes texture colour if alpha is different of given value
		//!\~french Prend la couleur de la texture si l'alpha est different de la valeur donnée
		eALPHA_FUNC_NOT_EQUAL,
		//!\~english Takes texture colour if alpha is grater than or equal to given value
		//!\~french Prend la couleur de la texture si l'alpha est superieur ou egal a la valeur donnée
		eALPHA_FUNC_GREATER_OR_EQUAL,
		//!\~english Takes texture colour if alpha is greater than given value
		//!\~french Prend la couleur de la texture si l'alpha est superieur a la valeur donnée
		eALPHA_FUNC_GREATER,
		//!\~english Never take texture colour
		//!\~french Ne prend jamais la couleur de la texture
		eALPHA_FUNC_NEVER,
		CASTOR_ENUM_BOUNDS( eALPHA_FUNC, eALPHA_FUNC_ALWAYS )
	}	eALPHA_FUNC;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Texture UVW enumeration
	\~french
	\brief		Enumération des UVW
	*/
	typedef enum eTEXTURE_UVW
		: uint8_t
	{
		eTEXTURE_UVW_U,
		eTEXTURE_UVW_V,
		eTEXTURE_UVW_W,
		CASTOR_ENUM_BOUNDS( eTEXTURE_UVW, eTEXTURE_UVW_U )
	}	eTEXTURE_UVW;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Texture wrap modes enumeration
	\~french
	\brief		Enumération des modes de wrapping de texture
	*/
	typedef enum eWRAP_MODE
		: uint8_t
	{
		eWRAP_MODE_REPEAT,
		eWRAP_MODE_MIRRORED_REPEAT,
		eWRAP_MODE_CLAMP_TO_BORDER,
		eWRAP_MODE_CLAMP_TO_EDGE,
		CASTOR_ENUM_BOUNDS( eWRAP_MODE, eWRAP_MODE_REPEAT )
	}	eWRAP_MODE;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Texture interpolation filters enumeration
	\~french
	\brief		Enumération des filtres d'interpolation
	*/
	typedef enum eINTERPOLATION_FILTER
		: uint8_t
	{
		eINTERPOLATION_FILTER_MIN,
		eINTERPOLATION_FILTER_MAG,
		eINTERPOLATION_FILTER_MIP,
		eINTERPOLATION_FILTER_COUNT
	}	eINTERPOLATION_FILTER;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Texture filter interpolation modes enumeration
	\~french
	\brief		Enumération des modes d'interpolation
	*/
	typedef enum eINTERPOLATION_MODE
		: uint8_t
	{
		eINTERPOLATION_MODE_UNDEFINED,
		eINTERPOLATION_MODE_NEAREST,
		eINTERPOLATION_MODE_LINEAR,
		CASTOR_ENUM_BOUNDS( eINTERPOLATION_MODE, eINTERPOLATION_MODE_UNDEFINED )
	}	eINTERPOLATION_MODE;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Texture blending source indices enumeration
	\~french
	\brief		Enumération des indices de source de blending
	*/
	typedef enum eBLEND_SRC_INDEX
		: uint8_t
	{
		eBLEND_SRC_INDEX_0,
		eBLEND_SRC_INDEX_1,
		eBLEND_SRC_INDEX_2,
		CASTOR_ENUM_BOUNDS( eBLEND_SRC_INDEX, eBLEND_SRC_INDEX_0 )
	}	eBLEND_SRC_INDEX;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Texture blending sources enumeration
	\~french
	\brief		Enumération des sources de blending
	*/
	typedef enum eBLEND_SOURCE
		: uint8_t
	{
		eBLEND_SOURCE_TEXTURE,
		eBLEND_SOURCE_TEXTURE0,
		eBLEND_SOURCE_TEXTURE1,
		eBLEND_SOURCE_TEXTURE2,
		eBLEND_SOURCE_TEXTURE3,
		eBLEND_SOURCE_CONSTANT,
		eBLEND_SOURCE_DIFFUSE,
		eBLEND_SOURCE_PREVIOUS,
		CASTOR_ENUM_BOUNDS( eBLEND_SOURCE, eBLEND_SOURCE_TEXTURE )
	}	eBLEND_SOURCE;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		RGB blending functions enumeration
	\~french
	\brief		Enumération de fonctions de mélange RGB
	*/
	typedef enum eRGB_BLEND_FUNC
		: uint8_t
	{
		//!\~english No blend
		//!\~french Pas de mélange
		eRGB_BLEND_FUNC_NONE,
		//!\~english Arg0
		//!\~french Arg0
		eRGB_BLEND_FUNC_FIRST_ARG,
		//!\~english Arg0 + Arg1
		//!\~french Arg0 + Arg1
		eRGB_BLEND_FUNC_ADD,
		//!\~english Arg0 + Arg1 - 0.5
		//!\~french Arg0 + Arg1 - 0.5
		eRGB_BLEND_FUNC_ADD_SIGNED,
		//!\~english Arg0 x Arg1
		//!\~french Arg0 x Arg1
		eRGB_BLEND_FUNC_MODULATE,
		//!\~english Arg0 × Arg2 + Arg1 × (1 - Arg2)
		//!\~french Arg0 × Arg2 + Arg1 × (1 - Arg2)
		eRGB_BLEND_FUNC_INTERPOLATE,
		//!\~english Arg0 - Arg1
		//!\~french Arg0 - Arg1
		eRGB_BLEND_FUNC_SUBTRACT,
		//!\~english 4 × (((Arg0.r - 0.5) × (Arg1.r - 0.5)) + ((Arg0.g - 0.5) × (Arg1.g - 0.5)) + ((Arg0.b - 0.5) × (Arg1.b - 0.5)))
		//!\~french 4 × (((Arg0.r - 0.5) × (Arg1.r - 0.5)) + ((Arg0.g - 0.5) × (Arg1.g - 0.5)) + ((Arg0.b - 0.5) × (Arg1.b - 0.5)))
		eRGB_BLEND_FUNC_DOT3_RGB,
		//!\~english 4 × (((Arg0.r - 0.5) × (Arg1.r - 0.5)) + ((Arg0.g - 0.5) × (Arg1.g - 0.5)) + ((Arg0.b - 0.5) × (Arg1.b - 0.5)) + ((Arg0.a - 0.5) × (Arg1.a - 0.5)))
		//!\~french 4 × (((Arg0.r - 0.5) × (Arg1.r - 0.5)) + ((Arg0.g - 0.5) × (Arg1.g - 0.5)) + ((Arg0.b - 0.5) × (Arg1.b - 0.5)) + ((Arg0.a - 0.5) × (Arg1.a - 0.5)))
		eRGB_BLEND_FUNC_DOT3_RGBA,
		CASTOR_ENUM_BOUNDS( eRGB_BLEND_FUNC, eRGB_BLEND_FUNC_NONE )
	}	eRGB_BLEND_FUNC;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Alpha blending functions enumeration
	\~french
	\brief		Enumération de fonctions de mélange alpha
	*/
	typedef enum eALPHA_BLEND_FUNC
		: uint8_t
	{
		//!\~english No blend
		//!\~french Pas de mélange
		eALPHA_BLEND_FUNC_NONE,
		//!\~english Arg0
		//!\~french Arg0
		eALPHA_BLEND_FUNC_FIRST_ARG,
		//!\~english Arg0 + Arg1
		//!\~french Arg0 + Arg1
		eALPHA_BLEND_FUNC_ADD,
		//!\~english Arg0 + Arg1 - 0.5
		//!\~french Arg0 + Arg1 - 0.5
		eALPHA_BLEND_FUNC_ADD_SIGNED,
		//!\~english Arg0 x Arg1
		//!\~french Arg0 x Arg1
		eALPHA_BLEND_FUNC_MODULATE,
		//!\~english Arg0 × Arg2 + Arg1 × (1 - Arg2)
		//!\~french Arg0 × Arg2 + Arg1 × (1 - Arg2)
		eALPHA_BLEND_FUNC_INTERPOLATE,
		//!\~english Arg0 - Arg1
		//!\~french Arg0 - Arg1
		eALPHA_BLEND_FUNC_SUBSTRACT,
		CASTOR_ENUM_BOUNDS( eALPHA_BLEND_FUNC, eALPHA_BLEND_FUNC_NONE )
	}	eALPHA_BLEND_FUNC;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Blending operations enumeration
	\~french
	\brief		Enumération de opérations de mélange
	*/
	typedef enum eBLEND_OP
		: uint8_t
	{
		//!\~english Src2 + Src1.
		//!\~french Src2 + Src1.
		eBLEND_OP_ADD,
		//!\~english Src2 - Src1.
		//!\~french Src2 - Src1.
		eBLEND_OP_SUBSTRACT,
		//!\~english Src1 - Src2.
		//!\~french Src1 - Src2.
		eBLEND_OP_REV_SUBSTRACT,
		//!\~english min( Src1, Src2 ).
		//!\~french min( Src1, Src2 ).
		eBLEND_OP_MIN,
		//!\~english max( Src1, Src2 ).
		//!\~french max( Src1, Src2 ).
		eBLEND_OP_MAX,
		eBLEND_OP_COUNT
	}	eBLEND_OP;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Blending modes enumeration
	\~french
	\brief		Enumération des modes de mélange
	*/
	typedef enum eBLEND_MODE
		: uint8_t
	{
		//!\~english Order dependent blending.
		//!\~french Mélange dépendant de l'ordre.
		eBLEND_MODE_NONE,
		//!\~english Order independent, add the components.
		//!\~french Mélange indépendant de l'ordre, additionnant les composantes.
		eBLEND_MODE_ADDITIVE,
		//!\~english Order independent, multiply the components.
		//!\~french Indépendant de l'ordre, multipliant les composantes.
		eBLEND_MODE_MULTIPLICATIVE,
		//!\~english Order dependent, interpolate the components.
		//!\~french Indépendant de l'ordre, interpolant les composantes.
		eBLEND_MODE_INTERPOLATIVE,
		//!\~english Order independent, using A-buffer, not implemented yet.
		//!\~french Indépendant de l'ordre, utilisant les A-Buffer (non implémenté).
		eBLEND_MODE_A_BUFFER,
		//!\~english Order independent, using depth peeling, not implemented yet.
		//!\~french Indépendant de l'ordre, utilisant le pelage en profondeur (non implémenté).
		eBLEND_MODE_DEPTH_PEELING,
		CASTOR_ENUM_BOUNDS( eBLEND_MODE, eBLEND_MODE_NONE )
	}	eBLEND_MODE;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Texture dimensions enumeration
	\~french
	\brief		Enumération des dimensions de texture
	*/
	typedef enum eTEXTURE_TYPE
		: uint8_t
	{
		//!\~english 1 dimension texture buffer, used to store data.
		//!\~french tampons de texture à 1 dimension, utilisé pour stocker des données.
		eTEXTURE_TYPE_BUFFER,
		//!\~english 1 dimension textures => one coordinate => U mapping
		//!\~french Textures à 1 dimension => une coordonnée => U mapping
		eTEXTURE_TYPE_1D,
		//!\~english 1 dimension textures array => one coordinate => U mapping
		//!\~french Tableau de textures à 1 dimension => une coordonnée => U mapping
		eTEXTURE_TYPE_1DARRAY,
		//!\~english 2 dimensions textures => two coordinates => UV mapping
		//!\~french Textures à 2 dimensions => deux coordonnées => UV mapping
		eTEXTURE_TYPE_2D,
		//!\~english 2 dimensions textures array => two coordinates => UV mapping
		//!\~french Tableau de textures à 2 dimensions => deux coordonnées => UV mapping
		eTEXTURE_TYPE_2DARRAY,
		//!\~english 2 dimensions textures with multisample support => two coordinates => UV mapping
		//!\~french Textures à 2 dimensions avec support du multisampling => deux coordonnées => UV mapping
		eTEXTURE_TYPE_2DMS,
		//!\~english 2 dimensions textures array with multisample support => two coordinates => UV mapping
		//!\~french Tableau de textures à 2 dimensions => deux coordonnées => UV mapping
		eTEXTURE_TYPE_2DMSARRAY,
		//!\~english 3 dimensions textures => three coordinates => UVW mapping
		//!\~french Textures à 3 dimensions => trois coordonnées => UVW mapping
		eTEXTURE_TYPE_3D,
		//!\~english Cube texture.
		//!\~french Textures cube.
		eTEXTURE_TYPE_CUBE,
		//!\~english Cube textures array.
		//!\~french Tableaux de textures cube.
		eTEXTURE_TYPE_CUBEARRAY,
		CASTOR_ENUM_BOUNDS( eTEXTURE_TYPE, eTEXTURE_TYPE_BUFFER )
	}	eTEXTURE_TYPE;
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
	\brief		Blend operators enumeration
	\~french
	\brief		Enumération des opérateurs de mélange
	*/
	typedef enum eBLEND
		: uint8_t
	{
		eBLEND_ZERO,
		eBLEND_ONE,
		eBLEND_SRC_COLOUR,
		eBLEND_INV_SRC_COLOUR,
		eBLEND_DST_COLOUR,
		eBLEND_INV_DST_COLOUR,
		eBLEND_SRC_ALPHA,
		eBLEND_INV_SRC_ALPHA,
		eBLEND_DST_ALPHA,
		eBLEND_INV_DST_ALPHA,
		eBLEND_CONSTANT,
		eBLEND_INV_CONSTANT,
		eBLEND_SRC_ALPHA_SATURATE,
		eBLEND_SRC1_COLOUR,
		eBLEND_INV_SRC1_COLOUR,
		eBLEND_SRC1_ALPHA,
		eBLEND_INV_SRC1_ALPHA,
		CASTOR_ENUM_BOUNDS( eBLEND, eBLEND_ZERO )
	}	eBLEND;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Texture channels enumeration
	\~french
	\brief		Enumération des canaux de texture
	*/
	typedef enum eTEXTURE_CHANNEL
		: uint32_t
	{
		//!\~english Colour map
		//!\~french Map de couleurs
		eTEXTURE_CHANNEL_COLOUR = 0x00000001,
		//!\~english Diffuse map
		//!\~french Map de diffuse
		eTEXTURE_CHANNEL_DIFFUSE = 0x00000002,
		//!\~english Normal map
		//!\~french Map de normales
		eTEXTURE_CHANNEL_NORMAL = 0x00000004,
		//!\~english Opacity map
		//!\~french Map d'opacité
		eTEXTURE_CHANNEL_OPACITY = 0x00000008,
		//!\~english Specular map
		//!\~french Map de spéculaire
		eTEXTURE_CHANNEL_SPECULAR = 0x00000010,
		//!\~english Height map
		//!\~french Map de hauteur
		eTEXTURE_CHANNEL_HEIGHT = 0x00000020,
		//!\~english Ambient map
		//!\~french Map d'ambiante
		eTEXTURE_CHANNEL_AMBIENT = 0x00000040,
		//!\~english Gloss map
		//!\~french Map de gloss
		eTEXTURE_CHANNEL_GLOSS = 0x00000080,
		//!\~english Emissive map
		//!\~french Map d'émissive
		eTEXTURE_CHANNEL_EMISSIVE = 0x00000100,
		//!\~english Mask for all the texture channels
		//!\~french Masque pour les canaux de texture
		eTEXTURE_CHANNEL_ALL = 0x0000FFFF,
		//!\~english Not really a texture channel (it is out of eTEXTURE_CHANNEL_ALL), used to tell we want text overlay shader source
		//!\~french Pas vraiment un canal de texture (hors de eTEXTURE_CHANNEL_ALL), utilisé pour dire que nous voulons un shader d'incrustation texte
		eTEXTURE_CHANNEL_TEXT = 0x00010000,
	}	eTEXTURE_CHANNEL;

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

	//@}
}

#endif
