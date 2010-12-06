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

#include "Module_Material.h"
#include <CastorUtils/Image.h>
#include "../render_system/Module_Render.h"
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
	class C3D_API TextureUnit : public Renderable<TextureUnit, TextureRenderer>
	{
	public:
		typedef enum eMAP_MODE
		{
			eNone,
			eReflexionMap,
			eSphereMap

		} eMAP_MODE;
		
		//! Texture dimensions enumerator
		/*!
		The different texture dimensions are 1 dimension, 2 dimensions and 3 dimensions
		*/
		typedef enum eDIMENSION
		{
			eTex1D	= 0,	//!< 1 dimension textures => one coordinate => U mapping
			eTex2D	= 1,	//!< 2 dimensions textures => two coordinates => UV mapping
			eTex3D	= 2		//!< 3 dimensions textures => three coordinates => UVW mapping

		} eDIMENSION;

	protected:
		friend class TextureRenderer;			//!< The texture renderer class can access protected and private functions
		unsigned int m_index;					//!< This texture index in parent pass
		Castor::Resource::ImagePtr m_image;		//!< The image resource
		TextureEnvironmentPtr m_environment;	//!< The environment mode
		eDIMENSION m_textureType;				//!< The texture dimension
		bool m_textureInitialised;				//!< Tells whether or not the texture is initialised
		bool m_textured;						//!< Tells whether the texture is loaded (the image path has been given) or not
		Colour m_primaryColour;					//!< The primary colour used in environment modes
		eMAP_MODE m_mode;						//!< The mapping mode for the texture

	public:
		/**
		 * Constructor
		 */
		TextureUnit();
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
		void SetTexture2D( const String & p_name);
		/**
		 * Applies the texture unit and it's environment mode
		 */
		virtual void Render( eDRAW_TYPE p_displayMode);
		/**
		 * Removes the texture unit from the stack, in order not to interfere with other ones
		 */
		virtual void EndRender();
		/**
		 * Writes the texture unit in a file
		 *@param p_pFile : [in] The file to write in
		 *@return true if successful, false if not
		 */
		virtual bool Write( Castor::Utils::File & p_pFile)const;
		/**
		 * Reads the texture unit from a file
		 *@param p_file : [in] The file to read from
		 *@return true if successful, false if not
		 */
		virtual bool Read( Castor::Utils::File & p_file);
		const unsigned char * GetImagePixels()const;
		unsigned int GetWidth()const;
		unsigned int GetHeight()const;
		String GetTexturePath()const;

	public:
		inline unsigned int				GetIndex			()const { return m_index; }
		inline eDIMENSION				GetTextureType		()const { return m_textureType; }
		inline TextureEnvironmentPtr	GetEnvironment		()const { return m_environment; }
		inline const float *			GetPrimaryColour	()const { return m_primaryColour.const_ptr(); }
		inline bool						IsTextured			()const { return m_textured; }
		inline bool						TextureInitialised	()const { return m_textureInitialised; }
		inline eMAP_MODE				GetTextureMapMode	()const { return m_mode; }

		inline void SetIndex			( unsigned int p_index)								{ m_index = p_index; }
		inline void SetPrimaryColour	( const Colour & p_crColour)						{ m_primaryColour = p_crColour; }
		inline void SetPrimaryColour	( float red, float green, float blue, float alpha)	{ m_primaryColour[0] = red;		m_primaryColour[1] = green;		m_primaryColour[2] = blue;		m_primaryColour[3] = alpha; }
		inline void SetPrimaryColour	( float * rvba)										{ m_primaryColour[0] = rvba[0];	m_primaryColour[1] = rvba[1];	m_primaryColour[2] = rvba[2];	m_primaryColour[3] = rvba[3]; }
		inline void SetPrimaryColour	( float * rvb, float alpha)							{ m_primaryColour[0] = rvb[0];	m_primaryColour[1] = rvb[1];	m_primaryColour[2] = rvb[2];	m_primaryColour[3] = alpha; }
		inline void SetTextureMapMode	( eMAP_MODE p_mode)									{ m_mode = p_mode; }
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

