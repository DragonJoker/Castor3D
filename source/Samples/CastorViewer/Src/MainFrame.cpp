#include "RenderPanel.hpp"

#include "MainFrame.hpp"
#include "CastorViewer.hpp"
#include "SceneExporter.hpp"
#include "PropertiesHolder.hpp"

#include <xpms/scene_blanc.xpm>
#include <xpms/mat_blanc.xpm>
#include <xpms/castor_transparent.xpm>
#include <xpms/castor.xpm>
#include <xpms/export.xpm>
#include <xpms/log.xpm>
#include <xpms/properties.xpm>
#include <xpms/node.xpm>
#include <xpms/node_sel.xpm>
#include <xpms/camera.xpm>
#include <xpms/camera_sel.xpm>
#include <xpms/directional.xpm>
#include <xpms/directional_sel.xpm>
#include <xpms/point.xpm>
#include <xpms/point_sel.xpm>
#include <xpms/spot.xpm>
#include <xpms/spot_sel.xpm>
#include <xpms/geometry.xpm>
#include <xpms/geometry_sel.xpm>
#include <xpms/submesh.xpm>
#include <xpms/submesh_sel.xpm>
#include <xpms/scene.xpm>
#include <xpms/scene_sel.xpm>
#include <xpms/panel.xpm>
#include <xpms/panel_sel.xpm>
#include <xpms/border_panel.xpm>
#include <xpms/border_panel_sel.xpm>
#include <xpms/text.xpm>
#include <xpms/text_sel.xpm>
#include <xpms/material.xpm>
#include <xpms/material_sel.xpm>
#include <xpms/pass.xpm>
#include <xpms/pass_sel.xpm>
#include <xpms/texture.xpm>
#include <xpms/texture_sel.xpm>
#include <xpms/viewport.xpm>
#include <xpms/viewport_sel.xpm>
#include <xpms/render_window.xpm>
#include <xpms/render_window_sel.xpm>
#include <xpms/render_target.xpm>
#include <xpms/render_target_sel.xpm>

#include <wx/display.h>
#include <wx/aui/auibar.h>
#include <wx/aui/dockart.h>
#include <wx/renderer.h>

#include <RenderTarget.hpp>
#include <ImagesLoader.hpp>
#include <FunctorEvent.hpp>
#include <InitialiseEvent.hpp>
#include <MaterialsList.hpp>
#include <RendererSelector.hpp>
#include <SplashScreen.hpp>

#include <GeometryBuffers.hpp>
#include <IndexBuffer.hpp>
#include <Material.hpp>
#include <MaterialManager.hpp>
#include <Mesh.hpp>
#include <Pass.hpp>
#include <RenderWindow.hpp>
#include <Sampler.hpp>
#include <Scene.hpp>
#include <SceneFileParser.hpp>
#include <Submesh.hpp>
#include <TextureUnit.hpp>
#include <Vertex.hpp>
#include <VertexBuffer.hpp>
#include <VersionException.hpp>
#include <PluginException.hpp>

using namespace Castor3D;
using namespace Castor;
using namespace GuiCommon;

#define CASTOR3D_THREADED 0

namespace CastorViewer
{
	namespace
	{
#if defined( NDEBUG )
		static const int CASTOR_WANTED_FPS	= 120;
#else
		static const int CASTOR_WANTED_FPS	= 30;
#endif

		typedef enum eID
		{
			eID_TOOL_EXIT,
			eID_TOOL_LOAD_SCENE,
			eID_TOOL_EXPORT_SCENE,
			eID_TOOL_MATERIALS,
			eID_TOOL_SHOW_LOGS,
			eID_TOOL_SHOW_PROPERTIES,
			eID_TOOL_SHOW_LISTS,
		}	eID;

		typedef enum eBMP
		{
			eBMP_SCENES = GuiCommon::eBMP_COUNT,
			eBMP_MATERIALS,
			eBMP_EXPORT,
			eBMP_LOGS,
			eBMP_PROPERTIES,
		}	eBMP;

		void IndentPressedBitmap( wxRect * rect, int button_state )
		{
			if ( button_state == wxAUI_BUTTON_STATE_PRESSED )
			{
				rect->x++;
				rect->y++;
			}
		}

		static wxString AuiChopText( wxDC & dc, const wxString & text, int max_size )
		{
			wxCoord x, y;
			// first check if the text fits with no problems
			dc.GetTextExtent( text, &x, &y );

			if ( x <= max_size )
			{
				return text;
			}

			size_t i, len = text.Length();
			size_t last_good_length = 0;

			for ( i = 0; i < len; ++i )
			{
				wxString s = text.Left( i );
				s += wxT( "..." );
				dc.GetTextExtent( s, &x, &y );

				if ( x > max_size )
				{
					break;
				}

				last_good_length = i;
			}

			wxString ret = text.Left( last_good_length );
			ret += wxT( "..." );
			return ret;
		}

		template< typename TObj >
		std::shared_ptr< TObj > CreateObject( Engine * p_engine )
		{
			return std::make_shared< TObj >( p_engine );
		}

		template<>
		std::shared_ptr< Sampler > CreateObject< Sampler >( Engine * p_engine )
		{
			return p_engine->CreateSampler( Castor::String() );
		}

		template<>
		std::shared_ptr< Scene > CreateObject< Scene >( Engine * p_engine )
		{
			return std::make_shared< Scene >( p_engine, p_engine->GetLightFactory() );
		}

		template< typename TObj >
		void InitialiseObject( std::shared_ptr< TObj > p_object, Engine * p_engine )
		{
			p_engine->PostEvent( MakeInitialiseEvent( *p_object ) );
		}

		template<>
		void InitialiseObject< Mesh >( std::shared_ptr< Mesh > p_object, Engine * p_engine )
		{
			p_engine->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [&p_object]()
			{
				p_object->GenerateBuffers();
			} ) );
		}

		template<>
		void InitialiseObject< Scene >( std::shared_ptr< Scene > p_object, Engine * p_engine )
		{
		}

		template< typename TObj, typename TKey >
		bool FillCollection( Engine * p_engine, Castor::Collection< TObj, TKey > & p_collection, BinaryChunk & p_chunk, typename TObj::BinaryParser p_parser )
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

		class AuiTabArt
			: public wxAuiDefaultTabArt
		{
		public:
			AuiTabArt()
			{
				wxAuiDefaultTabArt::SetColour( INACTIVE_TAB_COLOUR );
				wxAuiDefaultTabArt::SetActiveColour( ACTIVE_TAB_COLOUR );
				wxAuiDefaultTabArt::SetMeasuringFont( wxFont( 8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false ) );
				wxAuiDefaultTabArt::SetNormalFont( wxFont( 8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false ) );
				wxAuiDefaultTabArt::SetSelectedFont( wxFont( 8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false ) );
			}

			virtual wxAuiTabArt * Clone()
			{
				return new AuiTabArt( *this );
			}

			virtual void DrawBackground( wxDC & p_dc, wxWindow * p_window, wxRect const & p_rect )
			{
				p_dc.SetPen( m_baseColourPen );
				p_dc.SetBrush( m_baseColourBrush );
				p_dc.DrawRectangle( p_rect );
			}

			// DrawTab() draws an individual tab.
			//
			// dc       - output dc
			// in_rect  - rectangle the tab should be confined to
			// caption  - tab's caption
			// active   - whether or not the tab is active
			// out_rect - actual output rectangle
			// x_extent - the advance x; where the next tab should start
			void DrawTab( wxDC & dc, wxWindow * wnd, const wxAuiNotebookPage & page, const wxRect & in_rect, int close_button_state, wxRect * out_tab_rect, wxRect * out_button_rect, int * x_extent )
			{
				wxCoord normal_textx, normal_texty;
				wxCoord selected_textx, selected_texty;
				wxCoord texty;

				// if the caption is empty, measure some temporary text
				wxString caption = page.caption;

				if ( caption.empty() )
				{
					caption = wxT( "Xj" );
				}

				dc.SetFont( m_selectedFont );
				dc.GetTextExtent( caption, &selected_textx, &selected_texty );

				dc.SetFont( m_normalFont );
				dc.GetTextExtent( caption, &normal_textx, &normal_texty );

				// figure out the size of the tab
				wxSize tab_size = GetTabSize( dc, wnd, page.caption, page.bitmap, page.active, close_button_state, x_extent );

				wxCoord tab_height = m_tabCtrlHeight - 3;
				wxCoord tab_width = tab_size.x;
				wxCoord tab_x = in_rect.x;
				wxCoord tab_y = in_rect.y + in_rect.height - tab_height;

				caption = page.caption;

				// select pen, brush and font for the tab to be drawn
				if ( page.active )
				{
					dc.SetFont( m_selectedFont );
					texty = selected_texty;
				}
				else
				{
					dc.SetFont( m_normalFont );
					texty = normal_texty;
				}


				// create points that will make the tab outline

				int clip_width = tab_width;

				if ( tab_x + clip_width > in_rect.x + in_rect.width )
				{
					clip_width = ( in_rect.x + in_rect.width ) - tab_x;
				}

				wxPoint clip_points[6];
				clip_points[0] = wxPoint( tab_x,              tab_y + tab_height - 3 );
				clip_points[1] = wxPoint( tab_x,              tab_y + 2 );
				clip_points[2] = wxPoint( tab_x + 2,            tab_y );
				clip_points[3] = wxPoint( tab_x + clip_width - 1, tab_y );
				clip_points[4] = wxPoint( tab_x + clip_width + 1, tab_y + 2 );
				clip_points[5] = wxPoint( tab_x + clip_width + 1, tab_y + tab_height - 3 );
#if !defined(__WXDFB__) && !defined(__WXCOCOA__)
				// since the above code above doesn't play well with WXDFB or WXCOCOA,
				// we'll just use a rectangle for the clipping region for now --
				dc.SetClippingRegion( tab_x, tab_y, clip_width + 1, tab_height - 3 );
#endif // !wxDFB && !wxCocoa

				wxPoint border_points[6];

				if ( m_flags & wxAUI_NB_BOTTOM )
				{
					border_points[0] = wxPoint( tab_x,             tab_y );
					border_points[1] = wxPoint( tab_x,             tab_y + tab_height - 6 );
					border_points[2] = wxPoint( tab_x + 2,           tab_y + tab_height - 4 );
					border_points[3] = wxPoint( tab_x + tab_width - 2, tab_y + tab_height - 4 );
					border_points[4] = wxPoint( tab_x + tab_width,   tab_y + tab_height - 6 );
					border_points[5] = wxPoint( tab_x + tab_width,   tab_y );
				}
				else //if (m_flags & wxAUI_NB_TOP) {}
				{
					border_points[0] = wxPoint( tab_x,             tab_y + tab_height - 4 );
					border_points[1] = wxPoint( tab_x,             tab_y + 2 );
					border_points[2] = wxPoint( tab_x + 2,           tab_y );
					border_points[3] = wxPoint( tab_x + tab_width - 2, tab_y );
					border_points[4] = wxPoint( tab_x + tab_width,   tab_y + 2 );
					border_points[5] = wxPoint( tab_x + tab_width,   tab_y + tab_height - 4 );
				}

				// TODO: else if (m_flags &wxAUI_NB_LEFT) {}
				// TODO: else if (m_flags &wxAUI_NB_RIGHT) {}

				int drawn_tab_yoff = border_points[1].y;
				int drawn_tab_height = border_points[0].y - border_points[1].y;

				if ( page.active )
				{
					// draw active tab

					// draw base background color
					wxRect r( tab_x, tab_y, tab_width, tab_height );
					dc.SetPen( wxPen( m_activeColour ) );
					dc.SetBrush( wxBrush( m_activeColour ) );
					dc.DrawRectangle( r.x + 1, r.y + 1, r.width - 1, r.height - 4 );

					// these two points help the rounded corners appear more antialiased
					dc.SetPen( wxPen( m_activeColour ) );
					dc.DrawPoint( r.x + 2, r.y + 1 );
					dc.DrawPoint( r.x + r.width - 2, r.y + 1 );
				}
				else
				{
					// draw inactive tab

					// draw base background color
					wxRect r( tab_x, tab_y, tab_width, tab_height );
					dc.SetPen( wxPen( m_baseColour ) );
					dc.SetBrush( wxBrush( m_baseColour ) );
					dc.DrawRectangle( r.x + 1, r.y + 1, r.width - 1, r.height - 4 );
				}

				//// draw tab outline
				//dc.SetPen( m_borderPen );
				//dc.SetBrush( *wxTRANSPARENT_BRUSH );
				//dc.DrawPolygon( WXSIZEOF( border_points ), border_points );

				// there are two horizontal grey lines at the bottom of the tab control,
				// this gets rid of the top one of those lines in the tab control
				if ( page.active )
				{
					if ( m_flags & wxAUI_NB_BOTTOM )
					{
						dc.SetPen( wxPen( m_activeColour.ChangeLightness( 170 ) ) );
					}
					// TODO: else if (m_flags &wxAUI_NB_LEFT) {}
					// TODO: else if (m_flags &wxAUI_NB_RIGHT) {}
					else //for wxAUI_NB_TOP
					{
						dc.SetPen( m_activeColour );
					}

					dc.DrawLine( border_points[0].x + 1,
								 border_points[0].y,
								 border_points[5].x,
								 border_points[5].y );
				}


				int text_offset = tab_x + 8;
				int close_button_width = 0;

				if ( close_button_state != wxAUI_BUTTON_STATE_HIDDEN )
				{
					close_button_width = m_activeCloseBmp.GetWidth();
				}

				int bitmap_offset = 0;
				text_offset = tab_x + 8;

				wxString draw_text = AuiChopText( dc, caption, tab_width - ( text_offset - tab_x ) - close_button_width );

				// draw tab text
				if ( page.active )
				{
					dc.SetTextForeground( ACTIVE_TEXT_COLOUR );
				}
				else
				{
					dc.SetTextForeground( INACTIVE_TEXT_COLOUR );
				}

				dc.SetFont( m_normalFont );
				dc.DrawText( draw_text, text_offset, drawn_tab_yoff + ( drawn_tab_height ) / 2 - ( texty / 2 ) );

				// draw focus rectangle
				if ( page.active && ( wnd->FindFocus() == wnd ) )
				{
					wxRect focusRectText( text_offset, ( drawn_tab_yoff + ( drawn_tab_height ) / 2 - ( texty / 2 ) - 1 ), selected_textx, selected_texty );

					wxRect focusRect;
					wxRect focusRectBitmap;

					if ( page.bitmap.IsOk() )
					{
						focusRectBitmap = wxRect( bitmap_offset, drawn_tab_yoff + ( drawn_tab_height / 2 ) - ( page.bitmap.GetHeight() / 2 ), page.bitmap.GetWidth(), page.bitmap.GetHeight() );
					}

					if ( page.bitmap.IsOk() && draw_text.IsEmpty() )
					{
						focusRect = focusRectBitmap;
					}
					else if ( !page.bitmap.IsOk() && !draw_text.IsEmpty() )
					{
						focusRect = focusRectText;
					}
					else if ( page.bitmap.IsOk() && !draw_text.IsEmpty() )
					{
						focusRect = focusRectText.Union( focusRectBitmap );
					}

					focusRect.Inflate( 2, 2 );

					wxRendererNative::Get().DrawFocusRect( wnd, dc, focusRect, 0 );
				}

				// draw close button if necessary
				if ( close_button_state != wxAUI_BUTTON_STATE_HIDDEN )
				{
					wxBitmap bmp = m_disabledCloseBmp;

					if ( close_button_state == wxAUI_BUTTON_STATE_HOVER || close_button_state == wxAUI_BUTTON_STATE_PRESSED )
					{
						bmp = m_activeCloseBmp;
					}

					int offsetY = tab_y - 1;

					if ( m_flags & wxAUI_NB_BOTTOM )
					{
						offsetY = 1;
					}

					wxRect rect( tab_x + tab_width - close_button_width - 1,
								 offsetY + ( tab_height / 2 ) - ( bmp.GetHeight() / 2 ),
								 close_button_width,
								 tab_height );

					IndentPressedBitmap( &rect, close_button_state );
					dc.DrawBitmap( bmp, rect.x, rect.y, true );

					*out_button_rect = rect;
				}

				*out_tab_rect = wxRect( tab_x, tab_y, tab_width, tab_height );

				dc.DestroyClippingRegion();
			}

		private:
			wxColour m_default;
			wxColour m_active;
		};

		class AuiToolBarArt
			: public wxAuiDefaultToolBarArt
		{
		public:
			AuiToolBarArt()
				: wxAuiDefaultToolBarArt()
			{
			}

			virtual wxAuiToolBarArt * Clone()
			{
				return new AuiToolBarArt( *this );
			}

			virtual void DrawBackground( wxDC & p_dc, wxWindow * p_window, wxRect const & p_rect )
			{
				p_dc.SetPen( wxPen( INACTIVE_TAB_COLOUR, 1, wxSOLID ) );
				p_dc.SetBrush( wxBrush( INACTIVE_TAB_COLOUR, wxSOLID ) );
				p_dc.DrawRectangle( p_rect );
			}

			virtual void DrawPlainBackground( wxDC & p_dc, wxWindow * p_window, wxRect const & p_rect )
			{
				p_dc.SetPen( wxPen( INACTIVE_TAB_COLOUR, 1, wxSOLID ) );
				p_dc.SetBrush( wxBrush( INACTIVE_TAB_COLOUR, wxSOLID ) );
				p_dc.DrawRectangle( p_rect );
			}

			virtual void DrawSeparator( wxDC & p_dc, wxWindow * p_window, wxRect const & p_rect )
			{
				p_dc.SetPen( wxPen( BORDER_COLOUR, 1, wxSOLID ) );
				p_dc.DrawLine( ( p_rect.GetBottomLeft() + p_rect.GetBottomRight() ) / 2, ( p_rect.GetTopLeft() + p_rect.GetTopRight() ) / 2 );
			}
		};

		class AuiDockArt
			: public wxAuiDefaultDockArt
		{
		public:
			AuiDockArt()
				: wxAuiDefaultDockArt()
			{
				wxAuiDefaultDockArt::SetMetric( wxAuiPaneDockArtSetting::wxAUI_DOCKART_PANE_BORDER_SIZE, 0 );
				wxAuiDefaultDockArt::SetMetric( wxAuiPaneDockArtSetting::wxAUI_DOCKART_GRADIENT_TYPE, wxAUI_GRADIENT_NONE );
				wxAuiDefaultDockArt::SetColour( wxAuiPaneDockArtSetting::wxAUI_DOCKART_BACKGROUND_COLOUR, INACTIVE_TAB_COLOUR );
				wxAuiDefaultDockArt::SetColour( wxAuiPaneDockArtSetting::wxAUI_DOCKART_BORDER_COLOUR, BORDER_COLOUR );
				wxAuiDefaultDockArt::SetColour( wxAuiPaneDockArtSetting::wxAUI_DOCKART_GRIPPER_COLOUR, wxColour( 127, 127, 127 ) );
				wxAuiDefaultDockArt::SetColour( wxAuiPaneDockArtSetting::wxAUI_DOCKART_SASH_COLOUR, INACTIVE_TAB_COLOUR );
				wxAuiDefaultDockArt::SetColour( wxAuiPaneDockArtSetting::wxAUI_DOCKART_INACTIVE_CAPTION_COLOUR, INACTIVE_TAB_COLOUR );
				wxAuiDefaultDockArt::SetColour( wxAuiPaneDockArtSetting::wxAUI_DOCKART_INACTIVE_CAPTION_GRADIENT_COLOUR, INACTIVE_TAB_COLOUR );
				wxAuiDefaultDockArt::SetColour( wxAuiPaneDockArtSetting::wxAUI_DOCKART_ACTIVE_CAPTION_COLOUR, ACTIVE_TAB_COLOUR );
				wxAuiDefaultDockArt::SetColour( wxAuiPaneDockArtSetting::wxAUI_DOCKART_ACTIVE_CAPTION_GRADIENT_COLOUR, ACTIVE_TAB_COLOUR );
				wxAuiDefaultDockArt::SetColour( wxAuiPaneDockArtSetting::wxAUI_DOCKART_INACTIVE_CAPTION_TEXT_COLOUR, INACTIVE_TEXT_COLOUR );
				wxAuiDefaultDockArt::SetColour( wxAuiPaneDockArtSetting::wxAUI_DOCKART_ACTIVE_CAPTION_TEXT_COLOUR, ACTIVE_TEXT_COLOUR );
			}

			virtual void DrawBackground( wxDC & p_dc, wxWindow * p_window, wxRect const & p_rect )
			{
				p_dc.SetPen( wxPen( PANEL_BACKGROUND_COLOUR, 1, wxSOLID ) );
				p_dc.SetBrush( wxBrush( PANEL_BACKGROUND_COLOUR, wxSOLID ) );
				p_dc.DrawRectangle( p_rect );
			}
		};
	}

	DECLARE_APP( CastorViewerApp )

	MainFrame::MainFrame( wxWindow * p_pParent, wxString const & p_strTitle, eRENDERER_TYPE p_eRenderer )
		: wxFrame( p_pParent, wxID_ANY, p_strTitle, wxDefaultPosition, wxSize( 800, 700 ) )
		, m_pCastor3D( NULL )
		, m_pRenderPanel( NULL )
		, m_pImagesLoader( new wxImagesLoader )
		, m_timer( NULL )
		, m_logTabsContainer( NULL )
		, m_messageLog( NULL )
		, m_errorLog( NULL )
		, m_iLogsHeight( 100 )
		, m_iPropertiesWidth( 240 )
		, m_eRenderer( p_eRenderer )
		, m_sceneObjectsList( NULL )
		, m_materialsList( NULL )
		, m_propertiesContainer( NULL )
		, m_auiManager( this, wxAUI_MGR_ALLOW_FLOATING | wxAUI_MGR_TRANSPARENT_HINT | wxAUI_MGR_HINT_FADE | wxAUI_MGR_VENETIAN_BLINDS_HINT | wxAUI_MGR_LIVE_RESIZE )
		, m_toolBar( NULL )
	{
	}

	MainFrame::~MainFrame()
	{
		m_auiManager.UnInit();
		delete m_pImagesLoader;
	}

	bool MainFrame::Initialise()
	{
		Logger::RegisterCallback( std::bind( &MainFrame::DoLogCallback, this, std::placeholders::_1, std::placeholders::_2 ), this );
		wxDisplay l_display;
		wxRect l_rect = l_display.GetClientArea();
		wxString l_strCopyright;
		l_strCopyright << wxDateTime().Now().GetCurrentYear();
		wxSplashScreen l_splashScreen( this, wxT( "Castor\nViewer" ), l_strCopyright + cuT( " " ) + _( " DragonJoker, All rights shared" ), wxPoint( 10, 230 ), wxPoint( 200, 300 ), wxPoint( 180, 260 ), wxPoint( ( l_rect.width - 512 ) / 2, ( l_rect.height - 384 ) / 2 ), 9 );
		m_pSplashScreen = &l_splashScreen;
		bool l_bReturn = DoInitialiseImages();

		if ( l_bReturn )
		{
			DoPopulateStatusBar();
			DoPopulateToolBar();
			wxIcon l_icon = wxIcon( castor_xpm );
			SetIcon( l_icon );
			DoInitialiseGUI();
			DoInitialisePerspectives();
			l_bReturn = DoInitialise3D();
		}

		l_splashScreen.Close();
		Show( l_bReturn );
		return l_bReturn;
	}

	void MainFrame::LoadScene( wxString const & p_strFileName )
	{
		if ( m_pRenderPanel && m_pCastor3D )
		{
			Logger::LogDebug( ( wxChar const * )( cuT( "MainFrame::LoadScene - " ) + p_strFileName ).c_str() );

			if ( !p_strFileName.empty() )
			{
				m_strFilePath = ( wxChar const * )p_strFileName.c_str();
			}

			if ( !m_strFilePath.empty() )
			{
				String l_strLowered = str_utils::lower_case( m_strFilePath );

				if ( m_pMainScene.lock() )
				{
					m_materialsList->UnloadMaterials();
					m_sceneObjectsList->UnloadScene();
					m_pMainScene.reset();
					Logger::LogDebug( cuT( "MainFrame::LoadScene - Scene unloaded" ) );
				}

				m_pRenderPanel->SetRenderWindow( nullptr );
				m_pCastor3D->Cleanup();
				bool l_continue = true;
				Logger::LogDebug( cuT( "MainFrame::LoadScene - Engine cleared" ) );


				if ( DoLoadMeshFile() )
				{
					try
					{
						m_pCastor3D->Initialise( CASTOR_WANTED_FPS, CASTOR3D_THREADED );
					}
					catch ( std::exception & exc )
					{
						wxMessageBox( _( "Castor initialisation failed with following error:" ) + wxString( wxT( "\n" ) ) + wxString( exc.what(), wxMBConvLibc() ) );
					}

					DoLoadSceneFile();
				}
			}
		}
		else
		{
			wxMessageBox( _( "Can't open a scene file : no engine loaded" ) );
		}
	}

	void MainFrame::ToggleFullScreen( bool p_fullscreen )
	{
		ShowFullScreen( p_fullscreen, wxFULLSCREEN_ALL );

		if ( p_fullscreen )
		{
			m_currentPerspective = m_auiManager.SavePerspective();
			m_auiManager.LoadPerspective( m_fullScreenPerspective );
		}
		else
		{
			m_auiManager.LoadPerspective( m_currentPerspective );
		}
	}

	bool MainFrame::DoLoadMeshFile()
	{
		bool l_return = true;

		if ( m_strFilePath.GetExtension() != cuT( "cbsn" ) && m_strFilePath.GetExtension() != cuT( "zip" ) )
		{
			Path l_meshFilePath = m_strFilePath;
			str_utils::replace( l_meshFilePath, cuT( "cscn" ), cuT( "cmsh" ) );

			if ( File::FileExists( l_meshFilePath ) )
			{
				BinaryFile l_fileMesh( l_meshFilePath, File::eOPEN_MODE_READ );
				Logger::LogInfo( cuT( "Loading meshes file : " ) + l_meshFilePath );

				if ( m_pCastor3D->LoadMeshes( l_fileMesh ) )
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

	bool MainFrame::DoLoadTextSceneFile()
	{
		bool l_return = false;
		SceneFileParser l_parser( m_pCastor3D );

		if ( l_parser.ParseFile( m_strFilePath ) )
		{
			RenderWindowSPtr l_pRenderWindow = l_parser.GetRenderWindow();

			if ( l_pRenderWindow )
			{
				m_pRenderPanel->SetRenderWindow( l_pRenderWindow );

				if ( l_pRenderWindow->IsInitialised() )
				{
					m_pMainScene = l_pRenderWindow->GetScene();

					if ( l_pRenderWindow->IsFullscreen() )
					{
						ShowFullScreen( true, wxFULLSCREEN_ALL );
					}

					SetClientSize( l_pRenderWindow->GetSize().width() + m_iPropertiesWidth, l_pRenderWindow->GetSize().height() + m_iLogsHeight );
					l_return = true;
					Logger::LogInfo( cuT( "Scene file read" ) );
				}
				else
				{
					wxMessageBox( _( "Can't initialise the render window" ) );
				}

#if CASTOR3D_THREADED
				m_pCastor3D->StartRendering();
#endif
			}
		}
		else
		{
			Logger::LogWarning( cuT( "Can't read scene file" ) );
		}

		return l_return;
	}

	bool MainFrame::DoLoadBinarySceneFile()
	{
		bool l_return = true;
		Castor::BinaryFile l_file( m_strFilePath, Castor::File::eOPEN_MODE_READ );
		BinaryChunk l_chunkFile;
		RenderWindowSPtr l_window;
		Path l_path = l_file.GetFilePath();
		l_chunkFile.Read( l_file );

		if ( l_chunkFile.GetChunkType() == eCHUNK_TYPE_CBSN_FILE )
		{
			while ( l_return && l_chunkFile.CheckAvailable( 1 ) )
			{
				BinaryChunk l_chunk;
				l_return = l_chunkFile.GetSubChunk( l_chunk );

				switch ( l_chunk.GetChunkType() )
				{
				case eCHUNK_TYPE_SAMPLER:
					l_return = FillCollection( m_pCastor3D, m_pCastor3D->GetSamplerManager(), l_chunk, Sampler::BinaryParser( l_path ) );
					break;

				case eCHUNK_TYPE_MATERIAL:
					l_return = FillCollection( m_pCastor3D, m_pCastor3D->GetMaterialManager(), l_chunk, Material::BinaryParser( l_path, m_pCastor3D ) );
					break;

				case eCHUNK_TYPE_MESH:
					l_return = FillCollection( m_pCastor3D, m_pCastor3D->GetMeshManager(), l_chunk, Mesh::BinaryParser( l_path ) );
					break;

				case eCHUNK_TYPE_SCENE:
					l_return = FillCollection( m_pCastor3D, m_pCastor3D->GetSceneManager(), l_chunk, Scene::BinaryParser( l_path ) );
					break;

				case eCHUNK_TYPE_WINDOW:
					l_window = m_pCastor3D->CreateRenderWindow();
					l_return = RenderWindow::BinaryParser( l_path ).Parse( *l_window, l_chunk );
					break;
				}

				if ( !l_return )
				{
					l_chunk.EndParse();
				}
			}

			if ( l_return )
			{
				wxMessageBox( _( "Import successful" ) );
			}
		}
		else
		{
			wxMessageBox( _( "The given file is not a valid CBSN file" ) + wxString( wxT( "\n" ) ) + l_file.GetFileName() );
		}

		if ( !l_return )
		{
			wxMessageBox( _( "Failed to read the binary scene file" ) + wxString( wxT( "\n" ) ) + m_strFilePath );
			Logger::LogWarning( cuT( "Failed to read read binary scene file" ) );
		}
		else if ( l_window )
		{
			m_pMainScene = l_window->GetScene();

			if ( l_window->IsFullscreen() )
			{
				ToggleFullScreen( true );
			}

			SetClientSize( l_window->GetSize().width() + m_iPropertiesWidth, l_window->GetSize().height() + m_iLogsHeight );
			l_return = true;
		}

		return l_return;
	}

	void MainFrame::DoLoadSceneFile()
	{
		if ( File::FileExists( m_strFilePath ) )
		{
			Logger::LogInfo( cuT( "Loading scene file : " ) + m_strFilePath );
			bool l_initialised = false;

			if ( m_strFilePath.GetExtension() == cuT( "cscn" ) || m_strFilePath.GetExtension() == cuT( "zip" ) )
			{
				try
				{
					l_initialised = DoLoadTextSceneFile();
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
					l_initialised = DoLoadBinarySceneFile();
				}
				catch ( std::exception & exc )
				{
					wxMessageBox( _( "Failed to parse the binary scene file, with following error:" ) + wxString( wxT( "\n" ) ) + wxString( exc.what(), wxMBConvLibc() ) );
				}
			}

			if ( l_initialised )
			{
				m_sceneObjectsList->LoadScene( m_pCastor3D, m_pMainScene.lock() );
				m_materialsList->LoadMaterials( m_pCastor3D );
				wxSize l_size = GetClientSize();
#if wxCHECK_VERSION( 2, 9, 0 )
				SetMinClientSize( l_size );
#endif
			}
		}
		else
		{
			wxMessageBox( _( "Scene file doesn't exist :" ) + wxString( wxT( "\n" ) ) + m_strFilePath );
		}
	}

	void MainFrame::DoLoadPlugins()
	{
		m_pSplashScreen->Step( _( "Loading plugins" ), 1 );
		PathArray l_arrayFiles;
		PathArray l_arrayFailed;
		std::mutex l_mutex;
		ThreadPtrArray l_arrayThreads;
		File::ListDirectoryFiles( Engine::GetPluginsDirectory(), l_arrayFiles );

		if ( l_arrayFiles.size() > 0 )
		{
			for ( auto && l_file : l_arrayFiles )
			{
				Path l_path( l_file );

				if ( l_path.GetExtension() == CASTOR_DLL_EXT )
				{
					l_arrayThreads.push_back( std::make_shared< std::thread >( [&l_arrayFailed, &l_mutex, this]( Path const & p_path )
					{
						if ( !m_pCastor3D->LoadPlugin( p_path ) )
						{
							std::unique_lock< std::mutex > l_lock( l_mutex );
							l_arrayFailed.push_back( p_path );
						}
					}, l_path ) );
				}
			}

			for ( auto && l_thread : l_arrayThreads )
			{
				l_thread->join();
			}

			l_arrayThreads.clear();
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

		Logger::LogInfo( cuT( "Plugins loaded" ) );
	}

	void MainFrame::DoInitialiseGUI()
	{
		SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		SetClientSize( 800 + m_iPropertiesWidth, 600 + m_iLogsHeight );
		wxSize l_size = GetClientSize();
#if wxCHECK_VERSION( 2, 9, 0 )
		SetMinClientSize( l_size );
#endif
		m_auiManager.SetArtProvider( new AuiDockArt );
		m_pRenderPanel = new RenderPanel( this, wxID_ANY, wxDefaultPosition, wxSize( l_size.x - m_iPropertiesWidth, l_size.y - m_iLogsHeight ) );
		m_logTabsContainer = new wxAuiNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_TOP | wxAUI_NB_TAB_MOVE | wxAUI_NB_TAB_FIXED_WIDTH );
		m_logTabsContainer->SetArtProvider( new AuiTabArt );
		m_sceneTabsContainer = new wxAuiNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_TOP | wxAUI_NB_TAB_MOVE | wxAUI_NB_TAB_FIXED_WIDTH );
		m_sceneTabsContainer->SetArtProvider( new AuiTabArt );
		m_propertiesContainer = new wxPropertiesHolder( false, this, wxDefaultPosition, wxDefaultSize );
		m_propertiesContainer->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_propertiesContainer->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_propertiesContainer->SetCaptionBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_propertiesContainer->SetCaptionTextColour( PANEL_FOREGROUND_COLOUR );
		m_propertiesContainer->SetSelectionBackgroundColour( ACTIVE_TAB_COLOUR );
		m_propertiesContainer->SetSelectionTextColour( ACTIVE_TEXT_COLOUR );
		m_propertiesContainer->SetCellBackgroundColour( INACTIVE_TAB_COLOUR );
		m_propertiesContainer->SetCellTextColour( INACTIVE_TEXT_COLOUR );
		m_propertiesContainer->SetLineColour( BORDER_COLOUR );
		m_propertiesContainer->SetMarginColour( BORDER_COLOUR );

		m_auiManager.AddPane( m_pRenderPanel, wxAuiPaneInfo().CaptionVisible( false ).Center().CloseButton( false ).Name( wxT( "Render" ) ).MinSize( l_size.x - m_iPropertiesWidth, l_size.y - m_iLogsHeight ).Layer( 0 ).Movable( false ).PaneBorder( false ).Dockable( false ) );
		m_auiManager.AddPane( m_logTabsContainer, wxAuiPaneInfo().CaptionVisible( false ).Hide().CloseButton().Name( wxT( "Logs" ) ).Caption( _( "Logs" ) ).Bottom().Dock().BottomDockable().TopDockable().Movable().PinButton().MinSize( l_size.x, m_iLogsHeight ).Layer( 1 ).PaneBorder( false ) );
		m_auiManager.AddPane( m_sceneTabsContainer, wxAuiPaneInfo().CaptionVisible( false ).Hide().CloseButton().Name( wxT( "Objects" ) ).Caption( _( "Objects" ) ).Left().Dock().LeftDockable().RightDockable().Movable().PinButton().MinSize( m_iPropertiesWidth, l_size.y / 3 ).Layer( 2 ).PaneBorder( false ) );
		m_auiManager.AddPane( m_propertiesContainer, wxAuiPaneInfo().CaptionVisible( false ).Hide().CloseButton().Name( wxT( "Properties" ) ).Caption( _( "Properties" ) ).Left().Dock().LeftDockable().RightDockable().Movable().PinButton().MinSize( m_iPropertiesWidth, l_size.y / 3 ).Layer( 2 ).PaneBorder( false ) );


		auto l_logCreator = [this, &l_size]( wxString const & p_name, wxListBox *& p_log )
		{
			p_log = new wxListBox( m_logTabsContainer, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxBORDER_NONE );
			p_log->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
			p_log->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
			m_logTabsContainer->AddPage( p_log, p_name, true );
		};

		l_logCreator( _( "Messages" ), m_messageLog );
		l_logCreator( _( "Errors" ), m_errorLog );

		m_sceneObjectsList = new wxSceneObjectsList( m_propertiesContainer, m_sceneTabsContainer, wxDefaultPosition, wxDefaultSize );
		m_sceneObjectsList->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_sceneObjectsList->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_sceneTabsContainer->AddPage( m_sceneObjectsList, _( "Scenes" ), true );

		m_materialsList = new wxMaterialsList( m_propertiesContainer, m_sceneTabsContainer, wxDefaultPosition, wxDefaultSize );
		m_materialsList->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_materialsList->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_sceneTabsContainer->AddPage( m_materialsList, _( "Materials" ), true );

		m_auiManager.Update();
	}

	bool MainFrame::DoInitialise3D()
	{
		bool l_bReturn = true;
		Logger::LogInfo( cuT( "Initialising Castor3D" ) );
		m_pCastor3D = new Engine();
		DoLoadPlugins();

		try
		{
			if ( m_eRenderer == eRENDERER_TYPE_UNDEFINED )
			{
				wxRendererSelector m_dialog( m_pCastor3D, this, wxT( "Castor Viewer" ) );
				m_pSplashScreen->Step( _( "Initialising main window" ), 1 );
				int l_iReturn = m_dialog.ShowModal();

				if ( l_iReturn == wxID_OK )
				{
					m_eRenderer = m_dialog.GetSelectedRenderer();
				}
				else
				{
					l_bReturn = false;
				}
			}

			if ( l_bReturn )
			{
				l_bReturn = m_pCastor3D->LoadRenderer( m_eRenderer );
			}

			if ( l_bReturn )
			{
				Logger::LogInfo( cuT( "Castor3D Initialised" ) );

#if !CASTOR3D_THREADED

				if ( m_timer == NULL )
				{
					m_timer = new wxTimer( this, 1 );
					m_timer->Start( 1000 / CASTOR_WANTED_FPS );
				}

#endif
			}
		}
		catch ( ... )
		{
			wxMessageBox( _( "Problem occured while initialising Castor3D.\nLook at CastorViewer.log for more details" ), _( "Exception" ), wxOK | wxCENTRE | wxICON_ERROR );
			l_bReturn = false;
		}

		return l_bReturn;
	}

	bool MainFrame::DoInitialiseImages()
	{
		m_pSplashScreen->Step( _( "Loading images" ), 1 );
		m_pImagesLoader->AddBitmap( CV_IMG_CASTOR, castor_transparent_xpm );
		m_pImagesLoader->AddBitmap( eBMP_SCENE, scene_xpm );
		m_pImagesLoader->AddBitmap( eBMP_SCENE_SEL, scene_sel_xpm );
		m_pImagesLoader->AddBitmap( eBMP_NODE, node_xpm );
		m_pImagesLoader->AddBitmap( eBMP_NODE_SEL, node_sel_xpm );
		m_pImagesLoader->AddBitmap( eBMP_CAMERA, camera_xpm );
		m_pImagesLoader->AddBitmap( eBMP_CAMERA_SEL, camera_sel_xpm );
		m_pImagesLoader->AddBitmap( eBMP_GEOMETRY, geometry_xpm );
		m_pImagesLoader->AddBitmap( eBMP_GEOMETRY_SEL, geometry_sel_xpm );
		m_pImagesLoader->AddBitmap( eBMP_DIRECTIONAL_LIGHT, directional_xpm );
		m_pImagesLoader->AddBitmap( eBMP_DIRECTIONAL_LIGHT_SEL, directional_sel_xpm );
		m_pImagesLoader->AddBitmap( eBMP_POINT_LIGHT, point_xpm );
		m_pImagesLoader->AddBitmap( eBMP_POINT_LIGHT_SEL, point_sel_xpm );
		m_pImagesLoader->AddBitmap( eBMP_SPOT_LIGHT, spot_xpm );
		m_pImagesLoader->AddBitmap( eBMP_SPOT_LIGHT_SEL, spot_sel_xpm );
		m_pImagesLoader->AddBitmap( eBMP_SUBMESH, submesh_xpm );
		m_pImagesLoader->AddBitmap( eBMP_SUBMESH_SEL, submesh_sel_xpm );
		m_pImagesLoader->AddBitmap( eBMP_PANEL_OVERLAY, panel_xpm );
		m_pImagesLoader->AddBitmap( eBMP_PANEL_OVERLAY_SEL, panel_sel_xpm );
		m_pImagesLoader->AddBitmap( eBMP_BORDER_PANEL_OVERLAY, border_panel_xpm );
		m_pImagesLoader->AddBitmap( eBMP_BORDER_PANEL_OVERLAY_SEL, border_panel_sel_xpm );
		m_pImagesLoader->AddBitmap( eBMP_TEXT_OVERLAY, text_xpm );
		m_pImagesLoader->AddBitmap( eBMP_TEXT_OVERLAY_SEL, text_sel_xpm );
		m_pImagesLoader->AddBitmap( eBMP_MATERIAL, material_xpm );
		m_pImagesLoader->AddBitmap( eBMP_MATERIAL_SEL, material_sel_xpm );
		m_pImagesLoader->AddBitmap( eBMP_PASS, pass_xpm );
		m_pImagesLoader->AddBitmap( eBMP_PASS_SEL, pass_sel_xpm );
		m_pImagesLoader->AddBitmap( eBMP_TEXTURE, texture_xpm );
		m_pImagesLoader->AddBitmap( eBMP_TEXTURE_SEL, texture_sel_xpm );
		m_pImagesLoader->AddBitmap( eBMP_VIEWPORT, viewport_xpm );
		m_pImagesLoader->AddBitmap( eBMP_VIEWPORT_SEL, viewport_sel_xpm );
		m_pImagesLoader->AddBitmap( eBMP_RENDER_TARGET, render_target_xpm );
		m_pImagesLoader->AddBitmap( eBMP_RENDER_TARGET_SEL, render_target_sel_xpm );
		m_pImagesLoader->AddBitmap( eBMP_RENDER_WINDOW, render_window_xpm );
		m_pImagesLoader->AddBitmap( eBMP_RENDER_WINDOW_SEL, render_window_sel_xpm );

		m_pImagesLoader->AddBitmap( eBMP_SCENES, scene_blanc_xpm );
		m_pImagesLoader->AddBitmap( eBMP_MATERIALS, mat_blanc_xpm );
		m_pImagesLoader->AddBitmap( eBMP_EXPORT, export_xpm );
		m_pImagesLoader->AddBitmap( eBMP_LOGS, log_xpm );
		m_pImagesLoader->AddBitmap( eBMP_PROPERTIES, properties_xpm );
		m_pImagesLoader->WaitAsyncLoads();
		return true;
	}

	void MainFrame::DoPopulateStatusBar()
	{
		wxStatusBar * l_statusBar = CreateStatusBar();
		l_statusBar->SetBackgroundColour( INACTIVE_TAB_COLOUR );
		l_statusBar->SetForegroundColour( INACTIVE_TEXT_COLOUR );
	}

	void MainFrame::DoPopulateToolBar()
	{
		m_pSplashScreen->Step( _( "Loading toolbar" ), 1 );
		m_toolBar = new wxAuiToolBar( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_PLAIN_BACKGROUND | wxAUI_TB_HORIZONTAL );
		m_toolBar->SetArtProvider( new AuiToolBarArt );
		m_toolBar->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_toolBar->SetToolBitmapSize( wxSize( 32, 32 ) );
		m_toolBar->AddTool( eID_TOOL_LOAD_SCENE, _( "Load Scene" ), wxImage( *m_pImagesLoader->GetBitmap( eBMP_SCENES ) ).Rescale( 32, 32, wxIMAGE_QUALITY_HIGH ), _( "Open a new scene" ) );
		m_pSplashScreen->Step( 1 );
		m_toolBar->AddTool( eID_TOOL_EXPORT_SCENE, _( "Export Scene" ), wxImage( *m_pImagesLoader->GetBitmap( eBMP_EXPORT ) ).Rescale( 32, 32, wxIMAGE_QUALITY_HIGH ), _( "Export the current scene" ) );
		m_pSplashScreen->Step( 1 );
		m_toolBar->AddSeparator();
		m_toolBar->AddTool( eID_TOOL_SHOW_LOGS, _( "Logs" ), wxImage( *m_pImagesLoader->GetBitmap( eBMP_LOGS ) ).Rescale( 32, 32, wxIMAGE_QUALITY_HIGH ), _( "Display logs" ) );
		m_pSplashScreen->Step( 1 );
		m_toolBar->AddTool( eID_TOOL_SHOW_LISTS, _( "Lists" ), wxImage( *m_pImagesLoader->GetBitmap( eBMP_MATERIALS ) ).Rescale( 32, 32, wxIMAGE_QUALITY_HIGH ), _( "Display lists" ) );
		m_pSplashScreen->Step( 1 );
		m_toolBar->AddTool( eID_TOOL_SHOW_PROPERTIES, _( "Properties" ), wxImage( *m_pImagesLoader->GetBitmap( eBMP_PROPERTIES ) ).Rescale( 32, 32, wxIMAGE_QUALITY_HIGH ), _( "Display properties" ) );
		m_pSplashScreen->Step( 1 );
		m_toolBar->Realize();
		m_auiManager.AddPane( m_toolBar, wxAuiPaneInfo().Name( wxT( "MainToolBar" ) ).ToolbarPane().Top().Row( 1 ).Dockable( false ).Gripper( false ) );
	}

	void MainFrame::DoInitialisePerspectives()
	{
		wxAuiPaneInfoArray l_panes = m_auiManager.GetAllPanes();
		std::vector< bool > l_visibilities;
		m_currentPerspective = m_auiManager.SavePerspective();

		for ( size_t i = 0; i < l_panes.size(); ++i )
		{
			l_panes[i].Hide();
		}

		m_auiManager.GetPane( m_toolBar ).Hide();
		m_fullScreenPerspective = m_auiManager.SavePerspective();
		m_auiManager.LoadPerspective( m_currentPerspective );
	}

	void MainFrame::DoLogCallback( String const & p_strLog, ELogType p_eLogType )
	{
		switch ( p_eLogType )
		{
		case Castor::ELogType_DEBUG:
		case Castor::ELogType_INFO:
			if ( m_messageLog )
			{
				m_messageLog->Insert( wxArrayString( 1, p_strLog ), 0 );
			}

			break;

		case Castor::ELogType_WARNING:
		case Castor::ELogType_ERROR:
			if ( m_errorLog )
			{
				m_errorLog->Insert( wxArrayString( 1, p_strLog ), 0 );
			}

			break;

		default:
			break;
		}
	}

	BEGIN_EVENT_TABLE( MainFrame, wxFrame )
		EVT_TIMER( 1, MainFrame::OnTimer )
		EVT_PAINT( MainFrame::OnPaint )
		EVT_INIT_DIALOG( MainFrame::OnInit )
		EVT_CLOSE( MainFrame::OnClose )
		EVT_ENTER_WINDOW( MainFrame::OnEnterWindow )
		EVT_LEAVE_WINDOW( MainFrame::OnLeaveWindow )
		EVT_ERASE_BACKGROUND( MainFrame::OnEraseBackground )
		EVT_TOOL( eID_TOOL_LOAD_SCENE, MainFrame::OnLoadScene )
		EVT_TOOL( eID_TOOL_EXPORT_SCENE, MainFrame::OnExportScene )
		EVT_TOOL( eID_TOOL_SHOW_LOGS, MainFrame::OnShowLogs )
		EVT_TOOL( eID_TOOL_SHOW_LISTS, MainFrame::OnShowLists )
		EVT_TOOL( eID_TOOL_SHOW_PROPERTIES, MainFrame::OnShowProperties )
	END_EVENT_TABLE()

	void MainFrame::OnPaint( wxPaintEvent & p_event )
	{
		wxPaintDC l_paintDC( this );
		p_event.Skip();
	}

	void MainFrame::OnTimer( wxTimerEvent & p_event )
	{
		if ( m_pCastor3D )
		{
			m_pCastor3D->RenderOneFrame();
		}

		p_event.Skip();
	}

	void MainFrame::OnInit( wxInitDialogEvent & p_event )
	{
	}

	void MainFrame::OnClose( wxCloseEvent & p_event )
	{
		Logger::UnregisterCallback( this );
		m_auiManager.DetachPane( m_sceneTabsContainer );
		m_auiManager.DetachPane( m_propertiesContainer );
		m_auiManager.DetachPane( m_logTabsContainer );
		m_auiManager.DetachPane( m_pRenderPanel );
		m_messageLog = NULL;
		m_errorLog = NULL;
		Hide();

		if ( m_timer )
		{
			m_timer->Stop();
			delete m_timer;
			m_timer = nullptr;
		}

		m_pMainScene.reset();

		if ( m_pRenderPanel )
		{
			m_pRenderPanel->SetRenderWindow( nullptr );
		}

		if ( m_pCastor3D )
		{
#if CASTOR3D_THREADED
			m_pCastor3D->EndRendering();
#endif
			m_pCastor3D->Cleanup();
		}

		if ( m_pRenderPanel )
		{
			m_pRenderPanel->UnFocus();
			m_pRenderPanel->Close( true );
			m_pRenderPanel = NULL;
		}

		DestroyChildren();

		if ( m_pImagesLoader )
		{
			m_pImagesLoader->Cleanup();
		}

		delete m_pCastor3D;
		m_pCastor3D = NULL;
		p_event.Skip();
	}

	void MainFrame::OnEnterWindow( wxMouseEvent & p_event )
	{
		SetFocus();
		p_event.Skip();
	}

	void MainFrame::OnLeaveWindow( wxMouseEvent & p_event )
	{
		p_event.Skip();
	}

	void MainFrame::OnEraseBackground( wxEraseEvent & p_event )
	{
		p_event.Skip();
	}

	void MainFrame::OnLoadScene( wxCommandEvent & p_event )
	{
		wxString l_wildcard = _( "Castor3D scene files" );
		l_wildcard += wxT( " (*.cscn;*.cbsn;*.zip)|*.cscn;*.cbsn;*.zip|" );
		l_wildcard += _( "Castor3D text scene file" );
		l_wildcard += wxT( " (*.cscn)|*.cscn|" );
		l_wildcard += _( "Castor3D binary scene file" );
		l_wildcard += wxT( " (*.cbsn)|*.cbsn|" );
		l_wildcard += _( "Zip archive" );
		l_wildcard += wxT( " (*.zip)|*.zip" );
		wxFileDialog l_fileDialog( this, _( "Open a scene" ), wxEmptyString, wxEmptyString, l_wildcard );

		if ( l_fileDialog.ShowModal() == wxID_OK )
		{
			LoadScene( ( wxChar const * )l_fileDialog.GetPath().c_str() );
		}

		p_event.Skip();
	}

	void MainFrame::OnExportScene( wxCommandEvent & p_event )
	{
		wxString l_wildcard = _( "Castor3D text scene" );
		l_wildcard += wxT( " (*.cscn)|*.cscn|" );
		l_wildcard += _( "Castor3D binary scene" );
		l_wildcard += wxT( " (*.cbsn)|*.cbsn|" );
		l_wildcard += _( "Wavefront OBJ" );
		l_wildcard += wxT( " (*.obj)|*.obj" );
		wxFileDialog l_fileDialog( this, _( "Export the scene" ), wxEmptyString, wxEmptyString, l_wildcard, wxFD_SAVE | wxFD_OVERWRITE_PROMPT );
		SceneSPtr l_scene = m_pMainScene.lock();

		if ( l_scene )
		{
			if ( l_fileDialog.ShowModal() == wxID_OK )
			{
				Path l_pathFile( ( wxChar const * )l_fileDialog.GetPath().c_str() );

				if ( l_pathFile.GetExtension() == cuT( "obj" ) )
				{
					ObjSceneExporter l_exporter;
					l_exporter.ExportScene( m_pCastor3D, *m_pMainScene.lock(), l_pathFile );
				}
				else if ( l_pathFile.GetExtension() == cuT( "cscn" ) )
				{
					CscnSceneExporter l_exporter;
					l_exporter.ExportScene( m_pCastor3D, *m_pMainScene.lock(), l_pathFile );
				}
				else if ( l_pathFile.GetExtension() == cuT( "cbsn" ) )
				{
					CbsnSceneExporter l_exporter;
					l_exporter.ExportScene( m_pCastor3D, *m_pMainScene.lock(), l_pathFile );
				}
			}
		}
		else
		{
			wxMessageBox( _( "No scene Loaded." ), _( "Error" ), wxOK | wxCENTRE | wxICON_ERROR );
		}

		p_event.Skip();
	}

	void MainFrame::OnShowLogs( wxCommandEvent & p_event )
	{
		if ( !m_logTabsContainer->IsShown() )
		{
			m_auiManager.GetPane( m_logTabsContainer ).Show();
		}
		else
		{
			m_auiManager.GetPane( m_logTabsContainer ).Hide();
		}

		m_auiManager.Update();
		p_event.Skip();
	}

	void MainFrame::OnShowProperties( wxCommandEvent & p_event )
	{
		if ( !m_propertiesContainer->IsShown() )
		{
			m_auiManager.GetPane( m_propertiesContainer ).Show();
		}
		else
		{
			m_auiManager.GetPane( m_propertiesContainer ).Hide();
		}

		m_auiManager.Update();
		p_event.Skip();
	}

	void MainFrame::OnShowLists( wxCommandEvent & p_event )
	{
		if ( !m_sceneTabsContainer->IsShown() )
		{
			m_auiManager.GetPane( m_sceneTabsContainer ).Show();
		}
		else
		{
			m_auiManager.GetPane( m_sceneTabsContainer ).Hide();
		}

		m_auiManager.Update();
		p_event.Skip();
	}
}
