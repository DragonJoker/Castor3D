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

#include <FunctorEvent.hpp>
#include <InitialiseEvent.hpp>
#include <MaterialManager.hpp>
#include <MeshManager.hpp>
#include <PlatformWindowHandle.hpp>
#include <PluginManager.hpp>
#include <SamplerManager.hpp>
#include <SceneManager.hpp>
#include <SceneFileParser.hpp>
#include <TextureUnit.hpp>
#include <WindowManager.hpp>

#include <PixelBufferBase.hpp>

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

		template< typename TObj >
		std::shared_ptr< TObj > CreateObject( Engine & p_engine )
		{
			return std::make_shared< TObj >( p_engine );
		}

		template<>
		std::shared_ptr< Sampler > CreateObject< Sampler >( Engine & p_engine )
		{
			return p_engine.GetSamplerManager().Create( Castor::String() );
		}

		template< typename TObj, typename TMgr >
		std::shared_ptr< TObj > CreateObject( Engine & p_engine, TMgr & p_manager )
		{
			return p_manager.Create( String(), p_engine );
		}

		template<>
		std::shared_ptr< Sampler > CreateObject< Sampler, SamplerManager >( Engine & p_engine, SamplerManager & p_manager )
		{
			return p_manager.Create( String() );
		}

		template< typename TObj >
		void InitialiseObject( std::shared_ptr< TObj > p_object, Engine & p_engine )
		{
			p_engine.PostEvent( MakeInitialiseEvent( *p_object ) );
		}

		template<>
		void InitialiseObject< Mesh >( std::shared_ptr< Mesh > p_object, Engine & p_engine )
		{
			p_engine.PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [&p_object]()
			{
				p_object->GenerateBuffers();
			} ) );
		}

		template<>
		void InitialiseObject< Scene >( std::shared_ptr< Scene > p_object, Engine & p_engine )
		{
		}

		template< typename TObj, typename TKey >
		bool DoFillCollection( Engine & p_engine, Collection< TObj, TKey > & p_collection, BinaryChunk & p_chunk, typename TObj::BinaryParser p_parser )
		{
			std::shared_ptr< TObj > l_obj = CreateObject< TObj >( p_engine );
			bool l_return = p_parser.Parse( *l_obj, p_chunk );

			if ( l_return )
			{
				if ( !p_collection.has( l_obj->GetName() ) )
				{
					p_collection.insert( l_obj->GetName(), l_obj );
					InitialiseObject( l_obj, p_engine );
				}
				else
				{
					Logger::LogWarning( cuT( "Duplicate object found with name " ) + l_obj->GetName() );
					l_return = false;
				}
			}

			return l_return;
		}

		template< typename TObj >
		bool DoParse( TObj & p_obj, BinaryChunk & p_chunk, typename TObj::BinaryParser & p_parser )
		{
			return p_parser.Parse( p_obj, p_chunk );
		}

		template< typename TObj, typename TKey >
		bool DoFillManager( Engine & p_engine, Manager< TKey, TObj > & p_manager, BinaryChunk & p_chunk, typename TObj::BinaryParser p_parser )
		{
			return DoParse( *CreateObject< TObj >( p_engine, p_manager ), p_chunk, p_parser );
		}

		bool DoFillManager( Engine & p_engine, SamplerManager & p_manager, BinaryChunk & p_chunk, Sampler::BinaryParser p_parser )
		{
			return DoParse< Sampler >( *CreateObject< Sampler >( p_engine, p_manager ), p_chunk, p_parser );
		}

		bool DoLoadMeshFile( Engine & p_engine, Path const & p_fileName )
		{
			bool l_return = true;

			if ( p_fileName.GetExtension() != cuT( "cbsn" ) && p_fileName.GetExtension() != cuT( "zip" ) )
			{
				Path l_meshFilePath = p_fileName;
				string::replace( l_meshFilePath, cuT( "cscn" ), cuT( "cmsh" ) );

				if ( File::FileExists( l_meshFilePath ) )
				{
					BinaryFile l_fileMesh( l_meshFilePath, File::eOPEN_MODE_READ );
					Logger::LogInfo( cuT( "Loading meshes file : " ) + l_meshFilePath );

					if ( p_engine.GetMeshManager().Load( l_fileMesh ) )
					{
						Logger::LogInfo( cuT( "Meshes read" ) );
					}
					else
					{
						Logger::LogInfo( cuT( "Can't read meshes" ) );
						l_return = false;
					}
				}
			}

			return l_return;
		}

		RenderWindowSPtr DoLoadTextSceneFile( Engine & p_engine, Path const & p_fileName )
		{
			RenderWindowSPtr l_return;
			SceneFileParser l_parser( p_engine );

			if ( l_parser.ParseFile( p_fileName ) )
			{
				l_return = l_parser.GetRenderWindow();
			}
			else
			{
				Logger::LogWarning( cuT( "Can't read scene file" ) );
			}

			return l_return;
		}

		RenderWindowSPtr DoLoadBinarySceneFile( Engine & p_engine, Path const & p_fileName )
		{
			RenderWindowSPtr l_return;
			bool l_continue = true;
			BinaryFile l_file( p_fileName, File::eOPEN_MODE_READ );
			BinaryChunk l_chunkFile;
			RenderWindowSPtr l_window;
			Path l_path = l_file.GetFilePath();
			l_chunkFile.Read( l_file );

			if ( l_chunkFile.GetChunkType() == eCHUNK_TYPE_CBSN_FILE )
			{
				while ( l_continue && l_chunkFile.CheckAvailable( 1 ) )
				{
					BinaryChunk l_chunk;
					l_continue = l_chunkFile.GetSubChunk( l_chunk );

					switch ( l_chunk.GetChunkType() )
					{
					case eCHUNK_TYPE_SAMPLER:
						l_continue = DoFillManager( p_engine, p_engine.GetSamplerManager(), l_chunk, Sampler::BinaryParser( l_path ) );
						break;

					case eCHUNK_TYPE_MATERIAL:
						l_continue = DoFillManager( p_engine, p_engine.GetMaterialManager(), l_chunk, Material::BinaryParser( l_path, &p_engine ) );
						break;

					case eCHUNK_TYPE_MESH:
						l_continue = DoFillManager( p_engine, p_engine.GetMeshManager(), l_chunk, Mesh::BinaryParser( l_path ) );
						break;

					case eCHUNK_TYPE_SCENE:
						l_continue = DoFillManager( p_engine, p_engine.GetSceneManager(), l_chunk, Scene::BinaryParser( l_path ) );
						break;

					case eCHUNK_TYPE_WINDOW:
						l_return = p_engine.GetWindowManager().Create();
						l_continue = RenderWindow::BinaryParser( l_path ).Parse( *l_window, l_chunk );
						break;
					}

					if ( !l_continue )
					{
						l_chunk.EndParse();
					}
				}

				if ( l_continue )
				{
					wxMessageBox( _( "Import successful" ) );
				}
			}
			else
			{
				wxMessageBox( _( "The given file is not a valid CBSN file" ) + wxString( wxT( "\n" ) ) + l_file.GetFileName() );
			}

			if ( !l_continue )
			{
				wxMessageBox( _( "Failed to read the binary scene file" ) + wxString( wxT( "\n" ) ) + l_file.GetFileName() );
				Logger::LogWarning( cuT( "Failed to read read binary scene file" ) );
			}

			return l_return;
		}

		RenderWindowSPtr DoLoadSceneFile( Engine & p_engine, Path const & p_fileName )
		{
			RenderWindowSPtr l_return;

			if ( File::FileExists( p_fileName ) )
			{
				Logger::LogInfo( cuT( "Loading scene file : " ) + p_fileName );
				bool l_initialised = false;

				if ( p_fileName.GetExtension() == cuT( "cscn" ) || p_fileName.GetExtension() == cuT( "zip" ) )
				{
					try
					{
						l_return = DoLoadTextSceneFile( p_engine, p_fileName );
					}
					catch ( std::exception & exc )
					{
						wxMessageBox( _( "Failed to parse the scene file, with following error:" ) + wxString( wxT( "\n" ) ) + wxString( exc.what(), wxMBConvLibc() ) );
					}
				}
				else
				{
					try
					{
						l_return = DoLoadBinarySceneFile( p_engine, p_fileName );
					}
					catch ( std::exception & exc )
					{
						wxMessageBox( _( "Failed to parse the binary scene file, with following error:" ) + wxString( wxT( "\n" ) ) + wxString( exc.what(), wxMBConvLibc() ) );
					}
				}
			}
			else
			{
				wxMessageBox( _( "Scene file doesn't exist :" ) + wxString( wxT( "\n" ) ) + p_fileName );
			}

			return l_return;
		}
	}

	void CreateBitmapFromBuffer( uint8_t const * p_buffer, uint32_t p_width, uint32_t p_height, wxBitmap & p_bitmap )
	{
		p_bitmap.Create( p_width, p_height, 24 );
		wxNativePixelData l_data( p_bitmap );

		if ( p_bitmap.IsOk() && uint32_t( l_data.GetWidth() ) == p_width && uint32_t( l_data.GetHeight() ) == p_height )
		{
			wxNativePixelData::Iterator l_it( l_data );
			uint8_t const * l_pBuffer = p_buffer;

			try
			{
				for ( uint32_t i = 0; i < p_height && l_it.IsOk(); i++ )
				{
#if defined( _WIN32 )
					wxNativePixelData::Iterator l_rowStart = l_it;
#endif

					for ( uint32_t j = 0; j < p_width && l_it.IsOk(); j++ )
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

	RenderWindowSPtr LoadScene( Engine & p_engine, String const & p_fileName, uint32_t p_wantedFps, bool p_threaded )
	{
		RenderWindowSPtr l_return;
		Logger::LogDebug( ( wxChar const * )( cuT( "GuiCommon::LoadSceneFile - " ) + p_fileName ).c_str() );

		if ( !p_fileName.empty() )
		{
			String l_strLowered = string::lower_case( p_fileName );
			p_engine.Cleanup();

			bool l_continue = true;
			Logger::LogDebug( cuT( "GuiCommon::LoadSceneFile - Engine cleared" ) );

			if ( DoLoadMeshFile( p_engine, p_fileName ) )
			{
				try
				{
					p_engine.Initialise( p_wantedFps, p_threaded );
				}
				catch ( std::exception & exc )
				{
					wxMessageBox( _( "Castor initialisation failed with following error:" ) + wxString( wxT( "\n" ) ) + wxString( exc.what(), wxMBConvLibc() ) );
				}

				l_return = DoLoadSceneFile( p_engine, p_fileName );
			}
		}

		return l_return;
	}

	void LoadPlugins( Castor3D::Engine & p_engine )
	{
		PathArray l_arrayFiles;
		File::ListDirectoryFiles( Engine::GetPluginsDirectory(), l_arrayFiles );
		PathArray l_arrayKept;

		// Exclude debug plugin in release builds, and release plugins in debug builds
		for ( auto && l_file : l_arrayFiles )
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

			for ( auto && l_file : l_arrayKept )
			{
				if ( l_file.GetExtension() == CASTOR_DLL_EXT )
				{
					// Since techniques depend on renderers, we load these first
					if ( l_file.find( cuT( "RenderSystem" ) ) != String::npos )
					{
						if ( !p_engine.GetPluginManager().LoadPlugin( l_file ) )
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

			// Then we load other plugins
			for ( auto && l_file : l_otherPlugins )
			{
				if ( !p_engine.GetPluginManager().LoadPlugin( l_file ) )
				{
					l_arrayFailed.push_back( l_file );
				}
			}

			if ( !l_arrayFailed.empty() )
			{
				Logger::LogWarning( cuT( "Some plugins couldn't be loaded :" ) );

				for ( auto && l_file : l_arrayFailed )
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
		String l_name = make_String( p_font.GetFaceName() ) + string::to_string( p_font.GetPointSize() );
		FontManager & l_manager = p_engine->GetFontManager();
		FontSPtr l_font = l_manager.get( l_name );

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

	Castor::String make_String( wxString const & p_value )
	{
		return Castor::String( p_value.mb_str( wxConvUTF8 ).data() );
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
