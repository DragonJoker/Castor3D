#include "Castor3D/Gui/GuiModule.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Gui/Theme/StyleButton.hpp"
#include "Castor3D/Gui/Theme/StyleComboBox.hpp"
#include "Castor3D/Gui/Theme/StyleEdit.hpp"
#include "Castor3D/Gui/Theme/StyleExpandablePanel.hpp"
#include "Castor3D/Gui/Theme/StyleListBox.hpp"
#include "Castor3D/Gui/Theme/StylePanel.hpp"
#include "Castor3D/Gui/Theme/StyleProgress.hpp"
#include "Castor3D/Gui/Theme/StyleScrollable.hpp"
#include "Castor3D/Gui/Theme/StyleScrollBar.hpp"
#include "Castor3D/Gui/Theme/StyleSlider.hpp"
#include "Castor3D/Gui/Theme/StyleStatic.hpp"
#include "Castor3D/Gui/Theme/Theme.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Overlay/BorderPanelOverlay.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Overlay/PanelOverlay.hpp"
#include "Castor3D/Overlay/TextOverlay.hpp"

#include <CastorUtils/Graphics/Font.hpp>

CU_ImplementSmartPtr( c3d, StylesHolder )
CU_ImplementSmartPtr( c3d, Theme )
CU_ImplementSmartPtr( c3d, ControlStyle )
CU_ImplementSmartPtr( c3d, ButtonStyle )
CU_ImplementSmartPtr( c3d, ComboBoxStyle )
CU_ImplementSmartPtr( c3d, EditStyle )
CU_ImplementSmartPtr( c3d, ExpandablePanelStyle )
CU_ImplementSmartPtr( c3d, FrameStyle )
CU_ImplementSmartPtr( c3d, ListBoxStyle )
CU_ImplementSmartPtr( c3d, PanelStyle )
CU_ImplementSmartPtr( c3d, ProgressStyle )
CU_ImplementSmartPtr( c3d, ScrollableStyle )
CU_ImplementSmartPtr( c3d, ScrollBarStyle )
CU_ImplementSmartPtr( c3d, SliderStyle )
CU_ImplementSmartPtr( c3d, StaticStyle )

namespace c3d
{
	void setMaterialColour( Pass const & pass
		, HdrRgbColour const & colour )
	{
		pass.setColour( colour );
	}

	HdrRgbColour const & getMaterialColour( Pass const & pass )
	{
		return pass.getColour();
	}

	MaterialObs createMaterial( Engine & engine
		, String const & name
		, HdrRgbColour const & colour )
	{
		auto result = engine.tryFindMaterial( name );

		if ( !result )
		{
			result = engine.addNewMaterial( name
				, engine
				, engine.getDefaultLightingModel() );
			result->createPass();
		}

		setMaterialColour( *result->getPass( 0u ), colour );
		return result;
	}
}
