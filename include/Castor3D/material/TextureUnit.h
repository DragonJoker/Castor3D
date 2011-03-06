/*
This source file is part of Castor3D (http://dragonjoker.co.cc

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

#include "../Prerequisites.h"
#include "../render_system/Renderable.h"

namespace Castor3D
{
	//! Texture unit representation
	/*!
	A texture unit is a simple texture which can have an environment mode.
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class C3D_API TextureUnit : public Textable, public Serialisable, public Renderable<TextureUnit, TextureRenderer>
	{
	public:
		typedef enum
		{
			eNone,
			eReflexionMap,
			eSphereMap,
			eNbMapModes,
		}
		eMAP_MODE;

		//! Texture dimensions enumerator
		/*!
		The different texture dimensions are 1 dimension, 2 dimensions and 3 dimensions
		*/
		typedef enum
		{
			eTex1D	= 0,	//!< 1 dimension textures => one coordinate => U mapping
			eTex2D	= 1,	//!< 2 dimensions textures => two coordinates => UV mapping
			eTex3D	= 2,	//!< 3 dimensions textures => three coordinates => UVW mapping
			eNbTextureDimensions,
		}
		eDIMENSION;

		typedef enum
		{
			eAlphaAlways,
			eAlphaLess,
			eAlphaLessOrEqual,
			eAlphaEqual,
			eAlphaNotEqual,
			eAlphaGreaterOrEqual,
			eAlphaGreater,
			eAlphaNever,
			eNbAlphaFuncs,
		}
		eALPHA_FUNC;

		typedef enum
		{
			eWrapRepeat,
			eWrapClamp,
			eWrapClampToBorder,
			eWrapClampToEdge,
			eNbWrapModes,
		}
		eWRAP_MODE;

		typedef enum
		{
			eNearest,
			eLinear,
			eAnisotropic,
			eNbInterpolationModes,
		}
		eINTERPOLATION_MODE;

		typedef enum
		{
			eArgumentDiffuse,
			eArgumentTexture,
			eArgumentPrevious,
			eArgumentConstant,
			eNbTextureArguments,
		}
		eTEXTURE_ARGUMENT;

		typedef enum
		{
			eRgbModeNone,
			eRgbModeFirstArg,
			eRgbModeAdd,
			eRgbModeModulate,
			eNbTextureRgbModes
		}
		eTEXTURE_RGB_MODE;

		typedef enum
		{
			eAlphaModeNone,
			eAlphaModeFirstArg,
			eAlphaModeAdd,
			eAlphaModeModulate,
			eNbTextureAlphaModes
		}
		eTEXTURE_ALPHA_MODE;

		static std::map<String, eALPHA_FUNC> MapAlphaFuncs;
		static const String StringAlphaFuncs[eNbAlphaFuncs];

		static std::map<String, eTEXTURE_ARGUMENT> MapTextureArguments;
		static const String StringTextureArguments[eNbTextureArguments];

		static std::map<String, eTEXTURE_RGB_MODE> MapTextureRgbModes;
		static const String StringTextureRgbModes[eNbTextureRgbModes];

		static std::map<String, eTEXTURE_ALPHA_MODE> MapTextureAlphaModes;
		static const String StringTextureAlphaModes[eNbTextureAlphaModes];

	protected:
		friend class TextureRenderer;						//!< The texture renderer class can access protected and private functions
		unsigned int				m_uiIndex;				//!< This texture index in parent pass
		Castor::Resources::ImagePtr	m_pImage;				//!< The image resource
		TextureEnvironmentPtr		m_environment;			//!< The environment mode
		eDIMENSION					m_eTextureType;			//!< The texture dimension
		bool						m_bTextureInitialised;	//!< Tells whether or not the texture is initialised
		bool						m_bTextured;			//!< Tells whether the texture is loaded (the image path has been given) or not
		Colour						m_clrPrimaryColour;		//!< The primary colour used in environment modes
		eMAP_MODE					m_eMode;				//!< The mapping mode for the texture
		float						m_fAlphaValue;			//!< The max rejected alpha
		eALPHA_FUNC					m_eAlphaFunc;
		eWRAP_MODE					m_eWrapModes[3];
		eINTERPOLATION_MODE			m_eInterpolationModes[2];
		eTEXTURE_ARGUMENT			m_eRgbArguments[2];
		eTEXTURE_ARGUMENT			m_eAlpArguments[2];
		eTEXTURE_RGB_MODE			m_eRgbMode;
		eTEXTURE_ALPHA_MODE			m_eAlpMode;

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
		 * Sets the image path
		 *@param p_name : [in] The image path
		 */
		void SetTexture2D( ImagePtr p_pImage);
		/**
		 * Applies the texture unit and it's environment mode
		 */
		virtual void Render( ePRIMITIVE_TYPE p_displayMode);
		/**
		 * Removes the texture unit from the stack, in order not to interfere with other ones
		 */
		virtual void EndRender();
		const unsigned char * GetImagePixels()const;
		unsigned char * GetImagePixels();
		unsigned int GetWidth()const;
		unsigned int GetHeight()const;
		String GetTexturePath()const;
		ePIXEL_FORMAT GetPixelFormat();

		/**@name Inherited methods from Textable */
		//@{
		virtual bool Write( File & p_file)const;
		virtual bool Read( File & p_file) { return false; }
		//@}

		/**@name Inherited methods from Serialisable */
		//@{
		virtual bool Save( File & p_file)const;
		virtual bool Load( File & p_file);
		//@}

		/**@name Accessors */
		//@{
		inline unsigned int					GetIndex				()const 					{ return m_uiIndex; }
		inline eDIMENSION					GetTextureType			()const 					{ return m_eTextureType; }
		inline TextureEnvironmentPtr		GetEnvironment			()const						{ return m_environment; }
		inline const Colour &				GetPrimaryColour		()const 					{ return m_clrPrimaryColour; }
		inline bool							IsTextured				()const 					{ return m_bTextured; }
		inline bool							TextureInitialised		()const 					{ return m_bTextureInitialised; }
		inline eMAP_MODE					GetTextureMapMode		()const 					{ return m_eMode; }
		inline eALPHA_FUNC					GetAlphaFunc			()const 					{ return m_eAlphaFunc; }
		inline float						GetAlphaValue			()const 					{ return m_fAlphaValue; }
		inline eWRAP_MODE					GetWrapMode				( size_t uiDim)const		{ return m_eWrapModes[uiDim]; }
		inline eINTERPOLATION_MODE			GetInterpolationMode	( size_t uiDim)const		{ return m_eInterpolationModes[uiDim]; }
		inline eTEXTURE_RGB_MODE			GetRgbMode				()const 					{ return m_eRgbMode; }
		inline eTEXTURE_ALPHA_MODE			GetAlpMode				()const 					{ return m_eAlpMode; }
		inline eTEXTURE_ARGUMENT			GetAlpArgument			( size_t p_uiIndex)const	{ return m_eAlpArguments[p_uiIndex]; }
		inline eTEXTURE_ARGUMENT			GetRgbArgument			( size_t p_uiIndex)const	{ return m_eRgbArguments[p_uiIndex]; }

		inline void SetRgbArgument			( eTEXTURE_ARGUMENT val, size_t p_uiIndex)			{ m_eRgbArguments[p_uiIndex] = val; }
		inline void SetAlpArgument			( eTEXTURE_ARGUMENT val, size_t p_uiIndex)			{ m_eAlpArguments[p_uiIndex] = val; }
		inline void SetAlpMode				( eTEXTURE_ALPHA_MODE val)							{ m_eAlpMode = val; }
		inline void SetRgbMode				( eTEXTURE_RGB_MODE val)							{ m_eRgbMode = val; }
		inline void SetInterpolationMode	( size_t uiDim, eINTERPOLATION_MODE val)			{ m_eInterpolationModes[uiDim] = val; }
		inline void SetWrapMode				( size_t uiDim, eWRAP_MODE val)						{ m_eWrapModes[uiDim] = val; }
		inline void SetIndex				( unsigned int p_index)								{ m_uiIndex = p_index; }
		inline void SetPrimaryColour		( const Colour & p_crColour)						{ m_clrPrimaryColour = p_crColour; }
		inline void SetPrimaryColour		( float red, float green, float blue, float alpha)	{ m_clrPrimaryColour[0] = red;		m_clrPrimaryColour[1] = green;		m_clrPrimaryColour[2] = blue;		m_clrPrimaryColour[3] = alpha; }
		inline void SetPrimaryColour		( float * rvba)										{ m_clrPrimaryColour[0] = rvba[0];	m_clrPrimaryColour[1] = rvba[1];	m_clrPrimaryColour[2] = rvba[2];	m_clrPrimaryColour[3] = rvba[3]; }
		inline void SetPrimaryColour		( float * rvb, float alpha)							{ m_clrPrimaryColour[0] = rvb[0];	m_clrPrimaryColour[1] = rvb[1];	m_clrPrimaryColour[2] = rvb[2];	m_clrPrimaryColour[3] = alpha; }
		inline void SetTextureMapMode		( eMAP_MODE p_mode)									{ m_eMode = p_mode; }
		inline void SetAlphaValue			( float val)										{ m_fAlphaValue = val; }
		inline void SetAlphaFunc			( eALPHA_FUNC val)									{ m_eAlphaFunc = val; }
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
		TextureRenderer( SceneManager * p_pManager)
			:	Renderer<TextureUnit, TextureRenderer>( p_pManager)
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

