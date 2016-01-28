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
		\date 		14/02/2010
		\~english
		\brief		TextOverlay loader
		\remark		Loads and saves overlays from/into a file
		\~french
		\brief		TextOverlay loader
		\remark		Charge et enregistre les incrustations dans des fichiers
		*/
		class TextLoader
			: public OverlayCategory::TextLoader
		{
		public:
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
			C3D_API virtual bool operator()( TextOverlay const & p_overlay, Castor::TextFile & p_file );
		};
		/*!
		\author		Sylvain DOREMUS
		\date		08/04/2014
		\~english
		\brief		TextOverlay loader
		\~english
		\brief		Loader de TextOverlay
		*/
		class BinaryParser
			: public OverlayCategory::BinaryParser
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\param[in]	p_path	The current folder path
			 *\~french
			 *\brief		Constructeur
			 *\param[in]	p_path	Le chemin d'accès au dossier courant
			 */
			C3D_API BinaryParser( Castor::Path const & p_path );
			/**
			 *\~english
			 *\brief		Function used to fill the chunk from specific data
			 *\param[in]	p_obj	The object to write
			 *\param[out]	p_chunk	The chunk to fill
			 *\return		\p false if any error occured
			 *\~french
			 *\brief		Fonction utilisée afin de remplir le chunk de données spécifiques
			 *\param[in]	p_obj	L'objet à écrire
			 *\param[out]	p_chunk	Le chunk à remplir
			 *\return		\p false si une erreur quelconque est arrivée
			 */
			C3D_API virtual bool Fill( TextOverlay const & p_obj, BinaryChunk & p_chunk )const;
			/**
			 *\~english
			 *\brief		Function used to retrieve specific data from the chunk
			 *\param[out]	p_obj	The object to read
			 *\param[in]	p_chunk	The chunk containing data
			 *\return		\p false if any error occured
			 *\~french
			 *\brief		Fonction utilisée afin de récupérer des données spécifiques à partir d'un chunk
			 *\param[out]	p_obj	L'objet à lire
			 *\param[in]	p_chunk	Le chunk contenant les données
			 *\return		\p false si une erreur quelconque est arrivée
			 */
			C3D_API virtual bool Parse( TextOverlay & p_obj, BinaryChunk & p_chunk )const;
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
		 *\~english
		 *\brief		Initialises the overlay
		 *\~french
		 *\brief		Initialise l'incrustation
		 */
		C3D_API virtual void Initialise();
		/**
		 *\~english
		 *\brief		Flushes the overlay
		 *\~french
		 *\brief		Nettoie l'incrustation
		 */
		C3D_API virtual void Cleanup();
		/**
		 *\~english
		 *\brief		Sets the text font
		 *\param[in]	p_strFont	The new value
		 *\~french
		 *\brief		Définit la police du texte
		 *\param[in]	p_strFont	La nouvelle valeur
		 */
		C3D_API void SetFont( Castor::String const & p_strFont );
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
		inline OverlayCategory::VertexArray const & GetTextVertex()const
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
		 *\param[in]	p_strCaption	The new value
		 *\~french
		 *\brief		Définit le texte de l'incrustation
		 *\param[in]	p_strCaption	La nouvelle valeur
		 */
		inline void SetCaption( Castor::String const & p_strCaption )
		{
			m_currentCaption = p_strCaption;
			m_textChanged = true;
		}
		/**
		 *\~english
		 *\brief		Sets text wrapping mode
		 *\param[in]	p_mode	The new value
		 *\~french
		 *\brief		Définit le mode de découpe du texte
		 *\param[in]	p_mode	La nouvelle valeur
		 */
		inline void SetTextWrappingMode( eTEXT_WRAPPING_MODE p_mode )
		{
			m_textChanged |= m_wrappingMode != p_mode;
			m_wrappingMode = p_mode;
		}
		/**
		 *\~english
		 *\brief		Retrieves the horizontal alignment
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'alignement horizontal
		 *\return		La valeur
		*/
		inline eHALIGN GetHAlign()const
		{
			return m_hAlign;
		}
		/**
		 *\~english
		 *\brief		Retrieves the vertical alignment
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'alignement vertical
		 *\return		La valeur
		*/
		inline eVALIGN GetVAlign()const
		{
			return m_vAlign;
		}
		/**
		 *\~english
		 *\brief		Defines the horizontal alignment
		 *\param[in]	p_align	The new value
		 *\~french
		 *\brief		Définit l'alignement horizontal
		 *\param[in]	p_align	La nouvelle valeur
		*/
		inline void SetHAlign( eHALIGN p_align )
		{
			m_textChanged |= m_hAlign != p_align;
			m_hAlign = p_align;
		}
		/**
		 *\~english
		 *\brief		Defines the vertical alignment
		 *\param[in]	p_align	The new value
		 *\~french
		 *\brief		Définit l'alignement vertical
		 *\param[in]	p_align	La nouvelle valeur
		*/
		inline void SetVAlign( eVALIGN p_align )
		{
			m_textChanged |= m_vAlign != p_align;
			m_vAlign = p_align;
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
			//!\~english The character to display	\~french Le caractère à afficher.
			Castor::Glyph * m_glyph;
			//!\~english The character position, relative to its line.	\~french La position du caractère, relative à sa ligne.
			double m_left;
			//!\~english The character dimensions.	\~french Les dimensions du caractère.
			Castor::Point2d m_size;
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
			//!\~english The displayable characters.	\~french Les caractères affichables.
			std::vector< DisplayableChar > m_characters;
			//!\~english The line position.	\~french La position de la ligne.
			Castor::Point2d m_position;
			//!\~english The line width.	\~french La largeur de la ligne.
			double m_width;
		};
		using DisplayableLineArray = std::vector< DisplayableLine >;
		/**
		 *\copydoc		Castor3D::OverlayCategory::DoRender.
		 */
		C3D_API virtual void DoRender( OverlayRendererSPtr p_renderer );
		/**
		 *\copydoc		Castor3D::OverlayCategory::DoUpdateBuffer.
		 */
		C3D_API virtual void DoUpdateBuffer( Castor::Size const & p_size );
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
		 *\param[out]	p_left	The left position.
		 *\param[out]	p_line	The line.
		 *\param[out]	p_lines	The lines.
		 *\~french
		 *\brief		Finit la ligne et passe à la ligne suivante.
		 *\param[in]	p_size	La taille de l'incrustation.
		 *\param[out]	p_left	La position à gauche.
		 *\param[out]	p_line	La ligne.
		 *\param[out]	p_lines	Les lignes.
		 */
		C3D_API void DoFinishLine( Castor::Point2d const & p_size, double & p_left, DisplayableLine & p_line, DisplayableLineArray & p_lines );
		/**
		 *\~english
		 *\brief		Horizontally align a line.
		 *\param[in]	p_width		The overlay width.
		 *\param[out]	p_lineVtx	The line.
		 *\param[out]	p_linesVtx	The lines.
		 *\~french
		 *\brief		Aligne horizontalement une ligne.
		 *\param[in]	p_width		La largeur de l'incrustation.
		 *\param[out]	p_lineVtx	La ligne.
		 *\param[out]	p_linesVtx	Les lignes.
		 */
		C3D_API void DoAlignHorizontally( double p_width, DisplayableLine & p_line, DisplayableLineArray & p_lines );
		/**
		 *\~english
		 *\brief		Vertically align text
		 *\param[in]	p_height		The overlay width
		 *\param[out]	p_linesHeight	The lines height
		 *\param[out]	p_lines			The lines
		 *\~french
		 *\brief		Aligne verticalement un texte.
		 *\param[in]	p_height		La hauteur de l'incrustation.
		 *\param[out]	p_linesHeight	La hauteur des lignes.
		 *\param[out]	p_lines			Les lignes.
		 */
		C3D_API void DoAlignVertically( double p_height, double p_linesHeight, DisplayableLineArray & p_lines );

	protected:
		//!\~english The current overlay caption	\~french Le texte courant de l'incrustation
		Castor::String m_currentCaption;
		//!\~english The previous overlay caption	\~french Le texte précédent de l'incrustation
		Castor::String m_previousCaption;
		//!\~english The texture associated to the overlay font.	\~french La texture associée à la police de l'incrustation.
		FontTextureWPtr m_fontTexture;
		//!\~english The wrapping mode	\~french Le mode de découpe du texte
		eTEXT_WRAPPING_MODE m_wrappingMode;
		//!\~english The horizontal alignment.	\~french L'alignement horizontal du texte.
		eHALIGN m_hAlign;
		//!\~english The vertical alignment.	\~french L'alignement vertical du texte.
		eVALIGN m_vAlign;
		//!\~english Tells if the text (caption, wrap mode, or alignments) has changed.	\~french Dit si le texte (contenu, mode de découpe, alignements) a changé.
		bool m_textChanged;
		//!\~english The size (in spaces) of tabulation character.	\~french La taille (en espaces) du caractère de tabulation.
		uint32_t m_tabSize = 4;
	};
}

#endif
