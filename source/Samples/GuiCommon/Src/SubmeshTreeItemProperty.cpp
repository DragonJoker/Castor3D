#include "SubmeshTreeItemProperty.hpp"

#include <FunctorEvent.hpp>
#include <Geometry.hpp>
#include <Material.hpp>
#include <MaterialManager.hpp>
#include <Scene.hpp>
#include <Submesh.hpp>

#include "AdditionalProperties.hpp"

#include <wx/propgrid/advprops.h>

using namespace Castor;
using namespace Castor3D;

namespace GuiCommon
{
	namespace
	{
		static wxString PROPERTY_CATEGORY_SUBMESH = _( "Submesh: " );
		static wxString PROPERTY_SUBMESH_MATERIAL = _( "Material" );
	}

	SubmeshTreeItemProperty::SubmeshTreeItemProperty( bool p_editable, GeometrySPtr p_pGeometry, SubmeshSPtr p_pSubmesh )
		: TreeItemProperty( p_pSubmesh->GetOwner(), p_editable, ePROPERTY_DATA_TYPE_SUBMESH )
		, m_pGeometry( p_pGeometry )
		, m_pSubmesh( p_pSubmesh )
	{
		PROPERTY_CATEGORY_SUBMESH = _( "Submesh: " );
		PROPERTY_SUBMESH_MATERIAL = _( "Material" );

		CreateTreeItemMenu();
	}

	SubmeshTreeItemProperty::~SubmeshTreeItemProperty()
	{
	}

	void SubmeshTreeItemProperty::DoCreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid )
	{
		GeometrySPtr l_geometry = GetGeometry();
		SubmeshSPtr l_submesh = GetSubmesh();

		if ( l_geometry && l_submesh )
		{
			p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_SUBMESH + wxString( l_geometry->GetName() ) ) );
			p_grid->Append( DoCreateMaterialProperty( PROPERTY_SUBMESH_MATERIAL ) )->SetValue( wxVariant( l_geometry->GetMaterial( l_submesh )->GetName() ) );
		}
	}

	void SubmeshTreeItemProperty::DoPropertyChange( wxPropertyGridEvent & p_event )
	{
		wxPGProperty * l_property = p_event.GetProperty();
		GeometrySPtr l_geometry = GetGeometry();
		SubmeshSPtr l_submesh = GetSubmesh();

		if ( l_property )
		{
			if ( l_property->GetName() == PROPERTY_SUBMESH_MATERIAL )
			{
				OnMaterialChange( String( l_property->GetValueAsString().c_str() ) );
			}
		}
	}

	void SubmeshTreeItemProperty::OnMaterialChange( Castor::String const & p_name )
	{
		SubmeshSPtr l_submesh = GetSubmesh();
		GeometrySPtr l_geometry = GetGeometry();

		DoApplyChange( [p_name, l_geometry, l_submesh]()
		{
			MaterialManager & l_manager = l_submesh->GetOwner()->GetMaterialManager();
			MaterialSPtr l_material = l_manager.Find( p_name );

			if ( l_material )
			{
				l_geometry->SetMaterial( l_submesh, l_material );
				l_geometry->GetScene()->InitialiseGeometries();
			}
		} );
	}
}
