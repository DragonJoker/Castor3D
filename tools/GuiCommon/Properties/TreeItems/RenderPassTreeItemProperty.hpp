/*
See LICENSE file in root folder
*/
#ifndef ___GC_RenderPassTreeItemProperty_HPP___
#define ___GC_RenderPassTreeItemProperty_HPP___

#include "GuiCommon/Properties/TreeItems/TreeItemProperty.hpp"

#include <Castor3D/Render/Technique/TechniqueModule.hpp>

namespace GuiCommon
{
	void fillRenderPassConfiguration( wxPropertyGrid * grid
		, TreeItemProperty & properties
		, castor3d::RenderTechniquePass & renderPass );
}

#endif
