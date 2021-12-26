/*
See LICENSE file in root folder
*/
#ifndef ___CI_CtrlScrollablePane_H___
#define ___CI_CtrlScrollablePane_H___

#include "CtrlControl.hpp"

namespace CastorGui
{
	/**
	 *\brief		Description of an horizontally and/or vertically scrollable pane
	*/
	class ScrollablePaneCtrl
		: public Control
	{
	public:
		/** Constructor
		 *\param[in]	name	The control name.
		 *\param[in]	engine	The engine
		 *\param[in]	parent	The parent control, if any
		 *\param[in]	id		The control ID.
		*/
		ScrollablePaneCtrl( castor::String const & name
			, castor3d::Engine & engine
			, ControlRPtr parent
			, uint32_t id );

		/** Constructor
		 *\param[in]	name		The control name.
		 *\param[in]	engine		The engine
		 *\param[in]	parent		The parent control, if any
		 *\param[in]	position	The control position
		 *\param[in]	size		The control size
		 *\param[in]	flags		The configuration flags
		 *\param[in]	visible		Initial visibility status
		 */
		ScrollablePaneCtrl( castor::String const & name
			, castor3d::Engine & engine
			, ControlRPtr parent
			, uint32_t id
			, castor::Position const & position
			, castor::Size const & size
			, uint32_t flags = 0
			, bool visible = true );

	private:
		/** @copydoc CastorGui::Control::doCreate
		*/
		virtual void doCreate()override;

		/** @copydoc CastorGui::Control::doDestroy
		*/
		virtual void doDestroy()override;

		/** @copydoc CastorGui::Control::doSetPosition
		*/
		virtual void doSetPosition( castor::Position const & value )override;

		/** @copydoc CastorGui::Control::doSetSize
		*/
		virtual void doSetSize( castor::Size const & value )override;

		/** @copydoc CastorGui::Control::doSetBackgroundMaterial
		*/
		virtual void doSetBackgroundMaterial( castor3d::MaterialRPtr material )override;

		/** @copydoc CastorGui::Control::doSetForegroundMaterial
		*/
		virtual void doSetForegroundMaterial( castor3d::MaterialRPtr material )override;

		/** @copydoc CastorGui::Control::doSetVisible
		*/
		virtual void doSetVisible( bool p_visible )override;

		/** @copydoc CastorGui::Control::doUpdateFlags
		*/
		virtual void doUpdateFlags()override;
	};
}

#endif
