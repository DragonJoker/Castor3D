#include "PrecompiledHeader.h"

#include "MaterialsFrame.h"
#include "MaterialsListView.h"
#include "MaterialPanel.h"

using namespace Castor3D;
using namespace CastorArchitect;

MaterialsFrame :: MaterialsFrame( wxWindow * parent, const wxString & title,
									  const wxPoint & pos, const wxSize & size)
	:	wxFrame( parent, wxID_ANY, title, pos, size),
		m_listWidth( 120)
{
	wxSize l_size = GetClientSize();
	m_materialsList = new MaterialsListView( this, eMaterialsList, wxPoint( l_size.x - m_listWidth, 0), wxSize( m_listWidth, l_size.y));
	m_materialPanel = new MaterialPanel( this, wxPoint( 0, 25), wxSize( l_size.x - m_listWidth, l_size.y - 25));
	m_materialsList->Show();
}

MaterialsFrame :: ~MaterialsFrame()
{
}

void MaterialsFrame :: CreateMaterialPanel( const String & p_materialName)
{
	m_selectedMaterial = MaterialManager::GetElementByName( p_materialName);
	m_materialPanel->CreateMaterialPanel( p_materialName);
}

BEGIN_EVENT_TABLE( MaterialsFrame, wxFrame)
	EVT_LIST_ITEM_SELECTED(		eMaterialsList, MaterialsFrame::_onSelected)
	EVT_LIST_ITEM_DESELECTED(	eMaterialsList, MaterialsFrame::_onDeselected)
END_EVENT_TABLE()

void MaterialsFrame :: _onSelected( wxListEvent& event)
{
	CreateMaterialPanel( event.GetText().c_str());
}

void MaterialsFrame :: _onDeselected( wxListEvent& event)
{
	if ( ! m_selectedMaterial.null())
	{
		m_materialsList->CreateList();
		m_selectedMaterial.reset();
	}

	CreateMaterialPanel( C3DEmptyString);
}
