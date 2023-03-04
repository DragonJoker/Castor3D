/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextOverlay_H___
#define ___C3D_TextOverlay_H___

#include "Castor3D/Overlay/OverlayCategory.hpp"
#include "Castor3D/Overlay/FontTexture.hpp"

#include <CastorUtils/Design/ArrayView.hpp>

#include <ashespp/Pipeline/PipelineShaderStageCreateInfo.hpp>

namespace castor3d
{
	class TextOverlay
		: public OverlayCategory
	{
	public:
		/**
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
		 *\copydoc		castor3d::OverlayCategory::accept
		 */
		C3D_API void accept( OverlayVisitor & visitor )const override;
		/**
		 *\~english
		 *\return		The vertex count needed for this overlay.
		 *\~french
		 *\return		Le nombre de sommets nécessaires pour cette incrustation.
		 */
		C3D_API uint32_t getDisplayCount()const;
		/**
		 *\~english
		 *\return		The vertex count needed for this overlay.
		 *\~french
		 *\return		Le nombre de sommets nécessaires pour cette incrustation.
		 */
		C3D_API uint32_t getCount( bool )const;
		/**
		 *\~english
		 *\brief		Fills the given buffer.
		 *\param[out]	buffer	The buffer.
		 *\~french
		 *\brief		Remplit le tampon de sommets donné.
		 *\param[out]	buffer	Le buffer.
		 */
		C3D_API void fillBuffer( castor::Size const & refSize
			, Vertex * buffer
			, bool secondary )const;
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
		castor::String const & getFontName()const
		{
			return getFontTexture()->getFontName();
		}
		/**
		 *\~english
		 *\return		\p true if this overlay's has changed.
		 *\~french
		 *\return		\p true si cette incrustation a changé.
		 */
		bool isChanged()const override
		{
			return m_textChanged;
		}
		/**
		 *\~english
		 *\return		The FontTexture.
		 *\~french
		 *\return		La FontTexture.
		 */
		FontTextureSPtr getFontTexture()const
		{
			return m_fontTexture.lock();
		}
		/**
		 *\~english
		 *\brief		Retrieves the overlay text
		 *\return		The value
		 *\~french
		 *\brief		Récupère le texte de l'incrustation
		 *\return		La valeur
		 */
		std::u32string getCaption()const
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
		void setCaption( std::u32string const & value )
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
		TextWrappingMode getTextWrappingMode()const
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
		void setTextWrappingMode( TextWrappingMode value )
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
		HAlign getHAlign()const
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
		void setHAlign( HAlign value )
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
		VAlign getVAlign()const
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
		void setVAlign( VAlign value )
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
		TextTexturingMode getTexturingMode()const
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
		void setTexturingMode( TextTexturingMode value )
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
		TextLineSpacingMode getLineSpacingMode()const
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
		void setLineSpacingMode( TextLineSpacingMode value )
		{
			m_textChanged = m_textChanged || ( m_lineSpacingMode != value );
			m_lineSpacingMode = value;
		}

	private:
		using UvGenFunc = std::function< void( castor::Point2f const & size
			, castor::Point4i const & absolute
			, castor::Point4f & uv ) >;
		/**
		 *\copydoc	castor3d::OverlayCategory::doUpdate
		 */
		C3D_API void doUpdate( OverlayRenderer const & renderer )override;
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
		C3D_API void doFillBuffer( castor::Size const & refSize
			, UvGenFunc generateUvs );
		/**
		 *\~english
		 *\brief		Computes the lines to display.
		 *\param[in]	overlaySize	The overlay dimensions.
		 *\~french
		 *\brief		Calcule les lignes à afficher.
		 *\param[in]	overlaySize	The overlay dimensions.
		 */
		C3D_API void doPrepareText( castor::Point2f const & overlaySize );

	private:
		std::u32string m_currentCaption;
		std::u32string m_previousCaption;
		FontTextureWPtr m_fontTexture;
		TextWrappingMode m_wrappingMode{ TextWrappingMode::eNone };
		TextLineSpacingMode m_lineSpacingMode{ TextLineSpacingMode::eOwnHeight };
		HAlign m_hAlign{ HAlign::eLeft };
		VAlign m_vAlign{ VAlign::eCenter };
		bool m_textChanged{ true };
		FontTexture::OnChanged::connection m_connection;
		TextTexturingMode m_texturingMode{ TextTexturingMode::eText };
		std::vector< Vertex > m_buffer;
		std::array< TextChar, MaxCharsPerOverlay > m_text;
		uint32_t m_charsCount;
		OverlayWords m_words;
		OverlayLines m_lines;
	};
}

#endif
