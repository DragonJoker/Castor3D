#include "GuiCommon/Properties/TreeItems/TreeItemProperty.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/MaterialCache.hpp>
#include <Castor3D/Event/Frame/CpuFunctorEvent.hpp>

using namespace castor;
using namespace castor3d;

namespace GuiCommon
{
	typedef enum
	{
		eID_DELETE = 1
	}	eID;
	
	TreeItemProperty::PropertyChangeHandler const TreeItemProperty::EmptyHandler = []( wxVariant const & ){};

	TreeItemProperty::TreeItemProperty( castor3d::Engine * engine
		, bool editable
		, ePROPERTY_DATA_TYPE type )
		: wxTreeItemData()
		, m_type( type )
		, m_editable( editable )
		, m_engine( engine )
		, m_menu( nullptr )
	{
	}

	TreeItemProperty::~TreeItemProperty()
	{
		delete m_menu;
	}

	void TreeItemProperty::DisplayTreeItemMenu( wxWindow * window, wxCoord x, wxCoord y )
	{
		if ( m_editable && m_menu )
		{
			window->PopupMenu( m_menu );
		}
	}

	void TreeItemProperty::CreateProperties( wxPGEditor * editor
		, wxPropertyGrid * grid )
	{
		doCreateProperties( editor, grid );
	}

	void TreeItemProperty::onPropertyChange( wxPropertyGridEvent & event )
	{
		auto it = m_handlers.find( event.GetPropertyName() );

		if ( it != m_handlers.end() )
		{
			auto handler = it->second;
			auto value = event.GetValue();
			m_engine->postEvent( makeCpuFunctorEvent( EventType::ePreRender
				, [value, handler]()
				{
					handler( value );
				} ) );
		}
	}

	wxArrayString TreeItemProperty::getMaterialsList()
	{
		auto & cache = m_engine->getMaterialCache();
		wxArrayString choices;
		using LockType = std::unique_lock< MaterialCache >;
		LockType lock{ castor::makeUniqueLock( cache ) };

		for ( auto pair : cache )
		{
			choices.push_back( pair.first );
		}

		return choices;
	}

	void TreeItemProperty::CreateTreeItemMenu()
	{
		if ( m_editable )
		{
			m_menu = new wxMenu();
			m_menu->Append( eID_DELETE, _( "Delete" ) + wxT( "\tDEL" ) );
			doCreateTreeItemMenu();
		}
	}

	void TreeItemProperty::doCreateTreeItemMenu()
	{
	}

	wxPGProperty * TreeItemProperty::addProperty( wxPropertyGrid * grid
		, wxString const & name )
	{
		return grid->Append( new wxPropertyCategory{ name } );
	}

	wxPGProperty * TreeItemProperty::addProperty( wxPropertyGrid * grid
		, wxString const & name
		, wxArrayString const & choices
		, wxString const & selected
		, PropertyChangeHandler handler )
	{
		wxPGProperty * prop = createProperty( grid, name, choices, handler );
		prop->SetValue( selected );
		return prop;
	}

	wxPGProperty * TreeItemProperty::addProperty( wxPropertyGrid * grid
		, wxString const & name
		, wxPGEditor * editor
		, PropertyChangeHandler handler )
	{
		auto prop = grid->Append( new wxStringProperty{ _( "View shaders..." ), wxPG_LABEL, name } );
		prop->SetEditor( editor );
		prop->SetClientObject( new ButtonData{ handler } );
		return prop;
	}
}
