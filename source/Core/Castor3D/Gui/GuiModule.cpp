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

CU_ImplementSmartPtr( castor3d, StylesHolder )
CU_ImplementSmartPtr( castor3d, Theme )
CU_ImplementSmartPtr( castor3d, ControlStyle )
CU_ImplementSmartPtr( castor3d, ButtonStyle )
CU_ImplementSmartPtr( castor3d, ComboBoxStyle )
CU_ImplementSmartPtr( castor3d, EditStyle )
CU_ImplementSmartPtr( castor3d, ExpandablePanelStyle )
CU_ImplementSmartPtr( castor3d, FrameStyle )
CU_ImplementSmartPtr( castor3d, ListBoxStyle )
CU_ImplementSmartPtr( castor3d, PanelStyle )
CU_ImplementSmartPtr( castor3d, ProgressStyle )
CU_ImplementSmartPtr( castor3d, ScrollableStyle )
CU_ImplementSmartPtr( castor3d, ScrollBarStyle )
CU_ImplementSmartPtr( castor3d, SliderStyle )
CU_ImplementSmartPtr( castor3d, StaticStyle )

namespace castor3d
{
	void setMaterialColour( Pass & pass, castor::HdrRgbColour const & colour )
	{
		pass.setColour( colour );
	}

	castor::HdrRgbColour const & getMaterialColour( Pass const & pass )
	{
		return pass.getColour();
	}

	MaterialObs createMaterial( Engine & engine
		, castor::String const & name
		, castor::HdrRgbColour const & colour )
	{
		auto & cache = engine.getMaterialCache();
		MaterialObs created{};
		auto result = cache.tryAdd( name
			, true
			, created
			, engine
			, engine.getDefaultLightingModel() );

		if ( created == result )
		{
			result->createPass();
		}

		setMaterialColour( *result->getPass( 0u ), colour );
		return result;
	}
}
