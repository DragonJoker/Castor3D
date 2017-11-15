#include "GuiCommonPrerequisites.hpp"

#if defined( CASTOR_PLATFORM_WINDOWS ) && !defined( NDEBUG ) && !defined( VLD_AVAILABLE )
#	define _CRTDBG_MAP_ALLOC
#	include <cstdlib>
#	include <crtdbg.h>
#elif defined( CASTOR_PLATFORM_LINUX )
#	include <gdk/gdkx.h>
#	include <gtk/gtk.h>
#	undef None
#	undef Bool
#	undef Always
using Bool = int;
#endif

#include <Graphics/Font.hpp>

#include <Engine.hpp>
#include <Event/Frame/FunctorEvent.hpp>
#include <Event/Frame/InitialiseEvent.hpp>
#include <Material/Material.hpp>
#include <Mesh/Mesh.hpp>
#include <Miscellaneous/PlatformWindowHandle.hpp>
#include <Plugin/Plugin.hpp>
#include <Render/RenderWindow.hpp>
#include <Scene/Scene.hpp>
#include <Scene/SceneFileParser.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureLayout.hpp>
#include <Texture/TextureUnit.hpp>

#include <Graphics/PixelBufferBase.hpp>

#include <wx/window.h>
#include <wx/rawbmp.h>

using namespace castor;
using namespace castor3d;

namespace GuiCommon
{
	namespace
	{
		struct wxWidgetsFontImpl
			: public castor::Font::SFontImpl
		{
			explicit wxWidgetsFontImpl( wxFont const & p_font )
				: m_font( p_font )
			{
			}

			virtual ~wxWidgetsFontImpl()
			{
			}

			virtual void initialise()
			{
			}

			virtual void cleanup()
			{
			}

			virtual void loadGlyph( Glyph & p_glyph )
			{
				//FT_Glyph ftGlyph;
				//CHECK_FT_ERR( FT_Load_Glyph, m_face, FT_get_Char_Index( m_face, p_glyph.getCharacter() ), FT_LOAD_DEFAULT );
				//CHECK_FT_ERR( FT_get_Glyph, m_face->glyph, &ftGlyph );
				//CHECK_FT_ERR( FT_Glyph_To_Bitmap, &ftGlyph, FT_RENDER_MODE_NORMAL, 0, 1 );
				//FT_BitmapGlyph ftBmpGlyph = FT_BitmapGlyph( ftGlyph );
				//FT_Bitmap & ftBitmap = ftBmpGlyph->bitmap;
				//Size advance( uint32_t( std::abs( ftGlyph->advance.x ) / 65536.0 ), uint32_t( ftGlyph->advance.y  / 65536.0 ) );
				//uint32_t pitch = std::abs( ftBitmap.pitch );
				//Size size( pitch, ftBitmap.rows );
				//Position ptPosition( ftBmpGlyph->left, ftBmpGlyph->top );
				//uint32_t uiSize = size.getWidth() * size.getHeight();
				//ByteArray bitmap( uiSize, 0 );
				//uint32_t index = 0;

				//if ( ftBitmap.pitch < 0 )
				//{
				//	uint8_t * dst = bitmap.data();
				//	uint8_t const * src = ftBitmap.buffer;

				//	for ( uint32_t i = 0; i < uint32_t( ftBitmap.rows ); i++ )
				//	{
				//		memcpy( dst, src, ftBitmap.width );
				//		src += pitch;
				//		dst += pitch;
				//	}
				//}
				//else
				//{
				//	uint8_t * dst = bitmap.data() + uiSize - pitch;
				//	uint8_t const * src = ftBitmap.buffer;

				//	for ( uint32_t i = 0; i < uint32_t( ftBitmap.rows ); i++ )
				//	{
				//		memcpy( dst, src, ftBitmap.width );
				//		src += pitch;
				//		dst -= pitch;
				//	}
				//}

				//p_glyph.setSize( size );
				//p_glyph.setPosition( ptPosition );
				//p_glyph.setAdvance( advance );
				//p_glyph.setBitmap( bitmap );
				//return ftBmpGlyph->top;
			}

			wxFont m_font;
		};
	}

	void CreateBitmapFromBuffer( uint8_t const * p_buffer, uint32_t p_width, uint32_t p_height, bool p_flip, wxBitmap & p_bitmap )
	{
		p_bitmap.Create( p_width, p_height, 24 );
		wxNativePixelData data( p_bitmap );

		if ( p_bitmap.IsOk() && uint32_t( data.GetWidth() ) == p_width && uint32_t( data.GetHeight() ) == p_height )
		{
			wxNativePixelData::Iterator it( data );

			try
			{
				if ( p_flip )
				{
					uint32_t pitch = p_width * 4;
					uint8_t const * buffer = p_buffer + ( p_height - 1 ) * pitch;

					for ( uint32_t i = 0; i < p_height && it.IsOk(); i++ )
					{
						uint8_t const * line = buffer;
#if defined( CASTOR_PLATFORM_WINDOWS )
						wxNativePixelData::Iterator rowStart = it;
#endif

						for ( uint32_t j = 0; j < p_width && it.IsOk(); j++ )
						{
							it.Red() = *line;
							line++;
							it.Green() = *line;
							line++;
							it.Blue() = *line;
							line++;
							// don't write the alpha.
							line++;
							it++;
						}

						buffer -= pitch;

#if defined( CASTOR_PLATFORM_WINDOWS )
						it = rowStart;
						it.OffsetY( data, 1 );
#endif
					}
				}
				else
				{
					uint8_t const * buffer = p_buffer;

					for ( uint32_t i = 0; i < p_height && it.IsOk(); i++ )
					{
#if defined( CASTOR_PLATFORM_WINDOWS )
						wxNativePixelData::Iterator rowStart = it;
#endif

						for ( uint32_t j = 0; j < p_width && it.IsOk(); j++ )
						{
							it.Red() = *buffer;
							buffer++;
							it.Green() = *buffer;
							buffer++;
							it.Blue() = *buffer;
							buffer++;
							// don't write the alpha.
							buffer++;
							it++;
						}

#if defined( CASTOR_PLATFORM_WINDOWS )
						it = rowStart;
						it.OffsetY( data, 1 );
#endif
					}
				}
			}
			catch ( ... )
			{
				Logger::logWarning( cuT( "CreateBitmapFromBuffer encountered an exception" ) );
			}
		}
	}

	void CreateBitmapFromBuffer( PxBufferBaseSPtr p_buffer, bool p_flip, wxBitmap & p_bitmap )
	{
		PxBufferBaseSPtr buffer;

		if ( p_buffer->format() != PixelFormat::eA8R8G8B8 )
		{
			buffer = PxBufferBase::create( Size( p_buffer->getWidth(), p_buffer->getHeight() ), PixelFormat::eA8R8G8B8, p_buffer->constPtr(), p_buffer->format() );
		}
		else
		{
			buffer = p_buffer;
		}

		CreateBitmapFromBuffer( buffer->constPtr(), buffer->getWidth(), buffer->getHeight(), p_flip, p_bitmap );
	}

	void CreateBitmapFromBuffer( TextureUnitSPtr p_pUnit, bool p_flip, wxBitmap & p_bitmap )
	{
		if ( p_pUnit->getTexture()->getImage().getBuffer() )
		{
			CreateBitmapFromBuffer( p_pUnit->getTexture()->getImage().getBuffer(), p_flip, p_bitmap );
		}
		else
		{
			Path path{ p_pUnit->getTexture()->getImage().toString() };

			if ( !path.empty() )
			{
				wxImageHandler * pHandler = wxImage::FindHandler( path.getExtension(), wxBITMAP_TYPE_ANY );

				if ( pHandler )
				{
					wxImage image;

					if ( image.LoadFile( path, pHandler->GetType() ) && image.IsOk() )
					{
						p_bitmap = wxBitmap( image );
					}
					else
					{
						Logger::logWarning( cuT( "CreateBitmapFromBuffer encountered a problem loading file [" ) + path + cuT( "]" ) );
					}
				}
				else
				{
					Logger::logWarning( cuT( "CreateBitmapFromBuffer encountered a problem loading file [" ) + path + cuT( "] : Unsupported format" ) );
				}
			}
		}
	}

	RenderWindowSPtr loadScene( Engine & engine, Path const & p_fileName, uint32_t p_wantedFps, bool p_threaded )
	{
		RenderWindowSPtr result;

		if ( File::fileExists( p_fileName ) )
		{
			Logger::logInfo( cuT( "Loading scene file : " ) + p_fileName );

			if ( p_fileName.getExtension() == cuT( "cscn" ) || p_fileName.getExtension() == cuT( "zip" ) )
			{
				try
				{
					SceneFileParser parser( engine );

					if ( parser.parseFile( p_fileName ) )
					{
						result = parser.getRenderWindow();
					}
					else
					{
						Logger::logWarning( cuT( "Can't read scene file" ) );
					}
				}
				catch ( std::exception & exc )
				{
					wxMessageBox( _( "Failed to parse the scene file, with following error:" ) + wxString( wxT( "\n" ) ) + wxString( exc.what(), wxMBConvLibc() ) );
				}
			}
		}
		else
		{
			wxMessageBox( _( "Scene file doesn't exist :" ) + wxString( wxT( "\n" ) ) + p_fileName );
		}

		return result;
	}

	void loadPlugins( castor3d::Engine & engine )
	{
		PathArray arrayFiles;
		File::listDirectoryFiles( Engine::getPluginsDirectory(), arrayFiles );
		PathArray arrayKept;

		// Exclude debug plug-in in release builds, and release plug-ins in debug builds
		for ( auto file : arrayFiles )
		{
#if defined( NDEBUG )

			if ( file.find( String( cuT( "d." ) ) + CASTOR_DLL_EXT ) == String::npos )
#else

			if ( file.find( String( cuT( "d." ) ) + CASTOR_DLL_EXT ) != String::npos )

#endif
			{
				arrayKept.push_back( file );
			}
		}

		if ( !arrayKept.empty() )
		{
			PathArray arrayFailed;
			PathArray otherPlugins;

			for ( auto file : arrayKept )
			{
				if ( file.getExtension() == CASTOR_DLL_EXT )
				{
					// Since techniques depend on renderers, we load these first
					if ( file.find( cuT( "RenderSystem" ) ) != String::npos )
					{
						if ( !engine.getPluginCache().loadPlugin( file ) )
						{
							arrayFailed.push_back( file );
						}
					}
					else
					{
						otherPlugins.push_back( file );
					}
				}
			}

			// Then we load other plug-ins
			for ( auto file : otherPlugins )
			{
				if ( !engine.getPluginCache().loadPlugin( file ) )
				{
					arrayFailed.push_back( file );
				}
			}

			if ( !arrayFailed.empty() )
			{
				Logger::logWarning( cuT( "Some plug-ins couldn't be loaded :" ) );

				for ( auto file : arrayFailed )
				{
					Logger::logWarning( Path( file ).getFileName() );
				}

				arrayFailed.clear();
			}
		}

		Logger::logInfo( cuT( "Plugins loaded" ) );
	}

	castor3d::WindowHandle makeWindowHandle( wxWindow * p_window )
	{
#if defined( CASTOR_PLATFORM_WINDOWS )

		return WindowHandle( std::make_shared< IMswWindowHandle >( p_window->GetHandle() ) );

#elif defined( CASTOR_PLATFORM_LINUX )

		GtkWidget * pGtkWidget = static_cast< GtkWidget * >( p_window->GetHandle() );
		GLXDrawable drawable = 0;
		Display * pDisplay = nullptr;

		if ( pGtkWidget && pGtkWidget->window )
		{
			drawable = GDK_WINDOW_XID( pGtkWidget->window );
			GdkDisplay * pGtkDisplay = gtk_widget_get_display( pGtkWidget );

			if ( pGtkDisplay )
			{
				pDisplay = gdk_x11_display_get_xdisplay( pGtkDisplay );
			}
		}

		return WindowHandle( std::make_shared< IXWindowHandle >( drawable, pDisplay ) );

#endif
	}

	FontSPtr make_Font( Engine * engine, wxFont const & p_font )
	{
		String name = make_String( p_font.GetFaceName() ) + string::toString( p_font.GetPointSize() );
		auto & cache = engine->getFontCache();
		FontSPtr font = cache.find( name );

		if ( !font )
		{
			if ( p_font.IsOk() )
			{
				//l_font = std::make_shared< castor::Font >( name, p_font.GetPointSize() );
				//l_font->setGlyphLoader( std::make_unique< wxWidgetsFontImpl >( p_font ) );
				//Font::BinaryLoader()( *font, String( p_font.GetFaceName() ), uint32_t( std::abs( p_font.GetPointSize() ) ) );
				//l_cache.insert( name, font );
			}
		}

		return font;
	}

	castor::String make_String( wxString const & p_value )
	{
		return castor::String( p_value.mb_str( wxConvUTF8 ).data() );
	}

	castor::Path make_Path( wxString const & p_value )
	{
		return castor::Path( p_value.mb_str( wxConvUTF8 ).data() );
	}

	wxString make_wxString( castor::String const & p_value )
	{
		return wxString( p_value.c_str(), wxConvUTF8 );
	}

	castor::Size makeSize( wxSize const & p_value )
	{
		return castor::Size( p_value.x, p_value.y );
	}

	wxSize make_wxSize( castor::Size const & p_value )
	{
		return wxSize( p_value.getWidth(), p_value.getHeight() );
	}
}
