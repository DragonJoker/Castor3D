#include "Castor3D/Overlay/TextOverlay.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/OverlayCache.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Render/Overlays/OverlayRenderer.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslFont.hpp"
#include "Castor3D/Shader/Shaders/GlslOverlaySurface.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"
#include "Castor3D/Shader/Ubos/OverlayUbo.hpp"

#include <ShaderWriter/Source.hpp>

#include <CastorUtils/Graphics/Font.hpp>
#include <CastorUtils/Graphics/Image.hpp>
#include <CastorUtils/Graphics/Rectangle.hpp>

CU_ImplementCUSmartPtr( castor3d, TextOverlay )

#if defined( drawText )
#	undef drawText
#endif

#define C3D_Chars( writer, binding, set ) \
	auto charsBuffer = writer.declStorageBuffer( "C3D_CharsBuffer" \
		, uint32_t( binding ) \
		, set ); \
	auto c3d_chars = charsBuffer.declMember< ovrltxt::TextChar >( "c3d_chars", MaxCharsPerBuffer ); \
	charsBuffer.end()

#define C3D_Words( writer, binding, set ) \
	auto wordsBuffer = writer.declStorageBuffer( "C3D_WordsBuffer" \
		, uint32_t( binding ) \
		, set ); \
	auto c3d_words = wordsBuffer.declMember< ovrltxt::TextWord >( "c3d_words", MaxWordsPerBuffer ); \
	wordsBuffer.end()

#define C3D_Lines( writer, binding, set ) \
	auto linesBuffer = writer.declStorageBuffer( "C3D_LinesBuffer" \
		, uint32_t( binding ) \
		, set ); \
	auto c3d_lines = linesBuffer.declMember< ovrltxt::TextLine >( "c3d_lines", MaxLinesPerBuffer ); \
	linesBuffer.end()

namespace castor3d
{
	//*********************************************************************************************

	namespace ovrltxt
	{
		class TextChar
			: public sdw::StructInstanceHelperT< "C3D_TextChar"
				, sdw::type::MemoryLayout::eStd430
				, sdw::Vec2Field< "size" >
				, sdw::Vec2Field< "uvPosition" >
				, sdw::Vec2Field< "bearing" >
				, sdw::FloatField< "left" >
				, sdw::UIntField< "word" >
				, sdw::UIntField< "overlay" >
				, sdw::UIntField< "index" > >
		{
		public:
			TextChar( sdw::ShaderWriter & writer
				, sdw::expr::ExprPtr expr
				, bool enabled )
				: StructInstanceHelperT{ writer, std::move( expr ), enabled }
			{
			}

			auto size()const { return getMember< "size" >(); }
			auto uvPosition()const { return getMember< "uvPosition" >(); }
			auto bearing()const { return getMember< "bearing" >(); }
			auto left()const { return getMember< "left" >(); }
			auto word()const { return getMember< "word" >(); }
			auto overlay()const { return getMember< "overlay" >(); }
			auto index()const { return getMember< "index" >(); }
		};

		class TextWord
			: public sdw::StructInstanceHelperT< "C3D_TextWord"
				, sdw::type::MemoryLayout::eStd430
				, sdw::Vec2Field< "range" >
				, sdw::FloatField< "left" >
				, sdw::FloatField< "width" >
				, sdw::UIntField< "charBegin" >
				, sdw::UIntField< "charEnd" >
				, sdw::UIntField< "line" >
				, sdw::UIntField< "pad" > >
		{
		public:
			TextWord( sdw::ShaderWriter & writer
				, sdw::expr::ExprPtr expr
				, bool enabled )
				: StructInstanceHelperT{ writer, std::move( expr ), enabled }
			{
			}

			auto range()const { return this->template getMember< "range" >(); }
			auto left()const { return this->template getMember< "left" >(); }
			auto width()const { return this->template getMember< "width" >(); }
			auto line()const { return this->template getMember< "line" >(); }
		};

		class TextLine
			: public sdw::StructInstanceHelperT< "C3D_TextLine"
				, sdw::type::MemoryLayout::eStd430
				, sdw::Vec2Field< "position" >
				, sdw::Vec2Field< "range" >
				, sdw::FloatField< "width" >
				, sdw::UIntField< "wordBegin" >
				, sdw::UIntField< "wordEnd" >
				, sdw::UIntField< "charBegin" >
				, sdw::UIntField< "charEnd" >
				, sdw::UIntField< "pad" > >
		{
		public:
			TextLine( sdw::ShaderWriter & writer
				, sdw::expr::ExprPtr expr
				, bool enabled )
				: StructInstanceHelperT{ writer, std::move( expr ), enabled }
			{
			}

			auto position()const { return this->template getMember< "position" >(); }
			auto range()const { return this->template getMember< "range" >(); }
			auto width()const { return this->template getMember< "width" >(); }
		};

		static castor::Font & getFont( TextOverlay const & overlay )
		{
			FontTextureSPtr fontTexture = overlay.getFontTexture();

			if ( !fontTexture )
			{
				CU_Failure( cuT( "The TextOverlay has no FontTexture. Did you set its font?" ) );
				CU_Exception( cuT( "The TextOverlay [" ) + overlay.getOverlayName() + cuT( "] has no FontTexture. Did you set its font?" ) );
			}

			auto pfont = fontTexture->getFont();

			if ( !pfont )
			{
				CU_Failure( cuT( "The TextOverlay has no Font. Did you set its font?" ) );
				CU_Exception( cuT( "The TextOverlay [" ) + overlay.getOverlayName() + cuT( "] has no Font. Did you set its font?" ) );
			}

			return *pfont;
		}

		template< typename TextContainerT >
		static bool isEmpty( TextContainerT const & v )
		{
			return v.charEnd == v.charBegin;
		}
	}

	//*********************************************************************************************

	TextOverlay::TextOverlay()
		: OverlayCategory{ OverlayType::eText }
	{
	}

	OverlayCategorySPtr TextOverlay::create()
	{
		return std::make_shared< TextOverlay >();
	}

	void TextOverlay::accept( OverlayVisitor & visitor )const
	{
		visitor.visit( *this );
	}

	uint32_t TextOverlay::getCount( bool secondary )const
	{
		return m_charsCount * 6u;
	}

	float TextOverlay::fillBuffer( uint32_t overlayIndex
		, castor::ArrayView< TextChar > texts
		, castor::ArrayView< TextWord > words
		, castor::ArrayView< TextLine > lines )const
	{
		std::copy( m_text.begin()
			, m_text.begin() + m_charsCount
			, texts.begin() );
		std::copy( m_words.words().begin()
			, m_words.words().end()
			, words.begin() );
		std::copy( m_lines.lines().begin()
			, m_lines.lines().end()
			, lines.begin() );

		for ( auto & v : texts )
		{
			v.overlay = overlayIndex;
		}

		return m_lines.topOffset;
	}

	ashes::PipelineShaderStageCreateInfo TextOverlay::createProgram( RenderDevice const & device )
	{
		ShaderModule comp{ VK_SHADER_STAGE_COMPUTE_BIT, "PanelOverlayCompute" };
		sdw::ComputeWriter writer;

		C3D_Camera( writer
			, TextOverlay::ComputeBindingIdx::eCamera
			, 0u );
		C3D_Overlays( writer
			, TextOverlay::ComputeBindingIdx::eOverlays
			, 0u );
		C3D_Chars( writer
			, TextOverlay::ComputeBindingIdx::eChars
			, 0u );
		C3D_Words( writer
			, TextOverlay::ComputeBindingIdx::eWords
			, 0u );
		C3D_Lines( writer
			, TextOverlay::ComputeBindingIdx::eLines
			, 0u );
		shader::FontData c3d_fontData{ writer
			, uint32_t( TextOverlay::ComputeBindingIdx::eFont )
			, 0u };
		C3D_OverlaysSurfaces( writer
			, TextOverlay::ComputeBindingIdx::eVertex
			, 0u
			, true
			, true );

		auto batchData = writer.declPushConstantsBuffer( "BatchData" );
		auto c3d_batchOffset = batchData.declMember< sdw::UInt >( "c3d_batchOffset" );
		auto c3d_charCount = batchData.declMember< sdw::UInt >( "c3d_charCount" );
		batchData.end();

		auto processChar = [&]( shader::OverlayData & overlay
			, ovrltxt::TextChar const & character
			, sdw::Vec2 const & texDim
			, std::function< sdw::Vec4( sdw::Vec2 const
				, sdw::Vec4 const ) > generateUvs )
		{
			auto offset = writer.declLocale( "offset"
				, overlay.vertexOffset() + ( character.index() * 6u ) );
			auto word = writer.declLocale( "word"
				, c3d_words[overlay.textWordOffset() + character.word()] );
			auto line = writer.declLocale( "line"
				, c3d_lines[overlay.textLineOffset() + word.line()] );
			auto renderSize = writer.declLocale( "renderSize"
				, vec2( c3d_cameraData.renderSize() ) );
			auto ssAbsParentSize = writer.declLocale( "ssAbsParentSize"
				, overlay.parentRect().zw() - overlay.parentRect().xy() );
			auto ssAbsSize = writer.declLocale( "ssAbsSize"
				, overlay.relativeSize() * ssAbsParentSize );

			auto ssRelBounds = writer.declLocale( "ssRelBounds"
				, vec4( line.position().x() + word.left() + character.left() + character.bearing().x()
					, overlay.textTopOffset() + line.position().y() - character.bearing().y()
					, 0, 0 ) );
			ssRelBounds.z() = ssRelBounds.x() + character.size().x();
			ssRelBounds.w() = ssRelBounds.y() + character.size().y();
			ssRelBounds /= vec4( renderSize.xy(), renderSize.xy() );
			auto ssRelOvPosition = writer.declLocale( "ssRelOvPosition"
				, overlay.relativePosition() * ssAbsParentSize );
			auto ssAbsOvPosition = writer.declLocale( "ssAbsOvPosition"
				, ssRelOvPosition + overlay.parentRect().xy() );

			auto texUv = writer.declLocale( "texUv", vec4( 0.0_f ) );
			auto fontUv = writer.declLocale( "fontUv", vec4( 0.0_f ) );

			// check for full underflow/overflow
			IF( writer, ssAbsOvPosition.x() + ssRelBounds.x() < overlay.renderArea().z()
				&& ssAbsOvPosition.y() + ssRelBounds.y() < overlay.renderArea().w()
				&& ssAbsOvPosition.x() + ssRelBounds.z() > overlay.renderArea().x()
				&& ssAbsOvPosition.y() + ssRelBounds.w() > overlay.renderArea().y() )
			{
				//
				// Compute Letter's Font UV.
				//
				fontUv = vec4( character.uvPosition().x() / texDim.x()
					, 0.0, 0.0
					, character.uvPosition().y() / texDim.y() );
				fontUv.z() = fontUv.x() + ( character.size().x() / texDim.x() );
				fontUv.y() = fontUv.w() + ( character.size().y() / texDim.y() );
				//
				// Compute Letter's Texture UV.
				//
				texUv = generateUvs( ssAbsSize, ssRelBounds );

				auto ssAbsCharSize = writer.declLocale( "ssAbsCharSize"
					, ssRelBounds.zw() - ssRelBounds.xy() );
				auto srcUv = writer.declLocale( "srcUv"
					, texUv );
				auto srcFontUv = writer.declLocale( "srcFontUv"
					, fontUv );
				overlay.cropMinMinValue( ssAbsOvPosition.x(), ssAbsParentSize.x(), ssAbsSize.x(), ssAbsCharSize.x(), overlay.renderArea().xz(), srcUv.xz(), srcFontUv.xz(), ssRelBounds.x(), texUv.x(), fontUv.x() );
				overlay.cropMinMaxValue( ssAbsOvPosition.y(), ssAbsParentSize.y(), ssAbsSize.y(), ssAbsCharSize.y(), overlay.renderArea().yw(), srcUv.yw(), srcFontUv.wy(), ssRelBounds.y(), texUv.y(), fontUv.y() );
				overlay.cropMaxMaxValue( ssAbsOvPosition.x(), ssAbsParentSize.x(), ssAbsSize.x(), ssAbsCharSize.x(), overlay.renderArea().xz(), srcUv.xz(), srcFontUv.xz(), ssRelBounds.z(), texUv.z(), fontUv.z() );
				overlay.cropMaxMinValue( ssAbsOvPosition.y(), ssAbsParentSize.y(), ssAbsSize.y(), ssAbsCharSize.y(), overlay.renderArea().yw(), srcUv.yw(), srcFontUv.wy(), ssRelBounds.w(), texUv.w(), fontUv.w() );
			}
			ELSE
			{
				ssRelBounds.z() = ssRelBounds.x();
				ssRelBounds.w() = ssRelBounds.y();
			}
			FI;
			//
			// Fill buffer
			//
			uint32_t index = 0;
			c3d_overlaysSurfaces[offset + index].set( ssRelBounds.xy(), texUv.xy(), fontUv.xy() ); ++index;
			c3d_overlaysSurfaces[offset + index].set( ssRelBounds.xw(), texUv.xw(), fontUv.xw() ); ++index;
			c3d_overlaysSurfaces[offset + index].set( ssRelBounds.zw(), texUv.zw(), fontUv.zw() ); ++index;
			c3d_overlaysSurfaces[offset + index].set( ssRelBounds.xy(), texUv.xy(), fontUv.xy() ); ++index;
			c3d_overlaysSurfaces[offset + index].set( ssRelBounds.zw(), texUv.zw(), fontUv.zw() ); ++index;
			c3d_overlaysSurfaces[offset + index].set( ssRelBounds.zy(), texUv.zy(), fontUv.zy() ); ++index;
		};

		writer.implementMain( 1u, 1u
			, [&]( sdw::ComputeIn in )
			{
				auto charIndex = writer.declLocale( "charIndex"
					, c3d_batchOffset + in.globalInvocationID.x() * 16u + in.globalInvocationID.y() );

				IF( writer, charIndex < MaxCharsPerBuffer
					&& charIndex < c3d_charCount )
				{
					auto character = writer.declLocale( "character"
						, c3d_chars[charIndex] );
					auto overlay = writer.declLocale( "overlay"
						, c3d_overlaysData[character.overlay()] );
					auto texDim = writer.declLocale( "texDim"
						, vec2( c3d_fontData.imgWidth(), c3d_fontData.imgHeight() ) );

					IF( writer, overlay.textTexturingMode() == uint32_t( TextTexturingMode::eLetter ) )
					{
						processChar( overlay
							, character
							, texDim
							, []( sdw::Vec2 const ratio
								, sdw::Vec4 const absolute )
							{
								return vec4( 0.0_f, 0.0_f, 1.0_f, 1.0_f );
							} );
					}
					ELSE
					{
						processChar( overlay
							, character
							, texDim
							, []( sdw::Vec2 const ratio
								, sdw::Vec4 const absolute )
							{
								return vec4( absolute ) / vec4( ratio, ratio );
							} );
					}
					FI;
				}
				FI;
			} );

		comp.shader = std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		return makeShaderState( device, comp );
	}

	void TextOverlay::setFont( castor::String const & name )
	{
		// Récupération / Création de la police
		Engine * engine = m_overlay->getEngine();
		auto & fontCache = engine->getFontCache();

		if ( auto font = fontCache.find( name ).lock() )
		{
			FontTextureSPtr fontTexture = engine->getOverlayCache().getFontTexture( font->getName() );

			if ( !fontTexture )
			{
				fontTexture = engine->getOverlayCache().createFontTexture( font );
				fontTexture->update( false );
			}

			m_connection.disconnect();
			m_fontTexture = fontTexture;
			m_connection = fontTexture->onResourceChanged.connect( [this]( DoubleBufferedTextureLayout const & )
			{
				m_textChanged = true;
			} );
		}
		else
		{
			CU_Failure( cuT( "Font not found" ) );
			CU_Exception( "Font " + castor::string::stringCast< char >( name ) + "not found" );
		}

		m_textChanged = true;
	}

	void TextOverlay::doReset()
	{
		m_textChanged = false;
	}

	void TextOverlay::doUpdate( OverlayRenderer const & renderer )
	{
		FontTextureSPtr fontTexture = getFontTexture();

		if ( !fontTexture )
		{
			CU_Failure( cuT( "The TextOverlay has no FontTexture. Did you set its font?" ) );
			CU_Exception( cuT( "The TextOverlay [" ) + getOverlayName() + cuT( "] has no FontTexture. Did you set its font?" ) );
		}

		auto font = fontTexture->getFont();

		if ( !font )
		{
			setVisible( false );
			CU_Failure( cuT( "The TextOverlay has no Font. Did you set its font?" ) );
			CU_Exception( cuT( "The TextOverlay [" ) + getOverlayName() + cuT( "] has no Font. Did you set its font?" ) );
		}

		std::vector< char32_t > newCaption;

		for ( auto c : m_currentCaption )
		{
			if ( !font->hasGlyphAt( c ) )
			{
				newCaption.push_back( c );
			}
		}

		if ( !newCaption.empty() )
		{
			for ( auto c : newCaption )
			{
				font->loadGlyph( c );
			}

			fontTexture->update( true );
		}

		if ( !m_currentCaption.empty() )
		{
			m_previousCaption = m_currentCaption;
			auto count = std::count( m_previousCaption.begin(), m_previousCaption.end(), U' ' );
			count += std::count( m_previousCaption.begin(), m_previousCaption.end(), U'\t' );
			count += std::count( m_previousCaption.begin(), m_previousCaption.end(), U'\n' );
			m_charsCount = uint32_t( ( m_previousCaption.size() - uint32_t( std::max( ptrdiff_t{}, count ) ) ) );
			doPrepareText( renderer.getSize() );
		}
	}

	void TextOverlay::doPrepareText( castor::Size const & rndSize )
	{
		castor::Point2f renderSize{ castor::Point2f{ rndSize.getWidth(), rndSize.getHeight() }
			* getRenderRatio( rndSize ) };
		castor::Point2f overlaySize( renderSize * getOverlay().getAbsoluteSize() );

		m_words.count = 0u;
		m_lines.count = 0u;
		m_lines.maxRange = { 100.0, 0.0 };
		m_lines.topOffset = {};
		m_lines.count = {};

		auto caption = m_previousCaption;
		auto fontTexture = getFontTexture();
		auto & font = ovrltxt::getFont( *this );
		float lineTop{};
		{
			float totalLeft{};
			float wordLeft{};
			float charLeft{};
			uint32_t charIndex{};
			uint32_t wordIndex{};
			uint32_t lineIndex{};
			auto cit = caption.begin();
			auto tit = m_text.begin();

			auto nextWord = [&]()
			{
				auto & word = m_words.getNext();
				word.left = wordLeft;
				word.range = { 100.0, 0.0 };
				word.charBegin = charIndex;
				word.charEnd = word.charBegin;
				word.line = {};
				return &word;
			};

			auto nextLine = [&]()
			{
				auto & line = m_lines.getNext();
				line.position = { 0.0, lineTop };
				line.range = { 100.0, 0.0 };
				line.wordBegin = wordIndex;
				line.wordEnd = line.wordBegin;
				line.charBegin = charIndex;
				line.charEnd = line.charBegin;
				line.width = 0.0;
				charLeft = totalLeft - wordLeft;
				totalLeft = charLeft;
				wordLeft = 0.0;
				return &line;
			};

			auto word = nextWord();
			auto line = nextLine();

			auto alignLine = [&]()
			{
				if ( !ovrltxt::isEmpty( *line ) )
				{
					if ( m_lineSpacingMode == TextLineSpacingMode::eMaxFontHeight )
					{
						line->range = castor::Point2f{ ovrltxt::getFont( *this ).getMaxRange() };
					}

					// Move line according to halign
					if ( m_hAlign != HAlign::eLeft )
					{
						auto offset = overlaySize->x - line->width;

						if ( m_hAlign == HAlign::eCenter )
						{
							offset /= 2;
						}

						line->position->x = line->position->x + offset;
					}
				}

				lineTop += line->range->y - line->range->x;
				++lineIndex;
			};

			auto addWord = [&]()
			{
				if ( !ovrltxt::isEmpty( *word ) )
				{
					word->width = charLeft;
					word->line = lineIndex;
					line->range->x = std::min( line->range->x, word->range->x );
					line->range->y = std::max( line->range->y, word->range->y );
					line->width = totalLeft;
					m_lines.maxRange->x = std::min( m_lines.maxRange->x, line->range->x );
					m_lines.maxRange->y = std::max( m_lines.maxRange->y, line->range->y );
					++line->wordEnd;
					line->charEnd = word->charEnd;
				}
			};

			auto addChar = [&]( castor::Point2f charSize
				, castor::Point2f const & bearing )
			{
				auto xMin = bearing->x;
				auto xMax = xMin + charSize->x;
				auto yMin = -bearing->y;
				auto yMax = yMin + charSize->y;

				if ( m_wrappingMode == TextWrappingMode::eBreakWords
					&& wordLeft > 0.0
					&& ( wordLeft > overlaySize->x
						|| totalLeft + xMax > overlaySize->x ) )
				{
					// The word will overflow the overlay size.
					// So we jump to the next line,
					// and will write the word on this next line.
					alignLine();
					line = nextLine();
					line->charBegin = word->charBegin;
					word->left = wordLeft;
				}
				else if ( m_wrappingMode == TextWrappingMode::eBreak
					&& totalLeft + xMax > overlaySize->x )
				{
					// The char will overflow the overlay size.
					// So we write the current word,
					// jump to the next line,
					// then carry on the word on this next line.
					addWord();
					alignLine();
					wordLeft = totalLeft;
					++wordIndex;
					line = nextLine();
					word = nextWord();
				}

				// Setup char
				auto uvPosition = fontTexture->getGlyphPosition( *cit );
				auto & outChar = *tit;
				outChar.left = charLeft;
				outChar.size = charSize;
				outChar.bearing = bearing;
				outChar.uvPosition = { uvPosition.x(), uvPosition.y() };
				outChar.word = wordIndex;
				outChar.index = charIndex;

				// Complete word
				word->range->x = std::min( word->range->x, yMin );
				word->range->y = std::max( word->range->y, yMax );
				++word->charEnd;
			};

			while ( cit != caption.end() )
			{
				if ( *cit == U'\n' )
				{
					addWord();
					alignLine();
					line = nextLine();
					++wordIndex;
					word = nextWord();
				}
				else
				{
					castor::Glyph const & glyph{ font.getGlyphAt( *cit ) };

					if ( *cit == U' ' || *cit == U'\t' )
					{
						// write the word and leave space before next word.
						addWord();
						totalLeft += float( glyph.getAdvance() );
						wordLeft = totalLeft;
						charLeft = 0.0;
						++wordIndex;
						word = nextWord();
					}
					else
					{
						addChar( { glyph.getSize().getWidth(), glyph.getSize().getHeight() }
							, { glyph.getBearing().x(), glyph.getBearing().y() } );
						totalLeft += float( glyph.getAdvance() );
						charLeft += float( glyph.getAdvance() );
						++charIndex;
						++tit;
					}
				}

				++cit;
			}

			addWord();
			alignLine();
		}

		auto lines = m_lines.lines();

		if ( !lines.empty() )
		{
			if ( m_lineSpacingMode == TextLineSpacingMode::eMaxLineHeight )
			{
				// Adjust lines heights to maxHeight
				auto lineRange = m_lines.maxRange;
				auto lineHeight = lineRange->y - lineRange->x;
				lineTop = 0.0;

				for ( auto & line : lines )
				{
					line.position->y = lineTop;
					line.range = lineRange;
					lineTop += lineHeight;
				}
			}

			m_lines.topOffset = -lines.front().range->x;

			// Move lines according to valign
			if ( m_vAlign != VAlign::eTop )
			{
				auto offset = overlaySize->y - lineTop;

				if ( m_vAlign == VAlign::eCenter )
				{
					offset /= 2;
				}

				m_lines.topOffset += offset;
			}
		}
	}

	//*********************************************************************************************
}
