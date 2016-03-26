/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

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
	\brief		Texture mapping modes enumeration
	\~french
	\brief		Enumération des modes de mapping des textures
	*/
	typedef enum eTEXTURE_MAP_MODE
	CASTOR_TYPE( uint8_t )
	{
		eTEXTURE_MAP_MODE_NONE,			//!<\~english Usual map mode	\~french Mode habituel (aucune transformation des UVW)
		eTEXTURE_MAP_MODE_REFLECTION,	//!<\~english Reflexion map mode	\~french Mode reflexion map
		eTEXTURE_MAP_MODE_SPHERE,		//!<\~english Sphere map mode	\~french Mode sphere map
		eTEXTURE_MAP_MODE_COUNT,
	}	eTEXTURE_MAP_MODE;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Alpha functions enumeration
	\~french
	\brief		Enumération des fonctions alpha
	*/
	typedef enum eALPHA_FUNC
	CASTOR_TYPE( uint8_t )
	{
		eALPHA_FUNC_ALWAYS,				//!<\~english Always take texture colour	\~french Prend toujours la couleur de la texture
		eALPHA_FUNC_LESS,				//!<\~english Takes texture colour if alpha is less than given value	\~french Prend la couleur de la texture si l'alpha est inferieur a la valeur donnee
		eALPHA_FUNC_LESS_OR_EQUAL,		//!<\~english Takes texture colour if alpha is less than or equal to given value	\~french Prend la couleur de la texture si l'alpha est inferieur ou egal a la valeur donnee
		eALPHA_FUNC_EQUAL,				//!<\~english Takes texture colour if alpha is equal to given value	\~french Prend la couleur de la texture si l'alpha est egal a la valeur donnee
		eALPHA_FUNC_NOT_EQUAL,			//!<\~english Takes texture colour if alpha is different of given value	\~french Prend la couleur de la texture si l'alpha est different de la valeur donnee
		eALPHA_FUNC_GREATER_OR_EQUAL,	//!<\~english Takes texture colour if alpha is grater than or equal to given value	\~french Prend la couleur de la texture si l'alpha est superieur ou egal a la valeur donnee
		eALPHA_FUNC_GREATER,			//!<\~english Takes texture colour if alpha is greater than given value	\~french Prend la couleur de la texture si l'alpha est superieur a la valeur donnee
		eALPHA_FUNC_NEVER,				//!<\~english Never take texture colour	\~french Ne prend jamais la couleur de la texture
		eALPHA_FUNC_COUNT,
	}	eALPHA_FUNC;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Texture UVW enumeration
	\~french
	\brief		Enumération des UVW
	*/
	typedef enum eTEXTURE_UVW
	CASTOR_TYPE( uint8_t )
	{
		eTEXTURE_UVW_U,
		eTEXTURE_UVW_V,
		eTEXTURE_UVW_W,
		eTEXTURE_UVW_COUNT,
	}	eTEXTURE_UVW;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Texture wrap modes enumeration
	\~french
	\brief		Enumération des modes de wrapping de texture
	*/
	typedef enum eWRAP_MODE
	CASTOR_TYPE( uint8_t )
	{
		eWRAP_MODE_REPEAT,
		eWRAP_MODE_MIRRORED_REPEAT,
		eWRAP_MODE_CLAMP_TO_BORDER,
		eWRAP_MODE_CLAMP_TO_EDGE,
		eWRAP_MODE_COUNT,
	}	eWRAP_MODE;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Texture interpolation filters enumeration
	\~french
	\brief		Enumération des filtres d'interpolation
	*/
	typedef enum eINTERPOLATION_FILTER
	CASTOR_TYPE( uint8_t )
	{
		eINTERPOLATION_FILTER_MIN,
		eINTERPOLATION_FILTER_MAG,
		eINTERPOLATION_FILTER_MIP,
		eINTERPOLATION_FILTER_COUNT,
	}	eINTERPOLATION_FILTER;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Texture filter interpolation modes enumeration
	\~french
	\brief		Enumération des modes d'interpolation
	*/
	typedef enum eINTERPOLATION_MODE
	CASTOR_TYPE( uint8_t )
	{
		eINTERPOLATION_MODE_UNDEFINED,
		eINTERPOLATION_MODE_NEAREST,
		eINTERPOLATION_MODE_LINEAR,
		eINTERPOLATION_MODE_ANISOTROPIC,
		eINTERPOLATION_MODE_COUNT,
	}	eINTERPOLATION_MODE;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Texture blending source indices enumeration
	\~french
	\brief		Enumération des indices de source de blending
	*/
	typedef enum eBLEND_SRC_INDEX
	CASTOR_TYPE( uint8_t )
	{
		eBLEND_SRC_INDEX_0,
		eBLEND_SRC_INDEX_1,
		eBLEND_SRC_INDEX_2,
		eBLEND_SRC_INDEX_COUNT,
	}	eBLEND_SRC_INDEX;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Texture blending sources enumeration
	\~french
	\brief		Enumération des sources de blending
	*/
	typedef enum eBLEND_SOURCE
	CASTOR_TYPE( uint8_t )
	{
		eBLEND_SOURCE_TEXTURE,
		eBLEND_SOURCE_TEXTURE0,
		eBLEND_SOURCE_TEXTURE1,
		eBLEND_SOURCE_TEXTURE2,
		eBLEND_SOURCE_TEXTURE3,
		eBLEND_SOURCE_CONSTANT,
		eBLEND_SOURCE_DIFFUSE,
		eBLEND_SOURCE_PREVIOUS,
		eBLEND_SOURCE_COUNT,
	}	eBLEND_SOURCE;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		RGB blending functions enumeration
	\~french
	\brief		Enumération de fonctions de mélange RGB
	*/
	typedef enum eRGB_BLEND_FUNC
	CASTOR_TYPE( uint8_t )
	{
		eRGB_BLEND_FUNC_NONE,
		eRGB_BLEND_FUNC_FIRST_ARG,		//!< Arg0
		eRGB_BLEND_FUNC_ADD,			//!< Arg0 + Arg1
		eRGB_BLEND_FUNC_ADD_SIGNED,		//!< Arg0 + Arg1 - 0.5
		eRGB_BLEND_FUNC_MODULATE,		//!< Arg0 x Arg1
		eRGB_BLEND_FUNC_INTERPOLATE,	//!< Arg0 × Arg2 + Arg1 × (1 - Arg2)
		eRGB_BLEND_FUNC_SUBTRACT,		//!< Arg0 - Arg1
		eRGB_BLEND_FUNC_DOT3_RGB,		//!< 4 × (((Arg0.r - 0.5) × (Arg1.r - 0.5)) + ((Arg0.g - 0.5) × (Arg1.g - 0.5)) + ((Arg0.b - 0.5) × (Arg1.b - 0.5)))
		eRGB_BLEND_FUNC_DOT3_RGBA,		//!< 4 × (((Arg0.r - 0.5) × (Arg1.r - 0.5)) + ((Arg0.g - 0.5) × (Arg1.g - 0.5)) + ((Arg0.b - 0.5) × (Arg1.b - 0.5)) + ((Arg0.a - 0.5) × (Arg1.a - 0.5)))
		eRGB_BLEND_FUNC_COUNT,
	}	eRGB_BLEND_FUNC;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Alpha blending functions enumeration
	\~french
	\brief		Enumération de fonctions de mélange alpha
	*/
	typedef enum eALPHA_BLEND_FUNC
	CASTOR_TYPE( uint8_t )
	{
		eALPHA_BLEND_FUNC_NONE,
		eALPHA_BLEND_FUNC_FIRST_ARG,	//!< Arg0
		eALPHA_BLEND_FUNC_ADD,			//!< Arg0 + Arg1
		eALPHA_BLEND_FUNC_ADD_SIGNED,	//!< Arg0 + Arg1 - 0.5
		eALPHA_BLEND_FUNC_MODULATE,		//!< Arg0 x Arg1
		eALPHA_BLEND_FUNC_INTERPOLATE,	//!< Arg0 × Arg2 + Arg1 × (1 - Arg2)
		eALPHA_BLEND_FUNC_SUBSTRACT,	//!< Arg0 - Arg1
		eALPHA_BLEND_FUNC_COUNT,
	}	eALPHA_BLEND_FUNC;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Blending operations enumeration
	\~french
	\brief		Enumération de opérations de mélange
	*/
	typedef enum eBLEND_OP
	CASTOR_TYPE( uint8_t )
	{
		eBLEND_OP_ADD,				//!< Add source 1 and source 2.
		eBLEND_OP_SUBSTRACT,		//!< Subtract source 1 from source 2.
		eBLEND_OP_REV_SUBSTRACT,	//!< Subtract source 2 from source 1.
		eBLEND_OP_MIN,				//!< Find the minimum of source 1 and source 2.
		eBLEND_OP_MAX,				//!< Find the maximum of source 1 and source 2.
		eBLEND_OP_COUNT,
	}	eBLEND_OP;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Blending modes enumeration
	\~french
	\brief		Enumération des modes de mélange
	*/
	typedef enum eBLEND_MODE
	CASTOR_TYPE( uint8_t )
	{
		eBLEND_MODE_NONE,			//!< Order dependent blending.
		eBLEND_MODE_ADDITIVE,		//!< Order independent, add the components.
		eBLEND_MODE_MULTIPLICATIVE,	//!< Order independent, multiply the components.
		eBLEND_MODE_INTERPOLATIVE,	//!< Order dependent, interpolate the components.
		eBLEND_MODE_A_BUFFER,		//!< Order independent, using A-buffer, not implemented yet.
		eBLEND_MODE_DEPTH_PEELING,	//!< Order independent, using depth peeling, not implemented yet.
		eBLEND_MODE_COUNT,
	}	eBLEND_MODE;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Texture dimensions enumeration
	\~french
	\brief		Enumération des dimensions de texture
	*/
	typedef enum eTEXTURE_TYPE
	CASTOR_TYPE( uint8_t )
	{
		eTEXTURE_TYPE_BUFFER,		//!< 1 dimension texture buffer, used to store data.
		eTEXTURE_TYPE_1D,			//!< 1 dimension textures = > one coordinate = > U mapping
		eTEXTURE_TYPE_1DARRAY,		//!< 1 dimension textures = > one coordinate = > U mapping
		eTEXTURE_TYPE_2D,			//!< 2 dimensions textures = > two coordinates = > UV mapping
		eTEXTURE_TYPE_2DARRAY,		//!< 2 dimensions textures array = > two coordinates = > UV mapping
		eTEXTURE_TYPE_2DMS,			//!< 2 dimensions textures with multisample support = > two coordinates = > UV mapping
		eTEXTURE_TYPE_2DMSARRAY,	//!< 2 dimensions textures array with multisample support = > two coordinates = > UV mapping
		eTEXTURE_TYPE_3D,			//!< 3 dimensions textures = > three coordinates = > UVW mapping
		eTEXTURE_TYPE_CUBE,			//!< Cube texture, for environment rendering.
		eTEXTURE_TYPE_CUBEARRAY,	//!< Cube textures array, for environment rendering.
		eTEXTURE_TYPE_COUNT,
	}	eTEXTURE_TYPE;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Texture targets enumeration
	\~french
	\brief		Enumération des cibles de texture
	*/
	typedef enum eTEXTURE_TARGET
	CASTOR_TYPE( uint8_t )
	{
		eTEXTURE_TARGET_1D,		//!< 1 dimension texture
		eTEXTURE_TARGET_2D,		//!< 2 dimensions texture
		eTEXTURE_TARGET_3D,		//!< 3 dimensions texture
		eTEXTURE_TARGET_LAYER,	//!< Layer texture
		eTEXTURE_TARGET_COUNT,
	}	eTEXTURE_TARGET;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Texture types enumeration
	\~french
	\brief		Enumération des types de texture
	*/
	typedef enum eTEXTURE_BASE_TYPE
	CASTOR_TYPE( uint8_t )
	{
		eTEXTURE_BASE_TYPE_STATIC,
		eTEXTURE_BASE_TYPE_DYNAMIC,
	}	eTEXTURE_BASE_TYPE;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Blend operators enumeration
	\~french
	\brief		Enumération des opérateurs de mélange
	*/
	typedef enum eBLEND
	CASTOR_TYPE( uint8_t )
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
		eBLEND_COUNT,
	}	eBLEND;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Texture channels enumeration
	\~french
	\brief		Enumération des canaux de texture
	*/
	typedef enum eTEXTURE_CHANNEL
	CASTOR_TYPE( uint32_t )
	{
		eTEXTURE_CHANNEL_COLOUR = 0x00000001,	//!< Colour map
		eTEXTURE_CHANNEL_DIFFUSE = 0x00000002,	//!< Diffuse map
		eTEXTURE_CHANNEL_NORMAL = 0x00000004,	//!< Normal map
		eTEXTURE_CHANNEL_OPACITY = 0x00000008,	//!< Opacity map
		eTEXTURE_CHANNEL_SPECULAR = 0x00000010,	//!< Specular map
		eTEXTURE_CHANNEL_HEIGHT = 0x00000020,	//!< Height map
		eTEXTURE_CHANNEL_AMBIENT = 0x00000040,	//!< Ambient map
		eTEXTURE_CHANNEL_GLOSS = 0x00000080,	//!< Gloss map
		eTEXTURE_CHANNEL_EMISSIVE = 0x00000100,	//!< Gloss map
		eTEXTURE_CHANNEL_ALL = 0x0000FFFF,
		eTEXTURE_CHANNEL_TEXT = 0x00010000,		//!< Not really a texture channel (it is out of eTEXTURE_CHANNEL_ALL), used to tell we want text overlay shader source
	}	eTEXTURE_CHANNEL;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Light indexes enumeration
	\~french
	\brief		Enumération des indeices des lumières
	*/
	typedef enum eLIGHT_INDEXES
	CASTOR_TYPE( uint8_t )
	{
		eLIGHT_INDEXES_0,
		eLIGHT_INDEXES_1,
		eLIGHT_INDEXES_2,
		eLIGHT_INDEXES_3,
		eLIGHT_INDEXES_4,
		eLIGHT_INDEXES_5,
		eLIGHT_INDEXES_6,
		eLIGHT_INDEXES_7,
		eLIGHT_INDEXES_COUNT,
	}	eLIGHT_INDEXES;

	class Texture;
	class StaticTexture;
	class DynamicTexture;
	class TextureUnit;
	class Material;
	class Pass;
	class Sampler;

	DECLARE_SMART_PTR( Texture );
	DECLARE_SMART_PTR( StaticTexture );
	DECLARE_SMART_PTR( DynamicTexture );
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
