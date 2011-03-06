#include "CastorShape/PrecompiledHeader.h"

#include "CastorShape/material/MaterialsFrame.h"

using namespace Castor3D;
using namespace CastorShape;

MaterialsFrame :: MaterialsFrame( MaterialManager * p_pManager, wxWindow * parent, const wxString & title,
									  const wxPoint & pos, const wxSize & size)
	:	GuiCommon::MaterialsFrame( p_pManager, parent, title, pos, size)
{
	wxSize l_size = GetClientSize();

	m_newMaterial = new wxButton( this, mlfNewMaterial, CU_T( "Nouveau"), wxPoint( 10, 3), wxSize( 50, 20), wxBORDER_SIMPLE);
	m_deleteMaterial = new wxButton( this, mlfDeleteMaterial, CU_T( "Supprimer"), wxPoint( 180, 3), wxSize( 80, 20), wxBORDER_SIMPLE);
	m_deleteMaterial->Hide();
	m_newMaterialName = new wxTextCtrl( this, mlfNewMaterialName,
										wxEmptyString, wxPoint( 70, 3), wxSize( 100, 20),
										wxTE_PROCESS_ENTER | wxBORDER_SIMPLE);
	m_newMaterialName->Hide();
}

MaterialsFrame :: ~MaterialsFrame()
{
}

void MaterialsFrame :: CreateMaterialPanel( const String & p_materialName)
{
	GuiCommon::MaterialsFrame::CreateMaterialPanel( p_materialName);
	m_deleteMaterial->Show();
}

BEGIN_EVENT_TABLE( MaterialsFrame, GuiCommon::MaterialsFrame)
	EVT_LIST_ITEM_SELECTED(		mlfMaterialsList, 	MaterialsFrame::_onSelected)
	EVT_LIST_ITEM_DESELECTED(	mlfMaterialsList, 	MaterialsFrame::_onDeselected)
	EVT_BUTTON(	mlfNewMaterial,						MaterialsFrame::_onNewMaterial)
	EVT_BUTTON(	mlfDeleteMaterial,					MaterialsFrame::_onDeleteMaterial)
	EVT_TEXT_ENTER( mlfNewMaterialName,				MaterialsFrame::_onNewMaterialName)
END_EVENT_TABLE()

void MaterialsFrame :: _onNewMaterial( wxCommandEvent & event)
{
	m_newMaterialName->Show();
	m_newMaterialName->SetFocus();
}

void MaterialsFrame :: _onNewMaterialName ( wxCommandEvent & event)
{
	String l_name = m_newMaterialName->GetValue().c_str();
	m_selectedMaterial = m_pManager->CreateMaterial( l_name);
	m_materialsList->AddItem( l_name);
	CreateMaterialPanel( l_name);
	m_newMaterialName->Hide();
	m_newMaterialName->SetValue( C3DEmptyString);
}

void MaterialsFrame :: _onDeleteMaterial( wxCommandEvent & event)
{
	ScenePtr l_scene = m_pManager->GetParent()->GetElementByName( CU_T( "MainScene"));
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
				l_submesh->SetMaterial( m_pManager->GetDefaultMaterial());
			}
		}
	}

	m_pManager->RemoveElement( m_selectedMaterial);
	m_selectedMaterial.reset();
	CreateMaterialPanel( C3DEmptyString);
	m_materialsList->CreateList();
}
