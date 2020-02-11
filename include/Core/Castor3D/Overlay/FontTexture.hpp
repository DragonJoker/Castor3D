/*
See LICENSE file in root folder
*/
#ifndef ___C3D_FONT_TEXTURE_H___
#define ___C3D_FONT_TEXTURE_H___

#include "OverlayModule.hpp"

#include <CastorUtils/Design/OwnedBy.hpp>
#include <CastorUtils/Design/Signal.hpp>

namespace castor3d
{
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
		 *\param[in]	font	The font.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 *\param[in]	font	La police.
		 */
		C3D_API FontTexture( Engine & engine
			, castor::FontSPtr font );
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
		 *\param[in]	glyphChar	The glyph index.
		 *\return		The position.
		 *\~french
		 *\brief		Récupère la position de la glyphe voulue.
		 *\param[in]	glyphChar	L'indice de la glyphe.
		 *\return		La position.
		 */
		C3D_API castor::Position const & getGlyphPosition( char32_t glyphChar )const;
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
		castor::FontWPtr m_font;
		SamplerWPtr m_sampler;
		TextureLayoutSPtr m_texture;
		GlyphPositionMap m_glyphsPositions;
	};
}

#endif
