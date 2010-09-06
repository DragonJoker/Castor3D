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
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_TextureUnit___
#define ___C3D_TextureUnit___

#include <CastorUtils/Image.h>

namespace Castor3D
{
	//! Texture unit representation
	/*!
	A texture unit is a simple texture which can have an environment mode.
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class CS3D_API TextureUnit
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
		General::Resource::Image * m_image;		//!< The image resource
		TextureEnvironment * m_environment;		//!< The environment mode
		eDIMENSION m_textureType;				//!< The texture dimension
		bool m_textureInitialised;				//!< Tells whether or not this texture is initialised
		bool m_textured;						//!< Tells whether this is textured (the image path has been given) or not
		Colour m_primaryColour;				//!< The primary colour used in environment modes
		eMAP_MODE m_mode;						//!< The mapping mode for this texture

		TextureRenderer * m_renderer;			//!< The texture renderer

	public:
		/**
		 * Constructor
		 *@param p_renderer : [in] The texture renderer, may be OpenGL or Direct3D
		 */
		TextureUnit( TextureRenderer * p_renderer);
		/**
		 * Constructor
		 */
		~TextureUnit();
		/**
		 * Initialises this texture, id est : fills the image buffer, creates the texture in the render system
		 */
		void Initialise();
		/**
		 * Sets the image path
		 *@param p_name : [in] The image path
		 */
		void SetTexture2D( const String & p_name);
		/**
		 * Applies this texture unit and it's environment mode
		 */
		void Apply();
		/**
		 * Removes this texture unit from the stack, in order not to interfere with other ones
		 */
		void Remove();
		/**
		 * Writes this texture unit in a file
		 *@param p_pFile : [in] The file to write in
		 *@return true if successful, false if not
		 */
		bool Write( General::Utils::FileIO * p_pFile)const;
		/**
		 * Reads this texture unit from a file
		 *@param p_file : [in] The file to read from
		 *@return true if successful, false if not
		 */
		bool Read( General::Utils::FileIO & p_file);
		const unsigned char * GetImagePixels()const;
		unsigned int GetWidth()const;
		unsigned int GetHeight()const;
		String GetTexturePath()const;

	public:
		inline unsigned int				GetIndex			()const { return m_index; }
		inline eDIMENSION				GetTextureType		()const { return m_textureType; }
		inline TextureEnvironment *		GetEnvironment		()const { return m_environment; }
		inline const float *			GetPrimaryColour	()const { return m_primaryColour.const_ptr(); }
		inline bool						IsTextured			()const { return m_textured; }
		inline bool						TextureInitialised	()const { return m_textureInitialised; }
		inline eMAP_MODE				GetTextureMapMode	()const { return m_mode; }

		inline void SetIndex			( unsigned int p_index)								{ m_index = p_index; }
		inline void SetPrimaryColour	( const Colour & p_crColour)						{ m_primaryColour = p_crColour; }
		inline void SetPrimaryColour	( float red, float green, float blue, float alpha)	{ m_primaryColour.x = red;m_primaryColour.y = green;m_primaryColour.z = blue;m_primaryColour.w = alpha; }
		inline void SetPrimaryColour	( float * rvba)										{ m_primaryColour.x = rvba[0];m_primaryColour.y = rvba[1];m_primaryColour.z = rvba[2];m_primaryColour.w = rvba[3]; }
		inline void SetPrimaryColour	( float * rvb, float alpha)							{ m_primaryColour.x = rvb[0];m_primaryColour.y = rvb[1];m_primaryColour.z = rvb[2];m_primaryColour.w = alpha; }
		inline void SetTextureMapMode	( eMAP_MODE p_mode)									{ m_mode = p_mode; }
	};
}

#endif

