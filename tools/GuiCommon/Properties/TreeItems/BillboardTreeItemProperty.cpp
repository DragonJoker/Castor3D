#include "GuiCommon/Properties/TreeItems/BillboardTreeItemProperty.hpp"

#include "GuiCommon/Properties/AdditionalProperties.hpp"
#include "GuiCommon/Properties/Math/PointProperties.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/MaterialCache.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Scene/BillboardList.hpp>
#include <Castor3D/Scene/Scene.hpp>

#include <wx/propgrid/advprops.h>

namespace GuiCommon
{
	BillboardTreeItemProperty::BillboardTreeItemProperty( bool p_editable, castor3d::BillboardList & p_billboard )
		: TreeItemProperty( p_billboard.getParentScene().getEngine(), p_editable )
		, m_billboard( p_billboard )
	{
		CreateTreeItemMenu();
	}

	void BillboardTreeItemProperty::doCreateProperties( wxPGEditor * editor
		, wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY_BILLBOARD = _( "Billboard: " );
		static wxString PROPERTY_BILLBOARD_MATERIAL = _( "Material" );
		static wxString PROPERTY_BILLBOARD_SIZE = _( "Size" );

		addProperty( grid, PROPERTY_CATEGORY_BILLBOARD + wxString( m_billboard.getName() ) );
		addPropertyT( grid, PROPERTY_BILLBOARD_SIZE, m_billboard.getDimensions(), &m_billboard, &castor3d::BillboardList::setDimensions );
		addProperty( grid, PROPERTY_BILLBOARD_MATERIAL, getMaterialsList(), m_billboard.getMaterial()->getName()
			, [this]( wxVariant const & var )
			{
				auto & cache = m_billboard.getParentScene().getEngine()->getMaterialCache();
				auto material = cache.find( variantCast< castor::String >( var ) ).lock().get();

				if ( material )
				{
					m_billboard.setMaterial( material );
				}
			} );
	}
}
