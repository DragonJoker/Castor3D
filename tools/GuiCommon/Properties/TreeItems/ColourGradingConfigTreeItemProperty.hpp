/*
See LICENSE file in root folder
*/
#ifndef ___GuiCommon_ColourGradingConfigTreeItemProperty_H___
#define ___GuiCommon_ColourGradingConfigTreeItemProperty_H___

#include "GuiCommon/Properties/TreeItems/TreeItemProperty.hpp"

#include <Castor3D/Render/ToneMapping/ColourGradingConfig.hpp>

namespace GuiCommon
{
	using ColourGradingConfigTreeItemProperty = TreeItemPropertyT< c3d::ColourGradingConfig >;
}

#endif
