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
		struct wxWidgetsGlyphLoader
			: public c3d::Font::GlyphLoader
		{
			explicit wxWidgetsGlyphLoader( wxFont const & font )
				: m_font( font )
			{
			}

			void initialise()override
			{
			}

			void cleanup()override
			{
			}

			c3d::Glyph loadGlyph( char32_t glyph )override
			{
				return { glyph, {}, {}, {}, {}, {} };
			}

			void fillKerningTable( char32_t c32
				, c3d::Font::GlyphArray const & glyphs
				, c3d::Font::GlyphKerningMap & table )override
			{
			}

			wxFont m_font;
		};

		static c3d::PathArray listPluginsFiles( c3d::Path const & folder )
		{
			static c3d::String castor3DLibPrefix{ CU_LibPrefix + c3d::String{ cuT( "castor3d" ) } };
			c3d::PathArray files;
			c3d::File::listDirectoryFiles( folder, files );
			c3d::PathArray result;
			c3d::String endRel = c3d::String{ CU_SharedLibExt };

			for ( auto file : files )
			{
				auto fileName = file.getFileName( true );
				bool res = c3d::string::endsWith( fileName, endRel );

				if ( res && fileName.find( castor3DLibPrefix ) == 0u )
				{
					result.push_back( file );
				}
			}

			return result;
		}

		class PreprocessedSceneFile
			: public c3d::PreprocessedFile
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
			using c3d::PreprocessedFile::PreprocessedFile;

			c3d::SectionId getCategory( c3d::String const & name
				, c3d::SectionId curSection
				, c3d::SectionId nextSection
				, bool implicit )const
			{
				using namespace c3d;

				if ( implicit )
				{
					if ( name == cuT( "}" ) )
					{
						curSection = nextSection;
					}
				}
				else
				{
					if ( name != cuT( "}" ) )
					{
						curSection = nextSection;
					}
				}

				switch ( curSection )
				{
				case uint32_t( CSCNSection::eSampler ):
					return uint32_t( Category::eSampler );
				case uint32_t( CSCNSection::eLight ):
				case uint32_t( CSCNSection::eShadows ):
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
			}

			uint32_t getCategoryActionsCount( c3d::SectionId section )const
			{
				return m_totalCat[section];
			}

			uint32_t incCategoryActions( c3d::SectionId section, uint32_t count = 1u )
			{
				return m_currentCat[section] += count;
			}

			uint32_t getCategoriesCount()const
			{
				return uint32_t( std::count_if( m_totalCat.begin(), m_totalCat.end()
					, []( uint32_t value )
					{
						return value > 0u;
					} ) );
			}

			c3d::xchar const * getCategoryName( c3d::SectionId section )const
			{
				using namespace c3d;
				switch ( section )
				{
				case uint32_t( Category::eSampler ):
					return cuT( "Loading Samplers" );
				case uint32_t( Category::eLight ):
					return cuT( "Loading Lights" );
				case uint32_t( Category::eNode ):
					return cuT( "Loading Nodes" );
				case uint32_t( Category::eObject ):
					return cuT( "Loading Objects" );
				case uint32_t( Category::eMesh ):
					return cuT( "Loading Meshes" );
				case uint32_t( Category::eMaterial ):
					return cuT( "Loading Materials" );
				case uint32_t( Category::eTexture ):
					return cuT( "Loading Textures" );
				case uint32_t( Category::eOverlay ):
					return cuT( "Loading Overlays" );
				case uint32_t( Category::eImport ):
					return cuT( "Processing Imports" );
				case uint32_t( Category::eGui ):
					return cuT( "Loading GUI" );
				default:
					return cuT( "Others" );
				}
			}

		protected:
			void doAddParserAction( c3d::Path file
				, uint64_t line
				, c3d::String name
				, c3d::SectionId section
				, c3d::ParserFunctionAndParams function
				, c3d::String params
				, bool implicit )override
			{
				using namespace c3d;
				auto category = getCategory( name
					, section
					, function.resultSection
					, implicit );
				m_totalCat[category]++;
				c3d::PreprocessedFile::doAddParserAction( c3d::move( file )
					, line
					, c3d::move( name )
					, section
					, c3d::move( function )
					, c3d::move( params )
					, implicit );
			}

		private:
			c3d::Array< uint32_t, Category::eCount > m_totalCat{};
			c3d::Array< uint32_t, Category::eCount > m_currentCat{};
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
				c3d::Logger::logWarning( cuT( "CreateBitmapFromBuffer encountered an exception" ) );
			}
		}
	}

	void createBitmapFromBuffer( c3d::PxBufferBase const & buffer, bool flip, wxBitmap & bitmap )
	{
		if ( buffer.getFormat() != c3d::PixelFormat::eR8G8B8A8_UNORM )
		{
			auto buf = c3d::PxBufferBase::create( buffer.getDimensions()
				, c3d::PixelFormat::eR8G8B8A8_UNORM
				, buffer.getConstPtr()
				, buffer.getFormat() );
			createBitmapFromBuffer( *buf, flip, bitmap );
		}
		else
		{
			createBitmapFromBuffer( buffer.getConstPtr(), buffer.getWidth(), buffer.getHeight(), flip, bitmap );
		}

	}

	void createBitmapFromBuffer( c3d::TextureUnit const & unit, bool flip, wxBitmap & bitmap )
	{
		if ( unit.isTextureStatic() )
		{
			createBitmapFromBuffer( unit.getTextureImageBuffer(), flip, bitmap );
		}
		else
		{
			c3d::Path path{ unit.getTexturePath() };

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
						c3d::Logger::logWarning( cuT( "createBitmapFromBuffer encountered a problem loading file [" ) + path + cuT( "]" ) );
					}
				}
				else
				{
					c3d::Logger::logWarning( cuT( "createBitmapFromBuffer encountered a problem loading file [" ) + path + cuT( "] : Unsupported format" ) );
				}
			}
		}
	}

	c3d::RenderWindowDesc loadScene( c3d::Engine & engine
		, c3d::String const & appName
		, c3d::Path const & fileName
		, c3d::ProgressBar * progress )
	{
		c3d::RenderWindowDesc result{};

		if ( c3d::File::fileExists( fileName ) )
		{
			c3d::Logger::logInfo( cuT( "Loading scene file : " ) + fileName );

			if ( fileName.getExtension() == cuT( "cscn" ) || fileName.getExtension() == cuT( "zip" ) )
			{
				try
				{
					c3d::SceneFileParser parser{ engine, progress };

					if ( progress )
					{
						c3d::setProgressBarGlobalTitle( progress
							, cuT( "Loading scene..." ) );
						c3d::stepProgressBarGlobalStartLocal( progress
							, cuT( "Preprocessing scene file" )
							, 1u );

						helpers::PreprocessedSceneFile preprocessed{ parser, parser.initialiseParser( fileName ) };
						parser.processFile( appName, fileName, preprocessed );

						auto index = c3d::incProgressBarGlobalRange( progress
							, uint32_t( helpers::PreprocessedSceneFile::Category::eCount ) );
						auto actionConnection = preprocessed.onAction.connect( [progress, index, &preprocessed]( c3d::SectionId section
							, c3d::PreprocessedFile::Action const & action )
							{
									section = preprocessed.getCategory( action.name, section, action.function.resultSection, action.implicit );
									auto status = preprocessed.incCategoryActions( section );
									auto total = preprocessed.getCategoryActionsCount( section );
									c3d::setProgressBarGlobalStep( progress
										, cuT( "Loading scene..." )
										, index + section );
									c3d::setProgressBarLocal( progress
										, preprocessed.getCategoryName( section )
										, c3d::string::toString( status ) + cuT( " / " ) + c3d::string::toString( total )
										, total
										, status );
							} );

						if ( preprocessed.parse() )
						{
							result = parser.getRenderWindow();
						}
						else
						{
							c3d::Logger::logWarning( cuT( "Can't read scene file" ) );
						}
					}
					else if ( parser.parseFile( appName, fileName ) )
					{
						result = parser.getRenderWindow();
					}
					else
					{
						c3d::Logger::logWarning( cuT( "Can't read scene file" ) );
					}
				}
				catch ( std::exception & exc )
				{
					wxMessageBox( _( "Failed to parse the scene file, with following error:" ) + wxString( wxT( "\n" ) ) + wxString( exc.what(), wxMBConvLibc() ) );
				}
			}
			else
			{
				c3d::String fileContent{ cuT( R"(// Global configuration
default_lighting_model "c3d.pbr"

scene "Imported"
{
	// Scene configuration
	ambient_light 1.00000 1.00000 1.00000
	background_colour 0.500000 0.500000 0.500000
	lpv_indirect_attenuation 1.70000

	//Cameras nodes

	scene_node "MainCameraNode"
	{
	}

	//Cameras

	camera "MainCamera"
	{
		parent "MainCameraNode"

		viewport
		{
			type perspective
			near 0.1
			far 2000.0
			aspect_ratio 1.77780
			fov_y 45.0000
		}

		hdr_config
		{
			exposure 1.00000
			gamma 2.20000
		}
	}

	import
	{
		file "<import_file>"
		//preferred_importer "gltf"
		recenter_camera "MainCamera"
	}
}

//Windows

window "MainWindow"
{
	vsync false
	fullscreen false

	render_target
	{
		size 1920 1080
		format argb32
		scene "Imported"
		camera "MainCamera"
		tone_mapping "aces"

		ssao
		{
			enabled true
			high_quality true
			use_normals_buffer false
			blur_high_quality false
			intensity 1.00000
			radius 0.100000
			bias 0.0230000
			num_samples 19
			edge_sharpness 1.00000
			blur_step_size 2
			blur_radius 4
			bend_step_count 4
			bend_step_size 0.500000
		}

		smaa
		{
			mode T2X
			preset ultra
			edgeDetection colour
			enablePredication true
		}
	}
}
)" ) };
				c3d::string::replace( fileContent, cuT( "<import_file>" ), fileName.getFileName( true ).c_str() );

				if ( fileName.getExtension() == "glb" )
				{
					c3d::string::replace( fileContent, cuT( "//preferred_importer" ), cuT( "preferred_importer" ) );
				}

				auto newFileName = fileName.getPath() / ( fileName.getFileName() + cuT( ".cscn" ) );

				if ( c3d::TextFile file{ newFileName, c3d::File::OpenMode::eWrite };
					file.isOk() )
				{
					file.writeText( fileContent );
				}

				result = loadScene( engine, appName, newFileName, progress );
			}
		}
		else
		{
			wxMessageBox( _( "Scene file doesn't exist :" ) + wxString( wxT( "\n" ) ) + fileName );
		}

		return result;
	}

	void loadScene( c3d::Engine & engine
		, c3d::String const & appName
		, c3d::Path const & fileName
		, c3d::ProgressBar * progress
		, wxWindow * window
		, int eventID )
	{
		std::thread async{ [&engine, appName, fileName, progress, window, eventID]()
			{
				try
				{
					auto target = loadScene( engine, appName, fileName, progress );
					auto event = new wxThreadEvent{ wxEVT_THREAD, eventID };
					auto var = new wxVariant{ new c3d::RenderWindowDesc{ target } };
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

	void loadPlugins( c3d::Engine & engine )
	{
		c3d::PathArray arrayKept = helpers::listPluginsFiles( c3d::Engine::getPluginsDirectory() );

#if !defined( NDEBUG )

		// When debug is installed, plugins are installed in lib/Debug/Castor3D
		if ( arrayKept.empty() )
		{
			c3d::Path pathBin = c3d::File::getExecutableDirectory();

			while ( pathBin.getFileName() != cuT( "bin" ) )
			{
				pathBin = pathBin.getPath();
			}

			c3d::Path pathUsr = pathBin.getPath();
			arrayKept = helpers::listPluginsFiles( pathUsr / cuT( "lib" ) / cuT( "Castor3D" ) );
		}

#endif

		if ( !arrayKept.empty() )
		{
			c3d::PathArray arrayFailed;

			for ( auto file : arrayKept )
			{
				if ( !engine.getPluginCache().loadPlugin( file ) )
				{
					arrayFailed.push_back( file );
				}
			}

			if ( !arrayFailed.empty() )
			{
				c3d::Logger::logWarning( cuT( "Some plug-ins couldn't be loaded :" ) );

				for ( auto file : arrayFailed )
				{
					c3d::Logger::logWarning( file.getFileName() );
				}

				arrayFailed.clear();
			}
		}

		c3d::Logger::logInfo( cuT( "Plugins loaded" ) );
	}

	ashes::WindowHandle makeWindowHandle( wxWindow * window )
	{
#if defined( CU_PlatformWindows )

		return ashes::WindowHandle( c3d::makeRawUnique< ashes::IMswWindowHandle >( ::GetModuleHandle( nullptr )
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
					return ashes::WindowHandle( c3d::makeRawUnique< ashes::IWaylandWindowHandle >( display, surface ) );
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
					return ashes::WindowHandle( c3d::makeRawUnique< ashes::IXWindowHandle >( drawable, display ) );
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
		return ashes::WindowHandle( c3d::makeRawUnique< ashes::IMacOsWindowHandle >( handle ) );

#else

#	error "Yet unsupported platform"

#endif
	}

	c3d::FontUPtr make_Font( wxFont const & wxfont )
	{
		c3d::FontUPtr font;

		if ( wxfont.IsOk() )
		{
			c3d::String name = make_String( wxfont.GetFaceName() ) + c3d::string::toString( wxfont.GetPointSize() );
			font = c3d::makeUnique< c3d::Font >( name, wxfont.GetPointSize() );
			font->setGlyphLoader( c3d::makeRawUnique< helpers::wxWidgetsGlyphLoader >( wxfont ) );
			c3d::Font::BinaryLoader{}( *font
				, c3d::Path{ c3d::String{ wxfont.GetFaceName() } }
				, uint32_t( std::abs( wxfont.GetPointSize() ) ) );
		}

		return font;
	}

	c3d::String make_String( wxString const & value )
	{
		return c3d::makeString( value.mb_str( wxConvUTF8 ).data() );
	}

	c3d::U32String make_U32String( wxString const & value )
	{
		return c3d::toUtf8U32String( make_String( value ) );
	}

	c3d::Path make_Path( wxString const & value )
	{
		return c3d::Path( value.mb_str( wxConvUTF8 ).data() );
	}

	wxString make_wxString( c3d::MbString const & value )
	{
		return wxString( value.c_str(), wxConvUTF8 );
	}

	wxString make_wxString( c3d::WString const & value )
	{
		return wxString( value.c_str(), wxConvUTF8 );
	}

	wxString make_wxString( c3d::U32String const & value )
	{
		return make_wxString( c3d::makeString( value ) );
	}

	wxArrayString make_wxArrayString( c3d::StringArray const & values )
	{
		wxArrayString result;

		for ( auto & value : values )
		{
			result.push_back( make_wxString( value ) );
		}

		return result;
	}

	c3d::StringArray make_StringArray( wxArrayString const & values )
	{
		c3d::StringArray result;

		for ( auto & value : values )
		{
			result.push_back( make_String( value ) );
		}

		return result;
	}

	c3d::Size makeSize( wxSize const & value )
	{
		return c3d::Size( uint32_t( value.x )
			, uint32_t( value.y ) );
	}

	wxSize make_wxSize( c3d::Size const & value )
	{
		return wxSize( int( value.getWidth() )
			, int( value.getHeight() ) );
	}
}
