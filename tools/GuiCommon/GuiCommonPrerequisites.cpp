#include "GuiCommon/GuiCommonPrerequisites.hpp"

#if defined( CU_PlatformWindows ) && !defined( NDEBUG ) && !defined( VLD_AVAILABLE )
#	define _CRTDBG_MAP_ALLOC
#	include <cstdlib>
#	include <crtdbg.h>
#elif defined( CU_PlatformLinux )
#	include <gdk/gdkx.h>
#	include <gdk/gdkwayland.h>
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

namespace GuiCommon
{
	namespace
	{
		struct wxWidgetsFontImpl
			: public castor::Font::SFontImpl
		{
			explicit wxWidgetsFontImpl( wxFont const & font )
				: m_font( font )
			{
			}

			void initialise()override
			{
			}

			void cleanup()override
			{
			}

			castor::Glyph loadGlyph( char32_t glyph )override
			{
				return { glyph, {}, {}, {}, {} };
			}

			wxFont m_font;
		};

		castor::PathArray listPluginsFiles( castor::Path const & folder )
		{
			static castor::String castor3DLibPrefix{ CU_LibPrefix + castor::String{ cuT( "castor3d" ) } };
			castor::PathArray files;
			castor::File::listDirectoryFiles( folder, files );
			castor::PathArray result;
			castor::String endRel = "." + castor::String{ CU_SharedLibExt };
			castor::String endDbg = "d" + endRel;

			// Exclude debug plug-in in release builds, and release plug-ins in debug builds
			for ( auto file : files )
			{
				auto fileName = file.getFileName( true );
				bool res = castor::string::endsWith( fileName, endDbg );
#if defined( NDEBUG )
				res = castor::string::endsWith( fileName, endRel ) && !res;
#endif
				if ( res && fileName.find( castor3DLibPrefix ) == 0u )
				{
					result.push_back( file );
				}
			}

			return result;
		}
	}

	void createBitmapFromBuffer( uint8_t const * buffer
		, uint32_t width
		, uint32_t height
		, bool flip
		, wxBitmap & bitmap )
	{
		bitmap.Create( int( width ), int( height ), 24 );
		wxNativePixelData data( bitmap );

		if ( bitmap.IsOk() && uint32_t( data.GetWidth() ) == width && uint32_t( data.GetHeight() ) == height )
		{
			wxNativePixelData::Iterator it( data );

			try
			{
				if ( flip )
				{
					uint32_t pitch = width * 4;
					uint8_t const * buf = buffer + ( height - 1 ) * pitch;

					for ( uint32_t i = 0; i < height && it.IsOk(); i++ )
					{
						uint8_t const * line = buf;
#if defined( CU_PlatformWindows )
						wxNativePixelData::Iterator rowStart = it;
#endif

						for ( uint32_t j = 0; j < width && it.IsOk(); j++ )
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

						buf -= pitch;

#if defined( CU_PlatformWindows )
						it = rowStart;
						it.OffsetY( data, 1 );
#endif
					}
				}
				else
				{
					uint8_t const * buf = buffer;

					for ( uint32_t i = 0; i < height && it.IsOk(); i++ )
					{
#if defined( CU_PlatformWindows )
						wxNativePixelData::Iterator rowStart = it;
#endif

						for ( uint32_t j = 0; j < width && it.IsOk(); j++ )
						{
							it.Red() = *buf;
							buf++;
							it.Green() = *buf;
							buf++;
							it.Blue() = *buf;
							buf++;
							// don't write the alpha.
							buf++;
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

	void createBitmapFromBuffer( castor::PxBufferBase const & buffer, bool flip, wxBitmap & bitmap )
	{
		if ( buffer.getFormat() != castor::PixelFormat::eR8G8B8A8_UNORM )
		{
			auto buf = castor::PxBufferBase::create( buffer.getDimensions()
				, castor::PixelFormat::eR8G8B8A8_UNORM
				, buffer.getConstPtr()
				, buffer.getFormat() );
			createBitmapFromBuffer( *buf, flip, bitmap );
		}
		else
		{
			createBitmapFromBuffer( buffer.getConstPtr(), buffer.getWidth(), buffer.getHeight(), flip, bitmap );
		}

	}

	void createBitmapFromBuffer( castor3d::TextureUnit const & unit, bool flip, wxBitmap & bitmap )
	{
		if ( unit.getTexture()->getDefaultView().hasBuffer() )
		{
			createBitmapFromBuffer( *unit.getTexture()->getImage().getPixels(), flip, bitmap );
		}
		else
		{
			castor::Path path{ unit.getTexture()->getDefaultView().toString() };

			if ( !path.empty() )
			{
				if ( auto handler = wxImage::FindHandler( path.getExtension(), wxBITMAP_TYPE_ANY ) )
				{
					wxImage image;

					if ( image.LoadFile( path, handler->GetType() ) && image.IsOk() )
					{
						bitmap = wxBitmap( image );
					}
					else
					{
						castor::Logger::logWarning( cuT( "createBitmapFromBuffer encountered a problem loading file [" ) + path + cuT( "]" ) );
					}
				}
				else
				{
					castor::Logger::logWarning( cuT( "createBitmapFromBuffer encountered a problem loading file [" ) + path + cuT( "] : Unsupported format" ) );
				}
			}
		}
	}

	castor3d::RenderTargetRPtr loadScene( castor3d::Engine & engine
		, castor::String const & appName
		, castor::Path const & fileName
		, castor3d::ProgressBar * progress )
	{
		castor3d::RenderTargetRPtr result{};

		if ( castor::File::fileExists( fileName ) )
		{
			castor::Logger::logInfo( cuT( "Loading scene file : " ) + fileName );

			if ( fileName.getExtension() == cuT( "cscn" ) || fileName.getExtension() == cuT( "zip" ) )
			{
				try
				{
					castor3d::SceneFileParser parser( engine );
					auto preprocessed = parser.processFile( appName, fileName );

					if ( progress )
					{
						progress->setTitle( "Initialising: Scene" );
						progress->setLabel( "Loading scene..." );
						progress->setRange( int32_t( preprocessed.getCount() ) );
					}

					auto connection = preprocessed.onAction.connect( [progress]( castor::PreprocessedFile::Action const & action )
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

	void loadScene( castor3d::Engine & engine
		, castor::String const & appName
		, castor::Path const & fileName
		, castor3d::ProgressBar * progress
		, wxWindow * window
		, int eventID )
	{
		std::thread async{ [&engine, appName, fileName, progress, window, eventID]()
			{
				try
				{
					auto target = loadScene( engine, appName, fileName, progress );
					auto event = new wxThreadEvent{ wxEVT_THREAD, eventID };
					auto var = new wxVariant{ target };
					event->SetEventObject( var );
					window->GetEventHandler()->QueueEvent( event );
				}
				catch ( std::exception & exc )
				{
					wxMessageBox( _( "Scene file couldn't be loaded: " ) + wxString( wxT( "\n" ) ) + exc.what() );
				}
			} };
		async.detach();
	}

	void loadPlugins( castor3d::Engine & engine )
	{
		castor::PathArray arrayKept = listPluginsFiles( castor3d::Engine::getPluginsDirectory() );

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
			arrayKept = listPluginsFiles( pathUsr / cuT( "lib" ) / cuT( "Castor3D" ) );
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
					castor::Logger::logWarning( file.getFileName() );
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

		auto gtkWidget = window->GetHandle();

		if ( gtkWidget )
		{
			gtk_widget_realize( gtkWidget );
			auto gdkDisplay = gtk_widget_get_display( gtkWidget );

			if ( gdkDisplay )
			{
#if defined( VK_USE_PLATFORM_WAYLAND_KHR )
#	ifdef GDK_WINDOWING_WAYLAND
				if ( GDK_IS_WAYLAND_DISPLAY( gdkDisplay ) )
				{
					auto gdkWindow = gtk_widget_get_window( gtkWidget );
					auto display = gdk_wayland_display_get_wl_display( gdkDisplay );
					auto surface = gdkWindow
						? gdk_wayland_window_get_wl_surface( gdkWindow )
						: nullptr;
					return ashes::WindowHandle( std::make_unique< ashes::IWaylandWindowHandle >( display, surface ) );
				}
#	endif
#endif
#if defined( VK_USE_PLATFORM_XLIB_KHR )
#	ifdef GDK_WINDOWING_X11
				if ( GDK_IS_X11_DISPLAY( gdkDisplay ) )
				{
					auto gdkWindow = gtk_widget_get_window( gtkWidget );
					auto display = gdk_x11_display_get_xdisplay( gdkDisplay );
					GLXDrawable drawable = gdkWindow
						? gdk_x11_window_get_xid( gdkWindow )
						: 0;
					return ashes::WindowHandle( std::make_unique< ashes::IXWindowHandle >( drawable, display ) );
				}
#	endif
#endif
				CU_Exception( "Unsupported GTK surface type." );
			}
		}

		return ashes::WindowHandle{ nullptr };


#elif defined( CU_PlatformApple )

		auto handle = window->GetHandle();
		makeViewMetalCompatible( handle );
		return ashes::WindowHandle( std::make_unique< ashes::IMacOsWindowHandle >( handle ) );

#else

#	error "Yet unsupported platform"

#endif
	}

	castor::FontUPtr make_Font( wxFont const & wxfont )
	{
		castor::FontUPtr font;

		if ( wxfont.IsOk() )
		{
			castor::String name = make_String( wxfont.GetFaceName() ) + castor::string::toString( wxfont.GetPointSize() );
			font = castor::makeUnique< castor::Font >( name, wxfont.GetPointSize() );
			font->setGlyphLoader( std::make_unique< wxWidgetsFontImpl >( wxfont ) );
			castor::Font::BinaryLoader{}( *font
				, castor::Path{ castor::String{ wxfont.GetFaceName() } }
				, uint32_t( std::abs( wxfont.GetPointSize() ) ) );
		}

		return font;
	}

	castor::String make_String( wxString const & value )
	{
		return castor::String( value.mb_str( wxConvUTF8 ).data() );
	}

	castor::U32String make_U32String( wxString const & value )
	{
		return castor::string::toU32String( make_String( value ) );
	}

	castor::Path make_Path( wxString const & value )
	{
		return castor::Path( value.mb_str( wxConvUTF8 ).data() );
	}

	wxString make_wxString( castor::String const & value )
	{
		return wxString( value.c_str(), wxConvUTF8 );
	}

	wxString make_wxString( castor::U32String const & value )
	{
		return make_wxString( castor::string::stringCast< castor::xchar >( value ) );
	}

	wxArrayString make_wxArrayString( castor::StringArray const & values )
	{
		wxArrayString result;

		for ( auto & value : values )
		{
			result.push_back( make_wxString( value ) );
		}

		return result;
	}

	castor::StringArray make_StringArray( wxArrayString const & values )
	{
		castor::StringArray result;

		for ( auto & value : values )
		{
			result.push_back( make_String( value ) );
		}

		return result;
	}

	castor::Size makeSize( wxSize const & value )
	{
		return castor::Size( uint32_t( value.x )
			, uint32_t( value.y ) );
	}

	wxSize make_wxSize( castor::Size const & value )
	{
		return wxSize( int( value.getWidth() )
			, int( value.getHeight() ) );
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
