/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextOverlay_H___
#define ___C3D_TextOverlay_H___

#include "Castor3D/Overlay/OverlayCategory.hpp"
#include "Castor3D/Overlay/FontTexture.hpp"

namespace castor3d
{
	class TextOverlay
		: public OverlayCategory
	{
	public:
		/**
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
			castor::Point2f coords;
			castor::Point2f texture;
			castor::Point2f text;
		};
		CU_DeclareVector( Vertex, Vertex );

	public:
		CU_DeclareMap( char32_t, castor::Position, GlyphPosition );

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
		 *\brief		Creation function, used by the factory
		 *\return		An overlay
		 *\~french
		 *\brief		Fonction de création utilisée par la fabrique
		 *\return		Un overlay
		 */
		C3D_API static OverlayCategorySPtr create();
		/**
		 *\copydoc	castor3d::OverlayCategory::accept
		 */
		C3D_API void accept( OverlayVisitor & visitor )const override;
		/**
		 *\~english
		 *\brief		Sets the text font
		 *\param[in]	value	The new value
		 *\~french
		 *\brief		Définit la police du texte
		 *\param[in]	value	La nouvelle valeur
		 */
		C3D_API void setFont( castor::String const & value );
		/**
		 *\~english
		 *\brief		Retrieves the font name.
		 *\return		The value.
		 *\~french
		 *\brief		Récupère le nom de la police.
		 *\return		La valeur.
		 */
		inline castor::String const & getFontName()const
		{
			return getFontTexture()->getFontName();
		}
		/**
		 *\~english
		 *\return		\p true if this overlay's has changed.
		 *\~french
		 *\return		\p true si cette incrustation a changé.
		 */
		inline bool isChanged()const override
		{
			return m_textChanged;
		}
		/**
		 *\~english
		 *\return		The FontTexture.
		 *\~french
		 *\return		La FontTexture.
		 */
		inline FontTextureSPtr getFontTexture()const
		{
			return m_fontTexture.lock();
		}
		/**
		 *\~english
		 *\return		\p true if this overlay's font has changed.
		 *\~french
		 *\return		\p true si la police de cette incrustation a changé.
		 */
		inline bool isFontChanged()const
		{
			return m_fontChanged;
		}
		/**
		 *\~english
		 *\brief		Retrieves the panel vertex buffer
		 *\return		The buffer
		 *\~french
		 *\brief		Récupère le tampon de sommets du panneau
		 *\return		Le tampon
		 */
		inline VertexArray const & getTextVertex()const
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
		inline castor::String getCaption()const
		{
			return m_currentCaption;
		}
		/**
		 *\~english
		 *\brief		Sets the overlay text
		 *\param[in]	value	The new value
		 *\~french
		 *\brief		Définit le texte de l'incrustation
		 *\param[in]	value	La nouvelle valeur
		 */
		inline void setCaption( castor::String const & value )
		{
			m_currentCaption = value;
			m_textChanged = true;
		}
		/**
		 *\~english
		 *\return		The text wrapping mode.
		 *\~french
		 *\return		Le mode de découpe du texte.
		*/
		inline TextWrappingMode getTextWrappingMode()const
		{
			return m_wrappingMode;
		}
		/**
		 *\~english
		 *\brief		Sets text wrapping mode
		 *\param[in]	value	The new value
		 *\~french
		 *\brief		Définit le mode de découpe du texte
		 *\param[in]	value	La nouvelle valeur
		 */
		inline void setTextWrappingMode( TextWrappingMode value )
		{
			m_textChanged = m_textChanged || ( m_wrappingMode != value );
			m_wrappingMode = value;
		}
		/**
		 *\~english
		 *\return		The horizontal alignment.
		 *\~french
		 *\return		L'alignement horizontal.
		*/
		inline HAlign getHAlign()const
		{
			return m_hAlign;
		}
		/**
		 *\~english
		 *\brief		Defines the horizontal alignment
		 *\param[in]	value	The new value
		 *\~french
		 *\brief		Définit l'alignement horizontal
		 *\param[in]	value	La nouvelle valeur
		*/
		inline void setHAlign( HAlign value )
		{
			m_textChanged = m_textChanged || ( m_hAlign != value );
			m_hAlign = value;
		}
		/**
		 *\~english
		 *\return		The vertical alignment.
		 *\~french
		 *\return		L'alignement vertical.
		*/
		inline VAlign getVAlign()const
		{
			return m_vAlign;
		}
		/**
		 *\~english
		 *\brief		Defines the vertical alignment
		 *\param[in]	value	The new value
		 *\~french
		 *\brief		Définit l'alignement vertical
		 *\param[in]	value	La nouvelle valeur
		*/
		inline void setVAlign( VAlign value )
		{
			m_textChanged = m_textChanged || ( m_vAlign != value );
			m_vAlign = value;
		}
		/**
		 *\~english
		 *\return		The text texture mapping mode.
		 *\~french
		 *\return		Le mode de mappage de texture du texte.
		*/
		inline TextTexturingMode getTexturingMode()const
		{
			return m_texturingMode;
		}
		/**
		 *\~english
		 *\brief		Defines the text texture mapping mode.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit le mode de mappage de texture du texte.
		 *\param[in]	value	La nouvelle valeur.
		*/
		inline void setTexturingMode( TextTexturingMode value )
		{
			m_textChanged = m_textChanged || ( m_texturingMode != value );
			m_texturingMode = value;
		}
		/**
		 *\~english
		 *\return		The lines spacing mode.
		 *\~french
		 *\return		Le mode d'espacement des lignes.
		*/
		inline TextLineSpacingMode getLineSpacingMode()const
		{
			return m_lineSpacingMode;
		}
		/**
		 *\~english
		 *\brief		Defines the lines spacing mode.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit le mode d'espacement des lignes.
		 *\param[in]	value	La nouvelle valeur.
		*/
		inline void setLineSpacingMode( TextLineSpacingMode value )
		{
			m_textChanged = m_textChanged || ( m_lineSpacingMode != value );
			m_lineSpacingMode = value;
		}

	private:
		/**
		\author 	Sylvain DOREMUS
		\date 		28/01/2016
		\~english
		\brief		A character, along with its size and relative position.
		\~french
		\brief		Un caractère, avec ses dimensions et sa position relative.
		*/
		struct DisplayableChar
		{
			//!\~english	The character position, relative to its line.
			//!\~french		La position du caractère, relative à sa ligne.
			castor::Point2d m_position;
			//!\~english	The character dimensions.
			//!\~french		Les dimensions du caractère.
			castor::Point2d m_size;
			//!\~english	The character to display.
			//!\~french		Le caractère à afficher.
			castor::Glyph const & m_glyph;

			DisplayableChar( castor::Point2d const & position
				, castor::Point2d const & size
				, castor::Glyph const & glyph )
				: m_position{ position }
				, m_size{ size }
				, m_glyph{ glyph }
			{
			}

			DisplayableChar( DisplayableChar const & rhs )
				: m_position{ rhs.m_position }
				, m_size{ rhs.m_size }
				, m_glyph{ rhs.m_glyph }
			{
			}

			DisplayableChar( DisplayableChar && rhs )noexcept
				: m_position{ std::move( rhs.m_position ) }
				, m_size{ std::move( rhs.m_size ) }
				, m_glyph{ rhs.m_glyph }
			{
			}

			DisplayableChar & operator=( DisplayableChar const & rhs )
			{
				m_position = rhs.m_position;
				m_size = rhs.m_size;
				return *this;
			}

			DisplayableChar & operator=( DisplayableChar && rhs )noexcept
			{
				if ( &rhs != this )
				{
					m_position = std::move( rhs.m_position );
					m_size = std::move( rhs.m_size );
				}

				return *this;
			}
		};
		/**
		\author 	Sylvain DOREMUS
		\date 		28/01/2016
		\~english
		\brief		A text line, along with its size and position.
		\~french
		\brief		Une ligne de texte, avec ses dimensions et sa position.
		*/
		struct DisplayableLine
		{
			//!\~english	The line position.
			//!\~french		La position de la ligne.
			castor::Point2d m_position;
			//!\~english	The line width.
			//!\~french		La largeur de la ligne.
			double m_width;
			//!\~english	The line height.
			//!\~french		La hauteur de la ligne.
			double m_height;
			//!\~english	The displayable characters.
			//!\~french		Les caractères affichables.
			std::vector< DisplayableChar > m_characters{};
		};
		using DisplayableLineArray = std::vector< DisplayableLine >;
		using TextureCoordinates = std::array< float, 2 >;
		CU_DeclareVector( TextureCoordinates, TextureCoords );
		/**
		 *\copydoc	castor3d::OverlayCategory::doUpdate
		 */
		C3D_API void doUpdate( OverlayRenderer const & renderer )override;
		/**
		 *\copydoc	castor3d::OverlayCategory::doUpdateBuffer
		 */
		C3D_API void doUpdateBuffer( castor::Size const & size )override;
		/**
		 *\~english
		 *\brief		Updates the vertex buffer.
		 *\param[in]	size		The render target size.
		 *\param[in]	generateUvs	The UV generator function.
		 *\~french
		 *\brief		Met à jour le tampon de sommets.
		 *\param[in]	size		Les dimensions de la cible de rendu.
		 *\param[in]	generateUvs	La fonction de génération d'UV.
		 */
		C3D_API void doUpdateBuffer( castor::Size const & size
			, std::function< void( castor::Point2d const & size
				, castor::Rectangle const & absolute
				, castor::Point4f const & fontUV
				, float & uvLeft
				, float & uvTop
				, float & uvRight
				, float & uvBottom ) > generateUvs );
		/**
		 *\~english
		 *\brief		Computes the lines to display.
		 *\remarks		Takes care of vertical alignment to retrieve the right vertical offset.
		 *\param[in]	renderSize	The render size.
		 *\param[in]	size		The overlay dimensions.
		 *\return		The lines.
		 *\~french
		 *\brief		Calcule les lignes à afficher.
		 *\remarks		Prend en compte l'alignement vertical, pour calculer le décalage vertical.
		 *\param[in]	renderSize	Les dimensions de la zone de rendu.
		 *\param[in]	size		The overlay dimensions.
		 *\return		Les lignes.
		 */
		C3D_API DisplayableLineArray doPrepareText( castor::Size const & renderSize
			, castor::Point2d const & size );
		/**
		 *\~english
		 *\brief		adds a word to the vertex buffer.
		 *\param[in]	renderSize	The render size.
		 *\param[in]	word		The word to add.
		 *\param[in]	wordWidth	The word width.
		 *\param[in]	size		The overlay size.
		 *\param[out]	left		The left position.
		 *\param[out]	line		The line.
		 *\param[out]	lines		The lines.
		 *\~french
		 *\brief		Ajoute un mot au tampon de sommets.
		 *\param[in]	renderSize	Les dimensions de la zone de rendu.
		 *\param[in]	word		Le mot à ajouter.
		 *\param[in]	wordWidth	La largeur du mot.
		 *\param[in]	size		La taille de l'incrustation.
		 *\param[out]	left		La position à gauche.
		 *\param[out]	line		La ligne.
		 *\param[out]	lines		Les lignes.
		 */
		C3D_API void doPrepareWord( castor::Size const & renderSize
			, std::u32string const & word
			, double wordWidth
			, castor::Point2d const & size
			, double & left
			, DisplayableLine & line
			, DisplayableLineArray & lines );
		/**
		 *\~english
		 *\brief		Fills the line, and jumps to the next one.
		 *\param[in]	size	The overlay size.
		 *\param[in]	line	The line.
		 *\param[out]	left	The left position.
		 *\param[out]	lines	The lines.
		 *\~french
		 *\brief		Finit la ligne et passe à la ligne suivante.
		 *\param[in]	size	La taille de l'incrustation.
		 *\param[in]	line	La ligne.
		 *\param[out]	left	La position à gauche.
		 *\param[out]	lines	Les lignes.
		 */
		C3D_API DisplayableLine doFinishLine( castor::Point2d const & size
			, DisplayableLine line
			, double & left
			, DisplayableLineArray & lines );
		/**
		 *\~english
		 *\brief		Horizontally align a line.
		 *\param[in]	width	The overlay width.
		 *\param[in]	line	The line.
		 *\param[out]	lines	The lines.
		 *\~french
		 *\brief		Aligne horizontalement une ligne.
		 *\param[in]	width	La largeur de l'incrustation.
		 *\param[in]	line	La ligne.
		 *\param[out]	lines	Les lignes.
		 */
		C3D_API void doAlignHorizontally( double width
			, DisplayableLine line
			, DisplayableLineArray & lines );
		/**
		 *\~english
		 *\brief		Vertically align text.
		 *\param[in]	height	The overlay height.
		 *\param[out]	lines		The lines.
		 *\~french
		 *\brief		Aligne verticalement un texte.
		 *\param[in]	height	La hauteur de l'incrustation.
		 *\param[out]	lines		Les lignes.
		 */
		C3D_API void doAlignVertically( double height
			, DisplayableLineArray & lines );

	private:
		VertexArray m_arrayVtx;
		castor::String m_currentCaption;
		castor::String m_previousCaption;
		FontTextureWPtr m_fontTexture;
		bool m_fontChanged{ true };
		TextWrappingMode m_wrappingMode{ TextWrappingMode::eNone };
		TextLineSpacingMode m_lineSpacingMode{ TextLineSpacingMode::eOwnHeight };
		HAlign m_hAlign{ HAlign::eLeft };
		VAlign m_vAlign{ VAlign::eCenter };
		bool m_textChanged{ true };
		FontTexture::OnChanged::connection m_connection;
		TextTexturingMode m_texturingMode{ TextTexturingMode::eText };
		TextureCoordsArray m_arrayTextTexture;
	};
}

#endif
