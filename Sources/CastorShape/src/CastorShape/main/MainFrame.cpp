//******************************************************************************
#include "PrecompiledHeaders.h"

#include "main/MainFrame.h"
#include "main/CastorShape.h"
#include "main/RenderPanel.h"

#include "geometry/NewConeDialog.h"
#include "geometry/NewCubeDialog.h"
#include "geometry/NewCylinderDialog.h"
#include "geometry/NewIcosaedronDialog.h"
#include "geometry/NewPlaneDialog.h"
#include "geometry/NewSphereDialog.h"
#include "geometry/NewTorusDialog.h"
#include "geometry/GeometriesListFrame.h"
#include "material/MaterialsFrame.h"
#include "material/NewMaterialDialog.h"
//******************************************************************************
#define ID_NEW_WINDOW 10000
//******************************************************************************
using namespace Castor3D;
//******************************************************************************
unsigned int g_nbGeometries;
CSMainFrame * g_mainFrame;
DECLARE_APP( CSCastorShape)
//******************************************************************************

BEGIN_EVENT_TABLE( CSMainFrame, wxFrame)
	EVT_SIZE(				CSMainFrame::_onSize)
	EVT_MOVE(				CSMainFrame::_onMove)
	EVT_CLOSE(				CSMainFrame::_onClose)
	EVT_ENTER_WINDOW(		CSMainFrame::_onEnterWindow)
	EVT_LEAVE_WINDOW(		CSMainFrame::_onLeaveWindow)
	EVT_ERASE_BACKGROUND(	CSMainFrame::_onEraseBackground)
	EVT_KEY_DOWN(			CSMainFrame::_onKeyDown)
	EVT_KEY_UP(				CSMainFrame::_onKeyUp)
	EVT_LEFT_DOWN(			CSMainFrame::_onMouseLDown)
	EVT_LEFT_UP(			CSMainFrame::_onMouseLUp)
	EVT_MIDDLE_DOWN(		CSMainFrame::_onMouseMDown)
	EVT_MIDDLE_UP(			CSMainFrame::_onMouseMUp)
	EVT_RIGHT_DOWN(			CSMainFrame::_onMouseRDown)
	EVT_RIGHT_UP(			CSMainFrame::_onMouseRUp)
	EVT_MOTION(				CSMainFrame::_onMouseMove)
	EVT_MOUSEWHEEL(			CSMainFrame::_onMouseWheel)

    EVT_MENU( wxID_EXIT,				CSMainFrame::_onMenuClose)
	EVT_MENU( mbExit,					CSMainFrame::_onMenuClose)
	EVT_MENU( mbSaveScene,				CSMainFrame::_onSaveScene)
	EVT_MENU( mLoadScene,				CSMainFrame::_onLoadScene)
	EVT_MENU( mbNewCone,				CSMainFrame::_onNewCone)
	EVT_MENU( mbNewCube,				CSMainFrame::_onNewCube)
	EVT_MENU( mbNewCylinder,			CSMainFrame::_onNewCylinder)
	EVT_MENU( mbNewIcosaedron,			CSMainFrame::_onNewIcosaedron)
	EVT_MENU( mbNewPlane,				CSMainFrame::_onNewPlane)
	EVT_MENU( mbNewSphere,				CSMainFrame::_onNewSphere)
	EVT_MENU( mbNewTorus,				CSMainFrame::_onNewTorus)
	EVT_MENU( mbNewProjection,			CSMainFrame::_onNewProjection)
	EVT_MENU( mbNewMaterial,			CSMainFrame::_onNewMaterial)
	EVT_MENU( mbGeometries,				CSMainFrame::_onShowGeometriesList)
	EVT_MENU( mbMaterials,				CSMainFrame::_onShowMaterialsList)
	EVT_MENU( mbSelectGeometries,		CSMainFrame::_onSelectGeometries)
	EVT_MENU( mbSelectPoints,			CSMainFrame::_onSelectPoints)
	EVT_MENU( mbCloneSelection,			CSMainFrame::_onCloneSelection)
	EVT_MENU( mbSelectNone,				CSMainFrame::_onSelectNone)
	EVT_MENU( mbSubdividePNTriangles,	CSMainFrame::_onSubdivideAllPNTriangles)
	EVT_MENU( mbSubdivideLoop,			CSMainFrame::_onSubdivideAllLoop)
	EVT_MENU( mbSelect,					CSMainFrame::_onSelect)
	EVT_MENU( mbModify,					CSMainFrame::_onModify)
	EVT_MENU( mbNone,					CSMainFrame::_onNothing)
	EVT_MENU( mbRender,					CSMainFrame::_onRender)

END_EVENT_TABLE()

//******************************************************************************

CSMainFrame :: CSMainFrame( wxWindow * parent, const wxString & title, 
						    const wxPoint & pos, const wxSize & size,
						    long style, wxString name)
	:	wxFrame( parent, mainFrame, title, pos, size, style, name),
		m_3dFrame			( NULL),
		m_2dFrameHD			( NULL),
		m_2dFrameBG			( NULL),
		m_2dFrameBD			( NULL),
		m_castor3D			( NULL),
		m_selectedGeometry	( NULL),
		m_bWireFrame		( false)
{
	m_selectedMaterial.m_ambient[0] = 0.2f;
	m_selectedMaterial.m_ambient[1] = 0.0f;
	m_selectedMaterial.m_ambient[2] = 0.0f;
	m_selectedMaterial.m_emissive[0] = 0.2f;
	m_selectedMaterial.m_emissive[1] = 0.0f;
	m_selectedMaterial.m_emissive[2] = 0.0f;
	g_mainFrame = this;

	m_log = new Log( C3D_T( "Castor.log"));

	_buildMenuBar();
	_initialise3D();
	 wxInitAllImageHandlers();
}

//******************************************************************************

CSMainFrame :: ~CSMainFrame()
{
}

//******************************************************************************

void CSMainFrame :: SelectGeometry( Geometry * p_geometry)
{
	if (p_geometry == m_selectedGeometry)
	{
		return;
	}

	if (m_selectedGeometry != NULL)
	{
		Mesh * l_mesh = m_selectedGeometry->GetMesh();
		MaterialInfos * l_infos;
		for (size_t i = 0 ; i < l_mesh->GetNbSubmeshes() ; i++)
		{
			l_infos = m_selectedGeometryMaterials[i];
			l_mesh->GetSubmesh( i)->GetMaterial()->GetPass( 0)->SetAmbient( l_infos->m_ambient);
			l_mesh->GetSubmesh( i)->GetMaterial()->GetPass( 0)->SetEmissive( l_infos->m_emissive);
		}
		vector::deleteAll( m_selectedGeometryMaterials);
	}

	m_selectedGeometry = p_geometry;

	if (m_selectedGeometry != NULL)
	{
		Mesh * l_mesh = m_selectedGeometry->GetMesh();
		MaterialInfos * l_infos;
		for (size_t i = 0 ; i < l_mesh->GetNbSubmeshes() ; i++)
		{
			l_infos = new MaterialInfos;
			l_infos->m_ambient[0] = l_mesh->GetSubmesh( i)->GetMaterial()->GetPass( 0)->GetAmbient()[0];
			l_infos->m_ambient[1] = l_mesh->GetSubmesh( i)->GetMaterial()->GetPass( 0)->GetAmbient()[1];
			l_infos->m_ambient[2] = l_mesh->GetSubmesh( i)->GetMaterial()->GetPass( 0)->GetAmbient()[2];
			l_infos->m_emissive[0] = l_mesh->GetSubmesh( i)->GetMaterial()->GetPass( 0)->GetEmissive()[0];
			l_infos->m_emissive[1] = l_mesh->GetSubmesh( i)->GetMaterial()->GetPass( 0)->GetEmissive()[1];
			l_infos->m_emissive[2] = l_mesh->GetSubmesh( i)->GetMaterial()->GetPass( 0)->GetEmissive()[2];
			m_selectedGeometryMaterials.push_back( l_infos);
			l_mesh->GetSubmesh( i)->GetMaterial()->GetPass( 0)->SetAmbient( m_selectedMaterial.m_ambient);
			l_mesh->GetSubmesh( i)->GetMaterial()->GetPass( 0)->SetEmissive( m_selectedMaterial.m_emissive);
		}
	}
}

//******************************************************************************

void CSMainFrame :: SelectVertex( Vector3f * p_vertex)
{
}

//******************************************************************************

void CSMainFrame :: _onPaint( wxPaintEvent & WXUNUSED(event))
{
	wxPaintDC l_dc( this);
	Root::GetSingletonPtr()->RenderOneFrame();
}

//******************************************************************************

void CSMainFrame :: _onSize( wxSizeEvent & event)
{
	wxClientDC l_dc( this);

	int l_width = GetClientSize().x / 2;
	int l_height = GetClientSize().y / 2;

	if (m_3dFrame != NULL)
	{
		m_3dFrame->SetSize( l_width - 1, l_height - 1);
		m_3dFrame->SetPosition( wxPoint( 0, 0));
		m_2dFrameHD->SetSize( l_width - 1, l_height - 1);
		m_2dFrameHD->SetPosition( wxPoint( l_width + 1, 0));
		m_2dFrameBG->SetSize( l_width - 1, l_height - 1);
		m_2dFrameBG->SetPosition( wxPoint( 0, l_height + 1));
		m_2dFrameBD->SetSize( l_width - 1, l_height - 1);
		m_2dFrameBD->SetPosition( wxPoint( l_width + 1, l_height + 1));

		m_separator1->SetSize( l_width, 2);
		m_separator1->SetPosition( wxPoint( 0, l_height - 1));
		m_separator2->SetSize( l_width, 2);
		m_separator2->SetPosition( wxPoint( l_width, l_height - 1));
		m_separator3->SetSize( 2, l_height);
		m_separator3->SetPosition( wxPoint( l_width - 1, 0));
		m_separator4->SetSize( 2, l_height);
		m_separator4->SetPosition( wxPoint( l_width - 1, l_height));

	}

	ShowPanels();
}

//******************************************************************************

void CSMainFrame :: _onMove( wxMoveEvent & event)
{
	wxClientDC l_dc( this);
	ShowPanels();
}

//******************************************************************************

void CSMainFrame :: _onClose( wxCloseEvent & event)
{
	vector::deleteAll( m_selectedGeometryMaterials);

	if (m_castor3D != NULL)
	{
		m_castor3D->EndRendering();
	}

	if (m_3dFrame != NULL)
	{
		m_3dFrame->UnFocus();
		m_3dFrame->Close( true);
		m_3dFrame = NULL;

		m_2dFrameHD->UnFocus();
		m_2dFrameHD->Close( true);
		m_2dFrameHD = NULL;

		m_2dFrameBG->UnFocus();
		m_2dFrameBG->Close( true);
		m_2dFrameBG = NULL;

		m_2dFrameBD->UnFocus();
		m_2dFrameBD->Close( true);
		m_2dFrameBD = NULL;
	}

	if (m_castor3D != NULL)
	{
		delete m_castor3D;
	}

	delete m_log;

	Destroy();
}

//******************************************************************************

void CSMainFrame :: _onEnterWindow( wxMouseEvent & WXUNUSED(event))
{
    SetFocus();
}

//******************************************************************************

void CSMainFrame :: _onLeaveWindow( wxMouseEvent & WXUNUSED(event))
{
}

//******************************************************************************

void CSMainFrame :: _onEraseBackground(wxEraseEvent& event)
{
}

//******************************************************************************

void CSMainFrame :: _onKeyDown(wxKeyEvent& event)
{
}

//******************************************************************************

void CSMainFrame :: _onKeyUp(wxKeyEvent& event)
{
}

//******************************************************************************

void CSMainFrame :: _onMouseLDown( wxMouseEvent & event)
{
}

//******************************************************************************

void CSMainFrame :: _onMouseLUp( wxMouseEvent & event)
{
}

//******************************************************************************

void CSMainFrame :: _onMouseMDown( wxMouseEvent & event)
{
}

//******************************************************************************

void CSMainFrame :: _onMouseMUp( wxMouseEvent & event)
{
}

//******************************************************************************

void CSMainFrame :: _onMouseRDown( wxMouseEvent & event)
{
}

//******************************************************************************

void CSMainFrame :: _onMouseRUp( wxMouseEvent & event)
{
}

//******************************************************************************

void CSMainFrame :: _onMouseMove( wxMouseEvent & event)
{
}

//******************************************************************************

void CSMainFrame :: _onMouseWheel( wxMouseEvent & event)
{
}

//******************************************************************************

void CSMainFrame :: _onMenuClose( wxCommandEvent & event)
{
	Close( true);
}

//******************************************************************************

void CSMainFrame :: _onSaveScene( wxCommandEvent & event)
{
	wxFileDialog * l_fileDialog = new wxFileDialog( this, C3D_T( "Enregistrer une scene"), wxEmptyString, wxEmptyString, C3D_T( "Castor Shape files (*.escn)|*.escn"));
	if (l_fileDialog->ShowModal() == wxID_OK)
	{
		FileIO l_file( l_fileDialog->GetPath().c_str(), FileIO::eWrite);
		wxString l_filePath = l_fileDialog->GetPath();
		l_filePath.Replace( C3D_T( "\\"), C3D_T( "/"));
		if (MaterialManager::GetSingletonPtr()->Write( l_filePath.c_str()))
		{
			Log::LogMessage( C3D_T( "Materials written"));
		}
		else
		{
			Log::LogMessage( C3D_T( "Can't write materials"));
			return;
		}
		if (MeshManager::GetSingletonPtr()->Write( l_filePath.c_str()))
		{
			Log::LogMessage( C3D_T( "Meshes written"));
		}
		else
		{
			Log::LogMessage( C3D_T( "Can't write meshes"));
			return;
		}
		if (SceneManager::GetSingletonPtr()->GetElementByName( C3D_T( "MainScene"))->Write( & l_file))
		{
			Log::LogMessage( C3D_T( "Save Successfull"));
		}
		else
		{
			Log::LogMessage( C3D_T( "Save Failed"));
		}
	}
}

//******************************************************************************

void CSMainFrame :: _onLoadScene( wxCommandEvent & event)
{
	wxString l_wildcard = C3D_T( "Castor Shape files (*.escn)|*.escn|3DS files (*.3ds)|*.3ds|ASCII Scene Export files (*.ase)|*.ase|Quake 2 Model files (*.md2)|*.md2|Quake 3 Model files (*.md3)|*.md3|Obj files (*.obj)|*.obj|PLY files (*.ply)|*.ply");
	wxFileDialog * l_fileDialog = new wxFileDialog( this, C3D_T( "Ouvrir une scene"), wxEmptyString, wxEmptyString, l_wildcard);

	if (l_fileDialog->ShowModal() == wxID_OK)
	{
		if (l_fileDialog->GetPath().find( C3D_T( ".3ds")) != String::npos || l_fileDialog->GetPath().find( C3D_T( ".3DS")) != String::npos)
		{
			SceneManager::GetSingletonPtr()->GetElementByName( C3D_T( "MainScene"))->Import3DS( l_fileDialog->GetPath().c_str());
		}
		else if (l_fileDialog->GetPath().find( C3D_T( ".ase")) != String::npos || l_fileDialog->GetPath().find( C3D_T( ".ASE")) != String::npos)
		{
			SceneManager::GetSingletonPtr()->GetElementByName( C3D_T( "MainScene"))->ImportASE( l_fileDialog->GetPath().c_str());
		}
		else if (l_fileDialog->GetPath().find( C3D_T( ".bsp")) != String::npos || l_fileDialog->GetPath().find( C3D_T( ".BSP")) != String::npos)
		{
			SceneManager::GetSingletonPtr()->GetElementByName( C3D_T( "MainScene"))->ImportBSP( l_fileDialog->GetPath().c_str());
		}
		else if (l_fileDialog->GetPath().find( C3D_T( ".ply")) != String::npos || l_fileDialog->GetPath().find( C3D_T( ".PLY")) != String::npos)
		{
			SceneManager::GetSingletonPtr()->GetElementByName( C3D_T( "MainScene"))->ImportPLY( l_fileDialog->GetPath().c_str());
		}
		else if (l_fileDialog->GetPath().find( C3D_T( ".obj")) != String::npos || l_fileDialog->GetPath().find( C3D_T( ".OBJ")) != String::npos)
		{
			SceneManager::GetSingletonPtr()->GetElementByName( C3D_T( "MainScene"))->ImportObj( l_fileDialog->GetPath().c_str());
		}
		else if (l_fileDialog->GetPath().find( C3D_T( ".md2")) != String::npos || l_fileDialog->GetPath().find( C3D_T( ".MD2")) != String::npos)
		{
			wxFileDialog l_dialog( this, C3D_T( "Ouvrir une image"), wxEmptyString, wxEmptyString, C3D_T( "Fichiers BMP (*.bmp)|*.bmp|Fichiers GIF (*.gif)|*.gif\
																										  |Fichiers JPG (*.jpg)|*.jpg|Fichiers PNG (*.png)|*.png\
																										  |Images (*.bmp;*.gif;*.png;*.jpg)|*.bmp;*.gif;*.png;*.jpg"));
			if (l_dialog.ShowModal() == wxID_OK)
			{
				SceneManager::GetSingletonPtr()->GetElementByName( C3D_T( "MainScene"))->ImportMD2( l_fileDialog->GetPath().c_str(), l_dialog.GetPath().c_str());
			}
		}
		else if (l_fileDialog->GetPath().find( C3D_T( ".md3")) != String::npos || l_fileDialog->GetPath().find( C3D_T( ".MD3")) != String::npos)
		{
			SceneManager::GetSingletonPtr()->GetElementByName( C3D_T( "MainScene"))->ImportMD3( l_fileDialog->GetPath().c_str());
		}
		else
		{
			m_mainScene->ClearScene();
			Log::LogMessage( C3D_T( "Scene cleared"));
			MeshManager::GetSingletonPtr()->Clear();
			Log::LogMessage( C3D_T( "Mesh manager cleared"));
			MaterialManager::GetSingletonPtr()->Clear();
			Log::LogMessage( C3D_T( "Material manager cleared"));
			Log::LogMessage( l_fileDialog->GetPath().c_str());

			wxString l_filePath = l_fileDialog->GetPath();

			if (MaterialManager::GetSingletonPtr()->Read( l_filePath.c_str()))
			{
				Log::LogMessage( C3D_T( "Materials read"));
			}
			else
			{
				Log::LogMessage( C3D_T( "Can't read materials"));
				return;
			}

			SceneFileParser l_parser;
			l_parser.ParseFile( l_filePath.c_str());
		}
	}
	ShowPanels();
}

void CSMainFrame :: _onNewCone( wxCommandEvent & event)
{
	CSNewConeDialog * l_dialog = new CSNewConeDialog( this, mfNewGeometry);

	if (l_dialog->ShowModal() == wxID_OK)
	{
		Scene * l_mainScene = SceneManager::GetSingletonPtr()->GetElementByName( C3D_T( "MainScene"));

		if (l_mainScene != NULL)
		{
			float l_radius = l_dialog->GetConeRadius();
			float l_height = l_dialog->GetConeHeight();
			int l_nbFaces = l_dialog->GetFacesNumber();

			if (l_radius != 0.0 && l_height != 0.0  && l_nbFaces >= 1)
			{
				_createGeometry( Mesh::eCone, l_dialog->GetGeometryName(), l_dialog->GetFacesNumberStr(),
								 C3D_T( "Cone"), l_mainScene, l_dialog, l_nbFaces, 0, l_height, l_radius);
			}
		}

		ShowPanels();
	}

	l_dialog->Destroy();
}

void CSMainFrame :: _onNewCube( wxCommandEvent & event)
{
	CSNewCubeDialog * l_dialog = new CSNewCubeDialog( this, mfNewGeometry);

	if (l_dialog->ShowModal() == wxID_OK)
	{
		Scene * l_mainScene = SceneManager::GetSingletonPtr()->GetElementByName( C3D_T( "MainScene"));

		if (l_mainScene != NULL)
		{
			float l_width = l_dialog->GetCubeWidth();
			float l_height = l_dialog->GetCubeHeight();
			float l_depth = l_dialog->GetCubeDepth();

			if (l_width != 0.0 && l_height != 0.0 && l_depth != 0.0)
			{
				_createGeometry( Mesh::eCube, l_dialog->GetGeometryName(), C3DEmptyString,
								 C3D_T( "Cube"), l_mainScene, l_dialog, 0, 0, l_width, l_height, l_depth);
			}
		}

		ShowPanels();
	}

	l_dialog->Destroy();
}

void CSMainFrame :: _onNewCylinder( wxCommandEvent & event)
{
	CSNewCylinderDialog * l_dialog = new CSNewCylinderDialog( this, mfNewGeometry);

	if (l_dialog->ShowModal() == wxID_OK)
	{
		Scene * l_mainScene = SceneManager::GetSingletonPtr()->GetElementByName( C3D_T( "MainScene"));

		if (l_mainScene != NULL)
		{
			float l_radius = l_dialog->GetCylinderRadius();
			float l_height = l_dialog->GetCylinderHeight();
			int l_nbFaces = l_dialog->GetFacesNumber();

			if (l_radius != 0.0 && l_height != 0.0 && l_nbFaces >= 1)
			{
				_createGeometry( Mesh::eCylinder, l_dialog->GetGeometryName(), l_dialog->GetFacesNumberStr(),
								 C3D_T( "Cylinder"), l_mainScene, l_dialog, l_nbFaces, 0, l_height, l_radius);
			}
		}

		ShowPanels();
	}

	l_dialog->Destroy();
}

void CSMainFrame :: _onNewIcosaedron( wxCommandEvent & event)
{
	CSNewIcosaedronDialog * l_dialog = new CSNewIcosaedronDialog( this, mfNewGeometry);

	if (l_dialog->ShowModal() == wxID_OK)
	{
		Scene * l_mainScene = SceneManager::GetSingletonPtr()->GetElementByName( C3D_T( "MainScene"));

		if (l_mainScene != NULL)
		{
			float l_radius = l_dialog->GetIcosaedronRadius();
			int l_nbFaces = l_dialog->GetNbSubdiv();

			if (l_radius != 0.0 && l_nbFaces >= 1)
			{
				_createGeometry( Mesh::eIcosaedron, l_dialog->GetGeometryName(), l_dialog->GetNbSubdivStr(),
								 C3D_T( "Icosaedron"), l_mainScene, l_dialog, l_nbFaces, 0, l_radius);
			}
		}

		ShowPanels();
	}

	l_dialog->Destroy();
}

void CSMainFrame :: _onNewPlane( wxCommandEvent & event)
{
	CSNewPlaneDialog * l_dialog = new CSNewPlaneDialog( this, mfNewGeometry);

	if (l_dialog->ShowModal() == wxID_OK)
	{
		Scene * l_mainScene = SceneManager::GetSingletonPtr()->GetElementByName( C3D_T( "MainScene"));

		if (l_mainScene != NULL)
		{
			float l_width = l_dialog->GetGeometryWidth();
			float l_depth = l_dialog->GetGeometryDepth();
			int l_nbWidthSubdiv = l_dialog->GetNbWidthSubdiv();
			int l_nbDepthSubdiv = l_dialog->GetNbDepthSubdiv();

			if (l_width != 0.0 && l_depth != 0.0 && l_nbWidthSubdiv >= 0 && l_nbDepthSubdiv >= 0)
			{
				_createGeometry( Mesh::ePlane, l_dialog->GetGeometryName(), l_dialog->GetNbDepthSubdivStr() + C3D_T( "x") + l_dialog->GetNbWidthSubdivStr(),
								 C3D_T( "Plane"), l_mainScene, l_dialog, l_nbDepthSubdiv, l_nbWidthSubdiv, l_width, l_depth);
			}
		}

		ShowPanels();
	}

	l_dialog->Destroy();
}

void CSMainFrame :: _onNewSphere( wxCommandEvent & event)
{
	CSNewSphereDialog * l_dialog = new CSNewSphereDialog( this, mfNewGeometry);

	if (l_dialog->ShowModal() == wxID_OK)
	{
		Scene * l_mainScene = SceneManager::GetSingletonPtr()->GetElementByName( C3D_T( "MainScene"));

		if (l_mainScene != NULL)
		{
			float l_radius = l_dialog->GetSphereRadius();
			int l_nbFaces = l_dialog->GetFacesNumber();

			if (l_radius != 0.0 && l_nbFaces >= 3)
			{
				_createGeometry( Mesh::eSphere, l_dialog->GetGeometryName(), l_dialog->GetFacesNumberStr(),
								 C3D_T( "Sphere"), l_mainScene, l_dialog, l_nbFaces, 0, l_radius);
			}
		}

		ShowPanels();
	}

	l_dialog->Destroy();
}

void CSMainFrame :: _onNewTorus( wxCommandEvent & event)
{
	CSNewTorusDialog * l_dialog = new CSNewTorusDialog( this, mfNewGeometry);

	if (l_dialog->ShowModal() == wxID_OK)
	{
		Scene * l_mainScene = SceneManager::GetSingletonPtr()->GetElementByName( C3D_T( "MainScene"));

		if (l_mainScene != NULL)
		{
			float l_width = l_dialog->GetInternalRadius();
			float l_radius = l_dialog->GetExternalRadius();
			int l_nbRadiusSubdiv = l_dialog->GetExternalNbFaces();
			int l_nbWidthSubdiv = l_dialog->GetInternalNbFaces();

			if (l_width != 0.0 && l_radius != 0.0 && l_nbRadiusSubdiv >= 1 && l_nbWidthSubdiv >= 1)
			{
				_createGeometry( Mesh::eTorus, l_dialog->GetGeometryName(), l_dialog->GetExternalNbFacesStr() + C3D_T( "x") + l_dialog->GetInternalNbFacesStr(),
								 C3D_T( "Torus"), l_mainScene, l_dialog, l_nbWidthSubdiv, l_nbRadiusSubdiv, l_width, l_radius);
			}
		}

		ShowPanels();
	}

	l_dialog->Destroy();
}

void CSMainFrame :: _onNewProjection( wxCommandEvent & event)
{
	CSNewSphereDialog * l_dialog = new CSNewSphereDialog( this, mfNewGeometry);

	if (l_dialog->ShowModal() == wxID_OK)
	{
		Scene * l_mainScene = SceneManager::GetSingletonPtr()->GetElementByName( C3D_T( "MainScene"));

		if (l_mainScene != NULL)
		{
			float l_fDepth = l_dialog->GetSphereRadius();
			int l_nbFaces = l_dialog->GetFacesNumber();

			if (l_fDepth != 0.0 && l_nbFaces >= 1)
			{
				_createGeometry( Mesh::eProjection, l_dialog->GetGeometryName(), l_dialog->GetFacesNumberStr(),
								 C3D_T( "Projection"), l_mainScene, l_dialog, l_nbFaces, 0, l_fDepth, 0.0);
			}
		}

		ShowPanels();
	}

	l_dialog->Destroy();
}

void CSMainFrame :: _onNewMaterial( wxCommandEvent & event)
{
	CSNewMaterialDialog * l_dialog = new CSNewMaterialDialog( this, mfNewMaterial);
	if (l_dialog->ShowModal() == wxID_OK)
	{
		Log::LogMessage( C3D_T( "Material Created"));
	}
	l_dialog->Destroy();
}

void CSMainFrame :: _onShowGeometriesList( wxCommandEvent & event)
{
	Scene * l_scene = SceneManager::GetSingletonPtr()->GetElementByName( C3D_T( "MainScene"));
	CSGeometriesListFrame * l_listFrame = new CSGeometriesListFrame( this,
																	 C3D_T( "Geometries"),
																	 l_scene,
																	 wxDefaultPosition,
																	 wxSize( 200, 300));
	l_listFrame->Show();
}

void CSMainFrame :: _onShowMaterialsList( wxCommandEvent & event)
{
	CSMaterialsFrame * l_listFrame = new CSMaterialsFrame( this, C3D_T( "Materiaux"),
														   wxDefaultPosition);
	l_listFrame->Show();
}

void CSMainFrame :: _onSelectGeometries( wxCommandEvent & event)
{
	SelectGeometry( NULL);
	m_3dFrame->SetSelectionType( stGeometry);

	m_2dFrameHD->SetSelectionType( stGeometry);
	m_2dFrameBG->SetSelectionType( stGeometry);
	m_2dFrameBD->SetSelectionType( stGeometry);

}

void CSMainFrame :: _onSelectPoints( wxCommandEvent & event)
{
	SelectGeometry( NULL);
	m_3dFrame->SetSelectionType( stVertex);

	m_2dFrameHD->SetSelectionType( stVertex);
	m_2dFrameBG->SetSelectionType( stVertex);
	m_2dFrameBD->SetSelectionType( stVertex);

}

void CSMainFrame :: _onCloneSelection( wxCommandEvent & event)
{
	m_3dFrame->SetSelectionType( stClone);

	m_2dFrameHD->SetSelectionType( stClone);
	m_2dFrameBG->SetSelectionType( stClone);
	m_2dFrameBD->SetSelectionType( stClone);

}

void CSMainFrame :: _onSelectNone( wxCommandEvent & event)
{
	SelectGeometry( NULL);
	m_3dFrame->SetSelectionType( stNone);

	m_2dFrameHD->SetSelectionType( stNone);
	m_2dFrameBG->SetSelectionType( stNone);
	m_2dFrameBD->SetSelectionType( stNone);

}

void CSMainFrame :: _onSelectAll( wxCommandEvent & event)
{
	m_3dFrame->SetSelectionType( stAll);

	m_2dFrameHD->SetSelectionType( stAll);
	m_2dFrameBG->SetSelectionType( stAll);
	m_2dFrameBD->SetSelectionType( stAll);

}

void CSMainFrame :: _onSelect( wxCommandEvent & event)
{
	GetMenuBar()->FindItem( mbModify)->Check( false);
	if (event.IsChecked())
	{
		GetMenuBar()->FindItem( mbSelectGeometries)->Enable( true);
		GetMenuBar()->FindItem( mbSelectPoints)->Enable( true);

		m_3dFrame->SetActionType( atSelect);

		m_2dFrameHD->SetActionType( atSelect);
		m_2dFrameBG->SetActionType( atSelect);
		m_2dFrameBD->SetActionType( atSelect);

		SelectGeometry( NULL);
		if (GetMenuBar()->FindItem( mbSelectGeometries)->IsChecked())
		{
			m_3dFrame->SetSelectionType( stGeometry);

			m_2dFrameHD->SetSelectionType( stGeometry);
			m_2dFrameBG->SetSelectionType( stGeometry);
			m_2dFrameBD->SetSelectionType( stGeometry);
		}
		else
		{
			m_3dFrame->SetSelectionType( stVertex);

			m_2dFrameHD->SetSelectionType( stVertex);
			m_2dFrameBG->SetSelectionType( stVertex);
			m_2dFrameBD->SetSelectionType( stVertex);
		}
	}
	else
	{
		m_3dFrame->SetActionType( atNone);

		m_2dFrameHD->SetActionType( atNone);
		m_2dFrameBG->SetActionType( atNone);
		m_2dFrameBD->SetActionType( atNone);
	}
}

void CSMainFrame :: _onModify( wxCommandEvent & event)
{
	GetMenuBar()->FindItem( mbSelect)->Check( false);
	GetMenuBar()->FindItem( mbSelectGeometries)->Enable( false);
	GetMenuBar()->FindItem( mbSelectPoints)->Enable( false);
	if (event.IsChecked())
	{
		m_3dFrame->SetActionType( atModify);

		m_2dFrameHD->SetActionType( atModify);
		m_2dFrameBG->SetActionType( atModify);
		m_2dFrameBD->SetActionType( atModify);
	}
	else
	{
		m_3dFrame->SetActionType( atNone);

		m_2dFrameHD->SetActionType( atNone);
		m_2dFrameBG->SetActionType( atNone);
		m_2dFrameBD->SetActionType( atNone);
	}
}

void CSMainFrame :: _onNothing( wxCommandEvent & event)
{
	GetMenuBar()->FindItem( mbSelect)->Check( false);
	GetMenuBar()->FindItem( mbModify)->Check( false);
	GetMenuBar()->FindItem( mbSelectGeometries)->Enable( false);
	GetMenuBar()->FindItem( mbSelectPoints)->Enable( false);

	SelectGeometry( NULL);

	m_3dFrame->SetActionType( atNone);

	m_2dFrameHD->SetActionType( atNone);
	m_2dFrameBG->SetActionType( atNone);
	m_2dFrameBD->SetActionType( atNone);

}
void CSMainFrame :: _onSubdivideAllPNTriangles( wxCommandEvent & event)
{
	if (m_selectedGeometry != NULL)
	{
		size_t l_nbSubmeshes = m_selectedGeometry->GetMesh()->GetNbSubmeshes();
		for (size_t i = 0 ; i < l_nbSubmeshes ; i++)
		{
			m_selectedGeometry->Subdivide( i, SMPNTriangles);
		}
	}
}

void CSMainFrame :: _onSubdivideAllLoop( wxCommandEvent & event)
{
	if (m_selectedGeometry != NULL)
	{
		size_t l_nbSubmeshes = m_selectedGeometry->GetMesh()->GetNbSubmeshes();
		for (size_t i = 0 ; i < l_nbSubmeshes ; i++)
		{
			m_selectedGeometry->Subdivide( i, SMLoop);
		}
	}
}

void CSMainFrame :: _onRender( wxCommandEvent & event)
{
	RenderEngine renderEngine( C3D_T( "Scene.tga"), SceneManager::GetSingleton().GetElementByName( C3D_T( "MainScene")));
	renderEngine.Draw();
}

void CSMainFrame :: ShowPanels()
{
	if (m_3dFrame != NULL)
	{
		m_3dFrame->DrawOneFrame();

		m_2dFrameHD->DrawOneFrame();
		m_2dFrameBG->DrawOneFrame();
		m_2dFrameBD->DrawOneFrame();

	}
}

void CSMainFrame :: _initialise3D()
{
	Log::LogMessage( C3D_T( "Initialising Castor3D"));

	m_castor3D = new Root( 25);
	try
	{
#ifdef _DEBUG
#	if C3D_UNICODE
		m_castor3D->LoadPlugin( C3D_T( "GLRenderSystemdu.dll"));
#	else
		m_castor3D->LoadPlugin( C3D_T( "GLRenderSystemd.dll"));
#	endif
#else
#	if C3D_UNICODE
		m_castor3D->LoadPlugin( C3D_T( "GLRenderSystemu.dll"));
#	else
		m_castor3D->LoadPlugin( C3D_T( "GLRenderSystem.dll"));
#	endif
#endif

		RendererDriver * l_driver = m_castor3D->GetRendererServer().GetDriver( 0);

		if (l_driver != NULL)
		{
			l_driver->CreateRenderSystem();
		}
		else
		{
			return;
		}
		

		m_mainScene = SceneManager::GetSingletonPtr()->CreateElement( C3D_T( "MainScene"));

		Log::LogMessage( C3D_T( "Castor3D Initialised"));
		int l_width = GetClientSize().x / 2;
		int l_height = GetClientSize().y / 2;
		m_3dFrame = new CSRenderPanel( this, frame3d, pt3DView, m_mainScene,
									   wxPoint( 0, 0),
									   wxSize( l_width - 1, l_height - 1));
		m_2dFrameHD = new CSRenderPanel( this, frame2d, pt2DView, m_mainScene,
										 wxPoint( l_width + 1, 0),
										 wxSize( l_width - 1, l_height - 1));
		m_2dFrameBG = new CSRenderPanel( this, frame2d, pt2DView, m_mainScene,
										 wxPoint( 0, l_height + 1),
										 wxSize( l_width - 1, l_height - 1), pdLookToLeft);
		m_2dFrameBD = new CSRenderPanel( this, frame2d, pt2DView, m_mainScene,
										 wxPoint( l_width + 1, l_height + 1),
										 wxSize( l_width - 1, l_height - 1), pdLookToTop);

		m_separator1 = new wxPanel( this, wxID_ANY, wxPoint( 0, l_height - 1),
									wxSize( l_width, 2));
		m_separator2 = new wxPanel( this, wxID_ANY, wxPoint( l_width, l_height - 1),
									wxSize( l_width, 2));
		m_separator3 = new wxPanel( this, wxID_ANY, wxPoint( l_width - 1, 0),
									wxSize( 2, l_height));
		m_separator4 = new wxPanel( this, wxID_ANY, wxPoint( l_width - 1, l_height),
									wxSize( 2, l_height));

		m_castor3D->StartRendering();
		m_3dFrame->Show();
		m_2dFrameHD->Show();
		m_2dFrameBG->Show();
		m_2dFrameBD->Show();

		ShowPanels();

		DirectionalLight * l_light1 = reinterpret_cast <DirectionalLight *>( SceneManager::GetSingletonPtr()->GetElementByName( "MainScene")->CreateLight( Light::eDirectional, "Light1"));
		if (l_light1 != NULL)
		{
			l_light1->SetPosition( 0.0f, 0.0f, 1.0f);
			l_light1->SetDiffuse( 1.0f, 1.0f, 1.0f);
			l_light1->SetSpecular( 1.0f, 1.0f, 1.0f);
			l_light1->SetEnabled( true);
		}

		DirectionalLight * l_light2 = reinterpret_cast <DirectionalLight *>( SceneManager::GetSingletonPtr()->GetElementByName( "MainScene")->CreateLight( Light::eDirectional, "Light2"));
		if (l_light2 != NULL)
		{
			l_light2->SetPosition( 0.0f, -1.0f, 1.0f);
			l_light2->SetDiffuse( 1.0f, 1.0f, 1.0f);
			l_light2->SetSpecular( 1.0f, 1.0f, 1.0f);
			l_light2->SetEnabled( true);
		}

		DirectionalLight * l_light3 = reinterpret_cast <DirectionalLight *>( SceneManager::GetSingletonPtr()->GetElementByName( "MainScene")->CreateLight( Light::eDirectional, "Light3"));
		if (l_light3 != NULL)
		{
			l_light3->SetPosition( -1.0f, -1.0f, -1.0f);
			l_light3->SetDiffuse( 1.0f, 1.0f, 1.0f);
			l_light3->SetSpecular( 1.0f, 1.0f, 1.0f);
			l_light3->SetEnabled( true);
		}

/*
		Material * l_pMaterial = MaterialManager::GetSingleton().CreateMaterial( "Overlay");
		PanelOverlay * l_pOverlay = OverlayManager::GetSingleton().CreateOverlay<PanelOverlay>( "FirstOverlay", NULL, Point2D<float>( 0.25f, 0.25f), Point2D<float>( 0.5f, 0.5f));
		l_pOverlay->SetMaterial( l_pMaterial);
		Material * l_pMaterial2 = MaterialManager::GetSingleton().CreateMaterial( "Overlay2");
		OverlayManager::GetSingleton().CreateOverlay<PanelOverlay>( "SecondOverlay", l_pOverlay, Point2D<float>( 0.25f, 0.25f), Point2D<float>( 0.5f, 0.5f))->SetMaterial( l_pMaterial2);
*/
	}
	catch ( ... )
	{
	}
}

void CSMainFrame :: _buildMenuBar()
{
	wxMenu * l_menu;
	wxMenu * l_subMenu;
	wxMenuBar * l_menuBar = new wxMenuBar();

	l_menu = new wxMenu();
	l_menu->Append( mbSaveScene,					C3D_T( "&Sauver la Scene\tCTRL+F+S"));
	l_menu->Append( mLoadScene,						C3D_T( "&Ouvrir une Scene\tCTRL+F+O"));
	l_menu->Append( mbRender,						C3D_T( "&Rendu de scène\tCTRL+F+R"));
	l_menu->AppendSeparator();
	l_menu->Append( mbExit,							C3D_T( "&Quitter\tALT+F4"));
	l_menuBar->Append( l_menu,						C3D_T( "&Fichier"));
	
	l_menu = new wxMenu();
	l_subMenu = new wxMenu();
	l_subMenu->Append( mbNewCone,					C3D_T( "C&one"));
	l_subMenu->Append( mbNewCube,					C3D_T( "C&ube"));
	l_subMenu->Append( mbNewCylinder,				C3D_T( "C&ylindre"));
	l_subMenu->Append( mbNewIcosaedron,				C3D_T( "&Icosaedre"));
	l_subMenu->Append( mbNewPlane,					C3D_T( "&Plan"));
	l_subMenu->Append( mbNewSphere,					C3D_T( "&Sphere"));
	l_subMenu->Append( mbNewTorus,					C3D_T( "&Torre"));
	l_subMenu->Append( mbNewProjection,				C3D_T( "&Projection"));	
	l_menu->AppendSubMenu( l_subMenu,				C3D_T( "Nouvelle &Geometrie\tCTRL+N+G"));
	l_menu->Append( mbNewMaterial,					C3D_T( "&Material\tCTRL+N+M"));
	l_menuBar->Append( l_menu,						C3D_T( "&Nouveau"));

	l_menu = new wxMenu();
	l_menu->Append( mbGeometries,					C3D_T( "&Geometries\tCTRL+A+G"));
	l_menu->Append( mbMaterials,					C3D_T( "&Materiaux\tCTRL+A+M"));
	l_menuBar->Append( l_menu,						C3D_T( "&Affichage"));

	l_menu = new wxMenu();
	l_menu->AppendCheckItem( mbSelect,				C3D_T( "&Sélectionner\tCTRL+E+S"));
	l_menu->AppendCheckItem( mbModify,				C3D_T( "&Modifier\tCTRL+E+M"));
	l_menu->Append( mbNone,							C3D_T( "&Annuler\tCTRL+E+C"));
	l_menu->AppendSeparator();
	l_menu->AppendRadioItem( mbSelectGeometries,	C3D_T( "&Géométrie\tCTRL+E+G"))->Enable( false);
	l_menu->AppendRadioItem( mbSelectPoints,		C3D_T( "&Point\tCTRL+E+P"))->Enable( false);
	l_menu->AppendSeparator();
	l_menu->Append( mbCloneSelection,				C3D_T( "&Dupliquer\tCTRL+E+D"));
	l_subMenu = new wxMenu();
	l_subMenu->Append( mbSubdividePNTriangles,		C3D_T( "PN &Triangles\tCTRL+E+S+T"));
	l_subMenu->Append( mbSubdivideLoop,				C3D_T( "&Loop\tCTRL+E+S+L"));
	l_menu->AppendSubMenu( l_subMenu,				C3D_T( "&Subdiviser"));
	l_menu->Append( mbSelectNone,					C3D_T( "&Aucune\tCTRL+E+A"));
	l_menuBar->Append( l_menu,						C3D_T( "&Sélection"));

	SetMenuBar( l_menuBar);
}

void CSMainFrame :: _createGeometry( Mesh::eTYPE p_meshType, String & p_name,
									 const String & p_meshStrVars,
									 const String & p_baseName, Scene * p_scene,
									 CSNewGeometryDialog * p_dialog, unsigned int i,
									 unsigned int j, float a, float b, float c)
{
	if (p_name.empty() || p_name == C3D_T( "Geometry Name"))
	{
		Char l_buffer[256];
		Sprintf( l_buffer, 255, C3D_T( "%s%d"), p_baseName.c_str(), g_nbGeometries);
		p_name = l_buffer;
	}
	SceneNode * l_sceneNode = p_scene->CreateSceneNode( C3D_T( "SN_") + p_name);
	if (l_sceneNode != NULL)
	{
		UIntArray l_faces;
		FloatArray l_dimensions;
		l_faces.push_back( i);
		l_faces.push_back( j);
		l_dimensions.push_back( a);
		l_dimensions.push_back( b);
		l_dimensions.push_back( c);
		Geometry * l_geometry = p_scene->CreatePrimitive( p_name, p_meshType, p_baseName + C3D_T( "_") + p_meshStrVars, l_faces, l_dimensions);
		if (l_geometry != NULL)
		{
			String l_materialName = p_dialog->GetMaterialName();
			Mesh * l_mesh = l_geometry->GetMesh();
			for (size_t i = 0 ; i < l_mesh->GetNbSubmeshes() ; i++)
			{
				l_mesh->GetSubmesh( i)->SetMaterial( l_materialName);
			}
			l_sceneNode->AttachGeometry( l_geometry);
			l_sceneNode->SetVisible( true);
			g_nbGeometries++;
		}
	}
}
