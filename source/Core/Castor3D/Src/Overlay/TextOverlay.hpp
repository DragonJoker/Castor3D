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
#ifndef ___C3D_TEXT_OVERLAY_H___
#define ___C3D_TEXT_OVERLAY_H___

#include "OverlayCategory.hpp"
#include "FontTexture.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		25/08/2010
	\~english
	\brief		An overlay with a text
	\~french
	\brief		Une incrustation avec du texte
	*/
	class TextOverlay
		: public OverlayCategory
	{
	public:
		/*!
		\author 	Sylvain DOREMUS
		\date 		26/03/2016
		\version	0.8.0
		\~english
		\brief		Holds specific vertex data for a TextOverlay.
		\~french
		\brief		Contient les données spécifiques de sommet pour un TextOverlay.
		*/
		struct Vertex
		{
			int32_t coords[2];
			float text[2];
			float texture[2];
		};
		DECLARE_VECTOR( Vertex, Vertex );
		/*!
		\author 	Sylvain DOREMUS
		\date 		14/02/2010
		\~english
		\brief		TextOverlay loader
		\remark		Loads and saves overlays from/into a file
		\~french
		\brief		TextOverlay loader
		\remark		Charge et enregistre les incrustations dans des fichiers
		*/
		class TextWriter
			: public OverlayCategory::TextWriter
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			C3D_API TextWriter( Castor::String const & p_tabs, TextOverlay const * p_category = nullptr );
			/**
			 *\~english
			 *\brief		Saves an overlay into a text file
			 *\param[in]	p_file		the file to save the overlay in
			 *\param[in]	p_overlay	the overlay to save
			 *\return		\p true if everything is OK
			 *\~french
			 *\brief		Sauvegarde l'incrustation donnée dans un fichier texte
			 *\param[in]	p_file		Le fichier où enregistrer l'incrustation
			 *\param[in]	p_overlay	L'incrustation à enregistrer
			 *\return		\p true si tout s'est bien passé
			 */
			C3D_API bool operator()( TextOverlay const & p_overlay, Castor::TextFile & p_file );
			/**
			 *\copydoc		Castor::OverlayCategory::WriteInto
			 */
			C3D_API bool WriteInto( Castor::TextFile & p_file )override;

		private:
			TextOverlay const * m_category;
		};

	public:
		DECLARE_MAP( char32_t, Castor::Position, GlyphPosition );

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		C3D_API TextOverlay();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~TextOverlay();
		/**
		 *\~english
		 *\brief		Creation function, used by the factory
		 *\return		An overlay
		 *\~french
		 *\brief		Fonction de création utilisée par la fabrique
		 *\return		Un overlay
		 */
		C3D_API static OverlayCategorySPtr Create();
		/**
		 *\copydoc		Castor::OverlayCategory::CreateTextLoader
		 */
		C3D_API std::unique_ptr < OverlayCategory::TextWriter > CreateTextWriter( Castor::String const & p_tabs )override
		{
			return std::make_unique< TextWriter >( p_tabs, this );
		}
		/**
		 *\~english
		 *\brief		Sets the text font
		 *\param[in]	p_value	The new value
		 *\~french
		 *\brief		Définit la police du texte
		 *\param[in]	p_value	La nouvelle valeur
		 */
		C3D_API void SetFont( Castor::String const & p_value );
		/**
		 *\~english
		 *\brief		Retrieves the font name.
		 *\return		The value.
		 *\~french
		 *\brief		Récupère le nom de la police.
		 *\return		La valeur.
		 */
		C3D_API Castor::String const & GetFontName()const
		{
			return GetFontTexture()->GetFontName();
		}
		/**
		 *\~english
		 *\return		\p true if this overlay's has changed.
		 *\~french
		 *\return		\p true si cette incrustation a changé.
		 */
		C3D_API virtual bool IsChanged()const
		{
			return m_textChanged;
		}
		/**
		 *\~english
		 *\return		The FontTexture.
		 *\~french
		 *\return		La FontTexture.
		 */
		inline FontTextureSPtr GetFontTexture()const
		{
			return m_fontTexture.lock();
		}
		/**
		 *\~english
		 *\brief		Retrieves the panel vertex buffer
		 *\return		The buffer
		 *\~french
		 *\brief		Récupère le tampon de sommets du panneau
		 *\return		Le tampon
		 */
		inline VertexArray const & GetTextVertex()const
		{
			return m_arrayVtx;
		}
		/**
		 *\~english
		 *\brief		Retrieves the overlay text
		 *\return		The value
		 *\~french
		 *\brief		Récupère le texte de l'incrustation
		 *\return		La valeur
		 */
		inline Castor::String GetCaption()const
		{
			return m_currentCaption;
		}
		/**
		 *\~english
		 *\brief		Sets the overlay text
		 *\param[in]	p_value	The new value
		 *\~french
		 *\brief		Définit le texte de l'incrustation
		 *\param[in]	p_value	La nouvelle valeur
		 */
		inline void SetCaption( Castor::String const & p_value )
		{
			m_currentCaption = p_value;
			m_textChanged = true;
		}
		/**
		 *\~english
		 *\brief		Sets the overlay text
		 *\param[in]	p_value	The new value
		 *\~french
		 *\brief		Définit le texte de l'incrustation
		 *\param[in]	p_value	La nouvelle valeur
		 */
		inline void SetCaption( Castor::OutputStream const & p_value )
		{
			Castor::StringStream l_ss;
			l_ss << p_value.rdbuf();
			SetCaption( l_ss.str() );
		}
		/**
		 *\~english
		 *\return		The text wrapping mode.
		 *\~french
		 *\return		Le mode de découpe du texte.
		*/
		inline TextWrappingMode GetTextWrappingMode()const
		{
			return m_wrappingMode;
		}
		/**
		 *\~english
		 *\brief		Sets text wrapping mode
		 *\param[in]	p_value	The new value
		 *\~french
		 *\brief		Définit le mode de découpe du texte
		 *\param[in]	p_value	La nouvelle valeur
		 */
		inline void SetTextWrappingMode( TextWrappingMode p_value )
		{
			m_textChanged |= m_wrappingMode != p_value;
			m_wrappingMode = p_value;
		}
		/**
		 *\~english
		 *\return		The horizontal alignment.
		 *\~french
		 *\return		L'alignement horizontal.
		*/
		inline HAlign GetHAlign()const
		{
			return m_hAlign;
		}
		/**
		 *\~english
		 *\brief		Defines the horizontal alignment
		 *\param[in]	p_value	The new value
		 *\~french
		 *\brief		Définit l'alignement horizontal
		 *\param[in]	p_value	La nouvelle valeur
		*/
		inline void SetHAlign( HAlign p_value )
		{
			m_textChanged |= m_hAlign != p_value;
			m_hAlign = p_value;
		}
		/**
		 *\~english
		 *\return		The vertical alignment.
		 *\~french
		 *\return		L'alignement vertical.
		*/
		inline VAlign GetVAlign()const
		{
			return m_vAlign;
		}
		/**
		 *\~english
		 *\brief		Defines the vertical alignment
		 *\param[in]	p_value	The new value
		 *\~french
		 *\brief		Définit l'alignement vertical
		 *\param[in]	p_value	La nouvelle valeur
		*/
		inline void SetVAlign( VAlign p_value )
		{
			m_textChanged |= m_vAlign != p_value;
			m_vAlign = p_value;
		}
		/**
		 *\~english
		 *\return		The text texture mapping mode.
		 *\~french
		 *\return		Le mode de mappage de texture du texte.
		*/
		inline TextTexturingMode GetTexturingMode()const
		{
			return m_texturingMode;
		}
		/**
		 *\~english
		 *\brief		Defines the text texture mapping mode.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit le mode de mappage de texture du texte.
		 *\param[in]	p_value	La nouvelle valeur.
		*/
		inline void SetTexturingMode( TextTexturingMode p_value )
		{
			m_textChanged |= m_texturingMode != p_value;
			m_texturingMode = p_value;
		}
		/**
		 *\~english
		 *\return		The lines spacing mode.
		 *\~french
		 *\return		Le mode d'espacement des lignes.
		*/
		inline TextLineSpacingMode GetLineSpacingMode()const
		{
			return m_lineSpacingMode;
		}
		/**
		 *\~english
		 *\brief		Defines the lines spacing mode.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit le mode d'espacement des lignes.
		 *\param[in]	p_value	La nouvelle valeur.
		*/
		inline void SetLineSpacingMode( TextLineSpacingMode p_value )
		{
			m_textChanged |= m_lineSpacingMode != p_value;
			m_lineSpacingMode = p_value;
		}

	private:
		/*!
		\author 	Sylvain DOREMUS
		\date 		28/01/2016
		\~english
		\brief		A character, along with its size and relative position.
		\~french
		\brief		Un caractère, avec ses dimensions et sa position relative.
		*/
		struct DisplayableChar
		{
			//!\~english The character position, relative to its line.	\~french La position du caractère, relative à sa ligne.
			Castor::Point2d m_position;
			//!\~english The character dimensions.	\~french Les dimensions du caractère.
			Castor::Point2d m_size;
			//!\~english The character to display	\~french Le caractère à afficher.
			Castor::Glyph const & m_glyph;

			DisplayableChar( Castor::Point2d const & p_position, Castor::Point2d const & p_size, Castor::Glyph const & p_glyph )
				: m_position{ p_position }
				, m_size{ p_size }
				, m_glyph{ p_glyph }
			{
			}

			DisplayableChar( DisplayableChar const & p_rhs )
				: m_position{ p_rhs.m_position }
				, m_size{ p_rhs.m_size }
				, m_glyph{ p_rhs.m_glyph }
			{
			}

			DisplayableChar( DisplayableChar && p_rhs )
				: m_position{ std::move( p_rhs.m_position ) }
				, m_size{ std::move( p_rhs.m_size ) }
				, m_glyph{ p_rhs.m_glyph }
			{
			}

			DisplayableChar & operator=( DisplayableChar const & p_rhs )
			{
				m_position = p_rhs.m_position;
				m_size = p_rhs.m_size;
				return *this;
			}

			DisplayableChar & operator=( DisplayableChar && p_rhs )
			{
				if ( &p_rhs != this )
				{
					m_position = std::move( p_rhs.m_position );
					m_size = std::move( p_rhs.m_size );
				}

				return *this;
			}
		};
		/*!
		\author 	Sylvain DOREMUS
		\date 		28/01/2016
		\~english
		\brief		A text line, along with its size and position.
		\~french
		\brief		Une ligne de texte, avec ses dimensions et sa position.
		*/
		struct DisplayableLine
		{
			//!\~english The line position.	\~french La position de la ligne.
			Castor::Point2d m_position;
			//!\~english The line width.	\~french La largeur de la ligne.
			double m_width;
			//!\~english The line height.	\~french La hauteur de la ligne.
			double m_height;
			//!\~english The displayable characters.	\~french Les caractères affichables.
			std::vector< DisplayableChar > m_characters;
		};
		using DisplayableLineArray = std::vector< DisplayableLine >;
		using TextureCoordinates = std::array< float, 2 >;
		DECLARE_VECTOR( TextureCoordinates, TextureCoords );
		/**
		 *\~english
		 *\brief		Draws the overlay
		 *\param[in]	p_renderer	The renderer used to draw this overlay
		 *\~french
		 *\brief		Dessine l'incrustation
		 *\param[in]	p_renderer	Le renderer utilisé pour dessiner cette incrustation
		 */
		C3D_API virtual void DoRender( OverlayRendererSPtr p_renderer );
		/**
		 *\~english
		 *\brief		Updates the overlay position, size...
		 *\~french
		 *\brief		Met à jour la position, taille...
		 */
		C3D_API virtual void DoUpdate();
		/**
		 *\~english
		 *\brief		Updates the vertex buffer.
		 *\param[in]	p_size	The render target size.
		 *\~french
		 *\brief		Met à jour le tampon de sommets.
		 *\param[in]	p_size	Les dimensions de la cible de rendu.
		 */
		C3D_API virtual void DoUpdateBuffer( Castor::Size const & p_size );
		/**
		 *\~english
		 *\brief		Updates the vertex buffer.
		 *\param[in]	p_size			The render target size.
		 *\param[in]	p_generateUvs	The UV generator function.
		 *\~french
		 *\brief		Met à jour le tampon de sommets.
		 *\param[in]	p_size			Les dimensions de la cible de rendu.
		 *\param[in]	p_generateUvs	La fonction de génération d'UV.
		 */
		C3D_API virtual void DoUpdateBuffer( Castor::Size const & p_size, std::function< void( Castor::Point2d const & p_size, Castor::Rectangle const & p_absolute, Castor::Point4r const & p_fontUV,
																							   real & p_uvLeft, real & p_uvTop, real & p_uvRight, real & p_uvBottom ) > p_generateUvs );
		/**
		 *\~english
		 *\brief		Computes the lines to display.
		 *\remarks		Takes care of vertical alignment to retrieve the right vertical offset.
		 *\param[in]	p_renderSize	The render size.
		 *\param[in]	p_size			The overlay dimensions.
		 *\return		The lines.
		 *\~french
		 *\brief		Calcule les lignes à afficher.
		 *\remarks		Prend en compte l'alignement vertical, pour calculer le décalage vertical.
		 *\param[in]	p_renderSize	Les dimensions de la zone de rendu.
		 *\param[in]	p_size			The overlay dimensions.
		 *\return		Les lignes.
		 */
		C3D_API DisplayableLineArray DoPrepareText( Castor::Size const & p_renderSize, Castor::Point2d const & p_size );
		/**
		 *\~english
		 *\brief		Adds a word to the vertex buffer.
		 *\param[in]	p_renderSize	The render size.
		 *\param[in]	p_word			The word to add.
		 *\param[in]	p_wordWidth		The word width.
		 *\param[in]	p_size			The overlay size.
		 *\param[out]	p_left			The left position.
		 *\param[out]	p_line			The line.
		 *\param[out]	p_lines			The lines.
		 *\~french
		 *\brief		Ajoute un mot au tampon de sommets.
		 *\param[in]	p_renderSize	Les dimensions de la zone de rendu.
		 *\param[in]	p_word			Le mot à ajouter.
		 *\param[in]	p_wordWidth		La largeur du mot.
		 *\param[in]	p_size			La taille de l'incrustation.
		 *\param[out]	p_left			La position à gauche.
		 *\param[out]	p_line			La ligne.
		 *\param[out]	p_lines			Les lignes.
		 */
		C3D_API void DoPrepareWord( Castor::Size const & p_renderSize, std::u32string const & p_word, double p_wordWidth, Castor::Point2d const & p_size, double & p_left, DisplayableLine & p_line, DisplayableLineArray & p_lines );
		/**
		 *\~english
		 *\brief		Fills the line, and jumps to the next one.
		 *\param[in]	p_size	The overlay size.
		 *\param[in]	p_line	The line.
		 *\param[out]	p_left	The left position.
		 *\param[out]	p_lines	The lines.
		 *\~french
		 *\brief		Finit la ligne et passe à la ligne suivante.
		 *\param[in]	p_size	La taille de l'incrustation.
		 *\param[in]	p_line	La ligne.
		 *\param[out]	p_left	La position à gauche.
		 *\param[out]	p_lines	Les lignes.
		 */
		C3D_API DisplayableLine DoFinishLine( Castor::Point2d const & p_size, DisplayableLine p_line, double & p_left, DisplayableLineArray & p_lines );
		/**
		 *\~english
		 *\brief		Horizontally align a line.
		 *\param[in]	p_width	The overlay width.
		 *\param[in]	p_line	The line.
		 *\param[out]	p_lines	The lines.
		 *\~french
		 *\brief		Aligne horizontalement une ligne.
		 *\param[in]	p_width	La largeur de l'incrustation.
		 *\param[in]	p_line	La ligne.
		 *\param[out]	p_lines	Les lignes.
		 */
		C3D_API void DoAlignHorizontally( double p_width, DisplayableLine p_line, DisplayableLineArray & p_lines );
		/**
		 *\~english
		 *\brief		Vertically align text.
		 *\param[in]	p_height	The overlay height.
		 *\param[out]	p_lines		The lines.
		 *\~french
		 *\brief		Aligne verticalement un texte.
		 *\param[in]	p_height	La hauteur de l'incrustation.
		 *\param[out]	p_lines		Les lignes.
		 */
		C3D_API void DoAlignVertically( double p_height, DisplayableLineArray & p_lines );

	protected:
		//!\~english The vertex buffer data	\~french Les données du tampon de sommets
		VertexArray m_arrayVtx;
		//!\~english The current overlay caption	\~french Le texte courant de l'incrustation
		Castor::String m_currentCaption;
		//!\~english The previous overlay caption	\~french Le texte précédent de l'incrustation
		Castor::String m_previousCaption;
		//!\~english The texture associated to the overlay font.	\~french La texture associée à la police de l'incrustation.
		FontTextureWPtr m_fontTexture;
		//!\~english The wrapping mode	\~french Le mode de découpe du texte
		TextWrappingMode m_wrappingMode{ TextWrappingMode::eNone };
		//!\~english The lines spacing mode.	\~french Le mode d'espacement des lignes.
		TextLineSpacingMode m_lineSpacingMode{ TextLineSpacingMode::eOwnHeight };
		//!\~english The horizontal alignment.	\~french L'alignement horizontal du texte.
		HAlign m_hAlign{ HAlign::eLeft };
		//!\~english The vertical alignment.	\~french L'alignement vertical du texte.
		VAlign m_vAlign{ VAlign::eCenter };
		//!\~english Tells if the text (caption, wrap mode, or alignments) has changed.	\~french Dit si le texte (contenu, mode de découpe, alignements) a changé.
		bool m_textChanged{ true };
		//!\~english The size (in spaces) of tabulation character.	\~french La taille (en espaces) du caractère de tabulation.
		uint32_t m_tabSize{ 4 };
		//!\~english The connection to the FontTexture changed notification signal.	\~french La connexion au signal de notification de changement de la texture.
		uint32_t m_connection{ 0 };
		//!\~english The text texture mapping mode.	\~french Le mode de mappage de texture du texte.
		TextTexturingMode m_texturingMode{ TextTexturingMode::eText };
		//!\~english The text texture coordinates buffer data.	\~french Les données du tampon de coordonnées de texture du texte.
		TextureCoordsArray m_arrayTextTexture;
	};
}

#endif
