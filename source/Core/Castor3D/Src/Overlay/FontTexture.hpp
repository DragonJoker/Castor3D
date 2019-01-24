/*
See LICENSE file in root folder
*/
#ifndef ___C3D_FONT_TEXTURE_H___
#define ___C3D_FONT_TEXTURE_H___

#include "Castor3DPrerequisites.hpp"

#include <Design/OwnedBy.hpp>
#include <Design/Signal.hpp>

namespace castor3d
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
		: public castor::OwnedBy< Engine >
	{
	public:
		CU_DeclareMap( char32_t, castor::Position, GlyphPosition );
		using OnChangedFunction = std::function< void( FontTexture const & ) >;
		using OnChanged = castor::Signal< OnChangedFunction >;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\param[in]	p_font		The font.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 *\param[in]	p_font		La police.
		 */
		C3D_API FontTexture( Engine & engine, castor::FontSPtr p_font );
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
		C3D_API void initialise();
		/**
		 *\~english
		 *\brief		Flushes the teture.
		 *\~french
		 *\brief		Nettoie la texture.
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Updates the glyphs.
		 *\~french
		 *\brief		Met à jour les glyphes.
		 */
		C3D_API void update();
		/**
		 *\~english
		 *\brief		Retrieves the font name.
		 *\return		The value.
		 *\~french
		 *\brief		Récupère le nom de la police.
		 *\return		La valeur.
		 */
		C3D_API castor::String const & getFontName()const;
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
		C3D_API castor::Position const & getGlyphPosition( char32_t p_char )const;
		/**
		 *\~english
		 *\brief		Retrieves the font.
		 *\return		The value.
		 *\~french
		 *\brief		Récupère la police.
		 *\return		La valeur.
		 */
		castor::FontSPtr getFont()const
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
		inline TextureLayoutSPtr getTexture()const
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
		inline SamplerSPtr getSampler()const
		{
			return m_sampler.lock();
		}

	public:
		//!\~english	The signal used to notify clients that this texture has changed.
		//!\~french		Signal utilisé pour notifier les clients que cette texture a changé.
		OnChanged onChanged;

	private:
		//!\~english	The font.
		//!\~french		La police.
		castor::FontWPtr m_font;
		//!\~english	The texture sampler.
		//!\~french		L'échantillonneur de la texture.
		SamplerWPtr m_sampler;
		//!\~english	The texture that will receive the glyphs.
		//!\~french		La texture qui recevra les glyphes.
		TextureLayoutSPtr m_texture;
		//!\~english	Glyphs positions in the texture.
		//!\~french		Position des glyphes.
		GlyphPositionMap m_glyphsPositions;
	};
}

#endif
