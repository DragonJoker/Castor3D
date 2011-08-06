#include "GuiCommon/PrecompiledHeader.hpp"

#include "GuiCommon/MaterialsFrame.hpp"
#include "GuiCommon/MaterialsListView.hpp"
#include "GuiCommon/MaterialPanel.hpp"

using namespace Castor3D;
using namespace GuiCommon;

wxMaterialsFrame :: wxMaterialsFrame( wxWindow * parent, const wxString & title, const wxPoint & pos, const wxSize & size)
	:	wxFrame( parent, wxID_ANY, title, pos, size, wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN | wxFRAME_FLOAT_ON_PARENT)
	,	m_listWidth( 120)
{
	wxSize l_size = GetClientSize();
	m_materialsList = new wxMaterialsListView( this, eID_LIST_MATERIALS, wxPoint( l_size.x - m_listWidth, 0), wxSize( m_listWidth, l_size.y));
	m_materialPanel = new wxMaterialPanel( this, wxPoint( 0, 25), wxSize( l_size.x - m_listWidth, l_size.y - 25));
	m_materialsList->Show();
}

wxMaterialsFrame :: ~wxMaterialsFrame()
{
}

void wxMaterialsFrame :: CreateMaterialPanel( String const & p_materialName)
{
	Root::MaterialManager * l_pManager = Root::GetSingleton()->GetMaterialManager();
	m_selectedMaterial = l_pManager->GetElement( p_materialName);
	m_materialPanel->CreateMaterialPanel( p_materialName);
}

BEGIN_EVENT_TABLE( wxMaterialsFrame, wxFrame)
	EVT_SHOW(										wxMaterialsFrame::_onShow)
	EVT_LIST_ITEM_SELECTED(		eID_LIST_MATERIALS, wxMaterialsFrame::_onSelected)
	EVT_LIST_ITEM_DESELECTED(	eID_LIST_MATERIALS, wxMaterialsFrame::_onDeselected)
END_EVENT_TABLE()

void wxMaterialsFrame :: _onShow( wxShowEvent & event)
{
	if (event.IsShown())
	{
		m_materialsList->CreateList();
		CreateMaterialPanel( cuEmptyString);
	}
}

void wxMaterialsFrame :: _onSelected( wxListEvent& event)
{
	CreateMaterialPanel( (char const *)event.GetText().c_str());
}

void wxMaterialsFrame :: _onDeselected( wxListEvent& event)
{
	if (m_selectedMaterial)
	{
//		m_materialsList->CreateList();
		m_selectedMaterial.reset();
	}

	CreateMaterialPanel( cuEmptyString);
}
