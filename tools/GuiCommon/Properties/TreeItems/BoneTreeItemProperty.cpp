#include "GuiCommon/Properties/TreeItems/BoneTreeItemProperty.hpp"

#include "GuiCommon/Properties/AdditionalProperties.hpp"

#include <Castor3D/Model/Skeleton/BoneNode.hpp>

namespace GuiCommon
{
	BoneTreeItemProperty::BoneTreeItemProperty( bool editable
		, castor3d::Engine * engine )
		: TreeItemProperty{ engine, editable }
	{
		CreateTreeItemMenu();
	}

	void BoneTreeItemProperty::doCreateProperties( wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY_BONE = _( "Bone: " );
		static wxString PROPERTY_CATEGORY_TRANSLATE = _( "Translate" );
		static wxString PROPERTY_CATEGORY_ROTATE = _( "Rotate" );
		static wxString PROPERTY_CATEGORY_SCALE = _( "Scale" );
		static wxString PROPERTY_CATEGORY_INVERSE_BIND = _( "Inverse Bind" );

		if ( !m_bone )
		{
			return;
		}

		addProperty( grid, PROPERTY_CATEGORY_BONE + wxString( m_bone->getName() ) );
		addProperty( grid, PROPERTY_CATEGORY_TRANSLATE, m_bone->getTransform().translate, [&]( wxVariant const & var ){} );
		auto rotate = m_bone->getTransform().rotate;
		addProperty( grid, PROPERTY_CATEGORY_ROTATE, castor::Point4f{ rotate->x, rotate->y, rotate->z, rotate->w }, [&]( wxVariant const & var ){} );
		addProperty( grid, PROPERTY_CATEGORY_SCALE, m_bone->getTransform().scale, [&]( wxVariant const & var ){} );
		addProperty( grid, PROPERTY_CATEGORY_INVERSE_BIND, m_bone->getInverseTransform(), [&]( wxVariant const & var ){} );
	}
}
