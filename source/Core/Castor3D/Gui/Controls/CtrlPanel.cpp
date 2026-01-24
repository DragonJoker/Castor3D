#include "Castor3D/Gui/Controls/CtrlPanel.hpp"

#include <CastorUtils/Graphics/Font.hpp>

CU_ImplementSmartPtr( c3d, PanelCtrl )

namespace c3d
{
	PanelCtrl::PanelCtrl( SceneRPtr scene
		, String const & name
		, PanelStyle * style
		, ControlRPtr parent )
		: PanelCtrl{ scene
			, name
			, style
			, parent
			, Position{}
			, Size{}
			, 0
			, true }
	{
	}

	PanelCtrl::PanelCtrl( SceneRPtr scene
		, String const & name
		, PanelStyle * style
		, ControlRPtr parent
		, Position const & position
		, Size const & size
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
		setBorderSize( Point4ui{} );
		setStyle( style );
	}
}
