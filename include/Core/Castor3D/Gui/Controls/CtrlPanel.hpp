/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PanelCtrl_H___
#define ___C3D_PanelCtrl_H___

#include "Castor3D/Gui/Controls/CtrlLayoutControl.hpp"
#include "Castor3D/Gui/Theme/StylePanel.hpp"

namespace castor3d
{
	class PanelCtrl
		: public LayoutControl
	{
	public:
		/** Constructor
		 *\param[in]	name	The control name
		 *\param[in]	style	The control style
		 *\param[in]	parent	The parent control, if any
		 *\param[in]	id		The control ID.
		*/
		PanelCtrl( SceneRPtr scene
			, castor::String const & name
			, PanelStyle * style
			, ControlRPtr parent
			, uint32_t id );

		/** Constructor
		 *\param[in]	name		The control name
		 *\param[in]	style		The control style
		 *\param[in]	parent		The parent control, if any
		 *\param[in]	position	The control position
		 *\param[in]	size		The control size
		 *\param[in]	flags		The configuration flags
		 *\param[in]	visible		Initial visibility status
		 */
		PanelCtrl( SceneRPtr scene
			, castor::String const & name
			, PanelStyle * style
			, ControlRPtr parent
			, castor::Position const & position
			, castor::Size const & size
			, uint32_t flags = 0
			, bool visible = true );

		/**
		*\return	The static style
		*/
		PanelStyle const & getStyle()const
		{
			return static_cast< PanelStyle const & >( getBaseStyle() );
		}

		static ControlType constexpr Type{ ControlType::ePanel };

	private:
		PanelStyle & getStyle()
		{
			return static_cast< PanelStyle & >( getBaseStyle() );
		}
		/** @copydoc Control::doCreate
		*/
		void doCreate()override;

		/** @copydoc Control::doDestroy
		*/
		void doDestroy()override;

		/** @copydoc Control::doUpdateStyle
		*/
		void doUpdateStyle()override;

		/** @copydoc Control::doUpdateFlags
		*/
		void doUpdateFlags()override;

	private:
		static uint32_t m_count;
	};
}

#endif
