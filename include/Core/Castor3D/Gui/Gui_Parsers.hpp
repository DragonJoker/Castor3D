/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GuiParsers_H___
#define ___C3D_GuiParsers_H___

#include "Castor3D/Castor3DModule.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>

namespace castor3d
{
	enum class GUISection
		: uint32_t
	{
		eGUI = CU_MakeSectionName( 'C', 'G', 'U', 'I' ),
		eTheme = CU_MakeSectionName( 'C', 'G', 'T', 'H' ),
		eButtonStyle = CU_MakeSectionName( 'C', 'T', 'B', 'T' ),
		eEditStyle = CU_MakeSectionName( 'C', 'T', 'E', 'D' ),
		eComboStyle = CU_MakeSectionName( 'C', 'T', 'C', 'X' ),
		eListStyle = CU_MakeSectionName( 'C', 'T', 'L', 'B' ),
		eSliderStyle = CU_MakeSectionName( 'C', 'T', 'S', 'L' ),
		eStaticStyle = CU_MakeSectionName( 'C', 'T', 'S', 'T' ),
		ePanelStyle = CU_MakeSectionName( 'P', 'N', 'S', 'T' ),
		eProgressStyle = CU_MakeSectionName( 'P', 'G', 'S', 'T' ),
		eExpandablePanelStyle = CU_MakeSectionName( 'X', 'P', 'S', 'T' ),
		eFrameStyle = CU_MakeSectionName( 'F', 'M', 'S', 'T' ),
		eScrollBarStyle = CU_MakeSectionName( 'S', 'C', 'S', 'T' ),
		eButton = CU_MakeSectionName( 'B', 'U', 'T', 'N' ),
		eStatic = CU_MakeSectionName( 'S', 'T', 'T', 'C' ),
		eSlider = CU_MakeSectionName( 'S', 'L', 'D', 'R' ),
		eComboBox = CU_MakeSectionName( 'C', 'M', 'B', 'O' ),
		eListBox = CU_MakeSectionName( 'L', 'S', 'B', 'X' ),
		eEdit = CU_MakeSectionName( 'E', 'D', 'I', 'T' ),
		ePanel = CU_MakeSectionName( 'P', 'A', 'N', 'L' ),
		eProgress = CU_MakeSectionName( 'P', 'R', 'G', 'S' ),
		eExpandablePanel = CU_MakeSectionName( 'X', 'P', 'N', 'L' ),
		eExpandablePanelHeader = CU_MakeSectionName( 'X', 'P', 'H', 'D' ),
		eExpandablePanelExpand = CU_MakeSectionName( 'X', 'P', 'X', 'p' ),
		eExpandablePanelContent = CU_MakeSectionName( 'X', 'P', 'C', 'T' ),
		eFrame = CU_MakeSectionName( 'F', 'R', 'A', 'M' ),
		eFrameContent = CU_MakeSectionName( 'F', 'M', 'C', 'T' ),
		eBoxLayout = CU_MakeSectionName( 'B', 'X', 'L', 'T' ),
		eLayoutCtrl = CU_MakeSectionName( 'L', 'T', 'C', 'T' ),
	};

	C3D_API castor::AttributeParsers createGuiParsers();
	C3D_API castor::StrUInt32Map createGuiSections();
}

#endif
