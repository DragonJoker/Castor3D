#include "CastorViewer/PrecompiledHeader.hpp"

#include "CastorViewer/RenderPanel.hpp"

#include "CastorViewer/MainFrame.hpp"
#include "CastorViewer/CastorViewer.hpp"

#include "xpms/geo_blanc.xpm"
#include "xpms/scene_blanc.xpm"
#include "xpms/mat_blanc.xpm"
#include "xpms/castor_transparent.xpm"
#include "xpms/castor.xpm"
#include "xpms/geo_visible.xpm"
#include "xpms/geo_visible_sel.xpm"
#include "xpms/geo_cachee.xpm"
#include "xpms/geo_cachee_sel.xpm"
#include "xpms/dossier.xpm"
#include "xpms/dossier_sel.xpm"
#include "xpms/dossier_ouv.xpm"
#include "xpms/dossier_ouv_sel.xpm"
#include "xpms/submesh.xpm"
#include "xpms/submesh_sel.xpm"
#include "xpms/export.xpm"

using namespace CastorViewer;
using namespace Castor3D;
using namespace Castor;

#define CASTOR3D_THREADED 0

#if defined( NDEBUG )
static const int CASTOR_WANTED_FPS	= 120;
#else
static const int CASTOR_WANTED_FPS	= 30;
#endif

//*************************************************************************************************

struct PluginLoader
{
	StringArray * m_pArrayFailed;
	std::mutex * m_pMutex;

	PluginLoader( StringArray * p_pArrayFailed=NULL, std::mutex * p_pMutex=NULL )
		:	m_pArrayFailed	( p_pArrayFailed	)
		,	m_pMutex		( p_pMutex			)
	{
	}
	void operator()( Engine * p_pEngine, Path const & p_pathFile )
	{
		PluginBaseSPtr l_pPlugin = p_pEngine->LoadPlugin( p_pathFile );

		if( !l_pPlugin )
		{
			if( m_pMutex && m_pArrayFailed )
			{
				m_pMutex->lock();
				m_pArrayFailed->push_back( p_pathFile );
				m_pMutex->unlock();
			}
		}
	}
};

//*************************************************************************************************

DECLARE_APP( CastorViewerApp )

MainFrame :: MainFrame( wxWindow * p_pParent, wxString const & p_strTitle, eRENDERER_TYPE p_eRenderer )
	:	wxFrame				( p_pParent, wxID_ANY, p_strTitle, wxDefaultPosition, wxSize( 800, 700 )	)
	,	m_pCastor3D			( new Engine( Logger::GetSingletonPtr() )									)
	,	m_pRenderPanel		( NULL																		)
	,	m_pImagesLoader		( new ImagesLoader															)
	,	m_timer				( NULL																		)
	,	m_pListLog			( NULL																		)
	,	m_iListHeight		( 100																		)
	,	m_eRenderer			( p_eRenderer																)
	,	m_pGeometriesFrame	( NULL																		)
{
	m_pAuiManager =  new wxAuiManager( this );
}

bool MainFrame :: Initialise()
{
	wxDisplay l_display;
	wxRect l_rect = l_display.GetClientArea();
	wxString l_strCopyright;
	l_strCopyright << wxDateTime().Now().GetCurrentYear();
	wxSplashScreen l_splashScreen( this, wxT( "Castor\nViewer" ), l_strCopyright + cuT( " " ) + _( " DragonJoker, All rights shared" ), wxPoint( 10, 230 ), wxPoint( 200, 300 ), wxPoint( 180, 260 ), wxPoint( (l_rect.width - 512) / 2, (l_rect.height - 384) / 2 ), 8 );
	m_pSplashScreen = &l_splashScreen;
	bool l_bReturn = DoInitialiseImages();

	if( l_bReturn )
	{
		CreateStatusBar();
		DoPopulateToolbar();
		wxIcon l_icon = wxIcon( castor_xpm );
		SetIcon( l_icon );

		m_pListLog = new wxListView( this, wxID_ANY, wxDefaultPosition, wxSize( 200, m_iListHeight ) );
		m_pListLog->InsertColumn( 0, _( "Log"	), 0 );
		m_pAuiManager->AddPane( m_pListLog, wxAuiPaneInfo().CloseButton().Caption( _( "Log" ) ).Direction( wxBOTTOM ).Dock().Bottom().BottomDockable().TopDockable().Movable().PinButton() );
#if defined( NDEBUG )
		m_pListLog->Hide();
#else
		m_pListLog->Hide();
#endif
		Logger::SetCallback( Castor::PLogCallback( DoLogCallback ), this );

		l_bReturn = DoInitialise3D();
	}

	l_splashScreen.Close();
	Show( l_bReturn );

	return l_bReturn;
}

MainFrame :: ~MainFrame()
{
    if( m_pAuiManager )
    {
        m_pAuiManager->UnInit();
        delete m_pAuiManager;
    }

	delete m_pImagesLoader;
	delete m_pCastor3D;
}

void MainFrame :: LoadScene( wxString const & p_strFileName )
{
	if( m_pRenderPanel && m_pCastor3D )
	{
		Logger::LogDebug( (wxChar const *)(cuT( "MainFrame :: LoadScene - " ) + p_strFileName).c_str() );

		if( !p_strFileName.empty() )
		{
			Logger::LogDebug( cuT( "MainFrame :: LoadScene - param not empty" ) );
			m_strFilePath = (wxChar const *)p_strFileName.c_str();
			Logger::LogDebug( cuT( "MainFrame :: LoadScene - " ) + m_strFilePath );
		}

		if( !m_strFilePath.empty() )
		{
			Logger::LogDebug( cuT( "MainFrame :: LoadScene - file path not empty" ) );
			String l_strLowered = str_utils::lower_case( m_strFilePath );
			Logger::LogDebug( cuT( "MainFrame :: LoadScene - file path lowered : " ) + l_strLowered );

			if( m_pMainScene.lock() )
			{
				m_pMainScene.lock()->ClearScene();
				m_pCastor3D->GetSceneManager().erase( m_pMainScene.lock()->GetName() );
				Logger::LogDebug( cuT( "MainFrame :: LoadScene - scene erased from manager" ) );
				m_pMainScene.reset();
				Logger::LogDebug( cuT( "Scene cleared" ) );
			}

			m_pRenderPanel->SetRenderWindow( nullptr );

			m_pCastor3D->Cleanup();
			Logger::LogDebug( cuT( "MainFrame :: LoadScene - Engine cleared" ) );

			Path l_meshFilePath = m_strFilePath;
			str_utils::replace( l_meshFilePath, cuT( "cscn" ), cuT( "cmsh" ) );

			if( File::FileExists( l_meshFilePath ) )
			{
				BinaryFile l_fileMesh( l_meshFilePath, File::eOPEN_MODE_READ );
				Logger::LogMessage( cuT( "Loading meshes file : " ) + l_meshFilePath );

				if( m_pCastor3D->LoadMeshes( l_fileMesh ) )
				{
					Logger::LogMessage( cuT( "Meshes read" ) );
				}
				else
				{
					Logger::LogMessage( cuT( "Can't read meshes" ) );
					return;
				}
			}

			if( File::FileExists( m_strFilePath ) )
			{
				Logger::LogMessage( cuT( "Loading scene file : " ) + m_strFilePath );
				SceneFileParser l_parser( m_pCastor3D );

				if( l_parser.ParseFile( m_strFilePath ) )
				{
					m_pCastor3D->Initialise( CASTOR_WANTED_FPS, CASTOR3D_THREADED );
					RenderWindowSPtr l_pRenderWindow = l_parser.GetRenderWindow();

					if( l_pRenderWindow )
					{
						m_pRenderPanel->SetRenderWindow( l_pRenderWindow );

						if( l_pRenderWindow->IsInitialised() )
						{
							m_pMainScene = l_pRenderWindow->GetScene();

							if( l_pRenderWindow->IsFullscreen() )
							{
								ShowFullScreen( true, wxFULLSCREEN_ALL );
							}

							Logger::LogMessage( cuT( "Scene file read" ) );
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
			}
			else
			{
				wxMessageBox( _( "Scene file doesn't exist :" ) + wxString( wxT( "\n" ) ) + m_strFilePath );
			}
		}
	}
	else
	{
		wxMessageBox( _( "Can't open a scene file : no engine loaded" ) );
	}
}

bool MainFrame :: DoInitialise3D()
{
	bool l_bReturn = true;
	m_pSplashScreen->Step( _( "Loading plugins" ), 1 );
	Logger::LogMessage( cuT( "Initialising Castor3D" ) );

	m_pCastor3D->Initialise( CASTOR_WANTED_FPS, CASTOR3D_THREADED );

	StringArray l_arrayFiles;
	StringArray l_arrayFailed;
	std::mutex l_mutex;
	ThreadPtrArray l_arrayThreads;
	File::ListDirectoryFiles( Engine::GetPluginsPath(), l_arrayFiles );

 	if( l_arrayFiles.size() > 0 )
	{
		std::for_each( l_arrayFiles.begin(), l_arrayFiles.end(), [&]( Path const & p_pathFile )
		{
			if( p_pathFile.GetExtension() == CASTOR_DLL_EXT )
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


 	if( l_arrayFailed.size() > 0 )
	{
		StringArray l_arrayFailed2;
		std::swap( l_arrayFailed2, l_arrayFailed );

		std::for_each( l_arrayFailed2.begin(), l_arrayFailed2.end(), [&]( Path const & p_pathFile )
		{
			l_arrayThreads.push_back( std::make_shared< std::thread >( PluginLoader( &l_arrayFailed, &l_mutex ), m_pCastor3D, p_pathFile ) );
		} );

		std::for_each( l_arrayThreads.begin(), l_arrayThreads.end(), [&]( thread_sptr p_pThread )
		{
			p_pThread->join();
		} );

		l_arrayThreads.clear();
	}

	if( l_arrayFailed.size() > 0 )
	{
		Logger::LogWarning( cuT( "Some plugins couldn't be loaded :" ) );

		std::for_each( l_arrayFailed.begin(), l_arrayFailed.end(), [&]( Path const & p_pathFile )
		{
			Logger::LogWarning( p_pathFile.GetFileName() );
		} );

		l_arrayFailed.clear();
	}

	Logger::LogMessage( cuT( "Plugins loaded") );

	try
	{
		if( m_eRenderer == eRENDERER_TYPE_UNDEFINED )
		{
			wxRendererSelector m_dialog( m_pCastor3D, this, wxT( "Castor Viewer" ) );
			m_pSplashScreen->Step( _( "Initialising main window" ), 1 );
			int l_iReturn = m_dialog.ShowModal();

			if( l_iReturn == wxID_OK )
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

		if( l_bReturn )
		{
			l_bReturn = m_pCastor3D->LoadRenderer( m_eRenderer );
		}

		if( l_bReturn )
		{
			Logger::LogMessage( cuT( "Castor3D Initialised" ) );
			SetClientSize( 800, 600 + m_iListHeight );
			int l_width = GetClientSize().x;
			int l_height = GetClientSize().y;
			m_pRenderPanel = new RenderPanel( this, wxID_ANY, wxDefaultPosition, wxSize( l_width, l_height - m_iListHeight ) ), wxSizerFlags( 1 ).Expand();
			m_pRenderPanel->Show();

			wxBoxSizer * l_pSizerAll = new wxBoxSizer( wxVERTICAL	);
			wxBoxSizer * l_pSizerLog = new wxBoxSizer( wxHORIZONTAL	);

			l_pSizerLog->Add( m_pListLog,		wxSizerFlags( 1 )			);

			l_pSizerAll->Add( m_pRenderPanel,	wxSizerFlags( 1 ).Expand()	);
			l_pSizerAll->Add( l_pSizerLog,		wxSizerFlags( 0 ).Expand()	);

			SetSizer( l_pSizerAll );
			l_pSizerAll->SetSizeHints( this );

            if( m_pAuiManager )
            {
                m_pAuiManager->AddPane( m_pRenderPanel, wxAuiPaneInfo().Direction( wxCENTER ).Dock().CentrePane().PaneBorder( false ) );
            }

#if !CASTOR3D_THREADED
			if( m_timer == NULL )
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

bool MainFrame :: DoInitialiseImages()
{
	m_pSplashScreen->Step( _( "Loading images" ), 1 );
	m_pImagesLoader->AddBitmap( CV_IMG_CASTOR,									castor_transparent_xpm	);
	m_pImagesLoader->AddBitmap( wxGeometriesListFrame::eBMP_VISIBLE,			geo_visible_xpm			);
	m_pImagesLoader->AddBitmap( wxGeometriesListFrame::eBMP_VISIBLE_SEL,		geo_visible_sel_xpm		);
	m_pImagesLoader->AddBitmap( wxGeometriesListFrame::eBMP_HIDDEN,				geo_cachee_xpm			);
	m_pImagesLoader->AddBitmap( wxGeometriesListFrame::eBMP_HIDDEN_SEL,			geo_cachee_sel_xpm		);
	m_pImagesLoader->AddBitmap( wxGeometriesListFrame::eBMP_GEOMETRY,			dossier_xpm				);
	m_pImagesLoader->AddBitmap( wxGeometriesListFrame::eBMP_GEOMETRY_SEL,		dossier_sel_xpm			);
	m_pImagesLoader->AddBitmap( wxGeometriesListFrame::eBMP_GEOMETRY_OPEN,		dossier_ouv_xpm			);
	m_pImagesLoader->AddBitmap( wxGeometriesListFrame::eBMP_GEOMETRY_OPEN_SEL,	dossier_ouv_sel_xpm		);
	m_pImagesLoader->AddBitmap( wxGeometriesListFrame::eBMP_SUBMESH,			submesh_xpm				);
	m_pImagesLoader->AddBitmap( wxGeometriesListFrame::eBMP_SUBMESH_SEL,		submesh_sel_xpm			);
	m_pImagesLoader->AddBitmap( eBMP_SCENE,										scene_blanc_xpm			);
	m_pImagesLoader->AddBitmap( eBMP_GEOMETRIES,								geo_blanc_xpm			);
	m_pImagesLoader->AddBitmap( eBMP_MATERIALS,									mat_blanc_xpm			);
	m_pImagesLoader->AddBitmap( eBMP_EXPORT,									export_xpm				);
	m_pImagesLoader->WaitAsyncLoads();
	return true;
}

void MainFrame :: DoPopulateToolbar()
{
	m_pSplashScreen->Step( _( "Loading toolbar" ), 1 );
	wxToolBar * l_pToolbar = CreateToolBar( wxTB_FLAT | wxTB_HORIZONTAL );

	l_pToolbar->SetBackgroundColour( * wxWHITE );
	l_pToolbar->SetToolBitmapSize( wxSize( 32, 32 ) );

	l_pToolbar->AddTool( eID_TOOL_LOAD_SCENE,	_( "Load Scene"		),	wxImage( *m_pImagesLoader->GetBitmap( eBMP_SCENE		) ).Rescale( 32, 32, wxIMAGE_QUALITY_HIGH),	_( "Open a new scene"			) );m_pSplashScreen->Step( 1 );
	l_pToolbar->AddTool( eID_TOOL_EXPORT_SCENE,	_( "Export Scene"	),	wxImage( *m_pImagesLoader->GetBitmap( eBMP_EXPORT		) ).Rescale( 32, 32, wxIMAGE_QUALITY_HIGH),	_( "Export the current scene"	) );m_pSplashScreen->Step( 1 );
	l_pToolbar->AddSeparator();
	l_pToolbar->AddTool( eID_TOOL_GEOMETRIES,	_( "Geometries"	),	wxImage( *m_pImagesLoader->GetBitmap( eBMP_GEOMETRIES	) ).Rescale( 32, 32, wxIMAGE_QUALITY_HIGH),	_( "Display geometries"	) );m_pSplashScreen->Step( 1 );
	l_pToolbar->AddTool( eID_TOOL_MATERIALS,	_( "Materials"	),	wxImage( *m_pImagesLoader->GetBitmap( eBMP_MATERIALS	) ).Rescale( 32, 32, wxIMAGE_QUALITY_HIGH),	_( "Display materials"	) );m_pSplashScreen->Step( 1 );
	l_pToolbar->AddSeparator();

	l_pToolbar->Realize();
}

void MainFrame :: DoLog( String const & p_strLog, eLOG_TYPE p_eLogType )
{
	if( m_pListLog )
	{
		m_pListLog->InsertItem( 0, p_strLog );
	}
}

void MainFrame :: DoLogCallback( MainFrame * p_pThis, String const & p_strLog, eLOG_TYPE p_eLogType )
{
	p_pThis->DoLog( p_strLog, p_eLogType );
}

void MainFrame :: DoExportScene( Castor::Path const & p_pathFile )
{
	if( p_pathFile.GetExtension() == cuT( "obj" ) )
	{
		DoObjExportScene( p_pathFile );
	}
	else if( p_pathFile.GetExtension() == cuT( "cscn" ) )
	{
		DoCscnExportScene( p_pathFile );
	}
	else if( p_pathFile.GetExtension() == cuT( "cbsn" ) )
	{
		DoBinaryExportScene( p_pathFile );
	}
}

void MainFrame :: DoObjExportScene( Castor::Path const & p_pathFile )
{
	String l_strFinal;
	Version l_version;
	StringStream l_streamVersion;
	l_streamVersion << l_version.m_iMajor << cuT( "." ) << l_version.m_iMinor << cuT( "." ) << l_version.m_iBuild;

	// First we export the materials
	Path l_pathFileMtl( p_pathFile );
	str_utils::replace( l_pathFileMtl, l_pathFileMtl.GetExtension(), cuT( "mtl" ) );
	MaterialManager const & l_mtlManager = m_pCastor3D->GetMaterialManager();
	l_strFinal.clear();
	l_strFinal += cuT( "#######################################################\n" );
	l_strFinal += cuT( "#      MTL File generated by CastorViewer v" ) + l_streamVersion.str() + cuT( "      #\n" );
	l_strFinal += cuT( "#######################################################\n\n" );

	l_mtlManager.lock();
	std::for_each( l_mtlManager.begin(), l_mtlManager.end(), [&]( std::pair< String, MaterialSPtr > p_pair )
	{
		l_strFinal << DoObjExport( l_pathFileMtl.GetPath(), p_pair.second ) << cuT( "\n" );
	} );
	l_mtlManager.unlock();

	TextFile l_fileMtl( l_pathFileMtl, File::eOPEN_MODE_WRITE, File::eENCODING_MODE_ASCII );
	l_fileMtl.WriteText( l_strFinal );

	// Then we export meshes
	Path l_pathFileObj( p_pathFile );
	str_utils::replace( l_pathFileObj, l_pathFileObj.GetExtension(), cuT( "obj" ) );
	MeshCollection const & l_mshManager = m_pCastor3D->GetMeshManager();
	uint32_t l_uiOffset = 1;
	uint32_t l_uiCount = 0;
	l_strFinal.clear();
	l_strFinal += cuT( "#######################################################\n" );
	l_strFinal += cuT( "#      OBJ File generated by CastorViewer v" ) + l_streamVersion.str() + cuT( "      #\n" );
	l_strFinal += cuT( "#######################################################\n\n" );
	l_strFinal += cuT( "mtllib " ) + l_pathFileMtl.GetFullFileName() + cuT( "\n\n" );

	l_mshManager.lock();
	for( MeshCollection::TObjPtrMapConstIt l_it = l_mshManager.begin(); l_it != l_mshManager.end(); ++l_it )
	{
		l_strFinal << DoObjExport( l_it->second, l_uiOffset, l_uiCount ) << cuT( "\n" );
	}
	l_mshManager.unlock();

	TextFile l_fileObj( l_pathFileObj, File::eOPEN_MODE_WRITE, File::eENCODING_MODE_ASCII );
	l_fileObj.WriteText( l_strFinal );
}

Path GetTextureNewPath( Path const & p_pathSrcFile, Path const & p_pathFolder )
{
	Path l_pathReturn( cuT( "Texture" ) );
	l_pathReturn /= p_pathSrcFile.GetFullFileName();

	if( !wxDirExists( (p_pathFolder / cuT( "Texture" )).c_str() ) )
	{
		wxMkDir( str_utils::to_str( p_pathFolder / cuT( "Texture" ) ).c_str(), 0777 );
	}

	if( wxFileExists( p_pathSrcFile ) )
	{
		Logger::LogDebug( cuT( "Copying [" ) + p_pathSrcFile + cuT( "] to [" ) + p_pathFolder / l_pathReturn + cuT( "]" ) );
		wxCopyFile( p_pathSrcFile, p_pathFolder / l_pathReturn );
	}

	return l_pathReturn;
};

String MainFrame :: DoObjExport( Path const & p_pathMtlFolder, MaterialSPtr p_pMaterial )
{
	String l_strReturn;
	PassSPtr l_pPass = p_pMaterial->GetPass( 0 );

	if( l_pPass )
	{
		Colour l_clAmbient = l_pPass->GetAmbient();
		Colour l_clDiffuse = l_pPass->GetDiffuse();
		Colour l_clSpecular = l_pPass->GetSpecular();
		TextureUnitSPtr l_pAmbient	= l_pPass->GetTextureUnit( eTEXTURE_CHANNEL_AMBIENT		);
		TextureUnitSPtr l_pDiffuse	= l_pPass->GetTextureUnit( eTEXTURE_CHANNEL_DIFFUSE		);
		TextureUnitSPtr l_pNormal	= l_pPass->GetTextureUnit( eTEXTURE_CHANNEL_NORMAL		);
		TextureUnitSPtr l_pOpacity	= l_pPass->GetTextureUnit( eTEXTURE_CHANNEL_OPACITY		);
		TextureUnitSPtr l_pSpecular	= l_pPass->GetTextureUnit( eTEXTURE_CHANNEL_SPECULAR	);
		TextureUnitSPtr l_pGloss	= l_pPass->GetTextureUnit( eTEXTURE_CHANNEL_GLOSS		);
		l_strReturn << cuT( "newmtl "	) << p_pMaterial->GetName() << cuT( "\n" );
		l_strReturn << cuT( "	Ka "	) << l_clAmbient.red().value() << cuT( " " ) << l_clAmbient.green().value() << cuT( " " ) << l_clAmbient.blue().value() << cuT( "\n" );
		l_strReturn << cuT( "	Kd "	) << l_clDiffuse.red().value() << cuT( " " ) << l_clDiffuse.green().value() << cuT( " " ) << l_clDiffuse.blue().value() << cuT( "\n" );
		l_strReturn << cuT( "	Ks "	) << l_clSpecular.red().value() << cuT( " " ) << l_clSpecular.green().value() << cuT( " " ) << l_clSpecular.blue().value() << cuT( "\n" );
		l_strReturn << cuT( "	Ns "	) << l_pPass->GetShininess() << cuT( "\n" );
		l_strReturn << cuT( "	d "		) << l_pPass->GetAlpha() << cuT( "\n" );

		if( l_pAmbient && !l_pAmbient->GetTexturePath().empty() )
		{
			l_strReturn += cuT( "	map_Ka " ) + GetTextureNewPath( l_pAmbient->GetTexturePath(), p_pathMtlFolder ) + cuT( "\n" );
		}

		if( l_pDiffuse && !l_pDiffuse->GetTexturePath().empty() )
		{
			l_strReturn += cuT( "	map_Kd " ) + GetTextureNewPath( l_pDiffuse->GetTexturePath(), p_pathMtlFolder ) + cuT( "\n" );
		}

		if( l_pNormal && !l_pNormal->GetTexturePath().empty() )
		{
			l_strReturn += cuT( "	map_Bump " ) + GetTextureNewPath( l_pNormal->GetTexturePath(), p_pathMtlFolder ) + cuT( "\n" );
		}

		if( l_pOpacity && !l_pOpacity->GetTexturePath().empty() )
		{
			l_strReturn += cuT( "	map_d " ) + GetTextureNewPath( l_pOpacity->GetTexturePath(), p_pathMtlFolder ) + cuT( "\n" );
		}

		if( l_pSpecular && !l_pSpecular->GetTexturePath().empty() )
		{
			l_strReturn += cuT( "	map_Ks " ) + GetTextureNewPath( l_pSpecular->GetTexturePath(), p_pathMtlFolder ) + cuT( "\n" );
		}

		if( l_pGloss && !l_pGloss->GetTexturePath().empty() )
		{
			l_strReturn += cuT( "	map_Ns " ) + GetTextureNewPath( l_pGloss->GetTexturePath(), p_pathMtlFolder ) + cuT( "\n" );
		}
	}

	return l_strReturn;
}

String MainFrame :: DoObjExport( MeshSPtr p_pMesh, uint32_t & p_uiOffset, uint32_t & p_uiCount )
{
	String l_strReturn;

	std::for_each( p_pMesh->Begin(), p_pMesh->End(), [&]( SubmeshSPtr p_pSubmesh )
	{
		String l_strV;
		String l_strVT;
		String l_strVN;
		String l_strF;
		SubmeshRendererSPtr l_pRenderer = p_pSubmesh->GetRenderer();
		VertexBufferSPtr l_pVtxBuffer = l_pRenderer->GetVertex();
		IndexBufferSPtr l_pIdxBuffer = l_pRenderer->GetIndices();

		uint32_t l_uiStride = l_pVtxBuffer->GetDeclaration().GetStride();
		uint32_t l_uiNbPoints = l_pVtxBuffer->GetSize() / l_uiStride;
		uint32_t l_uiNbFaces = l_pIdxBuffer->GetSize() / 3;
		uint8_t * l_pVtx = l_pVtxBuffer->data();
		uint32_t * l_pIdx = l_pIdxBuffer->data();

		Point3r l_ptPos;
		Point3r l_ptNml;
		Point3r l_ptTex;

		for( uint32_t j = 0; j < l_uiNbPoints; j++ )
		{
			real * l_pVertex = reinterpret_cast< real * >( &l_pVtx[j * l_uiStride] );
			Vertex::GetPosition(	l_pVertex, l_ptPos );
			Vertex::GetNormal(		l_pVertex, l_ptNml );
			Vertex::GetTexCoord(	l_pVertex, l_ptTex );
			l_strV  << cuT(  "v " ) << l_ptPos[0] << " " << l_ptPos[1] << " " << l_ptPos[2] << cuT( "\n" );
			l_strVN << cuT( "vn " ) << l_ptNml[0] << " " << l_ptNml[1] << " " << l_ptNml[2] << cuT( "\n" );
			l_strVT << cuT( "vt " ) << l_ptTex[0] << " " << l_ptTex[1] << cuT( "\n" );
		}

		l_strF << cuT( "usemtl " ) << p_pSubmesh->GetMaterial()->GetName() << cuT( "\ns off\n" );

		for( uint32_t j = 0; j < l_uiNbFaces; j++ )
		{
			uint32_t * l_pFace = &l_pIdx[j * 3];
			uint32_t l_v0 = p_uiOffset + l_pFace[0];
			uint32_t l_v1 = p_uiOffset + l_pFace[1];
			uint32_t l_v2 = p_uiOffset + l_pFace[2];
			l_strF << cuT( "f " ) << l_v0 << cuT( "/" ) << l_v0 << cuT( "/" ) << l_v0 << cuT( " " ) << l_v1 << cuT( "/" ) << l_v1 << cuT( "/" ) << l_v1 << cuT( " " ) << l_v2 << cuT( "/" ) << l_v2 << cuT( "/" ) << l_v2 << cuT( "\n" );
		}

		l_strReturn << cuT( "g mesh" ) << p_uiCount << cuT( "\n" ) << l_strV << cuT( "\n" ) << l_strVN << cuT( "\n" ) << l_strVT << cuT( "\n" ) << l_strF << cuT( "\n" );
		p_uiOffset += l_uiNbPoints;
		p_uiCount++;
	} );

	return l_strReturn;
}

void MainFrame :: DoCscnExportScene( Path const & p_pathFile )
{
	SceneSPtr l_pScene = m_pMainScene.lock();

	if( l_pScene )
	{
		Scene::TextLoader()( *l_pScene, p_pathFile );
	}
}

void MainFrame :: DoBinaryExportScene( Path const & WXUNUSED( p_pathFile ) )
{
}

BEGIN_EVENT_TABLE( MainFrame, wxFrame)
	EVT_TIMER(	1,						MainFrame::OnTimer				)
	EVT_PAINT(							MainFrame::OnPaint				)
	EVT_INIT_DIALOG(					MainFrame::OnInit				)
	EVT_CLOSE(							MainFrame::OnClose				)
	EVT_ENTER_WINDOW(					MainFrame::OnEnterWindow		)
	EVT_LEAVE_WINDOW(					MainFrame::OnLeaveWindow		)
	EVT_ERASE_BACKGROUND(				MainFrame::OnEraseBackground	)
	EVT_KEY_UP(							MainFrame::OnKeyUp				)
	EVT_TOOL( eID_TOOL_LOAD_SCENE,		MainFrame::OnLoadScene			)
	EVT_TOOL( eID_TOOL_EXPORT_SCENE,	MainFrame::OnExportScene		)
	EVT_TOOL( eID_TOOL_GEOMETRIES,		MainFrame::OnShowGeometriesList	)
	EVT_TOOL( eID_TOOL_MATERIALS,		MainFrame::OnShowMaterialsList	)
END_EVENT_TABLE()

void MainFrame :: OnPaint( wxPaintEvent & p_event )
{
	wxPaintDC l_paintDC( this);
	p_event.Skip();
}

void MainFrame :: OnTimer( wxTimerEvent & p_event )
{
	if( m_pCastor3D )
	{
		m_pCastor3D->RenderOneFrame();
	}

	p_event.Skip();
}

void MainFrame :: OnInit( wxInitDialogEvent & p_event )
{
}

void MainFrame :: OnClose( wxCloseEvent & p_event )
{
	m_pListLog = NULL;
	Logger::SetCallback( NULL, NULL );
	Hide();

	if( m_pGeometriesFrame && m_pAuiManager )
	{
		m_pAuiManager->DetachPane( m_pGeometriesFrame );
	}

	if( m_timer )
	{
		m_timer->Stop();
		delete m_timer;
		m_timer = nullptr;
	}

	m_pMainScene.reset();

	if( m_pRenderPanel )
	{
        if( m_pAuiManager )
        {
            m_pAuiManager->DetachPane( m_pRenderPanel );
        }

		m_pRenderPanel->SetRenderWindow( nullptr );
	}

	if( m_pCastor3D )
	{
#if CASTOR3D_THREADED
		m_pCastor3D->EndRendering();
#endif
		m_pCastor3D->Cleanup();
	}

	if( m_pRenderPanel )
	{
		m_pRenderPanel->UnFocus();
		m_pRenderPanel->Close( true );
		m_pRenderPanel = NULL;
	}

	DestroyChildren();

	if( m_pImagesLoader )
	{
		m_pImagesLoader->Cleanup();
	}

	p_event.Skip();
}

void MainFrame :: OnEnterWindow( wxMouseEvent & p_event )
{
    SetFocus();
	p_event.Skip();
}

void MainFrame :: OnLeaveWindow( wxMouseEvent & p_event )
{
	p_event.Skip();
}

void MainFrame :: OnEraseBackground( wxEraseEvent & p_event )
{
	p_event.Skip();
}

void MainFrame :: OnKeyUp( wxKeyEvent & p_event )
{
	switch( p_event.GetKeyCode() )
	{
	case WXK_F5:	LoadScene();	break;
	}
}

void MainFrame :: OnLoadScene( wxCommandEvent & p_event )
{
	wxString l_wildcard = _( "Castor3D scene files (*.cscn)|*.cscn" );

	wxFileDialog l_fileDialog( this, _( "Open a scene" ), wxEmptyString, wxEmptyString, l_wildcard );

	if( l_fileDialog.ShowModal() == wxID_OK )
	{
		m_strFilePath = (wxChar const *)l_fileDialog.GetPath().c_str();
		LoadScene();
	}

	p_event.Skip();
}

void MainFrame :: OnExportScene( wxCommandEvent & p_event )
{
	wxString l_wildcard = _( "Castor3D text scene (*.cscn)|*.cscn|Castor3D binary scene (*.cbsn)|*.cbsn|Wavefront OBJ (*.obj)|*.obj" );

	wxFileDialog l_fileDialog( this, _( "Export the scene" ), wxEmptyString, wxEmptyString, l_wildcard, wxFD_SAVE | wxFD_OVERWRITE_PROMPT );

	if( l_fileDialog.ShowModal() == wxID_OK )
	{
		DoExportScene( (wxChar const *)l_fileDialog.GetPath().c_str() );
	}

	p_event.Skip();
}

void MainFrame :: OnShowGeometriesList( wxCommandEvent & p_event )
{
	if( !m_pGeometriesFrame )
	{
		m_pGeometriesFrame = new wxGeometriesListFrame( m_pCastor3D, this, _( "Geometries" ), m_pMainScene.lock(), wxDefaultPosition, wxSize( 200, 300 ) );

		if( m_pAuiManager )
		{
            m_pAuiManager->AddPane( m_pGeometriesFrame, wxAuiPaneInfo().CloseButton().Caption( _( "Geometries" ) ).Direction( wxLEFT ).Dock().Left().LeftDockable().RightDockable().Movable().PinButton() );
		}
	}

	if( !m_pGeometriesFrame->IsShown() )
	{
		m_pGeometriesFrame->Show();
	}
	else
	{
		m_pGeometriesFrame->Hide();
	}

    if( m_pAuiManager )
    {
        m_pAuiManager->Update();
    }

	p_event.Skip();
}

void MainFrame :: OnShowMaterialsList( wxCommandEvent & p_event )
{
	wxMaterialsFrame * l_pFrame = new wxMaterialsFrame( m_pCastor3D, false, this, _( "Materials" ), wxDefaultPosition );
	l_pFrame->Initialise();
	l_pFrame->Show();

	p_event.Skip();
}
