//******************************************************************************
#include "PrecompiledHeaders.h"

#include "material/MaterialsFrame.h"
#include "material/MaterialsListView.h"
#include "material/MaterialPanel.h"
//******************************************************************************
using namespace Castor3D;
//******************************************************************************
BEGIN_EVENT_TABLE( CSMaterialsFrame, wxFrame)
	EVT_LIST_ITEM_SELECTED(		mlfMaterialsList, CSMaterialsFrame::_onSelected)
	EVT_LIST_ITEM_DESELECTED(	mlfMaterialsList, CSMaterialsFrame::_onDeselected)
	EVT_BUTTON(	mlfNewMaterial,			CSMaterialsFrame::_onNewMaterial)
	EVT_BUTTON(	mlfDeleteMaterial,		CSMaterialsFrame::_onDeleteMaterial)
	EVT_TEXT_ENTER( mlfNewMaterialName,	CSMaterialsFrame::_onNewMaterialName)
END_EVENT_TABLE()
//******************************************************************************

CSMaterialsFrame :: CSMaterialsFrame( wxWindow * parent, const wxString & title,
									  const wxPoint & pos, const wxSize & size,
									  wxWindowID id, long style, wxString name)
	:	wxFrame( parent, id, title, pos, size, style, name), 
		m_listWidth( 120),
		m_selectedMaterial( NULL)
{
	wxSize l_size = GetClientSize();
	m_materialsList = new CSMaterialsListView( this, mlfMaterialsList,
											   wxPoint( l_size.x - m_listWidth, 0),
											   wxSize( m_listWidth, l_size.y));
	m_materialPanel = new CSMaterialPanel( this, wxPoint( 0, 25), wxSize( l_size.x - m_listWidth, l_size.y - 25));
	m_newMaterial = new wxButton( this, mlfNewMaterial, C3D_T( "Nouveau"), wxPoint( 10, 3), wxSize( 50, 20), wxBORDER_SIMPLE);
	m_deleteMaterial = new wxButton( this, mlfDeleteMaterial, C3D_T( "Supprimer"), wxPoint( 180, 3), wxSize( 80, 20), wxBORDER_SIMPLE);
	m_deleteMaterial->Hide();
	m_materialsList->Show();
	m_newMaterialName = new wxTextCtrl( this, mlfNewMaterialName,
										wxEmptyString, wxPoint( 70, 3), wxSize( 100, 20),
										wxTE_PROCESS_ENTER | wxBORDER_SIMPLE);
	m_newMaterialName->Hide();
}

//******************************************************************************

CSMaterialsFrame :: ~CSMaterialsFrame()
{
}

//******************************************************************************

void CSMaterialsFrame :: _onSelected( wxListEvent& event)
{
	CreateMaterialPanel( event.GetText().c_str());
}

//******************************************************************************

void CSMaterialsFrame :: _onDeselected( wxListEvent& event)
{
	if (m_selectedMaterial != NULL)
	{
		m_materialsList->CreateList();
		m_selectedMaterial = NULL;
	}
	CreateMaterialPanel( C3DEmptyString);
}

//******************************************************************************

void CSMaterialsFrame :: _onNewMaterial( wxCommandEvent & event)
{
	m_newMaterialName->Show();
	m_newMaterialName->SetFocus();
}

//******************************************************************************

void CSMaterialsFrame :: _onNewMaterialName ( wxCommandEvent & event)
{
	String l_name = m_newMaterialName->GetValue().c_str();
	m_selectedMaterial = MaterialManager::GetSingletonPtr()->CreateMaterial( l_name);
	m_materialsList->AddItem( l_name);
	CreateMaterialPanel( l_name);
	m_newMaterialName->Hide();
	m_newMaterialName->SetValue( C3DEmptyString);
}

//******************************************************************************

void CSMaterialsFrame :: _onDeleteMaterial( wxCommandEvent & event)
{
	Scene * l_scene = SceneManager::GetSingletonPtr()->GetElementByName( C3D_T( "MainScene"));
	std::map <String, bool> l_geometries = l_scene->GetGeometriesVisibility();
	std::map <String, bool>::iterator l_it = l_geometries.begin();
	Geometry * l_geometry;
	Mesh * l_mesh;
	Submesh * l_submesh;
	while (l_it != l_geometries.end())
	{
		l_geometry = l_scene->GetGeometry( l_it->first);
		l_mesh = l_geometry->GetMesh();
		for (size_t i = 0 ; i < l_mesh->GetNbSubmeshes() ; i++)
		{
			l_submesh = l_mesh->GetSubmesh( i);
			if (l_submesh->GetMaterialName() == m_selectedMaterial->GetName())
			{
				l_submesh->SetMaterial( C3D_T( "DefaultMaterial"));
			}
		}
	}
	MaterialManager::GetSingletonPtr()->RemoveElement( m_selectedMaterial);
	delete m_selectedMaterial;
	m_selectedMaterial = NULL;
	CreateMaterialPanel( C3DEmptyString);
	m_materialsList->CreateList();
}

//******************************************************************************

void CSMaterialsFrame :: CreateMaterialPanel( const String & p_materialName)
{
	m_selectedMaterial = MaterialManager::GetSingletonPtr()->GetElementByName( p_materialName);
	m_materialPanel->CreateMaterialPanel( p_materialName);
	m_deleteMaterial->Show();
}

//******************************************************************************
