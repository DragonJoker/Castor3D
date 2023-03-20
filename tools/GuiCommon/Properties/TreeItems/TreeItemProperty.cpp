#include "GuiCommon/Properties/TreeItems/TreeItemProperty.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/MaterialCache.hpp>
#include <Castor3D/Event/Frame/CpuFunctorEvent.hpp>

namespace GuiCommon
{
	namespace tiprop
	{
		typedef enum
		{
			eID_DELETE = 1
		}	eID;
	}
	
	TreeItemProperty::PropertyChangeHandler const TreeItemProperty::EmptyHandler = []( wxVariant const & ){};

	TreeItemProperty::TreeItemProperty( castor3d::Engine * engine
		, bool editable )
		: wxTreeItemData()
		, m_menu( nullptr )
		, m_editable( editable )
		, m_engine( engine )
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

			if ( m_engine )
			{
				m_engine->postEvent( castor3d::makeCpuFunctorEvent( castor3d::EventType::ePreRender
					, [value, handler]()
					{
						handler( value );
					} ) );
			}
			else
			{
				handler( value );
			}
		}
	}

	wxArrayString TreeItemProperty::getMaterialsList()const
	{
		wxArrayString choices;

		if ( m_engine )
		{
			auto & cache = m_engine->getMaterialCache();
			using LockType = std::unique_lock< castor3d::MaterialCache >;
			LockType lock{ castor::makeUniqueLock( cache ) };

			for ( auto & pair : cache )
			{
				choices.push_back( pair.first );
			}
		}

		return choices;
	}

	wxArrayString TreeItemProperty::getFontsList()const
	{
		wxArrayString choices;

		if ( m_engine )
		{
			auto & cache = m_engine->getFontCache();
			auto lock( castor::makeUniqueLock( cache ) );

			for ( auto & pair : cache )
			{
				choices.push_back( pair.first );
			}
		}

		return choices;
	}

	void TreeItemProperty::CreateTreeItemMenu()
	{
		if ( m_editable )
		{
			m_menu = new wxMenu();
			m_menu->Append( tiprop::eID_DELETE, _( "Delete" ) + wxT( "\tDEL" ) );
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

	wxPGProperty * TreeItemProperty::addProperty( wxPGProperty * grid
		, wxString const & name )
	{
		return grid->AppendChild( new wxPropertyCategory{ name } );
	}

	wxPGProperty * TreeItemProperty::addProperty( wxPropertyGrid * grid
		, wxString const & name
		, wxArrayString const & choices
		, wxString const & selected
		, PropertyChangeHandler handler
		, bool * control )
	{
		wxPGProperty * prop = createProperty( grid, name, choices, handler, control );
		prop->SetValue( selected );
		return prop;
	}

	wxPGProperty * TreeItemProperty::addProperty( wxPGProperty * grid
		, wxString const & name
		, wxArrayString const & choices
		, wxString const & selected
		, PropertyChangeHandler handler
		, bool * control )
	{
		wxPGProperty * prop = createProperty( grid, name, choices, handler, control );
		prop->SetValue( selected );
		return prop;
	}

	wxPGProperty * TreeItemProperty::addProperty( wxPropertyGrid * grid
		, wxString const & name
		, wxPGEditor * editor
		, PropertyChangeHandler handler
		, bool * control )
	{
		auto prop = grid->Append( new wxStringProperty{ _( "View shaders..." ), wxPG_LABEL, name } );
		prop->SetEditor( editor );
		prop->SetClientObject( new ButtonData{ doGetHandler( handler, castor3d::PassVisitorBase::makeControlsList( control ) ) } );
		return prop;
	}

	wxPGProperty * TreeItemProperty::addProperty( wxPGProperty * grid
		, wxString const & name
		, wxPGEditor * editor
		, PropertyChangeHandler handler
		, bool * control )
	{
		auto prop = grid->AppendChild( new wxStringProperty{ _( "View shaders..." ), wxPG_LABEL, name } );
		prop->SetEditor( editor );
		prop->SetClientObject( new ButtonData{ doGetHandler( handler, castor3d::PassVisitorBase::makeControlsList( control ) ) } );
		return prop;
	}

	wxPGProperty * TreeItemProperty::addProperty( wxPropertyGrid * grid
		, wxString const & name
		, wxArrayString const & choices
		, wxString const & selected
		, PropertyChangeHandler handler
		, std::atomic_bool * control )
	{
		wxPGProperty * prop = createProperty( grid, name, choices, handler, control );
		prop->SetValue( selected );
		return prop;
	}

	wxPGProperty * TreeItemProperty::addProperty( wxPGProperty * grid
		, wxString const & name
		, wxArrayString const & choices
		, wxString const & selected
		, PropertyChangeHandler handler
		, std::atomic_bool * control )
	{
		wxPGProperty * prop = createProperty( grid, name, choices, handler, control );
		prop->SetValue( selected );
		return prop;
	}

	wxPGProperty * TreeItemProperty::addProperty( wxPropertyGrid * grid
		, wxString const & name
		, wxPGEditor * editor
		, PropertyChangeHandler handler
		, std::atomic_bool * control )
	{
		auto prop = grid->Append( new wxStringProperty{ _( "View shaders..." ), wxPG_LABEL, name } );
		prop->SetEditor( editor );
		prop->SetClientObject( new ButtonData{ doGetHandler( handler, castor3d::PassVisitorBase::makeControlsList( control ) ) } );
		return prop;
	}

	wxPGProperty * TreeItemProperty::addProperty( wxPGProperty * grid
		, wxString const & name
		, wxPGEditor * editor
		, PropertyChangeHandler handler
		, std::atomic_bool * control )
	{
		auto prop = grid->AppendChild( new wxStringProperty{ _( "View shaders..." ), wxPG_LABEL, name } );
		prop->SetEditor( editor );
		prop->SetClientObject( new ButtonData{ doGetHandler( handler, castor3d::PassVisitorBase::makeControlsList( control ) ) } );
		return prop;
	}
	
	wxPGProperty * TreeItemProperty::addMaterial( wxPropertyGrid * parent
		, castor3d::Engine & engine
		, wxString const & name
		, wxArrayString const & choices
		, castor3d::MaterialRPtr selected
		, std::function< void( castor3d::MaterialRPtr ) > setter )
	{
		if ( selected )
		{
			return addProperty( parent
				, name
				, choices
				, selected->getName()
				, [&engine, &choices, setter]( wxVariant const & var )
				{
					auto name = make_String( choices[size_t( var.GetLong() )] );
					auto material = engine.findMaterial( name ).lock().get();

					if ( material )
					{
						setter( material );
					}
				} );
		}
		
		return addProperty( parent
			, name
			, choices
			, [&engine, &choices, setter]( wxVariant const & var )
			{
				auto name = make_String( choices[size_t( var.GetLong() )] );
				auto material = engine.findMaterial( name ).lock().get();

				if ( material )
				{
					setter( material );
				}
			} );
	}

	TreeItemProperty::PropertyChangeHandler TreeItemProperty::doGetHandler( TreeItemProperty::PropertyChangeHandler handler
		, castor3d::PassVisitorBase::ControlsList controls )
	{
		if ( controls.empty() )
		{
			return handler;
		}

		return [controls = std::move( controls ), handler]( wxVariant const & var )
		{
			for ( auto control : controls )
			{
				*control = true;
			}

			handler( var );
		};
	}

	TreeItemProperty::PropertyChangeHandler TreeItemProperty::doGetHandler( TreeItemProperty::PropertyChangeHandler handler
		, castor3d::PassVisitorBase::AtomicControlsList controls )
	{
		if ( controls.empty() )
		{
			return handler;
		}

		return [controls=std::move( controls ), handler]( wxVariant const & var )
		{
			for ( auto control : controls )
			{
				*control = true;
			}

			handler( var );
		};
	}
}
