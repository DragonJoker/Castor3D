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

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

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
	class C3D_API TextOverlay
		:	public OverlayCategory
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
		class C3D_API TextLoader
			:	public OverlayCategory::TextLoader
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
			virtual bool operator()( TextOverlay const & p_overlay, Castor::TextFile & p_file );
		};
		/*!
		\author		Sylvain DOREMUS
		\date		08/04/2014
		\~english
		\brief		TextOverlay loader
		\~english
		\brief		Loader de TextOverlay
		*/
		class C3D_API BinaryParser
			:	public OverlayCategory::BinaryParser
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
			BinaryParser( Castor::Path const & p_path );
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
			virtual bool Fill( TextOverlay const & p_obj, BinaryChunk & p_chunk )const;
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
			virtual bool Parse( TextOverlay & p_obj, BinaryChunk & p_chunk )const;
		};

	public:
		DECLARE_MAP( xchar, Castor::Position, GlyphPosition );

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		TextOverlay();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~TextOverlay();
		/**
		 *\~english
		 *\brief		Creation function, used by the factory
		 *\return		An overlay
		 *\~french
		 *\brief		Fonction de création utilisée par la fabrique
		 *\return		Un overlay
		 */
		static OverlayCategorySPtr Create();
		/**
		 *\~english
		 *\brief		Initialises the overlay
		 *\~french
		 *\brief		Initialise l'incrustation
		 */
		bool Initialise();
		/**
		 *\~english
		 *\brief		Flushes the overlay
		 *\~french
		 *\brief		Nettoie l'incrustation
		 */
		void Cleanup();
		/**
		 *\~english
		 *\brief		Sets the text font
		 *\param[in]	p_strFont	The new value
		 *\~french
		 *\brief		Définit la police du texte
		 *\param[in]	p_strFont	La nouvelle valeur
		 */
		void SetFont( Castor::String const & p_strFont );
		/**
		 *\~english
		 *\brief		Sets the material
		 *\param[in]	p_pMaterial	The new value
		 *\~french
		 *\brief		Définit le matériau
		 *\param[in]	p_pMaterial	La nouvelle valeur
		 */
		virtual void SetMaterial( MaterialSPtr p_pMaterial );
		/**
		 *\~english
		 *\brief		Retrieves the font name
		 *\return		The value
		 *\~french
		 *\brief		Récupère le nom de la police
		 *\return		La valeur
		 */
		Castor::String const & GetFontName()const;
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
		 *\brief		Retrieves the font
		 *\return		The value
		 *\~french
		 *\brief		Récupère la police
		 *\return		La valeur
		 */
		Castor::FontSPtr GetFont()const
		{
			return m_wpFont.lock();
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
			return m_strCaption;
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
			m_strCaption = p_strCaption;
			m_changed = true;
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
			m_wrappingMode = p_mode;
			m_changed = true;
		}
		/**
		 *\~english
		 *\brief		Retrieves the texture
		 *\return		The texture
		 *\~french
		 *\brief		Récupère la texture
		 *\return		La texture
		 */
		inline DynamicTextureSPtr GetTexture()const
		{
			return m_pTexture;
		}
		/**
		 *\~english
		 *\brief		Retrieves the wanted glyph position
		 *\param[in]	p_char	The glyph index
		 *\return		The position
		 *\~french
		 *\brief		Récupère la position de la glyphe voulue
		 *\param[in]	p_char	L'indice de la glyphe
		 *\return		La position
		 */
		Castor::Position const & GetGlyphPosition( xchar p_char )const;

	protected:
		/**
		 *\~english
		 *\brief		Draws the overlay
		 *\param[in]	p_renderer	The renderer used to draw this overlay
		 *\~french
		 *\brief		Dessine l'incrustation
		 *\param[in]	p_renderer	Le renderer utilisé pour dessiner cette incrustation
		 */
		virtual void DoRender( OverlayRendererSPtr p_renderer );
		/**
		 *\~english
		 *\brief		Updates the vertex buffer
		 *\param[in]	p_renderer	The renderer used to draw this overlay
		 *\~french
		 *\brief		Met à jour le tampon de sommets
		 *\param[in]	p_renderer	Le renderer utilisé pour dessiner cette incrustation
		 */
		virtual void DoUpdate( OverlayRendererSPtr p_renderer );
		/**
		 *\~english
		 *\brief		Adds a word to the vertex buffer
		 *\param[in]	p_renderer	The renderer used to draw this overlay
		 *\param[in]	p_word		The word to add
		 *\param[in]	p_wordWidth	The word width
		 *\param[in]	p_position	The word position
		 *\param[in]	p_size		The overlay size
		 *\~french
		 *\brief		Ajoute un mot au tampon de sommets
		 *\param[in]	p_renderer	Le renderer utilisé pour dessiner cette incrustation
		 *\param[in]	p_word		Le mot à ajouter
		 *\param[in]	p_wordWidth	La largeur du mot
		 *\param[in]	p_position	La position du mot
		 *\param[in]	p_size		La taille de l'incrustation
		 */
		void DoWriteWord( OverlayRendererSPtr p_renderer, Castor::String const & p_word, double p_wordWidth, Castor::Point2d const & p_size, Castor::Point2d & p_position );

	protected:
		//!\~english The current overlay caption	\~french Le texte courant de l'incrustation
		Castor::String m_strCaption;
		//!\~english The previous overlay caption	\~french Le texte précédent de l'incrustation
		Castor::String m_previousCaption;
		//!\~english The font	\~french La police
		Castor::FontWPtr m_wpFont;
		//!\~english The texture sampler	\~french L'échantillonneur de la texture
		SamplerWPtr m_wpSampler;
		//!\~english The texture that will receive the glyphs	\~french La texture qui recevra les glyphes
		DynamicTextureSPtr m_pTexture;
		//!\~english The font name	\~french Le nom de la police
		Castor::String m_strFontName;
		//!\~english Glyphs positions in the texture	\~french Position des glyphes
		GlyphPositionMap m_glyphsPositions;
		//!\~english The wrapping mode	\~french Le mode de découpe du texte
		eTEXT_WRAPPING_MODE m_wrappingMode;
	};
}

#pragma warning( pop )

#endif
