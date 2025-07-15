/*
See LICENSE file in root folder
*/
#ifndef ___GC_RenderPassTreeItemProperty_HPP___
#define ___GC_RenderPassTreeItemProperty_HPP___

#include "GuiCommon/Properties/TreeItems/TreeItemProperty.hpp"

#include <Castor3D/Render/RenderModule.hpp>

namespace GuiCommon
{
	void fillRenderPassConfiguration( wxPropertyGrid * grid
		, TreeItemProperty & properties
		, c3d::RenderTechniqueNodesPass & renderPass );
}

#endif
