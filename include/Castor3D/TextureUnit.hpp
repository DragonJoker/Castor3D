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
#ifndef ___CU_TextureUnit___
#define ___CU_TextureUnit___

#include "Prerequisites.hpp"
#include "Renderable.hpp"

namespace Castor
{	namespace Resources
{
	//! TextureUnit loader
	/*!
	Loads and saves texture units from/into a file
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	template <> class C3D_API Loader<Castor3D::TextureUnit>
	{
	public:
		/**
		 * Loads a texture unit from a binary file
		 *@param p_pass : [in/out] The texture unit to load
		 *@param p_file : [in/out] The file where to load the texture unit
		 */
		static bool Load( Castor3D::TextureUnit & p_unit, Utils::File & p_file);
		/**
		 * Saves a texture unit into a binary file
		 *@param p_pass : [in] The texture unit to save
		 *@param p_file : [in/out] The file where to save the texture unit
		 */
		static bool Save( const Castor3D::TextureUnit & p_unit, Utils::File & p_file);
		/**
		 * Writes a texture unit into a text file
		 *@param p_pass : [in] The texture unit to write
		 *@param p_file : [in/out] The file where to write the texture unit
		 */
		static bool Write( const Castor3D::TextureUnit & p_unit, Utils::File & p_file);
	};
}
}

namespace Castor3D
{
	//! Texture dimensions enumerator
	/*!
	The different texture dimensions are 1 dimension, 2 dimensions and 3 dimensions
	*/
	typedef enum
	{	eTEXTURE_TYPE_1D	//!< 1 dimension textures => one coordinate => U mapping
	,	eTEXTURE_TYPE_2D	//!< 2 dimensions textures => two coordinates => UV mapping
	,	eTEXTURE_TYPE_3D	//!< 3 dimensions textures => three coordinates => UVW mapping
	,	eTEXTURE_TYPE_COUNT
	}	eTEXTURE_TYPE;
	//! Texture unit representation
	/*!
	A texture unit is a simple texture which can have an environment mode.
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class C3D_API TextureUnit : public Renderable<TextureUnit, TextureRenderer>
	{
	public:
		//! Map modes enumerator
		typedef enum
		{	eMAP_MODE_NONE		//!< Usual map mode
		,	eMAP_MODE_REFLEXION	//!< Reflexion map mode
		,	eMAP_MODE_SPHERE	//!< Sphere map mode
		,	eMAP_MODE_COUNT
		}	eMAP_MODE;

		//! Alpha functions enumerator
		typedef enum
		{	eALPHA_FUNC_ALWAYS
		,	eALPHA_FUNC_LESS
		,	eALPHA_FUNC_LESS_OR_EQUAL
		,	eALPHA_FUNC_EQUAL
		,	eALPHA_FUNC_NOT_EQUAL
		,	eALPHA_FUNC_GREATER_OR_EQUAL
		,	eALPHA_FUNC_GREATER
		,	eALPHA_FUNC_NEVER
		,	eALPHA_FUNC_COUNT
		}	eALPHA_FUNC;

		//! Texture UVW enumerator
		typedef enum
		{	eUVW_U
		,	eUVW_V
		,	eUVW_W
		,	eUVW_COUNT
		}	eUVW;

		//! Texture wrap modes enumerator
		typedef enum
		{	eWRAP_MODE_REPEAT
		,	eWRAP_MODE_CLAMP
		,	eWRAP_MODE_CLAMP_TO_BORDER
		,	eWRAP_MODE_CLAMP_TO_EDGE
		,	eWRAP_MODE_COUNT
		}	eWRAP_MODE;

		//! Texture filter interpolations limits enumerator
		typedef enum
		{	eINTERPOLATION_FILTER_MIN
		,	eINTERPOLATION_FILTER_MAG
		,	eINTERPOLATION_FILTER_COUNT
		}	eINTERPOLATION_FILTER;

		//! Texture filter interpolations modes enumerator
		typedef enum
		{	eINTERPOLATION_MODE_NEAREST
		,	eINTERPOLATION_MODE_LINEAR
		,	eINTERPOLATION_MODE_ANISOTROPIC
		,	eINTERPOLATION_MODE_COUNT
		}	eINTERPOLATION_MODE;

		//! Texture blending sources count enumerator
		typedef enum
		{	eBLEND_SRC_INDEX_0
		,	eBLEND_SRC_INDEX_1
		,	eBLEND_SRC_INDEX_2
		,	eBLEND_SRC_INDEX_COUNT
		}	eBLEND_SRC_INDEX;

		//! Texture blending arguments enumerator
		typedef enum
		{	eBLEND_SOURCE_TEXTURE
		,	eBLEND_SOURCE_TEXTURE0
		,	eBLEND_SOURCE_TEXTURE1
		,	eBLEND_SOURCE_TEXTURE2
		,	eBLEND_SOURCE_TEXTURE3
		,	eBLEND_SOURCE_CONSTANT
		,	eBLEND_SOURCE_DIFFUSE
		,	eBLEND_SOURCE_PREVIOUS
		,	eBLEND_SOURCE_COUNT
		}	eBLEND_SOURCE;

		//! Texture blending RGB functions enumerator
		typedef enum
		{	eRGB_BLEND_FUNC_NONE
		,	eRGB_BLEND_FUNC_FIRST_ARG	// Arg0
		,	eRGB_BLEND_FUNC_ADD			// Arg0 + Arg1
		,	eRGB_BLEND_FUNC_ADD_SIGNED	// Arg0 + Arg1 - 0.5
		,	eRGB_BLEND_FUNC_MODULATE	// Arg0 x Arg1
		,	eRGB_BLEND_FUNC_INTERPOLATE	// Arg0 × Arg2 + Arg1 × (1 - Arg2)
		,	eRGB_BLEND_FUNC_SUBSTRACT	// Arg0 - Arg1
		,	eRGB_BLEND_FUNC_DOT3_RGB	// 4 × (((Arg0.r - 0.5) × (Arg1.r - 0.5)) + ((Arg0.g - 0.5) × (Arg1.g - 0.5)) + ((Arg0.b - 0.5) × (Arg1.b - 0.5)))
		,	eRGB_BLEND_FUNC_DOT3_RGBA	// 4 × (((Arg0.r - 0.5) × (Arg1.r - 0.5)) + ((Arg0.g - 0.5) × (Arg1.g - 0.5)) + ((Arg0.b - 0.5) × (Arg1.b - 0.5)) + ((Arg0.a - 0.5) × (Arg1.a - 0.5)))
		,	eRGB_BLEND_FUNC_COUNT
		}	eRGB_BLEND_FUNC;

		//! Texture blending ALPHA functions enumerator
		typedef enum
		{	eALPHA_BLEND_FUNC_NONE
		,	eALPHA_BLEND_FUNC_FIRST_ARG		// Arg0
		,	eALPHA_BLEND_FUNC_ADD			// Arg0 + Arg1
		,	eALPHA_BLEND_FUNC_ADD_SIGNED	// Arg0 + Arg1 - 0.5
		,	eALPHA_BLEND_FUNC_MODULATE		// Arg0 x Arg1
		,	eALPHA_BLEND_FUNC_INTERPOLATE	// Arg0 × Arg2 + Arg1 × (1 - Arg2)
		,	eALPHA_BLEND_FUNC_SUBSTRACT		// Arg0 - Arg1 
		,	eALPHA_BLEND_FUNC_COUNT
		}	eALPHA_BLEND_FUNC;

		static std::map<String, eTEXTURE_TYPE> MapTypes;
		static const String StringTypes[eTEXTURE_TYPE_COUNT];

		static std::map<String, eMAP_MODE> MapMapModes;
		static const String StringMapModes[eMAP_MODE_COUNT];

		static std::map<String, eALPHA_FUNC> MapAlphaFuncs;
		static const String StringAlphaFuncs[eALPHA_FUNC_COUNT];

		static std::map<String, eBLEND_SOURCE> MapTextureArguments;
		static const String StringTextureArguments[eBLEND_SOURCE_COUNT];

		static std::map<String, eRGB_BLEND_FUNC> MapTextureRgbFunctions;
		static const String StringTextureRgbFunctions[eRGB_BLEND_FUNC_COUNT];

		static std::map<String, eALPHA_BLEND_FUNC> MapTextureAlphaFunctions;
		static const String StringTextureAlphaFunctions[eALPHA_BLEND_FUNC_COUNT];

	protected:
		friend class TextureRenderer;						//!< The texture renderer class C3D_API can access protected and private functions
		unsigned int				m_uiIndex;				//!< This texture index in parent pass
		Castor::Resources::ImagePtr	m_pImage;				//!< The image resource
		eTEXTURE_TYPE						m_eDimension;			//!< The texture dimension
		bool						m_bTextureInitialised;	//!< Tells whether or not the texture is initialised
		bool						m_bTextured;			//!< Tells whether the texture is loaded (the image path has been given) or not
		Colour						m_clrPrimaryColour;		//!< The primary colour used in environment modes
		eMAP_MODE					m_eMode;				//!< The mapping mode for the texture
		float						m_fAlphaValue;			//!< The max rejected alpha
		eALPHA_FUNC					m_eAlphaFunc;
		eWRAP_MODE					m_eWrapModes[eUVW_COUNT];
		eINTERPOLATION_MODE			m_eInterpolationModes[eINTERPOLATION_FILTER_COUNT];
		eBLEND_SOURCE				m_eRgbArguments[eBLEND_SRC_INDEX_COUNT];
		eBLEND_SOURCE				m_eAlpArguments[eBLEND_SRC_INDEX_COUNT];
		eRGB_BLEND_FUNC				m_eRgbFunction;
		eALPHA_BLEND_FUNC			m_eAlpFunction;

	public:
		/**
		 * Constructor
		 */
		TextureUnit();
		/**
		 * Copy constructor
		 */
		TextureUnit( const TextureUnit & p_copy);
		/**
		 * Assignment operator
		 */
		TextureUnit & operator =( const TextureUnit & p_copy);
		/**
		 * Constructor
		 */
		~TextureUnit();
		/**
		 * Initialises the texture, id est : fills the image buffer, creates the texture in the render system
		 */
		void Initialise();
		/**
		 * Cleans up the texture
		 */
		void Cleanup();
		/**
		 * Sets the image path
		 *@param p_name : [in] The image path
		 */
		void SetTexture( eTEXTURE_TYPE p_eDimension, ImagePtr p_pImage);
		/**
		 * Applies the texture unit and it's environment mode
		 */
		virtual void Render( ePRIMITIVE_TYPE p_displayMode);
		/**
		 * Removes the texture unit from the stack, in order not to interfere with other ones
		 */
		virtual void EndRender();
		unsigned char const *	GetImagePixels()const;
		unsigned char *			GetImagePixels();
		size_t					GetImageSize()const;
		unsigned int			GetWidth()const;
		unsigned int			GetHeight()const;
		String					GetTexturePath()const;
		ePIXEL_FORMAT			GetPixelFormat();

		/**@name Accessors */
		//@{
		inline unsigned int			GetIndex				()const 								{ return m_uiIndex; }
		inline ImagePtr				GetImage				()const 								{ return m_pImage; }
		inline eTEXTURE_TYPE		GetDimension			()const 								{ return m_eDimension; }
		inline Colour const &		GetPrimaryColour		()const 								{ return m_clrPrimaryColour; }
		inline Colour &				GetPrimaryColour		()		 								{ return m_clrPrimaryColour; }
		inline bool					IsTextured				()const 								{ return m_bTextured; }
		inline bool					TextureInitialised		()const 								{ return m_bTextureInitialised; }
		inline eMAP_MODE			GetTextureMapMode		()const 								{ return m_eMode; }
		inline eALPHA_FUNC			GetAlphaFunc			()const 								{ return m_eAlphaFunc; }
		inline float				GetAlphaValue			()const 								{ return m_fAlphaValue; }
		inline eWRAP_MODE			GetWrapMode				( eUVW uiDim)const						{ return m_eWrapModes[uiDim]; }
		inline eINTERPOLATION_MODE	GetInterpolationMode	( eINTERPOLATION_FILTER uiDim)const		{ return m_eInterpolationModes[uiDim]; }
		inline eRGB_BLEND_FUNC		GetRgbFunction			()const 								{ return m_eRgbFunction; }
		inline eALPHA_BLEND_FUNC	GetAlpFunction			()const 								{ return m_eAlpFunction; }
		inline eBLEND_SOURCE		GetAlpArgument			( eBLEND_SRC_INDEX p_uiIndex)const		{ return m_eAlpArguments[p_uiIndex]; }
		inline eBLEND_SOURCE		GetRgbArgument			( eBLEND_SRC_INDEX p_uiIndex)const		{ return m_eRgbArguments[p_uiIndex]; }

		inline void SetRgbArgument			( eBLEND_SRC_INDEX p_uiIndex, eBLEND_SOURCE val)		{ m_eRgbArguments[p_uiIndex] = val; }
		inline void SetAlpArgument			( eBLEND_SRC_INDEX p_uiIndex, eBLEND_SOURCE val)		{ m_eAlpArguments[p_uiIndex] = val; }
		inline void SetAlpMode				( eALPHA_BLEND_FUNC val)								{ m_eAlpFunction = val; }
		inline void SetRgbMode				( eRGB_BLEND_FUNC val)									{ m_eRgbFunction = val; }
		inline void SetInterpolationMode	( eINTERPOLATION_FILTER uiDim, eINTERPOLATION_MODE val)	{ m_eInterpolationModes[uiDim] = val; }
		inline void SetWrapMode				( eUVW uiDim, eWRAP_MODE val)							{ m_eWrapModes[uiDim] = val; }
		inline void SetIndex				( unsigned int p_index)									{ m_uiIndex = p_index; }
		inline void SetPrimaryColour		( Colour const & p_crColour)							{ m_clrPrimaryColour = p_crColour; }
		inline void SetPrimaryColour		( float red, float green, float blue, float alpha)		{ m_clrPrimaryColour[0] = red;		m_clrPrimaryColour[1] = green;		m_clrPrimaryColour[2] = blue;		m_clrPrimaryColour[3] = alpha; }
		inline void SetPrimaryColour		( float * rvba)											{ m_clrPrimaryColour[0] = rvba[0];	m_clrPrimaryColour[1] = rvba[1];	m_clrPrimaryColour[2] = rvba[2];	m_clrPrimaryColour[3] = rvba[3]; }
		inline void SetPrimaryColour		( float * rvb, float alpha)								{ m_clrPrimaryColour[0] = rvb[0];	m_clrPrimaryColour[1] = rvb[1];	m_clrPrimaryColour[2] = rvb[2];	m_clrPrimaryColour[3] = alpha; }
		inline void SetTextureMapMode		( eMAP_MODE p_mode)										{ m_eMode = p_mode; }
		inline void SetAlphaValue			( float val)											{ m_fAlphaValue = val; }
		inline void SetAlphaFunc			( eALPHA_FUNC val)										{ m_eAlphaFunc = val; }
		inline void SetDimension			( eTEXTURE_TYPE eDim)									{ m_eDimension = eDim; }
		//@}

	private:
		void _initialiseMaps();
	};
	//! The TextureUnit renderer
	/*!
	Initialises a texture unit, draws it, removes it
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class C3D_API TextureRenderer : public Renderer<TextureUnit, TextureRenderer>
	{
	protected:
		/**
		 * Constructor, only RenderSystem can use it
		 */
		TextureRenderer()
			:	Renderer<TextureUnit, TextureRenderer>()
		{}

	public:
		/**
		 * Destructor
		 */
		virtual ~TextureRenderer(){ Cleanup(); }
		/**
		 * Cleans up the renderer
		 */
		virtual void Cleanup(){}
		/**
		 * Initialises the texture, generates mipmapping...
		 */
		virtual bool Initialise() = 0;
		/**
		 * Draws the texture
		 */
		virtual void Render() = 0;
		/**
		 * Removes the texture
		 */
		virtual void EndRender() = 0;
	};
}

#endif

