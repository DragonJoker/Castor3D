#include "Castor3D/Gui/Controls/CtrlPanel.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/OverlayCache.hpp"
#include "Castor3D/Gui/ControlsManager.hpp"
#include "Castor3D/Gui/Controls/CtrlScrollBar.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Overlay/BorderPanelOverlay.hpp"
#include "Castor3D/Overlay/TextOverlay.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <CastorUtils/Graphics/Font.hpp>

CU_ImplementCUSmartPtr( castor3d, PanelCtrl )

namespace castor3d
{
	PanelCtrl::PanelCtrl( SceneRPtr scene
		, castor::String const & name
		, PanelStyle * style
		, ControlRPtr parent )
		: PanelCtrl{ scene
			, name
			, style
			, parent
			, castor::Position{}
			, castor::Size{}
			, 0
			, true }
	{
	}

	PanelCtrl::PanelCtrl( SceneRPtr scene
		, castor::String const & name
		, PanelStyle * style
		, ControlRPtr parent
		, castor::Position const & position
		, castor::Size const & size
		, ControlFlagType flags
		, bool visible )
		: LayoutControl{ Type
			, scene
			, name
			, style
			, style
			, parent
			, position
			, size
			, flags
			, visible }
	{
		setBorderSize( castor::Point4ui{} );
		setStyle( style );
	}
}
