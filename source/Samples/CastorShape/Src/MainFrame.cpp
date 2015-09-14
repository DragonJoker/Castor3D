#include "MainFrame.hpp"

#include "CastorShape.hpp"
#include "RenderPanel.hpp"

#include "NewConeDialog.hpp"
#include "NewCubeDialog.hpp"
#include "NewCylinderDialog.hpp"
#include "NewIcosahedronDialog.hpp"
#include "NewPlaneDialog.hpp"
#include "NewSphereDialog.hpp"
#include "NewTorusDialog.hpp"
#include "NewMaterialDialog.hpp"
#include "RenderEngine.hpp"

#include <xpms/border_panel.xpm>
#include <xpms/border_panel_sel.xpm>
#include <xpms/camera.xpm>
#include <xpms/camera_sel.xpm>
#include <xpms/castor.xpm>
#include <xpms/castor_transparent.xpm>
#include <xpms/directional.xpm>
#include <xpms/directional_sel.xpm>
#include <xpms/export.xpm>
#include <xpms/frame_variable.xpm>
#include <xpms/frame_variable_sel.xpm>
#include <xpms/frame_variable_buffer.xpm>
#include <xpms/frame_variable_buffer_sel.xpm>
#include <xpms/geometry.xpm>
#include <xpms/geometry_sel.xpm>
#include <xpms/log.xpm>
#include <xpms/mat_blanc.xpm>
#include <xpms/material.xpm>
#include <xpms/material_sel.xpm>
#include <xpms/node.xpm>
#include <xpms/node_sel.xpm>
#include <xpms/panel.xpm>
#include <xpms/panel_sel.xpm>
#include <xpms/pass.xpm>
#include <xpms/pass_sel.xpm>
#include <xpms/point.xpm>
#include <xpms/point_sel.xpm>
#include <xpms/properties.xpm>
#include <xpms/render_target.xpm>
#include <xpms/render_target_sel.xpm>
#include <xpms/render_window.xpm>
#include <xpms/render_window_sel.xpm>
#include <xpms/scene.xpm>
#include <xpms/scene_blanc.xpm>
#include <xpms/scene_sel.xpm>
#include <xpms/spot.xpm>
#include <xpms/spot_sel.xpm>
#include <xpms/submesh.xpm>
#include <xpms/submesh_sel.xpm>
#include <xpms/text.xpm>
#include <xpms/text_sel.xpm>
#include <xpms/texture.xpm>
#include <xpms/texture_sel.xpm>
#include <xpms/viewport.xpm>
#include <xpms/viewport_sel.xpm>

#include <xpms/castor_dark.xpm>
#include <xpms/fichier.xpm>
#include <xpms/ajouter.xpm>
#include <xpms/parametres.xpm>

#include <MaterialsList.hpp>
#include <PropertiesHolder.hpp>
#include <SceneObjectsList.hpp>
#include <AuiDockArt.hpp>
#include <AuiTabArt.hpp>
#include <AuiToolBarArt.hpp>

#include <ImporterPlugin.hpp>
#include <DividerPlugin.hpp>
#include <MaterialManager.hpp>

#define ID_NEW_WINDOW 10000

using namespace Castor3D;
using namespace Castor;
using namespace GuiCommon;

namespace CastorShape
{
	enum eMENU_BAR_IDs
	{
		eExit,
		eFile,
		eNew,
		eSettings,
		eSaveScene,
		eLoadScene,
		eNewGeometry,
		eNewCone,
		eNewCube,
		eNewCylinder,
		eNewIcosahedron,
		eNewPlane,
		eNewSphere,
		eNewTorus,
		eNewProjection,
		eNewMaterial,
		eProperties,
		eLists,
		eSelectGeometries,
		eSelectPoints,
		eCloneSelection,
		eSubdividePNTriangles,
		eSubdivideLoop,
		eSelectNone,
		eSelect,
		eModify,
		eNone,
		eRender,
	};

	typedef enum eID
	{
		eID_RENDER_TIMER,
	}	eID;

	static const int CASTOR_WANTED_FPS = 35;

	DECLARE_APP( CastorShapeApp )

	MainFrame::MainFrame( wxWindow * parent, wxString const & p_strTitle, eRENDERER_TYPE p_eRenderer )
		: wxFrame( parent, wxID_ANY, p_strTitle, wxDefaultPosition, wxSize( 800, 700 ) )
		, m_castor3D( NULL )
		, m_3dFrame( NULL )
		, m_2dFrameHD( NULL )
		, m_2dFrameBG( NULL )
		, m_2dFrameBD( NULL )
		, m_selectedFrame( NULL )
		, m_bWireFrame( false )
		, m_bMultiFrames( false )
		, m_pImagesLoader( new ImagesLoader )
		, m_timer( NULL )
		, m_iNbGeometries( 0 )
		, m_eRenderer( p_eRenderer )
		, m_auiManager( this, wxAUI_MGR_ALLOW_FLOATING | wxAUI_MGR_TRANSPARENT_HINT | wxAUI_MGR_HINT_FADE | wxAUI_MGR_VENETIAN_BLINDS_HINT | wxAUI_MGR_LIVE_RESIZE )
		, m_iPropertiesWidth( 240 )
	{
		m_selectedMaterial.m_ambient[0] = 0.2f;
		m_selectedMaterial.m_ambient[1] = 0.0f;
		m_selectedMaterial.m_ambient[2] = 0.0f;
		m_selectedMaterial.m_emissive[0] = 0.2f;
		m_selectedMaterial.m_emissive[1] = 0.0f;
		m_selectedMaterial.m_emissive[2] = 0.0f;
	}

	MainFrame::~MainFrame()
	{
		m_auiManager.UnInit();
		delete m_pImagesLoader;
	}

	bool MainFrame::Initialise()
	{
		wxDisplay l_display;
		wxRect l_rect = l_display.GetClientArea();
		wxString l_strCopyright;
		l_strCopyright << wxDateTime().Now().GetCurrentYear();
		SplashScreen l_splashScreen( this, wxT( "Castor\nShape" ), l_strCopyright + cuT( " " ) + _( " DragonJoker, All rights shared" ), wxPoint( 10, 230 ), wxPoint( 200, 300 ), wxPoint( 180, 260 ), wxPoint( ( l_rect.width - 512 ) / 2, ( l_rect.height - 384 ) / 2 ), 9 );
		m_pSplashScreen = &l_splashScreen;
		bool l_return = DoInitialiseImages();

		if ( l_return )
		{
			DoPopulateStatusBar();
			DoPopulateToolbar();
			wxIcon l_icon = wxIcon( castor_dark_xpm );
			SetIcon( l_icon );
			l_return = DoInitialise3D();

			if ( l_return )
			{
				DoInitialiseGUI();
				DoInitialisePerspectives();
			}
		}

		l_splashScreen.Close();
		Show( l_return );
		return l_return;
	}

	void MainFrame::SelectGeometry( GeometrySPtr p_geometry )
	{
		if ( p_geometry != m_selectedGeometry )
		{
			if ( m_selectedGeometry )
			{
				MeshSPtr l_mesh = m_selectedGeometry->GetMesh();
				uint32_t l_uiIndex = 0;

				for ( auto && l_submesh : *l_mesh )
				{
					MaterialInfos * l_infos = m_selectedGeometryMaterials[l_uiIndex++];
					m_selectedGeometry->GetMaterial( l_submesh )->GetPass( 0 )->SetAmbient( Colour::from_rgb( l_infos->m_ambient ) );
					m_selectedGeometry->GetMaterial( l_submesh )->GetPass( 0 )->SetEmissive( Colour::from_rgb( l_infos->m_emissive ) );
				}

				clear_container( m_selectedGeometryMaterials );
			}

			m_selectedGeometry = p_geometry;

			if ( m_selectedGeometry )
			{
				MeshSPtr l_mesh = m_selectedGeometry->GetMesh();

				for ( auto && l_submesh : *l_mesh )
				{
					MaterialInfos * l_infos = new MaterialInfos;
					l_infos->m_ambient[0] = m_selectedGeometry->GetMaterial( l_submesh )->GetPass( 0 )->GetAmbient().red();
					l_infos->m_ambient[1] = m_selectedGeometry->GetMaterial( l_submesh )->GetPass( 0 )->GetAmbient().green();
					l_infos->m_ambient[2] = m_selectedGeometry->GetMaterial( l_submesh )->GetPass( 0 )->GetAmbient().blue();
					l_infos->m_emissive[0] = m_selectedGeometry->GetMaterial( l_submesh )->GetPass( 0 )->GetEmissive().red();
					l_infos->m_emissive[1] = m_selectedGeometry->GetMaterial( l_submesh )->GetPass( 0 )->GetEmissive().green();
					l_infos->m_emissive[2] = m_selectedGeometry->GetMaterial( l_submesh )->GetPass( 0 )->GetEmissive().blue();
					m_selectedGeometryMaterials.push_back( l_infos );
					m_selectedGeometry->GetMaterial( l_submesh )->GetPass( 0 )->SetAmbient( Colour::from_rgb( m_selectedMaterial.m_ambient ) );
					m_selectedGeometry->GetMaterial( l_submesh )->GetPass( 0 )->SetEmissive( Colour::from_rgb( m_selectedMaterial.m_emissive ) );
				}
			}
		}
	}

	void MainFrame::SelectVertex( Vertex * CU_PARAM_UNUSED( p_vertex ) )
	{
	}

	void MainFrame::ShowPanels()
	{
		if ( m_3dFrame )
		{
			m_3dFrame->DrawOneFrame();

			if ( m_bMultiFrames )
			{
				m_2dFrameHD->DrawOneFrame();
				m_2dFrameBG->DrawOneFrame();
				m_2dFrameBD->DrawOneFrame();
			}
		}
	}

	void MainFrame::SetCurrentPanel( RenderPanel * p_pCheck, RenderPanel * p_pValue )
	{
		if ( p_pValue == NULL )
		{
			if ( m_selectedFrame == p_pCheck )
			{
				m_selectedFrame = NULL;
			}
		}
		else
		{
			m_selectedFrame = p_pValue;
		}
	}

	void MainFrame::LoadScene( wxString const & p_strFileName )
	{
		if ( ! p_strFileName.empty() )
		{
			m_strFilePath = ( wxChar const * )p_strFileName.c_str();
		}

		if ( ! m_strFilePath.empty() )
		{
			String l_strLowered = str_utils::lower_case( m_strFilePath );
			m_materialsList->UnloadMaterials();
			m_sceneObjectsList->UnloadScene();

			if ( str_utils::lower_case( m_strFilePath.GetExtension() ) == cuT( "cscn" ) || str_utils::lower_case( m_strFilePath.GetExtension() ) == cuT( "cbsn" ) )
			{
				GuiCommon::LoadScene( *m_castor3D, m_strFilePath, CASTOR_WANTED_FPS, false );
			}
			else
			{
				ImporterPluginSPtr l_pPlugin;
				ImporterSPtr l_pImporter;
				ImporterPlugin::ExtensionArray l_arrayExtensions;

				for ( auto && l_it = m_castor3D->PluginsBegin( ePLUGIN_TYPE_IMPORTER ); l_it != m_castor3D->PluginsEnd( ePLUGIN_TYPE_IMPORTER ) && !l_pImporter; ++l_it )
				{
					l_pPlugin = std::static_pointer_cast< ImporterPlugin, PluginBase >( l_it->second );

					if ( l_pPlugin )
					{
						l_arrayExtensions = l_pPlugin->GetExtensions();

						for ( ImporterPlugin::ExtensionArrayIt l_itExt = l_arrayExtensions.begin(); l_itExt != l_arrayExtensions.end() && !l_pImporter; ++l_itExt )
						{
							if ( str_utils::lower_case( m_strFilePath.GetExtension() ) == str_utils::lower_case( l_itExt->first ) )
							{
								l_pImporter = l_pPlugin->GetImporter();
							}
						}
					}
				}

				if ( l_pImporter )
				{
					m_mainScene.lock()->ImportExternal( m_strFilePath, *l_pImporter );
				}
			}

			m_sceneObjectsList->LoadScene( m_castor3D, m_mainScene.lock() );
			m_materialsList->LoadMaterials( m_castor3D );
		}
	}

	void MainFrame::DoLoadPlugins()
	{
		m_pSplashScreen->Step( _( "Loading plugins" ), 1 );
		GuiCommon::LoadPlugins( *m_castor3D );
	}

	void MainFrame::DoInitialiseGUI()
	{
		SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		SetClientSize( 800 + m_iPropertiesWidth, 600 );
		wxSize l_size = GetClientSize();
#if wxCHECK_VERSION( 2, 9, 0 )
		SetMinClientSize( l_size );
#endif
		m_auiManager.SetArtProvider( new AuiDockArt );
		m_renderPanelsContainer = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxSize( l_size.x - m_iPropertiesWidth, l_size.y ) );
		m_sceneTabsContainer = new wxAuiNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_TOP | wxAUI_NB_TAB_MOVE | wxAUI_NB_TAB_FIXED_WIDTH );
		m_sceneTabsContainer->SetArtProvider( new AuiTabArt );
		m_propertiesContainer = new PropertiesHolder( true, this, wxDefaultPosition, wxDefaultSize );
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

		m_auiManager.AddPane( m_renderPanelsContainer, wxAuiPaneInfo().CaptionVisible( false ).Center().CloseButton( false ).Name( wxT( "Render" ) ).MinSize( l_size.x - m_iPropertiesWidth, l_size.y ).Layer( 0 ).Movable( false ).PaneBorder( false ).Dockable( false ) );
		m_auiManager.AddPane( m_sceneTabsContainer, wxAuiPaneInfo().CaptionVisible( false ).Hide().CloseButton().Name( wxT( "Objects" ) ).Caption( _( "Objects" ) ).Left().Dock().LeftDockable().RightDockable().Movable().PinButton().MinSize( m_iPropertiesWidth, l_size.y / 3 ).Layer( 1 ).PaneBorder( false ) );
		m_auiManager.AddPane( m_propertiesContainer, wxAuiPaneInfo().CaptionVisible( false ).Hide().CloseButton().Name( wxT( "Properties" ) ).Caption( _( "Properties" ) ).Left().Dock().LeftDockable().RightDockable().Movable().PinButton().MinSize( m_iPropertiesWidth, l_size.y / 3 ).Layer( 1 ).PaneBorder( false ) );

		m_renderPanelsContainer->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_renderPanelsContainer->SetForegroundColour( PANEL_FOREGROUND_COLOUR );

		m_sceneObjectsList = new SceneObjectsList( m_propertiesContainer, m_sceneTabsContainer, wxDefaultPosition, wxDefaultSize );
		m_sceneObjectsList->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_sceneObjectsList->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_sceneTabsContainer->AddPage( m_sceneObjectsList, _( "Scenes" ), true );

		m_materialsList = new MaterialsList( m_propertiesContainer, m_sceneTabsContainer, wxDefaultPosition, wxDefaultSize );
		m_materialsList->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_materialsList->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_sceneTabsContainer->AddPage( m_materialsList, _( "Materials" ), true );

		int l_width = l_size.x / ( m_bMultiFrames ? 2 : 1 );
		int l_height = l_size.y / ( m_bMultiFrames ? 2 : 1 );
		SceneSPtr l_scene = m_mainScene.lock();

		if ( l_scene )
		{
			m_3dFrame = new RenderPanel( this, m_renderPanelsContainer, wxID_ANY, eVIEWPORT_TYPE_3D, l_scene, wxPoint( 0, 0 ), wxSize( l_width - 1, l_height - 1 ) );

			if ( m_bMultiFrames )
			{
				m_2dFrameHD = new RenderPanel( this, m_renderPanelsContainer, wxID_ANY, eVIEWPORT_TYPE_2D, l_scene, wxPoint( l_width + 1, 0 ), wxSize( l_width - 1, l_height - 1 ) );
				m_2dFrameBG = new RenderPanel( this, m_renderPanelsContainer, wxID_ANY, eVIEWPORT_TYPE_2D, l_scene, wxPoint( 0, l_height + 1 ), wxSize( l_width - 1, l_height - 1 ), ePROJECTION_DIRECTION_LEFT );
				m_2dFrameBD = new RenderPanel( this, m_renderPanelsContainer, wxID_ANY, eVIEWPORT_TYPE_2D, l_scene, wxPoint( l_width + 1, l_height + 1 ), wxSize( l_width - 1, l_height - 1 ), ePROJECTION_DIRECTION_TOP );
				m_separator1 = new wxPanel( m_renderPanelsContainer, wxID_ANY, wxPoint( 0, l_height - 1 ), wxSize( l_width, 2 ) );
				m_separator2 = new wxPanel( m_renderPanelsContainer, wxID_ANY, wxPoint( l_width, l_height - 1 ), wxSize( l_width, 2 ) );
				m_separator3 = new wxPanel( m_renderPanelsContainer, wxID_ANY, wxPoint( l_width - 1, 0 ), wxSize( 2, l_height ) );
				m_separator4 = new wxPanel( m_renderPanelsContainer, wxID_ANY, wxPoint( l_width - 1, l_height ), wxSize( 2, l_height ) );
			}

			m_3dFrame->Show();

			if ( m_bMultiFrames )
			{
				m_2dFrameHD->Show();
				m_2dFrameBG->Show();
				m_2dFrameBD->Show();
			}

			m_3dFrame->InitialiseRenderWindow();

			if ( m_bMultiFrames )
			{
				m_2dFrameHD->InitialiseRenderWindow();
				m_2dFrameBG->InitialiseRenderWindow();
				m_2dFrameBD->InitialiseRenderWindow();
			}

			ShowPanels();
			LightSPtr l_light1 = l_scene->CreateLight( cuT( "Light1" ), l_scene->CreateSceneNode( cuT( "Light1Node" ) ), eLIGHT_TYPE_DIRECTIONAL );

			if ( l_light1 )
			{
				l_light1->GetDirectionalLight()->SetDirection( Point3r( 0.0f, 0.0f, 1.0f ) );
				l_light1->GetDirectionalLight()->SetDiffuse( Point4f( 1.0f, 1.0f, 1.0f, 1.0f ) );
				l_light1->GetDirectionalLight()->SetSpecular( Point4f( 1.0f, 1.0f, 1.0f, 1.0f ) );
				l_light1->SetEnabled( true );
			}

			LightSPtr l_light2 = l_scene->CreateLight( cuT( "Light2" ), l_scene->CreateSceneNode( cuT( "Light2Node" ) ), eLIGHT_TYPE_DIRECTIONAL );

			if ( l_light2 )
			{
				l_light2->GetDirectionalLight()->SetDirection( Point3r( 0.0f, -1.0f, 1.0f ) );
				l_light2->GetDirectionalLight()->SetDiffuse( Point4f( 1.0f, 1.0f, 1.0f, 1.0f ) );
				l_light2->GetDirectionalLight()->SetSpecular( Point4f( 1.0f, 1.0f, 1.0f, 1.0f ) );
				l_light2->SetEnabled( true );
			}

			LightSPtr l_light3 = l_scene->CreateLight( cuT( "Light3" ), l_scene->CreateSceneNode( cuT( "Light3Node" ) ), eLIGHT_TYPE_DIRECTIONAL );

			if ( l_light3 )
			{
				l_light3->GetDirectionalLight()->SetDirection( Point3r( -1.0f, -1.0f, -1.0f ) );
				l_light3->GetDirectionalLight()->SetDiffuse( Point4f( 1.0f, 1.0f, 1.0f, 1.0f ) );
				l_light3->GetDirectionalLight()->SetSpecular( Point4f( 1.0f, 1.0f, 1.0f, 1.0f ) );
				l_light3->SetEnabled( true );
			}

			if ( m_timer == NULL )
			{
				m_timer = new wxTimer( this, eID_RENDER_TIMER );
				m_timer->Start( 40 );
			}
		}

		m_auiManager.Update();
	}

	bool MainFrame::DoInitialise3D()
	{
		bool l_bReturn = true;
		Logger::LogInfo( cuT( "Initialising Castor3D" ) );
		m_castor3D = new Engine();
		DoLoadPlugins();

		try
		{
			m_pSplashScreen->Step( _( "Initialising main window" ), 1 );

			if ( m_eRenderer == eRENDERER_TYPE_UNDEFINED )
			{
				RendererSelector m_dialog( m_castor3D, this, wxT( "Castor Shape" ) );
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
			else
			{
				l_bReturn = true;
			}

			if ( l_bReturn )
			{
				l_bReturn = m_castor3D->LoadRenderer( m_eRenderer );
			}

			if ( l_bReturn )
			{
				m_castor3D->Initialise( CASTOR_WANTED_FPS, false );
			}

			if ( l_bReturn )
			{
				SceneSPtr l_scene = m_castor3D->CreateScene( cuT( "MainScene" ) );
				m_mainScene = l_scene;
				l_scene->SetBackgroundColour( Colour::from_components( 0.5, 0.5, 0.5, 1.0 ) );
				Logger::LogInfo( cuT( "Castor3D Initialised" ) );
			}
		}
		catch ( Castor::Exception & p_exc )
		{
			wxMessageBox( str_utils::from_str( p_exc.GetDescription() ), _( "Exception" ), wxOK | wxCENTRE | wxICON_ERROR );
			l_bReturn = false;
		}
		catch ( ... )
		{
			wxMessageBox( _( "Problem occured during Castor3D initialisation" ), _( "Exception" ), wxOK | wxCENTRE | wxICON_ERROR );
			l_bReturn = false;
		}

		return l_bReturn;
	}

	bool MainFrame::DoInitialiseImages()
	{
		m_pSplashScreen->Step( _( "Loading images" ), 1 );
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
		m_pImagesLoader->AddBitmap( eBMP_FRAME_VARIABLE, frame_variable_xpm );
		m_pImagesLoader->AddBitmap( eBMP_FRAME_VARIABLE_SEL, frame_variable_sel_xpm );
		m_pImagesLoader->AddBitmap( eBMP_FRAME_VARIABLE_BUFFER, frame_variable_buffer_xpm );
		m_pImagesLoader->AddBitmap( eBMP_FRAME_VARIABLE_BUFFER_SEL, frame_variable_buffer_sel_xpm );

		m_pImagesLoader->AddBitmap( CV_IMG_CASTOR, castor_transparent_xpm );
		m_pImagesLoader->AddBitmap( eBMP_FICHIER, fichier_xpm );
		m_pImagesLoader->AddBitmap( eBMP_AJOUTER, ajouter_xpm );
		m_pImagesLoader->AddBitmap( eBMP_PARAMETRES, parametres_xpm );
		m_pImagesLoader->AddBitmap( eBMP_SCENES, scene_blanc_xpm );
		m_pImagesLoader->AddBitmap( eBMP_MATERIALS, mat_blanc_xpm );
		m_pImagesLoader->AddBitmap( eBMP_PROPERTIES, parametres_xpm );
		m_pImagesLoader->WaitAsyncLoads();
		return true;
	}

	void MainFrame::DoPopulateStatusBar()
	{
		CreateStatusBar();
	}

	void MainFrame::DoPopulateToolbar()
	{
		m_pSplashScreen->Step( _( "Loading toolbar" ), 1 );
		m_toolBar = new wxAuiToolBar( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_PLAIN_BACKGROUND | wxAUI_TB_HORIZONTAL );
		m_toolBar->SetArtProvider( new AuiToolBarArt );
		m_toolBar->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_toolBar->SetToolBitmapSize( wxSize( 32, 32 ) );
		m_pFileMenu = new wxMenu();
		m_pFileMenu->Append( eSaveScene, _( "Save scene\tCTRL+F+S" ) );
		m_pFileMenu->Append( eLoadScene, _( "Open scene\tCTRL+F+O" ) );
		m_pFileMenu->Append( eRender, _( "Render scene\tCTRL+F+R" ) );
		m_pFileMenu->AppendSeparator();
		m_pFileMenu->Append( eExit, _( "Quit\tALT+F4" ) );
		m_pNewMenu = new wxMenu();
		wxMenu * l_pSubMenu = new wxMenu();
		l_pSubMenu->Append( eNewCone, _( "Cone" ) );
		l_pSubMenu->Append( eNewCube, _( "Cube" ) );
		l_pSubMenu->Append( eNewCylinder, _( "Cylinder" ) );
		l_pSubMenu->Append( eNewIcosahedron, _( "Icosahedron" ) );
		l_pSubMenu->Append( eNewPlane, _( "Plane" ) );
		l_pSubMenu->Append( eNewSphere, _( "Sphere" ) );
		l_pSubMenu->Append( eNewTorus, _( "Torus" ) );
		l_pSubMenu->Append( eNewProjection, _( "Projection" ) );
		m_pNewMenu->AppendSubMenu( l_pSubMenu, _( "New Geometry\tCTRL+N+G" ) );
		m_pNewMenu->Append( eNewMaterial, _( "Material\tCTRL+N+M" ) );
		m_pSettingsMenu = new wxMenu();
		m_pSettingsMenu->AppendCheckItem( eSelect, _( "Select\tCTRL+E+S" ) );
		m_pSettingsMenu->AppendCheckItem( eModify, _( "Modify\tCTRL+E+M" ) );
		m_pSettingsMenu->Append( eNone, _( "Cancel\tCTRL+E+C" ) );
		m_pSettingsMenu->AppendSeparator();
		m_pSettingsMenu->AppendRadioItem( eSelectGeometries, _( "Geometry\tCTRL+E+G" ) )->Enable( false );
		m_pSettingsMenu->AppendRadioItem( eSelectPoints, _( "Point\tCTRL+E+P" ) )->Enable( false );
		m_pSettingsMenu->AppendSeparator();
		m_pSettingsMenu->Append( eCloneSelection, _( "Clone\tCTRL+E+D" ) );
		l_pSubMenu = new wxMenu();
		l_pSubMenu->Append( eSubdividePNTriangles, _( "PN Triangles\tCTRL+E+S+T" ) );
		l_pSubMenu->Append( eSubdivideLoop, _( "Loop\tCTRL+E+S+L" ) );
		m_pSettingsMenu->AppendSubMenu( l_pSubMenu, _( "Subdivide" ) );
		m_pSettingsMenu->Append( eSelectNone, _( "No action\tCTRL+E+A" ) );
		m_toolBar->AddTool( eFile, _( "File" ), wxImage( *ImagesLoader::GetBitmap( eBMP_FICHIER ) ).Rescale( 32, 32, wxIMAGE_QUALITY_HIGH ), _( "File menu" ) );
		m_pSplashScreen->Step( 1 );
		m_toolBar->AddTool( eNew, _( "New" ), wxImage( *ImagesLoader::GetBitmap( eBMP_AJOUTER ) ).Rescale( 32, 32, wxIMAGE_QUALITY_HIGH ), _( "New element" ) );
		m_pSplashScreen->Step( 1 );
		m_toolBar->AddTool( eSettings, _( "Parameters" ), wxImage( *ImagesLoader::GetBitmap( eBMP_PARAMETRES ) ).Rescale( 32, 32, wxIMAGE_QUALITY_HIGH ), _( "Parameters menu" ) );
		m_pSplashScreen->Step( 1 );
		m_toolBar->AddSeparator();
		m_toolBar->AddTool( eProperties, _( "Properties" ), wxImage( *ImagesLoader::GetBitmap( eBMP_PROPERTIES ) ).Rescale( 32, 32, wxIMAGE_QUALITY_HIGH ), _( "Display objects properties" ) );
		m_pSplashScreen->Step( 1 );
		m_toolBar->AddTool( eLists, _( "Lists" ), wxImage( *ImagesLoader::GetBitmap( eBMP_MATERIALS ) ).Rescale( 32, 32, wxIMAGE_QUALITY_HIGH ), _( "Display objects lists" ) );
		m_pSplashScreen->Step( 1 );
		m_toolBar->AddSeparator();
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

	void MainFrame::DoCreateGeometry( eMESH_TYPE p_meshType, String p_name, OutputStream const & p_meshStrVars, wxString const & p_baseName, SceneSPtr p_scene, NewGeometryDialog * p_dialog, uint32_t i, uint32_t j, real a, real b, real c )
	{
		if ( p_name.empty() || p_name == _( "Geometry Name" ) )
		{
			p_name = p_baseName;
			p_name << m_iNbGeometries;
		}

		SceneNodeSPtr l_sceneNode = p_scene->CreateSceneNode( cuT( "SN_" ) + p_name, p_scene->GetObjectRootNode() );

		if ( l_sceneNode )
		{
			UIntArray l_faces;
			RealArray l_dimensions;
			l_faces.push_back( i );
			l_faces.push_back( j );
			l_dimensions.push_back( a );
			l_dimensions.push_back( b );
			l_dimensions.push_back( c );
			StringStream l_stream;
			l_stream << GuiCommon::make_String( p_baseName ) << cuT( "_" ) << p_meshStrVars.rdbuf();
			GeometrySPtr l_geometry = p_scene->CreateGeometry( p_name, p_meshType, l_stream.str(), l_faces, l_dimensions );

			if ( l_geometry )
			{
				l_sceneNode->AttachObject( l_geometry );
				String l_materialName = p_dialog->GetMaterialName();
				MeshSPtr l_mesh = l_geometry->GetMesh();

				for ( auto && l_submesh : *l_mesh )
				{
					l_geometry->SetMaterial( l_submesh, m_castor3D->GetMaterialManager().find( l_materialName ) );
				}

				l_sceneNode->SetVisible( true );
				m_iNbGeometries++;
			}
		}
	}

	void MainFrame::DoSetSelectionType( SelectionType p_eType )
	{
		m_3dFrame->SetSelectionType( p_eType );

		if ( m_bMultiFrames )
		{
			m_2dFrameHD->SetSelectionType( p_eType );
			m_2dFrameBG->SetSelectionType( p_eType );
			m_2dFrameBD->SetSelectionType( p_eType );
		}
	}
	void MainFrame::DoSetActionType( ActionType p_eType )
	{
		m_3dFrame->SetActionType( p_eType );

		if ( m_bMultiFrames )
		{
			m_2dFrameHD->SetActionType( p_eType );
			m_2dFrameBG->SetActionType( p_eType );
			m_2dFrameBD->SetActionType( p_eType );
		}
	}

	BEGIN_EVENT_TABLE( MainFrame, wxFrame )
		EVT_SIZE( MainFrame::OnSize )
		EVT_MOVE( MainFrame::OnMove )
		EVT_CLOSE( MainFrame::OnClose )
		EVT_ENTER_WINDOW( MainFrame::OnEnterWindow )
		EVT_LEAVE_WINDOW( MainFrame::OnLeaveWindow )
		EVT_ERASE_BACKGROUND( MainFrame::OnEraseBackground )
		EVT_KEY_DOWN( MainFrame::OnKeyDown )
		EVT_KEY_UP( MainFrame::OnKeyUp )
		EVT_LEFT_DOWN( MainFrame::OnMouseLDown )
		EVT_LEFT_UP( MainFrame::OnMouseLUp )
		EVT_MIDDLE_DOWN( MainFrame::OnMouseMDown )
		EVT_MIDDLE_UP( MainFrame::OnMouseMUp )
		EVT_RIGHT_DOWN( MainFrame::OnMouseRDown )
		EVT_RIGHT_UP( MainFrame::OnMouseRUp )
		EVT_MOTION( MainFrame::OnMouseMove )
		EVT_MOUSEWHEEL( MainFrame::OnMouseWheel )

		EVT_MENU( wxID_EXIT, MainFrame::OnMenuClose )
		EVT_MENU( eExit, MainFrame::OnMenuClose )
		EVT_MENU( eSaveScene, MainFrame::OnSaveScene )
		EVT_MENU( eLoadScene, MainFrame::OnLoadScene )
		EVT_MENU( eNewCone, MainFrame::OnNewCone )
		EVT_MENU( eNewCube, MainFrame::OnNewCube )
		EVT_MENU( eNewCylinder, MainFrame::OnNewCylinder )
		EVT_MENU( eNewIcosahedron, MainFrame::OnNewIcosahedron )
		EVT_MENU( eNewPlane, MainFrame::OnNewPlane )
		EVT_MENU( eNewSphere, MainFrame::OnNewSphere )
		EVT_MENU( eNewTorus, MainFrame::OnNewTorus )
		EVT_MENU( eNewProjection, MainFrame::OnNewProjection )
		EVT_MENU( eNewMaterial, MainFrame::OnNewMaterial )
		EVT_MENU( eProperties, MainFrame::OnShowProperties )
		EVT_MENU( eLists, MainFrame::OnShowLists )
		EVT_MENU( eSelectGeometries, MainFrame::OnSelectGeometries )
		EVT_MENU( eSelectPoints, MainFrame::OnSelectPoints )
		EVT_MENU( eCloneSelection, MainFrame::OnCloneSelection )
		EVT_MENU( eSelectNone, MainFrame::OnSelectNone )
		EVT_MENU( eSubdividePNTriangles, MainFrame::OnSubdivideAllPNTriangles )
		EVT_MENU( eSubdivideLoop, MainFrame::OnSubdivideAllLoop )
		EVT_MENU( eSelect, MainFrame::OnSelect )
		EVT_MENU( eModify, MainFrame::OnModify )
		EVT_MENU( eNone, MainFrame::OnNothing )
		EVT_MENU( eRender, MainFrame::OnRender )
		EVT_TOOL( eFile, MainFrame::OnFileMenu )
		EVT_TOOL( eNew, MainFrame::OnNewMenu )
		EVT_TOOL( eSettings, MainFrame::OnSettingsMenu )
		EVT_TIMER( eID_RENDER_TIMER, MainFrame::OnTimer )
	END_EVENT_TABLE()

	void MainFrame::OnPaint( wxPaintEvent & WXUNUSED( p_event ) )
	{
		wxPaintDC l_dc( this );
		m_castor3D->RenderOneFrame();
	}

	void MainFrame::OnTimer( wxTimerEvent & WXUNUSED( p_event ) )
	{
		m_castor3D->RenderOneFrame();
	}

	void MainFrame::OnSize( wxSizeEvent & WXUNUSED( p_event ) )
	{
		wxClientDC l_dc( this );
		int l_width = GetClientSize().x / 2;
		int l_height = GetClientSize().y / 2;

		if ( m_3dFrame )
		{
			if ( ! m_bMultiFrames )
			{
				m_3dFrame->SetSize( GetClientSize() );
				m_3dFrame->SetPosition( wxPoint( 0, 0 ) );
			}
			else
			{
				m_3dFrame->SetSize( l_width - 1, l_height - 1 );
				m_3dFrame->SetPosition( wxPoint( 0, 0 ) );
				m_2dFrameHD->SetSize( l_width - 1, l_height - 1 );
				m_2dFrameHD->SetPosition( wxPoint( l_width + 1, 0 ) );
				m_2dFrameBG->SetSize( l_width - 1, l_height - 1 );
				m_2dFrameBG->SetPosition( wxPoint( 0, l_height + 1 ) );
				m_2dFrameBD->SetSize( l_width - 1, l_height - 1 );
				m_2dFrameBD->SetPosition( wxPoint( l_width + 1, l_height + 1 ) );
				m_separator1->SetSize( l_width, 2 );
				m_separator1->SetPosition( wxPoint( 0, l_height - 1 ) );
				m_separator2->SetSize( l_width, 2 );
				m_separator2->SetPosition( wxPoint( l_width, l_height - 1 ) );
				m_separator3->SetSize( 2, l_height );
				m_separator3->SetPosition( wxPoint( l_width - 1, 0 ) );
				m_separator4->SetSize( 2, l_height );
				m_separator4->SetPosition( wxPoint( l_width - 1, l_height ) );
			}
		}

		ShowPanels();
	}

	void MainFrame::OnMove( wxMoveEvent & WXUNUSED( p_event ) )
	{
		wxClientDC l_dc( this );
		ShowPanels();
	}

	void MainFrame::OnClose( wxCloseEvent & p_event )
	{
		m_auiManager.DetachPane( m_sceneTabsContainer );
		m_auiManager.DetachPane( m_propertiesContainer );
		m_auiManager.DetachPane( m_renderPanelsContainer );
		m_auiManager.DetachPane( m_toolBar );
		Hide();

		if ( m_timer )
		{
			m_timer->Stop();
			delete m_timer;
			m_timer = nullptr;
		}

		delete m_pImagesLoader;
		m_selectedGeometry.reset();
		m_repereX.reset();
		m_repereY.reset();
		m_repereZ.reset();
		clear_container( m_selectedGeometryMaterials );
		m_mainScene.reset();

		if( m_3dFrame )
		{
			m_3dFrame->DestroyRenderWindow();

			if (m_bMultiFrames)
			{
				m_2dFrameHD->DestroyRenderWindow();
				m_2dFrameBG->DestroyRenderWindow();
				m_2dFrameBD->DestroyRenderWindow();
			}
		}

		m_castor3D->Cleanup();

		if ( m_3dFrame )
		{
			m_3dFrame->UnFocus();
			m_3dFrame->Close( true );
			m_3dFrame = nullptr;

			if ( m_bMultiFrames )
			{
				m_2dFrameHD->UnFocus();
				m_2dFrameHD->Close( true );
				m_2dFrameHD = nullptr;
				m_2dFrameBG->UnFocus();
				m_2dFrameBG->Close( true );
				m_2dFrameBG = nullptr;
				m_2dFrameBD->UnFocus();
				m_2dFrameBD->Close( true );
				m_2dFrameBD = nullptr;
			}
		}

		DestroyChildren();
		m_pImagesLoader->Cleanup();
		p_event.Skip();
	}

	void MainFrame::OnEnterWindow( wxMouseEvent & WXUNUSED( p_event ) )
	{
		SetFocus();
	}

	void MainFrame::OnLeaveWindow( wxMouseEvent & WXUNUSED( p_event ) )
	{
	}

	void MainFrame::OnEraseBackground( wxEraseEvent & WXUNUSED( p_event ) )
	{
	}

	void MainFrame::OnKeyDown( wxKeyEvent & event )
	{
		if ( m_selectedFrame )
		{
			m_selectedFrame->OnKeyDown( event );
		}
	}

	void MainFrame::OnKeyUp( wxKeyEvent & event )
	{
		if ( m_selectedFrame )
		{
			m_selectedFrame->OnKeyUp( event );
		}
	}

	void MainFrame::OnMouseLDown( wxMouseEvent & event )
	{
		if ( m_selectedFrame )
		{
			m_selectedFrame->OnMouseLDown( event );
		}
	}

	void MainFrame::OnMouseLUp( wxMouseEvent & event )
	{
		if ( m_selectedFrame )
		{
			m_selectedFrame->OnMouseLUp( event );
		}
	}

	void MainFrame::OnMouseMDown( wxMouseEvent & event )
	{
		if ( m_selectedFrame )
		{
			m_selectedFrame->OnMouseMDown( event );
		}
	}

	void MainFrame::OnMouseMUp( wxMouseEvent & event )
	{
		if ( m_selectedFrame )
		{
			m_selectedFrame->OnMouseMUp( event );
		}
	}

	void MainFrame::OnMouseRDown( wxMouseEvent & event )
	{
		if ( m_selectedFrame )
		{
			m_selectedFrame->OnMouseRDown( event );
		}
	}

	void MainFrame::OnMouseRUp( wxMouseEvent & event )
	{
		if ( m_selectedFrame )
		{
			m_selectedFrame->OnMouseRUp( event );
		}
	}

	void MainFrame::OnMouseMove( wxMouseEvent & event )
	{
		if ( m_selectedFrame )
		{
			m_selectedFrame->OnMouseMove( event );
		}
	}

	void MainFrame::OnMouseWheel( wxMouseEvent & event )
	{
		if ( m_selectedFrame )
		{
			m_selectedFrame->OnMouseWheel( event );
		}
	}

	void MainFrame::OnMenuClose( wxCommandEvent & WXUNUSED( p_event ) )
	{
		Close( true );
	}

	void MainFrame::OnSaveScene( wxCommandEvent & WXUNUSED( p_event ) )
	{
		wxFileDialog * l_fileDialog = new wxFileDialog( this, _( "Save scene" ), wxEmptyString, wxEmptyString, _( "Castor Shape files (*.cscn)|*.cscn" ) );

		if ( l_fileDialog->ShowModal() == wxID_OK )
		{
			BinaryFile l_file( ( char const * )l_fileDialog->GetPath().char_str(), File::eOPEN_MODE_WRITE );
			Path l_filePath = ( char const * )l_fileDialog->GetPath().c_str();
			Collection<Scene, String> l_scnManager;

			if ( m_castor3D->GetMaterialManager().Save( l_file ) )
			{
				Logger::LogInfo( cuT( "Materials written" ) );
			}
			else
			{
				Logger::LogInfo( cuT( "Can't write materials" ) );
				return;
			}

			if ( m_castor3D->SaveMeshes( l_file ) )
			{
				Logger::LogInfo( cuT( "Meshes written" ) );
			}
			else
			{
				Logger::LogInfo( cuT( "Can't write meshes" ) );
				return;
			}

			//if (Scene::BinaryLoader()( *l_scnManager.find( cuT( "MainScene")), l_file ) )
			//{
			//	Logger::LogInfo( cuT( "Save Successfull"));
			//}
			//else
			//{
			//	Logger::LogInfo( cuT( "Save Failed"));
			//}
		}
	}

	void MainFrame::OnLoadScene( wxCommandEvent & WXUNUSED( p_event ) )
	{
		wxString l_wildcard = _( "Castor3D scene files (*.cscn)|*.cscn" );

		for ( PluginStrMap::iterator l_it = m_castor3D->PluginsBegin( ePLUGIN_TYPE_IMPORTER ); l_it != m_castor3D->PluginsEnd( ePLUGIN_TYPE_IMPORTER ); ++l_it )
		{
			ImporterPluginSPtr l_pPlugin = std::static_pointer_cast< ImporterPlugin >( l_it->second );
			ImporterPlugin::ExtensionArray l_arrayExt = l_pPlugin->GetExtensions();

			for ( ImporterPlugin::ExtensionArrayIt l_itExt = l_arrayExt.begin(); l_itExt != l_arrayExt.end(); ++l_itExt )
			{
				std::string l_strName = str_utils::to_str( l_itExt->second );
				std::string l_strExt = str_utils::to_str( str_utils::lower_case( l_itExt->first ) );
				l_wildcard.Printf( cuT( "%s|%s (*.%s)|*.%s" ), ( char const * )l_wildcard.char_str(), l_strName.c_str(), l_strExt.c_str(), l_strExt.c_str() );
			}
		}

		wxFileDialog l_fileDialog( this, _( "Open a scene" ), wxEmptyString, wxEmptyString, l_wildcard );

		if ( l_fileDialog.ShowModal() == wxID_OK )
		{
			m_strFilePath = ( wxChar const * )l_fileDialog.GetPath().c_str();
			LoadScene( m_strFilePath );
		}

		ShowPanels();
	}

	void MainFrame::OnNewCone( wxCommandEvent & WXUNUSED( p_event ) )
	{
		NewConeDialog l_dialog( m_castor3D, this, wxID_ANY );

		if ( l_dialog.ShowModal() == wxID_OK )
		{
			SceneSPtr l_scene = m_mainScene.lock();

			if ( l_scene )
			{
				real l_radius = l_dialog.GetRadius();
				real l_height = l_dialog.GetHeight();
				int l_nbFaces = l_dialog.GetFacesCount();

				if ( l_radius != 0.0 && l_height != 0.0  && l_nbFaces >= 1 )
				{
					DoCreateGeometry( eMESH_TYPE_CONE, l_dialog.GetGeometryName(), StringStream() << l_nbFaces, _( "Cone" ), l_scene, &l_dialog, l_nbFaces, 0, l_height, l_radius, 0 );
				}
			}

			ShowPanels();
		}
	}

	void MainFrame::OnNewCube( wxCommandEvent & WXUNUSED( p_event ) )
	{
		NewCubeDialog l_dialog( m_castor3D, this, wxID_ANY );

		if ( l_dialog.ShowModal() == wxID_OK )
		{
			SceneSPtr l_scene = m_mainScene.lock();

			if ( l_scene )
			{
				real l_width = l_dialog.GetWidth();
				real l_height = l_dialog.GetHeight();
				real l_depth = l_dialog.GetDepth();

				if ( l_width != 0.0 && l_height != 0.0 && l_depth != 0.0 )
				{
					DoCreateGeometry( eMESH_TYPE_CUBE, l_dialog.GetGeometryName(), StringStream(), _( "Cube" ), l_scene, &l_dialog, 0, 0, l_width, l_height, l_depth );
				}
			}

			ShowPanels();
		}
	}

	void MainFrame::OnNewCylinder( wxCommandEvent & WXUNUSED( p_event ) )
	{
		NewCylinderDialog l_dialog( m_castor3D, this, wxID_ANY );

		if ( l_dialog.ShowModal() == wxID_OK )
		{
			SceneSPtr l_scene = m_mainScene.lock();

			if ( l_scene )
			{
				real l_radius = l_dialog.GetRadius();
				real l_height = l_dialog.GetHeight();
				int l_nbFaces = l_dialog.GetFacesCount();

				if ( l_radius != 0.0 && l_height != 0.0 && l_nbFaces >= 1 )
				{
					DoCreateGeometry( eMESH_TYPE_CYLINDER, l_dialog.GetGeometryName(), StringStream() << l_nbFaces, _( "Cylinder" ), l_scene, &l_dialog, l_nbFaces, 0, l_height, l_radius );
				}
			}

			ShowPanels();
		}
	}

	void MainFrame::OnNewIcosahedron( wxCommandEvent & WXUNUSED( p_event ) )
	{
		NewIcosahedronDialog l_dialog( m_castor3D, this, wxID_ANY );

		if ( l_dialog.ShowModal() == wxID_OK )
		{
			SceneSPtr l_scene = m_mainScene.lock();

			if ( l_scene )
			{
				real l_radius = l_dialog.GetRadius();
				int l_nbFaces = l_dialog.GetSubdivisionCount();

				if ( l_radius != 0.0 && l_nbFaces >= 1 )
				{
					DoCreateGeometry( eMESH_TYPE_ICOSAHEDRON, l_dialog.GetGeometryName(), StringStream() << l_nbFaces, _( "Icosahedron" ), l_scene, &l_dialog, l_nbFaces, 0, l_radius );
				}
			}

			ShowPanels();
		}
	}

	void MainFrame::OnNewPlane( wxCommandEvent & WXUNUSED( p_event ) )
	{
		NewPlaneDialog l_dialog( m_castor3D, this, wxID_ANY );

		if ( l_dialog.ShowModal() == wxID_OK )
		{
			SceneSPtr l_scene = m_mainScene.lock();

			if ( l_scene )
			{
				real l_width = l_dialog.GetWidth();
				real l_depth = l_dialog.GetDepth();
				int l_nbWidthSubdiv = l_dialog.GetWidthSubdivisionCount();
				int l_nbDepthSubdiv = l_dialog.GetDepthSubdivisionCount();

				if ( l_width != 0.0 && l_depth != 0.0 && l_nbWidthSubdiv >= 0 && l_nbDepthSubdiv >= 0 )
				{
					DoCreateGeometry( eMESH_TYPE_PLANE, l_dialog.GetGeometryName(), StringStream() << l_nbWidthSubdiv << cuT( "x" ) << l_nbDepthSubdiv, _( "Plane" ), l_scene, &l_dialog, l_nbDepthSubdiv, l_nbWidthSubdiv, l_width, l_depth );
				}
			}

			ShowPanels();
		}
	}

	void MainFrame::OnNewSphere( wxCommandEvent & WXUNUSED( p_event ) )
	{
		NewSphereDialog l_dialog( m_castor3D, this, wxID_ANY );

		if ( l_dialog.ShowModal() == wxID_OK )
		{
			SceneSPtr l_scene = m_mainScene.lock();

			if ( l_scene )
			{
				real l_radius = l_dialog.GetRadius();
				int l_nbFaces = l_dialog.GetFacesCount();

				if ( l_radius != 0.0 && l_nbFaces >= 3 )
				{
					DoCreateGeometry( eMESH_TYPE_SPHERE, l_dialog.GetGeometryName(), StringStream() << l_nbFaces, _( "Sphere" ), l_scene, &l_dialog, l_nbFaces, 0, l_radius );
				}
			}

			ShowPanels();
		}
	}

	void MainFrame::OnNewTorus( wxCommandEvent & WXUNUSED( p_event ) )
	{
		NewTorusDialog l_dialog( m_castor3D, this, wxID_ANY );

		if ( l_dialog.ShowModal() == wxID_OK )
		{
			SceneSPtr l_scene = m_mainScene.lock();

			if ( l_scene )
			{
				real l_width = l_dialog.GetInternalRadius();
				real l_radius = l_dialog.GetExternalRadius();
				int l_nbRadiusSubdiv = l_dialog.GetExternalFacesCount();
				int l_nbWidthSubdiv = l_dialog.GetInternalFacesCount();

				if ( l_width != 0.0 && l_radius != 0.0 && l_nbRadiusSubdiv >= 1 && l_nbWidthSubdiv >= 1 )
				{
					DoCreateGeometry( eMESH_TYPE_TORUS, l_dialog.GetGeometryName(), StringStream() << l_nbRadiusSubdiv << cuT( "x" ) << l_nbWidthSubdiv, _( "Torus" ), l_scene, &l_dialog, l_nbWidthSubdiv, l_nbRadiusSubdiv, l_width, l_radius );
				}
			}

			ShowPanels();
		}
	}

	void MainFrame::OnNewProjection( wxCommandEvent & WXUNUSED( p_event ) )
	{
		NewSphereDialog l_dialog( m_castor3D, this, wxID_ANY );

		if ( l_dialog.ShowModal() == wxID_OK )
		{
			SceneSPtr l_scene = m_mainScene.lock();

			if ( l_scene )
			{
				real l_fDepth = l_dialog.GetRadius();
				int l_nbFaces = l_dialog.GetFacesCount();

				if ( l_fDepth != 0.0 && l_nbFaces >= 1 )
				{
					DoCreateGeometry( eMESH_TYPE_PROJECTION, l_dialog.GetGeometryName(), StringStream() << l_nbFaces, _( "Projection" ), l_scene, &l_dialog, l_nbFaces, 0, l_fDepth, 0.0 );
				}
			}

			ShowPanels();
		}
	}

	void MainFrame::OnNewMaterial( wxCommandEvent & WXUNUSED( p_event ) )
	{
		NewMaterialDialog l_dialog( m_castor3D, this, wxID_ANY );

		if ( l_dialog.ShowModal() == wxID_OK )
		{
			Logger::LogInfo( cuT( "Material Created" ) );
		}
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

	void MainFrame::OnSelectGeometries( wxCommandEvent & WXUNUSED( p_event ) )
	{
		SelectGeometry( GeometrySPtr() );
		DoSetSelectionType( stGeometry );
	}

	void MainFrame::OnSelectPoints( wxCommandEvent & WXUNUSED( p_event ) )
	{
		SelectGeometry( GeometrySPtr() );
		DoSetSelectionType( stVertex );
	}

	void MainFrame::OnCloneSelection( wxCommandEvent & WXUNUSED( p_event ) )
	{
		DoSetSelectionType( stClone );
	}

	void MainFrame::OnSelectNone( wxCommandEvent & WXUNUSED( p_event ) )
	{
		SelectGeometry( GeometrySPtr() );
		DoSetSelectionType( stNone );
	}

	void MainFrame::OnSelectAll( wxCommandEvent & WXUNUSED( p_event ) )
	{
		DoSetSelectionType( stAll );
	}

	void MainFrame::OnSelect( wxCommandEvent & p_event )
	{
		m_pSettingsMenu->FindItem( eModify )->Check( false );

		if ( p_event.IsChecked() )
		{
			m_pSettingsMenu->FindItem( eSelectGeometries )->Enable( true );
			m_pSettingsMenu->FindItem( eSelectPoints )->Enable( true );
			DoSetActionType( atSelect );
			SelectGeometry( GeometrySPtr() );

			if ( m_pSettingsMenu->FindItem( eSelectGeometries )->IsChecked() )
			{
				DoSetSelectionType( stGeometry );
			}
			else
			{
				DoSetSelectionType( stVertex );
			}
		}
		else
		{
			DoSetActionType( atNone );
		}
	}

	void MainFrame::OnModify( wxCommandEvent & p_event )
	{
		m_pSettingsMenu->FindItem( eSelect )->Check( false );
		m_pSettingsMenu->FindItem( eSelectGeometries )->Enable( false );
		m_pSettingsMenu->FindItem( eSelectPoints )->Enable( false );

		if ( p_event.IsChecked() )
		{
			DoSetActionType( atModify );
		}
		else
		{
			DoSetActionType( atNone );
		}
	}

	void MainFrame::OnNothing( wxCommandEvent & WXUNUSED( p_event ) )
	{
		m_pSettingsMenu->FindItem( eSelect )->Check( false );
		m_pSettingsMenu->FindItem( eModify )->Check( false );
		m_pSettingsMenu->FindItem( eSelectGeometries )->Enable( false );
		m_pSettingsMenu->FindItem( eSelectPoints )->Enable( false );
		SelectGeometry( GeometrySPtr() );
		DoSetActionType( atNone );
	}

	void MainFrame::OnSubdivideAllPNTriangles( wxCommandEvent & WXUNUSED( p_event ) )
	{
		if ( m_selectedGeometry )
		{
			Subdivider 	*	l_pDivider = NULL;
			DividerPluginSPtr	l_pPlugin;

			for ( PluginStrMap::iterator l_it = m_castor3D->PluginsBegin( ePLUGIN_TYPE_DIVIDER ); l_it != m_castor3D->PluginsEnd( ePLUGIN_TYPE_DIVIDER ) && !l_pDivider; ++l_it )
			{
				l_pPlugin = std::static_pointer_cast< DividerPlugin, PluginBase >( l_it->second );

				if ( str_utils::lower_case( l_pPlugin->GetDividerType() ) == cuT( "pn_tri" ) )
				{
					l_pDivider = l_pPlugin->CreateDivider();
				}
			}

			if ( l_pDivider )
			{
				for ( auto l_submesh : *m_selectedGeometry->GetMesh() )
				{
					l_pDivider->Subdivide( l_submesh, 1, true );
				}

				l_pPlugin->DestroyDivider( l_pDivider );
			}
		}
	}

	void MainFrame::OnSubdivideAllLoop( wxCommandEvent & WXUNUSED( p_event ) )
	{
		if ( m_selectedGeometry )
		{
			Subdivider 	*	l_pDivider = NULL;
			DividerPluginSPtr	l_pPlugin;

			for ( PluginStrMap::iterator l_it = m_castor3D->PluginsBegin( ePLUGIN_TYPE_DIVIDER ); l_it != m_castor3D->PluginsEnd( ePLUGIN_TYPE_DIVIDER ) && !l_pDivider; ++l_it )
			{
				l_pPlugin = std::static_pointer_cast< DividerPlugin, PluginBase >( l_it->second );

				if ( str_utils::lower_case( l_pPlugin->GetDividerType() ) == cuT( "pn_tri" ) )
				{
					l_pDivider = l_pPlugin->CreateDivider();
				}
			}

			if ( l_pDivider )
			{
				for ( auto l_submesh : *m_selectedGeometry->GetMesh() )
				{
					l_pDivider->Subdivide( l_submesh, 1, true );
				}

				l_pPlugin->DestroyDivider( l_pDivider );
			}
		}
	}

	void MainFrame::OnRender( wxCommandEvent & WXUNUSED( p_event ) )
	{
		RenderEngine renderEngine( cuT( "Scene.tga" ), m_mainScene.lock() );
		renderEngine.Draw();
	}

	void MainFrame::OnFileMenu( wxCommandEvent & p_event )
	{
		wxAuiToolBarItem * l_pTool = m_toolBar->FindTool( eFile );

		if ( l_pTool )
		{
			wxPoint l_ptMouse = wxGetMousePosition();
			l_ptMouse = ScreenToClient( l_ptMouse );
			wxRect l_rcBar = m_toolBar->GetRect();
			PopupMenu( m_pFileMenu, l_ptMouse.x, l_ptMouse.y );
		}
		else
		{
			PopupMenu( m_pFileMenu );
		}

		p_event.Skip();
	}

	void MainFrame::OnNewMenu( wxCommandEvent & p_event )
	{
		wxAuiToolBarItem * l_pTool = m_toolBar->FindTool( eNew );

		if ( l_pTool )
		{
			wxPoint l_ptMouse = wxGetMousePosition();
			l_ptMouse = ScreenToClient( l_ptMouse );
			wxRect l_rcBar = m_toolBar->GetRect();
			PopupMenu( m_pNewMenu, l_ptMouse.x, l_ptMouse.y );
		}
		else
		{
			PopupMenu( m_pNewMenu );
		}

		p_event.Skip();
	}

	void MainFrame::OnSettingsMenu( wxCommandEvent & p_event )
	{
		wxAuiToolBarItem * l_pTool = m_toolBar->FindTool( eSettings );

		if ( l_pTool )
		{
			wxPoint l_ptMouse = wxGetMousePosition();
			l_ptMouse = ScreenToClient( l_ptMouse );
			wxRect l_rcBar = m_toolBar->GetRect();
			PopupMenu( m_pSettingsMenu, l_ptMouse.x, l_ptMouse.y );
		}
		else
		{
			PopupMenu( m_pSettingsMenu );
		}

		p_event.Skip();
	}
}
