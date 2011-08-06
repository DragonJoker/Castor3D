#include "CastorShape/PrecompiledHeader.hpp"

#include "CastorShape/material/MaterialsFrame.hpp"

using namespace Castor3D;
using namespace CastorShape;

wxMaterialsFrame :: wxMaterialsFrame( wxWindow * parent, const wxString & title,
									  const wxPoint & pos, const wxSize & size)
	:	GuiCommon::wxMaterialsFrame( parent, title, pos, size)
{
	wxSize l_size = GetClientSize();

	m_newMaterial = new wxButton( this, mlfNewMaterial, cuT( "Nouveau"), wxPoint( 10, 3), wxSize( 50, 20), wxBORDER_SIMPLE);
	m_deleteMaterial = new wxButton( this, mlfDeleteMaterial, cuT( "Supprimer"), wxPoint( 180, 3), wxSize( 80, 20), wxBORDER_SIMPLE);
	m_deleteMaterial->Hide();
	m_newMaterialName = new wxTextCtrl( this, mlfNewMaterialName,
										wxEmptyString, wxPoint( 70, 3), wxSize( 100, 20),
										wxTE_PROCESS_ENTER | wxBORDER_SIMPLE);
	m_newMaterialName->Hide();
}

wxMaterialsFrame :: ~wxMaterialsFrame()
{
}

void wxMaterialsFrame :: CreateMaterialPanel( const String & p_materialName)
{
	GuiCommon::wxMaterialsFrame::CreateMaterialPanel( p_materialName);
	m_deleteMaterial->Show();
}

BEGIN_EVENT_TABLE( wxMaterialsFrame, GuiCommon::wxMaterialsFrame)
	EVT_LIST_ITEM_SELECTED(		mlfMaterialsList, 	wxMaterialsFrame::_onSelected)
	EVT_LIST_ITEM_DESELECTED(	mlfMaterialsList, 	wxMaterialsFrame::_onDeselected)
	EVT_BUTTON(	mlfNewMaterial,						wxMaterialsFrame::_onNewMaterial)
	EVT_BUTTON(	mlfDeleteMaterial,					wxMaterialsFrame::_onDeleteMaterial)
	EVT_TEXT_ENTER( mlfNewMaterialName,				wxMaterialsFrame::_onNewMaterialName)
END_EVENT_TABLE()

void wxMaterialsFrame :: _onNewMaterial( wxCommandEvent & event)
{
	m_newMaterialName->Show();
	m_newMaterialName->SetFocus();
}

void wxMaterialsFrame :: _onNewMaterialName ( wxCommandEvent & event)
{
	String l_name = (const char *)m_newMaterialName->GetValue().c_str();
	m_selectedMaterial = Factory<Material>::Create( l_name, 1);
	m_materialsList->AddItem( l_name);
	CreateMaterialPanel( l_name);
	m_newMaterialName->Hide();
	m_newMaterialName->SetValue( cuEmptyString);
}

void wxMaterialsFrame :: _onDeleteMaterial( wxCommandEvent & event)
{
	Collection<Material, String> l_mtlCollection;
	Collection<Scene, String> l_scnCollection;
	ScenePtr l_scene = l_scnCollection.GetElement( cuT( "MainScene"));
	BoolStrMap l_geometries = l_scene->GetGeometriesVisibility();
	BoolStrMap::iterator l_it = l_geometries.begin();
	GeometryPtr l_geometry;
	MeshPtr l_mesh;
	SubmeshPtr l_submesh;

	while (l_it != l_geometries.end())
	{
		l_geometry = l_scene->GetGeometry( l_it->first);
		l_mesh = l_geometry->GetMesh();
		for (size_t i = 0 ; i < l_mesh->GetNbSubmeshes() ; i++)
		{
			l_submesh = l_mesh->GetSubmesh( i);
			if (l_submesh->GetMaterialName() == m_selectedMaterial->GetName())
			{
				l_submesh->SetMaterial( Root::GetSingleton()->GetDefaultMaterial());
			}
		}
	}

	l_mtlCollection.RemoveElement( m_selectedMaterial->GetName());
	m_selectedMaterial.reset();
	CreateMaterialPanel( cuEmptyString);
	m_materialsList->CreateList();
}
