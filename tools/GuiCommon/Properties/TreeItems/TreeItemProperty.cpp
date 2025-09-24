#include "GuiCommon/Properties/TreeItems/TreeItemProperty.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/MaterialCache.hpp>
#include <Castor3D/Event/Frame/CpuFunctorEvent.hpp>

CU_ImplementSmartPtr( GuiCommon, TreeItemProperty )

namespace GuiCommon
{
	namespace tiprop
	{
		enum class eID
		{
			eDelete = 1
		};
	}

	TreeItemProperty::PropertyChangeHandler const TreeItemProperty::EmptyHandler = []( wxVariant const & ){};

	TreeItemProperty::TreeItemProperty( c3d::Engine * engine
		, ImagesLoader & imagesLoader
		, bool editable )
		: wxTreeItemData{}
		, m_imagesLoader{ imagesLoader }
		, m_editable{ editable }
		, m_engine{ engine }
	{
	}

	void TreeItemProperty::displayTreeItemMenu( wxWindow * window, wxCoord, wxCoord )const
	{
		if ( m_editable && m_menu )
			window->PopupMenu( m_menu.get() );
	}

	void TreeItemProperty::clearProperties()
	{
		if ( m_grid )
		{
			m_handlers.clear();
			m_grid->Clear();
		}
	}

	void TreeItemProperty::createProperties( wxPropertyGrid * grid )
	{
		doCreateProperties( grid );
		m_grid = grid;
	}

	void TreeItemProperty::onPropertyChange( wxPropertyGridEvent & event )
	{
		if ( auto it = m_handlers.find( event.GetPropertyName() );
			it != m_handlers.end() )
		{
			auto handler = it->second;
			auto value = event.GetValue();

			if ( m_engine )
			{
				m_engine->postEvent( c3d::makeCpuFunctorEvent( c3d::CpuEventType::ePreGpuStep
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

		event.Skip( false );
	}

	wxArrayString TreeItemProperty::getMaterialsList()const
	{
		wxArrayString choices;

		if ( m_engine )
		{
			auto & cache = m_engine->getMaterialCache();
			using LockType = c3d::UniqueLock< c3d::MaterialCache >;
			LockType lock{ c3d::makeUniqueLock( cache ) };

			for ( auto const & [name, _] : cache )
			{
				choices.push_back( name );
			}
		}

		return choices;
	}

	wxArrayString TreeItemProperty::getFontsList()const
	{
		wxArrayString choices;

		if ( m_engine )
		{
			m_engine->forEachFont( [&choices]( c3d::String const & name, c3d::Font const & )
				{
					choices.push_back( name );
				} );
		}

		return choices;
	}

	void TreeItemProperty::CreateTreeItemMenu()
	{
		if ( m_editable )
		{
			m_menu = c3d::makeRawUnique< wxMenu >();
			m_menu->Append( int( tiprop::eID::eDelete ), _( "Delete" ) + wxT( "\tDEL" ) );
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
		wxPGProperty * prop = createProperty( grid, name, choices, c3d::move( handler ), control );
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
		wxPGProperty * prop = createProperty( grid, name, choices, c3d::move( handler ), control );
		prop->SetValue( selected );
		return prop;
	}

	wxPGProperty * TreeItemProperty::addProperty( wxPropertyGrid * grid
		, wxString const & name
		, PropertyChangeHandler handler
		, bool * control )
	{
		auto prop = grid->Append( new wxStringProperty{ _( "View shaders..." ), wxPG_LABEL, name } );
		prop->SetEditor( wxPGConstructButtonCtrlEditorClass() );
		prop->SetClientObject( new ButtonData{ doGetHandler( c3d::move( handler ), c3d::ConfigurationVisitorBase::makeControlsList( control ) ) } );
		return prop;
	}

	wxPGProperty * TreeItemProperty::addProperty( wxPGProperty * grid
		, wxString const & name
		, PropertyChangeHandler handler
		, bool * control )
	{
		auto prop = grid->AppendChild( new wxStringProperty{ _( "View shaders..." ), wxPG_LABEL, name } );
		prop->SetEditor( wxPGConstructButtonCtrlEditorClass() );
		prop->SetClientObject( new ButtonData{ doGetHandler( c3d::move( handler ), c3d::ConfigurationVisitorBase::makeControlsList( control ) ) } );
		return prop;
	}

	wxPGProperty * TreeItemProperty::addProperty( wxPropertyGrid * grid
		, wxString const & name
		, wxArrayString const & choices
		, wxString const & selected
		, PropertyChangeHandler handler
		, std::atomic_bool * control )
	{
		wxPGProperty * prop = createProperty( grid, name, choices, c3d::move( handler ), control );
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
		wxPGProperty * prop = createProperty( grid, name, choices, c3d::move( handler ), control );
		prop->SetValue( selected );
		return prop;
	}

	wxPGProperty * TreeItemProperty::addProperty( wxPropertyGrid * grid
		, wxString const & name
		, PropertyChangeHandler handler
		, std::atomic_bool * control )
	{
		auto prop = grid->Append( new wxStringProperty{ _( "View shaders..." ), wxPG_LABEL, name } );
		prop->SetEditor( wxPGConstructButtonCtrlEditorClass() );
		prop->SetClientObject( new ButtonData{ doGetHandler( c3d::move( handler ), c3d::ConfigurationVisitorBase::makeControlsList( control ) ) } );
		return prop;
	}

	wxPGProperty * TreeItemProperty::addProperty( wxPGProperty * grid
		, wxString const & name
		, PropertyChangeHandler handler
		, std::atomic_bool * control )
	{
		auto prop = grid->AppendChild( new wxStringProperty{ _( "View shaders..." ), wxPG_LABEL, name } );
		prop->SetEditor( wxPGConstructButtonCtrlEditorClass() );
		prop->SetClientObject( new ButtonData{ doGetHandler( c3d::move( handler ), c3d::ConfigurationVisitorBase::makeControlsList( control ) ) } );
		return prop;
	}
	
	wxPGProperty * TreeItemProperty::addMaterial( wxPropertyGrid * parent
		, c3d::Engine const & engine
		, wxString const & name
		, wxArrayString const & choices
		, c3d::Material const * selected
		, c3d::Function< void( c3d::MaterialObs ) > const & setter )
	{
		if ( selected )
		{
			return addProperty( parent
				, name
				, choices
				, selected->getName()
				, [&engine, &choices, &setter]( wxVariant const & var )
				{
					if ( auto name = make_String( choices[size_t( var.GetLong() )] );
						auto material = engine.findMaterial( name ) )
						setter( material );
				} );
		}
		
		return addProperty( parent
			, name
			, choices
			, [&engine, &choices, setter]( wxVariant const & var )
			{
				if ( auto name = make_String( choices[size_t( var.GetLong() )] );
					auto material = engine.findMaterial( name ) )
					setter( material );
			} );
	}

	TreeItemProperty::PropertyChangeHandler TreeItemProperty::doGetHandler( TreeItemProperty::PropertyChangeHandler handler
		, c3d::ConfigurationVisitorBase::ControlsList controls )const
	{
		if ( controls.empty() )
			return handler;

		return [controls = c3d::move( controls ), handler]( wxVariant const & var )
		{
			for ( auto const & [control, callback] : controls )
			{
				*control = true;
				if ( callback )
					callback();
			}

			handler( var );
		};
	}

	TreeItemProperty::PropertyChangeHandler TreeItemProperty::doGetHandler( TreeItemProperty::PropertyChangeHandler handler
		, c3d::ConfigurationVisitorBase::AtomicControlsList controls )const
	{
		if ( controls.empty() )
		{
			return handler;
		}

		return [controls=c3d::move( controls ), handler]( wxVariant const & var )
		{
			for ( auto const & [control, callback] : controls )
			{
				*control = true;
				if ( callback )
					callback();
			}

			handler( var );
		};
	}
}
