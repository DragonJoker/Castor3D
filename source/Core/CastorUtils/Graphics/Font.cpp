#include "CastorUtils/Graphics/Font.hpp"

#include <msdfgen/msdfgen.h>
#include <msdfgen/msdfgen-ext.h>
#include <ft2build.h>

FT_BEGIN_HEADER
#	include FT_FREETYPE_H
#	include FT_GLYPH_H
FT_END_HEADER

CU_ImplementSmartPtr( castor, Font )

namespace castor
{
	namespace ft
	{
#define CHECK_FT_ERR( func, ... ) CheckErr( func( __VA_ARGS__ ), #func )

		static Map< FT_Error, MbString > const MapErrors
		{
			{ 0x0000, "Success" },
			{ 0x0001, "Invalid face handle" },
			{ 0x0002, "Invalid instance handle" },
			{ 0x0003, "Invalid glyph handle" },
			{ 0x0004, "Invalid charmap handle" },
			{ 0x0005, "Invalid result address" },
			{ 0x0006, "Invalid glyph index" },
			{ 0x0007, "Invalid argument" },
			{ 0x0008, "Could not open file" },
			{ 0x0009, "File is not collection" },
			{ 0x000A, "Table missing" },
			{ 0x000B, "Invalid horiz metrics" },
			{ 0x000C, "Invalid charmap format" },
			{ 0x000D, "Invalid ppem" },
			{ 0x0010, "Invalid file format" },
			{ 0x0020, "Invalid engine" },
			{ 0x0021, "Too many extensions" },
			{ 0x0022, "Extensions unsupported" },
			{ 0x0023, "Invalid extension id" },
			{ 0x0080, "Max profile missing" },
			{ 0x0081, "Header table missing" },
			{ 0x0082, "Horiz header missing" },
			{ 0x0083, "Locations missing" },
			{ 0x0084, "Name table missing" },
			{ 0x0085, "CMap table missing" },
			{ 0x0086, "Hmtx table missing" },
			{ 0x0087, "OS2 table missing" },
			{ 0x0088, "Post table missing" },
			{ 0x0100, "Out of memory" },
			{ 0x0200, "Invalid file offset" },
			{ 0x0201, "Invalid file read" },
			{ 0x0202, "Invalid frame access" },
			{ 0x0300, "Too many points" },
			{ 0x0301, "Too many contours" },
			{ 0x0302, "Invalid composite glyph" },
			{ 0x0303, "Too many instructions" },
			{ 0x0400, "Invalid opcode" },
			{ 0x0401, "Too few arguments" },
			{ 0x0402, "Stack overflow" },
			{ 0x0403, "Code overflow" },
			{ 0x0404, "Bad argument" },
			{ 0x0405, "Divide by zero" },
			{ 0x0406, "Storage overflow" },
			{ 0x0407, "Cvt overflow" },
			{ 0x0408, "Invalid reference" },
			{ 0x0409, "Invalid distance" },
			{ 0x040A, "Interpolate twilight" },
			{ 0x040B, "Debug opcode" },
			{ 0x040C, "ENDF in exec stream" },
			{ 0x040D, "Out of coderanges" },
			{ 0x040E, "Nested defs" },
			{ 0x040F, "Invalid coderange" },
			{ 0x0410, "Invalid displacement" },
			{ 0x0411, "Execution too long" },
			{ 0x0500, "Nested frame access" },
			{ 0x0501, "Invalid cache list" },
			{ 0x0502, "Could not find context" },
			{ 0x0503, "Unlisted object" },
			{ 0x0600, "Raster pool overflow" },
			{ 0x0601, "Raster negative height" },
			{ 0x0602, "Raster invalid value" },
			{ 0x0603, "Raster not initialized" },
			{ 0x0A00, "Invalid kerning table format" },
			{ 0x0A01, "Invalid kerning table" },
			{ 0x0B00, "Invalid post table format" },
			{ 0x0B01, "Invalid post table" },
		};

		inline bool CheckErr( FT_Error error, const char * name )
		{
			bool result = error == 0;

			if ( !result )
			{
				auto it = MapErrors.find( error );
				auto err = "ERROR : " + MbString( name ) + " failed - " + toUtf8( string::toString( error ) );

				if ( it != MapErrors.end() )
				{
					err += " (" + it->second + ")";
				}

				CU_LoaderError( err );
			}

			return result;
		}

		struct GlyphLoader
			: public Font::GlyphLoader
		{
			static float constexpr f26dot6ToFloat = 1.0f / 64.0f;

			GlyphLoader( Path const & pathFile, uint32_t height )
				: m_path{ pathFile }
				, m_height{ height }
			{
			}

			void initialise()override
			{
				CHECK_FT_ERR( FT_Init_FreeType, &m_library );
				CHECK_FT_ERR( FT_New_Face, m_library, toUtf8( m_path ).c_str(), 0, &m_face );
				CHECK_FT_ERR( FT_Select_Charmap, m_face, FT_ENCODING_UNICODE );
				CHECK_FT_ERR( FT_Set_Pixel_Sizes, m_face, 0, m_height );
			}

			void cleanup()override
			{
				CHECK_FT_ERR( FT_Done_Face, m_face );
				CHECK_FT_ERR( FT_Done_FreeType, m_library );
				m_library = nullptr;
				m_face = nullptr;
			}

			Glyph loadGlyph( char32_t c32 )override
			{
				FT_Glyph glyph{};
				FT_ULong const cl( c32 );
				FT_UInt const index{ FT_Get_Char_Index( m_face, cl ) };
				CHECK_FT_ERR( FT_Load_Glyph, m_face, index, FT_LOAD_DEFAULT );
				auto glyphSlot = m_face->glyph;
				CHECK_FT_ERR( FT_Render_Glyph, glyphSlot, FT_RENDER_MODE_NORMAL );
				FT_Bitmap const & bitmap = glyphSlot->bitmap;
				auto const pitch( uint32_t( std::abs( bitmap.pitch ) ) );
				ByteArray buffer( size_t( bitmap.width ) * size_t( bitmap.rows ) );

				if ( bitmap.pitch < 0 )
				{
					uint8_t * dst = buffer.data();
					uint8_t const * src = bitmap.buffer;

					for ( uint32_t i = 0; i < uint32_t( bitmap.rows ); i++ )
					{
						memcpy( dst, src, bitmap.width );
						src += pitch;
						dst += pitch;
					}
				}
				else
				{
					uint8_t * dst = buffer.data() + buffer.size() - pitch;
					uint8_t const * src = bitmap.buffer;

					for ( uint32_t i = 0; i < uint32_t( bitmap.rows ); i++ )
					{
						memcpy( dst, src, bitmap.width );
						src += pitch;
						dst -= pitch;
					}
				}

				FT_Done_Glyph( glyph );
				return Glyph{ c32
					, { float( glyphSlot->metrics.width ) * f26dot6ToFloat, float( glyphSlot->metrics.height ) * f26dot6ToFloat }
					, { glyphSlot->bitmap_left, glyphSlot->bitmap_top }
					, float( glyphSlot->advance.x ) * f26dot6ToFloat
					, { bitmap.width, bitmap.rows }
					, buffer };
			}

			void fillKerningTable( char32_t c32
				, Font::GlyphArray const & glyphs
				, Font::GlyphKerningMap & table )override
			{
				if ( FT_HAS_KERNING( m_face ) )
				{
					// First complete the existing kerning tables agains this glyph.
					for ( auto & [l32, tab] : table )
					{
						FT_UInt const lhs{ FT_Get_Char_Index( m_face, FT_ULong( l32 ) ) };
						FT_UInt const rhs{ FT_Get_Char_Index( m_face, FT_ULong( c32 ) ) };
						FT_Vector result{};
						CHECK_FT_ERR( FT_Get_Kerning, m_face, lhs, rhs, FT_KERNING_UNSCALED, &result );
						tab.try_emplace( c32, float( result.x ) * f26dot6ToFloat );
					}

					// Then create kerning table for this glyph against all currently loaded ones.
					FT_UInt const lhs{ FT_Get_Char_Index( m_face, FT_ULong( c32 ) ) };
					Font::GlyphKerning lhsKerning;

					for ( auto & glyph : glyphs )
					{
						FT_UInt const rhs{ FT_Get_Char_Index( m_face, FT_ULong( glyph.getCharacter() ) ) };
						FT_Vector result{};
						CHECK_FT_ERR( FT_Get_Kerning, m_face, lhs, rhs, FT_KERNING_UNSCALED, &result );
						lhsKerning.try_emplace( glyph.getCharacter(), float( result.x ) * f26dot6ToFloat );
					}

					table.emplace( c32, lhsKerning );
				}
			}

		private:
			Path const m_path;
			uint32_t const m_height;
			FT_Library m_library{};
			FT_Face m_face{};
		};
	}

	namespace msdf
	{
		struct GlyphLoader
			: public Font::GlyphLoader
		{
			GlyphLoader( Path const & pathFile )
				: m_path{ pathFile }
			{
			}

			void initialise()override
			{
				m_freetype = msdfgen::initializeFreetype();

				if ( m_freetype )
				{
					m_font = msdfgen::loadFont( m_freetype, toUtf8( m_path ).c_str() );

					if ( !m_font )
					{
						CU_LoaderError( "Couldn't load the font" );
					}

					if ( !msdfgen::getFontMetrics( m_metrics, m_font ) )
					{
						CU_LoaderError( "Couldn't retrieve the font metrics" );
					}
					else
					{
						m_sdfInfo.emSize = uint32_t( m_metrics.emSize );
					}

					m_sdfInfo.verticalAdvance = float( m_metrics.ascenderY - m_metrics.descenderY );
					m_sdfInfo.pixelRange = float( m_sdfInfo.emSize ) * 0.2f;
				}
				else
				{
					CU_LoaderError( "Couldn't initialise Freetype" );
				}
			}

			void cleanup()override
			{
				msdfgen::destroyFont( m_font );
				msdfgen::deinitializeFreetype( m_freetype );
				m_freetype = nullptr;
			}

			Glyph loadGlyph( char32_t c32 )override
			{
				msdfgen::Shape shape{};
				auto pixelSize = 4u * sizeof( float );
				ByteArray buffer( ( m_sdfInfo.emSize + 2u ) * ( m_sdfInfo.emSize + 2u ) * pixelSize );
				double advance{};
				double width{};
				double height{};
				msdfgen::Vector2 translate{};

				if ( msdfgen::loadGlyph( shape, m_font, c32, &advance ) )
				{
					shape.normalize();
					auto bounds = shape.getBounds();
					double l = bounds.l;
					double b = bounds.b;
					double r = bounds.r;
					double t = bounds.t;
					width = r - l;
					height = t - b;

					if ( c32 == ' ' || c32 == '\t' )
					{
						height = 0;
						double space = {};
						double tab = {};
						msdfgen::getFontWhitespaceWidth( space, tab, m_font );

						if ( c32 == ' ' )
						{
							width = float( space );
						}
						else
						{
							width = float( tab );
						}
					}
					else
					{
						if ( std::isinf( width ) )
						{
							width = float( m_sdfInfo.emSize );
						}

						if ( std::isinf( height ) )
						{
							height = float( m_sdfInfo.emSize );
						}
					}

					translate.x = -l + 0.5 * ( m_sdfInfo.emSize - width );
					translate.y = -b + 0.5 * ( m_sdfInfo.emSize - height );
					msdfgen::edgeColoringSimple( shape, 3.0f );
					msdfgen::Bitmap< float, 4u > msdf{ int( m_sdfInfo.emSize ), int( m_sdfInfo.emSize ) };
					msdfgen::generateMTSDF( msdf
						, shape
						, msdfgen::Projection{ msdfgen::Vector2{ 1.0, 1.0 }, translate }
						, m_sdfInfo.pixelRange );
					auto * dst = buffer.data();
					auto const * src = msdf( 0, 0 );
					auto srcLineSize = m_sdfInfo.emSize * pixelSize;
					auto dstLineSize = pixelSize + srcLineSize + pixelSize;
					// First line is empty
					std::memset( dst, 0u, dstLineSize );
					dst += dstLineSize;

					for ( size_t line = 0; line < m_sdfInfo.emSize; ++line )
					{
						// First pixel of the line is empty.
						std::memset( dst, 0u, pixelSize );
						dst += pixelSize;
						std::memcpy( dst, src, srcLineSize );
						dst += srcLineSize;
						// Last pixel of the line is empty.
						std::memset( dst, 0u, pixelSize );
						dst += pixelSize;
						src += 4 * m_sdfInfo.emSize;
					}

					// Last line is empty
					std::memset( dst, 0u, dstLineSize );
				}

				return Glyph{ c32
					, { width, height }
					, { -translate.x, -translate.y }
					, float( advance )
					, Size{ m_sdfInfo.emSize + 2u, m_sdfInfo.emSize + 2u }
					, buffer };
			}

			void fillKerningTable( char32_t c32
				, Font::GlyphArray const & glyphs
				, Font::GlyphKerningMap & table )override
			{
				// First complete the existing kerning tables agains this glyph.
				for ( auto & [l32, tab] : table )
				{
					double result{};
					msdfgen::getKerning( result, m_font, l32, c32 );
					tab.try_emplace( c32, float( result ) );
				}

				// Then create kerning table for this glyph against all currently loaded ones.
				Font::GlyphKerning lhsKerning;

				for ( auto & glyph : glyphs )
				{
					double result{};
					auto r32 = glyph.getCharacter();
					msdfgen::getKerning( result, m_font, c32, r32 );
					lhsKerning.try_emplace( glyph.getCharacter(), float( result ) );
				}

				table.emplace( c32, lhsKerning );
			}

			Font::SdfInfo const & getSdfInfo()const noexcept
			{
				return m_sdfInfo;
			}

		private:
			Path const m_path;
			Font::SdfInfo m_sdfInfo{ .emSize = 32u, .pixelRange = 4.0f };
			msdfgen::FreetypeHandle * m_freetype{};
			msdfgen::FontHandle * m_font{};
			msdfgen::FontMetrics m_metrics{};
		};
	}

	//************************************************************************************************

	bool Font::BinaryLoader::operator()( Font & font
		, Path const & pathFile
		, uint32_t height )
	{
		m_height = height;
		return doLoad( font, pathFile );
	}

	bool Font::BinaryLoader::operator()( Font & font
		, Path const & pathFile )
	{
		return doLoad( font, pathFile );
	}

	bool Font::BinaryLoader::doLoad( Font & font
		, Path const & pathFile )
	{
		bool result = false;

		if ( !pathFile.empty() )
		{
			String strFontName = pathFile.getFullFileName();

			try
			{
				if ( !font.hasGlyphLoader() )
				{
					if ( font.isSDF() )
					{
						auto loader = castor::make_unique< msdf::GlyphLoader >( pathFile );
						font.m_sdfInfo = loader->getSdfInfo();
						font.setGlyphLoader( std::move( loader ) );
					}
					else
					{
						font.setGlyphLoader( castor::make_unique< ft::GlyphLoader >( pathFile, m_height ) );
					}
				}

				font.setFaceName( pathFile.getFileName() );
				font.getGlyphLoader().initialise();
				auto constexpr min = std::numeric_limits< uint8_t >::lowest();
				auto constexpr max = std::numeric_limits< uint8_t >::max();

				// We load the glyphs
				for ( uint8_t c = min; c < max; c++ )
				{
					Array< char, 4u > tmp{ char( c ), 0, 0, 0 };
					font.doLoadGlyph( string::utf8::toUtf8( tmp.data(), tmp.data() + tmp.size() ) );
				}

				font.getGlyphLoader().cleanup();
				result = true;
			}
			catch ( std::runtime_error & exc )
			{
				CU_LoaderError( "Font loading failed : " + MbString( exc.what() ) );
			}
		}

		return result;
	}

	//*********************************************************************************************

	Font::Font( String const & name, uint32_t height )
		: Named{ name }
		, m_height{ height }
	{
	}

	Font::Font( String const & name, uint32_t height, Path const & path )
		: Named{ name }
		, m_height{ height }
		, m_pathFile{ path }
		, m_glyphLoader{ castor::make_unique< ft::GlyphLoader >( path, height ) }
	{
		BinaryLoader{}( *this, path, height );
	}

	Font::Font( String const & name )
		: Named{ name }
		, m_sdf{ true }
	{
	}

	Font::Font( String const & name, Path const & path )
		: Named{ name }
		, m_pathFile{ path }
		, m_glyphLoader{ castor::make_unique< msdf::GlyphLoader >( path ) }
		, m_sdf{ true }
	{
		m_glyphLoader->initialise();
		auto & loader = static_cast< msdf::GlyphLoader const & >( *m_glyphLoader );
		m_sdfInfo = loader.getSdfInfo();
		m_glyphLoader->cleanup();
		BinaryLoader{}( *this, path, 0u );
	}

	void Font::loadGlyph( char32_t c32 )
	{
		m_glyphLoader->initialise();
		doLoadGlyph( c32 );
		m_glyphLoader->cleanup();
	}

	float Font::getKerning( char32_t lhs, char32_t rhs, uint32_t height )const
	{
		auto tit = m_kerningTable.find( lhs );
		float result{};

		if ( tit != m_kerningTable.end() )
		{
			auto it = tit->second.find( rhs );

			if ( it != tit->second.end() )
			{
				result = it->second;
			}
		}

		if ( isSDF() && result != 0.0f )
		{
			result *= float( height );
			result /= float( m_sdfInfo.emSize );
		}

		return result;
	}

	float Font::getKerning( char32_t lhs, char32_t rhs )const
	{
		return getKerning( lhs, rhs, getHeight() );
	}

	TextMetrics Font::getTextMetrics( std::u32string const & v
		, uint32_t maxWidth
		, bool splitLines
		, uint32_t height )const
	{
		TextMetrics result;
		uint32_t charIndex{};
		float charLeft{};
		float wordLeft{};
		float totalLeft{};
		TextLineMetrics word;
		char32_t previous{};
		auto ratio = m_sdf
			? float( height ) / float( getMaxImageHeight() )
			: 1.0f;

		auto nextLine = [&]()
			{
				if ( !word.chars.empty() )
				{
					for ( auto & c : word.chars )
					{
						c -= wordLeft;
					}
				}

				auto & line = result.lines.emplace_back();
				line.firstCharIndex = charIndex;
				line.top = result.height;
				charLeft = totalLeft - wordLeft;
				totalLeft = charLeft;
				line.width = totalLeft;
				wordLeft = 0;
				return &line;
			};

		auto line = nextLine();

		auto finishLine = [&]()
			{
				if ( !line->chars.empty() )
				{
					result.yMin = std::min( result.yMin, line->yMin );
					result.yMax = std::max( result.yMax, line->yMax );

					result.height += line->yMax - line->yMin;
					result.width = std::max( result.width, line->width );
				}
				else
				{
					result.height += float( height );
				}
			};

		auto finishWord = [&]()
			{
				if ( !word.chars.empty() )
				{
					line->yMin = std::min( line->yMin, word.yMin );
					line->yMax = std::max( line->yMax, word.yMax );

					line->chars.insert( line->chars.end()
						, word.chars.begin()
						, word.chars.end() );
				}

				wordLeft = totalLeft;
			};

		auto addChar = [&]( Point2f const & charSize
				, Point2f const & bearing
				, float advance )
			{
				auto xMin = bearing->x * ratio;
				auto xMax = xMin + advance;
				auto yMin = -bearing->y * ratio;
				auto yMax = yMin + charSize->y * ratio;

				if ( splitLines
					&& wordLeft > 0
					&& ( wordLeft > float( maxWidth )
						|| totalLeft + xMax > float( maxWidth ) ) )
				{
					finishLine();
					line = nextLine();
				}

				word.yMin = std::min( word.yMin, yMin );
				word.yMax = std::max( word.yMax, yMax );
			};

		for ( auto c : v )
		{
			if ( c == U'\n' )
			{
				finishWord();
				finishLine();
				line = nextLine();
				charLeft = 0;
				word.chars.push_back( 0u );
			}
			else
			{
				auto & glyph = getGlyphAt( c );
				auto advance = glyph.getAdvance() * ratio;

				if ( c == U' ' || c == U'\t' )
				{
					totalLeft += advance;
					finishWord();
					charLeft = 0;
					line->width += advance;
				}
				else
				{
					if ( previous != char32_t{} )
					{
						auto kerning = getKerning( previous, c, height );
						charLeft += kerning;
						totalLeft += kerning;
					}

					if ( m_sdf )
					{
						addChar( { float( glyph.getBitmapSize()->x ), glyph.getBitmapSize()->y }
							, glyph.getBearing()
							, advance );
					}
					else
					{
						addChar( glyph.getSize()
							, glyph.getBearing()
							, glyph.getSize()->x * ratio );
					}

					totalLeft += advance;
					charLeft += advance;
					word.width += advance;
					line->width += advance;
				}

				word.chars.push_back( totalLeft );
			}

			previous = c;
			++charIndex;
		}

		finishWord();
		finishLine();

		if ( !result.lines.empty() )
		{
			// Adjust lines heights to maxHeight
			auto lineMin = result.yMin;
			auto lineMax = result.yMax;
			auto lineHeight = lineMax - lineMin;
			result.height = 0;

			for ( auto & ln : result.lines )
			{
				ln.top = result.height;
				ln.yMin = lineMin;
				ln.yMax = lineMax;
				result.height += lineHeight;
			}
		}

		return result;
	}

	TextMetrics Font::getTextMetrics( std::u32string const & v
		, uint32_t maxWidth
		, bool splitLines )const
	{
		return getTextMetrics( v, maxWidth, splitLines, getHeight() );
	}

	Glyph const & Font::doLoadGlyph( char32_t c )
	{
		auto it = std::find_if( m_loadedGlyphs.begin()
			, m_loadedGlyphs.end()
			, [c]( Glyph const & lookup )
		{
			return lookup.getCharacter() == c;
		} );

		if ( it == m_loadedGlyphs.end() )
		{
			m_loadedGlyphs.push_back( m_glyphLoader->loadGlyph( c ) );
			it = std::next( m_loadedGlyphs.begin()
				, ptrdiff_t( m_loadedGlyphs.size() - 1u ) );
			m_maxSize->x = std::max( m_maxSize->x, it->getSize()->x );
			m_maxSize->y = std::max( m_maxSize->y, it->getSize()->y );
			m_maxImageSize->x = std::max( m_maxImageSize->x, int32_t( it->getBitmapSize()->x ) );
			m_maxImageSize->y = std::max( m_maxImageSize->y, int32_t( it->getBitmapSize()->y ) );
			m_maxBearing->x = std::min( m_maxBearing->x, it->getBearing()->x );
			m_maxBearing->y = std::max( m_maxBearing->y, it->getBearing()->y );
			m_glyphLoader->fillKerningTable( c, m_loadedGlyphs, m_kerningTable );
		}

		return *it;
	}
}
