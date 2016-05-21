#include "BillboardTreeItemProperty.hpp"

#include <MaterialManager.hpp>
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

	BillboardTreeItemProperty::BillboardTreeItemProperty( bool p_editable, BillboardListSPtr p_billboard )
		: TreeItemProperty( p_billboard->GetScene()->GetEngine(), p_editable, ePROPERTY_DATA_TYPE_BILLBOARD )
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
		BillboardListSPtr l_billboard = GetBillboard();

		if ( l_billboard )
		{
			p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_BILLBOARD + wxString( l_billboard->GetName() ) ) );
			p_grid->Append( new SizeProperty( PROPERTY_BILLBOARD_SIZE ) )->SetValue( WXVARIANT( l_billboard->GetDimensions() ) );
			p_grid->Append( DoCreateMaterialProperty( PROPERTY_BILLBOARD_MATERIAL ) )->SetValue( wxVariant( make_wxString( l_billboard->GetMaterial()->GetName() ) ) );
		}
	}

	void BillboardTreeItemProperty::DoPropertyChange( wxPropertyGridEvent & p_event )
	{
		BillboardListSPtr l_billboard = GetBillboard();
		wxPGProperty * l_property = p_event.GetProperty();

		if ( l_property && l_billboard )
		{
			if ( l_property->GetName() == PROPERTY_BILLBOARD_MATERIAL )
			{
				OnMaterialChange( String( l_property->GetValueAsString().c_str() ) );
			}
			else if ( l_property->GetName() == PROPERTY_BILLBOARD_MATERIAL )
			{
				const Size & l_size = SizeRefFromVariant( l_property->GetValue() );
				OnSizeChange( l_size );
			}
		}
	}

	void BillboardTreeItemProperty::OnMaterialChange( Castor::String const & p_name )
	{
		BillboardListSPtr l_billboard = GetBillboard();

		DoApplyChange( [p_name, l_billboard]()
		{
			MaterialManager & l_manager = l_billboard->GetScene()->GetEngine()->GetMaterialManager();
			MaterialSPtr l_material = l_manager.Find( p_name );

			if ( l_material )
			{
				l_billboard->SetMaterial( l_material );
				l_billboard->GetScene()->SetChanged();
			}
		} );
	}

	void BillboardTreeItemProperty::OnSizeChange( Castor::Size const & p_size )
	{
		BillboardListSPtr l_billboard = GetBillboard();

		DoApplyChange( [p_size, l_billboard]()
		{
			l_billboard->SetDimensions( p_size );
		} );
	}
}
