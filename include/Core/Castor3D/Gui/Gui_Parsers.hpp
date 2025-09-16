/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GuiParsers_H___
#define ___C3D_GuiParsers_H___

#include "Castor3D/Castor3DModule.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>

namespace c3d
{
	enum class GUISection
		: SectionId
	{
		eGUI = c3d::makeSectionName( 'C', 'G', 'U', 'I', 'C', 'G', 'U', 'I' ),
		eTheme = c3d::makeSectionName( 'C', 'G', 'U', 'I', 'C', 'G', 'T', 'H' ),
		eButtonStyle = c3d::makeSectionName( 'C', 'G', 'U', 'I', 'C', 'T', 'B', 'T' ),
		eEditStyle = c3d::makeSectionName( 'C', 'G', 'U', 'I', 'C', 'T', 'E', 'D' ),
		eComboStyle = c3d::makeSectionName( 'C', 'G', 'U', 'I', 'C', 'T', 'C', 'X' ),
		eListStyle = c3d::makeSectionName( 'C', 'G', 'U', 'I', 'C', 'T', 'L', 'B' ),
		eSliderStyle = c3d::makeSectionName( 'C', 'G', 'U', 'I', 'C', 'T', 'S', 'L' ),
		eStaticStyle = c3d::makeSectionName( 'C', 'G', 'U', 'I', 'C', 'T', 'S', 'T' ),
		ePanelStyle = c3d::makeSectionName( 'C', 'G', 'U', 'I', 'P', 'N', 'S', 'T' ),
		eProgressStyle = c3d::makeSectionName( 'C', 'G', 'U', 'I', 'P', 'G', 'S', 'T' ),
		eExpandablePanelStyle = c3d::makeSectionName( 'C', 'G', 'U', 'I', 'X', 'P', 'S', 'T' ),
		eFrameStyle = c3d::makeSectionName( 'C', 'G', 'U', 'I', 'F', 'M', 'S', 'T' ),
		eScrollBarStyle = c3d::makeSectionName( 'C', 'G', 'U', 'I', 'S', 'C', 'S', 'T' ),
		eButton = c3d::makeSectionName( 'C', 'G', 'U', 'I', 'B', 'U', 'T', 'N' ),
		eStatic = c3d::makeSectionName( 'C', 'G', 'U', 'I', 'S', 'T', 'T', 'C' ),
		eSlider = c3d::makeSectionName( 'C', 'G', 'U', 'I', 'S', 'L', 'D', 'R' ),
		eComboBox = c3d::makeSectionName( 'C', 'G', 'U', 'I', 'C', 'M', 'B', 'O' ),
		eListBox = c3d::makeSectionName( 'C', 'G', 'U', 'I', 'L', 'S', 'B', 'X' ),
		eEdit = c3d::makeSectionName( 'C', 'G', 'U', 'I', 'E', 'D', 'I', 'T' ),
		ePanel = c3d::makeSectionName( 'C', 'G', 'U', 'I', 'P', 'A', 'N', 'L' ),
		eProgress = c3d::makeSectionName( 'C', 'G', 'U', 'I', 'P', 'R', 'G', 'S' ),
		eExpandablePanel = c3d::makeSectionName( 'C', 'G', 'U', 'I', 'X', 'P', 'N', 'L' ),
		eExpandablePanelHeader = c3d::makeSectionName( 'C', 'G', 'U', 'I', 'X', 'P', 'H', 'D' ),
		eExpandablePanelExpand = c3d::makeSectionName( 'C', 'G', 'U', 'I', 'X', 'P', 'X', 'p' ),
		eExpandablePanelContent = c3d::makeSectionName( 'C', 'G', 'U', 'I', 'X', 'P', 'C', 'T' ),
		eFrame = c3d::makeSectionName( 'C', 'G', 'U', 'I', 'F', 'R', 'A', 'M' ),
		eFrameContent = c3d::makeSectionName( 'C', 'G', 'U', 'I', 'F', 'M', 'C', 'T' ),
		eBoxLayout = c3d::makeSectionName( 'C', 'G', 'U', 'I', 'B', 'X', 'L', 'T' ),
		eLayoutCtrl = c3d::makeSectionName( 'C', 'G', 'U', 'I', 'L', 'T', 'C', 'T' ),
	};

	C3D_API AttributeParsers createGuiParsers();
	C3D_API StrSectionIdMap createGuiSections();
}

#endif
