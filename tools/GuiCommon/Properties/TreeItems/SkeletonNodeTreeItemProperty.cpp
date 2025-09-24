#include "GuiCommon/Properties/TreeItems/SkeletonNodeTreeItemProperty.hpp"

#include "GuiCommon/Properties/AdditionalProperties.hpp"

#include <Castor3D/Model/Skeleton/SkeletonNode.hpp>

namespace GuiCommon
{
	SkeletonNodeTreeItemProperty::SkeletonNodeTreeItemProperty( ImagesLoader & imagesLoader
		, bool editable
		, c3d::Engine * engine )
		: TreeItemProperty{ engine, imagesLoader, editable }
	{
		CreateTreeItemMenu();
	}

	void SkeletonNodeTreeItemProperty::doCreateProperties( wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY_NODE = _( "Node: " );
		static wxString PROPERTY_CATEGORY_TRANSLATE = _( "Translate" );
		static wxString PROPERTY_CATEGORY_ROTATE = _( "Rotate" );
		static wxString PROPERTY_CATEGORY_SCALE = _( "Scale" );

		if ( !m_node )
		{
			return;
		}

		addProperty( grid, PROPERTY_CATEGORY_NODE + wxString( m_node->getName() ) );
		addProperty( grid, PROPERTY_CATEGORY_TRANSLATE, m_node->getTransform().translate, [&]( wxVariant const & var ){} );
		auto rotate = m_node->getTransform().rotate;
		addProperty( grid, PROPERTY_CATEGORY_ROTATE, c3d::Point4f{ rotate->x, rotate->y, rotate->z, rotate->w }, [&]( wxVariant const & var ){} );
		addProperty( grid, PROPERTY_CATEGORY_SCALE, m_node->getTransform().scale, [&]( wxVariant const & var ){} );
	}
}
