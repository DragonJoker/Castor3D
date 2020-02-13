#include "GuiCommon/Properties/TreeItems/BoneTreeItemProperty.hpp"

#include "GuiCommon/Properties/AdditionalProperties.hpp"

#include <Castor3D/Model/Skeleton/Bone.hpp>
#include <Castor3D/Model/Skeleton/Skeleton.hpp>
#include <Castor3D/Scene/Scene.hpp>

#include <wx/propgrid/advprops.h>

using namespace castor3d;
using namespace castor;

namespace GuiCommon
{
	namespace
	{
		static wxString PROPERTY_CATEGORY_BONE = _( "Bone: " );
	}

	BoneTreeItemProperty::BoneTreeItemProperty( bool editable, Bone & bone )
		: TreeItemProperty( bone.getSkeleton().getScene()->getEngine(), editable, ePROPERTY_DATA_TYPE_BONE )
		, m_bone( bone )
	{
		PROPERTY_CATEGORY_BONE = _( "Bone: " );

		CreateTreeItemMenu();
	}

	BoneTreeItemProperty::~BoneTreeItemProperty()
	{
	}

	void BoneTreeItemProperty::doCreateProperties( wxPGEditor * editor, wxPropertyGrid * grid )
	{
		grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_BONE + wxString( m_bone.getName() ) ) );
	}

	void BoneTreeItemProperty::doPropertyChange( wxPropertyGridEvent & event )
	{
		wxPGProperty * property = event.GetProperty();

		if ( property )
		{
		}
	}
}
