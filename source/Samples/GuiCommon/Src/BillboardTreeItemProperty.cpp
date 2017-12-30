#include "BillboardTreeItemProperty.hpp"

#include <Engine.hpp>
#include <Material/Material.hpp>
#include <Scene/BillboardList.hpp>
#include <Scene/Scene.hpp>

#include "AdditionalProperties.hpp"
#include "PointProperties.hpp"
#include <wx/propgrid/advprops.h>

using namespace castor3d;
using namespace castor;

namespace GuiCommon
{
	namespace
	{
		static wxString PROPERTY_CATEGORY_BILLBOARD = _( "Billboard: " );
		static wxString PROPERTY_BILLBOARD_MATERIAL = _( "Material" );
		static wxString PROPERTY_BILLBOARD_SIZE = _( "Size" );
	}

	BillboardTreeItemProperty::BillboardTreeItemProperty( bool p_editable, BillboardList & p_billboard )
		: TreeItemProperty( p_billboard.getParentScene().getEngine(), p_editable, ePROPERTY_DATA_TYPE_BILLBOARD )
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

	void BillboardTreeItemProperty::doCreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid )
	{
		p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_BILLBOARD + wxString( m_billboard.getName() ) ) );
		p_grid->Append( new Point2fProperty( PROPERTY_BILLBOARD_SIZE ) )->SetValue( WXVARIANT( m_billboard.getDimensions() ) );
		p_grid->Append( doCreateMaterialProperty( PROPERTY_BILLBOARD_MATERIAL ) )->SetValue( wxVariant( make_wxString( m_billboard.getMaterial()->getName() ) ) );
	}

	void BillboardTreeItemProperty::doPropertyChange( wxPropertyGridEvent & p_event )
	{
		wxPGProperty * property = p_event.GetProperty();

		if ( property )
		{
			if ( property->GetName() == PROPERTY_BILLBOARD_MATERIAL )
			{
				OnMaterialChange( String( property->GetValueAsString().c_str() ) );
			}
			else if ( property->GetName() == PROPERTY_BILLBOARD_SIZE )
			{
				const Point2f & size = Point2fRefFromVariant( property->GetValue() );
				OnSizeChange( size );
			}
		}
	}

	void BillboardTreeItemProperty::OnMaterialChange( castor::String const & p_name )
	{
		doApplyChange( [p_name, this]()
		{
			auto & cache = m_billboard.getParentScene().getEngine()->getMaterialCache();
			MaterialSPtr material = cache.find( p_name );

			if ( material )
			{
				m_billboard.setMaterial( material );
				m_billboard.getParentScene().setChanged();
			}
		} );
	}

	void BillboardTreeItemProperty::OnSizeChange( castor::Point2f const & p_size )
	{
		doApplyChange( [p_size, this]()
		{
			m_billboard.setDimensions( p_size );
		} );
	}
}
