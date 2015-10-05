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
#ifndef ___C3D_FONT_TEXTURE_H___
#define ___C3D_FONT_TEXTURE_H___

#include "Castor3DPrerequisites.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.8.0
	\date 		04/10/2015
	\~english
	\brief		Contains the font and the texture assiated to this font.
	\~french
	\brief		Contient la polica et la texture associée.
	*/
	class C3D_API FontTexture
	{
	public:
		DECLARE_MAP( char32_t, Castor::Position, GlyphPosition );

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_font	The font.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_font	La police.
		 */
		FontTexture( Engine * p_engine, Castor::FontSPtr p_font );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~FontTexture();
		/**
		 *\~english
		 *\brief		Initialises the texture.
		 *\~french
		 *\brief		Initialise la texture.
		 */
		virtual void Initialise();
		/**
		 *\~english
		 *\brief		Flushes the teture.
		 *\~french
		 *\brief		Nettoie la texture.
		 */
		virtual void Cleanup();
		/**
		 *\~english
		 *\brief		Retrieves the font name.
		 *\return		The value.
		 *\~french
		 *\brief		Récupère le nom de la police.
		 *\return		La valeur.
		 */
		Castor::String const & GetFontName()const;
		/**
		 *\~english
		 *\brief		Retrieves the wanted glyph position.
		 *\param[in]	p_char	The glyph index.
		 *\return		The position.
		 *\~french
		 *\brief		Récupère la position de la glyphe voulue.
		 *\param[in]	p_char	L'indice de la glyphe.
		 *\return		La position.
		 */
		Castor::Position const & GetGlyphPosition( char32_t p_char )const;
		/**
		 *\~english
		 *\brief		Retrieves the font.
		 *\return		The value.
		 *\~french
		 *\brief		Récupère la police.
		 *\return		La valeur.
		 */
		Castor::FontSPtr GetFont()const
		{
			return m_font.lock();
		}
		/**
		 *\~english
		 *\brief		Retrieves the texture.
		 *\return		The texture.
		 *\~french
		 *\brief		Récupère la texture.
		 *\return		La texture.
		 */
		inline TextureBaseSPtr GetTexture()const
		{
			return m_texture;
		}

	private:
		//!\~english The engine.	\~french Le moteur.
		Engine * m_engine;
		//!\~english The font.	\~french La police.
		Castor::FontWPtr m_font;
		//!\~english The texture sampler.	\~french L'échantillonneur de la texture.
		SamplerWPtr m_sampler;
		//!\~english The texture that will receive the glyphs.	\~french La texture qui recevra les glyphes.
		TextureBaseSPtr m_texture;
		//!\~english Glyphs positions in the texture.	\~french Position des glyphes.
		GlyphPositionMap m_glyphsPositions;
	};
}

#endif
