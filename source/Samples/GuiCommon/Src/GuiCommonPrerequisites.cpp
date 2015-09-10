#include "GuiCommonPrerequisites.hpp"

#if defined( _MSC_VER ) && !defined( NDEBUG ) && !defined( VLD_AVAILABLE )
#	define _CRTDBG_MAP_ALLOC
#	include <cstdlib>
#	include <crtdbg.h>
#elif defined( __linux__ )
#	include <gdk/gdkx.h>
#	include <gtk/gtk.h>
#endif

#include <Font.hpp>
#include <PlatformWindowHandle.hpp>

#include <wx/window.h>

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

			virtual int LoadGlyph( Glyph & p_glyph )
			{
				return 0;
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
				//uint32_t l_uiIndex = 0;
				//CASTOR_ASSERT( l_uiSize == l_size.width() * l_size.height() );

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

	void CreateBitmapFromBuffer( uint8_t const * p_pBuffer, uint32_t p_uiWidth, uint32_t p_uiHeight, wxBitmap & p_bitmap )
	{
		p_bitmap.Create( p_uiWidth, p_uiHeight, 24 );
		wxNativePixelData l_data( p_bitmap );

		if ( p_bitmap.IsOk() && uint32_t( l_data.GetWidth() ) == p_uiWidth && uint32_t( l_data.GetHeight() ) == p_uiHeight )
		{
			wxNativePixelData::Iterator l_it( l_data );
			uint8_t const * l_pBuffer = p_pBuffer;

			try
			{
				for ( uint32_t i = 0; i < p_uiHeight && l_it.IsOk(); i++ )
				{
#if defined( _WIN32 )
					wxNativePixelData::Iterator l_rowStart = l_it;
#endif

					for ( uint32_t j = 0; j < p_uiWidth && l_it.IsOk(); j++ )
					{
						l_it.Blue() = *l_pBuffer;
						l_pBuffer++;
						l_it.Green() = *l_pBuffer;
						l_pBuffer++;
						l_it.Red() = *l_pBuffer;
						l_pBuffer++;
						l_pBuffer++;
						l_it++;
					}

#if defined( _WIN32 )
					l_it = l_rowStart;
					l_it.OffsetY( l_data, 1 );
#endif
				}
			}
			catch ( ... )
			{
				Logger::LogWarning( cuT( "CreateBitmapFromBuffer encountered an exception" ) );
			}
		}
	}

	void CreateBitmapFromBuffer( TextureUnitSPtr p_pUnit, wxBitmap & p_bitmap )
	{
		if ( p_pUnit->GetImageBuffer() )
		{
			PxBufferBaseSPtr l_pBuffer = PxBufferBase::create( Size( p_pUnit->GetWidth(), p_pUnit->GetHeight() ), ePIXEL_FORMAT_A8R8G8B8, p_pUnit->GetImageBuffer(), p_pUnit->GetPixelFormat() );
			CreateBitmapFromBuffer( l_pBuffer->const_ptr(), l_pBuffer->width(), l_pBuffer->height(), p_bitmap );
		}
		else if ( !p_pUnit->GetTexturePath().empty() )
		{
			wxImageHandler * l_pHandler = wxImage::FindHandler( p_pUnit->GetTexturePath().GetExtension(), wxBITMAP_TYPE_ANY );

			if ( l_pHandler )
			{
				wxImage l_image;

				if ( l_image.LoadFile( p_pUnit->GetTexturePath(), l_pHandler->GetType() ) && l_image.IsOk() )
				{
					p_bitmap = wxBitmap( l_image );
				}
				else
				{
					Logger::LogWarning( cuT( "CreateBitmapFromBuffer encountered a problem loading file [" ) + p_pUnit->GetTexturePath() + cuT( "]" ) );
				}
			}
			else
			{
				Logger::LogWarning( cuT( "CreateBitmapFromBuffer encountered a problem loading file [" ) + p_pUnit->GetTexturePath() + cuT( "] : Unsupported format" ) );
			}
		}
	}

	Castor3D::WindowHandle make_WindowHandle( wxWindow * p_window )
	{
#if defined( _WIN32 )
		return WindowHandle( std::make_shared< IMswWindowHandle >( p_window->GetHandle() ) );
#elif defined( __linux__ )
		GtkWidget * l_pGtkWidget = static_cast< GtkWidget * >( p_window->GetHandle() );
		GLXDrawable l_drawable = None;
		Display * l_pDisplay = NULL;

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
#else
#	error "Yet unsupported OS"
#endif
	}

	FontSPtr make_Font( Engine * p_engine, wxFont const & p_font )
	{
		String l_name = make_String( p_font.GetFaceName() ) + str_utils::to_string( p_font.GetPointSize() );
		FontCollection & l_manager = p_engine->GetFontManager();
		FontSPtr l_font = l_manager.find( l_name );

		if ( !l_font )
		{
			if ( p_font.IsOk() )
			{
				//l_font = std::make_shared< Castor::Font >( l_name, p_font.GetPointSize() );
				//l_font->SetGlyphLoader( std::make_unique< wxWidgetsFontImpl >( p_font ) );
				//Font::BinaryLoader()( *l_font, String( p_font.GetFaceName() ), uint32_t( std::abs( p_font.GetPointSize() ) ) );
				//l_manager.insert( l_name, l_font );
			}
		}

		return l_font;
	}
}
