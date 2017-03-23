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

using namespace Castor;
using namespace Castor3D;

namespace GuiCommon
{
	namespace
	{
		struct wxWidgetsFontImpl
			: public Castor::Font::SFontImpl
		{
			wxWidgetsFontImpl( wxFont const & p_font )
				: m_font( p_font )
			{
			}

			virtual ~wxWidgetsFontImpl()
			{
			}

			virtual void Initialise()
			{
			}

			virtual void Cleanup()
			{
			}

			virtual void LoadGlyph( Glyph & p_glyph )
			{
				//FT_Glyph l_ftGlyph;
				//CHECK_FT_ERR( FT_Load_Glyph, m_face, FT_Get_Char_Index( m_face, p_glyph.GetCharacter() ), FT_LOAD_DEFAULT );
				//CHECK_FT_ERR( FT_Get_Glyph, m_face->glyph, &l_ftGlyph );
				//CHECK_FT_ERR( FT_Glyph_To_Bitmap, &l_ftGlyph, FT_RENDER_MODE_NORMAL, 0, 1 );
				//FT_BitmapGlyph l_ftBmpGlyph = FT_BitmapGlyph( l_ftGlyph );
				//FT_Bitmap & l_ftBitmap = l_ftBmpGlyph->bitmap;
				//Size l_advance( uint32_t( std::abs( l_ftGlyph->advance.x ) / 65536.0 ), uint32_t( l_ftGlyph->advance.y  / 65536.0 ) );
				//uint32_t l_pitch = std::abs( l_ftBitmap.pitch );
				//Size l_size( l_pitch, l_ftBitmap.rows );
				//Position l_ptPosition( l_ftBmpGlyph->left, l_ftBmpGlyph->top );
				//uint32_t l_uiSize = l_size.width() * l_size.height();
				//ByteArray l_bitmap( l_uiSize, 0 );
				//uint32_t l_index = 0;

				//if ( l_ftBitmap.pitch < 0 )
				//{
				//	uint8_t * l_dst = l_bitmap.data();
				//	uint8_t const * l_src = l_ftBitmap.buffer;

				//	for ( uint32_t i = 0; i < uint32_t( l_ftBitmap.rows ); i++ )
				//	{
				//		memcpy( l_dst, l_src, l_ftBitmap.width );
				//		l_src += l_pitch;
				//		l_dst += l_pitch;
				//	}
				//}
				//else
				//{
				//	uint8_t * l_dst = l_bitmap.data() + l_uiSize - l_pitch;
				//	uint8_t const * l_src = l_ftBitmap.buffer;

				//	for ( uint32_t i = 0; i < uint32_t( l_ftBitmap.rows ); i++ )
				//	{
				//		memcpy( l_dst, l_src, l_ftBitmap.width );
				//		l_src += l_pitch;
				//		l_dst -= l_pitch;
				//	}
				//}

				//p_glyph.SetSize( l_size );
				//p_glyph.SetPosition( l_ptPosition );
				//p_glyph.SetAdvance( l_advance );
				//p_glyph.SetBitmap( l_bitmap );
				//return l_ftBmpGlyph->top;
			}

			wxFont m_font;
		};
	}

	void CreateBitmapFromBuffer( uint8_t const * p_buffer, uint32_t p_width, uint32_t p_height, bool p_flip, wxBitmap & p_bitmap )
	{
		p_bitmap.Create( p_width, p_height, 24 );
		wxNativePixelData l_data( p_bitmap );

		if ( p_bitmap.IsOk() && uint32_t( l_data.GetWidth() ) == p_width && uint32_t( l_data.GetHeight() ) == p_height )
		{
			wxNativePixelData::Iterator l_it( l_data );

			try
			{
				if ( p_flip )
				{
					uint32_t l_pitch = p_width * 4;
					uint8_t const * l_buffer = p_buffer + ( p_height - 1 ) * l_pitch;

					for ( uint32_t i = 0; i < p_height && l_it.IsOk(); i++ )
					{
						uint8_t const * l_line = l_buffer;
#if defined( CASTOR_PLATFORM_WINDOWS )
						wxNativePixelData::Iterator l_rowStart = l_it;
#endif

						for ( uint32_t j = 0; j < p_width && l_it.IsOk(); j++ )
						{
							l_it.Red() = *l_line;
							l_line++;
							l_it.Green() = *l_line;
							l_line++;
							l_it.Blue() = *l_line;
							l_line++;
							// Don't write the alpha.
							l_line++;
							l_it++;
						}

						l_buffer -= l_pitch;

#if defined( CASTOR_PLATFORM_WINDOWS )
						l_it = l_rowStart;
						l_it.OffsetY( l_data, 1 );
#endif
					}
				}
				else
				{
					uint8_t const * l_buffer = p_buffer;

					for ( uint32_t i = 0; i < p_height && l_it.IsOk(); i++ )
					{
#if defined( CASTOR_PLATFORM_WINDOWS )
						wxNativePixelData::Iterator l_rowStart = l_it;
#endif

						for ( uint32_t j = 0; j < p_width && l_it.IsOk(); j++ )
						{
							l_it.Red() = *l_buffer;
							l_buffer++;
							l_it.Green() = *l_buffer;
							l_buffer++;
							l_it.Blue() = *l_buffer;
							l_buffer++;
							// Don't write the alpha.
							l_buffer++;
							l_it++;
						}

#if defined( CASTOR_PLATFORM_WINDOWS )
						l_it = l_rowStart;
						l_it.OffsetY( l_data, 1 );
#endif
					}
				}
			}
			catch ( ... )
			{
				Logger::LogWarning( cuT( "CreateBitmapFromBuffer encountered an exception" ) );
			}
		}
	}

	void CreateBitmapFromBuffer( PxBufferBaseSPtr p_buffer, bool p_flip, wxBitmap & p_bitmap )
	{
		PxBufferBaseSPtr l_buffer;

		if ( p_buffer->format() != PixelFormat::eA8R8G8B8 )
		{
			l_buffer = PxBufferBase::create( Size( p_buffer->width(), p_buffer->height() ), PixelFormat::eA8R8G8B8, p_buffer->const_ptr(), p_buffer->format() );
		}
		else
		{
			l_buffer = p_buffer;
		}

		CreateBitmapFromBuffer( l_buffer->const_ptr(), l_buffer->width(), l_buffer->height(), p_flip, p_bitmap );
	}

	void CreateBitmapFromBuffer( TextureUnitSPtr p_pUnit, bool p_flip, wxBitmap & p_bitmap )
	{
		if ( p_pUnit->GetTexture()->GetImage().GetBuffer() )
		{
			CreateBitmapFromBuffer( p_pUnit->GetTexture()->GetImage().GetBuffer(), p_flip, p_bitmap );
		}
		else
		{
			Path l_path{ p_pUnit->GetTexture()->GetImage().ToString() };

			if ( !l_path.empty() )
			{
				wxImageHandler * l_pHandler = wxImage::FindHandler( l_path.GetExtension(), wxBITMAP_TYPE_ANY );

				if ( l_pHandler )
				{
					wxImage l_image;

					if ( l_image.LoadFile( l_path, l_pHandler->GetType() ) && l_image.IsOk() )
					{
						p_bitmap = wxBitmap( l_image );
					}
					else
					{
						Logger::LogWarning( cuT( "CreateBitmapFromBuffer encountered a problem loading file [" ) + l_path + cuT( "]" ) );
					}
				}
				else
				{
					Logger::LogWarning( cuT( "CreateBitmapFromBuffer encountered a problem loading file [" ) + l_path + cuT( "] : Unsupported format" ) );
				}
			}
		}
	}

	RenderWindowSPtr LoadScene( Engine & p_engine, Path const & p_fileName, uint32_t p_wantedFps, bool p_threaded )
	{
		RenderWindowSPtr l_return;

		if ( File::FileExists( p_fileName ) )
		{
			Logger::LogInfo( cuT( "Loading scene file : " ) + p_fileName );

			if ( p_fileName.GetExtension() == cuT( "cscn" ) || p_fileName.GetExtension() == cuT( "zip" ) )
			{
				try
				{
					SceneFileParser l_parser( p_engine );

					if ( l_parser.ParseFile( p_fileName ) )
					{
						l_return = l_parser.GetRenderWindow();
					}
					else
					{
						Logger::LogWarning( cuT( "Can't read scene file" ) );
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

		return l_return;
	}

	void LoadPlugins( Castor3D::Engine & p_engine )
	{
		PathArray l_arrayFiles;
		File::ListDirectoryFiles( Engine::GetPluginsDirectory(), l_arrayFiles );
		PathArray l_arrayKept;

		// Exclude debug plug-in in release builds, and release plug-ins in debug builds
		for ( auto l_file : l_arrayFiles )
		{
#if defined( NDEBUG )

			if ( l_file.find( String( cuT( "d." ) ) + CASTOR_DLL_EXT ) == String::npos )
#else

			if ( l_file.find( String( cuT( "d." ) ) + CASTOR_DLL_EXT ) != String::npos )

#endif
			{
				l_arrayKept.push_back( l_file );
			}
		}

		if ( !l_arrayKept.empty() )
		{
			PathArray l_arrayFailed;
			PathArray l_otherPlugins;

			for ( auto l_file : l_arrayKept )
			{
				if ( l_file.GetExtension() == CASTOR_DLL_EXT )
				{
					// Since techniques depend on renderers, we load these first
					if ( l_file.find( cuT( "RenderSystem" ) ) != String::npos )
					{
						if ( !p_engine.GetPluginCache().LoadPlugin( l_file ) )
						{
							l_arrayFailed.push_back( l_file );
						}
					}
					else
					{
						l_otherPlugins.push_back( l_file );
					}
				}
			}

			// Then we load other plug-ins
			for ( auto l_file : l_otherPlugins )
			{
				if ( !p_engine.GetPluginCache().LoadPlugin( l_file ) )
				{
					l_arrayFailed.push_back( l_file );
				}
			}

			if ( !l_arrayFailed.empty() )
			{
				Logger::LogWarning( cuT( "Some plug-ins couldn't be loaded :" ) );

				for ( auto l_file : l_arrayFailed )
				{
					Logger::LogWarning( Path( l_file ).GetFileName() );
				}

				l_arrayFailed.clear();
			}
		}

		Logger::LogInfo( cuT( "Plugins loaded" ) );
	}

	Castor3D::WindowHandle make_WindowHandle( wxWindow * p_window )
	{
#if defined( CASTOR_PLATFORM_WINDOWS )

		return WindowHandle( std::make_shared< IMswWindowHandle >( p_window->GetHandle() ) );

#elif defined( CASTOR_PLATFORM_LINUX )

		GtkWidget * l_pGtkWidget = static_cast< GtkWidget * >( p_window->GetHandle() );
		GLXDrawable l_drawable = 0;
		Display * l_pDisplay = nullptr;

		if ( l_pGtkWidget && l_pGtkWidget->window )
		{
			l_drawable = GDK_WINDOW_XID( l_pGtkWidget->window );
			GdkDisplay * l_pGtkDisplay = gtk_widget_get_display( l_pGtkWidget );

			if ( l_pGtkDisplay )
			{
				l_pDisplay = gdk_x11_display_get_xdisplay( l_pGtkDisplay );
			}
		}

		return WindowHandle( std::make_shared< IXWindowHandle >( l_drawable, l_pDisplay ) );

#endif
	}

	FontSPtr make_Font( Engine * p_engine, wxFont const & p_font )
	{
		String l_name = make_String( p_font.GetFaceName() ) + string::to_string( p_font.GetPointSize() );
		auto & l_cache = p_engine->GetFontCache();
		FontSPtr l_font = l_cache.Find( l_name );

		if ( !l_font )
		{
			if ( p_font.IsOk() )
			{
				//l_font = std::make_shared< Castor::Font >( l_name, p_font.GetPointSize() );
				//l_font->SetGlyphLoader( std::make_unique< wxWidgetsFontImpl >( p_font ) );
				//Font::BinaryLoader()( *l_font, String( p_font.GetFaceName() ), uint32_t( std::abs( p_font.GetPointSize() ) ) );
				//l_cache.insert( l_name, l_font );
			}
		}

		return l_font;
	}

	Castor::String make_String( wxString const & p_value )
	{
		return Castor::String( p_value.mb_str( wxConvUTF8 ).data() );
	}

	Castor::Path make_Path( wxString const & p_value )
	{
		return Castor::Path( p_value.mb_str( wxConvUTF8 ).data() );
	}

	wxString make_wxString( Castor::String const & p_value )
	{
		return wxString( p_value.c_str(), wxConvUTF8 );
	}

	Castor::Size make_Size( wxSize const & p_value )
	{
		return Castor::Size( p_value.x, p_value.y );
	}

	wxSize make_wxSize( Castor::Size const & p_value )
	{
		return wxSize( p_value.width(), p_value.height() );
	}
}
