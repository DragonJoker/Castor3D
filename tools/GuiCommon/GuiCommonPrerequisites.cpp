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
#include <Castor3D/Gui/Gui_Parsers.hpp>
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
	namespace helpers
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

		static castor::PathArray listPluginsFiles( castor::Path const & folder )
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

		class PreprocessedSceneFile
			: public castor::PreprocessedFile
		{
		public:
			enum Category : uint32_t
			{
				eTexture,
				eSampler,
				eMaterial,
				eMesh,
				eNode,
				eObject,
				eLight,
				eOverlay,
				eGui,
				eImport,
				eOther,
				eCount,
			};

		public:
			explicit PreprocessedSceneFile( castor::FileParser & parser )
				: castor::PreprocessedFile{ parser }
			{
			}

			PreprocessedSceneFile( castor::FileParser & parser
				, castor::FileParserContextUPtr context )
				: castor::PreprocessedFile{ parser, std::move( context ) }
			{
			}

			castor::SectionId getCategory( castor::String const & name
				, castor::SectionId curSection
				, castor::SectionId nextSection
				, bool implicit )
			{
				using namespace castor3d;

				if ( name == "}" )
				{
					std::swap( curSection, nextSection );
				}

				if ( implicit )
				{
					nextSection = curSection;
				}

				auto simplifySection = []( castor::SectionId section )
					{
						switch ( section )
						{
						case uint32_t( CSCNSection::eSampler ):
							return uint32_t( Category::eSampler );
						case uint32_t( CSCNSection::eLight ):
						case uint32_t( CSCNSection::eShadows ):
						case uint32_t( CSCNSection::eRsm ):
						case uint32_t( CSCNSection::eLpv ):
						case uint32_t( CSCNSection::eRaw ):
						case uint32_t( CSCNSection::ePcf ):
						case uint32_t( CSCNSection::eVsm ):
							return uint32_t( Category::eLight );
						case uint32_t( CSCNSection::eNode ):
							return uint32_t( Category::eNode );
						case uint32_t( CSCNSection::eObject ):
						case uint32_t( CSCNSection::eObjectMaterials ):
							return uint32_t( Category::eObject );
						case uint32_t( CSCNSection::eMesh ):
						case uint32_t( CSCNSection::eSubmesh ):
						case uint32_t( CSCNSection::eBillboard ):
						case uint32_t( CSCNSection::eBillboardList ):
						case uint32_t( CSCNSection::eParticleSystem ):
						case uint32_t( CSCNSection::eParticle ):
						case uint32_t( CSCNSection::eMeshDefaultMaterials ):
						case uint32_t( CSCNSection::eSkeleton ):
						case uint32_t( CSCNSection::eMorphAnimation ):
							return uint32_t( Category::eMesh );
						case uint32_t( CSCNSection::eMaterial ):
						case uint32_t( CSCNSection::ePass ):
						case uint32_t( CSCNSection::eTextureUnit ):
						case uint32_t( CSCNSection::eShaderProgram ):
						case uint32_t( CSCNSection::eShaderStage ):
						case uint32_t( CSCNSection::eShaderUBO ):
						case uint32_t( CSCNSection::eUBOVariable ):
						case uint32_t( CSCNSection::eTextureAnimation ):
						case uint32_t( CSCNSection::eTextureTransform ):
							return uint32_t( Category::eMaterial );
						case uint32_t( CSCNSection::eTexture ):
							return uint32_t( Category::eTexture );
						case uint32_t( CSCNSection::ePanelOverlay ):
						case uint32_t( CSCNSection::eBorderPanelOverlay ):
						case uint32_t( CSCNSection::eTextOverlay ):
							return uint32_t( Category::eOverlay );
						case uint32_t( GUISection::eGUI ):
						case uint32_t( GUISection::eTheme ):
						case uint32_t( GUISection::eButtonStyle ):
						case uint32_t( GUISection::eEditStyle ):
						case uint32_t( GUISection::eComboStyle ):
						case uint32_t( GUISection::eListStyle ):
						case uint32_t( GUISection::eSliderStyle ):
						case uint32_t( GUISection::eStaticStyle ):
						case uint32_t( GUISection::ePanelStyle ):
						case uint32_t( GUISection::eProgressStyle ):
						case uint32_t( GUISection::eExpandablePanelStyle ):
						case uint32_t( GUISection::eFrameStyle ):
						case uint32_t( GUISection::eScrollBarStyle ):
						case uint32_t( GUISection::eButton ):
						case uint32_t( GUISection::eStatic ):
						case uint32_t( GUISection::eSlider ):
						case uint32_t( GUISection::eComboBox ):
						case uint32_t( GUISection::eListBox ):
						case uint32_t( GUISection::eEdit ):
						case uint32_t( GUISection::ePanel ):
						case uint32_t( GUISection::eProgress ):
						case uint32_t( GUISection::eExpandablePanel ):
						case uint32_t( GUISection::eExpandablePanelHeader ):
						case uint32_t( GUISection::eExpandablePanelExpand ):
						case uint32_t( GUISection::eExpandablePanelContent ):
						case uint32_t( GUISection::eFrame ):
						case uint32_t( GUISection::eFrameContent ):
						case uint32_t( GUISection::eBoxLayout ):
						case uint32_t( GUISection::eLayoutCtrl ):
							return uint32_t( Category::eGui );
						case uint32_t( CSCNSection::eSceneImport ):
							return uint32_t( Category::eImport );
						default:
							return uint32_t( Category::eOther );
						}
					};
				curSection = simplifySection( curSection );
				nextSection = simplifySection( nextSection );

				switch ( nextSection )
				{
				case uint32_t( Category::eSampler ):
					return uint32_t( Category::eSampler );
				case uint32_t( Category::eLight ):
					return uint32_t( Category::eLight );
				case uint32_t( Category::eNode ):
					return uint32_t( Category::eNode );
				case uint32_t( Category::eObject ):
					return uint32_t( Category::eObject );
				case uint32_t( Category::eMesh ):
					return uint32_t( Category::eMesh );
				case uint32_t( Category::eMaterial ):
					return uint32_t( Category::eMaterial );
				case uint32_t( Category::eTexture ):
					return uint32_t( Category::eTexture );
				case uint32_t( Category::eOverlay ):
					return uint32_t( Category::eOverlay );
				case uint32_t( Category::eImport ):
					return uint32_t( Category::eImport );
				case uint32_t( Category::eGui ):
					return uint32_t( Category::eGui );
				default:
					switch ( curSection )
					{
					case uint32_t( Category::eSampler ):
						return uint32_t( Category::eSampler );
					case uint32_t( Category::eLight ):
						return uint32_t( Category::eLight );
					case uint32_t( Category::eNode ):
						return uint32_t( Category::eNode );
					case uint32_t( Category::eObject ):
						return uint32_t( Category::eObject );
					case uint32_t( Category::eMesh ):
						return uint32_t( Category::eMesh );
					case uint32_t( Category::eMaterial ):
						return uint32_t( Category::eMaterial );
					case uint32_t( Category::eTexture ):
						return uint32_t( Category::eTexture );
					case uint32_t( Category::eOverlay ):
						return uint32_t( Category::eOverlay );
					case uint32_t( Category::eImport ):
						return uint32_t( Category::eImport );
					case uint32_t( Category::eGui ):
						return uint32_t( Category::eGui );
					default:
						return uint32_t( Category::eOther );
					}
				}
			}

			uint32_t getCategoryActionsCount( castor::SectionId section )const
			{
				return m_total[section];
			}

			uint32_t incCategoryActions( castor::SectionId section, uint32_t count = 1u )
			{
				return m_current[section] += count;
			}

			uint32_t getCategoriesCount()const
			{
				return uint32_t( std::count_if( m_total.begin(), m_total.end()
					, []( uint32_t value )
					{
						return value > 0u;
					} ) );
			}

			castor::xchar const * getCategoryName( castor::SectionId section )const
			{
				using namespace castor3d;
				switch ( section )
				{
				case uint32_t( Category::eSampler ):
					return "Loading Samplers";
				case uint32_t( Category::eLight ):
					return "Loading Lights";
				case uint32_t( Category::eNode ):
					return "Loading Nodes";
				case uint32_t( Category::eObject ):
					return "Loading Objects";
				case uint32_t( Category::eMesh ):
					return "Loading Meshes";
				case uint32_t( Category::eMaterial ):
					return "Loading Materials";
				case uint32_t( Category::eTexture ):
					return "Loading Textures";
				case uint32_t( Category::eOverlay ):
					return "Loading Overlays";
				case uint32_t( Category::eImport ):
					return "Processing Imports";
				case uint32_t( Category::eGui ):
					return "Loading GUI";
				default:
					return "Others";
				}
			}

		protected:
			void doAddParserAction( castor::Path file
				, uint64_t line
				, castor::String name
				, castor::SectionId section
				, castor::ParserFunctionAndParams function
				, castor::String params
				, bool implicit )override
			{
				using namespace castor3d;
				auto category = getCategory( name, section, function.resultSection, implicit );
				m_total[category]++;
				castor::PreprocessedFile::doAddParserAction( std::move( file )
					, line
					, std::move( name )
					, section
					, std::move( function )
					, std::move( params )
					, implicit );
			}

		private:
			std::array< uint32_t, Category::eCount > m_total{};
			std::array< uint32_t, Category::eCount > m_current{};
		};
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
		if ( unit.isTextureStatic() )
		{
			createBitmapFromBuffer( unit.getTextureImageBuffer(), flip, bitmap );
		}
		else
		{
			castor::Path path{ unit.getTexturePath() };

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

	castor3d::RenderWindowDesc loadScene( castor3d::Engine & engine
		, castor::String const & appName
		, castor::Path const & fileName
		, castor3d::ProgressBar * progress )
	{
		castor3d::RenderWindowDesc result{};

		if ( castor::File::fileExists( fileName ) )
		{
			castor::Logger::logInfo( cuT( "Loading scene file : " ) + fileName );

			if ( fileName.getExtension() == cuT( "cscn" ) || fileName.getExtension() == cuT( "zip" ) )
			{
				try
				{
					castor3d::SceneFileParser parser{ engine };

					if ( progress )
					{
						castor3d::setProgressBarGlobalTitle( progress
							, "Loading scene..." );
						castor3d::stepProgressBarGlobalStartLocal( progress
							, "Preprocessing"
							, 1u );

						helpers::PreprocessedSceneFile preprocessed{ parser, parser.initialiseParser( fileName ) };
						parser.processFile( appName, fileName, preprocessed );

						auto index = castor3d::incProgressBarGlobalRange( progress
							, uint32_t( helpers::PreprocessedSceneFile::Category::eCount ) );
						auto actionConnection = preprocessed.onAction.connect( [progress, index, &preprocessed]( castor::SectionId section
							, castor::PreprocessedFile::Action const & action )
							{
									section = preprocessed.getCategory( action.name, section, action.function.resultSection, action.implicit );
									auto status = preprocessed.incCategoryActions( section );
									auto total = preprocessed.getCategoryActionsCount( section );
									castor3d::setProgressBarGlobalStep( progress
										, "Loading scene..."
										, index + section );
									castor3d::setProgressBarLocal( progress
										, preprocessed.getCategoryName( section )
										, castor::string::toString( status ) + " / " + castor::string::toString( total )
										, total
										, status );
							} );

						if ( preprocessed.parse() )
						{
							result = parser.getRenderWindow();
						}
						else
						{
							castor::Logger::logWarning( cuT( "Can't read scene file" ) );
						}
					}
					else if ( parser.parseFile( appName, fileName ) )
					{
						result = parser.getRenderWindow();
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
					auto var = new wxVariant{ new castor3d::RenderWindowDesc{ target } };
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
		castor::PathArray arrayKept = helpers::listPluginsFiles( castor3d::Engine::getPluginsDirectory() );

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
			arrayKept = helpers::listPluginsFiles( pathUsr / cuT( "lib" ) / cuT( "Castor3D" ) );
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
			font->setGlyphLoader( std::make_unique< helpers::wxWidgetsFontImpl >( wxfont ) );
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
}
