#include "TreeItemProperty.hpp"

#include <Engine.hpp>
#include <Cache/MaterialCache.hpp>
#include <Event/Frame/FunctorEvent.hpp>

using namespace Castor;
using namespace Castor3D;

namespace GuiCommon
{
	typedef enum
	{
		eID_DELETE = 1
	}	eID;

	TreeItemProperty::TreeItemProperty( Castor3D::Engine * p_engine, bool p_editable, ePROPERTY_DATA_TYPE p_type )
		: wxTreeItemData()
		, m_type( p_type )
		, m_editable( p_editable )
		, m_engine( p_engine )
		, m_menu( nullptr )
	{
	}

	TreeItemProperty::~TreeItemProperty()
	{
		delete m_menu;
	}

	void TreeItemProperty::DisplayTreeItemMenu( wxWindow * p_window, wxCoord x, wxCoord y )
	{
		if ( m_editable && m_menu )
		{
			p_window->PopupMenu( m_menu );
		}
	}

	void TreeItemProperty::CreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid )
	{
		DoCreateProperties( p_editor, p_grid );
	}

	void TreeItemProperty::OnPropertyChange( wxPropertyGridEvent & p_event )
	{
		DoPropertyChange( p_event );
	}

	wxEnumProperty * TreeItemProperty::DoCreateMaterialProperty( wxString const & p_name )
	{
		wxEnumProperty * material = new wxEnumProperty( p_name );
		auto & cache = m_engine->GetMaterialCache();
		wxPGChoices choices;
		auto lock = make_unique_lock( cache );

		for ( auto pair : cache )
		{
			choices.Add( pair.first );
		}

		material->SetChoices( choices );
		return material;
	}

	void TreeItemProperty::DoApplyChange( std::function< void() > p_functor )
	{
		m_engine->PostEvent( MakeFunctorEvent( EventType::ePreRender, p_functor ) );
	}

	void TreeItemProperty::CreateTreeItemMenu()
	{
		if ( m_editable )
		{
			m_menu = new wxMenu();
			m_menu->Append( eID_DELETE, _( "Delete" ) + wxT( "\tDEL" ) );
			DoCreateTreeItemMenu();
		}
	}

	void TreeItemProperty::DoCreateTreeItemMenu()
	{
	}
}
