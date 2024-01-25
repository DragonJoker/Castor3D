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
		enum class ComputeBindingIdx
		{
			eCamera,
			eOverlays,
			eChars,
			eWords,
			eLines,
			eFont,
			eVertex,
		};
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
		C3D_API static OverlayCategoryUPtr create();
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
		C3D_API uint32_t getCount( bool )const;
		/**
		 *\~english
		 *\brief		Fills the text buffers.
		 *\param[out]	overlayIndex	The overlay index.
		 *\param[out]	texts			View on the allocated text chars for the overlay.
		 *\param[out]	words			View on the allocated text words for the overlay.
		 *\param[out]	lines			View on the allocated text lines for the overlay.
		 *\~french
		 *\brief		Remplit les tampons de texte donnés.
		 *\param[out]	overlayIndex	L'index de de l'incrustation.
		 *\param[out]	texts			Vue sur les text chars alloués pour l'incrustation.
		 *\param[out]	words			Vue sur les text words alloués pour l'incrustation.
		 *\param[out]	lines			Vue sur les text lines alloués pour l'incrustation.
		 */
		C3D_API float fillBuffer( uint32_t overlayIndex
			, castor::ArrayView< TextChar > texts
			, castor::ArrayView< TextWord > words
			, castor::ArrayView< TextLine > lines )const noexcept;
		/**
		 *\~english
		 *\brief		Creates the shader program used to compute the overlay's vertices.
		 *\return		The program.
		 *\~french
		 *\brief		Crée le programme utilisé pour calculer les sommets de l'incrustation.
		 *\return		Le programme.
		 */
		C3D_API static ashes::PipelineShaderStageCreateInfo createProgram( RenderDevice const & device );
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
		*\name
		*	Getters.
		*\~french
		*\name
		*	Accesseurs.
		*/
		/**@{*/
		bool isChanged()const noexcept override
		{
			return m_textChanged;
		}

		castor::String const & getFontName()const
		{
			return getFontTexture()->getFontName();
		}

		FontTextureRPtr getFontTexture()const
		{
			return m_fontTexture;
		}

		castor::U32String getCaption()const
		{
			return m_currentCaption;
		}

		TextWrappingMode getTextWrappingMode()const
		{
			return m_wrappingMode;
		}

		HAlign getHAlign()const
		{
			return m_hAlign;
		}

		VAlign getVAlign()const
		{
			return m_vAlign;
		}

		TextTexturingMode getTexturingMode()const
		{
			return m_texturingMode;
		}

		TextLineSpacingMode getLineSpacingMode()const
		{
			return m_lineSpacingMode;
		}

		uint32_t getCharCount()const noexcept
		{
			return m_charsCount;
		}

		uint32_t getWordCount()const noexcept
		{
			return m_words.count;
		}

		uint32_t getLineCount()const noexcept
		{
			return m_lines.count;
		}
		/**@}*/
		/**
		*\~english
		*\name
		*	Mutators.
		*\~french
		*\name
		*	Mutateurs.
		*/
		/**@{*/
		void setCaption( castor::U32String value )
		{
			m_currentCaption = castor::move( value );
			m_textChanged = true;
		}

		void setTextWrappingMode( TextWrappingMode value )
		{
			m_textChanged = m_textChanged || ( m_wrappingMode != value );
			m_wrappingMode = value;
		}

		void setHAlign( HAlign value )
		{
			m_textChanged = m_textChanged || ( m_hAlign != value );
			m_hAlign = value;
		}

		void setVAlign( VAlign value )
		{
			m_textChanged = m_textChanged || ( m_vAlign != value );
			m_vAlign = value;
		}

		void setTexturingMode( TextTexturingMode value )
		{
			m_textChanged = m_textChanged || ( m_texturingMode != value );
			m_texturingMode = value;
		}

		void setLineSpacingMode( TextLineSpacingMode value )
		{
			m_textChanged = m_textChanged || ( m_lineSpacingMode != value );
			m_lineSpacingMode = value;
		}
		/**@}*/

	private:
		using UvGenFunc = castor::Function< void( castor::Point2f const & size
			, castor::Point4i const & absolute
			, castor::Point4f & uv ) >;
		/**
		 *\copydoc	castor3d::OverlayCategory::doReset
		 */
		void doReset()override;
		/**
		 *\copydoc	castor3d::OverlayCategory::doUpdate
		 */
		void doUpdate( OverlayRenderer const & renderer )override;
		/**
		 *\~english
		 *\brief		Computes the lines to display.
		 *\param[in]	renderSize	The render area dimensions.
		 *\~french
		 *\brief		Calcule les lignes à afficher.
		 *\param[in]	renderSize	Les dimensions de la zone de rendu.
		 */
		void doPrepareText( castor::Size const & renderSize );

	private:
		castor::U32String m_currentCaption;
		castor::U32String m_previousCaption;
		FontTextureRPtr m_fontTexture{};
		TextWrappingMode m_wrappingMode{ TextWrappingMode::eNone };
		TextLineSpacingMode m_lineSpacingMode{ TextLineSpacingMode::eOwnHeight };
		HAlign m_hAlign{ HAlign::eLeft };
		VAlign m_vAlign{ VAlign::eCenter };
		bool m_textChanged{ true };
		FontTexture::OnChanged::connection m_connection;
		TextTexturingMode m_texturingMode{ TextTexturingMode::eText };
		castor::Array< TextChar, MaxCharsPerOverlay > m_text;
		uint32_t m_charsCount{};
		OverlayWords m_words;
		OverlayLines m_lines;
	};
}

#endif
