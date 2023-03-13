#include "Castor3D/Gui/Controls/CtrlPanel.hpp"

#include "Castor3D/Gui/ControlsManager.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/OverlayCache.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Overlay/BorderPanelOverlay.hpp"
#include "Castor3D/Overlay/TextOverlay.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <CastorUtils/Graphics/Font.hpp>

namespace castor3d
{
	uint32_t PanelCtrl::m_count = 0xFFFF0000u;

	PanelCtrl::PanelCtrl( SceneRPtr scene
		, castor::String const & name
		, PanelStyle * style
		, ControlRPtr parent
		, uint32_t id )
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
		, uint32_t flags
		, bool visible )
		: LayoutControl{ Type
			, scene
			, name
			, style
			, parent
			, m_count++
			, position
			, size
			, flags
			, visible }
	{
		setBackgroundBorders( castor::Point4ui{} );
		doUpdateStyle();
		doUpdateFlags();
	}

	void PanelCtrl::doCreate()
	{
	}

	void PanelCtrl::doDestroy()
	{
	}

	void PanelCtrl::doUpdateStyle()
	{
	}

	void PanelCtrl::doUpdateFlags()
	{
	}
}
