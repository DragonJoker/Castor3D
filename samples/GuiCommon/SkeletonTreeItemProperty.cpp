#include "GuiCommon/SkeletonTreeItemProperty.hpp"

#include "GuiCommon/AdditionalProperties.hpp"

#include <Castor3D/Mesh/Skeleton/Skeleton.hpp>
#include <Castor3D/Scene/Scene.hpp>

#include <wx/propgrid/advprops.h>

using namespace castor3d;
using namespace castor;

namespace GuiCommon
{
	namespace
	{
		static wxString PROPERTY_CATEGORY_SKELETON = _( "Skeleton: " );
	}

	SkeletonTreeItemProperty::SkeletonTreeItemProperty( bool editable, Skeleton & skeleton )
		: TreeItemProperty( skeleton.getScene()->getEngine(), editable, ePROPERTY_DATA_TYPE_MATERIAL )
		, m_skeleton( skeleton )
	{
		PROPERTY_CATEGORY_SKELETON = _( "Skeleton: " );

		CreateTreeItemMenu();
	}

	SkeletonTreeItemProperty::~SkeletonTreeItemProperty()
	{
	}

	void SkeletonTreeItemProperty::doCreateProperties( wxPGEditor * editor, wxPropertyGrid * grid )
	{
		grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_SKELETON ) );
	}

	void SkeletonTreeItemProperty::doPropertyChange( wxPropertyGridEvent & event )
	{
		wxPGProperty * property = event.GetProperty();

		if ( property )
		{
		}
	}
}
