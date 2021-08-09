#include "GuiCommon/GuiCommonPrerequisites.hpp"

#if defined( CU_PlatformWindows ) && !defined( NDEBUG ) && !defined( VLD_AVAILABLE )
#	define _CRTDBG_MAP_ALLOC
#	include <cstdlib>
#	include <crtdbg.h>
#elif defined( CU_PlatformLinux )
#	include <gdk/gdkx.h>
#	include <gtk/gtk.h>
#	include <GL/glx.h>
#	undef None
#	undef Bool
#	undef Always
using Bool = int;
#elif defined( CU_PlatformApple )
#	include "GuiCommon/System/MetalLayer.h"
#endif

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/PluginCache.hpp>
#include <Castor3D/Event/Frame/GpuFunctorEvent.hpp>
#include <Castor3D/Event/Frame/CpuFunctorEvent.hpp>
#include <Castor3D/Event/Frame/InitialiseEvent.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Miscellaneous/ProgressBar.hpp>
#include <Castor3D/Model/Mesh/Mesh.hpp>
#include <Castor3D/Plugin/Plugin.hpp>
#include <Castor3D/Render/RenderWindow.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>
#include <Castor3D/Material/Texture/Sampler.hpp>
#include <Castor3D/Material/Texture/TextureLayout.hpp>
#include <Castor3D/Material/Texture/TextureUnit.hpp>

#include <CastorUtils/Graphics/Font.hpp>
#include <CastorUtils/Graphics/PixelBufferBase.hpp>

#include <ashespp/Core/PlatformWindowHandle.hpp>

#include <wx/window.h>
#include <wx/rawbmp.h>

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

			virtual void loadGlyph( castor::Glyph & p_glyph )
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

		castor::PathArray listPluginsFiles( castor::Path const & folder )
		{
			static castor::String castor3DLibPrefix{ CU_LibPrefix + castor::String{ cuT( "castor3d" ) } };
			castor::PathArray files;
			castor::File::listDirectoryFiles( folder, files );
			castor::PathArray result;

			// Exclude debug plug-in in release builds, and release plug-ins in debug builds
			for ( auto file : files )
			{
				auto fileName = file.getFileName( true );

				if ( fileName.find( CU_SharedLibExt ) != castor::String::npos
					&& fileName.find( castor3DLibPrefix ) == 0u )
				{
					result.push_back( file );
				}
			}

			return result;
		}
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
#if defined( CU_PlatformWindows )
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

#if defined( CU_PlatformWindows )
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
#if defined( CU_PlatformWindows )
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

#if defined( CU_PlatformWindows )
						it = rowStart;
						it.OffsetY( data, 1 );
#endif
					}
				}
			}
			catch ( ... )
			{
				castor::Logger::logWarning( cuT( "CreateBitmapFromBuffer encountered an exception" ) );
			}
		}
	}

	void CreateBitmapFromBuffer( castor::PxBufferBaseSPtr buffer, bool flip, wxBitmap & bitmap )
	{
		if ( buffer->getFormat() != castor::PixelFormat::eR8G8B8A8_UNORM )
		{
			buffer = castor::PxBufferBase::create( buffer->getDimensions()
				, castor::PixelFormat::eR8G8B8A8_UNORM
				, buffer->getConstPtr()
				, buffer->getFormat() );
		}

		CreateBitmapFromBuffer( buffer->getConstPtr(), buffer->getWidth(), buffer->getHeight(), flip, bitmap );
	}

	void CreateBitmapFromBuffer( TextureUnitSPtr unit, bool flip, wxBitmap & bitmap )
	{
		if ( unit->getTexture()->getDefaultView().hasBuffer() )
		{
			CreateBitmapFromBuffer( unit->getTexture()->getImage().getPixels(), flip, bitmap );
		}
		else
		{
			castor::Path path{ unit->getTexture()->getDefaultView().toString() };

			if ( !path.empty() )
			{
				wxImageHandler * pHandler = wxImage::FindHandler( path.getExtension(), wxBITMAP_TYPE_ANY );

				if ( pHandler )
				{
					wxImage image;

					if ( image.LoadFile( path, pHandler->GetType() ) && image.IsOk() )
					{
						bitmap = wxBitmap( image );
					}
					else
					{
						castor::Logger::logWarning( cuT( "CreateBitmapFromBuffer encountered a problem loading file [" ) + path + cuT( "]" ) );
					}
				}
				else
				{
					castor::Logger::logWarning( cuT( "CreateBitmapFromBuffer encountered a problem loading file [" ) + path + cuT( "] : Unsupported format" ) );
				}
			}
		}
	}

	RenderTargetSPtr loadScene( Engine & engine
		, castor::Path const & fileName
		, castor3d::ProgressBar * progress )
	{
		RenderTargetSPtr result;

		if ( castor::File::fileExists( fileName ) )
		{
			castor::Logger::logInfo( cuT( "Loading scene file : " ) + fileName );

			if ( fileName.getExtension() == cuT( "cscn" ) || fileName.getExtension() == cuT( "zip" ) )
			{
				try
				{
					SceneFileParser parser( engine );
					auto preprocessed = parser.processFile( fileName );

					if ( progress )
					{
						progress->setRange( preprocessed.getCount() );
						progress->setLabel( "Loading scene" );
						progress->setTitle( "Initialising" );
					}

					auto connection = preprocessed.onAction.connect( [progress, &fileName]( castor::PreprocessedFile::Action const & action )
						{
							castor3d::stepProgressBar( progress );
						} );

					if ( preprocessed.parse() )
					{
						result = parser.getRenderWindow().renderTarget;
					}
					else
					{
						castor::Logger::logWarning( cuT( "Can't read scene file" ) );
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
			wxMessageBox( _( "Scene file doesn't exist :" ) + wxString( wxT( "\n" ) ) + fileName );
		}

		return result;
	}

	void loadPlugins( castor3d::Engine & engine )
	{
		castor::PathArray arrayKept = listPluginsFiles( Engine::getPluginsDirectory() );

#if !defined( NDEBUG )

		// When debug is installed, plugins are installed in lib/Debug/Castor3D
		if ( arrayKept.empty() )
		{
			castor::Path pathBin = castor::File::getExecutableDirectory();

			while ( pathBin.getFileName() != cuT( "bin" ) )
			{
				pathBin = pathBin.getPath();
			}

			castor::Path pathUsr = pathBin.getPath();
			arrayKept = listPluginsFiles( pathUsr / cuT( "lib" ) / cuT( "Debug" ) / cuT( "Castor3D" ) );
		}

#endif

		if ( !arrayKept.empty() )
		{
			castor::PathArray arrayFailed;

			for ( auto file : arrayKept )
			{
				if ( !engine.getPluginCache().loadPlugin( file ) )
				{
					arrayFailed.push_back( file );
				}
			}

			if ( !arrayFailed.empty() )
			{
				castor::Logger::logWarning( cuT( "Some plug-ins couldn't be loaded :" ) );

				for ( auto file : arrayFailed )
				{
					castor::Logger::logWarning( castor::Path( file ).getFileName() );
				}

				arrayFailed.clear();
			}
		}

		castor::Logger::logInfo( cuT( "Plugins loaded" ) );
	}

	ashes::WindowHandle makeWindowHandle( wxWindow * window )
	{
#if defined( CU_PlatformWindows )

		return ashes::WindowHandle( std::make_unique< ashes::IMswWindowHandle >( ::GetModuleHandle( nullptr )
			, window->GetHandle() ) );

#elif defined( CU_PlatformLinux )

		GtkWidget * gtkWidget = static_cast< GtkWidget * >( window->GetHandle() );
		GLXDrawable drawable = 0;
		Display * display = nullptr;

		if ( gtkWidget && gtkWidget->window )
		{
			drawable = GDK_WINDOW_XID( gtkWidget->window );
			GdkDisplay * gtkDisplay = gtk_widget_get_display( gtkWidget );

			if ( gtkDisplay )
			{
				display = gdk_x11_display_get_xdisplay( gtkDisplay );
			}
		}

		return ashes::WindowHandle( std::make_unique< ashes::IXWindowHandle >( drawable, display ) );

#elif defined( CU_PlatformApple )

		auto handle = window->GetHandle();
		makeViewMetalCompatible( handle );
		return ashes::WindowHandle( std::make_unique< ashes::IMacOsWindowHandle >( handle ) );

#else

#	error "Yet unsupported platform"

#endif
	}

	castor::FontSPtr make_Font( wxFont const & wxfont )
	{
		castor::FontSPtr font;

		if ( wxfont.IsOk() )
		{
			//castor::String name = make_String( wxfont.GetFaceName() ) + castor::string::toString( wxfont.GetPointSize() );
			//font = std::make_shared< castor::Font >( name, wxfont.GetPointSize() );
			//font->setGlyphLoader( std::make_unique< wxWidgetsFontImpl >( wxfont ) );
			//castor::Font::BinaryLoader()( *font
			//	, castor::String( wxfont.GetFaceName() )
			//	, uint32_t( std::abs( wxfont.GetPointSize() ) ) );
		}

		return font;
	}

	castor::String make_String( wxString const & value )
	{
		return castor::String( value.mb_str( wxConvUTF8 ).data() );
	}

	castor::Path make_Path( wxString const & value )
	{
		return castor::Path( value.mb_str( wxConvUTF8 ).data() );
	}

	wxString make_wxString( castor::String const & value )
	{
		return wxString( value.c_str(), wxConvUTF8 );
	}

	castor::Size makeSize( wxSize const & value )
	{
		return castor::Size( value.x, value.y );
	}

	wxSize make_wxSize( castor::Size const & value )
	{
		return wxSize( value.getWidth(), value.getHeight() );
	}

	ast::ShaderStage convert( VkShaderStageFlagBits stage )
	{
		switch ( stage )
		{
		case VK_SHADER_STAGE_VERTEX_BIT:
			return ast::ShaderStage::eVertex;
		case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
			return ast::ShaderStage::eTessellationControl;
		case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
			return ast::ShaderStage::eTessellationEvaluation;
		case VK_SHADER_STAGE_GEOMETRY_BIT:
			return ast::ShaderStage::eGeometry;
		case VK_SHADER_STAGE_FRAGMENT_BIT:
			return ast::ShaderStage::eFragment;
		case VK_SHADER_STAGE_COMPUTE_BIT:
			return ast::ShaderStage::eCompute;
		default:
			assert( false && "Unsupported VkShaderStageFlagBits" );
			return ast::ShaderStage::eVertex;
		}
	}
}
