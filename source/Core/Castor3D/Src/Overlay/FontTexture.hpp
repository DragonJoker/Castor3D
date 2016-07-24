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
#ifndef ___C3D_FONT_TEXTURE_H___
#define ___C3D_FONT_TEXTURE_H___

#include "Castor3DPrerequisites.hpp"

#include <Design/OwnedBy.hpp>
#include <Design/Signal.hpp>

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
	class FontTexture
		: public Castor::OwnedBy< Engine >
	{
	public:
		DECLARE_MAP( char32_t, Castor::Position, GlyphPosition );

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_engine	The engine.
		 *\param[in]	p_font		The font.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_engine	Le moteur.
		 *\param[in]	p_font		La police.
		 */
		C3D_API FontTexture( Engine & p_engine, Castor::FontSPtr p_font );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~FontTexture();
		/**
		 *\~english
		 *\brief		Initialises the texture.
		 *\~french
		 *\brief		Initialise la texture.
		 */
		C3D_API void Initialise();
		/**
		 *\~english
		 *\brief		Flushes the teture.
		 *\~french
		 *\brief		Nettoie la texture.
		 */
		C3D_API void Cleanup();
		/**
		 *\~english
		 *\brief		Updates the glyphs.
		 *\~french
		 *\brief		Met à jour les glyphes.
		 */
		C3D_API void Update();
		/**
		 *\~english
		 *\brief		Retrieves the font name.
		 *\return		The value.
		 *\~french
		 *\brief		Récupère le nom de la police.
		 *\return		La valeur.
		 */
		C3D_API Castor::String const & GetFontName()const;
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
		C3D_API Castor::Position const & GetGlyphPosition( char32_t p_char )const;
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
		inline TextureLayoutSPtr GetTexture()const
		{
			return m_texture;
		}
		/**
		 *\~english
		 *\brief		Retrieves the texture.
		 *\return		The texture.
		 *\~french
		 *\brief		Récupère la texture.
		 *\return		La texture.
		 */
		inline SamplerSPtr GetSampler()const
		{
			return m_sampler.lock();
		}
		/**
		 *\~english
		 *\brief		Connects a client to the changed notification signal.
		 *\param[in]	p_function	The client function.
		 *\return		The connection.
		 *\~french
		 *\brief		Connecte un client au signal de notification de changement.
		 *\param[in]	p_function	La fonction du client.
		 *\return		La connexion.
		 */
		inline uint32_t Connect( std::function< void( FontTexture const & ) > p_function )
		{
			return m_signal.connect( p_function );
		}
		/**
		 *\~english
		 *\brief		Disconnects a client from the changed notification signal.
		 *\return		The connection.
		 *\~french
		 *\brief		Déconnecte un client du signal de notification de changement.
		 *\return		La connexion.
		 */
		inline void Disconnect( uint32_t p_connection )
		{
			m_signal.disconnect( p_connection );
		}

	private:
		//!\~english The font.	\~french La police.
		Castor::FontWPtr m_font;
		//!\~english The texture sampler.	\~french L'échantillonneur de la texture.
		SamplerWPtr m_sampler;
		//!\~english The texture that will receive the glyphs.	\~french La texture qui recevra les glyphes.
		TextureLayoutSPtr m_texture;
		//!\~english Glyphs positions in the texture.	\~french Position des glyphes.
		GlyphPositionMap m_glyphsPositions;
		//!\~english The signal used to notify clients that this texture has changed.	\~french Signal utilisé pour notifier les clients que cette texture a changé.
		Castor::Signal< std::function< void( FontTexture const & ) > > m_signal;
	};
}

#endif
