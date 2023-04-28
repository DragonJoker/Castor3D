/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PanelCtrl_H___
#define ___C3D_PanelCtrl_H___

#include "Castor3D/Gui/Controls/CtrlLayoutControl.hpp"
#include "Castor3D/Gui/Theme/StylePanel.hpp"
#include "Castor3D/Gui/Theme/StyleScrollBar.hpp"

namespace castor3d
{
	class PanelCtrl
		: public LayoutControl
	{
	public:
		/** Constructor
		 *\param[in]	scene	The parent scene (nullptr for global).
		 *\param[in]	name	The control name
		 *\param[in]	style	The control style
		 *\param[in]	parent	The parent control, if any
		*/
		C3D_API PanelCtrl( SceneRPtr scene
			, castor::String const & name
			, PanelStyle * style
			, ControlRPtr parent );

		/** Constructor
		 *\param[in]	scene		The parent scene (nullptr for global).
		 *\param[in]	name		The control name
		 *\param[in]	style		The control style
		 *\param[in]	parent		The parent control, if any
		 *\param[in]	position	The control position
		 *\param[in]	size		The control size
		 *\param[in]	flags		The configuration flags
		 *\param[in]	visible		Initial visibility status
		 */
		C3D_API PanelCtrl( SceneRPtr scene
			, castor::String const & name
			, PanelStyle * style
			, ControlRPtr parent
			, castor::Position const & position
			, castor::Size const & size
			, ControlFlagType flags = 0
			, bool visible = true );

		/**
		*\return	The panel style.
		*/
		PanelStyle const & getStyle()const
		{
			return static_cast< PanelStyle const & >( getBaseStyle() );
		}

		C3D_API static ControlType constexpr Type{ ControlType::ePanel };

		PanelStyle & getStyle()
		{
			return static_cast< PanelStyle & >( getBaseStyle() );
		}
	};
}

#endif
