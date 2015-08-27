#if CHECK_MEMORYLEAKS && defined( VLD_AVAILABLE ) && USE_VLD
#	include <vld.h>
#endif

#include "MainFrame.hpp"

#include <Scene.hpp>
#include <PlatformWindowHandle.hpp>
#include <Parameter.hpp>
#include <TechniqueFactory.hpp>

using namespace Castor;
using namespace Castor3D;
using namespace OceanLighting;

#if defined( __linux )
#	include <GL/glx.h>
#	include <gtk/gtk.h>
#	include <gdk/gdkx.h>
#elif defined( _WIN32 )
#	include <Windows.h>
#endif

namespace OceanLighting
{
	struct PluginLoader
	{
		StringArray * m_pArrayFailed;
		std::mutex * m_pMutex;

		PluginLoader( StringArray * p_pArrayFailed = NULL, std::mutex * p_pMutex = NULL )
			:	m_pArrayFailed( p_pArrayFailed	)
			,	m_pMutex( p_pMutex	)
		{
		}
		void operator()( Engine * p_pEngine, Path const & p_pathFile )
		{
			PluginBaseSPtr l_pPlugin = p_pEngine->LoadPlugin( p_pathFile );

			if ( !l_pPlugin )
			{
				if ( m_pMutex && m_pArrayFailed )
				{
					m_pMutex->lock();
					m_pArrayFailed->push_back( p_pathFile );
					m_pMutex->unlock();
				}
			}
		}
	};

	template< class wxtElem >
	void AddElemToSizer( wxStaticText * p_pStatic, wxtElem * p_pElement, wxSizer * p_pSizer )
	{
		wxBoxSizer * l_pSizerLine = new wxBoxSizer( wxHORIZONTAL );
		l_pSizerLine->Add( p_pStatic,	wxSizerFlags( 0 ).Align( wxALIGN_CENTER_VERTICAL ).Border( wxRIGHT, 5 ) );
		l_pSizerLine->Add( p_pElement,	wxSizerFlags( 1 ).Align( wxALIGN_CENTER_VERTICAL ) );
		p_pSizer->Add( l_pSizerLine, wxSizerFlags( 0 ).Expand().Border( wxALL, 5 ) );
	}

	template< class wxtElem >
	void AddElemToSizer( wxtElem * p_pElement, wxSizer * p_pSizer )
	{
		wxBoxSizer * l_pSizerLine = new wxBoxSizer( wxHORIZONTAL );
		l_pSizerLine->Add( p_pElement,	wxSizerFlags( 1 ).Align( wxALIGN_CENTER_VERTICAL ) );
		p_pSizer->Add( l_pSizerLine, wxSizerFlags( 0 ).Expand().Border( wxALL, 5 ) );
	}

	void AddSpinCtrl( wxWindow * p_pParent, wxString const & p_strCaption, wxWindowID p_id, int p_iMin, int p_iMax, int p_iValue, void * p_pClientData, wxSizer * p_pSizer )
	{
		wxStaticText * l_pStaticText = new wxStaticText( p_pParent, wxID_ANY, p_strCaption );
		wxSpinCtrl * l_pSpinCtrl = new wxSpinCtrl( p_pParent, p_id );
		l_pSpinCtrl->SetRange( p_iMin, p_iMax );
		l_pSpinCtrl->SetValue( p_iValue );
		l_pSpinCtrl->SetClientData( &p_pClientData );
		AddElemToSizer( l_pStaticText, l_pSpinCtrl, p_pSizer );
	}

	void AddTextCtrl( wxWindow * p_pParent, wxString const & p_strCaption, wxWindowID p_id, float p_fValue, void * p_pClientData, wxSizer * p_pSizer, wxValidator const & p_validator = wxDefaultValidator )
	{
		StringStream l_strValue;
		l_strValue << p_fValue;
		wxStaticText * l_pStaticText = new wxStaticText( p_pParent, wxID_ANY, p_strCaption );
		wxTextCtrl * l_pTextCtrl = new wxTextCtrl( p_pParent, p_id, l_strValue.str() );
		l_pTextCtrl->SetValidator( p_validator );
		l_pTextCtrl->SetClientData( &p_pClientData );
		AddElemToSizer( l_pStaticText, l_pTextCtrl, p_pSizer );
	}

	void AddColourPickerCtrl( wxWindow * p_pParent, wxString const & p_strCaption, wxWindowID p_id, Point4f const & p_ptColour, void * p_pClientData, wxSizer * p_pSizer )
	{
		wxStaticText * l_pStaticText = new wxStaticText( p_pParent, wxID_ANY, p_strCaption );
		wxColourPickerCtrl * l_pColourPickerCtrl = new wxColourPickerCtrl( p_pParent, p_id, wxColour( uint8_t( p_ptColour[0] * 255 ), uint8_t( p_ptColour[1] * 255 ), uint8_t( p_ptColour[2] * 255 ), uint8_t( p_ptColour[3] * 255 ) ) );
		l_pColourPickerCtrl->SetClientData( p_pClientData );
		AddElemToSizer( l_pStaticText, l_pColourPickerCtrl, p_pSizer );
	}

	void AddButton( wxWindow * p_pParent, wxString const & p_strCaption, wxWindowID p_id, wxSizer * p_pSizer )
	{
		wxButton * l_pButton = new wxButton( p_pParent, p_id, p_strCaption );
		AddElemToSizer( l_pButton, p_pSizer );
	}

	void AddCheckBox( wxWindow * p_pParent, wxString const & p_strCaption, wxWindowID p_id, bool p_bValue, void * p_pClientData, wxSizer * p_pSizer )
	{
		wxCheckBox * l_pCheckBox = new wxCheckBox( p_pParent, p_id, p_strCaption );
		l_pCheckBox->SetClientData( p_pClientData );
		l_pCheckBox->SetValue( p_bValue );
		AddElemToSizer( l_pCheckBox, p_pSizer );
	}

	//*************************************************************************************************

	typedef enum eID
	{
		eID_ALTITUDE
		,	eID_THETA
		,	eID_PHI
		,	eID_GRID_SIZE
		,	eID_SEA_COLOUR
		,	eID_EXPOSURE
		,	eID_ANIMATION
		,	eID_GRID
		,	eID_SEA
		,	eID_SUN
		,	eID_SKY
		,	eID_MANUAL_FILTER
		,	eID_OCTAVES
		,	eID_LACUNARITY
		,	eID_GAIN
		,	eID_NORM
		,	eID_CLAMP1
		,	eID_CLAMP2
		,	eID_COLOUR
		,	eID_ENABLE
#if ENABLE_FFT
		,	eID_WIND_SPEED
		,	eID_WAVE_AGE
		,	eID_AMPLITUDE
		,	eID_GENERATE
		,	eID_CHOPPY
#endif
	}	eID;

	MainFrame::MainFrame()
		:	wxFrame( NULL, wxID_ANY, wxT( "OceanLighting" )	)
		,	m_oldx( 0	)
		,	m_oldy( 0	)
		,	m_drag( 0	)
		,	m_width( 1244	)
		,	m_height( 768	)
		,	m_pCastor3D( new Engine()	)
		,	m_timer( NULL	)
	{
		SetClientSize( m_width, m_height );
	}

	MainFrame::~MainFrame()
	{
		delete m_pCastor3D;
	}

	bool MainFrame::Initialise()
	{
		wxPanel * l_pMainPanel = new wxPanel( this, wxID_ANY, wxPoint( 220, 0 ), wxSize( 1024, 768 ) );
		l_pMainPanel->Show();
		bool l_bReturn = DoInitialise3D( l_pMainPanel );

		if ( l_bReturn )
		{
//			TwInit(TW_OPENGL, NULL);
//			TwGLUTModifiersFunc(glutGetModifiers);
//			bar = TwNewBar("Parameters");
//			TwDefine("Parameters size='220 460'");
			wxPanel * l_pOptionsPanel = new wxPanel( this, wxID_ANY, wxPoint( 0, 0 ), wxSize( 220, 768 ) );
			wxBoxSizer * l_pSizerPanel = new wxBoxSizer( wxVERTICAL );
			wxArrayString l_arrayCharsFloats;
			wxTextValidator l_validatorFloat( wxFILTER_INCLUDE_CHAR_LIST );
			l_arrayCharsFloats.push_back( wxT( "0" ) );
			l_arrayCharsFloats.push_back( wxT( "1" ) );
			l_arrayCharsFloats.push_back( wxT( "2" ) );
			l_arrayCharsFloats.push_back( wxT( "3" ) );
			l_arrayCharsFloats.push_back( wxT( "4" ) );
			l_arrayCharsFloats.push_back( wxT( "5" ) );
			l_arrayCharsFloats.push_back( wxT( "6" ) );
			l_arrayCharsFloats.push_back( wxT( "7" ) );
			l_arrayCharsFloats.push_back( wxT( "8" ) );
			l_arrayCharsFloats.push_back( wxT( "9" ) );
			l_arrayCharsFloats.push_back( wxT( "." ) );
			l_arrayCharsFloats.push_back( wxT( "-" ) );
			l_validatorFloat.SetIncludes( l_arrayCharsFloats );
#if ENABLE_FFT
//			TwAddVarCB(bar, "Wind speed", TW_TYPE_FLOAT, setFloat, getFloat, &WIND, "min=3.0 max=21.0 step=1.0 group=Spectrum");
			AddSpinCtrl( l_pOptionsPanel, wxT( "Wind speed" ), eID_WIND_SPEED, 3, 21, int( m_pTechnique->GetWindSpeed() ), &m_pTechnique->GetWindSpeed(), l_pSizerPanel );
//			TwAddVarCB(bar, "Inv. wave age", TW_TYPE_FLOAT, setFloat, getFloat, &OMEGA, "min=0.8 max=5.0 step=0.1 group=Spectrum");
			AddTextCtrl( l_pOptionsPanel, wxT( "Inv. wave age" ), eID_WAVE_AGE, m_pTechnique->GetOmega(), &m_pTechnique->GetOmega(), l_pSizerPanel, l_validatorFloat );
//			TwAddVarCB(bar, "Amplitude", TW_TYPE_FLOAT, setFloat, getFloat, &A, "min=0.01 max=1000.0 step=0.01 group=Spectrum");
			AddTextCtrl( l_pOptionsPanel, wxT( "Amplitude" ), eID_AMPLITUDE, m_pTechnique->GetA(), &m_pTechnique->GetA(), l_pSizerPanel, l_validatorFloat );
//			TwAddButton(bar, "Generate", computeSlopeVarianceTex, NULL, "group=Spectrum");
			AddButton( l_pOptionsPanel, wxT( "Generate" ), eID_GENERATE, l_pSizerPanel );
//			TwAddVarRW(bar, "Choppy", TW_TYPE_BOOL8, &m_choppy, "group=Rendering");
			AddCheckBox( l_pOptionsPanel, wxT( "Choppy" ), eID_CHOPPY, m_pTechnique->GetChoppy(), &m_pTechnique->GetChoppy(), l_pSizerPanel );
#endif
//			TwAddVarRW(bar, "Altitude", TW_TYPE_FLOAT, &m_cameraHeight, "min=-10.0 max=8000 group=Rendering");
			AddSpinCtrl( l_pOptionsPanel, wxT( "Altitude" ), eID_ALTITUDE, -10, 8000, int( m_pTechnique->GetCameraHeight() ), &m_pTechnique->GetCameraHeight(), l_pSizerPanel );
//			TwAddVarRO(bar, "Theta", TW_TYPE_FLOAT, &m_cameraTheta, "group=Rendering");
			AddTextCtrl( l_pOptionsPanel, wxT( "Theta" ), eID_THETA, m_pTechnique->GetCameraTheta(), &m_pTechnique->GetCameraTheta(), l_pSizerPanel, l_validatorFloat );
//			TwAddVarRO(bar, "Phi", TW_TYPE_FLOAT, &m_cameraPhi, "group=Rendering");
			AddTextCtrl( l_pOptionsPanel, wxT( "Phi" ), eID_PHI, m_pTechnique->GetCameraPhi(), &m_pTechnique->GetCameraPhi(), l_pSizerPanel, l_validatorFloat );
//			TwAddVarRW(bar, "Grid size", TW_TYPE_FLOAT, &m_gridSize, "min=1.0 max=10.0 step=1.0 group=Rendering");
			AddSpinCtrl( l_pOptionsPanel, wxT( "Grid size" ), eID_GRID_SIZE, 1, 10, int( m_pTechnique->GetGridSize() ), &m_pTechnique->GetGridSize(), l_pSizerPanel );
//			TwAddVarRW(bar, "Sea color", TW_TYPE_COLOR4F, &m_seaColor, "group=Rendering");
			AddColourPickerCtrl( l_pOptionsPanel, wxT( "Sea color" ), eID_SEA_COLOUR, m_pTechnique->GetSeaColour(), &m_pTechnique->GetSeaColour(), l_pSizerPanel );
//			TwAddVarRW(bar, "Exposure", TW_TYPE_FLOAT, &m_hdrExposure, "min=0.01 max=4.0 step=0.01 group=Rendering");
			AddTextCtrl( l_pOptionsPanel, wxT( "Exposure" ), eID_EXPOSURE, m_pTechnique->GetHdrExposure(), &m_pTechnique->GetHdrExposure(), l_pSizerPanel, l_validatorFloat );
//			TwAddVarRW(bar, "Animation", TW_TYPE_BOOL8, &m_animate, "group=Rendering");
			AddCheckBox( l_pOptionsPanel, wxT( "Animation" ), eID_ANIMATION, m_pTechnique->GetAnimate(), &m_pTechnique->GetAnimate(), l_pSizerPanel );
//			TwAddVarRW(bar, "Grid", TW_TYPE_BOOL8, &m_grid, "group=Rendering");
			AddCheckBox( l_pOptionsPanel, wxT( "Grid" ), eID_GRID, m_pTechnique->GetGrid(), &m_pTechnique->GetGrid(), l_pSizerPanel );
//			TwAddVarCB(bar, "Sea", TW_TYPE_BOOL8, setBool, getBool, &seaContrib, "group=Rendering");
			AddCheckBox( l_pOptionsPanel, wxT( "Sea" ), eID_SEA, m_pTechnique->GetSea(), &m_pTechnique->GetSea(), l_pSizerPanel );
//			TwAddVarCB(bar, "Sun", TW_TYPE_BOOL8, setBool, getBool, &sunContrib, "group=Rendering");
			AddCheckBox( l_pOptionsPanel, wxT( "Sun" ), eID_SUN, m_pTechnique->GetSun(), &m_pTechnique->GetSun(), l_pSizerPanel );
//			TwAddVarCB(bar, "Sky", TW_TYPE_BOOL8, setBool, getBool, &skyContrib, "group=Rendering");
			AddCheckBox( l_pOptionsPanel, wxT( "Sky" ), eID_SKY, m_pTechnique->GetSky(), &m_pTechnique->GetSky(), l_pSizerPanel );
//			TwAddVarCB(bar, "Manual filter", TW_TYPE_BOOL8, setBool, getBool, &manualFilter, "group=Rendering");
			AddCheckBox( l_pOptionsPanel, wxT( "Manual filter" ), eID_MANUAL_FILTER, m_pTechnique->GetManualFilter(), &m_pTechnique->GetManualFilter(), l_pSizerPanel );
//			TwAddVarRW(bar, "Octaves", TW_TYPE_FLOAT, &m_octaves, "min=1.0 max=16.0 step=1.0 group=Clouds");
			AddSpinCtrl( l_pOptionsPanel, wxT( "Clouds octaves" ), eID_OCTAVES, 1, 16, int( m_pTechnique->GetOctaves() ), &m_pTechnique->GetOctaves(), l_pSizerPanel );
//			TwAddVarRW(bar, "Clouds lacunarity", TW_TYPE_FLOAT, &m_lacunarity, "min=0.1 max=3.0 step=0.1 group=Clouds");
			AddTextCtrl( l_pOptionsPanel, wxT( "Clouds lacunarity" ), eID_LACUNARITY, m_pTechnique->GetLacunarity(), &m_pTechnique->GetLacunarity(), l_pSizerPanel, l_validatorFloat );
//			TwAddVarRW(bar, "Clouds gain", TW_TYPE_FLOAT, &m_gain, "min=0.01 max=2.0 step=0.01 group=Clouds");
			AddTextCtrl( l_pOptionsPanel, wxT( "Clouds gain" ), eID_GAIN, m_pTechnique->GetGain(), &m_pTechnique->GetGain(), l_pSizerPanel, l_validatorFloat );
//			TwAddVarRW(bar, "Clouds norm", TW_TYPE_FLOAT, &m_norm, "min=0.01 max=1.0 step=0.01 group=Clouds");
			AddTextCtrl( l_pOptionsPanel, wxT( "Clouds norm" ), eID_NORM, m_pTechnique->GetNorm(), &m_pTechnique->GetNorm(), l_pSizerPanel, l_validatorFloat );
//			TwAddVarRW(bar, "Clouds clamp1", TW_TYPE_FLOAT, &m_clamp1, "min=-1.0 max=1.0 step=0.01 group=Clouds");
			AddTextCtrl( l_pOptionsPanel, wxT( "Clouds clamp1" ), eID_CLAMP1, m_pTechnique->GetClamp1(), &m_pTechnique->GetClamp1(), l_pSizerPanel, l_validatorFloat );
//			TwAddVarRW(bar, "Clouds clamp2", TW_TYPE_FLOAT, &m_clamp2, "min=-1.0 max=1.0 step=0.01 group=Clouds");
			AddTextCtrl( l_pOptionsPanel, wxT( "Clouds clamp2" ), eID_CLAMP2, m_pTechnique->GetClamp2(), &m_pTechnique->GetClamp2(), l_pSizerPanel, l_validatorFloat );
//			TwAddVarRW(bar, "Clouds color", TW_TYPE_COLOR4F, &m_cloudColor, "group=Clouds");
			AddColourPickerCtrl( l_pOptionsPanel, wxT( "Clouds Color" ), eID_COLOUR, m_pTechnique->GetColour(), &m_pTechnique->GetColour(), l_pSizerPanel );
//			TwAddVarCB(bar, "Enable", TW_TYPE_BOOL8, setBool, getBool, &m_cloudLayer, "group=Clouds");
			AddCheckBox( l_pOptionsPanel, wxT( "Enable clouds" ), eID_ENABLE, m_pTechnique->GetClouds(), &m_pTechnique->GetClouds(), l_pSizerPanel );
			l_pOptionsPanel->SetSizer( l_pSizerPanel );
			l_pSizerPanel->SetSizeHints( l_pOptionsPanel );
			wxBoxSizer * l_pSizerFrame = new wxBoxSizer( wxHORIZONTAL );
			l_pSizerFrame->Add( l_pOptionsPanel, wxSizerFlags( 0 ).Expand().FixedMinSize() );
			l_pSizerFrame->Add( l_pMainPanel, wxSizerFlags( 1 ).Expand().FixedMinSize() );
			SetSizer( l_pSizerFrame );
			l_pSizerFrame->SetSizeHints( this );
		}

		return l_bReturn;
	}

	bool MainFrame::DoInitialise3D( wxWindow * p_pParent )
	{
		typedef std::shared_ptr< std::thread > thread_sptr;
		DECLARE_VECTOR( thread_sptr, ThreadPtr );
		bool l_bReturn = true;
		Logger::LogInfo( cuT( "Initialising Castor3D" ) );
		m_pCastor3D->Initialise( 30, false );
		m_pCastor3D->GetTechniqueFactory().Register( cuT( "ocean lighting" ), &RenderTechnique::Create );
		StringArray l_arrayFiles;
		StringArray l_arrayFailed;
		std::mutex l_mutex;
		ThreadPtrArray l_arrayThreads;
		File::ListDirectoryFiles( Engine::GetPluginsDirectory(), l_arrayFiles );

		if ( l_arrayFiles.size() > 0 )
		{
			std::for_each( l_arrayFiles.begin(), l_arrayFiles.end(), [&]( Path const & p_pathFile )
			{
				if ( p_pathFile.GetExtension() == CASTOR_DLL_EXT )
				{
					l_arrayThreads.push_back( std::make_shared< std::thread >( PluginLoader( &l_arrayFailed, &l_mutex ), m_pCastor3D, p_pathFile ) );
				}
			} );
			std::for_each( l_arrayThreads.begin(), l_arrayThreads.end(), [&]( thread_sptr p_pThread )
			{
				p_pThread->join();
			} );
			l_arrayThreads.clear();
		}

		if ( l_arrayFailed.size() > 0 )
		{
			std::for_each( l_arrayFailed.begin(), l_arrayFailed.end(), [&]( Path const & p_pathFile )
			{
				l_arrayThreads.push_back( std::make_shared< std::thread >( PluginLoader(), m_pCastor3D, p_pathFile ) );
			} );
			std::for_each( l_arrayThreads.begin(), l_arrayThreads.end(), [&]( thread_sptr p_pThread )
			{
				p_pThread->join();
			} );
			l_arrayThreads.clear();
		}

		if ( l_arrayFailed.size() > 0 )
		{
			Logger::LogWarning( cuT( "Some plugins couldn't be loaded :" ) );
			std::for_each( l_arrayFailed.begin(), l_arrayFailed.end(), [&]( Path const & p_pathFile )
			{
				Logger::LogWarning( p_pathFile.GetFileName() );
			} );
			l_arrayFailed.clear();
		}

		Logger::LogInfo( cuT( "Plugins loaded" ) );

		try
		{
			eRENDERER_TYPE l_eRenderer = eRENDERER_TYPE_OPENGL;

			if ( l_bReturn )
			{
				l_bReturn = m_pCastor3D->LoadRenderer( l_eRenderer );
			}
		}
		catch ( ... )
		{
			wxMessageBox( _( "Problem occured while initialising Castor3D.\nLook at OceanLighting.log for more details" ), _( "Exception" ), wxOK | wxCENTRE | wxICON_ERROR );
			l_bReturn = false;
		}

		if ( l_bReturn )
		{
			SceneSPtr l_pScene = m_pCastor3D->CreateScene( cuT( "DummyScene" ) );
			SceneNodeSPtr l_pNode = l_pScene->CreateSceneNode( cuT( "DummyCameraNode" ), l_pScene->GetCameraRootNode() );
			CameraSPtr l_pCamera = l_pScene->CreateCamera( cuT( "DummyCamera" ), m_width, m_height, l_pNode, eVIEWPORT_TYPE_3D );
			RenderTargetSPtr l_pTarget = m_pCastor3D->CreateRenderTarget( eTARGET_TYPE_WINDOW );
			l_pTarget->SetPixelFormat( ePIXEL_FORMAT_A8R8G8B8 );
			l_pTarget->SetDepthFormat( ePIXEL_FORMAT_DEPTH24S8 );
			l_pTarget->SetSize( Size( m_width, m_height ) );
			l_pTarget->SetScene( l_pScene );
			l_pTarget->SetCamera( l_pCamera );
			Parameters l_params;
			m_pTechnique = std::static_pointer_cast< RenderTechnique >( m_pCastor3D->CreateTechnique( cuT( "ocean lighting" ), *l_pTarget, l_params ) );
			m_pTechnique->SetWidth( m_width );
			m_pTechnique->SetHeight( m_height );
			l_pTarget->SetTechnique( m_pTechnique );
			m_pWindow = m_pCastor3D->CreateRenderWindow();
			m_pWindow->SetRenderTarget( l_pTarget );
#if defined( _WIN32 )
			WindowHandle l_handle( std::make_shared< IMswWindowHandle >( p_pParent->GetHandle() ) );
#elif defined( __linux__ )
			GtkWidget * l_pGtkWidget = static_cast< GtkWidget * >( p_pParent->GetHandle() );
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

			WindowHandle l_handle( std::make_shared< IXWindowHandle >( l_drawable, l_pDisplay ) );
#else
#	error "Yet unsupported OS"
#endif
			l_bReturn = m_pWindow->Initialise( l_handle );
		}

		if ( l_bReturn )
		{
			Logger::LogInfo( cuT( "Castor3D Initialised" ) );
			m_timer = new wxTimer( this, 1 );
			m_timer->Start( 1000 / 30 );
		}

		Show( l_bReturn );
		return l_bReturn;
	}

	BEGIN_EVENT_TABLE( MainFrame, wxFrame )
		EVT_PAINT(	MainFrame::OnPaint	)
		EVT_ERASE_BACKGROUND(	MainFrame::OnEraseBackground	)
		EVT_CLOSE(	MainFrame::OnClose	)
		EVT_TIMER(	1,			MainFrame::OnTimer	)
		EVT_SIZE(	MainFrame::OnSize	)
		EVT_KEY_UP(	MainFrame::OnKeyUp	)
		EVT_LEFT_DOWN(	MainFrame::OnMouseLDown	)
		EVT_LEFT_UP(	MainFrame::OnMouseLUp	)
		EVT_MOTION(	MainFrame::OnMouseMove	)
		EVT_LEAVE_WINDOW(	MainFrame::OnMouseLeave	)
	END_EVENT_TABLE()

	void MainFrame::OnPaint( wxPaintEvent & WXUNUSED( p_event ) )
	{
		wxPaintDC l_dc( this );

		if ( !m_pWindow )
		{
			l_dc.SetBrush( wxBrush( *wxWHITE ) );
			l_dc.SetPen( wxPen( *wxWHITE ) );
			l_dc.DrawRectangle( 0, 0, GetClientSize().x, GetClientSize().y );
		}
	}

	void MainFrame::OnEraseBackground( wxEraseEvent & WXUNUSED( p_event ) )
	{
	}

	void MainFrame::OnClose( wxCloseEvent & p_event )
	{
		Logger::UnregisterCallback( this );
		Hide();

		if ( m_timer )
		{
			m_timer->Stop();
			delete m_timer;
			m_timer = nullptr;
		}

		m_pTechnique.reset();
		m_pWindow.reset();

		if ( m_pCastor3D )
		{
			m_pCastor3D->Cleanup();
		}

		DestroyChildren();
		p_event.Skip();
	}

	void MainFrame::OnTimer( wxTimerEvent & p_event )
	{
		if ( m_pCastor3D )
		{
			m_pCastor3D->RenderOneFrame();
		}

//		TwDraw();
		p_event.Skip();
	}

	void MainFrame::OnSize( wxSizeEvent & p_event )
	{
		m_width = p_event.GetSize().x;
		m_height = p_event.GetSize().x;

		if ( m_pTechnique )
		{
			m_pTechnique->Resize( m_width, m_height );
		}

//		TwWindowSize(x, y);
		p_event.Skip();
	}

	void MainFrame::OnKeyUp( wxKeyEvent & p_event )
	{
//		if (TwEventKeyboardGLUT(c, x, y))
//		{
//			return;
//		}
		switch ( p_event.GetKeyCode() )
		{
			/*
					case 27:
						Close();
						break;
			*/
		case '+':
			if ( m_pTechnique )
			{
				m_pTechnique->CameraThetaPlus( 5.0f );
			}

			break;

		case '-':
			if ( m_pTechnique )
			{
				m_pTechnique->CameraThetaMinus( 5.0f );
			}

			break;

		case WXK_LEFT:
			if ( m_pTechnique )
			{
				m_pTechnique->UpdateCameraPhi( -5.0f );
			}

			break;

		case WXK_RIGHT:
			if ( m_pTechnique )
			{
				m_pTechnique->UpdateCameraPhi( 5.0f );
			}

			break;

		case WXK_PAGEUP:
			if ( m_pTechnique )
			{
				m_pTechnique->CameraHeightPlus( 1.1f );
			}

//			TwRefreshBar(bar);
			break;

		case WXK_PAGEDOWN:
			if ( m_pTechnique )
			{
				m_pTechnique->CameraHeightMinus( 1.1f );
			}

//			TwRefreshBar(bar);
			break;
		}

		p_event.Skip();
	}

	void MainFrame::OnMouseLDown( wxMouseEvent & p_event )
	{
		m_oldx = p_event.GetX();
		m_oldy = p_event.GetY();
		m_drag = true;
		p_event.Skip();
	}

	void MainFrame::OnMouseLUp( wxMouseEvent & p_event )
	{
		m_drag = false;
		p_event.Skip();
	}

	void MainFrame::OnMouseMove( wxMouseEvent & p_event )
	{
		if ( m_drag )
		{
			if ( m_pTechnique )
			{
				m_pTechnique->UpdateSunPhi( m_oldx - p_event.GetX() );
				m_pTechnique->UpdateSunTheta( p_event.GetY() - m_oldy );
			}

			m_oldx = p_event.GetX();
			m_oldy = p_event.GetY();
		}

		p_event.Skip();
	}

	void MainFrame::OnMouseLeave( wxMouseEvent & p_event )
	{
		m_drag = false;
		p_event.Skip();
	}

	void MainFrame::getBool( void * value, void * clientData )
	{
		*( ( bool * ) value ) = *( ( bool * ) clientData );
	}

	void MainFrame::setBool( const void * value, void * clientData )
	{
		*( ( bool * ) clientData ) = *( ( bool * ) value );

		if ( m_pTechnique )
		{
			m_pTechnique->SetReloadPrograms( *( ( bool * ) clientData ) );
		}
	}

	// precomputes filtered slope m_variances in a 3d texture, based on the wave spectrum
	void MainFrame::computeSlopeVarianceTex( void * unused )
	{
		if ( m_pTechnique )
		{
			m_pTechnique->SetComputeSlopeVarianceTex();
		}
	}

	void MainFrame::getFloat( void * value, void * clientData )
	{
		*( ( float * ) value ) = *( ( float * ) clientData );
	}

	void MainFrame::setFloat( const void * value, void * clientData )
	{
		*( ( float * ) clientData ) = *( ( float * ) value );

		if ( m_pTechnique )
		{
			m_pTechnique->SetGenerateWavesSpectrum();
		}
	}

	void MainFrame::getInt( void * value, void * clientData )
	{
		*( ( int * ) value ) = *( ( int * ) clientData );
	}

	void MainFrame::setInt( const void * value, void * clientData )
	{
		*( ( int * ) clientData ) = *( ( int * ) value );

		if ( m_pTechnique )
		{
			m_pTechnique->SetGenerateWavesSpectrum();
		}
	}

	void MainFrame::getBool2( void * value, void * clientData )
	{
		*( ( bool * ) value ) = *( ( bool * ) clientData );
	}

	void MainFrame::setBool2( const void * value, void * clientData )
	{
		*( ( bool * ) clientData ) = *( ( bool * ) value );

		if ( m_pTechnique )
		{
			m_pTechnique->SetGenerateWavesSpectrum();
		}
	}
}
