#include "BillboardTreeItemProperty.hpp"

#include <Engine.hpp>
#include <Material/Material.hpp>
#include <Scene/BillboardList.hpp>
#include <Scene/Scene.hpp>

#include "AdditionalProperties.hpp"
#include "SizeProperties.hpp"
#include <wx/propgrid/advprops.h>

using namespace Castor3D;
using namespace Castor;

namespace GuiCommon
{
	namespace
	{
		static wxString PROPERTY_CATEGORY_BILLBOARD = _( "Billboard: " );
		static wxString PROPERTY_BILLBOARD_MATERIAL = _( "Material" );
		static wxString PROPERTY_BILLBOARD_SIZE = _( "Size" );
	}

	BillboardTreeItemProperty::BillboardTreeItemProperty( bool p_editable, BillboardList & p_billboard )
		: TreeItemProperty( p_billboard.GetParentScene().GetEngine(), p_editable, ePROPERTY_DATA_TYPE_BILLBOARD )
		, m_billboard( p_billboard )
	{
		PROPERTY_CATEGORY_BILLBOARD = _( "Billboard: " );
		PROPERTY_BILLBOARD_MATERIAL = _( "Material" );
		PROPERTY_BILLBOARD_SIZE = _( "Size" );

		CreateTreeItemMenu();
	}

	BillboardTreeItemProperty::~BillboardTreeItemProperty()
	{
	}

	void BillboardTreeItemProperty::DoCreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid )
	{
		p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_BILLBOARD + wxString( m_billboard.GetName() ) ) );
		p_grid->Append( new SizeProperty( PROPERTY_BILLBOARD_SIZE ) )->SetValue( WXVARIANT( m_billboard.GetDimensions() ) );
		p_grid->Append( DoCreateMaterialProperty( PROPERTY_BILLBOARD_MATERIAL ) )->SetValue( wxVariant( make_wxString( m_billboard.GetMaterial()->GetName() ) ) );
	}

	void BillboardTreeItemProperty::DoPropertyChange( wxPropertyGridEvent & p_event )
	{
		wxPGProperty * property = p_event.GetProperty();

		if ( property )
		{
			if ( property->GetName() == PROPERTY_BILLBOARD_MATERIAL )
			{
				OnMaterialChange( String( property->GetValueAsString().c_str() ) );
			}
			else if ( property->GetName() == PROPERTY_BILLBOARD_MATERIAL )
			{
				const Size & size = SizeRefFromVariant( property->GetValue() );
				OnSizeChange( size );
			}
		}
	}

	void BillboardTreeItemProperty::OnMaterialChange( Castor::String const & p_name )
	{
		DoApplyChange( [p_name, this]()
		{
			auto & cache = m_billboard.GetParentScene().GetEngine()->GetMaterialCache();
			MaterialSPtr material = cache.Find( p_name );

			if ( material )
			{
				m_billboard.SetMaterial( material );
				m_billboard.GetParentScene().SetChanged();
			}
		} );
	}

	void BillboardTreeItemProperty::OnSizeChange( Castor::Size const & p_size )
	{
		DoApplyChange( [p_size, this]()
		{
			m_billboard.SetDimensions( p_size );
		} );
	}
}
